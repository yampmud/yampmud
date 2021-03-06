
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

#include <sys/types.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "lookup.h"
#include "tables.h"
#include "olc.h"
#include "fd_property.h"
#include "str_util.h"

extern int _filbuf args((FILE *));

char *print_flags(int flag)
{
  int count, pos = 0;
  static char buf[52];

  for (count = 0; count < 32; count++)
  {
    if (IS_SET(flag, 1 << count))
    {
      if (count < 26)
        buf[pos] = 'A' + count;
      else
        buf[pos] = 'a' + (count - 26);
      pos++;
    }
  }

  if (pos == 0)
  {
    buf[pos] = '0';
    pos++;
  }

  buf[pos] = '\0';

  return buf;
}

/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST	100
static OBJ_DATA *rgObjNest[MAX_NEST];

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
void save_char_obj(CHAR_DATA * ch)
{
  char strsave[MAX_INPUT_LENGTH];
  FILE *fp;

  remove(TEMP_FILE);

  if (IS_NPC(ch))
    return;

  if (!IS_VALID(ch))
  {
    bug("save_char_obj: trying to save invalid character", 0);
    return;
  }

  if (ch->desc != NULL && ch->desc->original != NULL)
    ch = ch->desc->original;

  /* create god log */
  if (IS_IMMORTAL(ch) || ch->level >= LEVEL_IMMORTAL)
  {
    sprintf(strsave, "%s%s", GOD_DIR, capitalize(ch->name));
    if ((fp = file_open(strsave, "w")) == NULL)
    {
      bug("Save_char_obj: fopen", 0);
      perror(strsave);
    }

    fprintf(fp, "Lev %2d Trust %2d  %s%s\n", ch->level, get_trust(ch),
            ch->name, ch->pcdata->title);
    file_close(fp);
  }

  sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
  if ((fp = file_open(TEMP_FILE, "w")) == NULL)
  {
    bug("Save_char_obj: fopen", 0);
    perror(strsave);
  }
  else
  {
    fwrite_char(ch, fp);
    if (ch->carrying != NULL)
      fwrite_obj(ch, ch->carrying, fp, 0);
    if (ch->bankeditems != NULL)
      fwrite_bank(ch, ch->bankeditems, fp, 0);
    /* save the pets */
    if (ch->pet != NULL && ch->pet->in_room == ch->in_room)
      fwrite_pet(ch->pet, fp);
    fprintf(fp, "#END\n");
  }
  file_close(fp);
  rename(TEMP_FILE, strsave);
  remove(TEMP_FILE);
  return;
}

/*
 * Write the char.
 */
void fwrite_char(CHAR_DATA * ch, FILE * fp)
{
  AFFECT_DATA *paf;
  long sn, gn, pos;
  char spell[MSL];

  fprintf(fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER");

  fprintf(fp, "Name %s~\n", ch->name);

  fprintf(fp, "Id   %ld\n", ch->id);
  fprintf(fp, "LogO %ld\n", current_time);
  fprintf(fp, "Vers %d\n", 6);
  if (ch->short_descr[0] != '\0')
    fprintf(fp, "ShD  %s~\n", ch->short_descr);
  if (ch->long_descr[0] != '\0')
    fprintf(fp, "LnD  %s~\n", ch->long_descr);
  if (ch->description[0] != '\0')
    fprintf(fp, "Desc %s~\n", ch->description);
  if (ch->prompt[0] != '\0')
    fprintf(fp, "Prom %s~\n", ch->prompt);

  fprintf(fp, "Race %s~\n", pc_race_table[ch->race].name);
  if (ch->clan)
  {
    fprintf(fp, "CRank   %d\n", ch->clan_rank);
    fprintf(fp, "Clan %s~\n", clan_table[ch->clan].name);
    if (ch->clead)
      fprintf(fp, "Clead %s~\n", clan_table[ch->clan].name);
  }
  if (ch->pcdata->nextquest != 0)
    fprintf(fp, "QuestNext %d\n", ch->pcdata->nextquest);
  fprintf(fp, "Sex  %d\n", ch->sex);
  fprintf(fp, "Clas %s~\n", class_table[ch->class].name);
  fprintf(fp, "Levl %d\n", ch->level);
  if (ch->recall_room > 0)
    fprintf(fp, "ReRm %ld\n", ch->recall_room);
  if (ch->talk_color > 0)
    fprintf(fp, "TCol %d\n", ch->talk_color);

  if (is_clan(ch))
  {
    ch->pcdata->pkset = true;
  }

  if (ch->pcdata->pkset > 0)
    fprintf(fp, "Pkset %d\n", ch->pcdata->pkset);

  if (ch->corner_timer > 0)
    fprintf(fp, "Ctim %d\n", ch->corner_timer);

  if (ch->pk_timer > 0)
    fprintf(fp, "Ptim %d\n", ch->pk_timer);

  fprintf(fp, "Tier %d\n", ch->pcdata->tier); /* add this in */
  fprintf(fp, "Ctier %d\n", ch->pcdata->ctier); /* add this in */

  if (ch->trust > 0)
    fprintf(fp, "Tru  %d\n", ch->trust);

  if (ch->pcdata->security > 0)
    fprintf(fp, "Sec  %d\n", URANGE(1, ch->pcdata->security, MAX_SECURITY));

  fprintf(fp, "Plyd %d\n", ch->played + (int) (current_time - ch->logon));

  /* Save note board status */
  /* Save number of boards in case that number changes */
  fprintf(fp, "Boards       %d ", MAX_BOARD);
  int i = 0;
  for (i = 0; i < MAX_BOARD; i++)
    fprintf(fp, "%s %ld ", boards[i].short_name, ch->pcdata->last_note[i]);
  fprintf(fp, "\n");

  if (ch->lines > 0)
    fprintf(fp, "Scro %d\n", ch->lines);

  fprintf(fp, "Room %ld\n",
          (ch->in_room == get_room_index(ROOM_VNUM_LIMBO) &&
           ch->was_in_room !=
           NULL) ? ch->was_in_room->vnum : ch->in_room ==
          NULL ? 3001 : ch->in_room->vnum);

  fprintf(fp, "HMV  %ld %ld %ld %ld %ld %ld\n", ch->hit, ch->max_hit,
          ch->mana, ch->max_mana, ch->move, ch->max_move);

  if (ch->platinum > 0)
    fprintf(fp, "Plat %ld\n", ch->platinum);

  if (ch->gold > 0)
    fprintf(fp, "Gold %ld\n", ch->gold);
  if (ch->silver > 0)
    fprintf(fp, "Silv %ld\n", ch->silver);

  if (ch->exp > 0)
    fprintf(fp, "Exp  %ld\n", ch->exp);
  if (ch->rps > 0)
    fprintf(fp, "Rps  %d\n", ch->rps);
  if (ch->rpst > 0)
    fprintf(fp, "RpsT  %d\n", ch->rpst);

  if (ch->redeem > 0)
    fprintf(fp, "Ree  %d\n", ch->redeem);

  if (ch->qps > 0)
    fprintf(fp, "Qps  %d\n", ch->qps);
  if (print_flags(ch->act))
    fprintf(fp, "Act  %s\n", print_flags(ch->act));

  if (print_flags(ch->act2))
    fprintf(fp, "Act2  %s\n", print_flags(ch->act2));

  if (ch->affected_by > 0)
    fprintf(fp, "AfBy %s\n", print_flags(ch->affected_by));
  if (ch->shielded_by > 0)
    fprintf(fp, "ShBy %s\n", print_flags(ch->shielded_by));

  if (ch->comm > 0)
    fprintf(fp, "Comm %s\n", print_flags(ch->comm));
  if (ch->pcdata->verbose > 0)
    fprintf(fp, "Verb %s\n", print_flags(ch->pcdata->verbose));

  if (ch->wiznet)
    fprintf(fp, "Wizn %s\n", print_flags(ch->wiznet));
  if (ch->invis_level)
    fprintf(fp, "Invi %d\n", ch->invis_level);
  if (ch->incog_level)
    fprintf(fp, "Inco %d\n", ch->incog_level);
  if (ch->ghost_level)
    fprintf(fp, "Ghos %d\n", ch->ghost_level);
  fprintf(fp, "Pos  %d\n",
          ch->position == POS_FIGHTING ? POS_STANDING : ch->position);
  if (ch->practice > 0)
    fprintf(fp, "Prac %d\n", ch->practice);
  if (ch->train > 0)
    fprintf(fp, "Trai %d\n", ch->train);
  if (ch->saving_throw > 0)
    fprintf(fp, "Save  %d\n", ch->saving_throw);

  fprintf(fp, "Alig  %d\n", ch->alignment);
  if (ch->hitroll > 0)
    fprintf(fp, "Hit   %d\n", ch->hitroll);
  if (ch->damroll > 0)
    fprintf(fp, "Dam   %d\n", ch->damroll);

  if (ch->armor[0] < 0 && ch->armor[1] < 0 && ch->armor[2] < 0 &&
      ch->armor[3] < 0)
    fprintf(fp, "ACs %d %d %d %d\n", ch->armor[0], ch->armor[1], ch->armor[2],
            ch->armor[3]);
  if (ch->wimpy > 0)
    fprintf(fp, "Wimp  %d\n", ch->wimpy);

  fprintf(fp, "Attr %d %d %d %d %d\n", ch->perm_stat[STAT_STR],
          ch->perm_stat[STAT_INT], ch->perm_stat[STAT_WIS],
          ch->perm_stat[STAT_DEX], ch->perm_stat[STAT_CON]);

  fprintf(fp, "AMod %d %d %d %d %d\n", ch->mod_stat[STAT_STR],
          ch->mod_stat[STAT_INT], ch->mod_stat[STAT_WIS],
          ch->mod_stat[STAT_DEX], ch->mod_stat[STAT_CON]);

  if (ch->pcdata->questpoints > 0)
    fprintf(fp, "QuestPnts %ld\n", ch->pcdata->questpoints);
  if (ch->pcdata->nextquest > 0)
    fprintf(fp, "QuestNext %d\n", ch->pcdata->nextquest);
  else if (ch->pcdata->countdown != 0)
  {
    fprintf(fp, "QuestCount %d\n", ch->pcdata->countdown);
    fprintf(fp, "QuestGiver %ld\n", ch->pcdata->questgiver);
    fprintf(fp, "QuestLoc   %ld\n", ch->pcdata->questloc);
    if (ch->pcdata->questobj != 0)
      fprintf(fp, "QuestObj %ld\n", ch->pcdata->questobj);
    else if (ch->pcdata->questmob != 0)
      fprintf(fp, "QuestMob %ld\n", ch->pcdata->questmob);
  }

  if (ch->pcdata->spouse)
    fprintf(fp, "Spouse      %s~\n", ch->pcdata->spouse);
  if (ch->pcdata->balance > 0)
    fprintf(fp, "Balance     %ld\n", ch->pcdata->balance);
  if (ch->pcdata->shares > 0)
    fprintf(fp, "Shares      %ld\n", ch->pcdata->shares);

  if (ch->pcdata->pwd)
    fprintf(fp, "Pass %s~\n", ch->pcdata->pwd);

  if (ch->pcdata->refer)
    fprintf(fp, "Refer %s~\n", ch->pcdata->refer);
  else
    fprintf(fp, "Refer None~\n");

  if (ch->pcdata->bamfin[0] != '\0')
    fprintf(fp, "Bin  %s~\n", ch->pcdata->bamfin);
  if (ch->pcdata->bamfout[0] != '\0')
    fprintf(fp, "Bout %s~\n", ch->pcdata->bamfout);
  if (ch->pcdata->who_descr[0] != '\0')
    fprintf(fp, "Whod %s~\n", ch->pcdata->who_descr);
  if (ch->pcdata->title[0] != '\0')
    fprintf(fp, "Titl %s~\n", ch->pcdata->title);
  if (ch->pcdata->points > 0)
    fprintf(fp, "Pnts %d\n", ch->pcdata->points);
  fprintf(fp, "TSex %d\n", ch->pcdata->true_sex);
  if (ch->pcdata->pkdeaths > 0)
    fprintf(fp, "Pkdt %ld\n", ch->pcdata->pkdeaths);
  if (ch->pcdata->pkkills > 0)
    fprintf(fp, "Pkkl %ld\n", ch->pcdata->pkkills);
  if (ch->pcdata->last_level > 0)
    fprintf(fp, "LLev %d\n", ch->pcdata->last_level);
  if (ch->pcdata->recall > 0)
    fprintf(fp, "Reca %d\n", ch->pcdata->recall);

  fprintf(fp, "HMVP %ld %ld %ld\n", ch->pcdata->perm_hit,
          ch->pcdata->perm_mana, ch->pcdata->perm_move);
  if (ch->pcdata->alosses > 0)
    fprintf(fp, "Alos	       %d\n", ch->pcdata->alosses);
  fprintf(fp, "Awin	       %d\n", ch->pcdata->awins);
  fprintf(fp, "Cnd  %d %d %d %d\n", ch->pcdata->condition[0],
          ch->pcdata->condition[1], ch->pcdata->condition[2],
          ch->pcdata->condition[3]);

  /* write forgets */
  for (pos = 0; pos < MAX_FORGET; pos++)
  {
    if (ch->pcdata->forget[pos] == NULL)
      break;

    fprintf(fp, "Forge %s~\n", ch->pcdata->forget[pos]);
  }

  /* write dupes */
  for (pos = 0; pos < MAX_DUPES; pos++)
  {
    if (ch->pcdata->dupes[pos] == NULL)
      break;

    fprintf(fp, "Dupes %s~\n", ch->pcdata->dupes[pos]);
  }

  /* write alias */
  for (pos = 0; pos < MAX_ALIAS; pos++)
  {
    if (ch->pcdata->alias[pos] == NULL || ch->pcdata->alias_sub[pos] == NULL)
      break;

    fprintf(fp, "Alias %s %s~\n", ch->pcdata->alias[pos],
            ch->pcdata->alias_sub[pos]);
  }

  for (sn = 2; sn < MAX_SKILL; sn++)
  {
    if (skill_table[sn].name != NULL && ch->pcdata->learned[sn] > 0)
    {
      fprintf(fp, "Sk %d '%s'\n", ch->pcdata->learned[sn],
              skill_table[sn].name);
    }
  }

  for (gn = 0; gn < MAX_GROUP; gn++)
  {
    if (group_table[gn].name != NULL && ch->pcdata->group_known[gn])
    {
      fprintf(fp, "Gr '%s'\n", group_table[gn].name);
    }
  }

  for (paf = ch->affected; paf != NULL; paf = paf->next)
  {
    sprintf(spell, "%s", skill_table[paf->type].name);

    if (paf->type < 0 || paf->type >= MAX_SKILL)
      continue;

    fprintf(fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
            skill_table[paf->type].name, paf->where, paf->level,
            paf->duration, paf->modifier, paf->location, paf->bitvector);
  }
  fprintf(fp, "Auth %d\n", ch->nameauthed);
  if (ch->namedenied > 0)
    fprintf(fp, "Deny %d\n", ch->namedenied);


  fwrite_rle(ch->pcdata->explored, fp);
  save_property(fp, "Prop", ch->property, "fwrite_char", ch->name);
  fprintf(fp, "End\n\n");
  return;
}

/* write a pet */
void fwrite_pet(CHAR_DATA * pet, FILE * fp)
{
  return;
  AFFECT_DATA *paf;

  fprintf(fp, "#PET\n");

  fprintf(fp, "Vnum %ld\n", pet->pIndexData->vnum);
  fprintf(fp, "Name %s~\n", pet->name);
  fprintf(fp, "Ptype %d\n", pet->ptype);
  if (pet->short_descr != pet->pIndexData->short_descr)
    fprintf(fp, "ShD  %s~\n", pet->short_descr);
  if (pet->long_descr != pet->pIndexData->long_descr)
    fprintf(fp, "LnD  %s~\n", pet->long_descr);
  if (pet->description != pet->pIndexData->description)
    fprintf(fp, "Desc %s~\n", pet->description);
  if (pet->clan)
    fprintf(fp, "Clan %s~\n", clan_table[pet->clan].name);
  fprintf(fp, "Sex  %d\n", pet->sex);
  if (pet->act != pet->pIndexData->act)
    fprintf(fp, "Act  %s\n", print_flags(pet->act));
  if (pet->affected_by != pet->pIndexData->affected_by)
    fprintf(fp, "AfBy %s\n", print_flags(pet->affected_by));
  if (pet->shielded_by != pet->pIndexData->shielded_by)
    fprintf(fp, "ShBy %s\n", print_flags(pet->shielded_by));
  fprintf(fp, "Pos  %d\n", pet->position =
          POS_FIGHTING ? POS_STANDING : pet->position);

  for (paf = pet->affected; paf != NULL; paf = paf->next)
  {
    if (paf->type < 0 || paf->type >= MAX_SKILL)
      continue;

    fprintf(fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
            skill_table[paf->type].name, paf->where, paf->level,
            paf->duration, paf->modifier, paf->location, paf->bitvector);
  }

  save_property(fp, "Prop", pet->property, "fwrite_pet", pet->name);

  fprintf(fp, "End\n");
  return;
}

/*
 * Write an object and its contents.
 */
void fwrite_obj(CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest)
{
  EXTRA_DESCR_DATA *ed;
  AFFECT_DATA *paf;

  /* 
   * Slick recursion to write lists backwards,
   *   so loading them will load in forwards order.
   */
  if (obj->next_content != NULL)
    fwrite_obj(ch, obj->next_content, fp, iNest);

  fprintf(fp, "#O\n");
  fprintf(fp, "Vnum %ld\n", obj->pIndexData->vnum);
  if (obj->enchanted)
    fprintf(fp, "Enchanted\n");
  fprintf(fp, "Nest %d\n", iNest);

  /* these data are only used if they do not match the defaults */

  if (obj->name != obj->pIndexData->name)
    fprintf(fp, "Name %s~\n", obj->name);
  if (obj->short_descr != obj->pIndexData->short_descr)
    fprintf(fp, "ShD  %s~\n", obj->short_descr);
  if (obj->description != obj->pIndexData->description)
    fprintf(fp, "Desc %s~\n", obj->description);
  if (obj->extra_flags != obj->pIndexData->extra_flags)
    fprintf(fp, "ExtF %d\n", obj->extra_flags);
  if (obj->wear_flags != obj->pIndexData->wear_flags)
    fprintf(fp, "WeaF %d\n", obj->wear_flags);
  if (obj->item_type != obj->pIndexData->item_type)
    fprintf(fp, "Ityp %d\n", obj->item_type);
  if (obj->weight != obj->pIndexData->weight)
    fprintf(fp, "Wt   %d\n", obj->weight);
  if (obj->condition != obj->pIndexData->condition)
    fprintf(fp, "Cond %d\n", obj->condition);

  /* variable data */

  fprintf(fp, "Wear %d\n", obj->wear_loc);
  if (obj->level != obj->pIndexData->level)
    fprintf(fp, "Lev  %d\n", obj->level);
  if (obj->timer > 0)
    fprintf(fp, "Time %d\n", obj->timer);
  if (obj->cost > 0)
    fprintf(fp, "Cost %d\n", obj->cost);
  if (obj->value[0] != obj->pIndexData->value[0] ||
      obj->value[1] != obj->pIndexData->value[1] ||
      obj->value[2] != obj->pIndexData->value[2] ||
      obj->value[3] != obj->pIndexData->value[3] ||
      obj->value[4] != obj->pIndexData->value[4])
    fprintf(fp, "Val  %ld %ld %ld %ld %ld\n", obj->value[0],
            obj->value[1], obj->value[2], obj->value[3], obj->value[4]);

  switch (obj->item_type)
  {
    case ITEM_POTION:
    case ITEM_SCROLL:
      if (obj->value[1] > 0)
      {
        fprintf(fp, "Spell 1 '%s'\n", skill_table[obj->value[1]].name);
      }

      if (obj->value[2] > 0)
      {
        fprintf(fp, "Spell 2 '%s'\n", skill_table[obj->value[2]].name);
      }

      if (obj->value[3] > 0)
      {
        fprintf(fp, "Spell 3 '%s'\n", skill_table[obj->value[3]].name);
      }

      break;

    case ITEM_PILL:
    case ITEM_STAFF:
    case ITEM_WAND:
    case ITEM_INSTRUMENT:
      if (obj->value[3] > 0)
      {
        fprintf(fp, "Spell 3 '%s'\n", skill_table[obj->value[3]].name);
      }

      break;
  }

  for (paf = obj->affected; paf != NULL; paf = paf->next)
  {
    if (paf->type < 0 || paf->type >= MAX_SKILL)
      continue;
    fprintf(fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
            skill_table[paf->type].name, paf->where, paf->level,
            paf->duration, paf->modifier, paf->location, paf->bitvector);
  }

  for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
  {
    fprintf(fp, "ExDe %s~ %s~\n", ed->keyword, ed->description);
  }

  save_property(fp, "Prop", obj->property, "fwrite_obj", obj->name);

  fprintf(fp, "End\n\n");

  if (obj->contains != NULL)
    fwrite_obj(ch, obj->contains, fp, iNest + 1);

  return;
}

/*
 * Load a char and inventory into a new ch structure.
 */
bool load_char_obj(DESCRIPTOR_DATA * d, char *name)
{
  char strsave[MAX_INPUT_LENGTH];
  CHAR_DATA *ch;
  FILE *fp;
  bool found;
  int stat;

  ch = new_char();
  ch->pcdata = new_pcdata();

  d->character = ch;
  ch->desc = d;
  ch->name = str_dup(name);
  ch->id = get_pc_id();
  ch->race = race_lookup("human");
  ch->act = PLR_NOSUMMON;
  ch->comm = COMM_COMBINE | COMM_PROMPT | COMM_STORE;
  ch->pcdata->ctier = TIER_01;
  ch->pcdata->pkset = 0;
  ch->recall_room = 0;
  ch->prompt = str_dup("<{R%hhp {M%mm {G%vmv{x>");
  ch->pcdata->confirm_delete = false;
  ch->pcdata->pwd = str_dup("");
  ch->pcdata->bamfin = str_dup("");
  ch->pcdata->bamfout = str_dup("");
  ch->pcdata->who_descr = str_dup("");
  ch->pcdata->title = str_dup("");
  ch->pcdata->spouse = str_dup("");
  ch->pcdata->tier = 0;
  ch->pcdata->verbose = VERBOSE_SHIELD_COMP;
  for (stat = 0; stat < MAX_STATS; stat++)
    ch->perm_stat[stat] = 13;

  ch->pcdata->security = 0;     /* OLC */
  ch->pcdata->pkdeaths = 0;
  ch->pcdata->pkkills = 0;
  ch->pcdata->awins = 0;
  ch->pcdata->alosses = 0;
  ch->corner_timer = -1;
  bzero(ch->pcdata->explored, MAX_EXPLORE);
  ch->pcdata->board = &boards[DEFAULT_BOARD];
  found = false;

  sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(name));
  if ((fp = file_open(strsave, "r")) != NULL)
  {
    int iNest;

    for (iNest = 0; iNest < MAX_NEST; iNest++)
      rgObjNest[iNest] = NULL;

    found = true;
    for (;;)
    {
      char letter;
      char *word;

      letter = fread_letter(fp);
      if (letter == '*')
      {
        fread_to_eol(fp);
        continue;
      }

      if (letter != '#')
      {
        bug("Load_char_obj: # not found.", 0);
        break;
      }

      word = fread_word(fp);
      if (!str_cmp(word, "PLAYER"))
        fread_char(ch, fp);
      else if (!str_cmp(word, "OBJECT"))
        fread_obj(ch, fp);
      else if (!str_cmp(word, "O"))
        fread_obj(ch, fp);
      else if (!str_cmp(word, "PET"))
        fread_pet(ch, fp);
      else if (!str_cmp(word, "BANK"))
        fread_bank(ch, fp);
      else if (!str_cmp(word, "END"))
        break;
      else
      {
        bug("Load_char_obj: bad section.", 0);
        break;
      }
    }
    file_close(fp);
  }

  /* initialize race */
  if (found)
  {
    int i;

    if (ch->race == 0)
      ch->race = race_lookup("human");

    ch->size = pc_race_table[ch->race].size;
    ch->dam_type = 17;          /* punch */

    for (i = 0; i < 5; i++)
    {
      if (pc_race_table[ch->race].skills[i] == NULL)
        break;
      group_add(ch, pc_race_table[ch->race].skills[i], false);
    }
    ch->affected_by = ch->affected_by | race_table[ch->race].aff;
    ch->shielded_by = ch->shielded_by | race_table[ch->race].shd;
    ch->imm_flags = ch->imm_flags | race_table[ch->race].imm;
    ch->res_flags = ch->res_flags | race_table[ch->race].res;
    ch->vuln_flags = ch->vuln_flags | race_table[ch->race].vuln;
    ch->form = race_table[ch->race].form;
    ch->parts = race_table[ch->race].parts;

    /* ream gold */
    if (found && ch->version < 4)
      ch->gold /= 100;

    if (ch->version < 6)
      if (IS_SET(ch->act, PLR_NOCOLOUR) && d)
        d->ansi = false;
  }

  if (found && !IS_SET(ch->pcdata->verbose, VERBOSE_SHIELD) &&
      !IS_SET(ch->pcdata->verbose, VERBOSE_SHIELD_COMP))
    SET_BIT(ch->pcdata->verbose, VERBOSE_SHIELD_COMP);
  return found;
}

/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = true;			\
				    break;				\
				}

#ifdef KEYS
#undef KEYS
#endif

#define KEYS( literal, field, value )		\
		if(!str_cmp(word, literal))	\
		{				\
			_free_string(field, __FILE__, __LINE__);	\
			field = value;		\
			fMatch = true;		\
			break;			\
		}

void fread_char(CHAR_DATA * ch, FILE * fp)
{
  char buf[MAX_STRING_LENGTH];
  char *word;
  bool fMatch;
  bool fauth = false;
  bool fdeny = false;

  int count = 0;
  int dcount = 0;
  int fcount = 0;
  int lastlogoff = current_time;
  int percent;


  sprintf(buf, "Loading %s.", ch->name);
  if (str_cmp(ch->name, ""))
  {
    log_string(buf);
  }

  for (;;)
  {
    word = feof(fp) ? "End" : fread_word(fp);
    fMatch = false;

    switch (UPPER(word[0]))
    {
      case '*':
        fMatch = true;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Act", ch->act, fread_flag(fp));
        KEY("Act2", ch->act2, fread_flag(fp));
        KEY("AffectedBy", ch->affected_by, fread_flag(fp));
        KEY("AfBy", ch->affected_by, fread_flag(fp));
        KEY("Alignment", ch->alignment, fread_number(fp));
        KEY("Alig", ch->alignment, fread_number(fp));
        KEY("Awin", ch->pcdata->awins, fread_number(fp));
        KEY("Alos", ch->pcdata->alosses, fread_number(fp));
        if (!str_cmp(word, "Auth"))
        {
          ch->nameauthed = fread_number(fp);
          fauth = true;
          fMatch = true;
          break;
        }
        if (!str_cmp(word, "Alia"))
        {
          if (count >= MAX_ALIAS)
          {
            fread_to_eol(fp);
            fMatch = true;
            break;
          }

          ch->pcdata->alias[count] = str_dup(fread_word(fp));
          ch->pcdata->alias_sub[count] = str_dup(fread_word(fp));
          count++;
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "Alias"))
        {
          if (count >= MAX_ALIAS)
          {
            fread_to_eol(fp);
            fMatch = true;
            break;
          }

          ch->pcdata->alias[count] = str_dup(fread_word(fp));
          ch->pcdata->alias_sub[count] = fread_string(fp);
          count++;
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "AC") || !str_cmp(word, "Armor"))
        {
          fread_to_eol(fp);
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "ACs"))
        {
          int i;

          for (i = 0; i < 4; i++)
            ch->armor[i] = fread_number(fp);
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "AffD"))
        {
          AFFECT_DATA *paf;
          int sn;

          paf = new_affect();

          sn = skill_lookup(fread_word(fp));
          if (sn < 0 || sn > MAX_SKILL)
            break;
          else
            paf->type = sn;

          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_long(fp);
          paf->next = ch->affected;
          ch->affected = paf;
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "Affc"))
        {
          AFFECT_DATA *paf;
          int sn;

          paf = new_affect();

          sn = skill_lookup(fread_word(fp));
          if (sn < 0 || sn > MAX_SKILL)
            bug("Fread_char: unknown skill.", 0);
          else
            paf->type = sn;

          paf->where = fread_number(fp);
          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_long(fp);
          paf->next = ch->affected;
          ch->affected = paf;
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "AttrMod") || !str_cmp(word, "AMod"))
        {
          int stat;

          for (stat = 0; stat < MAX_STATS; stat++)
            ch->mod_stat[stat] = fread_number(fp);
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "AttrPerm") || !str_cmp(word, "Attr"))
        {
          int stat;

          for (stat = 0; stat < MAX_STATS; stat++)
            ch->perm_stat[stat] = fread_number(fp);
          fMatch = true;
          break;
        }
        break;

      case 'B':
        KEYS("Bamfin", ch->pcdata->bamfin, fread_string(fp));
        KEYS("Bamfout", ch->pcdata->bamfout, fread_string(fp));
        KEYS("Bin", ch->pcdata->bamfin, fread_string(fp));
        KEYS("Bout", ch->pcdata->bamfout, fread_string(fp));
        KEY("Balance", ch->pcdata->balance, fread_number(fp));

        if (!str_cmp(word, "Boards"))
        {
          int i, num = fread_number(fp);  /* number of boards saved */
          char *boardname;

          for (; num; num--)    /* for each of the board saved */
          {
            boardname = fread_word(fp);
            i = board_lookup(boardname);  /* find board number */

            if (i == BOARD_NOTFOUND)  /* Does board still exist ? */
            {
              sprintf(buf,
                      "fread_char: %s had unknown board name: %s. Skipped.",
                      ch->name, boardname);
              log_string(buf);
              fread_number(fp); /* read last_note and skip info */
            }
            else                /* Save it */
              ch->pcdata->last_note[i] = fread_number(fp);
          }                     /* for */

          fMatch = true;
          break;
        }


        break;

      case 'C':
        if (!str_cmp(word, "Clas"))
        {
          char *tmp = fread_string(fp);

          ch->class = class_lookup(tmp);
          free_string(tmp);
          fMatch = true;
          break;
        }
        KEY("Class", ch->class, fread_number(fp));
        KEY("Cla", ch->class, fread_number(fp));
        if (!str_cmp(word, "Clan"))
        {
          char *tmp = fread_string(fp);

          ch->clan = clan_lookup(tmp);
          free_string(tmp);
          fMatch = true;
          break;
        }
        if (!str_cmp(word, "Clead"))
        {
          char *tmp = fread_string(fp);

          ch->clead = clan_lookup(tmp);
          free_string(tmp);
          fMatch = true;
          break;
        }
        KEY("CRank", ch->clan_rank, fread_number(fp));
        KEY("Coauc", ch->color_auc, fread_number(fp));
        KEY("Cocgo", ch->color_cgo, fread_number(fp));
        KEY("Cocla", ch->color_cla, fread_number(fp));
        KEY("Cocon", ch->color_con, fread_number(fp));
        KEY("Codis", ch->color_dis, fread_number(fp));
        KEY("Cofig", ch->color_fig, fread_number(fp));
        KEY("Cogos", ch->color_gos, fread_number(fp));
        KEY("Cogra", ch->color_gra, fread_number(fp));
        KEY("Cogte", ch->color_gte, fread_number(fp));
        KEY("Coimm", ch->color_imm, fread_number(fp));
        KEY("Comob", ch->color_mob, fread_number(fp));
        KEY("Comus", ch->color_mus, fread_number(fp));
        KEY("Coopp", ch->color_opp, fread_number(fp));
        KEY("Coqgo", ch->color_qgo, fread_number(fp));
        KEY("Coque", ch->color_que, fread_number(fp));
        KEY("Coquo", ch->color_quo, fread_number(fp));
        KEY("Coroo", ch->color_roo, fread_number(fp));
        KEY("Cosay", ch->color_say, fread_number(fp));
        KEY("Cosho", ch->color_sho, fread_number(fp));
        KEY("Cotel", ch->color_tel, fread_number(fp));
        KEY("Cowit", ch->color_wit, fread_number(fp));
        KEY("Cowiz", ch->color_wiz, fread_number(fp));
        KEY("Ctim", ch->corner_timer, fread_number(fp));
        KEY("Ctier", ch->pcdata->ctier, fread_number(fp));
        if (!str_cmp(word, "Condition") || !str_cmp(word, "Cond"))
        {
          ch->pcdata->condition[0] = fread_number(fp);
          ch->pcdata->condition[1] = fread_number(fp);
          ch->pcdata->condition[2] = fread_number(fp);
          fMatch = true;
          break;
        }
        if (!str_cmp(word, "Cnd"))
        {
          ch->pcdata->condition[0] = fread_number(fp);
          ch->pcdata->condition[1] = fread_number(fp);
          ch->pcdata->condition[2] = fread_number(fp);
          ch->pcdata->condition[3] = fread_number(fp);
          fMatch = true;
          break;
        }
        KEY("Comm", ch->comm, fread_flag(fp));

        break;

      case 'D':
        KEY("Damroll", ch->damroll, fread_number(fp));
        KEY("Dam", ch->damroll, fread_number(fp));
        if (!str_cmp(word, "Deny"))
        {
          ch->namedenied = fread_number(fp);
          fdeny = true;
          fMatch = true;
          break;
        }
        if (!str_cmp(word, "Dupes"))
        {
          if (dcount >= MAX_DUPES)
          {
            fread_to_eol(fp);
            fMatch = true;
            break;
          }

          ch->pcdata->dupes[dcount] = fread_string(fp);
          dcount++;
          fMatch = true;
        }
        break;

      case 'E':
        if (!str_cmp(word, "End"))
        {

          if (ch->level > MAX_LEVEL_NOAUTH && !ch->nameauthed)
          {
            ch->nameauthed = 1;
            ch->namedenied = 0;
          }

          /* adjust hp mana move up -- here for speed's sake */
          percent = (current_time - lastlogoff) * 25 / (2 * 60 * 60);

          percent = UMIN(percent, 100);

          if (percent > 0 && !IS_AFFECTED(ch, AFF_POISON) &&
              !IS_AFFECTED(ch, AFF_PLAGUE))
          {
            ch->hit += (ch->max_hit - ch->hit) * percent / 100;
            ch->mana += (ch->max_mana - ch->mana) * percent / 100;
            ch->move += (ch->max_move - ch->move) * percent / 100;
          }
          return;
        }
        KEY("Exp", ch->exp, fread_number(fp));
        break;

      case 'F':
        if (!str_cmp(word, "Forge"))
        {
          if (fcount >= MAX_FORGET)
          {
            fread_to_eol(fp);
            fMatch = true;
            break;
          }

          ch->pcdata->forget[fcount] = fread_string(fp);
          fcount++;
          fMatch = true;
        }
        break;

      case 'G':
        KEY("Ghos", ch->ghost_level, fread_number(fp));
        KEY("Gold", ch->gold, fread_number(fp));
        if (!str_cmp(word, "Group") || !str_cmp(word, "Gr"))
        {
          int gn;
          char *temp;

          temp = fread_word(fp);
          gn = group_lookup(temp);
          if (gn < 0)
          {
            fprintf(stderr, "%s", temp);
            bug("Fread_char: unknown group. ", 0);
          }
          else
            gn_add(ch, gn);
          fMatch = true;
        }
        break;

      case 'H':
        KEY("Hitroll", ch->hitroll, fread_number(fp));
        KEY("Hit", ch->hitroll, fread_number(fp));

        if (!str_cmp(word, "HpManaMove") || !str_cmp(word, "HMV"))
        {
          ch->hit = fread_number(fp);
          ch->max_hit = fread_number(fp);
          ch->mana = fread_number(fp);
          ch->max_mana = fread_number(fp);
          ch->move = fread_number(fp);
          ch->max_move = fread_number(fp);
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "HpManaMovePerm") || !str_cmp(word, "HMVP"))
        {
          ch->pcdata->perm_hit = fread_number(fp);
          ch->pcdata->perm_mana = fread_number(fp);
          ch->pcdata->perm_move = fread_number(fp);
          fMatch = true;
          break;
        }

        break;

      case 'I':
        KEY("Id", ch->id, fread_number(fp));
        KEY("InvisLevel", ch->invis_level, fread_number(fp));
        KEY("Inco", ch->incog_level, fread_number(fp));
        KEY("Invi", ch->invis_level, fread_number(fp));
        break;

      case 'J':
        break;

      case 'L':
        KEY("LastLevel", ch->pcdata->last_level, fread_number(fp));
        KEY("LLev", ch->pcdata->last_level, fread_number(fp));
        KEY("Level", ch->level, fread_number(fp));
        KEY("Lev", ch->level, fread_number(fp));
        KEY("Levl", ch->level, fread_number(fp));
        if (!str_cmp(word, "LogO"))
        {
          lastlogoff = fread_number(fp);
          ch->llogoff = (time_t) lastlogoff;
          fMatch = true;
          break;
        }
        KEYS("LongDescr", ch->long_descr, fread_string(fp));
        KEYS("LnD", ch->long_descr, fread_string(fp));
        break;

      case 'N':
        KEYS("Name", ch->name, fread_string(fp));
        break;

      case 'P':
        KEYS("Password", ch->pcdata->pwd, fread_string(fp));
        KEYS("Pass", ch->pcdata->pwd, fread_string(fp));
        KEY("Plat", ch->platinum, fread_number(fp));
        KEY("Played", ch->played, fread_number(fp));
        KEY("Plyd", ch->played, fread_number(fp));
        KEY("Points", ch->pcdata->points, fread_number(fp));
        KEY("Pnts", ch->pcdata->points, fread_number(fp));
        KEY("Position", ch->position, fread_number(fp));
        KEY("Pos", ch->position, fread_number(fp));
        KEY("Practice", ch->practice, fread_number(fp));
        KEY("Prac", ch->practice, fread_number(fp));
        KEYS("Prompt", ch->prompt, fread_string(fp));
        KEYS("Prom", ch->prompt, fread_string(fp));
        KEY("Pkdt", ch->pcdata->pkdeaths, fread_number(fp));
        KEY("Pkkl", ch->pcdata->pkkills, fread_number(fp));
        KEY("Pkset", ch->pcdata->pkset, fread_number(fp));
        KEY("Ptim", ch->pk_timer, fread_number(fp));
        if (!str_cmp(word, "Prop"))
        {
          char *key;
          char *type;
          char *value;
          int i;
          bool b;
          char c;
          long l;

          key = fread_string(fp);
          type = fread_string(fp);
          value = fread_string(fp);

          switch (which_keyword(type, "int", "bool", "string",
                                "char", "long", NULL))
          {
            case 1:
              i = atoi(value);
              SetCharProperty(ch, PROPERTY_INT, key, &i);
              fMatch = true;
              break;
            case 2:
              switch (which_keyword(value, "true", "false", NULL))
              {
                case 1:
                  b = true;
                  SetCharProperty(ch, PROPERTY_BOOL, key, &b);
                  fMatch = true;
                  break;
                case 2:
                  b = false;
                  SetCharProperty(ch, PROPERTY_BOOL, key, &b);
                  fMatch = true;
                  break;
                default:
                  break;
              }
              break;
            case 3:
              SetCharProperty(ch, PROPERTY_STRING, key, value);
              fMatch = true;
              break;
            case 4:
              c = value[0];
              SetCharProperty(ch, PROPERTY_CHAR, key, &c);
              fMatch = true;
              break;
            case 5:
              l = atol(value);
              SetCharProperty(ch, PROPERTY_LONG, key, &l);
              fMatch = true;
              break;
            default:
              break;
          }
          free_string(key);
          free_string(type);
          free_string(value);
          break;
        }
        break;

      case 'Q':
        KEY("QuestPnts", ch->pcdata->questpoints, fread_number(fp));
        KEY("QuestNext", ch->pcdata->nextquest, fread_number(fp));
        KEY("QuestCount", ch->pcdata->countdown, fread_number(fp));
        KEY("QuestLoc", ch->pcdata->questloc, fread_number(fp));
        KEY("QuestObj", ch->pcdata->questobj, fread_number(fp));
        KEY("QuestGiver", ch->pcdata->questgiver, fread_number(fp));
        KEY("QuestMob", ch->pcdata->questmob, fread_number(fp));
        KEY("QPS", ch->qps, fread_number(fp));
      case 'R':
        KEYS("Refer", ch->pcdata->refer, fread_string(fp));
        KEY("RPS", ch->rps, fread_number(fp));
        KEY("RPST", ch->rpst, fread_number(fp));
        KEY("Ree", ch->redeem, fread_number(fp));
        KEY("ReRm", ch->recall_room, fread_number(fp));
        if (!str_cmp(word, "Race"))
        {
          char *tmp = fread_string(fp);

          ch->race = race_lookup(tmp);
          free_string(tmp);
          fMatch = true;
          break;
        }
        KEY("Reca", ch->pcdata->recall, fread_number(fp));
        if (!str_cmp(word, "Room"))
        {
          ch->in_room = get_room_index(fread_number(fp));
          if (ch->in_room == NULL)
            ch->in_room = get_room_index(ROOM_VNUM_LIMBO);
          fMatch = true;
          break;
        }
        if (!str_cmp(word, "RoomRLE"))
        {
          fread_rle(ch->pcdata->explored, fp);
          fMatch = true;
          break;
        }

        break;

      case 'S':
        KEYS("SC", ch->sex_consenter, fread_string(fp));
        KEY("SavingThrow", ch->saving_throw, fread_number(fp));
        KEY("Save", ch->saving_throw, fread_number(fp));
        KEY("Scro", ch->lines, fread_number(fp));
        KEY("Sex", ch->sex, fread_number(fp));
        KEY("ShBy", ch->shielded_by, fread_flag(fp));
        KEY("ShieldedBy", ch->shielded_by, fread_flag(fp));
        KEYS("ShortDescr", ch->short_descr, fread_string(fp));
        KEYS("ShD", ch->short_descr, fread_string(fp));
        KEY("Sec", ch->pcdata->security, fread_number(fp)); /* OLC 
                                                             */
        KEY("Silv", ch->silver, fread_number(fp));
        KEY("Shares", ch->pcdata->shares, fread_number(fp));

        if (!str_cmp(word, "Skill") || !str_cmp(word, "Sk"))
        {
          int sn;
          int value;
          char *temp;

          value = fread_number(fp);
          temp = fread_word(fp);
          sn = skill_lookup(temp);
          if (sn < 0 || sn > MAX_SKILL)
          {
            fprintf(stderr, "%s", temp);
            bug("Fread_char: unknown skill. ", 0);
          }
          else
          {
            ch->pcdata->learned[sn] = value;
          }
          fMatch = true;
        }
        KEYS("Spouse", ch->pcdata->spouse, fread_string(fp));

        break;

      case 'T':
        KEY("Tier", ch->pcdata->tier, fread_number(fp));
        KEY("TrueSex", ch->pcdata->true_sex, fread_number(fp));
        KEY("TSex", ch->pcdata->true_sex, fread_number(fp));
        KEY("TCol", ch->talk_color, fread_number(fp));
        KEY("Trai", ch->train, fread_number(fp));
        KEY("Trust", ch->trust, fread_number(fp));
        KEY("Tru", ch->trust, fread_number(fp));

        if (!str_cmp(word, "Title") || !str_cmp(word, "Titl"))
        {
          ch->pcdata->title = fread_string(fp);
          if (ch->pcdata->title[0] != '.' &&
              ch->pcdata->title[0] != ',' &&
              ch->pcdata->title[0] != '!' && ch->pcdata->title[0] != '?')
          {
            sprintf(buf, " %s", ch->pcdata->title);
            free_string(ch->pcdata->title);
            ch->pcdata->title = str_dup(buf);
          }
          fMatch = true;
          break;
        }

        break;

      case 'V':
        KEY("Version", ch->version, fread_number(fp));
        KEY("Vers", ch->version, fread_number(fp));
        KEY("Verb", ch->pcdata->verbose, fread_flag(fp));
        if (!str_cmp(word, "Vnum"))
        {
          ch->pIndexData = get_mob_index(fread_number(fp));
          fMatch = true;
          break;
        }
        break;

      case 'W':
        KEYS("Whod", ch->pcdata->who_descr, fread_string(fp));
        KEY("Wimpy", ch->wimpy, fread_number(fp));
        KEY("Wimp", ch->wimpy, fread_number(fp));
        KEY("Wizn", ch->wiznet, fread_flag(fp));
        break;
    }
    if (is_clan(ch))
    {
      ch->pcdata->pkset = true;
    }

    if (!fMatch)
    {
      bug("Fread_char: no match. ( IGNORE THIS )", 0);
      fread_to_eol(fp);
    }
  }
}

/* load a pet from the forgotten reaches */
void fread_pet(CHAR_DATA * ch, FILE * fp)
{
  return;
  char *word;
  CHAR_DATA *pet;
  bool fMatch;

  /* first entry had BETTER be the vnum or we barf */
  word = feof(fp) ? "END" : fread_word(fp);
  if (!str_cmp(word, "Vnum"))
  {
    long vnum;

    vnum = fread_long(fp);
    if (get_mob_index(vnum) == NULL)
    {
      bug("Fread_pet: bad vnum %ld.", vnum);
      pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));

    }
    else
      pet = create_mobile(get_mob_index(vnum));
  }
  else
  {
    bug("Fread_pet: no vnum in file.", 0);
    pet = create_mobile(get_mob_index(MOB_VNUM_FIDO));
  }

  for (;;)
  {
    word = feof(fp) ? "END" : fread_word(fp);
    fMatch = false;

    switch (UPPER(word[0]))
    {
      case '*':
        fMatch = true;
        fread_to_eol(fp);
        break;

      case 'A':
        KEY("Act", pet->act, fread_flag(fp));
        KEY("AfBy", pet->affected_by, fread_flag(fp));

        if (!str_cmp(word, "AffD"))
        {
          AFFECT_DATA *paf;
          int sn;

          paf = new_affect();

          sn = skill_lookup(fread_word(fp));
          if (sn < 0)
            bug("Fread_char: unknown skill.", 0);
          else
            paf->type = sn;

          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_long(fp);
          paf->next = pet->affected;
          pet->affected = paf;
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "Affc"))
        {
          AFFECT_DATA *paf;
          int sn;
          paf = new_affect();

          sn = skill_lookup(fread_word(fp));
          if (sn < 0)
            bug("Fread_char: unknown skill.", 0);
          else
            paf->type = sn;

          paf->where = fread_number(fp);
          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_long(fp);
          paf->next = pet->affected;
          pet->affected = paf;
          fMatch = true;
          break;
        }

      case 'C':
        if (!str_cmp(word, "Clan"))
        {
          char *tmp = fread_string(fp);

          pet->clan = clan_lookup(tmp);
          free_string(tmp);
          fMatch = true;
          break;
        }
        break;

      case 'D':
        KEYS("Desc", pet->description, fread_string(fp));
        break;

      case 'E':
        if (!str_cmp(word, "End"))
        {
          pet->leader = ch;
          pet->master = ch;
          ch->pet = pet;
          char_to_room(pet, ch->in_room);
          return;
        }
        break;

      case 'H':
        if (!str_cmp(word, "HMV"))
        {
          pet->hit = fread_number(fp);
          pet->max_hit = fread_number(fp);
          pet->mana = fread_number(fp);
          pet->max_mana = fread_number(fp);
          pet->move = fread_number(fp);
          pet->max_move = fread_number(fp);
          fMatch = true;
          break;
        }
        break;

      case 'L':
        KEYS("LnD", pet->long_descr, fread_string(fp));
        break;

      case 'N':
        KEYS("Name", pet->name, fread_string(fp));
        break;

      case 'P':
        KEY("Ptype", pet->ptype, fread_number(fp));
        if (!str_cmp(word, "Prop"))
        {
          char *key;
          char *type;
          char *value;
          int i;
          bool b;
          char c;
          long l;

          key = fread_string(fp);
          type = fread_string(fp);
          value = fread_string(fp);

          switch (which_keyword(type, "int", "bool", "string",
                                "char", "long", NULL))
          {
            case 1:
              i = atoi(value);
              SetCharProperty(pet, PROPERTY_INT, key, &i);
              fMatch = true;
              break;
            case 2:
              switch (which_keyword(value, "true", "false", NULL))
              {
                case 1:
                  b = true;
                  SetCharProperty(pet, PROPERTY_BOOL, key, &b);
                  fMatch = true;
                  break;
                case 2:
                  b = false;
                  SetCharProperty(pet, PROPERTY_BOOL, key, &b);
                  fMatch = true;
                  break;
                default:
                  break;
              }
              break;
            case 3:
              SetCharProperty(pet, PROPERTY_STRING, key, value);
              fMatch = true;
              break;
            case 4:
              c = value[0];
              SetCharProperty(pet, PROPERTY_CHAR, key, &c);
              fMatch = true;
              break;
            case 5:
              l = atol(value);
              SetCharProperty(pet, PROPERTY_LONG, key, &l);
              fMatch = true;
              break;
            default:
              break;
          }
          free_string(key);
          free_string(type);
          free_string(value);
          break;
        }
        break;

      case 'S':
        KEY("Sex", pet->sex, fread_number(fp));
        KEYS("ShD", pet->short_descr, fread_string(fp));
        KEY("ShBy", pet->shielded_by, fread_flag(fp));
        break;

        if (!fMatch)
        {
          bug("Fread_pet: no match.", 0);
          fread_to_eol(fp);
        }

    }
  }
}

void fread_obj(CHAR_DATA * ch, FILE * fp)
{
  OBJ_DATA *obj;
  char *word;
  int iNest;
  bool fMatch;
  bool fNest;
  bool fVnum;
  bool first;

  fVnum = false;
  obj = NULL;
  first = true;                 /* used to counter fp offset */

  word = feof(fp) ? "End" : fread_word(fp);
  if (!str_cmp(word, "Vnum"))
  {
    long vnum;

    first = false;              /* fp will be in right place */

    vnum = fread_long(fp);
    if (get_obj_index(vnum) == NULL)
    {
      bug("Fread_obj: bad vnum %ld.", vnum);
      obj = create_object(get_obj_index(OBJ_VNUM_BAG));
    }
    else
    {
      obj = create_object(get_obj_index(vnum));
    }

  }

  if (obj == NULL)              /* either not found or old style */
  {
    obj = new_obj();
    obj->name = str_dup("");
    obj->short_descr = str_dup("");
    obj->description = str_dup("");
  }

  fNest = false;
  fVnum = true;
  iNest = 0;

  for (;;)
  {
    if (first)
      first = false;
    else
      word = feof(fp) ? "End" : fread_word(fp);
    fMatch = false;

    switch (UPPER(word[0]))
    {
      case '*':
        fMatch = true;
        fread_to_eol(fp);
        break;

      case 'A':
        if (!str_cmp(word, "AffD"))
        {
          AFFECT_DATA *paf;
          int sn;

          paf = new_affect();

          sn = skill_lookup(fread_word(fp));
          if (sn < 0)
            bug("Fread_obj: unknown skill.", 0);
          else
            paf->type = sn;

          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_long(fp);
          paf->next = obj->affected;
          obj->affected = paf;
          fMatch = true;
          break;
        }
        if (!str_cmp(word, "Affc"))
        {
          AFFECT_DATA *paf;
          int sn;


          paf = new_affect();

          sn = skill_lookup(fread_word(fp));
          if (sn < 0)
            bug("Fread_obj: unknown skill.", 0);
          else
            paf->type = sn;

          paf->where = fread_number(fp);
          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_long(fp);
          paf->next = obj->affected;
          obj->affected = paf;
          fMatch = true;
          break;
        }
        break;

      case 'C':
        KEY("Cond", obj->condition, fread_number(fp));
        KEY("Cost", obj->cost, fread_number(fp));
        break;

      case 'D':
        KEYS("Description", obj->description, fread_string(fp));
        KEYS("Desc", obj->description, fread_string(fp));
        break;

      case 'E':

        if (!str_cmp(word, "Enchanted"))
        {
          obj->enchanted = true;
          fMatch = true;
          break;
        }

        KEY("ExtraFlags", obj->extra_flags, fread_number(fp));
        KEY("ExtF", obj->extra_flags, fread_number(fp));

        if (!str_cmp(word, "ExtraDescr") || !str_cmp(word, "ExDe"))
        {
          EXTRA_DESCR_DATA *ed;

          ed = new_extra_descr();

          ed->keyword = fread_string(fp);
          ed->description = fread_string(fp);
          ed->next = obj->extra_descr;
          obj->extra_descr = ed;
          fMatch = true;
        }

        if (!str_cmp(word, "End"))
        {
          if (!fNest || (fVnum && obj->pIndexData == NULL))
          {
            bug("Fread_obj: incomplete object.", 0);
            free_obj(obj);
            return;
          }
          else
          {
            if (!fVnum)
            {
              free_obj(obj);
              obj = create_object(get_obj_index(OBJ_VNUM_DUMMY));
            }

            if (iNest == 0 || rgObjNest[iNest] == NULL)
              obj_to_char(obj, ch);
            else
              obj_to_obj(obj, rgObjNest[iNest - 1]);
            if (obj->pIndexData->vnum == OBJ_VNUM_QDIAMOND)
            {
              ch->platinum++;
              extract_obj(obj);
            }
            return;
          }
        }
        break;

      case 'I':
        KEY("ItemType", obj->item_type, fread_number(fp));
        KEY("Ityp", obj->item_type, fread_number(fp));
        break;

      case 'L':
        KEY("Level", obj->level, fread_number(fp));
        KEY("Lev", obj->level, fread_number(fp));
        break;

      case 'N':
        KEYS("Name", obj->name, fread_string(fp));

        if (!str_cmp(word, "Nest"))
        {
          iNest = fread_number(fp);
          if (iNest < 0 || iNest >= MAX_NEST)
          {
            bug("Fread_obj: bad nest %d.", iNest);
          }
          else
          {
            rgObjNest[iNest] = obj;
            fNest = true;
          }
          fMatch = true;
        }
        break;

      case 'O':
        break;

      case 'P':
        if (!str_cmp(word, "Prop"))
        {
          char *temp;
          char key[MAX_STRING_LENGTH];
          char type[MAX_STRING_LENGTH];
          char value[MAX_STRING_LENGTH];
          int i;
          bool b;
          char c;
          long l;

          // try to see the difference between '..' .. '..' and ..~ ..~ ..~
          c = fread_letter(fp);
          ungetc(c, fp);
          if (c == '\'')
          {
            temp = fread_string_eol(fp);
            temp = one_argument(temp, key);
            temp = one_argument(temp, type);
            temp = one_argument(temp, value);
          }
          else
          {
            temp = fread_string(fp);
            strncpy(key, temp, MAX_STRING_LENGTH);
            temp = fread_string(fp);
            strncpy(type, temp, MAX_STRING_LENGTH);
            temp = fread_string(fp);
            strncpy(value, temp, MAX_STRING_LENGTH);
          }

          switch (which_keyword(type, "int", "bool", "string",
                                "char", "long", NULL))
          {
            case 1:
              i = atoi(value);
              SetObjectProperty(obj, PROPERTY_INT, key, &i);
              fMatch = true;
              break;
            case 2:
              switch (which_keyword(value, "true", "false", NULL))
              {
                case 1:
                  b = true;
                  SetObjectProperty(obj, PROPERTY_BOOL, key, &b);
                  fMatch = true;
                  break;
                case 2:
                  b = false;
                  SetObjectProperty(obj, PROPERTY_BOOL, key, &b);
                  fMatch = true;
                  break;
              }
              break;
            case 3:
              SetObjectProperty(obj, PROPERTY_STRING, key, value);
              fMatch = true;
              break;
            case 4:
              c = value[0];
              SetObjectProperty(obj, PROPERTY_CHAR, key, &c);
              fMatch = true;
              break;
            case 5:
              l = atol(value);
              SetObjectProperty(obj, PROPERTY_LONG, key, &l);
              fMatch = true;
              break;
          }
          break;
        }
        break;

      case 'S':
        KEYS("ShortDescr", obj->short_descr, fread_string(fp));
        KEYS("ShD", obj->short_descr, fread_string(fp));

        if (!str_cmp(word, "Spell"))
        {
          int iValue;
          int sn;

          iValue = fread_number(fp);
          sn = skill_lookup(fread_word(fp));
          if (iValue < 0 || iValue > 3)
          {
            bug("Fread_obj: bad iValue %d.", iValue);
          }
          else if (sn < 0)
          {
            bug("Fread_obj: unknown skill.", 0);
          }
          else
          {
            obj->value[iValue] = sn;
          }
          fMatch = true;
          break;
        }

        break;

      case 'T':
        KEY("Timer", obj->timer, fread_number(fp));
        KEY("Time", obj->timer, fread_number(fp));
        break;

      case 'V':
        if (!str_cmp(word, "Values") || !str_cmp(word, "Vals"))
        {
          obj->value[0] = fread_long(fp);
          obj->value[1] = fread_long(fp);
          obj->value[2] = fread_long(fp);
          obj->value[3] = fread_long(fp);
          if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
            obj->value[0] = obj->pIndexData->value[0];
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "Val"))
        {
          obj->value[0] = fread_long(fp);
          obj->value[1] = fread_long(fp);
          obj->value[2] = fread_long(fp);
          obj->value[3] = fread_long(fp);
          obj->value[4] = fread_long(fp);

          fMatch = true;
          break;
        }

        if (!str_cmp(word, "Vnum"))
        {
          long vnum;

          vnum = fread_long(fp);
          if ((obj->pIndexData = get_obj_index(vnum)) == NULL)
            bug("Fread_obj: bad vnum %ld.", vnum);
          else
            fVnum = true;
          fMatch = true;
          break;
        }
        break;

      case 'W':
        KEY("WearFlags", obj->wear_flags, fread_number(fp));
        KEY("WeaF", obj->wear_flags, fread_number(fp));
        KEY("WearLoc", obj->wear_loc, fread_number(fp));
        KEY("Wear", obj->wear_loc, fread_number(fp));
        KEY("Weight", obj->weight, fread_number(fp));
        KEY("Wt", obj->weight, fread_number(fp));
        break;

    }

    if (!fMatch)
    {
      bug("Fread_obj: no match.", 0);
      fread_to_eol(fp);
    }
  }
  if (obj->pIndexData->vnum == OBJ_VNUM_QDIAMOND)
  {
    ch->platinum++;
    extract_obj(obj);
  }
}
void backup_char_obj(CHAR_DATA * ch)
{
  char buf[MSL];
  char strsave[MAX_INPUT_LENGTH];
  FILE *fp;

  if (IS_NPC(ch))
    return;

  if (!IS_VALID(ch))
  {
    bug("save_char_obj: trying to save invalid character", 0);
    return;
  }

  if (ch->desc != NULL && ch->desc->original != NULL)
    ch = ch->desc->original;

  sprintf(buf,
          "cp -f ../player/backup/backup2/%s ../player/backup/backup2/backup3/",
          capitalize(ch->name));
  system(buf);
  sprintf(buf, "cp -f ../player/backup/%s ../player/backup/backup2/",
          capitalize(ch->name));
  system(buf);


  /* create god log */
  if (IS_IMMORTAL(ch) || ch->level >= LEVEL_IMMORTAL)
  {
    sprintf(strsave, "%s%s", OLDPFS_DIR, capitalize(ch->name));
    if ((fp = file_open(strsave, "w")) == NULL)
    {
      bug("Save_char_obj: fopen", 0);
      perror(strsave);
    }

    fprintf(fp, "Lev %2d Trust %2d  %s%s\n", ch->level, get_trust(ch),
            ch->name, ch->pcdata->title);
    file_close(fp);
  }

  sprintf(strsave, "%s%s", OLDPFS_DIR, capitalize(ch->name));

  if ((fp = file_open(TEMP_FILE, "w")) == NULL)
  {
    bug("Save_char_obj: fopen", 0);
    perror(strsave);
  }
  else
  {
    fwrite_char(ch, fp);
    if (ch->carrying != NULL)
      fwrite_obj(ch, ch->carrying, fp, 0);
    if (ch->pet != NULL && ch->pet->in_room == ch->in_room)
      fwrite_pet(ch->pet, fp);
    fprintf(fp, "#END\n");
  }
  file_close(fp);
  rename(TEMP_FILE, strsave);

  send_to_char("A backup of your pfile has been made.\n\r", ch);
  return;
}
void fwrite_bank(CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest)
{

  EXTRA_DESCR_DATA *ed;
  AFFECT_DATA *paf;

  /* 
   * Slick recursion to write lists backwards,
   *   so loading them will load in forwards order.
   */
  if (obj->next_content != NULL)
    fwrite_bank(ch, obj->next_content, fp, iNest);

  fprintf(fp, "#BANK\n");
  fprintf(fp, "Vnum %ld\n", obj->pIndexData->vnum);
  if (obj->enchanted)
    fprintf(fp, "Enchanted\n");
  fprintf(fp, "Nest %d\n", iNest);

  /* these data are only used if they do not match the defaults */

  if (obj->name != obj->pIndexData->name)
    fprintf(fp, "Name %s~\n", obj->name);
  if (obj->short_descr != obj->pIndexData->short_descr)
    fprintf(fp, "ShD  %s~\n", obj->short_descr);
  if (obj->description != obj->pIndexData->description)
    fprintf(fp, "Desc %s~\n", obj->description);
  if (obj->extra_flags != obj->pIndexData->extra_flags)
    fprintf(fp, "ExtF %d\n", obj->extra_flags);
  if (obj->wear_flags != obj->pIndexData->wear_flags)
    fprintf(fp, "WeaF %d\n", obj->wear_flags);
  if (obj->item_type != obj->pIndexData->item_type)
    fprintf(fp, "Ityp %d\n", obj->item_type);
  if (obj->weight != obj->pIndexData->weight)
    fprintf(fp, "Wt   %d\n", obj->weight);
  if (obj->condition != obj->pIndexData->condition)
    fprintf(fp, "Cond %d\n", obj->condition);

  /* variable data */

  fprintf(fp, "Wear -1\n");
  if (obj->level != obj->pIndexData->level)
    fprintf(fp, "Lev  %d\n", obj->level);
  if (obj->timer > 0)
    fprintf(fp, "Time %d\n", obj->timer);
  if (obj->cost > 0)
    fprintf(fp, "Cost %d\n", obj->cost);
  if (obj->value[0] != obj->pIndexData->value[0] ||
      obj->value[1] != obj->pIndexData->value[1] ||
      obj->value[2] != obj->pIndexData->value[2] ||
      obj->value[3] != obj->pIndexData->value[3] ||
      obj->value[4] != obj->pIndexData->value[4])
    fprintf(fp, "Val  %ld %ld %ld %ld %ld\n", obj->value[0],
            obj->value[1], obj->value[2], obj->value[3], obj->value[4]);

  switch (obj->item_type)
  {
    case ITEM_POTION:
    case ITEM_SCROLL:
      if (obj->value[1] > 0)
      {
        fprintf(fp, "Spell 1 '%s'\n", skill_table[obj->value[1]].name);
      }

      if (obj->value[2] > 0)
      {
        fprintf(fp, "Spell 2 '%s'\n", skill_table[obj->value[2]].name);
      }

      if (obj->value[3] > 0)
      {
        fprintf(fp, "Spell 3 '%s'\n", skill_table[obj->value[3]].name);
      }

      break;

    case ITEM_PILL:
    case ITEM_STAFF:
    case ITEM_WAND:
    case ITEM_INSTRUMENT:
      if (obj->value[3] > 0)
      {
        fprintf(fp, "Spell 3 '%s'\n", skill_table[obj->value[3]].name);
      }

      break;
  }

  for (paf = obj->affected; paf != NULL; paf = paf->next)
  {
    if (paf->type < 0 || paf->type >= MAX_SKILL)
      continue;
    fprintf(fp, "Affc '%s' %3d %3d %3d %3d %3d %10ld\n",
            skill_table[paf->type].name, paf->where, paf->level,
            paf->duration, paf->modifier, paf->location, paf->bitvector);
  }

  for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
  {
    fprintf(fp, "ExDe %s~ %s~\n", ed->keyword, ed->description);
  }

  fprintf(fp, "End\n\n");

  if (obj->contains != NULL)
    fwrite_bank(ch, obj->contains, fp, iNest + 1);

  return;
}

void fread_bank(CHAR_DATA * ch, FILE * fp)
{
  OBJ_DATA *obj;
  char *word;
  int iNest;
  bool fMatch;
  bool fNest;
  bool fVnum;
  bool first;

  fVnum = false;
  obj = NULL;
  first = true;                 /* used to counter fp offset */

  word = feof(fp) ? "End" : fread_word(fp);
  if (!str_cmp(word, "Vnum"))
  {
    long vnum;

    first = false;              /* fp will be in right place */

    vnum = fread_long(fp);
    if (get_obj_index(vnum) == NULL)
    {
      bug("Fread_bank: bad vnum %ld.", vnum);
      obj = create_object(get_obj_index(OBJ_VNUM_BAG));
    }
    else
    {
      obj = create_object(get_obj_index(vnum));
    }

  }

  if (obj == NULL)              /* either not found or old style */
  {
    obj = new_obj();
    obj->name = str_dup("");
    obj->short_descr = str_dup("");
    obj->description = str_dup("");
  }

  fNest = false;
  fVnum = true;
  iNest = 0;

  for (;;)
  {
    if (first)
      first = false;
    else
      word = feof(fp) ? "End" : fread_word(fp);
    fMatch = false;

    switch (UPPER(word[0]))
    {
      case '*':
        fMatch = true;
        fread_to_eol(fp);
        break;

      case 'A':
        if (!str_cmp(word, "AffD"))
        {
          AFFECT_DATA *paf;
          int sn;

          paf = new_affect();

          sn = skill_lookup(fread_word(fp));
          if (sn < 0)
            bug("Fread_bank: unknown skill.", 0);
          else
            paf->type = sn;

          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_long(fp);
          paf->next = obj->affected;
          obj->affected = paf;
          fMatch = true;
          break;
        }
        if (!str_cmp(word, "Affc"))
        {
          AFFECT_DATA *paf;
          int sn;


          paf = new_affect();

          sn = skill_lookup(fread_word(fp));
          if (sn < 0)
            bug("Fread_bank: unknown skill.", 0);
          else
            paf->type = sn;

          paf->where = fread_number(fp);
          paf->level = fread_number(fp);
          paf->duration = fread_number(fp);
          paf->modifier = fread_number(fp);
          paf->location = fread_number(fp);
          paf->bitvector = fread_long(fp);
          paf->next = obj->affected;
          obj->affected = paf;
          fMatch = true;
          break;
        }
        break;

      case 'C':
        KEY("Cond", obj->condition, fread_number(fp));
        KEY("Cost", obj->cost, fread_number(fp));
        break;

      case 'D':
        KEYS("Description", obj->description, fread_string(fp));
        KEYS("Desc", obj->description, fread_string(fp));
        break;

      case 'E':

        if (!str_cmp(word, "Enchanted"))
        {
          obj->enchanted = true;
          fMatch = true;
          break;
        }

        KEY("ExtraFlags", obj->extra_flags, fread_number(fp));
        KEY("ExtF", obj->extra_flags, fread_number(fp));

        if (!str_cmp(word, "ExtraDescr") || !str_cmp(word, "ExDe"))
        {
          EXTRA_DESCR_DATA *ed;

          ed = new_extra_descr();

          ed->keyword = fread_string(fp);
          ed->description = fread_string(fp);
          ed->next = obj->extra_descr;
          obj->extra_descr = ed;
          fMatch = true;
        }

        if (!str_cmp(word, "End"))
        {
          if (!fNest || (fVnum && obj->pIndexData == NULL))
          {
            bug("Fread_bank: incomplete object.", 0);
            free_obj(obj);
            return;
          }
          else
          {
            if (!fVnum)
            {
              free_obj(obj);
              obj = create_object(get_obj_index(OBJ_VNUM_DUMMY));
            }

            if (iNest == 0 || rgObjNest[iNest] == NULL)
            {
              obj->next_content = ch->bankeditems;
              ch->bankeditems = obj;
            }
            else
              obj_to_obj(obj, rgObjNest[iNest - 1]);
            return;
          }
        }
        break;

      case 'I':
        KEY("ItemType", obj->item_type, fread_number(fp));
        KEY("Ityp", obj->item_type, fread_number(fp));
        break;

      case 'L':
        KEY("Level", obj->level, fread_number(fp));
        KEY("Lev", obj->level, fread_number(fp));
        break;

      case 'N':
        KEYS("Name", obj->name, fread_string(fp));

        if (!str_cmp(word, "Nest"))
        {
          iNest = fread_number(fp);
          if (iNest < 0 || iNest >= MAX_NEST)
          {
            bug("Fread_bank: bad nest %d.", iNest);
          }
          else
          {
            rgObjNest[iNest] = obj;
            fNest = true;
          }
          fMatch = true;
        }
        break;

      case 'O':
        break;

      case 'S':
        KEYS("ShortDescr", obj->short_descr, fread_string(fp));
        KEYS("ShD", obj->short_descr, fread_string(fp));

        if (!str_cmp(word, "Spell"))
        {
          int iValue;
          int sn;

          iValue = fread_number(fp);
          sn = skill_lookup(fread_word(fp));
          if (iValue < 0 || iValue > 3)
          {
            bug("Fread_bank: bad iValue %d.", iValue);
          }
          else if (sn < 0)
          {
            bug("Fread_bank: unknown skill.", 0);
          }
          else
          {
            obj->value[iValue] = sn;
          }
          fMatch = true;
          break;
        }

        break;

      case 'T':
        KEY("Timer", obj->timer, fread_number(fp));
        KEY("Time", obj->timer, fread_number(fp));
        break;

      case 'V':
        if (!str_cmp(word, "Values") || !str_cmp(word, "Vals"))
        {
          obj->value[0] = fread_long(fp);
          obj->value[1] = fread_long(fp);
          obj->value[2] = fread_long(fp);
          obj->value[3] = fread_long(fp);
          if (obj->item_type == ITEM_WEAPON && obj->value[0] == 0)
            obj->value[0] = obj->pIndexData->value[0];
          fMatch = true;
          break;
        }

        if (!str_cmp(word, "Val"))
        {
          obj->value[0] = fread_long(fp);
          obj->value[1] = fread_long(fp);
          obj->value[2] = fread_long(fp);
          obj->value[3] = fread_long(fp);
          obj->value[4] = fread_long(fp);

          fMatch = true;
          break;
        }

        if (!str_cmp(word, "Vnum"))
        {
          long vnum;

          vnum = fread_long(fp);
          if ((obj->pIndexData = get_obj_index(vnum)) == NULL)
            bug("Fread_bank: bad vnum %ld.", vnum);
          else
            fVnum = true;
          fMatch = true;
          break;
        }
        break;

      case 'W':
        KEY("WearFlags", obj->wear_flags, fread_number(fp));
        KEY("WeaF", obj->wear_flags, fread_number(fp));
        KEY("WearLoc", obj->wear_loc, fread_number(fp));
        KEY("Wear", obj->wear_loc, fread_number(fp));
        KEY("Weight", obj->weight, fread_number(fp));
        KEY("Wt", obj->weight, fread_number(fp));
        break;

    }

    if (!fMatch)
    {
      bug("Fread_bank: no match.", 0);
      fread_to_eol(fp);
    }
  }
  if (obj->pIndexData->vnum == OBJ_VNUM_QDIAMOND)
  {
    ch->platinum++;
    extract_obj(obj);
  }
}
