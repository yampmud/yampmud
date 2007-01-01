
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*	ROM 2.4 is copyright 1993-1995 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@pacinfo.com)				   *
*	    Gabrielle Taylor (gtaylor@pacinfo.com)			   *
*	    Brian Moore (rom@rom.efn.org)				   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

/*************************************************************************** 
*       ROT 1.4 is copyright 1996-1997 by Russ Walsh                       * 
*       By using this code, you have agreed to follow the terms of the     * 
*       ROT license, in the file doc/rot.license                           * 
***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sqlite3.h>

#include "merc.h"
#include "db.h"
#include "lookup.h"
#include "tables.h"
#include "sql_io.h"
#include "fd_property.h"
#include "str_util.h"

/*
 * Snarf a mob section.  new style
 */
void load_mobiles(FILE * fp)
{
  MOB_INDEX_DATA *pMobIndex;

  if (!area_last)               /* OLC */
  {
    bug("Load_mobiles: no #AREA seen yet.", 0);
    quit(1);
  }

  for (;;)
  {
    long vnum;
    char letter;
    int iHash;
    char *temp;

    letter = fread_letter(fp);
    if (letter != '#')
    {
      bug("Load_mobiles: # not found.", 0);
      quit(1);
    }

    vnum = fread_long(fp);
    if (vnum == 0)
      break;

    fBootDb = false;
    if (get_mob_index(vnum) != NULL)
    {
      bug("Load_mobiles: vnum %ld duplicated.", vnum);
      quit(1);
    }
    fBootDb = true;

    pMobIndex = new_mob_index();
    pMobIndex->vnum = vnum;
    pMobIndex->area = area_last;  /* OLC */
    pMobIndex->player_name = fread_string(fp);
    pMobIndex->short_descr = fread_string(fp);
    pMobIndex->long_descr = fread_string(fp);
    pMobIndex->description = fread_string(fp);
    temp = fread_string(fp);
    if (race_lookup(temp) == -1)
      pMobIndex->race = 0;
    else
      pMobIndex->race = race_lookup(temp);
    free_string(temp);

    pMobIndex->long_descr[0] = UPPER(pMobIndex->long_descr[0]);
    pMobIndex->description[0] = UPPER(pMobIndex->description[0]);

    pMobIndex->act =
      fread_flag(fp) | ACT_IS_NPC | race_table[pMobIndex->race].act;
    pMobIndex->act2 = fread_flag(fp) | race_table[pMobIndex->race].act2;

    pMobIndex->affected_by = fread_flag(fp) | race_table[pMobIndex->race].aff;

    pMobIndex->shielded_by = fread_flag(fp) | race_table[pMobIndex->race].shd;
    pMobIndex->pShop = NULL;
    pMobIndex->alignment = fread_number(fp);
    pMobIndex->group = fread_long(fp);

    pMobIndex->level = fread_number(fp);
    pMobIndex->hitroll = fread_number(fp);

    /* read hit dice */
    pMobIndex->hit[DICE_NUMBER] = fread_number(fp);
    /* 'd' */ fread_letter(fp);
    pMobIndex->hit[DICE_TYPE] = fread_number(fp);
    /* '+' */ fread_letter(fp);
    pMobIndex->hit[DICE_BONUS] = fread_number(fp);

    /* read mana dice */
    pMobIndex->mana[DICE_NUMBER] = fread_number(fp);
    fread_letter(fp);
    pMobIndex->mana[DICE_TYPE] = fread_number(fp);
    fread_letter(fp);
    pMobIndex->mana[DICE_BONUS] = fread_number(fp);

    /* read damage dice */
    pMobIndex->damage[DICE_NUMBER] = fread_number(fp);
    fread_letter(fp);
    pMobIndex->damage[DICE_TYPE] = fread_number(fp);
    fread_letter(fp);
    pMobIndex->damage[DICE_BONUS] = fread_number(fp);
    pMobIndex->dam_type = attack_lookup(fread_word(fp));

    /* read armor class */
    pMobIndex->ac[AC_PIERCE] = fread_number(fp) * 10;
    pMobIndex->ac[AC_BASH] = fread_number(fp) * 10;
    pMobIndex->ac[AC_SLASH] = fread_number(fp) * 10;
    pMobIndex->ac[AC_EXOTIC] = fread_number(fp) * 10;

    /* read flags and add in data from the race table */
    pMobIndex->off_flags = fread_flag(fp) | race_table[pMobIndex->race].off;
    pMobIndex->imm_flags = fread_flag(fp) | race_table[pMobIndex->race].imm;
    pMobIndex->res_flags = fread_flag(fp) | race_table[pMobIndex->race].res;
    pMobIndex->vuln_flags = fread_flag(fp) | race_table[pMobIndex->race].vuln;

    /* vital statistics */
    pMobIndex->start_pos = position_lookup(fread_word(fp));
    pMobIndex->default_pos = position_lookup(fread_word(fp));
    pMobIndex->sex = sex_lookup(fread_word(fp));

    pMobIndex->wealth = fread_number(fp);

    pMobIndex->form = fread_flag(fp) | race_table[pMobIndex->race].form;
    pMobIndex->parts = fread_flag(fp) | race_table[pMobIndex->race].parts;
    /* size */
    pMobIndex->size = size_lookup(fread_word(fp));
    pMobIndex->material = str_dup(fread_word(fp));

    pMobIndex->die_descr = str_dup("");
    pMobIndex->say_descr = str_dup("");

    for (;;)
    {
      letter = fread_letter(fp);

      if (letter == 'F')
      {
        char *word;
        long vector;

        word = fread_word(fp);
        vector = fread_flag(fp);

        if (!str_prefix(word, "act"))
          REMOVE_BIT(pMobIndex->act, vector);
        if (!str_prefix(word, "act2"))
          REMOVE_BIT(pMobIndex->act2, vector);
        else if (!str_prefix(word, "aff"))
          REMOVE_BIT(pMobIndex->affected_by, vector);
        else if (!str_prefix(word, "shd"))
          REMOVE_BIT(pMobIndex->shielded_by, vector);
        else if (!str_prefix(word, "off"))
          REMOVE_BIT(pMobIndex->off_flags, vector);
        else if (!str_prefix(word, "imm"))
          REMOVE_BIT(pMobIndex->imm_flags, vector);
        else if (!str_prefix(word, "res"))
          REMOVE_BIT(pMobIndex->res_flags, vector);
        else if (!str_prefix(word, "vul"))
          REMOVE_BIT(pMobIndex->vuln_flags, vector);
        else if (!str_prefix(word, "for"))
          REMOVE_BIT(pMobIndex->form, vector);
        else if (!str_prefix(word, "par"))
          REMOVE_BIT(pMobIndex->parts, vector);
        else
        {
          bug("Flag remove: flag not found.", 0);
          quit(1);
        }
      }
      else if (letter == 'D')
      {
        pMobIndex->die_descr = fread_string(fp);
      }
      else if (letter == 'T')
      {
        pMobIndex->say_descr = fread_string(fp);
        pMobIndex->say_descr[0] = UPPER(pMobIndex->say_descr[0]);
      }
      else if (letter == 'M')
      {
        MPROG_LIST *pMprog;
        char *word;
        int trigger = 0;

        pMprog = alloc_perm(sizeof(*pMprog));
        word = fread_word(fp);
        if (!(trigger = flag_lookup(word, mprog_flags)))
        {
          bug("MOBprogs: invalid trigger.", 0);
          quit(1);
        }
        SET_BIT(pMobIndex->mprog_flags, trigger);
        pMprog->trig_type = trigger;
        pMprog->vnum = fread_long(fp);
        pMprog->trig_phrase = fread_string(fp);
        pMprog->next = pMobIndex->mprogs;
        pMobIndex->mprogs = pMprog;
      }

      else if (letter == 'P')
      {                         // property
        char key[MAX_STRING_LENGTH];
        char type[MAX_STRING_LENGTH];
        char value[MAX_STRING_LENGTH];
        int i;
        bool b;
        char c;
        long l;

        strcpy(key, fread_string_temp(fp));
        strcpy(type, fread_string_temp(fp));
        strcpy(value, fread_string_temp(fp));

        switch (which_keyword(type, "int", "bool", "string",
                              "char", "long", NULL))
        {
          case 1:
            i = atoi(value);
            SetDCharProperty(pMobIndex, PROPERTY_INT, key, &i);
            break;
          case 2:
            switch (which_keyword(value, "true", "false", NULL))
            {
              case 1:
                b = true;
                SetDCharProperty(pMobIndex, PROPERTY_BOOL, key, &b);
                break;
              case 2:
                b = false;
                SetDCharProperty(pMobIndex, PROPERTY_BOOL, key, &b);
                break;
              default:
                ;
            }
            break;
          case 3:
            SetDCharProperty(pMobIndex, PROPERTY_STRING, key, value);
            break;
          case 4:
            c = value[0];
            SetDCharProperty(pMobIndex, PROPERTY_CHAR, key, &c);
            break;
          case 5:
            l = atol(value);
            SetDCharProperty(pMobIndex, PROPERTY_LONG, key, &l);
            break;
          default:
            bugf("Property: unknown keyword '%s'", type);
        }

      }

      else
      {
        ungetc(letter, fp);
        break;
      }
    }

    iHash = vnum % MAX_KEY_HASH;
    pMobIndex->next = mob_index_hash[iHash];
    mob_index_hash[iHash] = pMobIndex;
    top_mob_index++;
    top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob; /* OLC */
    assign_area_vnum(vnum);     /* OLC */
    kill_table[URANGE(0, pMobIndex->level, MAX_LEVEL - 1)].number++;
  }

  return;
}

/*
 * Snarf an obj section. new style
 */
void load_objects(FILE * fp)
{
  OBJ_INDEX_DATA *pObjIndex;

  if (!area_last)               /* OLC */
  {
    bug("Load_objects: no #AREA seen yet.", 0);
    quit(1);
  }

  for (;;)
  {
    long vnum;
    char letter;
    int iHash;

    letter = fread_letter(fp);
    if (letter != '#')
    {
      bug("Load_objects: # not found.", 0);
      quit(1);
    }

    vnum = fread_long(fp);
    if (vnum == 0)
      break;

    fBootDb = false;
    if (get_obj_index(vnum) != NULL)
    {
      bug("Load_objects: vnum %ld duplicated.", vnum);
      quit(1);
    }
    fBootDb = true;

    pObjIndex = new_obj_index();
    pObjIndex->vnum = vnum;
    pObjIndex->area = area_last;  /* OLC */
    pObjIndex->reset_num = 0;
    pObjIndex->name = fread_string(fp);
    pObjIndex->short_descr = fread_string(fp);
    pObjIndex->description = fread_string(fp);
    pObjIndex->material = fread_string(fp);

    pObjIndex->item_type = item_lookup(fread_word(fp));
    pObjIndex->extra_flags = fread_flag(fp);
    pObjIndex->wear_flags = fread_flag(fp);
    switch (pObjIndex->item_type)
    {
      case ITEM_WEAPON:
        pObjIndex->value[0] = weapon_type(fread_word(fp));
        pObjIndex->value[1] = fread_long(fp);
        pObjIndex->value[2] = fread_long(fp);
        pObjIndex->value[3] = attack_lookup(fread_word(fp));
        pObjIndex->value[4] = fread_flag(fp);

        break;
      case ITEM_CONTAINER:
      case ITEM_PIT:
        pObjIndex->value[0] = fread_long(fp);
        pObjIndex->value[1] = fread_flag(fp);
        pObjIndex->value[2] = fread_long(fp);
        pObjIndex->value[3] = fread_long(fp);
        pObjIndex->value[4] = fread_long(fp);

        break;
      case ITEM_DRINK_CON:
      case ITEM_FOUNTAIN:
        pObjIndex->value[0] = fread_long(fp);
        pObjIndex->value[1] = fread_long(fp);
        pObjIndex->value[2] = liq_lookup(fread_word(fp));
        pObjIndex->value[3] = fread_long(fp);
        pObjIndex->value[4] = fread_long(fp);

        break;
      case ITEM_WAND:
      case ITEM_STAFF:
      case ITEM_INSTRUMENT:
        pObjIndex->value[0] = fread_long(fp);
        pObjIndex->value[1] = fread_long(fp);
        pObjIndex->value[2] = fread_long(fp);
        pObjIndex->value[3] = skill_lookup(fread_word(fp));
        pObjIndex->value[4] = fread_long(fp);

        break;
      case ITEM_POTION:
      case ITEM_PILL:
      case ITEM_SCROLL:
        pObjIndex->value[0] = fread_long(fp);
        pObjIndex->value[1] = skill_lookup(fread_word(fp));
        pObjIndex->value[2] = skill_lookup(fread_word(fp));
        pObjIndex->value[3] = skill_lookup(fread_word(fp));
        pObjIndex->value[4] = skill_lookup(fread_word(fp));

        break;
      default:
        pObjIndex->value[0] = fread_flag(fp);
        pObjIndex->value[1] = fread_flag(fp);
        pObjIndex->value[2] = fread_flag(fp);
        pObjIndex->value[3] = fread_flag(fp);
        pObjIndex->value[4] = fread_flag(fp);

        break;
    }
    pObjIndex->level = fread_number(fp);
    pObjIndex->weight = fread_number(fp);
    pObjIndex->cost = fread_number(fp);

    /* condition */
    letter = fread_letter(fp);
    switch (letter)
    {
      case ('P'):
        pObjIndex->condition = 100;
        break;
      case ('G'):
        pObjIndex->condition = 90;
        break;
      case ('A'):
        pObjIndex->condition = 75;
        break;
      case ('W'):
        pObjIndex->condition = 50;
        break;
      case ('D'):
        pObjIndex->condition = 25;
        break;
      case ('B'):
        pObjIndex->condition = 10;
        break;
      case ('R'):
        pObjIndex->condition = 0;
        break;
      default:
        pObjIndex->condition = 100;
        break;
    }

    for (;;)
    {
      char letter;

      letter = fread_letter(fp);

      if (letter == 'A')
      {
        AFFECT_DATA *paf;

        paf = alloc_perm(sizeof(*paf));
        paf->where = TO_OBJECT;
        paf->type = -1;
        paf->level = pObjIndex->level;
        paf->duration = -1;
        paf->location = fread_number(fp);
        paf->modifier = fread_number(fp);
        paf->bitvector = 0;
        paf->next = pObjIndex->affected;
        pObjIndex->affected = paf;
        top_affect++;
      }

      else if (letter == 'F')
      {
        AFFECT_DATA *paf;

        paf = alloc_perm(sizeof(*paf));
        letter = fread_letter(fp);
        switch (letter)
        {
          case 'A':
            paf->where = TO_AFFECTS;
            break;
          case 'I':
            paf->where = TO_IMMUNE;
            break;
          case 'R':
            paf->where = TO_RESIST;
            break;
          case 'V':
            paf->where = TO_VULN;
            break;
          case 'S':
            paf->where = TO_SHIELDS;
            break;
          default:
            bug("Load_objects: Bad where on flag set.", 0);
            quit(1);
        }
        paf->type = -1;
        paf->level = pObjIndex->level;
        paf->duration = -1;
        paf->location = fread_number(fp);
        paf->modifier = fread_number(fp);
        paf->bitvector = fread_flag(fp);
        paf->next = pObjIndex->affected;
        pObjIndex->affected = paf;
        top_affect++;
      }

      else if (letter == 'E')
      {
        EXTRA_DESCR_DATA *ed;

        ed = alloc_perm(sizeof(*ed));
        ed->keyword = fread_string(fp);
        ed->description = fread_string(fp);
        ed->next = pObjIndex->extra_descr;
        pObjIndex->extra_descr = ed;
        top_ed++;
      }

      else if (letter == 'C')   /* clan */
      {
        char *temp;

        if (pObjIndex->clan)
        {
          bug("Load_objects: duplicate clan fields.", 0);
          quit(1);
        }
        temp = fread_string(fp);
        pObjIndex->clan = clan_lookup(temp);
        free_string(temp);
        if (pObjIndex->item_type == ITEM_ARMOR)
        {
          pObjIndex->value[0] = 0;
          pObjIndex->value[1] = 0;
          pObjIndex->value[2] = 0;
          pObjIndex->value[3] = 0;
          pObjIndex->level = 1;
          pObjIndex->cost = 0;
        }
        if (pObjIndex->item_type == ITEM_WEAPON)
        {
          pObjIndex->value[1] = 1;
          pObjIndex->value[2] = 1;
          pObjIndex->level = 1;
          pObjIndex->cost = 0;
        }
      }

      else if (letter == 'G')   /* guild */
      {
        char *temp;

        if (pObjIndex->class)
        {
          bug("Load_objects: duplicate class fields.", 0);
          quit(1);
        }
        temp = fread_string(fp);
        pObjIndex->class = class_lookup(temp);
        free_string(temp);
      }



      else if (letter == 'P')
      {                         // property
        char key[MAX_STRING_LENGTH];
        char type[MAX_STRING_LENGTH];
        char value[MAX_STRING_LENGTH];
        int i;
        bool b;
        char c;
        long l;

        strcpy(key, fread_string_temp(fp));
        strcpy(type, fread_string_temp(fp));
        strcpy(value, fread_string_temp(fp));

        switch (which_keyword(type, "int", "bool", "string",
                              "char", "long", NULL))
        {
          case 1:
            i = atoi(value);
            SetDObjectProperty(pObjIndex, PROPERTY_INT, key, &i);
            break;
          case 2:
            switch (which_keyword(value, "true", "false", NULL))
            {
              case 1:
                b = true;
                SetDObjectProperty(pObjIndex, PROPERTY_BOOL, key, &b);
                break;
              case 2:
                b = false;
                SetDObjectProperty(pObjIndex, PROPERTY_BOOL, key, &b);
                break;
              default:
                ;
            }
            break;
          case 3:
            SetDObjectProperty(pObjIndex, PROPERTY_STRING, key, value);
            break;
          case 4:
            c = value[0];
            SetDObjectProperty(pObjIndex, PROPERTY_CHAR, key, &c);
            break;
          case 5:
            l = atol(value);
            SetDObjectProperty(pObjIndex, PROPERTY_LONG, key, &l);
            break;
          default:
            bugf("Property: unknown keyword '%s'", type);
        }
      }

      else
      {
        ungetc(letter, fp);
        break;
      }
    }

    iHash = vnum % MAX_KEY_HASH;
    pObjIndex->next = obj_index_hash[iHash];
    obj_index_hash[iHash] = pObjIndex;
    top_obj_index++;
    top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj; /* OLC */
    assign_area_vnum(vnum);     /* OLC */
  }

  return;
}

FILE *file_open(char *file, const char *mode)
{
  FILE *fp = NULL;

  fp = fopen(file, mode);

  return fp;
}

bool file_close(FILE * fp)
{
  if (fp != NULL)
    fclose(fp);
  return true;
}

void _quit(int iError, char *file, int line)
{

  close_sqlite3();

  if (stderr)
  {
    fflush(stderr);
    file_close(stderr);
  }

  if (fpArea)                   // flush any open area file
  {
    fflush(fpArea);
    file_close(fpArea);
  }

  if (iError == 3)
  {
    bugf("Aborting from %s %d.", file, line);
    abort();
  }
  else
  {
    bugf("Exiting from %s %d.", file, line);
    exit(iError);
  }
}
