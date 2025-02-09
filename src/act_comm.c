
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
 **************************************************************************/

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

/***************************************************************************
*                                                                          *
*                                                                          *
*       DRM 1.0a is copyright 2000-2002 by Joshua Chance Blackwell         *
*       By using this code, you have agreed to follow the terms of the     *
*       DRM license, in the file doc/DRM.license                           *
*                                                                          *
*                                                                          *
***************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "db.h"
#include "olc.h"
#include "lookup.h"
#include "ctype.h"

void do_pkset(CHAR_DATA * ch, char *argument)
{
  if (IS_NPC(ch))
    return;

  if (ch->pcdata->pkset == true)
  {
    send_to_char("You are already set with PK status.\n\r", ch);
    return;
  }

  if (ch->pcdata->confirm_pkset == true)
  {
    if (argument[0] != '\0')
    {
      send_to_char("Pkset status removed.\n\r", ch);
      ch->pcdata->confirm_pkset = false;
      return;
    }
    else
    {
      send_to_char
        ("Pk status has been set.  You are now open to any and all pk attacks.\n\r",
         ch);
      ch->pcdata->confirm_pkset = false;
      ch->pcdata->pkset = true;
    }
  }
  else
  {
    send_to_char
      ("Warning, this command will irreversibly make you vulnerable to pkills.\n\r",
       ch);
    send_to_char("Type 'pkset' again to confirm this command.", ch);
    send_to_char("Typing pkset with an argument will cancel the command.\n\r",
                 ch);
    ch->pcdata->confirm_pkset = true;
  }

  return;
}

/* RT code to delete yourself */

CH_CMD(do_delet)
{
  send_to_char("You must type the full command to delete yourself.\n\r", ch);
}

CH_CMD(do_delete)
{
  char newbuf[MSL];
  char buf[MAX_INPUT_LENGTH];
  char strsave[MAX_INPUT_LENGTH];

  if (IS_NPC(ch))
    return;

  if (auction_list != NULL &&
      (auction_list->high_bidder == ch || auction_list->owner == ch))
  {
    send_to_char("You still have a stake in the auction!\n\r", ch);
    return;
  }

  if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
  {
    send_to_char("Not while in the arena you dont.\n\r", ch);
    return;
  }

  if (ch->pcdata->confirm_delete)
  {
    if (argument[0] != '\0')
    {
      send_to_char("Delete status removed.\n\r", ch);
      ch->pcdata->confirm_delete = false;
      return;
    }
    else
    {
      if (is_clead(ch))
        update_clanlist(ch, ch->clead, false, true);
      if (is_clan(ch))
        update_clanlist(ch, ch->clan, false, false);
      sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
      sprintf(log_buf, "%s turns %sself into line noise.", ch->name,
              him_her[URANGE(0, ch->sex, 2)]);
      wiznet(log_buf, ch, NULL, WIZ_LOGS, 0, 0);

      sprintf(newbuf, "{w[{RDELETE{w] %s has deleted.\n\r", ch->name);
      do_gmessage(newbuf);

      stop_fighting(ch, true);
      if (ch->level > HERO)
      {
        update_wizlist(ch, 1);
      }
      clean_char_flags(ch);
      REMOVE_BIT(ch->comm, COMM_AFK);
      force_quit(ch, "");
      unlink(strsave);
      sprintf(buf, "rm -f %s.gz ", strsave);
      system(buf);
      return;
    }
  }

  if (argument[0] != '\0')
  {
    send_to_char("Just type delete. No argument.\n\r", ch);
    return;
  }

  send_to_char("Type delete again to confirm this command.\n\r", ch);
  send_to_char("WARNING: this command is irreversible.\n\r", ch);
  send_to_char
    ("Typing delete with an argument will undo delete status.\n\r", ch);
  ch->pcdata->confirm_delete = true;

  sprintf(newbuf, "{w[{RDELETE{w] %s is considering deleting.\n\r", ch->name);
  do_gmessage(newbuf);

  sprintf(newbuf, "%s is contemplating deletion.", ch->name);
  wiznet(newbuf, ch, NULL, WIZ_LOGS, 0, get_trust(ch));
}

CH_CMD(do_rerol)
{
  send_to_char("You must type the full command to reroll yourself.\n\r", ch);
}

CH_CMD(do_questforge)
{
  char arg1[MAX_INPUT_LENGTH];  /* weapon class */
  char arg2[MAX_INPUT_LENGTH];  /* damage type */
  char flag[3][MAX_INPUT_LENGTH]; /* first/second/third flag */
  int wc = 0, dt = 0, flags = 0, i;
  OBJ_DATA *obj;
  char buf[MAX_STRING_LENGTH];
  char numstr[31];
  AFFECT_DATA af;

  arg1[0] = '\0';
  arg2[0] = '\0';
  flag[0][0] = '\0';
  flag[1][0] = '\0';
  flag[2][0] = '\0';
  buf[0] = '\0';
  numstr[0] = '\0';

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, flag[0]);
  argument = one_argument(argument, flag[1]);
  argument = one_argument(argument, flag[2]);

  if (IS_NPC(ch))
  {
    send_to_char("You are an NPC. Go away.\n\r", ch);
    return;
  }

  if ((ch->in_room == NULL) || (ch->in_room->vnum != 3398))
  {
    send_to_char("Does this look like the journeymen guild?\n\r", ch);
    return;
  }
  if (arg1[0] == '\0')
  {
    send_to_char
      ("{cSyntax{D:{x qforge <weaponclass> <damagetype> <flag1> <flag2> <flag3>\n\r",
       ch);
    send_to_char
      ("        qforge wclass {D- {clist available weapon classes{x\n\r", ch);
    send_to_char
      ("	qforge dtype  {D- {clist available damage types{x\n\r", ch);
    send_to_char
      ("	qforge flags  {D- {clist available weapon flags{x\n\r", ch);
    return;
  }
  if (!str_cmp(arg1, "wclass"))
  {
    send_to_char("{cValid weapon classes are{D:{x\n\r", ch);
    do_help(ch, "wclass");
    return;
  }
  else if (!str_cmp(arg1, "dtype"))
  {
    send_to_char("{cValid damage types are{D:{x\n\r", ch);
    do_help(ch, "wdam");
    return;
  }
  else if (!str_cmp(arg1, "flags"))
  {
    send_to_char("{cValid weapon flags are{D:{x\n\r", ch);
    do_help(ch, "wtype");
    return;
  }
  else if (arg2[0] == '\0' || flag[0][0] == '\0' || flag[1][0] == '\0' ||
           flag[2][0] == '\0')
  {
    do_questforge(ch, "");
    return;
  }
  if (ch->qps - 200 < 0)
  {
    send_to_char
      ("Forging a weapon costs 200 immquest points. You don't have enough.\n\r",
       ch);
    return;
  }
  if ((weapon_lookup(arg1) == -1) && str_cmp(arg1, "exotic"))
  {
    send_to_char
      ("This weapon class is not available. Valid classes are:\n\r", ch);
    do_help(ch, "wclass");
    return;
  }
  wc = weapon_type(arg1);
  if (((dt = attack_lookup(arg2)) == 0) || ((dt >= 40) && (dt <= 60)))
  {
    send_to_char
      ("This damage type is not available. Valid types are:\n\r", ch);
    do_help(ch, "wdam");
    return;
  }
  for (i = 0; i < 3; i++)
  {
    switch (i)
    {
      case 0:
        sprintf(numstr, "first");
        break;
      case 1:
        sprintf(numstr, "second");
        break;
      case 2:
        sprintf(numstr, "third");
        break;
    }
    if (flag_value(weapon_type2, flag[i]) == NO_FLAG)
    {
      sprintf(buf,
              "The %s flag you have specified is not available. Valid flags are:\n\r",
              numstr);
      send_to_char(buf, ch);
      show_help(ch, "wtype");
      return;
    }
    if (IS_SET(flags, flag_value(weapon_type2, flag[i])))
    {
      send_to_char("You can't use a weapon flag twice.\n\r", ch);
      return;
    }
    flags ^= flag_value(weapon_type2, flag[i]);
  }

  obj = create_object(get_obj_index(20050));
  ch->qps -= 200;
  free_string(obj->name);
  sprintf(buf, "forged %s",
          ((wc == 0) ? ("weapon") : (weapon_class[wc].name)));
  obj->name = str_dup(buf);
  free_string(obj->short_descr);
  sprintf(buf, "{DA {Rf{ro{Wr{xg{re{Rd {D%s{x",
          ((wc == 0) ? ("weapon") : (weapon_class[wc].name)));
  obj->short_descr = str_dup(buf);
  free_string(obj->description);
  sprintf(buf, "%s{D is lying here.{x", buf);
  obj->description = str_dup(buf);
  obj->value[0] = wc;
  obj->value[1] = ch->level / 10;
  obj->value[2] = ch->level / 7;
  obj->value[3] = dt;
  obj->value[4] = flags;
  obj->weight = ch->level / 5;
  af.where = TO_OBJECT;

  af.level = ch->level;
  af.duration = -1;
  af.bitvector = 0;

  af.location = APPLY_HITROLL;
  af.modifier = ch->level / 5;
  affect_to_obj(obj, &af);

  af.location = APPLY_DAMROLL;
  af.modifier = ch->level / 5;
  affect_to_obj(obj, &af);
  af.location = APPLY_HIT;
  af.modifier = ch->level;
  affect_to_obj(obj, &af);
  af.location = APPLY_MANA;
  af.modifier = ch->level;
  affect_to_obj(obj, &af);
  af.location = APPLY_MOVE;
  af.modifier = ch->level;
  affect_to_obj(obj, &af);
  af.location = APPLY_SAVES;
  af.modifier = ch->level / 8;
  affect_to_obj(obj, &af);

  obj_to_char(obj, ch);
  act("$n forges $p.", ch, obj, NULL, TO_ROOM);
  act("You forge $p.", ch, obj, NULL, TO_CHAR);
  send_to_char("200 immquest points have been deducted.\n\r", ch);
  return;
}

CH_CMD(do_reform)
{
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  int amount;

  arg1[0] = '\0';
  arg2[0] = '\0';

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0')
  {
    if (arg2[0] == '\0')
    {
      send_to_char("\n\rSyntax:\n\r", ch);
      send_to_char("reform <type> <amount>\n\r", ch);
      send_to_char("\n\rtypes - AQP IQP\n\r", ch);
      return;
    }
  }

  amount = is_number(arg2) ? atol(arg2) : -1;

  if (!str_prefix(arg1, "AQP"))
  {
    if (IS_NPC(ch))
    {
      printf_to_char(ch, "Mobs have no need to do that");
      return;
    }

    if (amount > ch->pcdata->questpoints)
    {
      printf_to_char(ch,
                     "\n\rHow can you reform %d AQP when you only have %ld?",
                     amount, ch->pcdata->questpoints);
      return;
    }
    if (amount < 0)
    {
      printf_to_char(ch, "\n\rOnly positive amounts allowed...\n\r");
      return;
    }
    if (amount % 20 == 0)
    {
      ch->pcdata->questpoints -= amount;
      ch->qps += amount / 20;
      printf_to_char(ch,
                     "\n\rYou reform %d AQP.  Your now have %d IQP.\n\r",
                     amount, ch->qps);
      return;
    }
    else
    {
      printf_to_char(ch, "\n\rAmount {WMUST{x be divisible by 20\n\r");
      return;
    }
  }

  if (!str_prefix(arg1, "IQP"))
  {
    if (IS_NPC(ch))
    {
      printf_to_char(ch, "Mobs have no need to do that");
      return;
    }

    if (amount < 0 || amount > 200)
    {
      send_to_char
        ("\n\rYou can only reform up to 200 iqp at a time.\n\r", ch);
      return;
    }
    if (amount > ch->qps)
    {
      printf_to_char(ch,
                     "\n\rHow can you reform %d IQP when you only have %ld?\n\r",
                     amount, ch->pcdata->questpoints);
      return;
    }
    if (amount < 0)
    {
      printf_to_char(ch, "\n\rOnly positive amounts allowed...\n\r");
      return;
    }
    ch->qps -= amount;
    ch->pcdata->questpoints += amount * 20;
    printf_to_char(ch,
                   "\n\rYou reform %d IQP.  Your now have %ld AQP.\n\r",
                   amount, ch->pcdata->questpoints);
    return;
  }

  do_reform(ch, "");

  return;
}

struct tier_type
{
  int num;
  long bit;
};

static const struct tier_type tier_table[] = {
  {1, TIER_01},
  {2, TIER_02},
  {3, TIER_03},
  {4, TIER_04},
  {0, 0}
};

CH_CMD(do_reroll)
{
  DESCRIPTOR_DATA *d = NULL;
  char strsave[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
  char player_psswd[MAX_STRING_LENGTH], player_title[MAX_STRING_LENGTH];
  char player_name[MAX_INPUT_LENGTH], player_prmpt[MAX_INPUT_LENGTH];
  int player_rps, player_rpst, player_ree;
  char newbuf[MSL];
  OBJ_DATA *player_banked;
  int player_plat, player_gold, player_slvr, player_balance, player_tier,
    player_comm, player_iqp, player_aqp;
  int iClass, iSecv, wNet, ctier, player_trust;
  bool wasHero = false;

  if (IS_NPC(ch) || (d = ch->desc) == NULL)
    return;

  backup_char_obj(ch);

  if (IS_SET(ch->act2, PLR_MADMIN))
  {
    sprintf(madmin_reroll, "%s", ch->name);
  }
  else
  {
    sprintf(madmin_reroll, "someone");
  }

  if (ch->pcdata->nextquest > 1)
  {
    send_to_char("You can not reroll with a quest timer.\n\r", ch);
    return;
  }

  if ((ch->reroll_timer > 1 && ch->reroll_timer != -1) && ch->level < 25)
  {
    sprintf(buf,
            "You must wait %d more minutes before you may reroll, or make level 25.\n\r",
            ch->reroll_timer);
    send_to_char(buf, ch);
    return;
  }

  if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
  {
    send_to_char("Not while in the arena you dont.\n\r", ch);
    return;
  }

  if (copyover_countdown > 0 && copyover_countdown < 3)
  {
    send_to_char
      ("Sorry, the mud is getting ready for a copyover.\n\rPlease try again later.\n\r",
       ch);
    return;
  }

  if (ch->level < LEVEL_HERO && !IS_SET(ch->act, PLR_REROLL))
  {
    sprintf(buf,
            "You must be level %d or already have rerolled to reroll.\n\r",
            LEVEL_HERO);
    send_to_char(buf, ch);
    return;
  }

  if (ch->level >= LEVEL_HERO)
    wasHero = true;
  iClass = ch->class;

  if (ch->pcdata->confirm_reroll)
  {
    if (argument[0] == '\0')
    {
      send_to_char("Reroll status removed.\n\r", ch);
      ch->pcdata->confirm_reroll = false;
      return;
    }
    else
    {
      if (!is_number(argument))
      {
        send_to_char("Reroll status removed.\n\r", ch);
        ch->pcdata->confirm_reroll = false;
        return;
      }

      /* Make sure ctier is not greater or less than number tiers in
         tier_table */
      if ((ctier = atoi(argument)) <= 0 || ctier > 4)
      {
        send_to_char
          ("That is not a valid tier.  Please type REROLL <tier>.\n\r", ch);
        return;
      }

      if (!IS_SET(ch->pcdata->tier, tier_table[ctier - 1].bit))
      {
        send_to_char
          ("That tier is not available to you.\n\rReroll status removed.\n\r",
           ch);
        ch->pcdata->confirm_reroll = false;
        return;
      }

      if (is_clead(ch))
        update_clanlist(ch, ch->clead, false, true);
      if (is_clan(ch))
        update_clanlist(ch, ch->clan, false, false);
      if (ch->level > ANCIENT)
        update_wizlist(ch, 1);

      /* 
       * Get ready to delete the pfile, send a nice informational message.
       */
      sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
      stop_fighting(ch, true);

      send_to_char
        ("{RYou have chosen to reroll.  You will now be dropped in at the\n\r",
         ch);
      send_to_char
        ("race selection section of character creation, and will be allowed\n\r",
         ch);
      send_to_char
        ("to choose from a wider selection of races and classes.\n\r\n\r",
         ch);
      send_to_char
        ("In the event that you are disconnected or the MUD crashes\n\r", ch);
      send_to_char
        ("while you are creating your character, your pfile is lost.  Create\n\r",
         ch);
      send_to_char
        ("a new character as normal and write a note to 'immortal'.\n\r", ch);
      send_to_char
        ("\n\r\n\r{Y*** {R{zD O  N O T{x{Y  D I S C O N N E C T  W H I L E  R E R O L L I N G ***{x\n\r",
         ch);
      send_to_char
        ("{C*** IF YOU DISCONNECT DURING REROLL YOUR CHARACTER WILL BE LOST!!!{x{C ***{x",
         ch);
      send_to_char("\n\r{R[{WHit Enter to Continue{R]{x\n\r", ch);

      sprintf(log_buf, "%s has rerolled.", ch->name);
      wiznet(log_buf, ch, NULL, WIZ_LOGS, 0, 0);

      sprintf(newbuf, "{w[{RREROLL{w] %s has rerolled.\n\r", ch->name);
      do_gmessage(newbuf);

      sprintf(player_name, "%s", capitalize(ch->name));
      sprintf(player_psswd, "%s", ch->pcdata->pwd);
      sprintf(player_title, "%s", ch->pcdata->title);
      sprintf(player_prmpt, "%s", ch->prompt);
      player_iqp = ch->qps;
      player_aqp = ch->pcdata->questpoints;
      wNet = ch->wiznet;
      iSecv = ch->pcdata->security;
      player_tier = ch->pcdata->tier;
      player_comm = ch->comm;
      player_trust = get_trust(ch);
      player_plat = ch->platinum;
      player_gold = ch->gold;
      player_slvr = ch->silver;
      player_balance = ch->pcdata->balance;
      player_rps = ch->rps;
      player_rpst = ch->rpst;
      player_ree = ch->redeem;
      player_banked = ch->bankeditems;
      ch->bankeditems = NULL;

      /* 
       * "After extract_char the ch is no longer valid!" */
      extract_char(ch, true);
      ch = NULL;
      unlink(strsave);
      sprintf(buf, "rm -f %s.gz", strsave);
      system(buf);

      /* 
       * Delete the pfile, but don't boot the character. * Instead, do
       a load_char_obj to get a new ch, * saving the password, and some 
       variables.  Then, * set the PLR_REROLL bit and drop the player
       in at * CON_BEGIN_REROLL. */
      load_char_obj(d, player_name);

      d->character->gold = player_gold;
      d->character->silver = player_slvr;
      d->character->platinum = player_plat;
      d->character->pcdata->balance = player_balance;
      d->character->prompt = str_dup(player_prmpt);
      d->character->pcdata->pwd = str_dup(player_psswd);
      d->character->pcdata->title = str_dup(player_title);
      d->character->pcdata->socket = str_dup(d->host);
      d->character->pcdata->tier = player_tier;
      d->character->comm = player_comm;
      d->character->trust = player_trust;
      d->character->pcdata->security = iSecv;
      d->character->wiznet = wNet;
      d->character->qps = player_iqp;
      d->character->pcdata->questpoints = player_aqp;
      d->character->rps = player_rps;
      d->character->rpst = player_rpst;
      d->character->redeem = player_ree;
      d->character->bankeditems = player_banked;
      d->character->nameauthed = 1;

      SET_BIT(d->character->pcdata->ctier, tier_table[ctier - 1].bit);
      SET_BIT(d->character->act, PLR_REROLL);

      d->connected = CON_BEGIN_REROLL;
      return;
    }
  }

  if (argument[0] != '\0')
  {
    send_to_char
      ("To set reroll status type REROLL with no arguments.\n\r", ch);
    send_to_char("Proper syntax will be show at that time.\n\r", ch);
    return;
  }

  /* NOTE: these next two statements needed as a catch-all * * to prevent the 
     need of a pwipe, -- Seronis *
     ******************************************************* */
  SET_BIT(d->character->pcdata->tier, TIER_01);
  SET_BIT(d->character->pcdata->tier, class_table[iClass].tier);
  if (wasHero)
    SET_BIT(d->character->pcdata->tier, class_table[iClass].tier_next);

  sprintf(newbuf, "{w[{RREROLL{w] %s is considering rerolling.\n\r",
          ch->name);
  do_gmessage(newbuf);

  send_to_char
    ("Reroll status confirmed.\n\rThis command is NOT REVERSIBLE.\n\r", ch);
  send_to_char("PLEASE READ HELP REROLL BEFORE REROLLING!\n\r", ch);
  send_to_char("You MAY choose a different class or lesser tier!\n\r", ch);
  send_to_char("\n\rSyntax: REROLL <tier> \n\r", ch);
  send_to_char("You are allowed to select from the following\n\r", ch);
  send_to_char("Tiers: ", ch);

  if (IS_SET(ch->pcdata->tier, TIER_01))
    send_to_char("1 ", ch);
  if (IS_SET(ch->pcdata->tier, TIER_02))
    send_to_char("2 ", ch);
  if (IS_SET(ch->pcdata->tier, TIER_03))
    send_to_char("3 ", ch);
  if (IS_SET(ch->pcdata->tier, TIER_04))
    send_to_char("4 ", ch);

  send_to_char("\n\r", ch);

  ch->pcdata->confirm_reroll = true;
  sprintf(log_buf, "%s is contemplating rerolling.", ch->name);
  wiznet(log_buf, ch, NULL, WIZ_LOGS, 0, get_trust(ch));
}

CH_CMD(do_ancient)
{

  if (ch->level < LEVEL_HERO)
  {
    printf_to_char(ch, "{xYou must be level %d to become an Ancient.\n\r",
                   LEVEL_HERO);
    return;
  }

  if (ch->level == 202)
  {
    send_to_char("You are already an Ancient.\n\r", ch);
    return;
  }

  if (IS_NPC(ch))
  {
    send_to_char("Mobs can't become Ancients!\n\r", ch);
    return;
  }

  if (IS_IMMORTAL(ch))
  {
    send_to_char("Immortals have no need to become Ancients!\n\r", ch);
    return;
  }

  if (!IS_SET(class_table[ch->class].tier, TIER_04))
  {

    send_to_char("{xYou must be tier 4 to become an Ancient.\n\r", ch);
    return;
  }

  if (ch->pcdata->questpoints < 4000)
  {

    send_to_char
      ("You must have 4,000 autoquest points to become an Ancient.\n\r", ch);
    return;
  }

  if (ch->qps < 225)
  {
    send_to_char
      ("You must have 225 immquest points to becom an Ancient.\n\r", ch);
    return;
  }

  if (ch->platinum < 2000)
  {
    send_to_char
      ("�You must be carrying 2,000 platinum to become an Ancient.\n\r",
       ch);
    return;
  }

  send_to_char
    ("{RCongratulations you have become an {YA{yn{Yc{yi{Ye{yn{Yt{R!!!{x\n\r",
     ch);

  backup_char_obj(ch);
  do_announce(ch, "has become an {YA{yn{Yc{yi{Ye{yn{Yt{x!!!\n\r");
  ch->level += 1;
  ch->pcdata->questpoints -= 4000;
  ch->qps -= 225;
  ch->platinum -= 2000;
  ch->max_hit += 10000;
  ch->max_mana += 10000;
  ch->max_move += 10000;
  ch->pcdata->perm_hit += 10000;
  ch->pcdata->perm_mana += 10000;
  ch->pcdata->perm_move += 10000;
  save_char_obj(ch);

  return;
}

/* show suppression settings - Gregor Stipicic aka Fade, 2001 */
CH_CMD(do_verbose)
{
#define VER_OO(ch,flag) (!IS_SET((ch)->pcdata->verbose,(flag))?" {y({YON{y){x":"{b({BOFF{b){x")
  char arg[MAX_INPUT_LENGTH];
  arg[0] = '\0';

  argument = one_argument(argument, arg);

  if (IS_NPC(ch))
    return;

  if (arg[0] == '\0')
  {
    send_to_char("{CV{cerbose{x settings{D:\n\r", ch);
    printf_to_char(ch,
                   "{CW{ceapon flags{D  :    %s\n\r{CD{codge/etc.  {D  :    %s{x\n\r",
                   VER_OO(ch, VERBOSE_FLAGS), VER_OO(ch, VERBOSE_DODGE));

    printf_to_char(ch, "{CO{cbject effects{D:    %s{x\n\r",
                   VER_OO(ch, VERBOSE_BURN));

    printf_to_char(ch, "{cShields are %scompressed into one line.{x\n\r",
                   IS_SET(ch->pcdata->verbose, VERBOSE_SHIELD) ? "" : "not ");
    return;
  }
  if (!str_prefix(arg, "weapon"))
  {
    if (IS_SET(ch->pcdata->verbose, VERBOSE_FLAGS))
    {
      REMOVE_BIT(ch->pcdata->verbose, VERBOSE_FLAGS);
      send_to_char("{cWeapon flags are no longer suppressed.{x\n\r", ch);
    }
    else
    {
      SET_BIT(ch->pcdata->verbose, VERBOSE_FLAGS);
      send_to_char("{cWeapon flags are suppressed now.{x\n\r", ch);
    }
    return;
  }
  if (!str_prefix(arg, "dodge"))
  {
    if (IS_SET(ch->pcdata->verbose, VERBOSE_DODGE))
    {
      REMOVE_BIT(ch->pcdata->verbose, VERBOSE_DODGE);
      send_to_char
        ("{cDodges, parries, and shield blocks are no longer suppressed.{x\n\r",
         ch);
    }
    else
    {
      SET_BIT(ch->pcdata->verbose, VERBOSE_DODGE);
      send_to_char
        ("{cDodges, parries, and shield blocks are suppressed now.{x\n\r",
         ch);
    }
    return;
  }

  if (!str_prefix(arg, "damage"))
  {
    if (IS_SET(ch->pcdata->verbose, VERBOSE_DAMAGE))
    {
      REMOVE_BIT(ch->pcdata->verbose, VERBOSE_DAMAGE);
      send_to_char("{cDamage messages are no longer summarized.{x\n\r", ch);
    }
    return;
  }

  if (!str_prefix(arg, "object"))
  {
    if (IS_SET(ch->pcdata->verbose, VERBOSE_BURN))
    {
      REMOVE_BIT(ch->pcdata->verbose, VERBOSE_BURN);
      send_to_char
        ("{cObject effects (burning potions, etc.) are no longer suppressed.{x\n\r",
         ch);
    }
    else
    {
      SET_BIT(ch->pcdata->verbose, VERBOSE_BURN);
      send_to_char
        ("{cObject effects (burning potions, etc.) are suppressed now.{x\n\r",
         ch);
    }
    return;
  }
  if (!str_prefix(arg, "shields"))
  {
    if (IS_SET(ch->pcdata->verbose, VERBOSE_SHIELD))
    {
      REMOVE_BIT(ch->pcdata->verbose, VERBOSE_SHIELD);
      SET_BIT(ch->pcdata->verbose, VERBOSE_SHIELD_COMP);
      send_to_char
        ("{cShields are no longer compressed into one line.{x\n\r", ch);
    }
    else
    {
      SET_BIT(ch->pcdata->verbose, VERBOSE_SHIELD);
      REMOVE_BIT(ch->pcdata->verbose, VERBOSE_SHIELD_COMP);
      send_to_char("{cShields are compressed into one line now.{x\n\r", ch);
    }
    return;
  }
  send_to_char
    ("{cSyntax{D: {CVerbose {cweapon{D/{cflags{D/{cdodge{D/{cdamage{D/{cobject{D/{cshields{x\n\r",
     ch);
  return;
}

/* RT code to display channel status */

CH_CMD(do_channels)
{
  char buf[MAX_STRING_LENGTH];

  /* lists all channels and their status */
  send_to_char("\n\r{WChannel{x       {RStatus{x\n\r", ch);
  send_to_char("{x-------{D---------{W=========={D-------{x---------\n\r",
               ch);

  send_to_char("{WIC{x            ", ch);
  if (!IS_SET(ch->comm, COMM_NOGOSSIP))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WOOC{x           ", ch);
  if (!IS_SET(ch->comm, COMM_NOOOC))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  if (ch->level >= 202)
  {
    send_to_char("{WAncient{x       ", ch);
    if (!IS_SET(ch->comm, COMM_ANCTALK))
      send_to_char("{w[{GON{w]{x\n\r", ch);
    else
      send_to_char("{w[{ROFF{w]\n\r", ch);
  }

  send_to_char("{WPray          {x", ch);
  send_to_char("{w[{GON{w]{x\n\r", ch);

  send_to_char("{WRace Talk     {x", ch);
  if (!IS_SET(ch->comm, COMM_NORACE))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WSocial{x        ", ch);
  if (!IS_SET(ch->comm, COMM_NOSOCIAL))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WClan Gossip{x   ", ch);
  if (!IS_SET(ch->comm, COMM_NOCGOSSIP))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WQuest Gossip{x  ", ch);
  if (!IS_SET(ch->comm, COMM_NOCGOSSIP))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WMusic{x         ", ch);
  if (!IS_SET(ch->comm, COMM_NOMUSIC))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WQ/A           {x", ch);
  if (!IS_SET(ch->comm, COMM_NOASK))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WQuote {x        ", ch);
  if (!IS_SET(ch->comm, COMM_NOQUOTE))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WGlobal Emote{x  ", ch);
  if (!IS_SET(ch->comm, COMM_NOGMOTE))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WGrats{x         ", ch);
  if (!IS_SET(ch->comm, COMM_NOGRATS))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  if (IS_IMMORTAL(ch))
  {
    send_to_char("{WImmchat{x       ", ch);
    if (!IS_SET(ch->comm, COMM_NOWIZ))
      send_to_char("{w[{GON{w]{x\n\r", ch);
    else
      send_to_char("{w[{ROFF{w]\n\r", ch);

    if (IS_IMMORTAL(ch))
    {
      send_to_char("{WWiznet{x        ", ch);
      if (IS_SET(ch->wiznet, WIZ_ON))
        send_to_char("{w[{GON{w]{x\n\r", ch);
      else
        send_to_char("{w[{ROFF{w]\n\r", ch);
    }

  }

  send_to_char("{WShouts{x        ", ch);
  if (!IS_SET(ch->comm, COMM_NOSHOUT))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WTells{x         ", ch);
  if (!IS_SET(ch->comm, COMM_DEAF))
    send_to_char("{w[{GON{w]{x\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r", ch);

  send_to_char("{WQuiet Mode{x    ", ch);
  if (IS_SET(ch->comm, COMM_QUIET))
    send_to_char("{w[{GON{w]{x\n\r\n\r", ch);
  else
    send_to_char("{w[{ROFF{w]\n\r\n\r", ch);

  send_to_char("{x-------{D---------{W=========={D-------{x---------\n\r",
               ch);

  if (IS_SET(ch->comm, COMM_AFK))
    send_to_char("{WYou are {RAFK{W.{x\n\r", ch);

  if (IS_SET(ch->comm, COMM_STORE))
    send_to_char("{WYou store tells during fights.{x\n\r", ch);

  if (IS_SET(ch->comm, COMM_SNOOP_PROOF))
    send_to_char("{WYou are immune to snooping.{x\n\r", ch);

  if (ch->lines != PAGELEN)
  {
    if (ch->lines)
    {
      sprintf(buf, "{WYou display{Y %d{W lines of scroll.{x\n\r",
              ch->lines + 2);
      send_to_char(buf, ch);
    }
    else
      send_to_char("{WScroll buffering is {w[{ROFF{w].{x\n\r", ch);
  }

  if (ch->prompt != NULL)
  {
    sprintf(buf, "{WYour current prompt is:{x %s{x\n\r", ch->prompt);
    send_to_char(buf, ch);
  }

  sprintf(buf,
          "{WYour current talk color is {R%d{W.{x\n\rSee the tcolor command.\n\r",
          ch->talk_color);
  send_to_char(buf, ch);

  if (IS_SET(ch->comm, COMM_NOSHOUT))
    send_to_char("{WYou cannot shout.{x\n\r", ch);

  if (IS_SET(ch->comm, COMM_NOTELL))
    send_to_char("{WYou cannot use tell.{x\n\r", ch);

  if (IS_SET(ch->comm, COMM_NOCHANNELS))
    send_to_char("{WYou cannot use channels.{x\n\r", ch);

  if (IS_SET(ch->comm, COMM_NOEMOTE))
    send_to_char("{WYou cannot show emotions.{x\n\r", ch);

}

/* RT deaf blocks out all shouts */

CH_CMD(do_deaf)
{

  if (IS_SET(ch->comm, COMM_DEAF))
  {
    send_to_char("You can now hear tells again.\n\r", ch);
    REMOVE_BIT(ch->comm, COMM_DEAF);
  }
  else
  {
    send_to_char("From now on, you won't hear tells.\n\r", ch);
    SET_BIT(ch->comm, COMM_DEAF);
  }
}

/* RT quiet blocks out all communication */

CH_CMD(do_quiet)
{
  if (IS_SET(ch->comm, COMM_QUIET))
  {
    send_to_char("Quiet mode removed.\n\r", ch);
    REMOVE_BIT(ch->comm, COMM_QUIET);
  }
  else
  {
    send_to_char("From now on, you will only hear says and emotes.\n\r", ch);
    SET_BIT(ch->comm, COMM_QUIET);
  }
}

void clean_char_flags(CHAR_DATA * ch)
{
  if (arena == FIGHT_OPEN)
  {
    REMOVE_BIT(ch->act2, PLR2_CHALLENGED);
    REMOVE_BIT(ch->act2, PLR2_CHALLENGER);
    REMOVE_BIT(ch->act2, PLR2_CONSENT);
  }
}

CH_CMD(do_busy)
{

  if (IS_SET(ch->act2, PLR2_BUSY))
  {
    send_to_char("You are no longer marked as being busy.\n\r", ch);
    REMOVE_BIT(ch->act2, PLR2_BUSY);
  }
  else
  {
    send_to_char("You are now marked as being busy.\n\r", ch);
    SET_BIT(ch->act2, PLR2_BUSY);
  }

}

/* afk command */
CH_CMD(do_afk)
{
  char buf[MAX_STRING_LENGTH];

  if (IS_SET(ch->comm, COMM_AFK))
  {
    if (ch->tells)
    {
      sprintf(buf,
              "AFK mode removed.  You have {R%d{x tells waiting.\n\r",
              ch->tells);
      send_to_char(buf, ch);
      send_to_char("Type 'replay' to see tells.\n\r", ch);
    }
    else
    {
      send_to_char("AFK mode removed.  You have no tells waiting.\n\r", ch);
    }
    REMOVE_BIT(ch->comm, COMM_AFK);

  }
  else
  {
    SET_BIT(ch->comm, COMM_AFK);
    send_to_char("\n\rYou are now in AFK mode.\n\r", ch);
  }
}

CH_CMD(do_autostore)
{
  if (IS_SET(ch->comm, COMM_STORE))
  {
    send_to_char("You will no longer store tells during fights.\n\r", ch);
    REMOVE_BIT(ch->comm, COMM_STORE);
  }
  else
  {
    send_to_char("You will now store tells during fights.\n\r", ch);
    SET_BIT(ch->comm, COMM_STORE);
  }
}

CH_CMD(do_replay)
{
  if (IS_NPC(ch))
  {
    send_to_char("You can't replay.\n\r", ch);
    return;
  }

  if (buf_string(ch->pcdata->buffer)[0] == '\0')
  {
    send_to_char("You have no tells to replay.\n\r", ch);
    return;
  }

  send_to_char(buf_string(ch->pcdata->buffer), ch);
  clear_buf(ch->pcdata->buffer);
  ch->tells = 0;
}

CH_CMD(do_racetalk)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  if (IS_NPC(ch))
    return;

  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NORACE))
    {
      send_to_char("Race channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NORACE);
    }
    else
    {
      send_to_char("Race channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_NORACE);
    }
  }
  else
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channels priviliges.\n\r", ch);
      return;
    }
    if (IS_SHIELDED(ch, SHD_SILENCE))
    {
      send_to_char("You are silent and can not speak.", ch);
      return;
    }
    argument = makedrunk(argument, ch);
    argument = pcolor(ch, argument, 0);
    REMOVE_BIT(ch->comm, COMM_NORACE);

    sprintf(buf, "{W$n {c-{C={D%s{C={c-{x '{r$t{x'",
            pc_race_table[ch->race].name);
    act(buf, ch, argument, NULL, TO_CHAR);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && victim != ch &&
          (victim->race == ch->race || IS_IMMORTAL(victim)) &&
          !IS_NPC(victim) && !IS_SET(victim->comm, COMM_NORACE) &&
          !IS_SET(victim->comm, COMM_QUIET))
      {
        act(buf, ch, argument, d->character, TO_VICT);
      }
    }
  }
}

CH_CMD(do_pray)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (argument[0] == '\0')
  {
    if (IS_IMMORTAL(ch))
    {
      if (IS_SET(ch->comm, COMM_PRAY))
      {
        send_to_char("Pray channel is now {w[{GON{w]{x.\n\r", ch);
        REMOVE_BIT(ch->comm, COMM_PRAY);
      }
      else
      {
        send_to_char("Pray channel is now {w[{ROFF{w].\n\r", ch);
        SET_BIT(ch->comm, COMM_PRAY);
      }
    }
    else
    {
      send_to_char
        ("Mortals do not have the option of being deaf to the gods.\n\r", ch);
    }
  }
  else                          /* flame message sent, turn gossip on if it
                                   isn't already */
  {

    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges{x\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }
    argument = makedrunk(argument, ch);
    argument = pcolor(ch, argument, 0);
    REMOVE_BIT(ch->comm, COMM_PRAY);

    if (IS_IMMORTAL(ch) || IS_SET(ch->act2, PLR_MADMIN))
    {
      sprintf(buf,
              "{D-{r={R*{Y) You Boom {Y({R*{r={D- {D'{W%s{D'{x\n\r",
              argument);
    }
    else
    {
      sprintf(buf,
              "{r[{=P{=r{=a{=y{r] {r-{R={DYou{R={r- {r'{D%s{r'{x\n\r",
              argument);
    }

    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_PRAY) && (victim->level >= 204
                                               || ch->level >= 204 ||
                                               IS_SET(victim->act2,
                                                      PLR_MADMIN)
                                               || IS_SET(ch->act2,
                                                         PLR_MADMIN)))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found && (IS_IMMORTAL(ch) || IS_SET(ch->act2, PLR_MADMIN)))
        {
          act_new("{D-{r={R*{Y) $n Booms {Y({R*{r={D- {D'{W$t{D'{x",
                  ch, argument, d->character, TO_VICT, POS_SLEEPING);
        }
        else
        {
          act_new
            ("{r[{=P{=r{=a{=y{r] {r-{R={D$n{R={r- {r'{D$t{r'{x",
             ch, argument, d->character, TO_VICT, POS_SLEEPING);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));

  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);

}

CH_CMD(do_anctalk)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;

  if (ch->level < 202)
    return;

  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_ANCTALK))
    {
      send_to_char("Ancient channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_ANCTALK);
    }
    else
    {
      send_to_char("Ancient channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_ANCTALK);
    }
  }
  else                          /* flame message sent, turn gossip on if it
                                   isn't already */
  {

    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges{x\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }
    argument = makedrunk(argument, ch);
    argument = pcolor(ch, argument, 0);
    REMOVE_BIT(ch->comm, COMM_ANCTALK);

    sprintf(buf,
            "{W[{+A{+n{+c{+i{+e{+n{+t{W] {r-{R={WYou{R={r- {W'{D%s{W'{x\n\r",
            argument);

    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_ANCTALK) &&
          !IS_SET(victim->comm, COMM_QUIET) && victim->level >= 202)
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        act_new
          ("{W[{+A{+n{+c{+i{+e{+n{+t{W] {r-{R={W$n{R={r- {W'{D$t{W'{x",
           ch, argument, d->character, TO_VICT, POS_SLEEPING);
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));

  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);

}

char *pcolor(CHAR_DATA * ch, const char *s, int tcolor)
{

  char *p = new_arg;
  int ttemp;
  int skip;

  tctoggle = false;
  skip = 0;
  ttemp = 0;
  sprintf(new_arg, " ");

  if (ch != NULL)
    ttemp = ch->talk_color;
  else
    ttemp = tcolor;

  if (ttemp == 12)
    ttemp = number_range(1, 11);

  for (; *s != '\0'; ++s)
    //        if ( !isspace ( *s ) && *s != '{' )
    if (skip == 0)
    {
      if (!isspace(*s))
      {
        if (tctoggle)
        {
          tctoggle = false;
        }
        else
        {
          tctoggle = true;
        }

        if (*s == '{')
          skip = 1;

        if (skip == 0)
        {
          if (ttemp != 0 && tctoggle)
            *p++ = '{';

          if (ttemp == 1 && tctoggle)
            *p++ = '=';

          if (ttemp == 2 && tctoggle)
            *p++ = '-';

          if (ttemp == 3 && tctoggle)
            *p++ = '+';

          if (ttemp == 4 && tctoggle)
            *p++ = '*';

          if (ttemp == 5 && tctoggle)
            *p++ = 'n';

          if (ttemp == 6 && tctoggle)
            *p++ = 'p';

          if (ttemp == 7 && tctoggle)
            *p++ = 'I';

          if (ttemp == 8 && tctoggle)
            *p++ = 'q';

          if (ttemp == 9 && tctoggle)
            *p++ = 'd';

          if (ttemp == 10 && tctoggle)
            *p++ = 'o';

          if (ttemp == 11 && tctoggle)
            *p++ = 'O';

          *p++ = *s;
        }
      }
      else
      {
        *p++ = *s;
      }
    }
    else
    {
      if (skip > 0)
        --skip;
    }
  *p = '\0';

  return new_arg;
}

char *pcolorr(CHAR_DATA * ch, const char *g, int tcolor)
{
  char *q = new_argg;
  int ttemp;
  int skip;

  tctogglee = false;
  skip = 0;
  ttemp = 0;
  sprintf(new_arg, " ");

  if (ch != NULL)
    ttemp = ch->talk_color;
  else
    ttemp = tcolor;

  if (ttemp == 12)
    ttemp = number_range(1, 11);

  for (; *g != '\0'; ++g)
    if (skip == 0)
    {
      if (!isspace(*g))
      {
        if (tctoggle)
        {
          tctogglee = false;
        }
        else
        {
          tctogglee = true;
        }

        if (*g == '{')
          skip = 1;

        if (skip == 0)
        {
          if (ttemp != 0 && tctogglee)
            *q++ = '{';

          if (ttemp == 1 && tctogglee)
            *q++ = '=';

          if (ttemp == 2 && tctogglee)
            *q++ = '-';

          if (ttemp == 3 && tctogglee)
            *q++ = '+';

          if (ttemp == 4 && tctogglee)
            *q++ = '*';

          if (ttemp == 5 && tctogglee)
            *q++ = 'n';

          if (ttemp == 6 && tctogglee)
            *q++ = 'p';

          if (ttemp == 7 && tctogglee)
            *q++ = 'I';

          if (ttemp == 8 && tctogglee)
            *q++ = 'q';

          if (ttemp == 9 && tctogglee)
            *q++ = 'd';

          if (ttemp == 10 && tctogglee)
            *q++ = 'o';

          if (ttemp == 11 && tctogglee)
            *q++ = 'O';

          *q++ = *g;
        }
      }
      else
      {
        *q++ = *g;
      }
    }
    else
    {
      if (skip > 0)
        --skip;
    }
  *q = '\0';

  return new_argg;
}

char *fucktext(CHAR_DATA * ch, const char *s)
{
  char *p = fuck_arg;

  sprintf(fuck_arg, " ");

  for (; *s != '\0'; ++s)
    if (*s != 'a' && *s != 'e' && *s != 'i' && *s != 'o' && *s != 'u' &&
        *s != 'y' && *s != 'A' && *s != 'E' && *s != 'I' && *s != 'O' &&
        *s != 'U' && *s != 'Y')
    {
      *p++ = *s;
    }

  *p = '\0';
  return fuck_arg;
}

CH_CMD(do_tcolor)
{
  char buf[MSL];
  int pcolornum;

  if (IS_NPC(ch))
    return;

  if (argument[0] == '\0')
  {
    send_to_char
      ("\n\r\n\r{x 0: None\n\r 1: {=D{=a{=r{=k {=F{=l{=a{=m{=e{x\n\r 2: {-S{-h{-i{-n{-i{-n{-g {-I{-c{-e{x\n\r 3: {+D{+a{+r{+k {+W{+h{+i{+s{+p{+e{+r{+s{x\n\r 4: {*M{*i{*d{*n{*i{*g{*h{*t {*S{*u{*n{x\n\r 5: {nS{nt{nr{na{nn{ng{ne {nF{no{nr{nr{ne{ns{nt{x\n\r 6: {pA{pm{pb{pe{pr {pR{pe{pf{pl{pe{pc{pt{pi{po{pn{ps{x\n\r 7: {IS{Io{Iu{Il{Il{Ie{Is{Is {IB{Ir{Ie{Ia{It{Ih{x\n\r 8: {qA{qr{qc{qa{qn{qe {qM{qi{qs{qt{x\n\r 9: {dW{dr{de{dt{dc{dh{de{dd {dB{de{dt{dr{da{dy{da{dl{x\n\r10: {oO{ol{od {oH{oe{or{oo{x\n\r11: {OD{Oe{Om{Oo{On{O\'s {OT{Oo{On{Og{Ou{Oe{w\n\r12: Random tcolor each line.\n\r\n\r",
       ch);
    sprintf(buf,
            "Your current color pattern number is %d.\n\rTcolor skips any messages with color codes in it, like say {{xHello there!\n\r\n\r",
            ch->talk_color);
    send_to_char(buf, ch);
    return;
  }

  if (!is_number(argument))
  {
    send_to_char("Numbers only please.\n\r", ch);
    return;
  }

  pcolornum = atoi(argument);
  if (pcolornum >= 0 && pcolornum <= 12)
  {
    ch->talk_color = pcolornum;
    send_to_char("Your color has been set.\n\r", ch);
    return;
  }

}

/* RT ooc rewritten in ROM style */
CH_CMD(do_ooc)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOOOC))
    {
      send_to_char("Ooc channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOOOC);
    }
    else
    {
      send_to_char("Ooc channel is now {ROFF{.\n\r", ch);
      SET_BIT(ch->comm, COMM_NOOOC);
    }
  }
  else                          /* ooc message sent, turn ooc on if it is off */
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }

    REMOVE_BIT(ch->comm, COMM_NOOOC);

    argument = makedrunk(argument, ch);

    argument = pcolor(ch, argument, 0);

    if (is_blinky(argument))
    {
      send_to_char("You can not use blink in ooc.\n\r", ch);
      return;
    }

    sprintf(buf, "{W[{pO{pO{pC{W] {g-{G={wYou{G={g- {W'{c%s{W'{x\n\r",
            argument);
    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_NOOOC) &&
          !IS_SET(victim->comm, COMM_QUIET))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found)
        {
          act_new("{W[{pO{pO{pC{W] {g-{G={w$n{G={g- {W'{c$t{W'{x",
                  ch, argument, d->character, TO_VICT, POS_DEAD);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));

  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);

}

/* RT chat replaced with ROM gossip */
CH_CMD(do_gossip)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOGOSSIP))
    {
      send_to_char("IC channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOGOSSIP);
    }
    else
    {
      send_to_char("IC channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_NOGOSSIP);
    }
  }
  else                          /* gossip message sent, turn gossip on if it
                                   isn't already */
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }
    if (IS_SHIELDED(ch, SHD_SILENCE))
    {
      send_to_char("You are silent and can not speak.", ch);
      return;
    }

    if (is_blinky(argument))
    {
      send_to_char("You can not use blink in gossip.\n\r", ch);
      return;
    }

    REMOVE_BIT(ch->comm, COMM_NOGOSSIP);
    if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)

      argument = makedrunk(argument, ch);

    argument = pcolor(ch, argument, 0);

    sprintf(buf,
            "{W[{=I{=n {=C{=h{=a{=r{=a{=c{=t{=e{=r{W] {y-{Y={BYou{Y={y- {W'{D%s{W'{x",
            argument);
    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_NOGOSSIP) &&
          !IS_SET(victim->comm, COMM_QUIET))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found)
        {
          //MORE DRUNK
          if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
            argument = makedrunk(argument, ch);
          //END
          act_new
            ("{W[{=I{=n {=C{=h{=a{=r{=a{=c{=t{=e{=r{W] {y-{Y={B$n{Y={y- {W'{D$t{W'{x",
             ch, argument, d->character, TO_VICT, POS_SLEEPING);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);
}

CH_CMD(do_qgossip)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (!ch->on_quest && !IS_IMMORTAL(ch))
  {
    send_to_char("You are not on a quest!\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOQGOSSIP))
    {
      send_to_char("Quest gossip channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOQGOSSIP);
    }
    else
    {
      send_to_char("Quest gossip channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_NOQGOSSIP);
    }
  }
  else                          /* qgossip message sent, turn qgossip on if it
                                   isn't already */
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }

    if (is_blinky(argument))
    {
      send_to_char("You can not use blink in qgossip.\n\r", ch);
      return;
    }

    argument = makedrunk(argument, ch);

    argument = pcolor(ch, argument, 0);
    REMOVE_BIT(ch->comm, COMM_NOQGOSSIP);

    sprintf(buf, "You qgossip '{C%s{x'\n\r", argument);
    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_NOQGOSSIP) &&
          !IS_SET(victim->comm, COMM_QUIET) && ((victim->on_quest)
                                                || IS_IMMORTAL(victim)))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found)
        {
          act_new("$n qgossips '{C$t{x'", ch, argument,
                  d->character, TO_VICT, POS_SLEEPING);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);
}

CH_CMD(do_grats)
{
  char buf[MAX_STRING_LENGTH];
  char bufy[MAX_STRING_LENGTH];
  char bufz[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  buf[0] = '\0';
  bufy[0] = '\0';
  bufz[0] = '\0';
  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOGRATS))
    {
      send_to_char("Grats channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOGRATS);
    }
    else
    {
      send_to_char("Grats channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_NOGRATS);
    }
  }
  else                          /* grats message sent, turn grats on if it
                                   isn't already */
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }

    if (is_blinky(argument))
    {
      send_to_char("You can not use blink on grats.\n\r", ch);
      return;
    }

    argument = makedrunk(argument, ch);

    argument = pcolor(ch, argument, 0);
    REMOVE_BIT(ch->comm, COMM_NOGRATS);

    sprintf(buf,
            "{W[{RC{Yo{Rn{Yg{Rr{Ya{Rt{Ys{W] {R-{Y={BYou{Y={R- {W'{Y%s{W'{x",
            argument);
    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_NOGRATS) &&
          !IS_SET(victim->comm, COMM_QUIET))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found)
        {
          act_new
            ("{W[{RC{Yo{Rn{Yg{Rr{Ya{Rt{Ys{W] {R-{Y={B$n{Y={R- {W'{Y$t{W'{x",
             ch, argument, d->character, TO_VICT, POS_SLEEPING);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);
}

CH_CMD(do_quote)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOQUOTE))
    {
      send_to_char("Quote channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOQUOTE);
    }
    else
    {
      send_to_char("Quote channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_NOQUOTE);
    }
  }
  else                          /* quote message sent, turn quote on if it
                                   isn't already */
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

    if (is_blinky(argument))
    {
      send_to_char("You can not use blink in quote.\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }
    argument = makedrunk(argument, ch);

    argument = pcolor(ch, argument, 0);
    REMOVE_BIT(ch->comm, COMM_NOQUOTE);

    sprintf(buf,
            "{W[{=Q{=u{=o{=t{=e{W] {D-{r={RYou{r={D- {W'{w%s{W'{x\n\r",
            argument);
    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_NOQUOTE) &&
          !IS_SET(victim->comm, COMM_QUIET))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found)
        {
          act_new
            ("{W[{=Q{=u{=o{=t{=e{W] {D-{r={R$n{r={D- {W'{w$t{W'{x",
             ch, argument, d->character, TO_VICT, POS_SLEEPING);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);
}

CH_CMD(do_gmote)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOGMOTE))
    {
      send_to_char("Global emote channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOGMOTE);
    }
    else
    {
      send_to_char("Global emote channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_NOGMOTE);
    }
  }
  else                          /* quote message sent, turn quote on if it
                                   isn't already */
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

    if (is_blinky(argument))
    {
      send_to_char("You can not use blink in gmote.\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }

    REMOVE_BIT(ch->comm, COMM_NOQUOTE);

    sprintf(buf, "{C[{-G{-l{-o{-b{-a{-l {-E{-m{-o{-t{-e{C] {w%s %s{x\n\r",
            ch->name, argument);
    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_NOQUOTE) &&
          !IS_SET(victim->comm, COMM_QUIET))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found)
        {
          act_new
            ("{C[{-G{-l{-o{-b{-a{-l {-E{-m{-o{-t{-e{C] {w$n $t{x",
             ch, argument, d->character, TO_VICT, POS_SLEEPING);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);
}

void social_channel(const char *format, CHAR_DATA * ch, const void *arg2,
                    int type)
{
  CHAR_DATA *to;
  CHAR_DATA *vch = (CHAR_DATA *) arg2;
  const char *str;
  char *i;
  char *point;
  char buf[MAX_STRING_LENGTH];

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
  {
    send_to_char
      ("Just keep your nose in the corner like a good little player.\n\r",
       ch);
    return;
  }

  if (IS_SET(ch->comm, COMM_QUIET))
  {
    send_to_char("You must turn off quiet mode first.\n\r", ch);
    return;
  }

  if (IS_SET(ch->comm, COMM_NOCHANNELS))
  {
    send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
    return;
  }

  if (!format || !*format)
    return;

  if (!ch || !ch->desc || !ch->in_room)
    return;

  to = char_list;
  if (type == TO_VICT)
  {
    if (!vch)
    {
      bug("Act: null vch with TO_VICT.", 0);
      return;
    }

    if (!vch->in_room)
      return;

    to = vch;
  }
  if (type == TO_CHAR)
  {
    if (ch == NULL)
    {
      return;
    }
    to = ch;
  }

  for (; to; to = to->next)
  {
    if (!to->desc || to->position < POS_RESTING || !to->in_room)
      continue;

    if (type == TO_CHAR && to != ch)
      continue;
    if (type == TO_VICT && (to != vch || to == ch))
      continue;
    if (type == TO_ROOM && to == ch)
      continue;
    if (type == TO_NOTVICT && (to == ch || to == vch))
      continue;
    if (IS_SET(to->comm, COMM_NOSOCIAL) || IS_SET(to->comm, COMM_QUIET))
      continue;
    point = buf;
    sprintf(buf, "[*SOC*] ");
    point += 8;
    str = format;
    while (*str)
    {
      if (*str != '$')
      {
        *point++ = *str++;
        continue;
      }

      i = NULL;
      switch (*str)
      {
        case '$':
          ++str;
          i = " <@@@> ";
          if (!arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G')
          {
            bug("Act: missing arg2 for code %d.", *str);
            i = " <@@@> ";
          }
          else
          {
            switch (*str)
            {
              default:
                bug("Act: bad code %d.", *str);
                i = " <@@@> ";
                break;

              case 'T':
                i = (char *) arg2;
                break;

              case 'n':
                i = PERS(ch, to);
                break;

              case 'N':
                i = PERS(vch, to);
                break;

              case 'e':
                i = he_she[URANGE(0, ch->sex, 2)];
                break;

              case 'E':
                i = he_she[URANGE(0, vch->sex, 2)];
                break;

              case 'm':
                i = him_her[URANGE(0, ch->sex, 2)];
                break;

              case 'M':
                i = him_her[URANGE(0, vch->sex, 2)];
                break;

              case 's':
                i = his_her[URANGE(0, ch->sex, 2)];
                break;

              case 'S':
                i = his_her[URANGE(0, vch->sex, 2)];
                break;

            }
          }
          break;

        default:
          *point++ = *str++;
          break;
      }

      ++str;

      while ((*point = *i) != '\0')
        ++point, ++i;
    }

    *point++ = '\n';
    *point++ = '\r';
    *point = '\0';
    buf[0] = UPPER(buf[0]);
    send_to_char("{W", to);
    if (to->desc)
      write_to_buffer(to->desc, buf, point - buf);
    send_to_char("{x", to);
  }
  return;
}

/* RT ask channel */
CH_CMD(do_ask)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOASK))
    {
      send_to_char("Q/A channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOASK);
    }
    else
    {
      send_to_char("Q/A channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_NOASK);
    }
  }
  else                          /* ask sent, turn Q/A on if it isn't already */
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

    if (is_blinky(argument))
    {
      send_to_char("You can not use blink on ask.\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }
    argument = makedrunk(argument, ch);
    argument = pcolor(ch, argument, 0);
    REMOVE_BIT(ch->comm, COMM_NOASK);

    sprintf(buf,
            "{W[{YQ{y&{YA {W- {YA{ysk{W] {Y-{W={wYou{W={Y- {W'{y%s{W'{x",
            argument);
    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_NOASK) &&
          !IS_SET(victim->comm, COMM_QUIET))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found)
        {
          act_new
            ("{W[{YQ{y&{YA {W- {YA{ysk{W] {Y-{W={w$n{W={Y- {W'{y$t{W'{x",
             ch, argument, d->character, TO_VICT, POS_SLEEPING);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);
}

/* RT answer channel - uses same line as asks */
CH_CMD(do_answer)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOASK))
    {
      send_to_char("Q/A channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOASK);
    }
    else
    {
      send_to_char("Q/A channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_NOASK);
    }
  }
  else                          /* answer sent, turn Q/A on if it isn't already 
                                 */
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }

    if (is_blinky(argument))
    {
      send_to_char("You can not use blink in answer.\n\r", ch);
      return;
    }

    argument = makedrunk(argument, ch);
    argument = pcolor(ch, argument, 0);
    REMOVE_BIT(ch->comm, COMM_NOASK);

    sprintf(buf,
            "{W[{YQ{y&{YA{W - {YA{ynswer{W] {Y-{W={wYou{W={Y- {W'{y%s{W'{x",
            argument);
    send_to_char(buf, ch);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_NOASK) &&
          !IS_SET(victim->comm, COMM_QUIET))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found)
        {
          act_new
            ("{W[{YQ{y&{YA{W - {YA{ynswer{W] {Y-{W={w$n{W={Y- {W'{y$t{W'{x",
             ch, argument, d->character, TO_VICT, POS_SLEEPING);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);
}

/* RT music channel */
CH_CMD(do_music)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int wtime;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOMUSIC))
    {
      send_to_char("Music channel is now {w[{GON{w]{x.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOMUSIC);
    }
    else
    {
      send_to_char("Music channel is now {w[{ROFF{w].\n\r", ch);
      SET_BIT(ch->comm, COMM_NOMUSIC);
    }
  }
  else                          /* music sent, turn music on if it isn't
                                   already */
  {
    if (IS_SET(ch->comm, COMM_QUIET))
    {
      send_to_char("You must turn off quiet mode first.\n\r", ch);
      return;
    }

    if (IS_SET(ch->comm, COMM_NOCHANNELS))
    {
      send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
      return;
    }

    if (is_blinky(argument))
    {
      send_to_char("You can not use blink in music.\n\r", ch);
      return;
    }

    if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
    {
      send_to_char
        ("Just keep your nose in the corner like a good little player.\n\r",
         ch);
      return;
    }

    argument = makedrunk(argument, ch);
    argument = pcolor(ch, argument, 0);
    REMOVE_BIT(ch->comm, COMM_NOMUSIC);

    sprintf(buf,
            "{W[{-M{-u{-s{-i{-c{W] {C-{W={BYou{W={C- {W'{w%s{W'{x\n\r",
            argument);
    send_to_char(buf, ch);
    sprintf(buf, "{W[{-M{-u{-s{-i{-c{W] {C-{W={B$n{W={C- {W'{w%s{W'{x",
            argument);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *victim;
      int pos;
      bool found = false;

      victim = d->original ? d->original : d->character;

      if (d->connected == CON_PLAYING && d->character != ch &&
          !IS_SET(victim->comm, COMM_NOMUSIC) &&
          !IS_SET(victim->comm, COMM_QUIET))
      {
        for (pos = 0; pos < MAX_FORGET; pos++)
        {
          if (victim->pcdata->forget[pos] == NULL)
            break;
          if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
            found = true;
        }
        if (!found)
        {
          act_new
            ("{W[{-M{-u{-s{-i{-c{W] {C-{W={B$n{W={C- {W'{w$t{W'{x",
             ch, argument, d->character, TO_VICT, POS_SLEEPING);
        }
      }
    }
  }
  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);
}

CH_CMD(do_announce)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOANNOUNCE))
    {
      send_to_char("Announcement channel is now ON\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOANNOUNCE);
    }
    else
    {
      send_to_char("Announce channel is now OFF\n\r", ch);
      SET_BIT(ch->comm, COMM_NOANNOUNCE);
    }
    return;
  }

  REMOVE_BIT(ch->comm, COMM_NOANNOUNCE);

  sprintf(buf, "{w-{D={w-{D={RA{rnnouncement{w-{D={D{w-{W> {D$n {D%s{x",
          argument);
  act_new("{w-{D={w-{D={RA{rnnouncement{w-{D={w-{W> {D$n {W$t{x", ch,
          argument, NULL, TO_CHAR, POS_DEAD);
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING && d->character != ch &&
        !IS_SET(d->character->comm, COMM_NOANNOUNCE))
    {
      act_new("{W-=-={RA{rnnouncement{W-=->> {D$n {D$t{x", ch, argument,
              d->character, TO_VICT, POS_DEAD);
    }
  }

  return;
}

void do_gmessage(char *argument)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  sprintf(buf, "%s", argument);

  for (d = descriptor_list; d; d = d->next)
  {
    if (d->connected == CON_PLAYING)
    {
      send_to_char(buf, d->character);
    }
  }

  return;
}

CH_CMD(do_immtalk)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOWIZ))
    {
      send_to_char("Immortal channel is now ON\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOWIZ);
    }
    else
    {
      send_to_char("Immortal channel is now OFF\n\r", ch);
      SET_BIT(ch->comm, COMM_NOWIZ);
    }
    return;
  }

  argument = makedrunk(argument, ch);
  argument = pcolor(ch, argument, 0);
  REMOVE_BIT(ch->comm, COMM_NOWIZ);

  sprintf(buf, "{W[{BI{bmm{BT{balt{w] {r-{R={G$n{R={r- {x'{C%s{x'", argument);
  act_new("{W[{BI{bmm{BT{balk{W] {r-{R={G$n{R={r- {x'{C$t{x'", ch, argument,
          NULL, TO_CHAR, POS_DEAD);
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING && IS_IMMORTAL(d->character) &&
        !IS_SET(d->character->comm, COMM_NOWIZ))
    {
      act_new("{W[{BI{bmm{BT{balk{W] {r-{R={G$n{R={r-{x'{C$t{x'", ch,
              argument, d->character, TO_VICT, POS_DEAD);
    }
  }

  return;
}

CH_CMD(do_say)
{
  if (IS_SET(ch->act2, PLR_RP) || IS_NPC(ch))
  {
    do_sayic(ch, argument);
    return;
  }
  else
  {
    do_sayooc(ch, argument);
  }
}


static char *say_verb(char *argument)
{
  int length = 0;

  if (argument[0] == '\0')
    return "say";

  length = (int) strlen(argument);

  /* 
   * trailing {7 is to assure the last ' is colored right
   * trailing {x is to assure no colour bleed from the green -M
   * Added in functionality for different ways to say
   */

  if (argument[length - 1] == '!')
  {
    /* 'something!' = exclaim */
    if (argument[length - 2] != '!')
      return "{Yexclaim";
    /* 'something!!!' = screams */
    else
      return "{Rscream";
  }
  else if (argument[length - 1] == '?')
  {
    /* 'something?!' = boggles */
    if (argument[length - 2] == '!')
      return "{gboggle";
    /* 'something?' = asks */
    else if (argument[length - 2] != '?')
      return "{Cask";
    /* 'something??' = demands */
    else if (argument[length - 20] != '?')
      return "{Gdemand";
  }
  /* 'something...' = mutter */
  else if (argument[length - 1] == '.' && argument[length - 2] == '.' &&
           argument[length - 3] == '.')
    return "{Dmutter";
  /* '=), :) = chuckles */
  else if (argument[length - 1] == ')' &&
           (argument[length - 2] == '=' || argument[length - 2] == ':'))
    return "{cchuckle";
  /* '=(, :( = sulks */
  else if (argument[length - 1] == '(' &&
           (argument[length - 2] == '=' || argument[length - 2] == ':'))
    return "{bsulk";
  /* '=P, :P = smirks */
  else if (argument[length - 1] == 'P' &&
           (argument[length - 2] == '=' || argument[length - 2] == ':'))
    return "{rsmirk";
  else if (argument[length - 1] == ')' && (argument[length - 2] == ';'))
    return "{yleer";
  /* '=O, :O = sings */
  else if (argument[length - 1] == 'O' &&
           (argument[length - 2] == '=' || argument[length - 2] == ':'))
    return "{Msing";
  /* Add more punctuations with another else if statement here */

  return "say";
}

/*
 * Different statements for punctuation at the end of a say now. 
 * -Marquoz (marquoz@gost.net)
 */
CH_CMD(do_sayooc)
{
  char buf[MAX_STRING_LENGTH];

  buf[0] = '\0';

  if (!IS_NPC(ch))
    ch->movement_timer = 0;

  if (is_blinky(argument))
  {
    send_to_char("You can not use blink in say.\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char("Say what out of character?\n\r", ch);
    return;
  }

  /* 
   * trailing {7 is to assure the last ' is colored right
   * trailing {x is to assure no colour bleed from the green -M
   * Added in functionality for different ways to say
   */
  sprintf(buf, "{W$n %ss{x {C({DOOC{C) '{W$T{C'{x", say_verb(argument));
  act(buf, ch, NULL, argument, TO_ROOM);
  sprintf(buf, "{WYou %s{x {C({DOOC{C) '{W$T{C'{x", say_verb(argument));
  act(buf, ch, NULL, argument, TO_CHAR);

  if (!IS_NPC(ch))
  {
    CHAR_DATA *mob, *mob_next;

    for (mob = ch->in_room->people; mob != NULL; mob = mob_next)
    {
      mob_next = mob->next_in_room;
      if (IS_NPC(mob) && HAS_TRIGGER(mob, TRIG_SPEECH) &&
          mob->position == mob->pIndexData->default_pos)
        mp_act_trigger(argument, mob, ch, NULL, NULL, TRIG_SPEECH);
    }
  }
  return;
}

CH_CMD(do_sayic)
{
  char buf[MAX_STRING_LENGTH];
  char verb[MAX_STRING_LENGTH-256];

  buf[0] = '\0';

  if (!IS_NPC(ch))
    ch->movement_timer = 0;

  if (IS_SHIELDED(ch, SHD_SILENCE))
  {
    send_to_char("You are silent and can not speak.", ch);
    return;
  }

  if (is_blinky(argument))
  {
    send_to_char("You can not use blink in say.\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char("Say what in character?\n\r", ch);
    return;
  }

  /* 
   * trailing {7 is to assure the last ' is colored right
   * trailing {x is to assure no colour bleed from the green -M
   * Added in functionality for different ways to say
   */
  sprintf(verb, "%s", say_verb(argument));
  argument = makedrunk(argument, ch);
  sprintf(buf, "{W$n %ss{x {R({DIC{R) '{W$T{R'{x", verb);
  act(buf, ch, NULL, argument, TO_ROOM);
  sprintf(buf, "{WYou %s{x {R({DIC{R) '{W$T{R'{x", verb);
  act(buf, ch, NULL, argument, TO_CHAR);

  if (!IS_NPC(ch))
  {
    CHAR_DATA *mob, *mob_next;

    for (mob = ch->in_room->people; mob != NULL; mob = mob_next)
    {
      mob_next = mob->next_in_room;
      if (IS_NPC(mob) && HAS_TRIGGER(mob, TRIG_SPEECH) &&
          mob->position == mob->pIndexData->default_pos)
        mp_act_trigger(argument, mob, ch, NULL, NULL, TRIG_SPEECH);
    }
  }
  return;
}

CH_CMD(do_shout)
{
  DESCRIPTOR_DATA *d;

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (argument[0] == '\0')
  {
    if (IS_SET(ch->comm, COMM_NOSHOUT))
    {
      send_to_char("You can hear shouts again.\n\r", ch);
      REMOVE_BIT(ch->comm, COMM_NOSHOUT);
    }
    else
    {
      send_to_char("You will no longer hear shouts.\n\r", ch);
      SET_BIT(ch->comm, COMM_NOSHOUT);
    }
    return;
  }

  if (IS_SET(ch->comm, COMM_NOSHOUT))
  {
    send_to_char("You can't shout.\n\r", ch);
    return;
  }
  if (IS_SHIELDED(ch, SHD_SILENCE))
  {
    send_to_char("You are silent and can not speak.", ch);
    return;
  }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
  {
    send_to_char
      ("Just keep your nose in the corner like a good little player.\n\r",
       ch);
    return;
  }

  if (is_blinky(argument))
  {
    send_to_char("You can not use blink in shout.\n\r", ch);
    return;
  }
  argument = makedrunk(argument, ch);
  argument = pcolor(ch, argument, 0);
  REMOVE_BIT(ch->comm, COMM_NOSHOUT);

  if (ch->level < 5)
    WAIT_STATE(ch, 18);

  act
    ("{W[{r*{R*{YSHOUT{R*{r*{W] {r-{R={Y*{W) {GYou {W({Y*{R={R- {W'{R$T{W'{x",
     ch, NULL, argument, TO_CHAR);
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *victim;
    int pos;
    bool found = false;

    victim = d->original ? d->original : d->character;

    if (d->connected == CON_PLAYING && d->character != ch &&
        !IS_SET(victim->comm, COMM_NOSHOUT) &&
        !IS_SET(victim->comm, COMM_QUIET))
    {
      for (pos = 0; pos < MAX_FORGET; pos++)
      {
        if (victim->pcdata->forget[pos] == NULL)
          break;
        if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
          found = true;
      }
      if (!found)
      {
        act
          ("{W[{r*{R*{YSHOUT{R*{r*{W] {r-{R={Y*{W) {G$n {W({Y*{R={R- {W'{R$t{W'{x",
           ch, argument, d->character, TO_VICT);
      }
    }
  }

  return;
}

CH_CMD(do_tell)
{
  char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
  CHAR_DATA *victim;
  int pos;
  bool found = false;
  int wtime;

  arg[0] = '\0';

  if (!IS_NPC(ch))
    ch->movement_timer = 0;
  if (IS_SET(ch->comm, COMM_NOTELL) || IS_SET(ch->comm, COMM_DEAF))
  {
    send_to_char("Your message didn't get through.\n\r", ch);
    return;
  }
  if (IS_SET(ch->act2, PLR2_BUSY))
  {
    send_to_char
      ("The player is marked as being busy. Your tell will still go through.\n\r",
       ch);
  }
  if (IS_SET(ch->comm, COMM_QUIET))
  {
    send_to_char("You must turn off quiet mode first.\n\r", ch);
    return;
  }

  if (IS_SET(ch->comm, COMM_DEAF))
  {
    send_to_char("You must turn off deaf mode first.\n\r", ch);
    return;
  }
  if (IS_SHIELDED(ch, SHD_SILENCE))
  {
    send_to_char("You are silent and can not speak.", ch);
    return;
  }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
  {
    send_to_char
      ("Just keep your nose in the corner like a good little player.\n\r",
       ch);
    return;
  }

  if (is_blinky(argument))
  {
    send_to_char("You can not use blink in tells.\n\r", ch);
    return;
  }

  argument = one_argument(argument, arg);

  if (arg[0] == '\0' || argument[0] == '\0')
  {
    send_to_char("Tell whom what?\n\r", ch);
    return;
  }

  /* 
   * Can tell to PC's anywhere, but NPC's only in same room.
   * -- Furey
   */
  if ((victim = get_char_world(ch, arg)) == NULL ||
      (IS_NPC(victim) && victim->in_room != ch->in_room))
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  argument = makedrunk(argument, ch);
  argument = pcolor(ch, argument, 0);
  if (victim->desc == NULL && !IS_NPC(victim))
  {
    act("{W$N seems to have misplaced $S link...try again later{x.", ch,
        NULL, victim, TO_CHAR);
    sprintf(buf, "{g[{GTell{g] {D-{g={G%s{g={D- {g'{G%s{g'{x\n\r",
            PERS(ch, victim), argument);
    buf[0] = UPPER(buf[0]);
    add_buf(victim->pcdata->buffer, buf);
    victim->tells++;
    return;
  }

  if (!IS_NPC(victim))
  {
    for (pos = 0; pos < MAX_FORGET; pos++)
    {
      if (victim->pcdata->forget[pos] == NULL)
        break;
      if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
        found = true;
    }
  }
  if (found)
  {
    act("$N doesn't seem to be listening to you.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if ((IS_SET(victim->comm, COMM_QUIET) ||
       IS_SET(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch))
  {
    act("$E is not receiving tells.", ch, 0, victim, TO_CHAR);
    return;
  }

  if (IS_SET(victim->comm, COMM_AFK))
  {
    if (IS_NPC(victim))
    {
      act("$E is AFK, and not receiving tells.", ch, NULL, victim, TO_CHAR);
      return;
    }

    act("$E is AFK, but your tell will go through when $E returns.", ch,
        NULL, victim, TO_CHAR);
    sprintf(buf, "{g[{GTell{g] {D-{g={G%s{g={D- {g'{G%s{g'{x\n\r",
            PERS(ch, victim), argument);
    buf[0] = UPPER(buf[0]);
    add_buf(victim->pcdata->buffer, buf);
    victim->tells++;
    return;
  }

  if (IS_SET(victim->comm, COMM_STORE) && (victim->fighting != NULL))
  {
    if (IS_NPC(victim))
    {
      act("$E is fighting, and not receiving tells.", ch, NULL, victim,
          TO_CHAR);
      return;
    }

    act("$E is fighting, but your tell will go through when $E finishes.",
        ch, NULL, victim, TO_CHAR);
    sprintf(buf, "{g[{GTell{g] {D-{g={G%s{g={D- {g'{G%s{g'{x\n\r",
            PERS(ch, victim), argument);
    buf[0] = UPPER(buf[0]);
    add_buf(victim->pcdata->buffer, buf);
    victim->tells++;
    return;
  }
  act("{g[{GTell{g] {D-{g={GTo $N{g={D-  {g'{G$t{g'{x", ch, argument,
      victim, TO_CHAR);
  act_new("{g[{GTell{g] {D-{g={G$n{g={D- {g'{G$t{g'{x", ch, argument,
          victim, TO_VICT, POS_DEAD);
  victim->reply = ch;

  if (!IS_NPC(ch) && IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_SPEECH))
    mp_act_trigger(argument, victim, ch, NULL, NULL, TRIG_SPEECH);

  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);

  return;
}

CH_CMD(do_reply)
{
  CHAR_DATA *victim;
  char buf[MAX_STRING_LENGTH];
  int pos;
  bool found = false;
  int wtime;

  if (argument[0] == '\0')
  {
    send_to_char("Reply what?\n\r", ch);
    return;
  }
  if (IS_SET(ch->comm, COMM_NOTELL))
  {
    send_to_char("Your message didn't get through.\n\r", ch);
    return;
  }

  if (IS_SHIELDED(ch, SHD_SILENCE))
  {
    send_to_char("You are silent and can not speak.", ch);
    return;
  }
  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
  {
    send_to_char
      ("Just keep your nose in the corner like a good little player.\n\r",
       ch);
    return;
  }

  if (is_blinky(argument))
  {
    send_to_char("You can not use blink in replys.\n\r", ch);
    return;
  }
  argument = makedrunk(argument, ch);
  argument = pcolor(ch, argument, 0);

  if ((victim = ch->reply) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (victim->desc == NULL && !IS_NPC(victim))
  {
    act("$N seems to have misplaced $S link...try again later.", ch, NULL,
        victim, TO_CHAR);
    sprintf(buf, "{g[{GTell{g] {D-{g={G%s{g={D- {g'{G%s{g'{x\n\r",
            PERS(ch, victim), argument);
    buf[0] = UPPER(buf[0]);
    add_buf(victim->pcdata->buffer, buf);
    victim->tells++;
    return;
  }

  if (!IS_NPC(victim))
  {
    for (pos = 0; pos < MAX_FORGET; pos++)
    {
      if (victim->pcdata->forget[pos] == NULL)
        break;
      if (!str_cmp(ch->name, victim->pcdata->forget[pos]))
        found = true;
    }
  }
  if (found)
  {
    act("$N doesn't seem to be listening to you.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if ((IS_SET(victim->comm, COMM_QUIET) ||
       IS_SET(victim->comm, COMM_DEAF)) && !IS_IMMORTAL(ch) &&
      !IS_IMMORTAL(victim))
  {
    act_new("$E is not receiving tells.", ch, 0, victim, TO_CHAR, POS_DEAD);
    return;
  }

  if (IS_SET(victim->comm, COMM_AFK))
  {
    if (IS_NPC(victim))
    {
      act_new("$E is AFK, and not receiving tells.", ch, NULL, victim,
              TO_CHAR, POS_DEAD);
      return;
    }

    act_new("$E is AFK, but your tell will go through when $E returns.",
            ch, NULL, victim, TO_CHAR, POS_DEAD);
    sprintf(buf, "{g[{GTell{g] {D-{g={G%s{g={D- {g'{G%s{g'{x\n\r",
            PERS(ch, victim), argument);
    buf[0] = UPPER(buf[0]);
    add_buf(victim->pcdata->buffer, buf);
    victim->tells++;
    return;
  }

  act_new("{g[{GReply{g] {D-{g={GTo $N{g={D-  {g'{G$t{g'{x", ch, argument,
          victim, TO_CHAR, POS_DEAD);
  act_new("{g[{GReply{g] {D-{g={G$n{g={D-  {g'{G$t{g'{x", ch, argument,
          victim, TO_VICT, POS_DEAD);
  victim->reply = ch;

  wtime = UMAX(2, 9 - (ch->level));
  if (ch->level < 5)
    WAIT_STATE(ch, 18);
  else
    WAIT_STATE(ch, wtime);

  return;
}

CH_CMD(do_yell)
{
  DESCRIPTOR_DATA *d;

  if (IS_SET(ch->comm, COMM_NOSHOUT))
  {
    send_to_char("You can't yell.\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char("Yell what?\n\r", ch);
    return;
  }

  if (IS_SHIELDED(ch, SHD_SILENCE))
  {
    send_to_char("You are silent and can not speak.", ch);
    return;
  }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
  {
    send_to_char
      ("Just keep your nose in the corner like a good little player.\n\r",
       ch);
    return;
  }

  kill_swalk(ch);

  if (is_blinky(argument))
  {
    send_to_char("You can not use blink in yells.\n\r", ch);
    return;
  }

  argument = makedrunk(argument, ch);
  argument = pcolor(ch, argument, 0);

  act("{YYou {b!!{WYELL{b!!{x '{R$t{x'", ch, argument, NULL, TO_CHAR);
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    int pos;
    bool found = false;

    if (d->connected == CON_PLAYING && d->character != ch &&
        d->character->in_room != NULL &&
        d->character->in_room->area == ch->in_room->area &&
        !IS_SET(d->character->comm, COMM_QUIET))
    {
      for (pos = 0; pos < MAX_FORGET; pos++)
      {
        if (d->character->pcdata->forget[pos] == NULL)
          break;
        if (!str_cmp(ch->name, d->character->pcdata->forget[pos]))
          found = true;
      }
      if (!found)
      {
        act("{Y$n {b!!{WYELLS{b!!{x '{R$t{x'", ch, argument,
            d->character, TO_VICT);
      }
    }
  }

  return;
}

CH_CMD(do_emote)
{
  if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE))
  {
    send_to_char("You can't show your emotions.\n\r", ch);
    return;
  }

  if (is_blinky(argument))
  {
    send_to_char("You can not use blink in emote.\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char("Emote what?\n\r", ch);
    return;
  }

  MOBtrigger = false;
  act("$n $T{x", ch, NULL, argument, TO_ROOM);
  act("$n $T{x", ch, NULL, argument, TO_CHAR);
  MOBtrigger = true;
  return;
}

CH_CMD(do_pmote)
{
  CHAR_DATA *vch;
  char *letter, *name;
  char last[MAX_INPUT_LENGTH], temp[MAX_STRING_LENGTH];
  int matches = 0;

  if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOEMOTE))
  {
    send_to_char("You can't show your emotions.\n\r", ch);
    return;
  }

  if (argument[0] == '\0')
  {
    send_to_char("Emote what?\n\r", ch);
    return;
  }

  if (is_blinky(argument))
  {
    send_to_char("You can not use blink in emote.\n\r", ch);
    return;
  }

  act("{c$n $t{x", ch, argument, NULL, TO_CHAR);

  for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
  {
    if (vch->desc == NULL || vch == ch)
      continue;

    if ((letter = strstr(argument, vch->name)) == NULL)
    {
      MOBtrigger = false;
      act("{c$N $t{x", vch, argument, ch, TO_CHAR);
      MOBtrigger = true;
      continue;
    }

    strcpy(temp, argument);
    temp[strlen(argument) - strlen(letter)] = '\0';
    last[0] = '\0';
    name = vch->name;

    for (; *letter != '\0'; letter++)
    {
      if (*letter == '\'' && matches == (int) strlen(vch->name))
      {
        strcat(temp, "r");
        continue;
      }

      if (*letter == 's' && matches == (int) strlen(vch->name))
      {
        matches = 0;
        continue;
      }

      if (matches == (int) strlen(vch->name))
      {
        matches = 0;
      }

      if (*letter == *name)
      {
        matches++;
        name++;
        if (matches == (int) strlen(vch->name))
        {
          strcat(temp, "you");
          last[0] = '\0';
          name = vch->name;
          continue;
        }
        strncat(last, letter, 1);
        continue;
      }

      matches = 0;
      strcat(temp, last);
      strncat(temp, letter, 1);
      last[0] = '\0';
      name = vch->name;
    }

    MOBtrigger = false;
    act("{c$N $t{x", vch, temp, ch, TO_CHAR);
    MOBtrigger = true;
  }

  return;
}

void do_bug(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char *buf2;

  if (!argument || argument[0] == '\0')
  {
    send_to_char("Submit what bug?\n\r", ch);
    return;
  }

  smash_tilde(argument);
  sprintf(buf,
          "While standing in %s [%ld], %s noticed the following bug:\n\r\n\r%s",
          ch->in_room->name, ch->in_room->vnum, ch->name, argument);
  buf2 = str_dup(buf);
  buf2 = format_string(buf2);
  make_note("Bugs", ch->name, "imms", "Quick Bug Report", 30, buf2);
  free_string(buf2);
  send_to_char("Bug logged.\n\r", ch);
  return;
}

void do_typo(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char *buf2;

  if (!argument || argument[0] == '\0')
  {
    send_to_char("Submit what typo?\n\r", ch);
    return;
  }

  smash_tilde(argument);
  sprintf(buf,
          "While standing in %s [%ld],  %s noticed the following typo:\n\r\n\r%s",
          ch->in_room->name, ch->in_room->vnum, ch->name, argument);
  buf2 = str_dup(buf);
  buf2 = format_string(buf2);
  make_note("Typos", ch->name, "imms", "Quick Typo Report", 30, buf2);
  free_string(buf2);
  send_to_char("Typo logged.\n\r", ch);
  return;
}

void do_idea(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char *buf2;

  if (!argument || argument[0] == '\0')
  {
    send_to_char("Submit what idea?\n\r", ch);
    return;
  }

  smash_tilde(argument);
  switch (number_range(1, 5))
  {
    case 1:
      sprintf(buf,
              "While standing in %s [%ld], a light bulb appeared above %s's head:\n\r\n\r%s",
              ch->in_room->name, ch->in_room->vnum, ch->name, argument);
      break;
    case 2:
      sprintf(buf,
              "While standing in %s [%ld], %s had a miraculous revelation!\n\r\n\r%s",
              ch->in_room->name, ch->in_room->vnum, ch->name, argument);
      break;
    case 3:
      sprintf(buf,
              "While standing in %s [%ld], God asked %s the meaning of life, and %s said:\n\r\n\r%s",
              ch->in_room->name, ch->in_room->vnum, ch->name, ch->name,
              argument);
      break;
    case 4:
      sprintf(buf,
              "While standing in %s [%ld], %s slipped and hit his head while playing twister and thought of the following:\n\r\n\r%s",
              ch->in_room->name, ch->in_room->vnum, ch->name, argument);
      break;
    case 5:
      sprintf(buf,
              "While standing in %s [%ld], %s had a mental breakdown while pondering this:\n\r\n\r%s",
              ch->in_room->name, ch->in_room->vnum, ch->name, argument);
      break;
    default:
      sprintf(buf, "While standing in %s [%ld], %s thought:\n\r\n\r%s",
              ch->in_room->name, ch->in_room->vnum, ch->name, argument);
      break;
  }

  buf2 = str_dup(buf);
  buf2 = format_string(buf2);
  make_note("Ideas", ch->name, "imms", "Quick Idea Report", 30, buf2);
  free_string(buf2);
  send_to_char("Idea logged.\n\r", ch);
  return;
}


CH_CMD(do_qui)
{
  send_to_char("If you want to QUIT, you have to spell it out.\n\r", ch);
  return;
}

/* 
 * The Routine
 * Quote Code is by elfren@aros.net
 */

void do_qquote(CHAR_DATA * ch)
{
  char buf[MAX_STRING_LENGTH];
  int number;

  number = number_range(0, MAX_QUOTES);

  sprintf(buf, "\n\r%s\n\r - %s\n\r", quote_table[number].text,
          quote_table[number].by);
  send_to_char(buf, ch);
  return;
}

CH_CMD(do_quit)
{
  DESCRIPTOR_DATA *d;
  bool relog = false;
  char buf[MSL];

  if (ch == NULL)
    return;

  if (IS_NPC(ch))
    return;

  d = ch->desc;

  if (!strcmp("force", argument))
  {
    force_quit(ch, "");
    return;
  }

  if (!strcmp("relog", argument))
  {
    relog = true;
  }

  if (ch->position == POS_FIGHTING)
  {
    send_to_char("No way! You are fighting.\n\r", ch);
    return;
  }
  if (auction_list != NULL &&
      (auction_list->high_bidder == ch || auction_list->owner == ch))
  {
    send_to_char("You still have a stake in the auction!\n\r", ch);
    return;
  }

  if (ch->in_room && IS_SET(ch->in_room->room_flags, ROOM_ARENA))
  {
    send_to_char("Not while in the arena you dont.\n\r", ch);
    return;
  }

  if (IS_SET(ch->act, PLR_QUESTOR))
  {
    printf_to_char(ch,
                   "{WHow do you expect to {Yleave{W while on a quest?{x\n\r");
    return;
  }

  if (IS_SET(ch->comm, COMM_AFK) && !IS_IMMORTAL(ch))
  {
    send_to_char("You can not quit while AFK.\n\r", ch);
    return;
  }

  if (ch->position < POS_STUNNED)
  {
    send_to_char("You're not DEAD yet.\n\r", ch);
    return;
  }

  save_char_obj(ch);

  send_to_char("\n\r\n\r{R I hope you have enjoyed your stay!\n\r\n\r", ch);

  sprintf(buf, "%s rejoins the real world.", ch->name);
  wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);

  act(buf, NULL, 0, 0, TO_ROOM);

  sprintf(log_buf, "%s has quit.", ch->name);
  log_string(log_buf);

  if (!IS_IMMORTAL(ch))
  {
    char lbuf[MSL];

    if (ch->pcdata->title != NULL)
      sprintf(lbuf, "{W[{RL{rog{Dout{W] %s{x%s{w has left %s.\n\r",
              ch->name, ch->pcdata->title, mudname);
    else
      sprintf(lbuf, "{W[{RL{rog{Dout{W] %s{w has left %s.\n\r",
              ch->name, mudname);
    do_gmessage(lbuf);
  }

  /* 
   * After extract_char the ch is no longer valid!
   */
  if (!relog)
  {
    if (d != NULL)
      close_socket(d);
    extract_char(ch, true);
    ch = NULL;
  }
  else
  {
    do_help(ch, "greeting");
    send_to_char("What name do you go by friend? ", ch);
    d->connected = CON_GET_NAME;

    extract_char(ch, true);
    ch = NULL;
  }

  return;
}

void force_quit(CHAR_DATA * ch, char *argument)
{
  DESCRIPTOR_DATA *d;
  char buf[MSL];
  int lose;

  if (ch == NULL)
    return;

  if (IS_NPC(ch))
    return;

  d = ch->desc;

  if (ch->position == POS_FIGHTING)
  {
    lose = (d != NULL) ? 50 : 50;
    gain_exp(ch, 0 - lose);
    stop_fighting(ch, true);
  }

  if (ch->position < POS_STUNNED)
  {
    lose = (d != NULL) ? 50 : 100;
    gain_exp(ch, 0 - lose);
  }

  send_to_char("\n\r\n\r{rGoodbye for now!{x\n\r", ch);
  act("$n has left the game.", ch, NULL, NULL, TO_ROOM);

  sprintf(log_buf, "%s has quit.", ch->name);
  log_string(log_buf);

  sprintf(buf, "%s rejoins the real world.", ch->name);
  wiznet(buf, NULL, NULL, WIZ_LOGINS, 0, 0);

  save_char_obj(ch);

  if (d != NULL)
    close_socket(d);

  /* 
   * After extract_char the ch is no longer valid!
   */
  extract_char(ch, true);
  ch = NULL;
  return;
}

CH_CMD(do_follow)
{
  /* RT changed to allow unlimited following and follow the NOFOLLOW rules */
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  arg[0] = '\0';

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Follow whom?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL)
  {
    act("But you'd rather follow $N!", ch, NULL, ch->master, TO_CHAR);
    return;
  }

  if (victim == ch)
  {
    if (ch->master == NULL)
    {
      send_to_char("You already follow yourself.\n\r", ch);
      return;
    }
    stop_follower(ch);
    return;
  }

  if (!IS_NPC(victim) && IS_SET(victim->act, PLR_NOFOLLOW) &&
      !IS_IMMORTAL(ch))
  {
    act("$N doesn't seem to want any followers.\n\r", ch, NULL, victim,
        TO_CHAR);
    return;
  }

  if ((!IS_NPC(ch) && !IS_NPC(victim)) && (!IS_IMMORTAL(ch)) &&
      (!IS_IMMORTAL(victim)) && (ch != victim) &&
      (!str_cmp(ch->pcdata->socket, victim->pcdata->socket)))
  {
    act("You can't seem to follow $N.\n\r", ch, NULL, victim, TO_CHAR);
    return;
  }

  REMOVE_BIT(ch->act, PLR_NOFOLLOW);
  if (ch->master != NULL)
    stop_follower(ch);
  add_follower(ch, victim);
  return;
}

void add_follower(CHAR_DATA * ch, CHAR_DATA * master)
{
  CHAR_DATA *gch;
  int i;
  int others;
  int followers;
  bool follower;

  i = 0;
  others = 0;
  followers = 0;

  if (ch->master != NULL)
  {
    bug("Add_follower: non-null master.", 0);
    return;
  }

  if (char_list != NULL)
    for (gch = char_list; gch != NULL; gch = gch->next)
    {
      if (is_same_group(gch, master) && IS_NPC(gch))
      {
        i = 0;

        if (gch->pIndexData->vnum == 3175)
          others++;

        while (i < MAX_SUMMON)
        {
          if (summon_table[i].vnum == gch->pIndexData->vnum)
          {
            others++;
          }
          i++;
        }

        if (IS_NPC(gch))
          followers++;
      }
    }

  follower = true;

  if (IS_NPC(ch))
    if (ch->pIndexData->vnum == 3175)
      follower = false;

  if (IS_NPC(ch))
    while (i < MAX_SUMMON)
    {
      if (summon_table[i].vnum == ch->pIndexData->vnum)
      {
        follower = false;
      }
      i++;
    }

  if (IS_NPC(ch))
    if ((followers - others) >= 2 && follower)
    {
      send_to_char("You can not have any more NPC followers.\n\r", master);
      ch->master = NULL;
      ch->leader = NULL;
      return;
    }

  ch->master = master;
  ch->leader = NULL;

  if (can_see(master, ch))
    act("$n now follows you.", ch, NULL, master, TO_VICT);
  act("You now follow $N.", ch, NULL, master, TO_CHAR);
  return;
}

void stop_follower(CHAR_DATA * ch)
{
  if (ch->master == NULL)
  {
    bug("Stop_follower: null master.", 0);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM))
  {
    REMOVE_BIT(ch->affected_by, AFF_CHARM);
    affect_strip(ch, gsn_charm_person);
  }
  if (IS_NPC(ch) && IS_SET(ch->act, ACT_PET))
  {
    REMOVE_BIT(ch->act, ACT_PET);
    ch->timer = 0;
    ch->hastimer = true;
  }

  if (can_see(ch->master, ch) && ch->in_room != NULL)
  {
    act("$n stops following you.", ch, NULL, ch->master, TO_VICT);
    act("You stop following $N.", ch, NULL, ch->master, TO_CHAR);
  }
  if (ch->master->pet == ch)
    ch->master->pet = NULL;
  if (ch->master->familiar == ch)
    ch->master->familiar = NULL;

  if (ch->master->pet != NULL)
    if (ch->master->pet->in_room == NULL)
      ch->master->pet = NULL;

  if (ch->master->familiar != NULL)
    if (ch->master->familiar->in_room == NULL)
      ch->master->familiar = NULL;

  ch->master = NULL;
  ch->leader = NULL;
  return;
}

/* nukes charmed monsters and pets */
void nuke_pets(CHAR_DATA * ch)
{
  if (ch->pet != NULL)
  {
    extract_char(ch->pet, true);
    ch->pet = NULL;
  }
  if (ch->familiar != NULL)
  {
    extract_char(ch->familiar, true);
    ch->familiar = NULL;
  }

  return;
}

void die_follower(CHAR_DATA * ch)
{
  CHAR_DATA *fch;

  if (ch->master != NULL)
  {
    if (ch->master->pet == ch)
      ch->master->pet = NULL;
    if (ch->master->familiar == ch)
      ch->master->familiar = NULL;
    stop_follower(ch);
  }

  ch->leader = NULL;
  for (fch = char_list; fch != NULL; fch = fch->next)
  {
    if (fch->master == ch)
      stop_follower(fch);
    if (fch->leader == ch)
      fch->leader = fch;
  }

  return;
}

CH_CMD(do_order)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *och;
  CHAR_DATA *och_next;
  bool found;
  bool fAll;

  arg[0] = '\0';

  argument = one_argument(argument, arg);
  one_argument(argument, arg2);
  if (!str_cmp(arg2, "delete") || !str_cmp(arg2, "mob"))
  {
    send_to_char("That will NOT be done.\n\r", ch);
    return;
  }

  if (!str_cmp(arg2, "reroll"))
  {
    send_to_char("That will NOT be done.\n\r", ch);
    return;
  }

  if (arg[0] == '\0' || argument[0] == '\0')
  {
    send_to_char("Order whom to do what?\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM))
  {
    send_to_char("You feel like taking, not giving, orders.\n\r", ch);
    return;
  }

  if (!str_cmp(arg, "all"))
  {
    fAll = true;
    victim = NULL;
  }
  else
  {
    fAll = false;
    if ((victim = get_char_room(ch, arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }

    if (victim == ch)
    {
      send_to_char("Aye aye, right away!\n\r", ch);
      return;
    }

    if (!IS_AFFECTED(victim, AFF_CHARM) || victim->master != ch ||
        (IS_IMMORTAL(victim) && victim->trust >= ch->trust))
    {
      send_to_char("Do it yourself!\n\r", ch);
      return;
    }
  }

  found = false;
  for (och = ch->in_room->people; och != NULL; och = och_next)
  {
    och_next = och->next_in_room;
    if (IS_AFFECTED(och, AFF_CHARM) && och->master == ch &&
        (fAll || och == victim))
    {
      found = true;
      sprintf(buf, "$n orders you to '%s'.", argument);
      act(buf, ch, NULL, och, TO_VICT);
      interpret(och, argument);
    }
  }

  if (found)
  {
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, PULSE_VIOLENCE);
    send_to_char("Ok.\n\r", ch);
  }
  else
    send_to_char("You have no followers here.\n\r", ch);
  return;
}

CH_CMD(do_group)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  arg[0] = '\0';

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    CHAR_DATA *gch;
    CHAR_DATA *leader;

    leader = (ch->leader != NULL) ? ch->leader : ch;
    sprintf(buf, "%s's group:\n\r", PERS(leader, ch));
    send_to_char(buf, ch);
    for (gch = char_list; gch != NULL; gch = gch->next)
    {
      if (is_same_group(gch, ch))
      {
        sprintf(buf,
                "[%2d %s] %-16s %ld/%ld hp %ld/%ld mana %ld/%ld mv %5ld xp\n\r",
                gch->level,
                IS_NPC(gch) ? "Mob" : class_table[gch->class].
                who_name, capitalize(PERS(gch, ch)), gch->hit,
                gch->max_hit, gch->mana, gch->max_mana, gch->move,
                gch->max_move, gch->exp);
        send_to_char(buf, ch);
      }
    }
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (ch->master != NULL || (ch->leader != NULL && ch->leader != ch))
  {
    send_to_char("But you are following someone else!\n\r", ch);
    return;
  }

  if (victim->master != ch && ch != victim)
  {
    act_new("$N isn't following you.", ch, NULL, victim, TO_CHAR,
            POS_SLEEPING);
    return;
  }

  if (IS_AFFECTED(victim, AFF_CHARM))
  {
    send_to_char("You can't remove charmed mobs from your group.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM))
  {
    act("You like your master too much to leave $m!", ch, NULL, victim,
        TO_VICT);
    return;
  }

  if (victim->level - ch->level > 14)
  {
    send_to_char("They are to high of a level for your group.\n\r", ch);
    return;
  }

  if (victim->level - ch->level < -14)
  {
    send_to_char("They are to low of a level for your group.\n\r", ch);
    return;
  }

  if (is_same_group(victim, ch) && ch != victim)
  {
    victim->leader = NULL;
    act("$n removes $N from $s group.", ch, NULL, victim, TO_NOTVICT);
    act("$n removes you from $s group.", ch, NULL, victim, TO_VICT);
    act("You remove $N from your group.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if ((!IS_NPC(ch) && !IS_NPC(victim)) && (!IS_IMMORTAL(ch)) &&
      (!IS_IMMORTAL(victim)) && (ch != victim) &&
      (!str_cmp(ch->pcdata->socket, victim->pcdata->socket)))
  {
    send_to_char("They are unable to join your group.\n\r", ch);
    return;
  }

  victim->leader = ch;
  act("$N joins $n's group.", ch, NULL, victim, TO_NOTVICT);
  act("You join $n's group.", ch, NULL, victim, TO_VICT);
  act("$N joins your group.", ch, NULL, victim, TO_CHAR);
  return;
}

/*
 * 'Split' originally by Gnort, God of Chaos.
 */
CH_CMD(do_split)
{
  char buf[MAX_STRING_LENGTH];
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  char arg3[MAX_INPUT_LENGTH];
  CHAR_DATA *gch;
  int members;
  int amount_platinum = 0, amount_gold = 0, amount_silver = 0;
  int share_platinum, share_gold, share_silver;
  int extra_platinum, extra_gold, extra_silver;

  arg1[0] = '\0';
  arg2[0] = '\0';
  arg3[0] = '\0';

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  one_argument(argument, arg3);

  if (arg1[0] == '\0')
  {
    send_to_char("Split how much?\n\r", ch);
    return;
  }

  amount_silver = atoi(arg1);
  if (arg2[0] != '\0')
    amount_gold = atoi(arg2);
  if (arg3[0] != '\0')
    amount_platinum = atoi(arg3);
  if (amount_platinum < 0 || amount_gold < 0 || amount_silver < 0)
  {
    send_to_char("Your group wouldn't like that.\n\r", ch);
    return;
  }

  if (amount_platinum == 0 && amount_gold == 0 && amount_silver == 0)
  {
    send_to_char("You hand out zero coins, but no one notices.\n\r", ch);
    return;
  }

  if ((ch->silver + (ch->gold * 100) + (ch->platinum * 10000)) <
      (amount_silver + (amount_gold * 100) + (amount_platinum * 10000)))
  {
    send_to_char("You don't have that much to split.\n\r", ch);
    return;
  }

  members = 0;
  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
  {
    if (is_same_group(gch, ch) && !IS_AFFECTED(gch, AFF_CHARM))
      members++;
  }

  if (members < 2)
  {
    send_to_char("Just keep it all.\n\r", ch);
    return;
  }

  share_platinum = amount_platinum / members;
  extra_platinum = amount_platinum % members;
  amount_gold += (extra_platinum * 100);
  share_gold = amount_gold / members;
  extra_gold = amount_gold % members;
  amount_silver += (extra_gold * 100);
  share_silver = amount_silver / members;
  extra_silver = amount_silver % members;
  if (share_platinum == 0 && share_gold == 0 && share_silver == 0)
  {
    send_to_char("Don't even bother, cheapskate.\n\r", ch);
    return;
  }

  deduct_cost(ch, amount_platinum - extra_platinum, VALUE_PLATINUM);
  add_cost(ch, share_platinum, VALUE_PLATINUM);
  deduct_cost(ch, amount_gold - extra_gold, VALUE_GOLD);
  add_cost(ch, share_gold, VALUE_GOLD);
  deduct_cost(ch, amount_silver, VALUE_SILVER);
  add_cost(ch, share_silver + extra_silver, VALUE_SILVER);
  if (share_platinum > 0)
  {
    sprintf(buf,
            "You split %d platinum coins. Your share is %d platinum.\n\r",
            amount_platinum - extra_platinum, share_platinum);
    send_to_char(buf, ch);
  }
  if (share_gold > 0)
  {
    sprintf(buf, "You split %d gold coins. Your share is %d gold.\n\r",
            amount_gold - extra_gold, share_gold);
    send_to_char(buf, ch);
  }
  if (share_silver > 0)
  {
    sprintf(buf,
            "You split %d silver coins. Your share is %d silver.\n\r",
            amount_silver, share_silver + extra_silver);
    send_to_char(buf, ch);
  }

  if (share_gold == 0 && share_silver == 0)
  {
    sprintf(buf,
            "$n splits %d platinum coins. Your share is %d platinum.",
            amount_platinum - extra_platinum, share_platinum);
  }
  else if (share_platinum == 0 && share_silver == 0)
  {
    sprintf(buf, "$n splits %d gold coins. Your share is %d gold.",
            amount_gold - extra_gold, share_gold);
  }
  else if (share_platinum == 0 && share_gold == 0)
  {
    sprintf(buf, "$n splits %d silver coins. Your share is %d silver.",
            amount_silver, share_silver);
  }
  else if (share_silver == 0)
  {
    sprintf(buf,
            "$n splits %d platinum and %d gold coins. giving you %d platinum and %d gold.\n\r",
            amount_platinum - extra_platinum, amount_gold - extra_gold,
            share_platinum, share_gold);
  }
  else if (share_gold == 0)
  {
    sprintf(buf,
            "$n splits %d platinum and %d silver coins. giving you %d platinum and %d silver.\n\r",
            amount_platinum - extra_platinum, amount_silver,
            share_platinum, share_silver);
  }
  else if (share_platinum == 0)
  {
    sprintf(buf,
            "$n splits %d gold and %d silver coins. giving you %d gold and %d silver.\n\r",
            amount_gold - extra_gold, amount_silver, share_gold,
            share_silver);
  }
  else
  {
    sprintf(buf,
            "$n splits %d platinum, %d gold and %d silver coins. giving you %d platinum, %d gold and %d silver.\n\r",
            amount_platinum - extra_platinum, amount_gold - extra_gold,
            amount_silver, share_platinum, share_gold, share_silver);
  }

  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
  {
    if (gch != ch && is_same_group(gch, ch) && !IS_AFFECTED(gch, AFF_CHARM))
    {
      act(buf, ch, NULL, gch, TO_VICT);
      add_cost(gch, share_platinum, VALUE_PLATINUM);
      add_cost(gch, share_gold, VALUE_GOLD);
      add_cost(gch, share_silver, VALUE_SILVER);
    }
  }

  return;
}

CH_CMD(do_gtell)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *gch;

  if (argument[0] == '\0')
  {
    send_to_char("Tell your group what?\n\r", ch);
    return;
  }

  if (IS_SET(ch->comm, COMM_NOTELL))
  {
    send_to_char("Your message didn't get through!\n\r", ch);
    return;
  }

  if ((ch->in_room->vnum == ROOM_VNUM_CORNER) && (!IS_IMMORTAL(ch)))
  {
    send_to_char
      ("Just keep your nose in the corner like a good little player.\n\r",
       ch);
    return;
  }

  /* 
   * Note use of send_to_char, so gtell works on sleepers.
   */
  sprintf(buf, "%s tells the group '{K%s{x'\n\r", ch->name, argument);
  for (gch = char_list; gch != NULL; gch = gch->next)
  {
    if (is_same_group(gch, ch))
      send_to_char(buf, gch);
  }

  return;
}

/*
 * It is very important that this be an equivalence relation:
 * (1) A ~ A
 * (2) if A ~ B then B ~ A
 * (3) if A ~ B  and B ~ C, then A ~ C
 */
bool is_same_group(CHAR_DATA * ach, CHAR_DATA * bch)
{
  if (ach == NULL || bch == NULL)
    return false;
  if (ach->leader != NULL)
    ach = ach->leader;
  if (bch->leader != NULL)
    bch = bch->leader;
  return ach == bch;
}

/*
 * Colour setting and unsetting, way cool, Lope Oct '94
 */
CH_CMD(do_colour)
{
  char arg[MAX_STRING_LENGTH];
  int ccolor;

  arg[0] = '\0';

  argument = one_argument(argument, arg);
  if (!*arg)
  {
    if (IS_SET(ch->act, PLR_NOCOLOUR))
    {
      REMOVE_BIT(ch->act, PLR_NOCOLOUR);
      if (ch->desc)
        ch->desc->ansi = true;
      send_to_char("{bC{ro{yl{co{mu{gr{x is now {rON{x, Way Cool!\n\r", ch);
    }
    else
    {
      send_to_char("Colour is now OFF, <sigh>\n\r", ch);
      SET_BIT(ch->act, PLR_NOCOLOUR);
      if (ch->desc)
        ch->desc->ansi = false;
    }
  }
  else if (!str_prefix(arg, "list"))
  {
    send_to_char("\n\rColors:\n\r", ch);
    send_to_char("     0 - Reset           9 - Bright Red\n\r", ch);
    send_to_char("     1 - Red            10 - Bright Green\n\r", ch);
    send_to_char("     2 - Green          11 - Yellow\n\r", ch);
    send_to_char("     3 - Brown          12 - Bright Blue\n\r", ch);
    send_to_char("     4 - Blue           13 - Bright Magenta\n\r", ch);
    send_to_char("     5 - Magenta        14 - Bright Cyan\n\r", ch);
    send_to_char("     6 - Cyan           15 - Bright White\n\r", ch);
    send_to_char("     7 - White          16 - Black\n\r", ch);
    send_to_char("     8 - Grey           17 - None\n\r", ch);
    send_to_char("Channels:\n\r", ch);
    send_to_char("     ooc        cgossip    clan\n\r", ch);
    send_to_char("     gossip     grats      gtell\n\r", ch);
    send_to_char("     immtalk    music      ask\n\r", ch);
    send_to_char("     quote      say        shout\n\r", ch);
    send_to_char("     tell       wiznet     mobsay\n\r", ch);
    send_to_char("     room       condition  fight\n\r", ch);
    send_to_char("     opponent   witness    disarm\n\r", ch);
    send_to_char("     qgossip\n\r", ch);
    send_to_char("For a more detailed list, see HELP COLORS\n\r", ch);
    send_to_char("For a list of current settings, see HELP SETTINGS\n\r", ch);
  }
  else if (!str_cmp(arg, "0"))
  {
    ch->color = 0;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "1"))
  {
    ch->color = 1;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "2"))
  {
    ch->color = 2;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "3"))
  {
    ch->color = 3;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "4"))
  {
    ch->color = 4;

    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "5"))
  {
    ch->color = 5;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "6"))
  {
    ch->color = 6;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "7"))
  {
    ch->color = 7;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "8"))
  {
    ch->color = 8;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "9"))
  {
    ch->color = 9;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "10"))
  {
    ch->color = 10;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "11"))
  {
    ch->color = 11;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "12"))
  {
    ch->color = 12;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "13"))
  {
    ch->color = 13;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "14"))
  {
    ch->color = 14;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "15"))
  {
    ch->color = 15;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "16"))
  {
    ch->color = 16;
    send_to_char("{xOK\n\r", ch);
  }
  else if (!str_cmp(arg, "17"))
  {
    ch->color = 0;
    send_to_char("{xOK\n\r", ch);
  }
  else if (argument[0] == '\0')
  {
    send_to_char("Syntax: color {{list|#|<channel> #}\n\r", ch);
  }
  else if (!is_number(argument))
  {
    send_to_char("Syntax: color {{list|#|<channel> #}\n\r", ch);
  }
  else
  {
    ccolor = atoi(argument);
    if (ccolor >= 18)
    {
      send_to_char("Color number must be 0-17\n\r", ch);
      return;
    }
    if (!str_prefix(arg, "ooc"))
    {
      ch->color_auc = ccolor;
      send_to_char("ooc channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "cgossip"))
    {
      ch->color_cgo = ccolor;
      send_to_char("clan gossip channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "clan"))
    {
      ch->color_cla = ccolor;
      send_to_char("clan talk channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "gossip"))
    {
      ch->color_gos = ccolor;
      send_to_char("gossip channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "grats"))
    {
      ch->color_gra = ccolor;
      send_to_char("grats channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "gtell"))
    {
      ch->color_gte = ccolor;
      send_to_char("group tell channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "immtalk"))
    {
      ch->color_imm = ccolor;
      send_to_char("immortal talk channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "music"))
    {
      ch->color_mus = ccolor;
      send_to_char("music channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "ask"))
    {
      ch->color_que = ccolor;
      send_to_char("question/answer channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "quote"))
    {
      ch->color_quo = ccolor;
      send_to_char("quote channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "say"))
    {
      ch->color_say = ccolor;
      send_to_char("say channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "shout"))
    {
      ch->color_sho = ccolor;
      send_to_char("shout/yell channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "tell"))
    {
      ch->color_tel = ccolor;
      send_to_char("tell/reply channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "wiznet"))
    {
      ch->color_wiz = ccolor;
      send_to_char("wiznet channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "mobsay"))
    {
      ch->color_mob = ccolor;
      send_to_char("mobile talk channel set.\n\r", ch);
    }
    else if (!str_prefix(arg, "room"))
    {
      ch->color_roo = ccolor;
      send_to_char("room name display set.\n\r", ch);
    }
    else if (!str_prefix(arg, "condition"))
    {
      ch->color_con = ccolor;
      send_to_char("character condition display set.\n\r", ch);
    }
    else if (!str_prefix(arg, "fight"))
    {
      ch->color_fig = ccolor;
      send_to_char("your fight actions set.\n\r", ch);
    }
    else if (!str_prefix(arg, "opponent"))
    {
      ch->color_opp = ccolor;
      send_to_char("opponents fight actions set.\n\r", ch);
    }
    else if (!str_prefix(arg, "disarm"))
    {
      ch->color_dis = ccolor;
      send_to_char("disarm display set.\n\r", ch);
    }
    else if (!str_prefix(arg, "witness"))
    {
      ch->color_wit = ccolor;
      send_to_char("witness fight actions set.\n\r", ch);
    }
    else if (!str_prefix(arg, "qgossip"))
    {
      ch->color_qgo = ccolor;
      send_to_char("quest gossip channel set.\n\r", ch);
    }
    else
    {
      send_to_char("Syntax: color {{list|#|<channel> #}\n\r", ch);
    }

  }
  return;
}

/********************************************************************
* This is the code for gocials, or socials over the gossip channel. *
* It is fairly long and complicated and took me a whole hour and a  *
* half to write up. I finally did get it working though, but I am   *
* sure there are neater ways to code it...the code towards the      *
* end where it tries to do the replacing done normally by acts      *
* so that it can get the victim's name/etc in and then send it to   *
* another victim with the act command. Feel free to edit it if you  *
* can do a quick re-write using much neater code. This should work  *
* for all possible socials that normally work in the mud.           *
********************************************************************/

/********************************************************************
* I ask for no credit on the mud for this, nor do i ask for any-    *
* thing in exchange. I believe in the philosophy of giving what you *
* can to the mud community and then taking what others offer. Use   *
* this, copy this, give it to friends...whatever you like.          *
*                                                 -Bangle           *
********************************************************************/

CH_CMD(do_consent)
{
  CHAR_DATA *victim;
  char arg[MSL];
  char buf[MSL];

  if (IS_NPC(ch))
    return;

  if ((int) strlen(argument) <= 0)
  {
    send_to_char("Consent who?\n\r", ch);
    return;
  }

  argument = one_argument(argument, arg);

  if (!str_cmp("all", arg))
  {
    send_to_char("You now give consent to everyone.. You whore.\n\r", ch);
    return;
  }

  if (!str_cmp("none", arg))
  {
    ch->sex_consenter = NULL;
    send_to_char("You no longer give consent to anyone.\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) != NULL)
  {
    ch->sex_consenter = victim->name;
    sprintf(buf, "You now consent for sexual acts with %s.\n\r",
            victim->name);
    send_to_char(buf, ch);
    sprintf(buf, "%s now consents for sexual acts with you.\n\r", ch->name);
    send_to_char(buf, victim);
    return;
  }
  else
  {
    send_to_char("You dont see them here.\n\r", ch);
    return;
  }
}

CH_CMD(do_rp)
{
  if (IS_SET(ch->act2, PLR_RP))
  {
    REMOVE_BIT(ch->act2, PLR_RP);
    send_to_char("Your RP flag is now {w[{ROFF{w].\n\r", ch);
  }
  else
  {
    SET_BIT(ch->act2, PLR_RP);
    send_to_char("Your RP flag is now {w[{GON{w]{x.\n\r", ch);
  }
}

CH_CMD(do_gocial)
{
  char command[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int cmd;
  bool found;
  char arg[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *d;
  char buf[MAX_STRING_LENGTH];
  int counter;
  int count;
  char buf2[MAX_STRING_LENGTH];

  command[0] = '\0';
  arg[0] = '\0';

  argument = one_argument(argument, command);

  found = false;
  for (cmd = 0; social_table[cmd].name[0] != '\0'; cmd++)
  {
    if (command[0] == social_table[cmd].name[0] &&
        !str_prefix(command, social_table[cmd].name))
    {
      found = true;
      break;
    }
  }

  if (!found)
  {
    send_to_char("What kind of social is that?!?!\n\r", ch);
    return;
  }

  if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_QUIET))
  {
    send_to_char("You must turn off quiet mode first.\n\r", ch);
    return;
  }

  if (!IS_NPC(ch) && IS_SET(ch->comm, COMM_NOCHANNELS))
  {
    send_to_char("The gods have revoked your channel priviliges.\n\r", ch);
    return;
  }

  switch (ch->position)
  {
    case POS_DEAD:
      send_to_char("Lie still; you are DEAD!\n\r", ch);
      return;
    case POS_INCAP:
    case POS_MORTAL:
      send_to_char("You are hurt far too bad for that.\n\r", ch);
      return;
    case POS_STUNNED:
      send_to_char("You are too stunned for that.\n\r", ch);
      return;
  }

  one_argument(argument, arg);
  victim = NULL;
  if (arg[0] == '\0')
  {
    sprintf(buf, "{w[{dGl{dob{dal {dSo{dci{dal{w] {x'%s'",
            social_table[cmd].char_no_arg);
    act_new(buf, ch, NULL, NULL, TO_CHAR, POS_DEAD);
    sprintf(buf, "{w[{dGl{dob{dal {dSo{dci{dal{w] {x'%s'",
            social_table[cmd].others_no_arg);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *vch;

      vch = d->original ? d->original : d->character;
      if (d->connected == CON_PLAYING && d->character != ch &&
          //                 !IS_SET ( vch->comm, COMM_NOGOC ) &&
          !IS_SET(vch->comm, COMM_QUIET))
      {
        act_new(buf, ch, NULL, vch, TO_VICT, POS_DEAD);
      }
    }
  }
  else if ((victim = get_char_world(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  else if (victim == ch)
  {
    sprintf(buf, "{w[{dGl{dob{dal {dSo{dci{dal{w] {x'%s'",
            social_table[cmd].char_auto);
    act_new(buf, ch, NULL, NULL, TO_CHAR, POS_DEAD);
    sprintf(buf, "{w[{dGl{dob{dal {dSo{dci{dal{w] {x'%s'",
            social_table[cmd].others_auto);
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *vch;

      vch = d->original ? d->original : d->character;
      if (d->connected == CON_PLAYING && d->character != ch &&
          //            !IS_SET ( vch->comm, COMM_NOGOC ) &&
          !IS_SET(vch->comm, COMM_QUIET))
      {
        act_new(buf, ch, NULL, vch, TO_VICT, POS_DEAD);
      }
    }
  }
  else
  {
    sprintf(buf, "{w[{dGl{dob{dal {dSo{dci{dal{w] {x'%s'",
            social_table[cmd].char_found);
    act_new(buf, ch, NULL, victim, TO_CHAR, POS_DEAD);
    sprintf(buf, "{w[{dGl{dob{dal {dSo{dci{dal{w] {x'%s'",
            social_table[cmd].vict_found);
    act_new(buf, ch, NULL, victim, TO_VICT, POS_DEAD);
    sprintf(buf, "{w[{dGl{dob{dal {dSo{dci{dal{w] {x'%s'",
            social_table[cmd].others_found);
    for (counter = 0; buf[counter + 1] != '\0'; counter++)
    {
      if (buf[counter] == '$' && buf[counter + 1] == 'N')
      {
        strcpy(buf2, buf);
        buf2[counter] = '\0';
        strcat(buf2, victim->name);
        for (count = 0; buf[count] != '\0'; count++)
        {
          buf[count] = buf[count + counter + 2];
        }
        strcat(buf2, buf);
        strcpy(buf, buf2);
      }
      else if (buf[counter] == '$' && buf[counter + 1] == 'E')
      {
        switch (victim->sex)
        {
          default:
            strcpy(buf2, buf);
            buf2[counter] = '\0';
            strcat(buf2, "it");
            for (count = 0; buf[count] != '\0'; count++)
            {
              buf[count] = buf[count + counter + 2];
            }
            strcat(buf2, buf);
            strcpy(buf, buf2);
            break;
          case 1:
            strcpy(buf2, buf);
            buf2[counter] = '\0';
            strcat(buf2, "it");
            for (count = 0; buf[count] != '\0'; count++)
            {
              buf[count] = buf[count + counter + 2];
            }
            strcat(buf2, buf);
            strcpy(buf, buf2);
            break;
          case 2:
            strcpy(buf2, buf);
            buf2[counter] = '\0';
            strcat(buf2, "it");
            for (count = 0; buf[count] != '\0'; count++)
            {
              buf[count] = buf[count + counter + 2];
            }
            strcat(buf2, buf);
            strcpy(buf, buf2);
            break;
        }
      }
      else if (buf[counter] == '$' && buf[counter + 1] == 'M')
      {
        buf[counter] = '%';
        buf[counter + 1] = 's';
        switch (victim->sex)
        {
          default:
            strcpy(buf2, buf);
            buf2[counter] = '\0';
            strcat(buf2, "it");
            for (count = 0; buf[count] != '\0'; count++)
            {
              buf[count] = buf[count + counter + 2];
            }
            strcat(buf2, buf);
            strcpy(buf, buf2);
            break;
          case 1:
            strcpy(buf2, buf);
            buf2[counter] = '\0';
            strcat(buf2, "him");
            for (count = 0; buf[count] != '\0'; count++)
            {
              buf[count] = buf[count + counter + 2];
            }
            strcat(buf2, buf);
            strcpy(buf, buf2);
            break;
          case 2:
            strcpy(buf2, buf);
            buf2[counter] = '\0';
            strcat(buf2, "her");
            for (count = 0; buf[count] != '\0'; count++);
            {
              buf[count] = buf[count + counter + 2];
            }
            strcat(buf2, buf);
            strcpy(buf, buf2);
            break;
        }
      }
      else if (buf[counter] == '$' && buf[counter + 1] == 'S')
      {
        switch (victim->sex)
        {
          default:
            strcpy(buf2, buf);
            buf2[counter] = '\0';
            strcat(buf2, "its");
            for (count = 0; buf[count] != '\0'; count++)
            {
              buf[count] = buf[count + counter + 2];
            }
            strcat(buf2, buf);
            strcpy(buf, buf2);
            break;
          case 1:
            strcpy(buf2, buf);
            buf2[counter] = '\0';
            strcat(buf2, "his");
            for (count = 0; buf[count] != '\0'; count++)
            {
              buf[count] = buf[count + counter + 2];
            }
            strcat(buf2, buf);
            strcpy(buf, buf2);
            break;
          case 2:
            strcpy(buf2, buf);
            buf2[counter] = '\0';
            strcat(buf2, "hers");
            for (count = 0; buf[count] != '\0'; count++)
            {
              buf[count] = buf[count + counter + 2];
            }
            strcat(buf2, buf);
            strcpy(buf, buf2);
            break;
        }
      }

    }
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      CHAR_DATA *vch;

      vch = d->original ? d->original : d->character;
      if (d->connected == CON_PLAYING && d->character != ch &&
          d->character != victim &&
          //                    !IS_SET ( vch->comm, COMM_NOGOC ) &&
          !IS_SET(vch->comm, COMM_QUIET))
      {
        act_new(buf, ch, NULL, vch, TO_VICT, POS_DEAD);
      }
    }
  }
  return;
}

CH_CMD(do_backup)
{
  if (ch->level < 5)
  {
    WAIT_STATE(ch, 4);
  }

  save_char_obj(ch);
  backup_char_obj(ch);
  //    send_to_char ( "{YYour P-File has been backed up.{x\n\r", ch );
  return;
}

CH_CMD(do_newname)
{
  FILE *fp;
  char arg1[MSL], arg2[MSL];
  char strsave[MAX_INPUT_LENGTH];
  char buf[MSL];
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  arg1[0] = '\0';
  arg2[0] = '\0';

  if (ch->nameauthed)
  {
    send_to_char("Your name has been authorized.  You may not change it.\n\r",
                 ch);
    return;
  }

  if (ch->namedenied == 0)
  {
    send_to_char("You may not change your name unless it's been denied.\n\r",
                 ch);
    return;
  }

  if ((arg1[0] == '\0') || (arg2[0] == '\0'))
  {
    send_to_char("Usage:  newname <yournewname> <yournewname>\n\r", ch);
    return;
  }

  if (strcmp(arg1, arg2))
  {
    send_to_char("You must enter the new name twice for verification.\n\r",
                 ch);
    return;
  }

  if (!check_parse_name(arg1))
  {
    send_to_char("That name is unavailable.  Please try another.", ch);
    return;
  }

  sprintf(buf, "../player/%s", capitalize(arg1));

  if (!((fp = file_open(buf, "r")) == NULL))
  {
    file_close(fp);
    send_to_char
      ("A character with that name already exists.  Please try another.", ch);
    return;
  }

  sprintf(strsave, "%s%s", PLAYER_DIR, capitalize(ch->name));
  sprintf(log_buf, "(AUTHNAME) %s has chosen a new name for approval: %s\n\r",
          ch->name, capitalize(arg1));
  wiznet(log_buf, NULL, NULL, WIZ_NEWBIE, 0, 0);
  strcpy(ch->name, capitalize(arg1));
  unlink(strsave);
  ch->namedenied = 0;
  save_char_obj(ch);

  sprintf(buf, "Your name has been changed to %s..\n\r", ch->name);
  send_to_char(buf, ch);
  send_to_char
    ("Your new name will be reviewed by the administration as soon as possible.\n\r",
     ch);

  return;
}

CH_CMD(do_ignor)
{
  send_to_char("I'm sorry, ignore must be entered in full.\n\r", ch);
  return;
}

CH_CMD(do_ignore)
{
  CHAR_DATA *rch;
  char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int pos;
  bool found = false;

  arg[0] = '\0';

  if (ch->desc == NULL)
    rch = ch;
  else
    rch = ch->desc->original ? ch->desc->original : ch;

  if (IS_NPC(rch))
    return;

  smash_tilde(argument);

  argument = one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    if (rch->pcdata->forget[0] == NULL)
    {
      send_to_char("You are not ignoring anyone.\n\r", ch);
      return;
    }
    send_to_char("You are currently ignoring:\n\r", ch);

    for (pos = 0; pos < MAX_FORGET; pos++)
    {
      if (rch->pcdata->forget[pos] == NULL)
        break;

      sprintf(buf, "    %s\n\r", rch->pcdata->forget[pos]);
      send_to_char(buf, ch);
    }
    return;
  }

  for (pos = 0; pos < MAX_FORGET; pos++)
  {
    if (rch->pcdata->forget[pos] == NULL)
      break;

    if (!str_cmp(arg, rch->pcdata->forget[pos]))
    {
      send_to_char("You have already ignored that person.\n\r", ch);
      return;
    }
  }

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *wch;

    if (d->connected != CON_PLAYING || !can_see(ch, d->character))
      continue;

    wch = (d->original != NULL) ? d->original : d->character;

    if (!can_see(ch, wch))
      continue;

    if (!str_cmp(arg, wch->name))
    {
      found = true;
      if (wch == ch)
      {
        send_to_char
          ("You ignore yourself for a moment, but it passes.\n\r", ch);
        return;
      }
      if (wch->level >= LEVEL_IMMORTAL)
      {
        send_to_char("That person is very hard to ignore.\n\r", ch);
        return;
      }
    }
  }

  if (!found)
  {
    send_to_char("No one by that name is playing.\n\r", ch);
    return;
  }

  for (pos = 0; pos < MAX_FORGET; pos++)
  {
    if (rch->pcdata->forget[pos] == NULL)
      break;
  }

  if (pos >= MAX_FORGET)
  {
    send_to_char("Sorry, you have reached the ignore limit.\n\r", ch);
    return;
  }

  /* make a new forget */
  rch->pcdata->forget[pos] = str_dup(arg);
  sprintf(buf, "You are now deaf to %s.\n\r", arg);
  send_to_char(buf, ch);
}

CH_CMD(do_unignor)
{
  send_to_char("I'm sorry, unignore must be entered in full.\n\r", ch);
  return;
}

CH_CMD(do_unignore)
{
  CHAR_DATA *rch;
  char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
  int pos;
  bool found = false;

  arg[0] = '\0';

  if (ch->desc == NULL)
    rch = ch;
  else
    rch = ch->desc->original ? ch->desc->original : ch;

  if (IS_NPC(rch))
    return;

  argument = one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    if (rch->pcdata->forget[0] == NULL)
    {
      send_to_char("You are not ignoring anyone.\n\r", ch);
      return;
    }
    send_to_char("You are currently ignoring:\n\r", ch);

    for (pos = 0; pos < MAX_FORGET; pos++)
    {
      if (rch->pcdata->forget[pos] == NULL)
        break;

      sprintf(buf, "    %s\n\r", rch->pcdata->forget[pos]);
      send_to_char(buf, ch);
    }
    return;
  }

  for (pos = 0; pos < MAX_FORGET; pos++)
  {
    if (rch->pcdata->forget[pos] == NULL)
      break;

    if (found)
    {
      rch->pcdata->forget[pos - 1] = rch->pcdata->forget[pos];
      rch->pcdata->forget[pos] = NULL;
      continue;
    }

    if (!str_cmp(arg, rch->pcdata->forget[pos]))
    {
      send_to_char("Ignore removed.\n\r", ch);
      free_string(rch->pcdata->forget[pos]);
      rch->pcdata->forget[pos] = NULL;
      found = true;
    }
  }

  if (!found)
    send_to_char("No one by that name is ignored.\n\r", ch);
}
