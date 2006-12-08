
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
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <crypt.h>
#include <sqlite3.h>

#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "db.h"
#include "sql_io.h"

char *const where_name[] = {
  "{R<{yused as light       {R:{x",
  "{R<{yworn on finger      {R:{x",
  "{R<{yworn on finger      {R:{x",
  "{R<{yworn around neck    {R:{x",
  "{R<{yworn around neck    {R:{x",
  "{R<{yworn on torso       {R:{x",
  "{R<{yworn on head        {R:{x",
  "{R<{yworn on legs        {R:{x",
  "{R<{yworn on feet        {R:{x",
  "{R<{yworn on hands       {R:{x",
  "{R<{yworn on arms        {R:{x",
  "{R<{yworn as shield      {R:{x",
  "{R<{yworn about body     {R:{x",
  "{R<{yworn about waist    {R:{x",
  "{R<{yworn around wrist   {R:{x",
  "{R<{yworn around wrist   {R:{x",
  "{R<{yprimary wield       {R:{x",
  "{R<{yheld                {R:{x",
  "{R<{yfloating nearby     {R:{x",
  "{R<{ysecondary wield     {R:{x",
  "{R<{yworn on face        {R:{x",
  "{R<{yworn on the ankle   {R:{x",
  "{R<{yworn on the ankle   {R:{x",
  "{R<{yworn as tattoo      {R:{x",
  "{R<{yworn as aura        {R:{x",
  "{R<{yworn on left ear    {R:{x",
  "{R<{yworn on right ear   {R:{x",
  "{R<{yworn as clan tattoo {R:{x",
  "{R<{yworn on back        {R:{x"
};

sh_int const where_order[] = {
  /*   1,  2,  3,  4,  5, 25, 26,
     6, 20,  7,  8,  9, 21, 22,
     10, 11, 23, 12, 13, 14,
     15, 16, 19, 17, 18, 24,
     0 */
  6, 25, 26, 20, 3, 4, 10, 14, 15, 9, 1, 2, 16, 19, 11, 17, 0, 5, 28, 12, 13,
  7, 21, 22, 8, 18, 24, 23, 27
};

/* for do_count */
bool is_pm = false;

char *format_obj_to_char(OBJ_DATA * obj, CHAR_DATA * ch, bool fShort)
{
  static char buf[MSL];

  buf[0] = '\0';

  if ((fShort &&
       (obj->short_descr == NULL || obj->short_descr[0] == '\0')) ||
      (obj->description == NULL || obj->description[0] == '\0'))
    return buf;

  if (!IS_SET(ch->comm, COMM_LONG))
  {
    strcat(buf, "{x[{y.{R.{B.{M.{Y.{W.{G.{x]");
    if (IS_OBJ_STAT(obj, ITEM_INVIS))
      buf[5] = 'V';
    if (IS_AFFECTED(ch, AFF_DETECT_EVIL) && IS_OBJ_STAT(obj, ITEM_EVIL))
      buf[8] = 'E';
    if (IS_AFFECTED(ch, AFF_DETECT_GOOD) && IS_OBJ_STAT(obj, ITEM_BLESS))
      buf[11] = 'B';
    if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT(obj, ITEM_MAGIC))
      buf[14] = 'M';
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
      buf[17] = 'G';
    if (IS_OBJ_STAT(obj, ITEM_HUM))
      buf[20] = 'H';
    if (IS_OBJ_STAT(obj, ITEM_QUEST))
      buf[23] = 'Q';
    if (!str_cmp(buf, "{x[{y.{R.{B.{M.{Y.{W.{G.{x]"))
      buf[0] = '\0';
  }
  else
  {
    if (IS_OBJ_STAT(obj, ITEM_INVIS))
      strcat(buf, "({yInvis{x)");
    if (IS_OBJ_STAT(obj, ITEM_DARK))
      strcat(buf, "({DHidden{x)");
    if (IS_AFFECTED(ch, AFF_DETECT_EVIL) && IS_OBJ_STAT(obj, ITEM_EVIL))
      strcat(buf, "({RRed Aura{x)");
    if (IS_AFFECTED(ch, AFF_DETECT_GOOD) && IS_OBJ_STAT(obj, ITEM_BLESS))
      strcat(buf, "({BBlue Aura{x)");
    if (IS_AFFECTED(ch, AFF_DETECT_MAGIC) && IS_OBJ_STAT(obj, ITEM_MAGIC))
      strcat(buf, "({yMagical{x)");
    if (IS_OBJ_STAT(obj, ITEM_GLOW))
      strcat(buf, "({YGlowing{x)");
    if (IS_OBJ_STAT(obj, ITEM_HUM))
      strcat(buf, "({yHumming{x)");
    if (IS_OBJ_STAT(obj, ITEM_QUEST))
      strcat(buf, "({GQuest{x)");
    if (!IS_NPC(ch) && (ch->pcdata->questobj > 0) &&
        (obj->pIndexData->vnum == ch->pcdata->questobj))
      strcat(buf, "{W[{RT{rARGE{RT{W]{x");
  }

  if (buf[0] != '\0')
  {
    strcat(buf, " ");
  }

  if (fShort)
  {
    if (obj->short_descr != NULL)
      strcat(buf, obj->short_descr);
  }
  else
  {
    if (obj->description != NULL)
      strcat(buf, obj->description);
  }
  if (strlen(buf) <= 0)
    strcat(buf, "This object has no description. Please inform an IMM.");

  return buf;
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
BUFFER *show_list_to_char(OBJ_DATA * list, CHAR_DATA * ch, bool fShort,
                          bool fShowNothing)
{
  char buf[MSL];
  BUFFER *output;
  char **prgpstrShow;
  int *prgnShow;
  char *pstrShow;
  OBJ_DATA *obj;
  int nShow;
  int iShow;
  int count;
  bool fCombine;

  /* 
   * Alloc space for output lines.
   */
  output = new_buf();
  count = 0;
  for (obj = list; obj != NULL; obj = obj->next_content)
    count++;
  prgpstrShow = alloc_mem(count * sizeof(char *));
  prgnShow = alloc_mem(count * sizeof(int));
  nShow = 0;

  /* 
   * Format the list of objects.
   */
  for (obj = list; obj != NULL; obj = obj->next_content)
  {
    if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj))
    {
      pstrShow = format_obj_to_char(obj, ch, fShort);

      fCombine = false;

      if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
      {
        /* 
         * Look for duplicates, case sensitive.
         * Matches tend to be near end so run loop backwords.
         */
        for (iShow = nShow - 1; iShow >= 0; iShow--)
        {
          if (!str_cmp(prgpstrShow[iShow], pstrShow))
          {
            prgnShow[iShow]++;
            fCombine = true;
            break;
          }
        }
      }

      /* 
       * Couldn't combine, or didn't want to.
       */
      if (!fCombine)
      {
        prgpstrShow[nShow] = str_dup(pstrShow);
        prgnShow[nShow] = 1;
        nShow++;
      }
    }
  }

  /* 
   * Output the formatted list.
   */
  for (iShow = 0; iShow < nShow; iShow++)
  {
    if (prgpstrShow[iShow][0] == '\0')
    {
      free_string(prgpstrShow[iShow]);
      continue;
    }

    if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
    {
      if (prgnShow[iShow] != 1)
      {
        sprintf(buf, "(%2d) ", prgnShow[iShow]);
        add_buf(output, buf);
      }
      else
      {
        add_buf(output, "     ");
      }
    }
    add_buf(output, prgpstrShow[iShow]);
    add_buf(output, "\n\r");
    free_string(prgpstrShow[iShow]);
  }

  if (fShowNothing && nShow == 0)
  {
    if (IS_NPC(ch) || IS_SET(ch->comm, COMM_COMBINE))
      send_to_char("     ", ch);
    send_to_char("Nothing.\n\r", ch);
  }
  /* 
   * Clean up.
   */
  free_mem(prgpstrShow, count * sizeof(char *));
  free_mem(prgnShow, count * sizeof(int));

  return output;
}

void show_char_to_char_0(CHAR_DATA * victim, CHAR_DATA * ch)
{
  char buf[MSL], message[MSL];
  int vict_condition;

  buf[0] = '\0';

  if (victim->max_hit > 0)
    vict_condition = victim->hit * 100 / victim->max_hit;
  else
    vict_condition = -1;

  if (!IS_SET(ch->comm, COMM_LONG))
  {
    strcat(buf, "{x[{y.{D.{c.{b.{w.{C.{r.{B.{g.{m.{R.{Y.{W.{G.{x]");
    if (IS_SHIELDED(victim, SHD_INVISIBLE))
      buf[5] = 'V';
    if (IS_AFFECTED(victim, AFF_HIDE) || IS_AFFECTED(victim, AFF_SNEAK))
      buf[8] = 'H';
    if (IS_AFFECTED(victim, AFF_CHARM))
      buf[11] = 'C';
    if (IS_AFFECTED(victim, AFF_PASS_DOOR))
      buf[14] = 'T';
    if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))
      buf[17] = 'P';
    if (IS_SHIELDED(victim, SHD_ICE))
      buf[20] = 'I';
    if (IS_SHIELDED(victim, SHD_FIRE))
      buf[23] = 'F';
    if (IS_SHIELDED(victim, SHD_SHOCK))
      buf[26] = 'L';
    if (IS_SHIELDED(victim, SHD_ACID))
      buf[29] = 'A';
    if (IS_SHIELDED(victim, SHD_POISON))
      buf[32] = 'P';
    if (IS_EVIL(victim) && IS_AFFECTED(ch, AFF_DETECT_EVIL))
      buf[35] = 'E';
    if (IS_GOOD(victim) && IS_AFFECTED(ch, AFF_DETECT_GOOD))
      buf[38] = 'G';
    if (IS_SHIELDED(victim, SHD_SANCTUARY))
      buf[41] = 'S';
    if (victim->on_quest)
      buf[44] = 'Q';
    if (!str_cmp(buf, "{x[{y.{D.{c.{b.{w.{C.{r.{B.{R.{Y.{W.{G.{x]"))
      buf[0] = '\0';
    if (IS_SET(victim->comm, COMM_AFK))
      strcat(buf, " [{yAFK{x]");
    if (IS_SET(victim->act2, PLR2_BUSY))
      strcat(buf, " [{yBUSY{x]");
    if (victim->timer > 3)
      strcat(buf, " [{WIDLE{x]");

    if (WR(ch, victim) && !IS_NPC(victim) && !IS_IMMORTAL(ch) &&
        !IS_IMMORTAL(victim))
    {
      strcat(buf, "{D[{GWR{D]{x");
    }

    if (!IS_NPC(victim) && victim->pk_timer > 0 && is_pkill(victim)
        && is_pkill(ch) && !IS_IMMORTAL(victim))
    {
      strcat(buf, "{D[{WPK TIMER{D]{x");
    }

    if (IS_SET(victim->act2, PLR_RP))
      strcat(buf, "[{RRP{x]");
    if (victim->invis_level >= LEVEL_ANCIENT)
      strcat(buf, "({WWizi{x)");
    if (vict_condition < 33)
      strcat(buf, "{R(Wounded){x ");
    if (!IS_NPC(victim) && !victim->desc)
      strcat(buf, "{G({cLinkdead{G){x");
    if (!IS_NPC(ch) && IS_NPC(victim) && (ch->pcdata->questmob > 0)
        && (victim->pIndexData->vnum == ch->pcdata->questmob))
      strcat(buf, "{W[{RT{rARGE{RT{x{W]{x");

  }
  else
  {
    if (IS_SET(victim->comm, COMM_AFK))
      strcat(buf, "[{yAFK{x]");
    if (IS_SHIELDED(victim, SHD_INVISIBLE))
      strcat(buf, "({yInvis{x)");
    if (victim->invis_level >= LEVEL_ANCIENT)
      strcat(buf, "({WWizi{x)");
    if (IS_AFFECTED(victim, AFF_HIDE))
      strcat(buf, "({DHide{x)");
    if (IS_AFFECTED(victim, AFF_CHARM))
      strcat(buf, "({cCharmed{x)");
    if (IS_AFFECTED(victim, AFF_PASS_DOOR))
      strcat(buf, "({bTranslucent{x)");
    if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))
      strcat(buf, "({wPink Aura{x)");
    if (IS_SHIELDED(victim, SHD_ICE))
      strcat(buf, "({CI{DS{x)");
    if (IS_SHIELDED(victim, SHD_FIRE))
      strcat(buf, "({rF{DS{x)");
    if (IS_SHIELDED(victim, SHD_SHOCK))
      strcat(buf, "({BS{DS{x)");
    if (IS_SHIELDED(victim, SHD_ACID))
      strcat(buf, "({GA{DS{x)");
    if (IS_SHIELDED(victim, SHD_POISON))
      strcat(buf, "({MP{DS{x)");
    if (IS_SHIELDED(victim, SHD_BRIAR))
      strcat(buf, "({gT{DS{x)");
    if (IS_EVIL(victim) && IS_AFFECTED(ch, AFF_DETECT_EVIL))
      strcat(buf, "({RRed Aura{x)");
    if (IS_GOOD(victim) && IS_AFFECTED(ch, AFF_DETECT_GOOD))
      strcat(buf, "({YGolden Aura{x)");
    if (IS_SHIELDED(victim, SHD_SANCTUARY))
      strcat(buf, "({WWhite Aura{x)");
    if (victim->on_quest)
      strcat(buf, "({GQuest{x)");
    if (vict_condition < 33)
      strcat(buf, "{R(Wounded){x ");
    if (!IS_NPC(victim) && !victim->desc)
      strcat(buf, "{G({cLinkdead{G){x");
    if (!IS_NPC(ch) && IS_NPC(victim) && (ch->pcdata->questmob > 0)
        && (victim->pIndexData->vnum == ch->pcdata->questmob))
      strcat(buf, "{W[{RT{rARGE{RT{x{W]{x");
    if (IS_SHIELDED(victim, SHD_WARD))
      strcat(buf, "({WWard{x)");
  }
  if (!IS_NPC(victim) && IS_SET(victim->act, PLR_TWIT))
    strcat(buf, "({rTWIT{x)");
  // make sure victim is an NPC - M.
  /*  if (!IS_NPC(ch) && is_gqmob(ch, victim->pIndexData->vnum) != -1)
     {
     strcat(buf, "(Gquest) ");
     }
   */
  if (buf[0] != '\0')
  {
    strcat(buf, " ");
  }
  if (victim->position == victim->start_pos && victim->long_descr[0] != '\0')
  {
    strcat(buf, victim->long_descr);
    send_to_char(buf, ch);
    return;
  }

  strcat(buf, PERS(victim, ch));
  if (!IS_NPC(victim) && !IS_SET(ch->comm, COMM_BRIEF) &&
      victim->position == POS_STANDING && ch->on == NULL)
  {
    if (!IS_AFFECTED(victim, AFF_ENTANGLE))
    {
      strcat(buf, victim->pcdata->title);
    }
    else
    {
      strcat(buf, ", entangled in a web of mana");
    }
  }
  switch (victim->position)
  {
    case POS_DEAD:
      strcat(buf, " is DEAD!!");
      break;
    case POS_MORTAL:
      strcat(buf, " is mortally wounded.");
      break;
    case POS_INCAP:
      strcat(buf, " is incapacitated.");
      break;
    case POS_STUNNED:
      strcat(buf, " is lying here stunned.");
      break;
    case POS_SLEEPING:
      if (victim->on != NULL)
      {
        if (IS_SET(victim->on->value[2], SLEEP_AT))
        {
          sprintf(message, " is sleeping at %s.", victim->on->short_descr);
          strcat(buf, message);
        }
        else if (IS_SET(victim->on->value[2], SLEEP_ON))
        {
          sprintf(message, " is sleeping on %s.", victim->on->short_descr);
          strcat(buf, message);
        }
        else
        {
          sprintf(message, " is sleeping in %s.", victim->on->short_descr);
          strcat(buf, message);
        }
      }
      else
        strcat(buf, " is sleeping here.");
      break;
    case POS_RESTING:
      if (victim->on != NULL)
      {
        if (IS_SET(victim->on->value[2], REST_AT))
        {
          sprintf(message, " is resting at %s.", victim->on->short_descr);
          strcat(buf, message);
        }
        else if (IS_SET(victim->on->value[2], REST_ON))
        {
          sprintf(message, " is resting on %s.", victim->on->short_descr);
          strcat(buf, message);
        }
        else
        {
          sprintf(message, " is resting in %s.", victim->on->short_descr);
          strcat(buf, message);
        }
      }
      else
        strcat(buf, " is resting here.");
      break;
    case POS_SITTING:
      if (victim->on != NULL)
      {
        if (IS_SET(victim->on->value[2], SIT_AT))
        {
          sprintf(message, " is sitting at %s.", victim->on->short_descr);
          strcat(buf, message);
        }
        else if (IS_SET(victim->on->value[2], SIT_ON))
        {
          sprintf(message, " is sitting on %s.", victim->on->short_descr);
          strcat(buf, message);
        }
        else
        {
          sprintf(message, " is sitting in %s.", victim->on->short_descr);
          strcat(buf, message);
        }
      }
      else
        strcat(buf, " is sitting here.");
      break;
    case POS_STANDING:
      if (victim->on != NULL)
      {
        if (IS_SET(victim->on->value[2], STAND_AT))
        {
          sprintf(message, " is standing at %s.", victim->on->short_descr);
          strcat(buf, message);
        }
        else if (IS_SET(victim->on->value[2], STAND_ON))
        {
          sprintf(message, " is standing on %s.", victim->on->short_descr);
          strcat(buf, message);
        }
        else
        {
          sprintf(message, " is standing in %s.", victim->on->short_descr);
          strcat(buf, message);
        }
      }
      else
        strcat(buf, " is here.");
      break;
    case POS_FIGHTING:
      strcat(buf, " is here, fighting ");
      if (victim->fighting == NULL)
        strcat(buf, "thin air??");
      else if (victim->fighting == ch)
        strcat(buf, "YOU!");
      else if (victim->in_room == victim->fighting->in_room)
      {
        strcat(buf, PERS(victim->fighting, ch));
        strcat(buf, ".");
      }
      else
        strcat(buf, "someone who left??");
      break;
  }

  strcat(buf, "\n\r");
  buf[0] = UPPER(buf[0]);
  send_to_char(buf, ch);
  return;
}

void show_char_to_char_1(CHAR_DATA * victim, CHAR_DATA * ch)
{
  char buf[MSL];
  OBJ_DATA *obj;
  BUFFER *output;
  BUFFER *outlist;
  int iWear;
  int oWear;
  int percent;
  bool found;

  if (can_see(victim, ch) && get_trust(victim) >= ch->ghost_level)
  {
    if (ch == victim)
      act("$n looks at $mself.", ch, NULL, NULL, TO_ROOM);
    else
    {
      act("$n looks at you.", ch, NULL, victim, TO_VICT);
      act("$n looks at $N.", ch, NULL, victim, TO_NOTVICT);
    }
  }

  output = new_buf();
  if (victim->description[0] != '\0')
  {
    sprintf(buf, "{C%s{x", victim->description);
  }
  else
  {
    sprintf(buf, "{CYou see nothing special about %s{x\n\r", victim->name);
  }

  add_buf(output, buf);

  if (victim->max_hit > 0)
    percent = (100 * victim->hit) / victim->max_hit;
  else
    percent = -1;

  buf[0] = '\0';
  strcpy(buf, PERS(victim, ch));

  if (percent >= 100)
    strcat(buf, " {fis in excellent condition.{x\n\r");
  else if (percent >= 90)
    strcat(buf, " {fhas a few scratches.{x\n\r");
  else if (percent >= 75)
    strcat(buf, " {fhas some small wounds and bruises.{x\n\r");
  else if (percent >= 50)
    strcat(buf, " {fhas quite a few wounds.{x\n\r");
  else if (percent >= 30)
    strcat(buf, " {fhas some big nasty wounds and scratches.{x\n\r");
  else if (percent >= 15)
    strcat(buf, " {flooks pretty hurt.{x\n\r");
  else if (percent >= 0)
    strcat(buf, " {fis in awful condition.{x\n\r");
  else
    strcat(buf, " {fis bleeding to death.{x\n\r");

  buf[0] = UPPER(buf[0]);
  add_buf(output, buf);

  if (IS_SHIELDED(victim, SHD_ICE))
  {
    sprintf(buf, "%s is surrounded by an {Cicy{x shield.\n\r",
            PERS(victim, ch));
    buf[0] = UPPER(buf[0]);
    add_buf(output, buf);
  }
  if (IS_SHIELDED(victim, SHD_FIRE))
  {
    sprintf(buf, "%s is surrounded by a {Rfiery{x shield.\n\r",
            PERS(victim, ch));
    buf[0] = UPPER(buf[0]);
    add_buf(output, buf);
  }
  if (IS_SHIELDED(victim, SHD_SHOCK))
  {
    sprintf(buf, "%s is surrounded by a {Bcrackling{x shield.\n\r",
            PERS(victim, ch));
    buf[0] = UPPER(buf[0]);
    add_buf(output, buf);
  }
  if (IS_SHIELDED(victim, SHD_ACID))
  {
    sprintf(buf, "%s is surrounded by a {GA{gcidic{x shield.\n\r",
            PERS(victim, ch));
    buf[0] = UPPER(buf[0]);
    add_buf(output, buf);
  }
  if (IS_SHIELDED(victim, SHD_POISON))
  {
    sprintf(buf, "%s is surrounded by a {MP{moisonous{x shield.\n\r",
            PERS(victim, ch));
    buf[0] = UPPER(buf[0]);
    add_buf(output, buf);
  }
  if (IS_SHIELDED(victim, SHD_BRIAR))
  {
    sprintf(buf, "%s is surrounded by a {gt{yh{go{yr{gn{yy shield.\n\r",
            PERS(victim, ch));
    buf[0] = UPPER(buf[0]);
    add_buf(output, buf);
  }

  found = false;
  for (oWear = 0; oWear < MAX_WEAR; oWear++)
  {
    iWear = where_order[oWear];
    if ((obj = get_eq_char(victim, iWear)) != NULL && can_see_obj(ch, obj))
    {
      if (!found)
      {
        sprintf(buf, "\n\r");
        add_buf(output, buf);
        sprintf(buf, "{G%s is using:{x\n\r", victim->name);
        add_buf(output, buf);
        found = true;
      }
      sprintf(buf, "%s\n\r", format_obj_to_char(obj, ch, true));

      //            sprintf ( buf, "%s%s\n\r", where_name[iWear],
      //                      format_obj_to_char ( obj, ch, true ) );
      add_buf(output, buf);
    }
  }

  if (victim != ch && !IS_NPC(ch) &&
      number_percent() < get_skill(ch, gsn_peek) &&
      IS_SET(ch->act, PLR_AUTOPEEK))
  {
    sprintf(buf, "\n\r{GYou peek at the inventory:{x\n\r");
    add_buf(output, buf);
    check_improve(ch, gsn_peek, true, 4);

    outlist = show_list_to_char(victim->carrying, ch, true, true);
    add_buf(output, buf_string(outlist));
    free_buf(outlist);
  }
  page_to_char(buf_string(output), ch);
  free_buf(output);
  return;
}

void show_char_to_char(CHAR_DATA * list, CHAR_DATA * ch)
{
  CHAR_DATA *rch;

  for (rch = list; rch != NULL; rch = rch->next_in_room)
  {
    if (rch == ch)
      continue;

    if (get_trust(ch) < rch->invis_level)
      continue;

    if (get_trust(ch) < rch->ghost_level)
      continue;

    if (can_see(ch, rch))
    {
      show_char_to_char_0(rch, ch);
    }
    else if (room_is_dark(ch->in_room) && IS_AFFECTED(rch, AFF_INFRARED))
    {
      send_to_char("You see {Rglowing red{x eyes watching YOU!\n\r", ch);
    }
  }

  return;
}

CH_CMD(do_peek)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[MSL];
  BUFFER *output;
  BUFFER *outlist;
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

  if (arg[0] == '\0')
  {
    send_to_char("Peek at who?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    do_inventory(ch, "");
    return;
  }

  if (can_see(victim, ch) && get_trust(victim) >= ch->ghost_level)
  {
    act("$n peers intently at you.", ch, NULL, victim, TO_VICT);
    act("$n peers intently at $N.", ch, NULL, victim, TO_NOTVICT);
  }

  output = new_buf();

  if (number_percent() < get_skill(ch, gsn_peek))
  {
    sprintf(buf, "\n\r{GYou peek at the inventory:{x\n\r");
    add_buf(output, buf);
    check_improve(ch, gsn_peek, true, 4);

    outlist = show_list_to_char(victim->carrying, ch, true, true);
    add_buf(output, buf_string(outlist));
    free_buf(outlist);
  }
  else
  {
    sprintf(buf, "{RYou fail to see anything.{x\n\r");
    add_buf(output, buf);
    check_improve(ch, gsn_peek, false, 2);
  }
  page_to_char(buf_string(output), ch);
  free_buf(output);
  return;
}

bool check_blind(CHAR_DATA * ch)
{

  if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
    return true;

  if (IS_AFFECTED(ch, AFF_BLIND))
  {
    send_to_char("You can't see a thing!\n\r", ch);
    return false;
  }

  return true;
}

/* changes your scroll */
CH_CMD(do_scroll)
{
  char arg[MAX_INPUT_LENGTH];
  char buf[100];
  int lines;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    if (ch->lines == 0)
      send_to_char("You do not page long messages.\n\r", ch);
    else
    {
      sprintf(buf, "You currently display %d lines per page.\n\r",
              ch->lines + 2);
      send_to_char(buf, ch);
    }
    return;
  }

  if (!is_number(arg))
  {
    send_to_char("You must provide a number. Use 0 to disable.\n\r", ch);
    return;
  }

  lines = atoi(arg);

  /*    if ( lines == 0 )
     {
     send_to_char ( "Paging disabled.\n\r", ch );
     ch->lines = 0;
     return;
     } */

  if (lines < 10 || lines > 50)
  {
    send_to_char("You must provide a number between 10 and 50.\n\r", ch);
    return;
  }

  sprintf(buf, "Scroll set to %d lines.\n\r", lines);
  send_to_char(buf, ch);
  ch->lines = lines - 2;
}

/* RT does socials */
CH_CMD(do_socials)
{
  char buf[MSL];
  int iSocial;
  int col;

  col = 0;

  for (iSocial = 0; social_table[iSocial].name[0] != '\0'; iSocial++)
  {
    sprintf(buf, "%-12s", social_table[iSocial].name);
    send_to_char(buf, ch);
    if (++col % 6 == 0)
      send_to_char("\n\r", ch);
  }

  if (col % 6 != 0)
    send_to_char("\n\r", ch);
  return;
}

/* RT Commands to replace news, motd, imotd, etc from ROM */

CH_CMD(do_motd)
{
  do_help(ch, "motd");
}

CH_CMD(do_imotd)
{
  do_help(ch, "imotd");
}

CH_CMD(do_rules)
{
  do_help(ch, "rules");
}

CH_CMD(do_story)
{
  do_help(ch, "story");
}

/* RT this following section holds all the auto commands from ROM, as well as
   replacements for config */

CH_CMD(do_autolist)
{
  /* lists most player flags */
  if (IS_NPC(ch))
    return;

  send_to_char("   {cOption     {YS{Dtatus\n\r", ch);
  send_to_char("{B=={b--{B=={b--{B=={b--{B=={b--{B=={b--{B=={x\n\r", ch);

  send_to_char("{cautoassist{x:     ", ch);
  if (IS_SET(ch->act, PLR_AUTOASSIST))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{cautoexit{x:       ", ch);
  if (IS_SET(ch->act, PLR_AUTOEXIT))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{cautogold{x:       ", ch);
  if (IS_SET(ch->act, PLR_AUTOGOLD))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{cautoloot{x:       ", ch);
  if (IS_SET(ch->act, PLR_AUTOLOOT))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{cautosac{x:        ", ch);
  if (IS_SET(ch->act, PLR_AUTOSAC))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{cautosplit{x:      ", ch);
  if (IS_SET(ch->act, PLR_AUTOSPLIT))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{cautopeek{x:       ", ch);
  if (IS_SET(ch->act, PLR_AUTOPEEK))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{cautostore{x:      ", ch);
  if (IS_SET(ch->comm, COMM_STORE))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{ccompact mode{x:   ", ch);
  if (IS_SET(ch->comm, COMM_COMPACT))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{cprompt{x:         ", ch);
  if (IS_SET(ch->comm, COMM_PROMPT))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{ccombine items{x:  ", ch);
  if (IS_SET(ch->comm, COMM_COMBINE))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  send_to_char("{clong flags{x:     ", ch);
  if (IS_SET(ch->comm, COMM_LONG))
    send_to_char("{YON{x\n\r", ch);
  else
    send_to_char("{DOFF{x\n\r", ch);

  if (!IS_SET(ch->act, PLR_CANLOOT))
    send_to_char("{cCorpse{x:{Y         No Loot{x\n\r", ch);
  else
    send_to_char("{cCorpse{x: {D        Loot{x\n\r", ch);

  if (IS_SET(ch->act, PLR_NOSUMMON))
    send_to_char("{cSummonable{x:  {D   No{x\n\r", ch);
  else
    send_to_char("{cSummonable{x:  {Y   Yes\n\r", ch);

  if (IS_SET(ch->act, PLR_NOFOLLOW))
    send_to_char("{cFollowers{x: {D     No\n\r", ch);
  else
    send_to_char("{cFollowers{x: {Y     Yes{x\n\r", ch);
}

CH_CMD(do_autoassist)
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_AUTOASSIST))
  {
    send_to_char("Autoassist removed.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_AUTOASSIST);
  }
  else
  {
    send_to_char("You will now assist when needed.\n\r", ch);
    SET_BIT(ch->act, PLR_AUTOASSIST);
  }
}

CH_CMD(do_autoexit)
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_AUTOEXIT))
  {
    send_to_char("Exits will no longer be displayed.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_AUTOEXIT);
  }
  else
  {
    send_to_char("Exits will now be displayed.\n\r", ch);
    SET_BIT(ch->act, PLR_AUTOEXIT);
  }
}

CH_CMD(do_autogold)
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_AUTOGOLD))
  {
    send_to_char("Autogold removed.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_AUTOGOLD);
  }
  else
  {
    send_to_char("Automatic gold looting set.\n\r", ch);
    SET_BIT(ch->act, PLR_AUTOGOLD);
  }
}

CH_CMD(do_autoloot)
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_AUTOLOOT))
  {
    send_to_char("Autolooting removed.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_AUTOLOOT);
  }
  else
  {
    send_to_char("Automatic corpse looting set.\n\r", ch);
    SET_BIT(ch->act, PLR_AUTOLOOT);
  }
}

CH_CMD(do_autosac)
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_AUTOSAC))
  {
    send_to_char("Autosacrificing removed.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_AUTOSAC);
  }
  else
  {
    send_to_char("Automatic corpse sacrificing set.\n\r", ch);
    SET_BIT(ch->act, PLR_AUTOSAC);
  }
}

CH_CMD(do_autosplit)
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_AUTOSPLIT))
  {
    send_to_char("Autosplitting removed.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_AUTOSPLIT);
  }
  else
  {
    send_to_char("Automatic gold splitting set.\n\r", ch);
    SET_BIT(ch->act, PLR_AUTOSPLIT);
  }
}

CH_CMD(do_autopeek)
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_AUTOPEEK))
  {
    send_to_char("Autopeek removed.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_AUTOPEEK);
  }
  else
  {
    send_to_char("Automatic peek set.\n\r", ch);
    SET_BIT(ch->act, PLR_AUTOPEEK);
  }
}

CH_CMD(do_brief)
{
  if (IS_SET(ch->comm, COMM_BRIEF))
  {
    send_to_char("Full descriptions activated.\n\r", ch);
    REMOVE_BIT(ch->comm, COMM_BRIEF);
  }
  else
  {
    send_to_char("Short descriptions activated.\n\r", ch);
    SET_BIT(ch->comm, COMM_BRIEF);
  }
}

CH_CMD(do_compact)
{
  if (IS_SET(ch->comm, COMM_COMPACT))
  {
    send_to_char("Compact mode removed.\n\r", ch);
    REMOVE_BIT(ch->comm, COMM_COMPACT);
  }
  else
  {
    send_to_char("Compact mode set.\n\r", ch);
    SET_BIT(ch->comm, COMM_COMPACT);
  }
}

CH_CMD(do_long)
{
  if (!IS_SET(ch->comm, COMM_LONG))
  {
    send_to_char("Long flags activated.\n\r", ch);
    SET_BIT(ch->comm, COMM_LONG);
  }
  else
  {
    send_to_char("Short flags activated.\n\r", ch);
    REMOVE_BIT(ch->comm, COMM_LONG);
  }
}

CH_CMD(do_show)
{
  if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
  {
    send_to_char("Affects will no longer be shown in score.\n\r", ch);
    REMOVE_BIT(ch->comm, COMM_SHOW_AFFECTS);
  }
  else
  {
    send_to_char("Affects will now be shown in score.\n\r", ch);
    SET_BIT(ch->comm, COMM_SHOW_AFFECTS);
  }
}

CH_CMD(do_prompt)
{
  char buf[MSL];

  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_PROMPT))
    {
      send_to_char("You will no longer see prompts.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_PROMPT);
    }
    else
    {
      send_to_char("You will now see prompts.\n\r", ch);
      SET_BIT(ch->comm, COMM_PROMPT);
    }
    return;
  }

  if (!str_cmp(argument, "all"))
    strcpy(buf,
           "{r%h{w-{R%H{whp {b%m{w-{B%M{wm {g%v{w-{G%V{wmv {C%X{wtnl {Y%b{wtb {M%q{wqt{W>{x%c");
  else if (!str_cmp(argument, "all2"))
    strcpy(buf,
           "{c({r%h{c/{R%H{chp {b%m{c/{B%M{cm {y%v{c/{Y%V{cmv{C ({g%X{ctnl{C){c>{x");
  else if (!str_cmp(argument, "imm"))
  {
    if (!IS_IMMORTAL(ch))
    {
      return;
    }
    else
    {
      strcpy(buf, "{r%h-{R%Hhp {b%m-{B%Mm {g%v-{G%Vmv {C%Xtl {Y%bx{w>{x");
    }
  }
  else if (!str_cmp(argument, "imm2"))
  {
    if (!IS_IMMORTAL(ch))
    {
      return;
    }
    else
    {
      strcpy(buf,
             "{r%h{w-{R%H{whp {b%m{w-{B%M{wm {g%v{w-{G%V{wmv {C%X{wtnl {Y%b{wtb {M%q{wqt{W>{x%c");
    }
  }
  else
  {
    if (strlen(argument) > 90)
      argument[90] = '\0';
    strcpy(buf, argument);
    smash_tilde(buf);
    if (str_suffix("%c", buf))
      strcat(buf, "{x ");

  }

  free_string(ch->prompt);
  ch->prompt = str_dup(buf);
  sprintf(buf, "Prompt set to %s\n\r", ch->prompt);
  send_to_char(buf, ch);
  return;
}

CH_CMD(do_combine)
{
  if (IS_SET(ch->comm, COMM_COMBINE))
  {
    send_to_char("Long inventory selected.\n\r", ch);
    REMOVE_BIT(ch->comm, COMM_COMBINE);
  }
  else
  {
    send_to_char("Combined inventory selected.\n\r", ch);
    SET_BIT(ch->comm, COMM_COMBINE);
  }
}

CH_CMD(do_noloot)
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_CANLOOT))
  {
    send_to_char("Your corpse is now safe from thieves.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_CANLOOT);
  }
  else
  {
    send_to_char("Your corpse may now be looted.\n\r", ch);
    SET_BIT(ch->act, PLR_CANLOOT);
  }
}

CH_CMD(do_nofollow)
{
  if (IS_NPC(ch))
    return;

  if (IS_SET(ch->act, PLR_NOFOLLOW))
  {
    send_to_char("You now accept followers.\n\r", ch);
    REMOVE_BIT(ch->act, PLR_NOFOLLOW);
  }
  else
  {
    send_to_char("You no longer accept followers.\n\r", ch);
    SET_BIT(ch->act, PLR_NOFOLLOW);
    die_follower(ch);
  }
}

CH_CMD(do_nosummon)
{
  if (IS_NPC(ch))
  {
    if (IS_SET(ch->imm_flags, IMM_SUMMON))
    {
      send_to_char("You are no longer immune to summon.\n\r", ch);
      REMOVE_BIT(ch->imm_flags, IMM_SUMMON);
    }
    else
    {
      send_to_char("You are now immune to summoning.\n\r", ch);
      SET_BIT(ch->imm_flags, IMM_SUMMON);
    }
  }
  else
  {
    if (IS_SET(ch->act, PLR_NOSUMMON))
    {
      send_to_char("You are no longer immune to summon.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_NOSUMMON);
    }
    else
    {
      send_to_char("You are now immune to summoning.\n\r", ch);
      SET_BIT(ch->act, PLR_NOSUMMON);
    }
  }
}

CH_CMD(do_notran)
{
  if (IS_NPC(ch))
  {
    return;
  }
  else
  {
    if (IS_SET(ch->act, PLR_NOTRAN))
    {
      send_to_char("You are no longer immune to transport.\n\r", ch);
      REMOVE_BIT(ch->act, PLR_NOTRAN);
    }
    else
    {
      send_to_char("You are now immune to transport.\n\r", ch);
      SET_BIT(ch->act, PLR_NOTRAN);
    }
  }
}

CH_CMD(do_look)
{
  char buf[MSL * 100];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  BUFFER *outlist;
  EXIT_DATA *pexit;
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  char *pdesc;
  int door;
  int number, count;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (ch->desc == NULL)
    return;

  if (ch->position < POS_SLEEPING)
  {
    send_to_char("You can't see anything but stars!\n\r", ch);
    return;
  }

  if (ch->position == POS_SLEEPING)
  {
    send_to_char("You can't see anything, you're sleeping!\n\r", ch);
    return;
  }

  if (!check_blind(ch))
    return;

  if (!IS_NPC(ch) && !IS_SET(ch->act, PLR_HOLYLIGHT) &&
      room_is_dark(ch->in_room))
  {
    send_to_char("{xIt is pitch {Dblack{x ... \n\r", ch);
    show_char_to_char(ch->in_room->people, ch);
    return;
  }

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  number = number_argument(arg1, arg3);
  count = 0;

  if (arg1[0] == '\0' || !str_cmp(arg1, "auto"))
  {
    /* 'look' or 'look auto' */
    send_to_char("{w", ch);
    send_to_char(ch->in_room->name, ch);
    if (IS_SET(ch->in_room->room_flags, ROOM_LAW))
    {
      sprintf(buf, " {w[{BL{cA{BW{w]");
      send_to_char(buf, ch);
    }
    send_to_char("{x", ch);

    if (IS_IMMORTAL(ch) && (IS_NPC(ch) || IS_SET(ch->act, PLR_HOLYLIGHT)))
    {
      sprintf(buf, " {y[{YRoom %ld{y]{x", ch->in_room->vnum);
      send_to_char(buf, ch);
    }

    send_to_char("\n\r", ch);

    if (arg1[0] == '\0' || (!IS_NPC(ch) && !IS_SET(ch->comm, COMM_BRIEF)))
    {
      send_to_char("  ", ch);
      send_to_char(ch->in_room->description, ch);
      if (ch->in_room->vnum == chain)
      {
        send_to_char
          ("A huge black iron chain as thick as a tree trunk is drifting above the ground\n\r",
           ch);
        send_to_char("here.\n\r", ch);
      }
    }

    if (!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT))
    {
      send_to_char("\n\r", ch);
      do_exits(ch, "auto");
    }

    outlist = show_list_to_char(ch->in_room->contents, ch, false, false);
    page_to_char(buf_string(outlist), ch);
    free_buf(outlist);
    show_char_to_char(ch->in_room->people, ch);
    return;
  }

  if (!str_cmp(arg1, "i") || !str_cmp(arg1, "in") || !str_cmp(arg1, "on"))
  {
    /* 'look in' */
    if (arg2[0] == '\0')
    {
      send_to_char("Look in what?\n\r", ch);
      return;
    }

    if ((obj = get_obj_here(ch, arg2)) == NULL)
    {
      send_to_char("You do not see that here.\n\r", ch);
      return;
    }

    switch (obj->item_type)
    {
      default:
        send_to_char("That is not a container.\n\r", ch);
        break;

      case ITEM_DRINK_CON:
        if (obj->value[1] <= 0)
        {
          send_to_char("It is empty.\n\r", ch);
          break;
        }

        sprintf(buf, "It's %sfilled with  a %s liquid.\n\r",
                obj->value[1] <
                obj->value[0] /
                4 ? "less than half-" : obj->value[1] <
                3 * obj->value[0] /
                4 ? "about half-" : "more than half-",
                liq_table[obj->value[2]].liq_color);

        send_to_char(buf, ch);
        break;

      case ITEM_CONTAINER:
      case ITEM_PIT:
      case ITEM_CORPSE_NPC:
      case ITEM_CORPSE_PC:
        if (IS_SET(obj->value[1], CONT_CLOSED))
        {
          send_to_char("It is closed.\n\r", ch);
          break;
        }

        act("$p holds:", ch, obj, NULL, TO_CHAR);
        outlist = show_list_to_char(obj->contains, ch, true, true);
        page_to_char(buf_string(outlist), ch);
        free_buf(outlist);
        break;
      case ITEM_ITEMPILE:
        if (IS_SET(obj->value[1], CONT_CLOSED))
        {
          send_to_char("It is closed.\n\r", ch);
          break;
        }

        act("$p holds:", ch, obj, NULL, TO_CHAR);
        outlist = show_list_to_char(obj->contains, ch, true, true);
        page_to_char(buf_string(outlist), ch);
        free_buf(outlist);
        break;
    }
    return;
  }

  if ((victim = get_char_room(ch, arg1)) != NULL)
  {
    if (victim == ch)
    {
      do_equipment(ch, "");
      return;
    }
    show_char_to_char_1(victim, ch);
    return;
  }

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
  {
    if (can_see_obj(ch, obj))
    {                           /* player can see object */
      pdesc = get_extra_descr(arg3, obj->extra_descr);
      if (pdesc != NULL)
      {
        if (++count == number)
        {
          send_to_char(pdesc, ch);
          return;
        }
        else
          continue;
      }
      pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
      if (pdesc != NULL)
      {
        if (++count == number)
        {
          send_to_char(pdesc, ch);
          return;
        }
        else
          continue;
      }
      if (is_name(arg3, obj->name))
        if (++count == number)
        {
          send_to_char(obj->description, ch);
          send_to_char("\n\r", ch);
          return;
        }
    }
  }

  for (obj = ch->in_room->contents; obj != NULL; obj = obj->next_content)
  {
    if (can_see_obj(ch, obj))
    {
      pdesc = get_extra_descr(arg3, obj->extra_descr);
      if (pdesc != NULL)
        if (++count == number)
        {
          send_to_char(pdesc, ch);
          return;
        }

      pdesc = get_extra_descr(arg3, obj->pIndexData->extra_descr);
      if (pdesc != NULL)
        if (++count == number)
        {
          send_to_char(pdesc, ch);
          return;
        }
      if (is_name(arg3, obj->name))
        if (++count == number)
        {
          send_to_char(obj->description, ch);
          send_to_char("\n\r", ch);
          return;
        }
    }
  }

  pdesc = get_extra_descr(arg3, ch->in_room->extra_descr);
  if (pdesc != NULL)
  {
    if (++count == number)
    {
      send_to_char(pdesc, ch);
      return;
    }
  }

  if (count > 0 && count != number)
  {
    if (count == 1)
      sprintf(buf, "You only see one %s here.\n\r", arg3);
    else
      sprintf(buf, "You only see %d of those here.\n\r", count);

    send_to_char(buf, ch);
    return;
  }

  if (!str_cmp(arg1, "n") || !str_cmp(arg1, "north"))
    door = 0;
  else if (!str_cmp(arg1, "e") || !str_cmp(arg1, "east"))
    door = 1;
  else if (!str_cmp(arg1, "s") || !str_cmp(arg1, "south"))
    door = 2;
  else if (!str_cmp(arg1, "w") || !str_cmp(arg1, "west"))
    door = 3;
  else if (!str_cmp(arg1, "u") || !str_cmp(arg1, "up"))
    door = 4;
  else if (!str_cmp(arg1, "d") || !str_cmp(arg1, "down"))
    door = 5;
  else if (!str_cmp(arg1, "ne") || !str_cmp(arg1, "northeast"))
    door = 6;
  else if (!str_cmp(arg1, "se") || !str_cmp(arg1, "southeast"))
    door = 7;
  else if (!str_cmp(arg1, "sw") || !str_cmp(arg1, "southwest"))
    door = 8;
  else if (!str_cmp(arg1, "nw") || !str_cmp(arg1, "northwest"))
    door = 9;
  else
  {
    send_to_char("You do not see that here.\n\r", ch);
    return;
  }

  /* 'look direction' */
  if ((pexit = ch->in_room->exit[door]) == NULL)
  {
    send_to_char("Nothing special there.\n\r", ch);
    return;
  }

  if (pexit->description != NULL && pexit->description[0] != '\0')
    send_to_char(pexit->description, ch);
  else
    send_to_char("Nothing special there.\n\r", ch);

  if (pexit->keyword != NULL && pexit->keyword[0] != '\0' &&
      pexit->keyword[0] != ' ')
  {
    if (IS_SET(pexit->exit_info, EX_CLOSED))
    {
      act("The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR);
    }
    else if (IS_SET(pexit->exit_info, EX_ISDOOR))
    {
      act("The $d is open.", ch, NULL, pexit->keyword, TO_CHAR);
    }
  }

  return;
}

/* RT added back for the hell of it */
CH_CMD(do_read)
{
  do_look(ch, argument);
}

CH_CMD(do_examine)
{
  char buf[MSL];
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Examine what?\n\r", ch);
    return;
  }

  if ((obj = get_obj_here(ch, arg)) != NULL)
  {
    switch (obj->item_type)
    {
      default:
        do_look(ch, arg);
        break;

      case ITEM_JUKEBOX:
        do_play(ch, "list");
        break;

      case ITEM_MONEY:
        if (obj->value[0] == 0)
        {
          if (obj->value[1] == 0)
            sprintf(buf, "Odd...there's no coins in the pile.\n\r");
          else if (obj->value[1] == 1)
            sprintf(buf, "Wow. One gold coin.\n\r");
          else
            sprintf(buf,
                    "There are %ld gold coins in the pile.\n\r",
                    obj->value[1]);
        }
        else if (obj->value[1] == 0)
        {
          if (obj->value[0] == 1)
            sprintf(buf, "Wow. One silver coin.\n\r");
          else
            sprintf(buf,
                    "There are %ld silver coins in the pile.\n\r",
                    obj->value[0]);
        }
        else
          sprintf(buf,
                  "There are %ld gold and %ld silver coins in the pile.\n\r",
                  obj->value[1], obj->value[0]);
        send_to_char(buf, ch);
        break;

      case ITEM_DRINK_CON:
        sprintf(buf, "in %s", argument);
        do_look(ch, buf);
        break;
      case ITEM_CONTAINER:
        sprintf(buf, "in %s", argument);
        do_look(ch, buf);
        break;
      case ITEM_PIT:
        sprintf(buf, "in %s", argument);
        do_look(ch, buf);
        break;
      case ITEM_CORPSE_NPC:
        sprintf(buf, "in %s", argument);
        do_look(ch, buf);
        break;
      case ITEM_CORPSE_PC:
        sprintf(buf, "in %s", argument);
        do_look(ch, buf);
        break;
      case ITEM_ITEMPILE:
        sprintf(buf, "in %s", argument);
        do_look(ch, buf);
        break;
    }
  }

  return;
}

CH_CMD(do_lore)
{
  char buf[MSL];
  char arg[MAX_INPUT_LENGTH];
  AFFECT_DATA *paf;
  OBJ_DATA *obj;

  if (!IS_NPC(ch) && ch->level < skill_table[gsn_lore].skill_level[ch->class])
  {
    send_to_char("You would like to what?\n\r", ch);
    return;
  }

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("What Would you like to know more about?\n\r", ch);
    return;
  }

  if ((obj = get_obj_carry(ch, arg)) == NULL)
  {
    send_to_char("You cannot lore that because you do not have that.\n\r",
                 ch);
    return;
  }
  if (number_percent() < get_skill(ch, gsn_lore))
  {
    send_to_char("You learn more about this object:\n\r", ch);
    check_improve(ch, gsn_lore, true, 4);

    sprintf(buf, "Name(s): %s\n\r", obj->name);
    send_to_char(buf, ch);
    sprintf(buf, "Short description: %s\n\rLong description: %s\n\r",
            obj->short_descr, obj->description);
    send_to_char(buf, ch);
    //    sprintf( buf, "Wear bits: %s\n\rExtra bits: %s\n\r",
    sprintf(buf, "Extra Flags: %s\n\r",
            //        wear_bit_name(obj->wear_flags), 
            extra_bit_name(obj->extra_flags));
    send_to_char(buf, ch);
    sprintf(buf, "Weight: %d\n\r", obj->weight / 10);
    send_to_char(buf, ch);
    sprintf(buf, "Level: %d  Cost: %d\n\r", obj->level, obj->cost);
    send_to_char(buf, ch);
    /* now give out vital statistics as per identify */
    switch (obj->item_type)
    {
      case ITEM_SCROLL:
      case ITEM_POTION:
      case ITEM_PILL:
        sprintf(buf, "Level %ld spells of:", obj->value[0]);
        send_to_char(buf, ch);
        if (obj->value[1] >= 0 && obj->value[1] < MAX_SKILL)
        {
          send_to_char(" '", ch);
          send_to_char(skill_table[obj->value[1]].name, ch);
          send_to_char("'", ch);
        }

        if (obj->value[2] >= 0 && obj->value[2] < MAX_SKILL)
        {
          send_to_char(" '", ch);
          send_to_char(skill_table[obj->value[2]].name, ch);
          send_to_char("'", ch);
        }

        if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
        {
          send_to_char(" '", ch);
          send_to_char(skill_table[obj->value[3]].name, ch);
          send_to_char("'", ch);
        }

        if (obj->value[4] >= 0 && obj->value[4] < MAX_SKILL)
        {
          send_to_char(" '", ch);
          send_to_char(skill_table[obj->value[4]].name, ch);

          send_to_char("'", ch);
        }

        send_to_char(".\n\r", ch);
        break;
      case ITEM_WAND:
      case ITEM_STAFF:
      case ITEM_INSTRUMENT:
        sprintf(buf, "Has %ld(%ld) charges of level %ld",
                obj->value[1], obj->value[2], obj->value[0]);
        send_to_char(buf, ch);
        if (obj->value[3] >= 0 && obj->value[3] < MAX_SKILL)
        {
          send_to_char(" '", ch);
          send_to_char(skill_table[obj->value[3]].name, ch);
          send_to_char("'", ch);
        }

        send_to_char(".\n\r", ch);
        break;
      case ITEM_DRINK_CON:
        sprintf(buf, "It holds %s-colored %s.\n\r",
                liq_table[obj->value[2]].liq_color,
                liq_table[obj->value[2]].liq_name);
        send_to_char(buf, ch);
        break;
      case ITEM_WEAPON:
        send_to_char("Weapon type is ", ch);
        switch (obj->value[0])
        {
          case (WEAPON_EXOTIC):
            send_to_char("exotic\n\r", ch);
            break;
          case (WEAPON_SWORD):
            send_to_char("sword\n\r", ch);
            break;
          case (WEAPON_DAGGER):
            send_to_char("dagger\n\r", ch);
            break;
          case (WEAPON_SPEAR):
            send_to_char("spear/staff\n\r", ch);
            break;
          case (WEAPON_MACE):
            send_to_char("mace/club\n\r", ch);
            break;
          case (WEAPON_AXE):
            send_to_char("axe\n\r", ch);
            break;
          case (WEAPON_FLAIL):
            send_to_char("flail\n\r", ch);
            break;
          case (WEAPON_WHIP):
            send_to_char("whip\n\r", ch);
            break;
          case (WEAPON_POLEARM):
            send_to_char("polearm\n\r", ch);
            break;
          default:
            send_to_char("unknown\n\r", ch);
            break;
        }
        if (obj->pIndexData->new_format)
          sprintf(buf, "Damage is %ldd%ld (average %ld)\n\r",
                  obj->value[1], obj->value[2],
                  (1 + obj->value[2]) * obj->value[1] / 2);
        else
          sprintf(buf, "Damage is %ld to %ld (average %ld)\n\r",
                  obj->value[1], obj->value[2],
                  (obj->value[1] + obj->value[2]) / 2);
        send_to_char(buf, ch);
        /* sprintf(buf,"Weapon type is %s.\n\r", (obj->value[3] > 0 &&
           obj->value[3] < MAX_DAMAGE_MESSAGE) ?
           attack_table[obj->value[3]].noun : "undefined");
           send_to_char(buf,ch); */
        if (obj->value[4])      /* weapon flags */
        {
          sprintf(buf, "Weapons flags: %s\n\r",
                  weapon_bit_name(obj->value[4]));
          send_to_char(buf, ch);
        }
        break;
      case ITEM_ARMOR:
        sprintf(buf,
                "Armor class is %ld pierce, %ld bash, %ld slash, and %ld vs. magic\n\r",
                obj->value[0], obj->value[1], obj->value[2], obj->value[3]);
        send_to_char(buf, ch);
        break;
      case ITEM_CONTAINER:
        sprintf(buf,
                "Capacity: %ld#  Maximum weight: %ld# flags: %s\n\r",
                obj->value[0], obj->value[3], cont_bit_name(obj->value[1]));
        send_to_char(buf, ch);
        if (obj->value[4] != 100)
        {
          sprintf(buf, "Weight multiplier: %ld%%\n\r", obj->value[4]);
          send_to_char(buf, ch);
        }
        break;
    }

    if (obj->extra_descr != NULL || obj->pIndexData->extra_descr != NULL)
    {
      EXTRA_DESCR_DATA *ed;

      send_to_char("Extra description keywords: '", ch);
      for (ed = obj->extra_descr; ed != NULL; ed = ed->next)
      {
        send_to_char(ed->keyword, ch);
        if (ed->next != NULL)
          send_to_char(" ", ch);
      }

      for (ed = obj->pIndexData->extra_descr; ed != NULL; ed = ed->next)
      {
        send_to_char(ed->keyword, ch);
        if (ed->next != NULL)
          send_to_char(" ", ch);
      }

      send_to_char("'\n\r", ch);
    }

    for (paf = obj->affected; paf != NULL; paf = paf->next)
    {
      sprintf(buf, "Affects %s by %d, level %d",
              affect_loc_name(paf->location), paf->modifier, paf->level);
      send_to_char(buf, ch);
      if (paf->duration > -1)
        sprintf(buf, ", %d hours.\n\r", paf->duration);
      else
        sprintf(buf, ".\n\r");
      send_to_char(buf, ch);
      if (paf->bitvector)
      {
        switch (paf->where)
        {
          case TO_AFFECTS:
            sprintf(buf, "Adds %s affect.\n",
                    affect_bit_name(paf->bitvector));
            break;
          case TO_WEAPON:
            sprintf(buf, "Adds %s weapon flags.\n",
                    weapon_bit_name(paf->bitvector));
            break;
          case TO_OBJECT:
            sprintf(buf, "Adds %s object flag.\n",
                    extra_bit_name(paf->bitvector));
            break;
          case TO_IMMUNE:
            sprintf(buf, "Adds immunity to %s.\n",
                    imm_bit_name(paf->bitvector));
            break;
          case TO_RESIST:
            sprintf(buf, "Adds resistance to %s.\n\r",
                    imm_bit_name(paf->bitvector));
            break;
          case TO_VULN:
            sprintf(buf, "Adds vulnerability to %s.\n\r",
                    imm_bit_name(paf->bitvector));
            break;
          default:
            sprintf(buf, "Unknown bit %d: %ld\n\r", paf->where,
                    paf->bitvector);
            break;
        }
        send_to_char(buf, ch);
      }
    }

    if (!obj->enchanted)
      for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
      {
        sprintf(buf, "Affects %s by %d, level %d.\n\r",
                affect_loc_name(paf->location), paf->modifier, paf->level);
        send_to_char(buf, ch);
        if (paf->bitvector)
        {
          switch (paf->where)
          {
            case TO_AFFECTS:
              sprintf(buf, "Adds %s affect.\n",
                      affect_bit_name(paf->bitvector));
              break;
            case TO_OBJECT:
              sprintf(buf, "Adds %s object flag.\n",
                      extra_bit_name(paf->bitvector));
              break;
            case TO_IMMUNE:
              sprintf(buf, "Adds immunity to %s.\n",
                      imm_bit_name(paf->bitvector));
              break;
            case TO_RESIST:
              sprintf(buf, "Adds resistance to %s.\n\r",
                      imm_bit_name(paf->bitvector));
              break;
            case TO_VULN:
              sprintf(buf, "Adds vulnerability to %s.\n\r",
                      imm_bit_name(paf->bitvector));
              break;
            default:
              sprintf(buf, "Unknown bit %d: %ld\n\r",
                      paf->where, paf->bitvector);
              break;
          }
          send_to_char(buf, ch);
        }
      }
  }
  return;
}

/*
 * Thanks to Zrin for auto-exit part.
 */
CH_CMD(do_exits)
{
  char buf[MSL * 100];
  char buf2[MSL * 100];
  int num_exits = 0;
  int num_doors = 0;
  EXIT_DATA *pexit;
  bool found;
  bool round;
  bool fAuto;
  int door;

  fAuto = !str_cmp(argument, "auto");
  if (!check_blind(ch))
    return;
  if (fAuto)
  {
    sprintf(buf, "{g[{GExits:");
    sprintf(buf2, "{c[{CDoors:");
  }
  else if (IS_IMMORTAL(ch))
    sprintf(buf, "{CO{cbvious exits from room {W%ld{C:{x\n\r",
            ch->in_room->vnum);
  else
    sprintf(buf, "{CO{cbvious exits{c:{x\n\r");
  found = false;
  for (door = 0; door < MAX_DIR; door++)
  {
    round = false;
    if ((pexit = ch->in_room->exit[door]) != NULL &&
        pexit->u1.to_room != NULL && can_see_room(ch, pexit->u1.to_room))
    {
      found = true;
      round = true;
      if (fAuto)
      {
        if (!IS_SET(pexit->exit_info, EX_CLOSED))
        {
          num_exits++;
          strcat(buf, " ");
          strcat(buf, dir_name[door]);
        }
        else
        {
          num_doors++;
          strcat(buf2, " ");
          strcat(buf2, dir_name[door]);
        }

      }
      else
      {
        sprintf(buf + strlen(buf), "%-5s{x - %s",
                capitalize(dir_name[door]),
                room_is_dark(pexit->u1.
                             to_room) ? "Too dark to tell" :
                pexit->u1.to_room->name);
        if (IS_IMMORTAL(ch))
          sprintf(buf + strlen(buf), " (room %ld)\n\r",
                  pexit->u1.to_room->vnum);
        else
          sprintf(buf + strlen(buf), "\n\r");
      }
    }
    if (!round)
    {
      OBJ_DATA *portal;
      ROOM_INDEX_DATA *to_room;

      portal = get_obj_exit(dir_name[door], ch->in_room->contents);
      if (portal != NULL)
      {
        found = true;
        round = true;
        if (fAuto)
        {
          strcat(buf, " ");
          strcat(buf, dir_name[door]);
        }
        else
        {
          to_room = get_room_index(portal->value[0]);
          sprintf(buf + strlen(buf), "%-5s {x- %s",
                  capitalize(dir_name[door]),
                  room_is_dark(to_room) ? "Too dark to tell" : to_room->name);
          if (IS_IMMORTAL(ch))
            sprintf(buf + strlen(buf), " (room %ld)\n\r", to_room->vnum);
          else
            sprintf(buf + strlen(buf), "\n\r");
        }
      }
    }
  }

  if (!found)
    strcat(buf, fAuto ? " none" : "None.\n\r");
  if (fAuto)
  {
    if (num_exits > 0)
    {
      strcat(buf, "{g]{x ");
    }
    else
    {
      strcat(buf, " {GNone{g]{x ");
    }

    if (num_doors > 0)
    {
      strcat(buf2, "{c]{x\n\r");
    }
    else
    {
      strcat(buf2, " {CNone{c]{x\n\r");
    }

  }
  send_to_char(buf, ch);
  send_to_char(buf2, ch);
  return;
}

CH_CMD(do_worth)
{
  char buf[MSL];

  if (IS_NPC(ch))
  {
    sprintf(buf,
            "{yYou have {C%ld {yplatinum, {Y%ld{y gold and {W%ld{y silver.{x\n\r",
            ch->platinum, ch->gold, ch->silver);
    send_to_char(buf, ch);
    return;
  }

  sprintf(buf,
          "{yYou have {C%ld{y platinum, {Y%ld {ygold, {W%ld{y silver.\n\r{yYou have {G%ld{y experience.{W({G%ld{y exp to level{W){y\n\r{yYour bank balance is {C%ld{y platinum.{x\n\r{yYou have {W%ld{y quest points.{x\n\r",
          ch->platinum, ch->gold, ch->silver, ch->exp,
          (ch->level + 1) * exp_per_level(ch,
                                          ch->pcdata->points) -
          ch->exp, ch->pcdata->balance, ch->pcdata->questpoints);
  send_to_char(buf, ch);
  return;
}

CH_CMD(do_charinfo)
{
  int i;

  printf_to_char(ch, "\n\r                           Character Info{x\n\r");
  printf_to_char(ch,
                 "{W-------------------------------------------------------------------------{x\n\r");
  printf_to_char(ch,
                 "{W|                            {GStats{x\n\r"
                 "{W| {RCon{x:{b %d{y({B%d{y) {RInt:{b %d{y({B%d{y) {RWis{x:{b %d{y({B%d{y){x"
                 "{RDex{x:{b %d{y({B%d{y) {RStr{x:{b %d{y({B%d{y){x\n\r",
                 ch->perm_stat[STAT_CON], get_curr_stat(ch, STAT_CON),
                 ch->perm_stat[STAT_INT], get_curr_stat(ch, STAT_INT),
                 ch->perm_stat[STAT_WIS], get_curr_stat(ch, STAT_WIS),
                 ch->perm_stat[STAT_DEX], get_curr_stat(ch, STAT_DEX),
                 ch->perm_stat[STAT_STR], get_curr_stat(ch, STAT_STR));
  printf_to_char(ch, "{W|\n\r{W| {G                           Armor{x\n\r");
  if (ch->level >= 1)
  {
    printf_to_char(ch,
                   "{W| {RPierce{x:{B %d{R   Bash{x:{B %d{x   {RSlash {x:{B %d{R   Magic{x:{B %d{x\n\r",
                   GET_AC(ch, AC_PIERCE), GET_AC(ch, AC_BASH),
                   GET_AC(ch, AC_SLASH), GET_AC(ch, AC_EXOTIC));
  }
  for (i = 0; i < 4; i++)
  {
    char *temp;

    switch (i)
    {
      case (AC_PIERCE):
        temp = "piercing";
        break;
      case (AC_BASH):
        temp = "bashing";
        break;
      case (AC_SLASH):
        temp = "slashing";
        break;
      case (AC_EXOTIC):
        temp = "magic";
        break;
      default:
        temp = "error";
        break;
    }
  }

  printf_to_char(ch,
                 "{W---------------------------------------------------------------------------{x\n\r");
}

/* 
   New score function 
   copyright of Loki of Death Rising.
   If you use this score you must leave this header intact. */

CH_CMD(do_newscore)
{

  return;
}


CH_CMD(do_score_loki)
{
  int i;
  char buf[MSL];

  if (IS_NPC(ch))
    return;

  printf_to_char(ch,
                 "\n\r{Y=============================================================================={x\n\r");
  printf_to_char(ch, "{Y|             {b        %s%s{x\n\r", ch->name,
                 IS_NPC(ch) ? ", the mobile." : ch->pcdata->title);
  printf_to_char(ch,
                 "{Y+-----------------------------------------------------------------------------{x\n\r");
  printf_to_char(ch,
                 "{Y| {cRace {x:{g %-13s{Y| {cAQP{x :{g %-8ld{Y| {cHP{x     :{g %-6ld{x/{G%ld\n\r"
                 "{Y| {cAge  {x:{g %-13d{Y| {cIQP{x :{g %-8d{Y| {cMN{x     :{g %-6ld{x/{G%ld\n\r"
                 "{Y| {cSex  {x:{g %-13s{Y| {cDamr{x:{g %-8d{Y| {cMV{x     :{g %-6ld{x/{G%ld\n\r"
                 "{Y| {cLevel{x:{g %-13d{Y| {cHitr{x:{g %-8d{Y| {cItems{x  :{g %-6d{x/{G%d\n\r"
                 "{Y| {cClass{x:{g %-13s{Y| {cPlat{x:{g %-8ld{Y| {cWeight{x :{g %-6ld{x/{G%d\n\r"
                 "{Y| {cAlign{x:{g %-13d{Y| {cGold{x:{g %-8ld{Y| {cSaves{x  :{g %-6d{x\n\r"
                 "{Y| {cWimpy{x:{g %-13d{Y| {cSilv{x:{g %-8ld{Y| {cBalance{x:{g %ld{x\n\r",
                 IS_NPC(ch) ? "mobile" : race_table[ch->race].name,
                 ch->pcdata->questpoints, ch->hit, ch->max_hit,
                 get_age(ch), ch->qps, ch->mana, ch->max_mana,
                 ch->sex == 0 ? "sexless" : ch->sex ==
                 1 ? "male" : "female", GET_DAMROLL(ch), ch->move,
                 ch->max_move, ch->level, GET_HITROLL(ch),
                 ch->carry_number, can_carry_n(ch),
                 class_table[ch->class].name, ch->platinum,
                 get_carry_weight(ch) / 10, can_carry_w(ch) / 10,
                 ch->alignment, ch->gold, ch->saving_throw, ch->wimpy,
                 ch->silver, ch->pcdata->balance);

  /*stats */
  printf_to_char(ch,
                 "{Y+-----------------------------------------------------------------------------{x\n\r");
  printf_to_char(ch,
                 "{Y| {cStr{x:{g %d{x({G%d{x) {cInt{x: {g%d{x({G%d{x) {cWis{x: {g%d{x({G%d{x) {cDex{x: {g%d{x({G%d{x) {cCon{x: {g%d{x({G%d{x)\n\r",
                 ch->perm_stat[STAT_STR], get_curr_stat(ch, STAT_STR),
                 ch->perm_stat[STAT_INT], get_curr_stat(ch, STAT_INT),
                 ch->perm_stat[STAT_WIS], get_curr_stat(ch, STAT_WIS),
                 ch->perm_stat[STAT_DEX], get_curr_stat(ch, STAT_DEX),
                 ch->perm_stat[STAT_CON], get_curr_stat(ch, STAT_CON));
   /*AC*/ if (ch->level >= 1)
  {
    printf_to_char(ch,
                   "{Y| {cPierce{x:{g %d{c   Bash{x:{g %d{x   {cSlash {x:{g %d{c   Magic{x:{g %d{x\n\r",
                   GET_AC(ch, AC_PIERCE), GET_AC(ch, AC_BASH),
                   GET_AC(ch, AC_SLASH), GET_AC(ch, AC_EXOTIC));
  }
  for (i = 0; i < 4; i++)
  {
    char *temp;

    switch (i)
    {
      case (AC_PIERCE):
        temp = "piercing";
        break;
      case (AC_BASH):
        temp = "bashing";
        break;
      case (AC_SLASH):
        temp = "slashing";
        break;
      case (AC_EXOTIC):
        temp = "magic";
        break;
      default:
        temp = "error";
        break;
    }
  }
  printf_to_char(ch,
                 "{Y+-----------------------------------------------------------------------------{x\n\r");
  /* RT wizinvis and holy light */
  if (IS_IMMORTAL(ch))
  {
    printf_to_char(ch, "{Y| {cHoly Light{x: ");
    if (IS_SET(ch->act, PLR_HOLYLIGHT))
      printf_to_char(ch, "{Gon{x");
    else
      printf_to_char(ch, "{goff{x");
  }
  if (IS_IMMORTAL(ch))
  {
    printf_to_char(ch,
                   "{c   Invis{x:{g %d   {cIncognito{x: %d   {cGhost{x:{g %d\n\r",
                   ch->invis_level, ch->incog_level, ch->ghost_level);
    printf_to_char(ch,
                   "{Y+-----------------------------------------------------------------------------{x\n\r");
  }
  if (!IS_NPC(ch))
  {
    printf_to_char(ch,
                   "{Y| {c[{gArena Stats{c]{c Wins{x:{g %d  {cLosses{x:{g %d{x\n\r",
                   ch->pcdata->awins, ch->pcdata->alosses);
    if (ch->challenger != NULL)
    {
      printf_to_char(ch,
                     "{Y| {c[{gArena{c]{c You have been challenged by{r %s{c.{x\n\r",
                     ch->challenger->name);
    }
    if (ch->gladiator != NULL)
    {
      printf_to_char(ch,
                     "{Y| {c[{gArena bet{c]{c You have a {Y%d platinum{c bet on %s.{x\n\r",
                     ch->pcdata->plr_wager, ch->gladiator->name);
    }
  }

  if (!IS_NPC(ch))
  {
    printf_to_char(ch,
                   "{Y| {c[{gPK stats{c]{R Kills{x: %ld  {DDeaths{x: %ld\n\r",
                   ch->pcdata->pkkills, ch->pcdata->pkdeaths);
  }
  if (is_clan(ch))
  {
    printf_to_char(ch, "{Y| {cClan{x: %s {cRank{x: %s{x, %s {x\n\r",
                   clan_table[ch->clan].who_name,
                   clan_rank_table[ch->clan_rank].title_of_rank[ch->sex],
                   is_pkill(ch) ? "{RPkill{c.{x" : "{mNon-Pkill{c.{x");
  }
  if (ch->invited)
  {
    printf_to_char(ch,
                   "{Y| {RYou have been invited to join clan {x[{%s%s{x]\n\r",
                   clan_table[ch->invited].pkill ? "B" : "M",
                   clan_table[ch->invited].who_name);
  }
  if (!IS_NPC(ch))
  {
    int rcnt = roomcount(ch);
    int arcnt = areacount(ch);
    double rooms = top_room, percent = (double) rcnt / (rooms / 100);
    double arooms = (double) (arearooms(ch)), apercent =
      (double) arcnt / (arooms / 100);
    printf_to_char(ch,
                   "{Y| {cExplored: {g%4d{c/{G%4d{c rooms. ({W%5.2f%%{c of the world) ({W%5.2f%%{c of current area)\n\r{x",
                   rcnt, top_room, percent, apercent);
  }
  sprintf(buf, "{Y| {cRole-play points: (Current/Total) {g%-6d / %-6d {x\n\r",
          ch->rps, ch->rpst);
  send_to_char(buf, ch);

  printf_to_char(ch,
                 "{Y=============================================================================={x\n\r");
  if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
    do_affects(ch, "");

  if (IS_SET(ch->act2, PLR_RP))
  {
    printf_to_char(ch, "{D[{RRP{D]{x flag is {GON{x.\n\r");
  }

  if (ch->pk_timer > 0)
  {
    printf_to_char(ch, "You have a {RPK{x timer.\n\r");
  }

  sprintf(buf, "{GT{gime {GB{gonus{G: {Y%.2f{x\n\r", (ch->btime / 40 + 1));
  send_to_char(buf, ch);
  if (ch->nameauthed == 0)
    send_to_char("{RYour name has not yet been approved.\n\r", ch);
}

/* This is NEW score listing for The Mage's Lair */
CH_CMD(do_lscore)
{
  char buf[MSL];
  int i;

  if (IS_NPC(ch))
  {
    return;
  }

  send_to_char
    ("{Y------------------------------------------------------------------{x\n\r",
     ch);
  printf_to_char(ch,
                 "{cYou are {W%s%s{c, level {W%d{c, {W%d {cyears old ({W%d{c hours).\n\r",
                 ch->name, IS_NPC(ch) ? "" : ch->pcdata->title,
                 ch->level, get_age(ch),
                 (ch->played + (int) (current_time - ch->logon)) / 3600);
  if (get_trust(ch) != ch->level)
  {
    printf_to_char(ch, "{cYou are trusted at level {W%d{c.\n\r",
                   get_trust(ch));
  }

  printf_to_char(ch, "{cRace: {W%s  {cSex: {W%s  {cClass: {W%s\n\r",
                 race_table[ch->race].name,
                 ch->sex == 0 ? "sexless" : ch->sex ==
                 1 ? "male" : "female",
                 IS_NPC(ch) ? "mobile" : class_table[ch->class].name);
  if (ch->clan)
  {
    printf_to_char(ch, "{cClan:{x %s  {cRank:{x %s\n\r",
                   clan_table[ch->clan].who_name,
                   clan_rank_table[ch->clan_rank].title_of_rank[ch->sex]);
  }
  printf_to_char(ch,
                 "{cStr: {W%d(%d)  {cInt: {W%d(%d)  {cWis: {W%d(%d)  {cDex: {W%d(%d)  {cCon: {W%d(%d)\n\r",
                 ch->perm_stat[STAT_STR], get_curr_stat(ch, STAT_STR),
                 ch->perm_stat[STAT_INT], get_curr_stat(ch, STAT_INT),
                 ch->perm_stat[STAT_WIS], get_curr_stat(ch, STAT_WIS),
                 ch->perm_stat[STAT_DEX], get_curr_stat(ch, STAT_DEX),
                 ch->perm_stat[STAT_CON], get_curr_stat(ch, STAT_CON));
  if (ch->level >= 15)
  {
    printf_to_char(ch,
                   "{cToHit: {W%-9d                            {cToDam: {W%d\n\r",
                   GET_HITROLL(ch), GET_DAMROLL(ch));
  }
  printf_to_char(ch,
                 "{cHP: {W%5ld/%-5ld                             {cPracs: {W%d \n\r",
                 ch->hit, ch->max_hit, ch->practice);
  printf_to_char(ch,
                 "{cMANA: {W%5ld/%-5ld                           {cTrains: {W%d \n\r",
                 ch->mana, ch->max_mana, ch->train);
  printf_to_char(ch,
                 "{cMOVE: {W%5ld/%-5ld                           {cItems: {W%d/%d \n\r",
                 ch->move, ch->max_move, ch->carry_number, can_carry_n(ch));
  printf_to_char(ch,
                 "{cAQP: {W%-9ld                            {cWeight: {W%ld/%d \n\r",
                 ch->pcdata->questpoints, get_carry_weight(ch) / 10,
                 can_carry_w(ch) / 10);
  /* 
   * This section (exp, gold, balance and shares) was written by
   * The Maniac from Mythran Mud
   */
  if (IS_NPC(ch))               /* NPC's have no bank balance and shares */
  {                             /* and don't level !! -- Maniac -- */
    printf_to_char(ch,
                   "You have scored %ld exp, and have %ld gold coins\n\r",
                   ch->exp, ch->gold);
  }

  if (!IS_NPC(ch))              /* PC's do level and can have bank accounts */
  {                             /* HERO's don't level anymore */
    printf_to_char(ch,
                   "{cExp: {W%-6ld       {cTNL: {W%-7ld       {cWimpy: {W%d\n\r",
                   ch->exp,
                   IS_HERO(ch) ? (1) : (ch->level +
                                        1) * exp_per_level(ch,
                                                           ch->
                                                           pcdata->
                                                           points)
                   - ch->exp, ch->wimpy);
    printf_to_char(ch,
                   "{cGold: {W%-6ld  {cPlat: {W%-7ld       {cBank: {W%-7ld      {cSilver: {W%ld\n\r",
                   ch->gold, ch->platinum, ch->pcdata->balance, ch->silver);
    if (ch->pcdata->shares)
    {
      printf_to_char(ch,
                     "You have %ld plat invested in %ld shares (%d each).\n\r",
                     (ch->pcdata->shares * share_value),
                     ch->pcdata->shares, share_value);
    }
  }

  send_to_char("{cCONDITION(S): ", ch);
  if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
    send_to_char("{WDRUNK ", ch);
  if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] == 0)
    send_to_char("{WTHIRSTY ", ch);
  if (!IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] == 0)
    send_to_char("{WHUNGRY{x", ch);
  send_to_char(".\n\r", ch);
  send_to_char("{cPOSITION: ", ch);
  switch (ch->position)
  {
    case POS_DEAD:
      send_to_char("{RDEAD{x!!\n\r", ch);
      break;
    case POS_MORTAL:
      send_to_char("{WMortally wounded{x.\n\r", ch);
      break;
    case POS_INCAP:
      send_to_char("{WIncapacitated{x.\n\r", ch);
      break;
    case POS_STUNNED:
      send_to_char("{Wstunned{x.\n\r", ch);
      break;
    case POS_SLEEPING:
      send_to_char("{WSleeping{x.\n\r", ch);
      break;
    case POS_RESTING:
      send_to_char("{WResting{x.\n\r", ch);
      break;
    case POS_SITTING:
      send_to_char("{WSitting{x.\n\r", ch);
      break;
    case POS_STANDING:
      send_to_char("{WStanding{x.\n\r", ch);
      break;
    case POS_FIGHTING:
      send_to_char("{WFighting{x.\n\r", ch);
      break;
  }
  /* print AC values */
  if (ch->level >= 25)
  {
    printf_to_char(ch,
                   "{cArmor: pierce: {W%d  {cbash: {W%d  {cslash: {W%d  {cmagic: {W%d\n\r",
                   GET_AC(ch, AC_PIERCE), GET_AC(ch, AC_BASH),
                   GET_AC(ch, AC_SLASH), GET_AC(ch, AC_EXOTIC));
  }
  else
  {
    for (i = 0; i < 4; i++)
    {
      char *temp;

      switch (i)
      {
        case (AC_PIERCE):
          temp = "piercing";
          break;
        case (AC_BASH):
          temp = "bashing";
          break;
        case (AC_SLASH):
          temp = "slashing";
          break;
        case (AC_EXOTIC):
          temp = "magic";
          break;
        default:
          temp = "error";
          break;
      }

      send_to_char("You are ", ch);
      if (GET_AC(ch, i) >= 101)
        sprintf(buf, "hopelessly vulnerable to %s.\n\r", temp);
      else if (GET_AC(ch, i) >= 80)
        sprintf(buf, "defenseless against %s.\n\r", temp);
      else if (GET_AC(ch, i) >= 60)
        sprintf(buf, "barely protected from %s.\n\r", temp);
      else if (GET_AC(ch, i) >= 40)
        sprintf(buf, "slightly armored against %s.\n\r", temp);
      else if (GET_AC(ch, i) >= 20)
        sprintf(buf, "somewhat armored against %s.\n\r", temp);
      else if (GET_AC(ch, i) >= 0)
        sprintf(buf, "armored against %s.\n\r", temp);
      else if (GET_AC(ch, i) >= -20)
        sprintf(buf, "well-armored against %s.\n\r", temp);
      else if (GET_AC(ch, i) >= -40)
        sprintf(buf, "very well-armored against %s.\n\r", temp);
      else if (GET_AC(ch, i) >= -60)
        sprintf(buf, "heavily armored against %s.\n\r", temp);
      else if (GET_AC(ch, i) >= -80)
        sprintf(buf, "superbly armored against %s.\n\r", temp);
      else if (GET_AC(ch, i) >= -100)
        sprintf(buf, "almost invulnerable to %s.\n\r", temp);
      else
        sprintf(buf, "divinely armored against %s.\n\r", temp);
      send_to_char(buf, ch);
    }
  }

  /* RT wizinvis and holy light */
  if (IS_IMMORTAL(ch))
  {
    send_to_char("{cHoly Light: ", ch);
    if (IS_SET(ch->act, PLR_HOLYLIGHT))
      send_to_char("{WON", ch);
    else
      send_to_char("{WOFF", ch);
    if (ch->invis_level)
    {
      sprintf(buf, " {c Invisible: level {W%d", ch->invis_level);
      send_to_char(buf, ch);
    }

    if (ch->incog_level)
    {
      sprintf(buf, "  {cIncognito: level {W%d", ch->incog_level);
      send_to_char(buf, ch);
    }
    send_to_char("\n\r", ch);
  }

  if (ch->level >= 10)
  {
    printf_to_char(ch, "{cAlignment: {W%d.  ", ch->alignment);
  }

  send_to_char("You are ", ch);
  if (ch->alignment > 900)
    send_to_char("angelic.\n\r", ch);
  else if (ch->alignment > 700)
    send_to_char("saintly.\n\r", ch);
  else if (ch->alignment > 350)
    send_to_char("good.\n\r", ch);
  else if (ch->alignment > 100)
    send_to_char("kind.\n\r", ch);
  else if (ch->alignment > -100)
    send_to_char("neutral.\n\r", ch);
  else if (ch->alignment > -350)
    send_to_char("mean.\n\r", ch);
  else if (ch->alignment > -700)
    send_to_char("evil.\n\r", ch);
  else if (ch->alignment > -900)
    send_to_char("demonic.\n\r", ch);
  else
    send_to_char("satanic.\n\r", ch);
  send_to_char
    ("{Y------------------------------------------------------------------{x\n\r",
     ch);
  if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
  {
    do_affects(ch, "");
    send_to_char
      ("{Y------------------------------------------------------------------{x\n\r",
       ch);
  }
}

CH_CMD(do_score_new)
{
  char buf[MSL];
  BUFFER *output;

  /* int i; */
  output = new_buf();
  sprintf(buf, "\n\r");
  add_buf(output, buf);
  sprintf(buf, "          %s: %s\n\r", ch->name,
          IS_NPC(ch) ? ", the mobile." : ch->pcdata->title);
  add_buf(output, buf);
  sprintf(buf, "\n\r");
  add_buf(output, buf);
  sprintf(buf, "     Level: %-14dAge: %-14dSex: %-10s\n\r", ch->level,
          get_age(ch),
          ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female");
  add_buf(output, buf);
  sprintf(buf, "      Race: %-28s   Class: %-10s\n\r",
          race_table[ch->race].name, class_table[ch->class].name);
  add_buf(output, buf);
  sprintf(buf, "  Platinum: %-13ldGold: %-11ldSilver: %-12ld\n\r",
          ch->platinum, ch->gold, ch->silver);
  add_buf(output, buf);
  sprintf(buf, "        HP: %ld/%ld  Mana: %ld/%ld    MV: %ld/%ld\n\r",
          ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
          ch->max_move);
  add_buf(output, buf);
  sprintf(buf, "   Hitroll: %-12dWimpy: %-10dDamroll: %-11d\n\r",
          GET_HITROLL(ch), ch->wimpy, GET_DAMROLL(ch));
  add_buf(output, buf);
  sprintf(buf, "       AQP: %-32ldImmQP: %-13d\n\r",
          ch->pcdata->questpoints, ch->qps);
  add_buf(output, buf);
  page_to_char(buf_string(output), ch);
  free_buf(output);
  if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
    do_affects(ch, "");
}

CH_CMD(do_score)
{
  char buf[MSL];
  BUFFER *output;
  int i;

  output = new_buf();
  sprintf(buf, "{WYou are {G%s{x%s{x\n\r", ch->name,
          IS_NPC(ch) ? ", the mobile." : ch->pcdata->title);
  add_buf(output, buf);
  sprintf(buf, "{WLevel {B%d{W,  {B%d{W years old.{x\n\r", ch->level,
          get_age(ch));
  add_buf(output, buf);
  if (get_trust(ch) != ch->level)
  {
    sprintf(buf, "{WYou are trusted at level {B%d{x.\n\r", get_trust(ch));
    add_buf(output, buf);
  }

  sprintf(buf, "{WRace{x: {M%s{W  Sex{x: {M%s{W  Class{x:  {M%s{x\n\r",
          race_table[ch->race].name,
          ch->sex == 0 ? "sexless" : ch->sex == 1 ? "male" : "female",
          IS_NPC(ch) ? "mobile" : class_table[ch->class].name);
  add_buf(output, buf);
  sprintf(buf,
          "{WYou have {r%ld{W/{R%ld{W hitpoints, {b%ld{W/{B%ld{W mana, {y%ld{W/{Y%ld{W movement.{x\n\r",
          ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
          ch->max_move);
  add_buf(output, buf);
  sprintf(buf,
          "{WYou have {y%d{W practices and {y%d{W training sessions.{x\n\r",
          ch->practice, ch->train);
  add_buf(output, buf);
  /* if (is_clan(ch)) { sprintf( buf, "{xYou are in the [{G%s{x] clan, which
     is a %s clan.\n\r", clan_table[ch->clan].who_name, is_pkill(ch) ?
     "{RPkill{x" : "{WNon-Pkill{x"); add_buf(output,buf); } */
  sprintf(buf,
          "{WYou are carrying {g%d{x/{G%d{W items with weight {g%ld{x/{G%d{W pounds.{x\n\r",
          ch->carry_number, can_carry_n(ch),
          get_carry_weight(ch) / 10, can_carry_w(ch) / 10);
  add_buf(output, buf);
  sprintf(buf,
          "{WStr{x: {Y%d{x({D%d{x)  {WInt{x: {Y%d{x({D%d{x)  {WWis{x: {Y%d{x({D%d{x){W Dex{x: {Y%d{x({D%d{x){W  Con{x: {Y%d{x({D%d{x)\n\r",
          ch->perm_stat[STAT_STR], get_curr_stat(ch, STAT_STR),
          ch->perm_stat[STAT_INT], get_curr_stat(ch, STAT_INT),
          ch->perm_stat[STAT_WIS], get_curr_stat(ch, STAT_WIS),
          ch->perm_stat[STAT_DEX], get_curr_stat(ch, STAT_DEX),
          ch->perm_stat[STAT_CON], get_curr_stat(ch, STAT_CON));
  add_buf(output, buf);
  sprintf(buf,
          "{WYou have {Y%ld{W platinum, {Y%ld{W gold and {Y%ld{W silver coins.\n\r",
          ch->platinum, ch->gold, ch->silver);
  add_buf(output, buf);
  if (!IS_NPC(ch) && ch->level == LEVEL_ANCIENT)
  {
    sprintf(buf, "{WYou have scored {c%ld exp{x.\n\r", ch->exp);
    add_buf(output, buf);
  }
  else if (!IS_NPC(ch) && ch->level < LEVEL_ANCIENT)
  {
    sprintf(buf,
            "{WYou have scored {c%ld exp{W. You need {C%ld exp{W to level.{x\n\r",
            ch->exp,
            ((ch->level + 1) * exp_per_level(ch,
                                             ch->pcdata->points) - ch->exp));
    add_buf(output, buf);
  }

  if (ch->wimpy)
  {
    sprintf(buf, "{WWimpy set to{r %d{W hit points.{x\n\r", ch->wimpy);
    add_buf(output, buf);
  }

  if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
  {
    sprintf(buf, "{cYou are drunk.{x\n\r");
    add_buf(output, buf);
  }
  if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] == 0)
  {
    sprintf(buf, "{cYou are thirsty.{x\n\r");
    add_buf(output, buf);
  }
  if (!IS_NPC(ch) && ch->pcdata->condition[COND_HUNGER] == 0)
  {
    sprintf(buf, "{cYou are hungry.{x\n\r");
    add_buf(output, buf);
  }

  switch (ch->position)
  {
    case POS_DEAD:
      sprintf(buf, "{RYou are DEAD!!{x\n\r");
      add_buf(output, buf);
      break;
    case POS_MORTAL:
      sprintf(buf, "{RYou are mortally wounded.{x\n\r");
      add_buf(output, buf);
      break;
    case POS_INCAP:
      sprintf(buf, "{RYou are incapacitated.{x\n\r");
      add_buf(output, buf);
      break;
    case POS_STUNNED:
      sprintf(buf, "{RYou are stunned.{x\n\r");
      add_buf(output, buf);
      break;
    case POS_SLEEPING:
      sprintf(buf, "{BYou are sleeping.{x\n\r");
      add_buf(output, buf);
      break;
    case POS_RESTING:
      sprintf(buf, "{BYou are resting.{x\n\r");
      add_buf(output, buf);
      break;
    case POS_STANDING:
      sprintf(buf, "{BYou are standing.{x\n\r");
      add_buf(output, buf);
      break;
    case POS_FIGHTING:
      sprintf(buf, "{RYou are fighting.{x\n\r");
      add_buf(output, buf);
      break;
  }

  /* print AC values */
  if (ch->level >= 25)
  {
    sprintf(buf,
            "{WArmor{x:{W Pierce{x: {D%d{W  Bash{x: {D%d{W  Slash{x: {D%d{W Magic{x: {M%d{x\n\r",
            GET_AC(ch, AC_PIERCE), GET_AC(ch, AC_BASH), GET_AC(ch,
                                                               AC_SLASH),
            GET_AC(ch, AC_EXOTIC));
    add_buf(output, buf);
  }

  for (i = 0; i < 4; i++)
  {
    char *temp;

    switch (i)
    {
      case (AC_PIERCE):
        temp = "piercing";
        break;
      case (AC_BASH):
        temp = "bashing";
        break;
      case (AC_SLASH):
        temp = "slashing";
        break;
      case (AC_EXOTIC):
        temp = "magic";
        break;
      default:
        temp = "error";
        break;
    }

    sprintf(buf, "{xYou are ");
    add_buf(output, buf);
    if (GET_AC(ch, i) >= 101)
      sprintf(buf, "{Rhopelessly vulnerable{x to %s.\n\r", temp);
    else if (GET_AC(ch, i) >= 80)
      sprintf(buf, "{Rdefenseless{x against %s.\n\r", temp);
    else if (GET_AC(ch, i) >= 60)
      sprintf(buf, "{Rbarely protected{x from %s.\n\r", temp);
    else if (GET_AC(ch, i) >= 40)
      sprintf(buf, "{yslightly armored{x against %s.\n\r", temp);
    else if (GET_AC(ch, i) >= 20)
      sprintf(buf, "{ysomewhat armored{x against %s.\n\r", temp);
    else if (GET_AC(ch, i) >= 0)
      sprintf(buf, "{yarmored{x against %s.\n\r", temp);
    else if (GET_AC(ch, i) >= -20)
      sprintf(buf, "{ywell-armored{x against %s.\n\r", temp);
    else if (GET_AC(ch, i) >= -40)
      sprintf(buf, "{yvery well-armored{x against %s.\n\r", temp);
    else if (GET_AC(ch, i) >= -60)
      sprintf(buf, "{Bheavily armored{x against %s.\n\r", temp);
    else if (GET_AC(ch, i) >= -80)
      sprintf(buf, "{Bsuperbly armored{x against %s.\n\r", temp);
    else if (GET_AC(ch, i) >= -100)
      sprintf(buf, "{Balmost invulnerable{x to %s.\n\r", temp);
    else
      sprintf(buf, "{Wdivinely armored{x against %s.\n\r", temp);
    add_buf(output, buf);
  }

  /* RT wizinvis and holy light */
  if (IS_IMMORTAL(ch))
  {
    sprintf(buf, "Holy Light: ");
    add_buf(output, buf);
    if (IS_SET(ch->act, PLR_HOLYLIGHT))
      sprintf(buf, "on");
    else
      sprintf(buf, "off");
    add_buf(output, buf);
    if (ch->invis_level)
    {
      sprintf(buf, "  Invisible: level %d", ch->invis_level);
      add_buf(output, buf);
    }

    if (ch->incog_level)
    {
      sprintf(buf, "  Incognito: level %d", ch->incog_level);
      add_buf(output, buf);
    }
    sprintf(buf, "\n\r");
    add_buf(output, buf);
  }

  if (ch->level >= 15)
  {
    sprintf(buf, "{WHitroll: {G%d{W  Damroll: {G%d{x.\n\r",
            GET_HITROLL(ch), GET_DAMROLL(ch));
    add_buf(output, buf);
  }

  if (is_clan(ch))
  {
    sprintf(buf,
            "{YYou are in the {w[%s{w]{Y clan, which is a %s clan.{x\n\r",
            clan_table[ch->clan].who_name,
            is_pkill(ch) ? "{RPkill{x" : "{WNon-Pkill{x");
    add_buf(output, buf);
  }

  if (!IS_NPC(ch))
  {
    sprintf(buf,
            "{w[{RPK {gstats{w] {WKills:{x {R%ld{W  Deaths: {r%ld{x\n\r",
            ch->pcdata->pkkills, ch->pcdata->pkdeaths);
    add_buf(output, buf);
  }

  if (ch->level >= 10)
  {
    sprintf(buf, "{xAlignment: {B%d{x.  ", ch->alignment);
    add_buf(output, buf);
  }

  sprintf(buf, "{xYou are ");
  add_buf(output, buf);
  if (ch->alignment > 900)
    sprintf(buf, "{Wangelic{x.\n\r");
  else if (ch->alignment > 700)
    sprintf(buf, "{Wsaintly{x.\n\r");
  else if (ch->alignment > 350)
    sprintf(buf, "{wgood{x.\n\r");
  else if (ch->alignment > 100)
    sprintf(buf, "{ykind.\n\r");
  else if (ch->alignment > -100)
    sprintf(buf, "{yneutral.\n\r");
  else if (ch->alignment > -350)
    sprintf(buf, "{ymean.\n\r");
  else if (ch->alignment > -700)
    sprintf(buf, "{revil{x.\n\r");
  else if (ch->alignment > -900)
    sprintf(buf, "{Rdemonic{x.\n\r");
  else
    sprintf(buf, "{Rsatanic{x.\n\r");
  add_buf(output, buf);
  if (ch->qps)
  {
    if (ch->qps == 1)
      sprintf(buf, "{WYou have {M%d{W quest point.{x\n\r", ch->qps);
    else
      sprintf(buf, "{WYou have {M%d{W quest points.{x\n\r", ch->qps);
    add_buf(output, buf);
  }
  if (ch->invited)
  {
    sprintf(buf, "{RYou have been invited to join clan {x[{%s%s{x]\n\r",
            clan_table[ch->invited].pkill ? "B" : "M",
            clan_table[ch->invited].who_name);
    add_buf(output, buf);
  }
  page_to_char(buf_string(output), ch);
  free_buf(output);
  if (IS_SET(ch->comm, COMM_SHOW_AFFECTS))
    do_affects(ch, "");
}

CH_CMD(do_affects)
{
  AFFECT_DATA *paf, *paf_last = NULL;
  char buf[MSL];
  char *buf4;
  char buf3[MSL];
  char buf2[MSL];
  bool found = false;
  long cheat = 0;
  long filter;
  long printme;
  BUFFER *buffer;
  OBJ_DATA *obj;
  int iWear;

  buffer = new_buf();
  cheat = ch->affected_by;
  if (ch->affected != NULL)
  {
    add_buf(buffer, "{cYou are affected by the following spells:{x\n\r");
    for (paf = ch->affected; paf != NULL; paf = paf->next)
    {
      if (paf_last != NULL && paf->type == paf_last->type)
      {
        if (ch->level >= 20)
          sprintf(buf, "                          ");
        else
          continue;
      }
      else
        sprintf(buf, "{cSpell: {g%-19s{x", skill_table[paf->type].name);
      add_buf(buffer, buf);
      if (IS_SET(cheat, paf->bitvector))
        cheat -= paf->bitvector;
      if (ch->level >= 20)
      {
        sprintf(buf, "{c: modifies {g%-16s {cby {g%-6d{x ",
                affect_loc_name(paf->location), paf->modifier);
        add_buf(buffer, buf);
        if (paf->duration == -1)
          sprintf(buf, "{cpermanently{x");
        else
          sprintf(buf, "{cfor{g %-4d {chours{x", paf->duration);
        add_buf(buffer, buf);
      }

      add_buf(buffer, "\n\r");
      paf_last = paf;
    }
    found = true;
    add_buf(buffer, "\n\r");
  }
  if (race_table[ch->race].aff != 0 &&
      IS_AFFECTED(ch, race_table[ch->race].aff))
  {
    add_buf(buffer,
            "{Y------------------------------------------------------------------------------{x\n\r");
    add_buf(buffer,
            "{cYou are affected by the following racial affects:{x\n\r");
    if (IS_SET(cheat, race_table[ch->race].aff));
    cheat -= race_table[ch->race].aff;
    strcpy(buf3, affect_bit_name(race_table[ch->race].aff));
    buf4 = buf3;
    buf4 = one_argument(buf4, buf2);
    while (buf2[0])
    {
      sprintf(buf, "{cSpell: {g%-19s{x", buf2);
      add_buf(buffer, buf);
      add_buf(buffer, "\n\r");
      buf4 = one_argument(buf4, buf2);
    }
    found = true;
    add_buf(buffer, "\n\r");
  }
  if (ch->affected_by != 0 && (ch->affected_by != race_table[ch->race].aff))
  {
    bool print = false;

    for (iWear = 0; iWear < MAX_WEAR; iWear++)
    {
      if ((obj = get_eq_char(ch, iWear)) != NULL)
      {
        for (paf = obj->affected; paf != NULL; paf = paf->next)
        {
          if (!IS_SET(ch->affected_by, paf->bitvector))
            continue;
          if (paf->where != TO_AFFECTS)
            continue;
          filter = paf->bitvector;
          filter &= ch->affected_by;
          printme = filter;
          filter &= cheat;
          cheat -= filter;
          if (!print)
          {
            add_buf(buffer,
                    "{Y------------------------------------------------------------------------------{x\n\r");
            add_buf(buffer,
                    "{cYou are affected by the following equipment spells:{x\n\r");
            print = true;
          }

          strcpy(buf3, affect_bit_name(printme));
          buf4 = buf3;
          buf4 = one_argument(buf4, buf2);
          while (buf2[0])
          {
            sprintf(buf, "{cSpell: {g%-19s{c:{x %s", buf2, obj->short_descr);
            add_buf(buffer, buf);
            add_buf(buffer, "\n\r");
            buf4 = one_argument(buf4, buf2);
          }
        }
        if (!obj->enchanted)
        {
          for (paf = obj->pIndexData->affected; paf != NULL; paf = paf->next)
          {
            if (!IS_SET(ch->affected_by, paf->bitvector))
              continue;
            if (paf->where != TO_AFFECTS)
              continue;
            filter = paf->bitvector;
            filter &= ch->affected_by;
            printme = filter;
            filter &= cheat;
            cheat -= filter;
            if (!print)
            {
              add_buf(buffer,
                      "{cYou are affected by the following equipment spells:{x\n\r");
              print = true;
            }

            strcpy(buf3, affect_bit_name(printme));
            buf4 = buf3;
            buf4 = one_argument(buf4, buf2);
            while (buf2[0])
            {
              sprintf(buf, "{cSpell: {g%-19s:{x %s", buf2, obj->short_descr);
              add_buf(buffer, buf);
              add_buf(buffer, "\n\r");
              buf4 = one_argument(buf4, buf2);
            }
          }
        }
      }
    }

    if (print)
    {
      add_buf(buffer, "\n\r");
      found = true;
    }
  }

  if (!found)
    send_to_char("{cYou are not affected by any spells.{x\n\r", ch);
  else
  {
    page_to_char(buf_string(buffer), ch);
    free_buf(buffer);
  }
  return;
}

char *const day_name[] = {
  "the Moon",
  "the Bull",
  "Deception",
  "Thunder",
  "Freedom",
  "the Great Gods",
  "the Sun"
};
char *const month_name[] = {
  "Winter",
  "the Winter Wolf",
  "the Frost Giant",
  "the Old Forces",
  "the Grand Struggle",
  "the Spring",
  "Nature",
  "Futility",
  "the Dragon",
  "the Sun",
  "the Heat",
  "the Battle",
  "the Dark Shades",
  "the Shadows",
  "the Long Shadows",
  "the Ancient Darkness",
  "the Great Evil"
};

CH_CMD(do_time)
{
  char buf[MSL];
  char *suf;
  int day;

  day = time_info.day + 1;
  if (day > 4 && day < 20)
    suf = "th";
  else if (day % 10 == 1)
    suf = "st";
  else if (day % 10 == 2)
    suf = "nd";
  else if (day % 10 == 3)
    suf = "rd";
  else
    suf = "th";
  sprintf(buf,
          "{cIt is {W%d{c o'clock {W%s{c, Day of {W%s{c,{W %d%s{c the Month of{W %s{c.{x\n\r",
          (time_info.hour % 12 == 0) ? 12 : time_info.hour % 12,
          time_info.hour >= 12 ? "pm" : "am", day_name[day % 7], day, suf,
          month_name[time_info.month]);
  send_to_char(buf, ch);
  sprintf(buf,
          "{cIllusions of Grandeur started up at {W%s{x\n\r{cThe system time is {W%s{x\n\r",
          str_boot_time, (char *) ctime(&current_time));
  send_to_char(buf, ch);
  return;
}

CH_CMD(do_weather)
{
  char buf[MSL];
  static char *const sky_look[4] = {
    "cloudless",
    "cloudy",
    "rainy",
    "lit by flashes of lightning"
  };

  if (!IS_OUTSIDE(ch))
  {
    send_to_char("You can't see the weather indoors.\n\r", ch);
    return;
  }

  sprintf(buf, "The sky is %s and %s.\n\r", sky_look[weather_info.sky],
          weather_info.change >=
          0 ? "a warm southerly breeze blows" : "a cold northern gust blows");
  send_to_char(buf, ch);
  return;
}

CH_CMD(do_help)
{
  sqlite3_stmt *stmt;
  char *sql;
  const char *tail;
  int rc;

  int level;
  char *keyword;
  char *htext;

  char argall[MAX_INPUT_LENGTH], argone[MAX_INPUT_LENGTH];
  char buf[MSL], part[MSL];
  char nohelp[MSL];
  char *output;
  int skill;
  bool fRegular = false;

  if (argument[0] == '\0')
    argument = "summary";
  strcpy(nohelp, argument);
  /* this parts handles help a b so that it returns help 'a b' */
  argall[0] = '\0';
  while (argument[0] != '\0')
  {
    argument = one_argument(argument, argone);
    if (argall[0] != '\0')
      strcat(argall, " ");
    strcat(argall, argone);
  }

  sql =
    sqlite3_mprintf
    ("SELECT id,level,keyword,htext FROM helps WHERE keyword LIKE '%%%q%%' AND level <= %d",
     argall, get_trust(ch));
  rc = sqlite3_prepare(world_db, sql, strlen(sql), &stmt, &tail);

  if (rc != SQLITE_OK)
  {
    sprintf(log_buf, "SQL error: %s", sqlite3_errmsg(world_db));
    log_string(log_buf);
    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    send_to_char("Could not access help files. Check back later.", ch);
    return;
  }

  rc = sqlite3_step(stmt);

  while (rc == SQLITE_ROW)
  {
    level = sqlite3_column_int(stmt, 1);
    keyword = (char *) sqlite3_column_text(stmt, 2);
    htext = (char *) sqlite3_column_text(stmt, 3);

    if (is_name(argall, keyword))
    {
      if (level >= 0)
      {
        one_argument(keyword, part);
        if ((skill = skill_lookup(part)) != -1)
        {
          if (skill_table[skill].spell_fun == spell_null)
          {
            send_to_char("{D<{GS{gkill{D>{x", ch);
          }
          else
          {
            int gn, sn;

            send_to_char("{D<{GS{gpell{D>{x", ch);
            for (gn = 0; gn < MAX_GROUP; gn++)
              for (sn = 0; sn < MAX_IN_GROUP; sn++)
              {
                if (group_table[gn].spells[sn] == NULL)
                  break;
                if (!str_cmp(part, group_table[gn].spells[sn]))
                {
                  sprintf(buf,
                          "                  {D<{GG{group{D: {G%s{D>{x",
                          capitalize(group_table[gn].name));
                  send_to_char(buf, ch);
                }
              }
          }
        }
        else
        {
          send_to_char(keyword, ch);
        }
        send_to_char("\n\r", ch);
        fRegular = true;
      }
      else if (level != -2)
      {
        send_to_char("{D<{GG{general {Gi{gnformation{D>{x\n\r", ch);
        fRegular = true;
      }
      output = malloc(strlen(htext) + 200);
      strcpy(output, "");
      /* 
       * Strip leading '.' to allow initial blanks.
       */
      if (htext[0] == '.')
        strcat(output, htext + 1);
      else
        strcat(output, htext);
      if (fRegular)
        send_to_char(output, ch);
      free(output);
      sqlite3_free(sql);
      sqlite3_finalize(stmt);
      return;
    }

    rc = sqlite3_step(stmt);
  }


  send_to_char("No help on that word.\n\rMissing help file logged.\n\r", ch);
  append_file(ch, HELP_FILE, nohelp);
  sprintf(buf, "%s tried 'help %s'. Error: {D({wNon-Existent{D){w\n\r",
          ch->name, nohelp);
  wiznet(buf, NULL, NULL, WIZ_HELP, 0, 0);
  return;
}


/* whois command */
CH_CMD(do_whois)
{
  char arg[MAX_INPUT_LENGTH];
  BUFFER *output;
  char buf[MSL];
  char buf2[MSL];
  char sexbuf[MSL];
  char pkbuf[MSL];
  char idletbuf[MSL];
  char clandat[MSL];
  DESCRIPTOR_DATA *d;
  bool found = false;

  if (IS_AFFECTED(ch, AFF_BLIND))
  {
    send_to_char("Lets see here.. Ahh! Darkness is online.\n\r", ch);
    return;
  }

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("You must provide a name.\n\r", ch);
    return;
  }

  output = new_buf();
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *wch;
    char class[MSL];

    if (d->connected != CON_PLAYING || !can_see(ch, d->character))
      continue;
    wch = (d->original != NULL) ? d->original : d->character;
    if (!can_see(ch, wch))
      continue;
    if (!str_prefix(arg, wch->name))
    {
      found = true;
      /* work out the printing */
      sprintf(buf2, "%3d", wch->level);
      switch (wch->level)
      {
        case MAX_LEVEL - 0:
          sprintf(buf2, "{CGOD{x");
          break;
        case MAX_LEVEL - 1:
          sprintf(buf2, "{GCRE{x");
          break;
        case MAX_LEVEL - 2:
          sprintf(buf2, "{GSUP{x");
          break;
        case MAX_LEVEL - 3:
          sprintf(buf2, "{GDEI{x");
          break;
        case MAX_LEVEL - 4:
          sprintf(buf2, "{GGOD{x");

          break;
        case MAX_LEVEL - 5:
          sprintf(buf2, "{GIMM{x");
          break;
        case MAX_LEVEL - 6:
          sprintf(buf2, "{GDEM{x");
          break;
        case MAX_LEVEL - 7:
          sprintf(buf2, "{CKNI{x");
          break;
        case MAX_LEVEL - 8:
          sprintf(buf2, "{CSQU{x");
          break;
        case MAX_LEVEL - 9:
          sprintf(buf2, "{RA{rnc{x");
          break;
        case MAX_LEVEL - 10:
          sprintf(buf2, "{CH{cro{x");
          break;
      }

      if (wch->sex == 1)
      {
        sprintf(sexbuf, "{D[{x%s{D]{x ", "{CM");
      }
      else if (wch->sex == 2)
      {
        sprintf(sexbuf, "{D[{x%s{D]{x ", "{MF");
      }
      else
      {
        sprintf(sexbuf, "{D[{x%s{D]{x ", "{WN");
      }
      if (wch->timer > 3 && (!IS_IMMORTAL(wch) || IS_IMMORTAL(ch)))
      {
        if (wch->timer < 60)
          sprintf(idletbuf, " {D[{widle {W%d{w mins{D]{x",
                  ((wch->timer) - 1));

        if (wch->timer < 3600 && wch->timer >= 60)
          sprintf(idletbuf, " {D[{widle {W%d{w hours{D]{x",
                  ((wch->timer / 60) - 1));
      }
      else
      {
        sprintf(idletbuf, "{x");
      }

      if (WR(ch, wch) && !IS_NPC(wch) && !IS_IMMORTAL(ch) &&
          !IS_IMMORTAL(wch))
      {
        sprintf(pkbuf, "{D[{GWR{D]{x");
      }
      else
      {
        sprintf(pkbuf, "{x");
      }

      if (IS_SET(class_table[wch->class].tier, TIER_03))
      {
        sprintf(class, "{R%c{r%c%c{x",
                class_table[wch->class].who_name[0],
                class_table[wch->class].who_name[1],
                class_table[wch->class].who_name[2]);
      }
      else if (IS_SET(class_table[wch->class].tier, TIER_02))
      {
        sprintf(class, "{Y%c{y%c%c{x",
                class_table[wch->class].who_name[0],
                class_table[wch->class].who_name[1],
                class_table[wch->class].who_name[2]);
      }
      else if (IS_SET(class_table[wch->class].tier, TIER_04))
      {
        sprintf(class, "{B%c{b%c%c{x",
                class_table[wch->class].who_name[0],
                class_table[wch->class].who_name[1],
                class_table[wch->class].who_name[2]);
      }
      else
      {
        sprintf(class, "{G%c{g%c%c{x",
                class_table[wch->class].who_name[0],
                class_table[wch->class].who_name[1],
                class_table[wch->class].who_name[2]);
      }

      /* a little formatting */
      if (!is_clan(wch))
      {
        sprintf(clandat, "%s", clan_table[wch->clan].who_name);
      }
      else
      {
        if (clan_table[wch->clan].independent)
        {
          sprintf(clandat, "{W[%s{W]{x ", clan_table[wch->clan].who_name);
        }
        else if (is_clead(wch) && is_pkill(wch))
        {
          sprintf(clandat, "{R[{W<{GL{W>{x%s{R]{x ",
                  clan_table[wch->clan].who_name);
        }
        else if (is_clead(wch) && !is_pkill(wch))
        {
          sprintf(clandat, "{M[{W<{GL{W>{x%s{M]{x ",
                  clan_table[wch->clan].who_name);
        }
        else if (is_pkill(wch))
        {
          sprintf(clandat, "{R[{x%s{R]{x ", clan_table[wch->clan].who_name);
        }
        else
        {
          sprintf(clandat, "{M[%s{M]{x ", clan_table[wch->clan].who_name);
        }
      }
      sprintf(buf,
              "{c<{C<{x%s%s %-6s %s{C>{c>{x %s%s%s%s%s%s%s%s%s%s%s\n\r",
              sexbuf, buf2,
              wch->race <
              MAX_PC_RACE ? pc_race_table[wch->race].who_name : "     ",
              class,
              //                ON_GQUEST(wch) ? "(GQuest)" : "",
              ((wch->ghost_level >= LEVEL_ANCIENT) &&
               (ch->level >= wch->level)) ? "(Ghost) " : "",
              wch->incog_level >= LEVEL_ANCIENT ? "(Incog) " : "",
              wch->invis_level >= LEVEL_ANCIENT ? "(Wizi) " : "",
              clandat, idletbuf, pkbuf, IS_SET(wch->act2,
                                               PLR_RP) ?
              "{D[{RRP{D]{x " : "", IS_SET(wch->comm,
                                           COMM_AFK) ? "[{yAFK{x] " :
              "", IS_SET(wch->act, PLR_TWIT) ? "({RTWIT{x) " : "",
              wch->name, IS_NPC(wch) ? "" : wch->pcdata->title);
      add_buf(output, buf);
    }
  }

  if (!found)
  {
    send_to_char("No one of that name is playing.\n\r", ch);
    return;
  }

  page_to_char(buf_string(output), ch);
  free_buf(output);
}

/*
 * New 'who' command originally by Alander of Rivers of Mud.
 */
CH_CMD(do_who)
{
  int thour, tday, tmonth;
  char buf[MSL];
  char invisbuf[MSL];
  char buf2[MSL];
  char whon[MSL];
  char abuf[MSL];
  char utbuf[MSL];
  char sexbuf[MSL];
  char pkbuf[MSL];
  char afkbuf[MSL];
  char clandat[MSL];
  char questdat[MSL];
  char sepbuf[MSL];
  DESCRIPTOR_DATA *d;
  int iClass;
  int iRace;
  int iClan;
  int iLevelLower;
  int iLevelUpper;
  int nNumber;
  int nMatch;
  int count;
  int countimm;
  int countmort;
  int countnpk;
  int countpk;
  int ccount;
  int hour;
  int wlevel;
  int itog;
  int ctog;
  bool rgfClass[MAX_CLASS];
  bool rgfRace[MAX_PC_RACE];
  bool rgfClan[MAX_CLAN];
  bool fClassRestrict = false;
  bool fClanRestrict = false;
  bool fClan = false;
  bool fRaceRestrict = false;
  bool fImmortalOnly = false;
  bool wr_only = false;

  iClass = 0;
  iRace = 0;
  iClan = 0;
  iLevelUpper = MAX_LEVEL;
  iLevelLower = 0;
  nNumber = 0;
  nMatch = 0;
  count = 0;
  countimm = 0;
  countmort = 0;
  countnpk = 0;
  countpk = 0;
  ccount = 0;
  hour = 0;
  wlevel = 0;
  itog = 0;
  ctog = 0;

  sync_max_ever();

  if (IS_AFFECTED(ch, AFF_BLIND))
  {
    send_to_char("You cant see a thing!\n\r", ch);
    return;
  }

  sprintf(sepbuf,
          " {D----{c-{D---{c-{D--{c-{D-{c-{D-{c--{D-{c-{C-{c--{C-{c-{C-{c-{C--{C-{c-{W-{C--{W-{C-{W----{C-{W----{C-{W-{C--{W-{c-{C-{C--{c-{C-{c-{C-{c--{C-{c-{D-{c--{D-{c-{D-{c-{D--{c-{D---{c-{D---\n\r");
  /* 
   * Set default arguments.
   */
  for (iClass = 0; iClass < MAX_CLASS; iClass++)
    rgfClass[iClass] = false;
  for (iRace = 0; iRace < MAX_PC_RACE; iRace++)
    rgfRace[iRace] = false;
  for (iClan = 0; iClan < MAX_CLAN; iClan++)
    rgfClan[iClan] = false;
  /* 
   * Parse arguments.
   */
  for (;;)
  {
    char arg[MSL];

    argument = one_argument(argument, arg);
    if (arg[0] == '\0')
      break;
    if (is_number(arg))
    {
      switch (++nNumber)
      {
        case 1:
          iLevelLower = atoi(arg);
          break;
        case 2:
          iLevelUpper = atoi(arg);
          break;
        default:
          send_to_char("Only two level numbers allowed.\n\r", ch);
          return;
      }
    }
    else
    {

      /* 
       * Look for classes to turn on.
       */
      if (!str_prefix(arg, "immortals"))
      {
        fImmortalOnly = true;
      }
      else
      {
        iClass = class_lookup(arg);
        if (iClass == -1)
        {
          iRace = pcrace_lookup(arg);
          if (iRace == -1 || iRace >= MAX_PC_RACE)
          {
            if (!str_prefix(arg, "clan"))
              fClan = true;
            else
            {
              iClan = clan_lookup(arg);
              if (iClan)
              {
                fClanRestrict = true;
                rgfClan[iClan] = true;
              }
              else
              {
                if (!str_prefix(arg, "wr") || !str_prefix(arg, "pk"))
                {
                  wr_only = true;
                }
                else
                {
                  send_to_char
                    ("That's not a valid race, class, or clan.\n\r", ch);
                  return;
                }
              }
            }
          }
          else
          {
            fRaceRestrict = true;
            rgfRace[iRace] = true;
          }
        }
        else
        {
          fClassRestrict = true;
          rgfClass[iClass] = true;
        }
      }
    }
  }

  /* 
   * Now show matching chars.
   */
  nMatch = 0;
  buf[0] = '\0';
  /*    sprintf ( buf,
     "\n\r                    {CP{clay{Ders {CC{conne{Dcted {CT{co %s{w:{x\n\r",
     mudname );
     send_to_char ( buf, ch ); */
  send_to_char(sepbuf, ch);
  sprintf(buf,
          " {D| {wSex Lvl Race Class {D[  {wFlags  {D] [    {wClan    {D] {wName   Title {D|  {CSee help who {D|\n\r");
  send_to_char(buf, ch);
  send_to_char(sepbuf, ch);

  for (itog = 1; itog <= 2; itog++)
  {
    if (itog >= 2 && ctog == 0)
    {
      send_to_char(sepbuf, ch);
      ctog++;
    }
    for (ccount = MAX_CLAN; ccount >= 0; ccount--)
    {
      for (wlevel = MAX_LEVEL; wlevel >= 0; wlevel--)
      {
        for (d = descriptor_list; d != NULL; d = d->next)
        {
          CHAR_DATA *wch;
          char class[MSL];

          /* 
           * Check for match against restrictions.
           * Don't use trust as that exposes trusted mortals.
           */
          if (d->connected != CON_PLAYING)
            continue;

          wch = (d->original != NULL) ? d->original : d->character;
          if (IS_NPC(wch))
            continue;
          if ((itog >= 2 && IS_IMMORTAL(wch)) ||
              (itog == 1 && !IS_IMMORTAL(wch)))
            continue;
          if (wch->level != wlevel)
            continue;
          if (ccount != wch->clan)
            continue;

          // We want to see invis people as "someone"
          //                    if ( !can_see ( ch, wch ) && 
          //                         IS_SHIELDED ( wch, SHD_INVISIBLE ) )
          //                        continue;

          if (!can_see(ch, wch) && wch->level >= SQUIRE &&
              ch->level < wch->level)
            continue;
          count++;
          if ((wch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
            continue;
          if (wch->level < iLevelLower || wch->level > iLevelUpper ||
              (fImmortalOnly && wch->level < LEVEL_IMMORTAL) ||
              (wr_only &&
               (!WR(ch, wch) || IS_IMMORTAL(wch))) ||
              (fClassRestrict && !rgfClass[wch->class]) ||
              (fRaceRestrict && !rgfRace[wch->race]) || (fClan &&
                                                         !is_clan
                                                         (wch))
              || (fClanRestrict && !rgfClan[wch->clan]))
            continue;
          if (!is_pkill(wch) && !IS_IMMORTAL(wch))
            countnpk++;
          else
            countpk++;
          if (IS_IMMORTAL(wch))
          {
            countimm++;
          }
          else
          {
            countmort++;
          }
          nMatch++;
          /* 
           * Figure out what to print for class.
           */

          sprintf(buf2, "%3d", wch->level);
          switch (wch->level)
          {
            default:
              break;
              {
            case MAX_LEVEL - 0:
                sprintf(buf2, "{CGOD{x");
                break;
            case MAX_LEVEL - 1:
                sprintf(buf2, "{GCRE{x");
                break;
            case MAX_LEVEL - 2:
                sprintf(buf2, "{GSUP{x");
                break;
            case MAX_LEVEL - 3:
                sprintf(buf2, "{GDEI{x");
                break;
            case MAX_LEVEL - 4:
                sprintf(buf2, "{GGOD{x");

                break;
            case MAX_LEVEL - 5:
                sprintf(buf2, "{GIMM{x");
                break;
            case MAX_LEVEL - 6:
                sprintf(buf2, "{GDEM{x");
                break;
            case MAX_LEVEL - 7:
                sprintf(buf2, "{CKNI{x");
                break;
            case MAX_LEVEL - 8:
                sprintf(buf2, "{CSQU{x");
                break;
            case MAX_LEVEL - 9:
                sprintf(buf2, "{RA{rnc{x");
                break;
            case MAX_LEVEL - 10:
                sprintf(buf2, "{CH{cro{x");
                break;
              }
          }

          if (wch->sex == 1)
          {
            sprintf(sexbuf, "{D[{x%s{D]{x ", "{cM");
          }
          if (wch->sex == 2)
          {
            sprintf(sexbuf, "{D[{x%s{D]{x ", "{mF");
          }
          if (wch->sex == 0)
          {
            sprintf(sexbuf, "{D[{x%s{D]{x ", "{wN");
          }

          sprintf(afkbuf, "{y.");
          if (!IS_IMMORTAL(wch) || IS_IMMORTAL(ch))
          {
            if (wch->timer > 1)
              sprintf(afkbuf, "{WI");
          }

          if (IS_SET(wch->act2, PLR2_BUSY))
          {
            sprintf(afkbuf, "{YB");
          }

          if (IS_SET(wch->comm, COMM_AFK))
          {
            sprintf(afkbuf, "{YA");
          }

          if (WR(ch, wch) && !IS_IMMORTAL(ch) && !IS_IMMORTAL(wch))
          {
            sprintf(pkbuf, "{GW");
          }
          else
          {
            sprintf(pkbuf, "{g.");
          }

          if (wch->pk_timer > 0 && is_pkill(wch) && !IS_IMMORTAL(wch))
            sprintf(pkbuf, "{YT");

          if (IS_SET(class_table[wch->class].tier, TIER_03))
          {
            sprintf(class, "{R%c{r%c%c{x",
                    class_table[wch->class].who_name[0],
                    class_table[wch->class].who_name[1],
                    class_table[wch->class].who_name[2]);
          }
          else if (IS_SET(class_table[wch->class].tier, TIER_02))
          {
            sprintf(class, "{Y%c{y%c%c{x",
                    class_table[wch->class].who_name[0],
                    class_table[wch->class].who_name[1],
                    class_table[wch->class].who_name[2]);
          }
          else if (IS_SET(class_table[wch->class].tier, TIER_04))
          {
            sprintf(class, "{B%c{b%c%c{x",
                    class_table[wch->class].who_name[0],
                    class_table[wch->class].who_name[1],
                    class_table[wch->class].who_name[2]);
          }
          else
          {
            sprintf(class, "{G%c{g%c%c{x",
                    class_table[wch->class].who_name[0],
                    class_table[wch->class].who_name[1],
                    class_table[wch->class].who_name[2]);
          }

          /* 
           * Format it up.
           */

          if (!wch->on_quest)
          {
            questdat[0] = '\0';
            sprintf(questdat, "{m.");
          }
          else
          {
            sprintf(questdat, "{MQ");
          }
          if (!is_clan(wch))
          {
            sprintf(clandat, "%s", clan_table[wch->clan].who_name);
          }
          else
          {
            if (clan_table[wch->clan].independent)
            {
              sprintf(clandat, "{x[%s{x] ", clan_table[wch->clan].who_name);
            }
            else if (is_pkill(wch))
            {
              sprintf(clandat, "{R[{x%s{R]{x ",
                      clan_table[wch->clan].who_name);
            }
            else
            {
              sprintf(clandat, "{M[%s{M]{x ", clan_table[wch->clan].who_name);
            }
          }

          if (strlen(wch->pcdata->who_descr) >= 10)
          {
            sprintf(whon, "%-10s", wch->pcdata->who_descr);
          }
          else
          {
            sprintf(whon, "%-6s %-3s",
                    wch->race <
                    MAX_PC_RACE ? pc_race_table[wch->race].
                    who_name : "     ", class);
          }

          if (IS_SET(wch->act2, PLR2_CHALLENGED) ||
              IS_SET(wch->act2, PLR2_CHALLENGER))
          {
            sprintf(abuf, "{RA");
          }
          else
          {
            if (IS_SET(wch->act2, PLR2_MASS_ARENA))
              sprintf(abuf, "{RM");
            else
              sprintf(abuf, "{r.");
          }

          if ((wch->ghost_level >= LEVEL_ANCIENT) &&
              (ch->level >= wch->level))
            sprintf(invisbuf, "{WG");
          else if (wch->incog_level >= LEVEL_ANCIENT)
            sprintf(invisbuf, "{wI");
          else if (wch->invis_level >= LEVEL_ANCIENT)
            sprintf(invisbuf, "{DW");
          else
            sprintf(invisbuf, "{D.");

          sprintf(buf,
                  "   {x%s%s %s {D[%s%s%s%s%s%s%s%s%s{D] %s{w%s{w%s\n\r",
                  !can_see(ch, wch) ? "{D[{W?{D] " : sexbuf,
                  !can_see(ch, wch) ? "???" : buf2,
                  !can_see(ch, wch) ? "?????  ???" : whon,
                  IS_SET(wch->act2, PLR_MADMIN) ? "{CM" : "{c.",
                  is_clead(wch) ? "{GL" : "{g.", questdat,
                  afkbuf, IS_SET(wch->act2,
                                 PLR_RP) ? "{RR" : "{r.",
                  IS_SET(wch->act, PLR_TWIT) ? "{RT" : "{r.",
                  abuf, pkbuf, invisbuf, !can_see(ch,
                                                  wch) ?
                  "[    ???     ] " : clandat, !can_see(ch,
                                                        wch) ?
                  pcolor(wch, "Someone", 0) : pcolor(wch, wch->name, 0),
                  !can_see(ch, wch) ? " " : wch->pcdata->title);

          send_to_char(buf, ch);
        }
      }
    }
  }
  hour = (int) (struct tm *) localtime(&current_time)->tm_hour;
  if (countmort > 0)
    send_to_char(sepbuf, ch);
  if (nMatch != count)
  {
    sprintf(buf2, "\n\r{RMatches found: {W%d{x\n\r", nMatch);
    send_to_char(buf2, ch);
  }
  else
  {
    sprintf(buf2,
            " {D| {wTotal People Connected: {C%-2d {D| {wMost On Today: {C%-2d   {D| {wMost On Ever: {C%-2d        {D|\n\r {D| {wLogins This Copyover:  {C%-2ld  {D| {wLogin This Hour: {C%-2ld ",
            count, max_on, max_ever, logins_today, logins_hour);
    send_to_char(buf2, ch);

    if (uptime_ticks < 60)
      sprintf(utbuf, "{D| {wCopyover: {C%-2.0f {wminutes ago{D|\n\r",
              (uptime_ticks));

    if (uptime_ticks >= 60)
      sprintf(utbuf, "{D| {wCopyover: {C%-2.0f {whours ago  {D|\n\r",
              (uptime_ticks / 60));

    if (uptime_ticks >= 1440)
      sprintf(utbuf, "{D| {wCopyover: {C%-2.0f {wdays ago   {D|\n\r",
              (uptime_ticks / 1440));

    if (copyover_countdown > 0 && copyover_countdown != 1 &&
        is_copyover_countdown == true && copyover_countdown <= 5 &&
        copyover_countdown != 2)
      sprintf(utbuf,
              "{wCopyover: {YPENDING{x, less than {Y%d{x minutes!{x\n\r",
              copyover_countdown);

    if (copyover_countdown == 1 && is_copyover_countdown == true)
      sprintf(utbuf,
              "{wCopyover: {YLess than 60 seconds!{x\n\rCommand LOCKDOWN is {GON{x.\n\rLogin LOCKDOWN is {GON{x.\n\r");

    if (copyover_countdown == 2 && is_copyover_countdown == true)
      sprintf(utbuf,
              "{wCopyover: {YLess than 2 minutes!{x\n\rCommand LOCKDOWN is {GON{x.\n\r");

    send_to_char(utbuf, ch);
    send_to_char(sepbuf, ch);
    send_to_char(sepbuf, ch);
    if (global_quest)
    {
      sprintf(buf2, " {wGlobal Quests {w[{GON{w]\n\r");
      send_to_char(buf2, ch);
    }
    if (is_mass_arena)
    {
      sprintf(buf2, " {wMass-Arena {w[{GOPEN{w]\n\r");
      send_to_char(buf2, ch);
    }
    if (double_qp && !happy_hour)
    {
      sprintf(buf2, " {wDouble QP {w[{GON{w]\n\r");
      send_to_char(buf2, ch);
    }
    if (double_exp && !happy_hour)
    {
      sprintf(buf2, " {wDouble XP {w[{GON{w]\n\r");
      send_to_char(buf2, ch);
    }
    if (happy_hour)
    {
      thour = (int) (struct tm *) localtime(&current_time)->tm_hour;
      tday = (int) (struct tm *) localtime(&current_time)->tm_mday;
      tmonth = (int) (struct tm *) localtime(&current_time)->tm_mon;

      if ((tday == 24 || tday == 25) && tmonth == 11) // It is Actually 12
        sprintf(buf2,
                "\n\r{w** {GM {RE {GR {RR {GY   {RC {GH {RR {GI {RS {GT {RM {GA {RS {G!!  {w**\n\r");
      else
        sprintf(buf2, "\n\r{w** {oH A {oP P {oY  H {oO U {oR ! {w**\n\r");

      send_to_char(buf2, ch);
    }
    send_to_char("\n\r", ch);
  }
  return;

}

CH_CMD(do_count)
{
  int count;
  int hour;
  DESCRIPTOR_DATA *d;
  char buf[MSL];

  count = 0;
  hour = (int) (struct tm *) localtime(&current_time)->tm_hour;
  if (hour < 12)
  {
    if (is_pm)
    {
      is_pm = false;
      //expire_notes (  );
      randomize_entrances(ROOM_VNUM_CLANS);
    }
  }
  else
  {
    is_pm = true;
  }

  for (d = descriptor_list; d != NULL; d = d->next)
    if (d->connected == CON_PLAYING && can_see(ch, d->character))
      count++;

  if (max_on == count)
    sprintf(buf,
            "{BThere are {W%d {Bcharacters on, the most so far today.{x\n\r",
            count);
  else
    sprintf(buf,
            "{BThere are {W%d {Bcharacters on, the most on this boot was {W%d{x.\n\r",
            count, max_on);
  send_to_char(buf, ch);
}

CH_CMD(do_inventory)
{
  BUFFER *outlist;

  if (IS_SHIELDED(ch, SHD_VANISH))
  {
    send_to_char("You try to look at yourself, but see nothing.\n\r", ch);
    return;
  }
  send_to_char("You are carrying:\n\r", ch);
  outlist = show_list_to_char(ch->carrying, ch, true, true);
  page_to_char(buf_string(outlist), ch);
  free_buf(outlist);
  return;
}

CH_CMD(do_equipment)
{
  OBJ_DATA *obj;
  int iWear;
  int oWear;
  bool found;

  send_to_char("You are using:\n\r", ch);
  found = false;
  for (oWear = 0; oWear < MAX_WEAR; oWear++)
  {
    iWear = where_order[oWear];
    send_to_char(where_name[iWear], ch);
    if ((obj = get_eq_char(ch, iWear)) == NULL)
    {
      send_to_char("{W[{BE{bmpt{By{W]{x\n\r", ch);
    }
    else if (can_see_obj(ch, obj) && !IS_SHIELDED(ch, SHD_VANISH))
    {
      send_to_char(format_obj_to_char(obj, ch, true), ch);
      send_to_char("\n\r", ch);
    }
    else
    {
      send_to_char("something.\n\r", ch);
    }
    found = true;
  }

  if (!found)
    send_to_char("Nothing.\n\r", ch);
  return;
}

CH_CMD(do_compare)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  OBJ_DATA *obj1;
  OBJ_DATA *obj2;
  int value1;
  int value2;
  char *msg;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  if (arg1[0] == '\0')
  {
    send_to_char("Compare what to what?\n\r", ch);
    return;
  }

  if ((obj1 = get_obj_carry(ch, arg1)) == NULL)
  {
    send_to_char("You do not have that item.\n\r", ch);
    return;
  }

  if (arg2[0] == '\0')
  {
    for (obj2 = ch->carrying; obj2 != NULL; obj2 = obj2->next_content)
    {
      if (obj2->wear_loc != WEAR_NONE && can_see_obj(ch, obj2) &&
          obj1->item_type == obj2->item_type &&
          (obj1->wear_flags & obj2->wear_flags & ~ITEM_TAKE) != 0)
        break;
    }

    if (obj2 == NULL)
    {
      send_to_char("You aren't wearing anything comparable.\n\r", ch);
      return;
    }
  }

  else if ((obj2 = get_obj_carry(ch, arg2)) == NULL)
  {
    send_to_char("You do not have that item.\n\r", ch);
    return;
  }

  msg = NULL;
  value1 = 0;
  value2 = 0;
  if (obj1 == obj2)
  {
    msg = "You compare $p to itself.  It looks about the same.";
  }
  else if (obj1->item_type != obj2->item_type)
  {
    msg = "You can't compare $p and $P.";
  }
  else
  {
    switch (obj1->item_type)
    {
      default:
        msg = "You can't compare $p and $P.";
        break;
      case ITEM_ARMOR:
        if (obj1->clan)
        {
          value1 = apply_ac(ch->level, obj1, WEAR_HOLD, 0);
          value1 += apply_ac(ch->level, obj1, WEAR_HOLD, 1);
          value1 += apply_ac(ch->level, obj1, WEAR_HOLD, 2);
        }
        else
        {
          value1 = obj1->value[0] + obj1->value[1] + obj1->value[2];
        }
        if (obj2->clan)
        {
          value2 = apply_ac(ch->level, obj2, WEAR_HOLD, 0);
          value2 += apply_ac(ch->level, obj2, WEAR_HOLD, 1);
          value2 += apply_ac(ch->level, obj2, WEAR_HOLD, 2);
        }
        else
        {
          value2 = obj2->value[0] + obj2->value[1] + obj2->value[2];
        }
        break;
      case ITEM_WEAPON:
        if (obj1->clan)
        {
          value1 = 4 * (ch->level / 3);
        }
        else
        {
          if (obj1->pIndexData->new_format)
            value1 = (1 + obj1->value[2]) * obj1->value[1];
          else
            value1 = obj1->value[1] + obj1->value[2];
        }
        if (obj2->clan)
        {
          value2 = 4 * (ch->level / 3);
        }
        else
        {
          if (obj2->pIndexData->new_format)
            value2 = (1 + obj2->value[2]) * obj2->value[1];
          else
            value2 = obj2->value[1] + obj2->value[2];
        }
        break;
    }
  }

  if (msg == NULL)
  {
    if (value1 == value2)
      msg = "$p and $P look about the same.";
    else if (value1 > value2)
      msg = "$p looks better than $P.";
    else
      msg = "$p looks worse than $P.";
  }

  act(msg, ch, obj1, obj2, TO_CHAR);
  return;
}

CH_CMD(do_credits)
{
  do_help(ch, "diku");
  return;
}

CH_CMD(do_where)
{
  char buf[MSL];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  DESCRIPTOR_DATA *d;
  bool found;

  if (IS_AFFECTED(ch, AFF_BLIND))
  {
    send_to_char("You are blind and can not see who is near you.\n\r", ch);
    return;
  }

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Players near you:\n\r", ch);
    found = false;
    for (d = descriptor_list; d; d = d->next)
    {
      if (d->connected == CON_PLAYING &&
          (victim = d->character) != NULL && !IS_NPC(victim) &&
          victim->in_room != NULL &&
          !IS_SET(victim->in_room->room_flags, ROOM_NOWHERE) &&
          (is_room_owner(ch, victim->in_room) ||
           !room_is_private(ch, victim->in_room)) &&
          victim->in_room->area == ch->in_room->area &&
          get_trust(ch) >= victim->ghost_level && can_see(ch, victim))
      {
        found = true;
        sprintf(buf, "%-28s %s\n\r", victim->name, victim->in_room->name);
        send_to_char(buf, ch);
      }
    }
    if (!found)
      send_to_char("None\n\r", ch);
  }
  else
  {
    found = false;
    for (victim = char_list; victim != NULL; victim = victim->next)
    {
      if (victim->in_room != NULL &&
          victim->in_room->area == ch->in_room->area &&
          !IS_AFFECTED(victim, AFF_HIDE) &&
          !IS_AFFECTED(victim, AFF_SNEAK) &&
          get_trust(ch) >= victim->ghost_level &&
          can_see(ch, victim) && is_name(arg, victim->name))
      {
        found = true;
        sprintf(buf, "%-28s %s\n\r", PERS(victim, ch), victim->in_room->name);
        send_to_char(buf, ch);
        break;
      }
    }
    if (!found)
      act("You didn't find any $T.", ch, NULL, arg, TO_CHAR);
  }

  return;
}

CH_CMD(do_track)
{
  char buf[MSL];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  EXIT_DATA *pexit;
  ROOM_INDEX_DATA *in_room;
  long track_vnum;
  int door, move, chance, track;

  one_argument(argument, arg);
  if ((chance = get_skill(ch, gsn_track)) == 0)
  {
    send_to_char("You don't know how to track.\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    send_to_char("Track whom?\n\r", ch);
    return;
  }

  in_room = ch->in_room;
  track_vnum = in_room->vnum;
  move = movement_loss[UMIN(SECT_MAX - 1, in_room->sector_type)];
  if (ch->move < move)
  {
    send_to_char("You are too exhausted.\n\r", ch);
    return;
  }

  if (number_percent() < (100 - chance))
  {
    sprintf(buf, "You can find no recent tracks for %s.\n\r", arg);
    send_to_char(buf, ch);
    check_improve(ch, gsn_track, false, 1);
    if (!IS_IMMORTAL(ch))
      if (!IS_IMMORTAL(ch))
        WAIT_STATE(ch, 1);
    ch->move -= move / 2;
    return;
  }

  for (victim = char_list; victim != NULL; victim = victim->next)
  {
    if (victim->in_room != NULL && !IS_NPC(victim) &&
        can_see(ch, victim) && is_name(arg, victim->name))
    {
      if (victim->in_room->vnum == track_vnum)
      {
        act("The tracks end right under $S feet.", ch, NULL, victim, TO_CHAR);
        return;
      }
      for (track = 0; track < MAX_TRACK; track++)
      {
        if (victim->track_from[track] == track_vnum)
        {
          for (door = 0; door < MAX_DIR; door++)
          {
            if ((pexit = in_room->exit[door]) != NULL)
            {
              if (pexit->u1.to_room->vnum == victim->track_to[track])
              {
                sprintf(buf,
                        "Some tracks lead off to the %s.\n\r",
                        dir_name[door]);
                send_to_char(buf, ch);
                check_improve(ch, gsn_track, true, 1);
                if (!IS_IMMORTAL(ch))
                  WAIT_STATE(ch, 1);
                ch->move -= move;
                return;
              }
            }
          }
          act("$N seems to have vanished here.", ch, NULL, victim, TO_CHAR);
          check_improve(ch, gsn_track, true, 1);
          if (!IS_IMMORTAL(ch))
            WAIT_STATE(ch, 1);
          ch->move -= move;
          return;
        }
      }
      act("You can find no recent tracks for $N.", ch, NULL, victim, TO_CHAR);
      check_improve(ch, gsn_track, false, 1);
      if (!IS_IMMORTAL(ch))
        WAIT_STATE(ch, 1);
      ch->move -= move / 2;
      return;
    }
  }
  for (victim = char_list; victim != NULL; victim = victim->next)
  {
    if ((victim->in_room != NULL) && IS_NPC(victim) &&
        can_see(ch, victim) &&
        (victim->in_room->area == ch->in_room->area) &&
        is_name(arg, victim->name))
    {
      if (victim->in_room->vnum == track_vnum)
      {
        act("The tracks end right under $S feet.", ch, NULL, victim, TO_CHAR);
        return;
      }
      for (track = 0; track < MAX_TRACK; track++)
      {
        if (victim->track_from[track] == track_vnum)
        {
          for (door = 0; door < MAX_DIR; door++)
          {
            if ((pexit = in_room->exit[door]) != NULL)
            {
              if (pexit->u1.to_room->vnum == victim->track_to[track])
              {
                sprintf(buf,
                        "Some tracks lead off to the %s.\n\r",
                        dir_name[door]);
                send_to_char(buf, ch);
                check_improve(ch, gsn_track, true, 1);
                if (!IS_IMMORTAL(ch))
                  WAIT_STATE(ch, 1);
                ch->move -= move;
                return;
              }
            }
          }
          act("$N seems to have vanished here.", ch, NULL, victim, TO_CHAR);
          check_improve(ch, gsn_track, true, 1);
          if (!IS_IMMORTAL(ch))
            WAIT_STATE(ch, 1);
          ch->move -= move;
          return;
        }
      }
    }
  }
  sprintf(buf, "You can find no recent tracks for %s.\n\r", arg);
  send_to_char(buf, ch);
  check_improve(ch, gsn_track, false, 1);
  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, 1);
  ch->move -= move / 2;
  return;
}

CH_CMD(do_consider)
{
  //    BUFFER *output;
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  char *msg;
  int diff, vac, cac;
  char buf[MSL];

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Consider killing whom?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They're not here.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
  {
    send_to_char("Don't even think about it.\n\r", ch);
    return;
  }

  diff = ((victim->hit / 50) - (ch->hit / 50));
  vac =
    -(GET_AC(victim, AC_PIERCE) + GET_AC(victim, AC_BASH) +
      GET_AC(victim, AC_SLASH) + GET_AC(victim, AC_EXOTIC));
  cac =
    -(GET_AC(ch, AC_PIERCE) + GET_AC(ch, AC_BASH) +
      GET_AC(ch, AC_SLASH) + GET_AC(ch, AC_EXOTIC));
  diff += (vac - cac);
  diff += (GET_DAMROLL(victim) - GET_DAMROLL(ch));
  diff += (GET_HITROLL(victim) - GET_HITROLL(ch));
  diff += (get_curr_stat(victim, STAT_STR) - get_curr_stat(ch, STAT_STR));
  if (diff <= -110)
    msg = "You can kill $N naked and weaponless.";
  else if (diff <= -70)
    msg = "$N is no match for you.";
  else if (diff <= -20)
    msg = "$N looks like an easy kill.";
  else if (diff <= 20)
    msg = "$N can hold $S own against you.";
  else if (diff <= 70)
    msg = "$N says '{aDo you feel lucky, punk?{x'.";
  else if (diff <= 110)
    msg = "$N laughs at you mercilessly.";
  else
    msg = "Death will thank you for your gift.";
  act(msg, ch, NULL, victim, TO_CHAR);
  sprintf(buf, "Hp: %ld/%ld  Mana: %ld/%ld  Lv: %d  Align: %d\n\r",
          victim->hit, victim->max_hit, victim->mana, victim->max_mana,
          victim->level, victim->alignment);
  send_to_char(buf, ch);
  if (victim->imm_flags)
  {
    sprintf(buf, "Immune: %s\n\r", imm_bit_name(victim->imm_flags));
    send_to_char(buf, ch);
  }

  if (victim->res_flags)
  {
    sprintf(buf, "Resist: %s\n\r", imm_bit_name(victim->res_flags));
    send_to_char(buf, ch);
  }
  if (victim->vuln_flags)
  {
    sprintf(buf, "Vulnerable: %s\n\r", imm_bit_name(victim->vuln_flags));
    send_to_char(buf, ch);
  }

  return;
}

void set_title(CHAR_DATA * ch, char *title)
{
  char buf[MSL];

  if (IS_NPC(ch))
  {
    bug("Set_title: NPC.", 0);
    return;
  }

  if (title[0] != '.' && title[0] != ',' && title[0] != '!' &&
      title[0] != '?')
  {
    buf[0] = ' ';
    strcpy(buf + 1, title);
  }
  else
  {
    strcpy(buf, title);
  }

  free_string(ch->pcdata->title);
  ch->pcdata->title = str_dup(buf);
  return;
}

bool is_blinky(char *argument)
{
  if (!str_infix("{z", argument) || !str_cmp("{z", argument) ||
      !str_prefix("{z", argument))
  {
    return true;
  }
  if (!str_infix("{$", argument) || !str_cmp("{$", argument) ||
      !str_prefix("{$", argument))
  {
    return true;
  }
  return false;
}

bool is_stupid(char *argument)
{
  return false;
}

void do_spacetext(CHAR_DATA * ch, char *argument)
{
  int namel;
  int titl;
  int temp;
  int len;
  int offset;
  int value;
  int i;

  namel = (strlen(ch->name));
  titl = (strlen(argument));

  i = 1;
  offset = 0;
  temp = titl;
  for (i = 0; i < temp; i++)
    if (argument[i] == '{')
    {
      titl -= 2;
      offset += 2;
    }
  value = (26 - (titl + namel));
  len = (titl + namel);

  if (len < 26)
  {
    i = titl;
    while (i < (26))
    {

      argument[i + offset] = ' ';
      i++;
    }
  }
  argument[26 - namel + offset] = '\0';

  ch->pcdata->sptitle = argument;
  return;
}

CH_CMD(do_title)
{
  int value;

  if (IS_NPC(ch))
    return;
  if (IS_SET(ch->act, PLR_NOTITLE))
    return;
  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
  {
    send_to_char
      ("Just keep your nose in the corner like a good little player.\n\r",
       ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char("Change your title to what?\n\r", ch);
    return;
  }

  if (is_blinky(argument))
  {
    send_to_char("You cannot use blink tags in your title.\n\r", ch);
    return;
  }

  if (strlen(argument) > 50)
  {
    argument[51] = '{';
    argument[52] = 'x';
    argument[53] = '\0';
  }
  else
  {
    value = strlen(argument);
    argument[value] = '{';
    argument[value + 1] = 'x';
    argument[value + 2] = '\0';
  }
  smash_tilde(argument);
  set_title(ch, argument);
  send_to_char("Ok.\n\r", ch);
}

CH_CMD(do_description)
{
  char buf[MIL];

  smash_tilde(argument);
  if (argument[0] != '\0')
  {
    buf[0] = '\0';
    if (argument[0] == '-')
    {
      int len;
      bool found = false;

      if (ch->description == NULL || ch->description[0] == '\0')
      {
        send_to_char("No lines left to remove.\n\r", ch);
        return;
      }

      strcpy(buf, ch->description);
      for (len = strlen(buf); len > 0; len--)
      {
        if (buf[len] == '\r')
        {
          if (!found)           /* back it up */
          {
            if (len > 0)
              len--;
            found = true;
          }
          else                  /* found the second one */
          {
            buf[len + 1] = '\0';
            free_string(ch->description);
            ch->description = str_dup(buf);
            send_to_char("Your description is:\n\r", ch);
            send_to_char(ch->description ? ch->
                         description : "(None).\n\r", ch);
            return;
          }
        }
      }
      buf[0] = '\0';
      free_string(ch->description);
      ch->description = str_dup(buf);
      send_to_char("Description cleared.\n\r", ch);
      return;
    }

    if (argument[0] == '+')
    {
      if (ch->description != NULL)
        strcat(buf, ch->description);
      argument++;
      while (isspace(*argument))
        argument++;
    }

    if (strlen(buf) + strlen(argument) >= 1000 - 2)
    {
      send_to_char("Description too long.\n\r", ch);
      return;
    }

    strcat(buf, argument);
    strcat(buf, "\n\r");
    free_string(ch->description);
    ch->description = str_dup(buf);
  }

  send_to_char("Your description is:\n\r", ch);
  send_to_char(ch->description ? ch->description : "(None).\n\r", ch);
  return;
}

CH_CMD(do_report)
{
  char buf[MAX_INPUT_LENGTH];

  sprintf(buf,
          "You say '{SI have %ld/%ld hp %ld/%ld mana %ld/%ld mv %ld xp.{x'\n\r",
          ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
          ch->max_move, ch->exp);
  send_to_char(buf, ch);
  sprintf(buf,
          "$n says '{SI have %ld/%ld hp %ld/%ld mana %ld/%ld mv %ld xp.{x'",
          ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
          ch->max_move, ch->exp);
  act(buf, ch, NULL, NULL, TO_ROOM);
  return;
}

CH_CMD(do_practice)
{
  BUFFER *buffer;
  char buf[MSL];
  int sn, i;
  bool found;

  if (IS_NPC(ch))
    return;
  if (argument[0] == '\0')
  {
    int col;
    int linetog;

    col = 0;
    linetog = 0;
    buffer = new_buf();
    for (sn = 2; sn < MAX_SKILL; sn++)
    {
      found = false;
      if (skill_table[sn].name == NULL)
        break;

      for (i = 0; i < 5; i++)
      {
        if (pc_race_table[ch->race].skills == NULL)
          break;
        if (pc_race_table[ch->race].skills[i] == NULL)
          break;
        if (!str_cmp(pc_race_table[ch->race].skills[i], skill_table[sn].name))
          found = true;
      }

      if ((ch->level < skill_table[sn].skill_level[ch->class] &&
           !found) || ch->pcdata->learned[sn] <= 0)
        continue;

      sprintf(buf, "%s", linetog == 0 ? "{W" : "{w");
      send_to_char(buf, ch);
      sprintf(buf, "%19s %s%%%-3d {w|", skill_table[sn].name,
              ch->pcdata->learned[sn] < 26 ? "{r" : ch->pcdata->learned[sn] <
              51 ? "{y" : ch->pcdata->learned[sn] <
              76 ? "{g" : ch->pcdata->learned[sn] <
              100 ? "{c" : ch->pcdata->learned[sn] >= 100 ? "{W" : "{W",
              ch->pcdata->learned[sn]);
      send_to_char(buf, ch);
      if (++col > 2)
      {
        sprintf(buf, "\n\r");
        send_to_char(buf, ch);

        if (linetog == 0)
          linetog = 1;
        else
          linetog = 0;

        col = 0;
      }
    }
    sprintf(buf, "\n\r{xYou have {R%d{x practice sessions remaining.\n\r",
            ch->practice);
    send_to_char(buf, ch);
  }
  else
  {
    CHAR_DATA *mob;
    int adept;

    if (!IS_AWAKE(ch))
    {
      send_to_char("In your dreams, or what?\n\r", ch);
      return;
    }

    for (mob = ch->in_room->people; mob != NULL; mob = mob->next_in_room)
    {
      if (IS_NPC(mob) && IS_SET(mob->act, ACT_PRACTICE))
        break;
    }

    if (mob == NULL)
    {
      send_to_char("You can't do that here.\n\r", ch);
      return;
    }

    if (ch->practice <= 0)
    {
      send_to_char("You have no practice sessions left.\n\r", ch);
      return;
    }

    sn = skill_lookup(argument);
    if (sn == -1)
    {
      send_to_char("I dont know skill by that name.\n\r", ch);
      return;
    }
    if (ch->level < skill_table[sn].skill_level[ch->class] ||
        ch->pcdata->learned[sn] == 0)
    {
      send_to_char("You can't practice that.\n\r", ch);
      return;
    }

    adept = IS_NPC(ch) ? 100 : class_table[ch->class].skill_adept;
    if (ch->pcdata->learned[sn] >= adept)
    {
      sprintf(buf, "I cannot teach you any more about %s.\n\r",
              skill_table[sn].name);
      send_to_char(buf, ch);
    }
    else
    {
      ch->practice--;
      ch->pcdata->learned[sn] +=
        int_app[get_curr_stat(ch, STAT_INT)].learn /
        skill_table[sn].rating[ch->class];
      if (ch->pcdata->learned[sn] < adept)
      {
        act("You practice $T.", ch, NULL, skill_table[sn].name, TO_CHAR);
        // act ( "$n practices $T.", ch, NULL, skill_table[sn].name,
        // TO_ROOM );
      }
      else
      {
        ch->pcdata->learned[sn] = adept;
        act("You now know everything about $T!", ch, NULL,
            skill_table[sn].name, TO_CHAR);
      }
    }
  }
  return;
}

/*
 * 'Wimpy' originally by Dionysos.
 */
CH_CMD(do_wimpy)
{
  char buf[MSL];
  char arg[MAX_INPUT_LENGTH];
  int wimpy;

  one_argument(argument, arg);
  if (arg[0] == '\0')
    wimpy = ch->max_hit / 5;
  else
    wimpy = atoi(arg);
  if (wimpy < 0)
  {
    send_to_char("Your courage exceeds your wisdom.\n\r", ch);
    return;
  }

  if (wimpy > ch->max_hit / 2)
  {
    send_to_char("Such cowardice ill becomes you.\n\r", ch);
    return;
  }

  ch->wimpy = wimpy;
  sprintf(buf, "Wimpy set to %d hit points.\n\r", wimpy);
  send_to_char(buf, ch);
  return;
}

CH_CMD(do_password)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char *pArg;
  char *pwdnew;
  char *p;
  char cEnd;

  if (IS_NPC(ch))
    return;
  /* 
   * Can't use one_argument here because it smashes case.
   * So we just steal all its code.  Bleagh.
   */
  pArg = arg1;
  while (isspace(*argument))
    argument++;
  cEnd = ' ';
  if (*argument == '\'' || *argument == '"')
    cEnd = *argument++;
  while (*argument != '\0')
  {
    if (*argument == cEnd)
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\0';
  pArg = arg2;
  while (isspace(*argument))
    argument++;
  cEnd = ' ';
  if (*argument == '\'' || *argument == '"')
    cEnd = *argument++;
  while (*argument != '\0')
  {
    if (*argument == cEnd)
    {
      argument++;
      break;
    }
    *pArg++ = *argument++;
  }
  *pArg = '\0';
  if (arg1[0] == '\0' || arg2[0] == '\0')
  {
    send_to_char("Syntax: password <old> <new>.\n\r", ch);
    return;
  }

  if (str_cmp(crypt(arg1, ch->pcdata->pwd), ch->pcdata->pwd))
  {
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, 40);
    send_to_char("Wrong password.  Wait 10 seconds.\n\r", ch);
    return;
  }

  if (strlen(arg2) < 5)
  {
    send_to_char
      ("New password must be at least five characters long.\n\r", ch);
    return;
  }

  /* 
   * No tilde allowed because of player file format.
   */
  pwdnew = crypt(arg2, ch->name);
  for (p = pwdnew; *p != '\0'; p++)
  {
    if (*p == '~')
    {
      send_to_char("New password not acceptable, try again.\n\r", ch);
      return;
    }
  }

  free_string(ch->pcdata->pwd);
  ch->pcdata->pwd = str_dup(pwdnew);
  save_char_obj(ch);
  send_to_char("Ok.\n\r", ch);
  return;
}

CH_CMD(do_autoall)
{
  if (IS_NPC(ch))
    return;
  SET_BIT(ch->act, PLR_AUTOASSIST);
  SET_BIT(ch->act, PLR_AUTOEXIT);
  SET_BIT(ch->act, PLR_AUTOGOLD);
  SET_BIT(ch->act, PLR_AUTOLOOT);
  SET_BIT(ch->act, PLR_AUTOSAC);
  SET_BIT(ch->act, PLR_AUTOSPLIT);
  send_to_char("All autos turned on.\n\r", ch);
}

CH_CMD(do_omni)
{
  char buf[MSL * 100];
  char buf2[MSL * 100];
  BUFFER *output;
  DESCRIPTOR_DATA *d;
  int immmatch;
  int mortmatch;
  int hptemp;

  /* 
   * Initalize Variables. */
  immmatch = 0;
  mortmatch = 0;
  buf[0] = '\0';
  output = new_buf();
  /* 
   * Count and output the IMMs. */
  sprintf(buf,
          "{D-{w={D-{w={D-{w={D-{w={D-{w={D-{W>{RImmortals{W<{D-{w={D-{w={D-{w={D-{w={D-{w={D-{x\n\r");
  send_to_char(buf, ch);
  sprintf(buf,
          "{GName           {YLevel    {WWiz     {rIncog   {w[{RVnum{w]{x\n\r");
  send_to_char(buf, ch);
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *wch;

    if (d->connected != CON_PLAYING || !can_see(ch, d->character))
      continue;
    wch = (d->original != NULL) ? d->original : d->character;
    if (!can_see(ch, wch) || wch->level < 203)
      continue;
    immmatch++;
    sprintf(buf,
            "{G%-14s  {Y%-3d     {W%-3d     {r%-3d{w     [{R%-6ld{w] %2.f %d{x\n\r",
            wch->name, wch->level, wch->invis_level, wch->incog_level,
            wch->in_room->vnum, wch->btime, wch->bflip);
    send_to_char(buf, ch);
  }

  /* 
   * Count and output the Morts. */
  sprintf(buf,
          "\n\r{D-{w={D-{w={D-{w={D-{w={D-{w={D-{W>{RMortals{W<{w-{D={w-{D={w-{D={w-{D={w-{D={w-\n\r");
  add_buf(output, buf);
  sprintf(buf,
          "{GName           {DRace{w/{YClass   {gPosition      {mLevel  {R%%hps {w[{yVnum{w]{x\n\r");
  add_buf(output, buf);
  hptemp = 0;
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *wch;
    char const *class;

    if (d->connected != CON_PLAYING || !can_see(ch, d->character))
      continue;
    wch = (d->original != NULL) ? d->original : d->character;
    if (!can_see(ch, wch) || wch->level > ch->level || wch->level > 202)
      continue;
    mortmatch++;
    if ((wch->max_hit != wch->hit) && (wch->hit > 0))
      hptemp = (wch->hit * 100) / wch->max_hit;
    else if (wch->max_hit == wch->hit)
      hptemp = 100;
    else if (wch->hit < 0)
      hptemp = 0;
    class = class_table[wch->class].who_name;
    sprintf(buf,
            "{G%-14s {D%6s{w/{Y%3s    {g%-15s {m%-3d   {R%3d%%   {w[{y%ld{w] %2.f %d{x\n\r",
            wch->name,
            wch->race <
            MAX_PC_RACE ? pc_race_table[wch->race].who_name : "     ",
            class, capitalize(position_table[wch->position].name),
            wch->level, hptemp, wch->in_room->vnum, wch->btime, wch->bflip);
    add_buf(output, buf);
  }

  /*
   * Tally the counts and send the whole list out.
   */
  sprintf(buf2, "\n\r{RIMMs {wfound{x: {M%d{x\n\r", immmatch);
  add_buf(output, buf2);
  sprintf(buf2, "{BMorts {wfound{x: {M%d{x\n\r", mortmatch);
  add_buf(output, buf2);
  page_to_char(buf_string(output), ch);
  free_buf(output);
  return;
}

CH_CMD(do_anew)
{

  char buf[MSL];
  char result[MSL * 2];
  AREA_DATA *pArea;

  sprintf(result, "{m[{W%-9s{m] {m({W%-27s{m  ){W %-10s{x\n\r\n\r",
          "Lvl Range", "Area Name", "Credits");
  // sprintf(
  // result,"{c************************************************************{x\n\r");
  for (pArea = area_first; pArea; pArea = pArea->next)
  {
    sprintf(buf, "{m[{W%-9s{m] {m({W%-29.29s{m){W %-10.10s{x\n\r",
            "Lvl Range", pArea->name, pArea->builders);
    strcat(result, buf);
  }
  //    sprintf( result,"{c************************************************************{x\n\r");
  send_to_char(result, ch);
  return;
}
