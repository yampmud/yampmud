
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
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "merc.h"
#include "music.h"
#include "lookup.h"
#include "recycle.h"
/* used for saving */

/*
 * Advancement stuff.
 */

void advance_level(CHAR_DATA * ch)
{
  char buf[MAX_STRING_LENGTH];
  int loc, i;
  int add_hp;
  int add_mana;
  int add_move;
  int add_prac;
  OBJ_DATA *obj;

  ch->pcdata->last_level =
    (ch->played + (int) (current_time - ch->logon)) / 3600;

  sprintf(buf, "the %s",
          title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0]);

  add_hp =
    con_app[get_curr_stat(ch, STAT_CON)].hitp +
    number_range(class_table[ch->class].hp_min,
                 class_table[ch->class].hp_max);
  add_mana =
    number_range(10,
                 (5 * get_curr_stat(ch, STAT_INT) +
                  get_curr_stat(ch, STAT_WIS)));
  if (!class_table[ch->class].fMana)
    add_mana /= 3.5;
  add_move =
    number_range(1,
                 (get_curr_stat(ch, STAT_CON) +
                  get_curr_stat(ch, STAT_DEX)) / 5);
  add_prac = wis_app[get_curr_stat(ch, STAT_WIS)].practice;

  add_hp = add_hp * 9 / 10;
  add_hp = add_hp * 7 / 4;

  add_mana = add_mana * 9 / 10;
  add_mana = add_mana * 7 / 4;

  add_move = add_move * 9 / 10;
  add_move = add_move * 2;

  add_hp = UMAX(4, add_hp);
  add_mana = UMAX(4, add_mana);

  add_move = UMAX(12, add_move);

  ch->max_hit += add_hp;
  ch->max_mana += add_mana;
  ch->max_move += add_move;
  ch->practice += add_prac;
  ch->train += number_range(1, 4);

  ch->pcdata->perm_hit += add_hp;
  ch->pcdata->perm_mana += add_mana;
  ch->pcdata->perm_move += add_move;

  sprintf(buf,
          "\nYour gain is: {r%d{W/{R%ld{Bhp{x {m%d{W/{M%ld{Bm{x {y%d{W/{Y%ld{Bmv {c%d{M/{C%d{Bprac{x.\n\r",
          add_hp, ch->max_hit, add_mana, ch->max_mana, add_move,
          ch->max_move, add_prac, ch->practice);
  send_to_char(buf, ch);
  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    if (IS_OBJ_STAT(obj, ITEM_QUEST))
      update_questobjs(ch, obj);
  for (loc = 0; loc < MAX_WEAR; loc++)
  {
    obj = get_eq_char(ch, loc);
    if (obj == NULL)
      continue;
    if (obj->clan)
    {
      for (i = 0; i < 4; i++)
      {
        ch->armor[i] += apply_ac(ch->level - 1, obj, loc, i);
        ch->armor[i] -= apply_ac(ch->level, obj, loc, i);
      }
    }
  }
  return;
}

void advance_level_quiet(CHAR_DATA * ch)
{
  char buf[MAX_STRING_LENGTH];
  int loc, i;
  int add_hp;
  int add_mana;
  int add_move;
  int add_prac;
  OBJ_DATA *obj;

  ch->pcdata->last_level =
    (ch->played + (int) (current_time - ch->logon)) / 3600;

  sprintf(buf, "the %s",
          title_table[ch->class][ch->level][ch->sex == SEX_FEMALE ? 1 : 0]);

  add_hp =
    con_app[get_curr_stat(ch, STAT_CON)].hitp +
    number_range(class_table[ch->class].hp_min,
                 class_table[ch->class].hp_max);
  add_mana =
    number_range(4,
                 (4 * get_curr_stat(ch, STAT_INT) +
                  get_curr_stat(ch, STAT_WIS)) / 2);
  if (!class_table[ch->class].fMana)
    add_mana /= 2;
  add_move =
    number_range(1,
                 (get_curr_stat(ch, STAT_CON) +
                  get_curr_stat(ch, STAT_DEX)) / 5);
  add_prac = wis_app[get_curr_stat(ch, STAT_WIS)].practice;

  add_hp = add_hp * 9 / 10;
  add_hp = add_hp * 7 / 4;

  add_mana = add_mana * 9 / 10;
  add_mana = add_mana * 7 / 4;

  add_move = add_move * 9 / 10;
  add_move = add_move * 2;

  add_hp = UMAX(4, add_hp);
  add_mana = UMAX(4, add_mana);

  add_move = UMAX(12, add_move);

  ch->max_hit += add_hp;
  ch->max_mana += add_mana;
  ch->max_move += add_move;
  ch->practice += add_prac;
  ch->train += 1;

  ch->pcdata->perm_hit += add_hp;
  ch->pcdata->perm_mana += add_mana;
  ch->pcdata->perm_move += add_move;

  for (loc = 0; loc < MAX_WEAR; loc++)
  {
    obj = get_eq_char(ch, loc);
    if (obj == NULL)
      continue;
    if (obj->clan)
    {
      for (i = 0; i < 4; i++)
      {
        ch->armor[i] += apply_ac(ch->level - 1, obj, loc, i);
        ch->armor[i] -= apply_ac(ch->level, obj, loc, i);
      }
    }
  }
  return;
}

void gain_exp(CHAR_DATA * ch, int gain)
{
  char buf[MAX_STRING_LENGTH];
  int exp, i;

  if (IS_NPC(ch) || ch->level >= LEVEL_HERO)
    return;

  exp = UMAX(exp_per_level(ch, ch->pcdata->points), ch->exp + gain);

  i = 1;

  /* Characters with unauthorized names can only reach MAX_LEVEL_NOAUTH */
  while (exp_per_level(ch, ch->pcdata->points) * (ch->level + i) < exp)
  {
    if ((ch->level + i > MAX_LEVEL_NOAUTH) && (ch->nameauthed == 0))
    {
      send_to_char
        ("Your name must be authorized before you may gain that much experience.\n\r",
         ch);
      return;
    }
    i++;
  }

  ch->exp = exp;
  while (ch->level < LEVEL_HERO &&
         ch->exp >= exp_per_level(ch, ch->pcdata->points) * (ch->level + 1))
  {
    send_to_char("{YYou raise a level{R!!{x  ", ch);
    ch->level += 1;
    sprintf(buf, "{R%s {Bhas {Rattained {Blevel{Y %d{G!{g!{G!{g!{x",
            ch->name, ch->level);
    wiznet(buf, ch, NULL, WIZ_LEVELS, 0, 0);
    advance_level(ch);
    affect_strip(ch, gsn_plague);
    affect_strip(ch, gsn_poison);
    affect_strip(ch, gsn_blindness);
    affect_strip(ch, gsn_sleep);
    affect_strip(ch, gsn_curse);
    save_char_obj(ch);

    if (!ch->pcdata->pkset && !is_clan(ch))
    {
      switch (ch->level)
      {
        case 40:
          add_buf(ch->pcdata->buffer,
                  "System Message: {RYou are now level 40.\n\r"
                  "If you wish to become a PK player you MUST do so before level 50.\n\r"
                  "Type PKSET at any time to do so.\n\r"
                  "Rerolling is the only way after level 50 to change your PK status.\n\r"
                  "You DO NOT have to go PK before 4th tier!\n\r");
          ch->tells++;
          break;
        case 49:
          add_buf(ch->pcdata->buffer,
                  "System Message: {RYou are now level 49.\n\r"
                  "If you wish to become a PK player you MUST do so now.\n\r"
                  "Type PKSET at any time to do so.\n\r"
                  "Rerolling is the only way after level 50 to change your PK status.\n\r"
                  "You DO NOT have to go PK before 4th tier!\n\r");
          ch->tells++;
          break;
        case 50:
          add_buf(ch->pcdata->buffer,
                  "System Message: {RYou are now level 50.\n\r"
                  "You are now permanently a non-playerkill player.\n\r"
                  "Rerolling is the only way to change your PK status now.\n\r"
                  "You DO NOT have to become PK before 4th tier!\n\r");
          ch->tells++;
          break;
        default:
          break;
      }
    }
  }
  return;
}

/*
 * Regeneration stuff.
 */
int hit_gain(CHAR_DATA * ch)
{
  int gain;
  int number;

  if (ch->in_room == NULL)
    return 0;
  if (IS_NPC(ch))
  {
    gain = 5 + ch->level;
    if (IS_AFFECTED(ch, AFF_REGENERATION))
      gain *= 2;
    switch (ch->position)
    {
      default:
        gain /= 2;
        break;
      case POS_SLEEPING:
        gain = 3 * gain / 2;
        break;
      case POS_RESTING:
        break;
      case POS_FIGHTING:
        gain /= 3;
        break;
    }

  }
  else
  {
    gain = UMAX(3, get_curr_stat(ch, STAT_CON) - 3 + ch->level / 2);
    gain += class_table[ch->class].hp_max - 10;
    number = number_percent();
    if (number < get_skill(ch, gsn_fast_healing))
    {
      gain += number * gain / 100;
      if (ch->hit < ch->max_hit)
        check_improve(ch, gsn_fast_healing, true, 8);
    }

    switch (ch->position)
    {
      default:
        gain /= 4;

        break;
      case POS_SLEEPING:
        break;
      case POS_RESTING:
        gain /= 2;
        break;
      case POS_FIGHTING:
        gain /= 6;
        break;
    }
  }

  gain = gain * ch->in_room->heal_rate / 100;
  if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
    gain = gain * ch->on->value[3] / 100;
  if (IS_AFFECTED(ch, AFF_POISON))
    gain /= 4;

  if (IS_AFFECTED(ch, AFF_PLAGUE))
    gain /= 8;
  if (IS_AFFECTED(ch, AFF_HASTE) || IS_AFFECTED(ch, AFF_SLOW))
    gain /= 2;
  return UMIN(gain, ch->max_hit - ch->hit);
}

int mana_gain(CHAR_DATA * ch)
{
  int gain;
  int number;

  if (ch->in_room == NULL)
    return 0;
  if (IS_NPC(ch))
  {
    gain = 5 + ch->level;
    switch (ch->position)
    {
      default:
        gain /= 2;
        break;
      case POS_SLEEPING:
        gain = 3 * gain / 2;
        break;
      case POS_RESTING:
        break;
      case POS_FIGHTING:
        gain /= 3;
        break;
    }
  }
  else
  {
    gain =
      ((get_curr_stat(ch, STAT_WIS) +
        get_curr_stat(ch, STAT_INT) + ch->level) / 3) * 2;
    number = number_percent();
    if (number < get_skill(ch, gsn_meditation))
    {
      gain += number * gain / 100;
      if (ch->mana < ch->max_mana)
        check_improve(ch, gsn_meditation, true, 8);
    }
    if (!class_table[ch->class].fMana)
      gain /= 2;
    switch (ch->position)
    {
      default:
        gain /= 4;

        break;
      case POS_SLEEPING:
        break;
      case POS_RESTING:
        gain /= 2;
        break;
      case POS_FIGHTING:
        gain /= 6;
        break;
    }
  }

  gain = gain * ch->in_room->mana_rate / 100;
  if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
    gain = gain * ch->on->value[4] / 100;

  if (IS_AFFECTED(ch, AFF_POISON))
    gain /= 4;

  if (IS_AFFECTED(ch, AFF_PLAGUE))
    gain /= 8;
  if (IS_AFFECTED(ch, AFF_HASTE) || IS_AFFECTED(ch, AFF_SLOW))
    gain /= 2;
  return UMIN(gain, ch->max_mana - ch->mana);
}

int move_gain(CHAR_DATA * ch)
{
  int gain;

  if (ch->in_room == NULL)
    return 0;
  if (IS_NPC(ch))
  {
    gain = ch->level;
  }
  else
  {
    gain = UMAX(15, ch->level);
    switch (ch->position)
    {
      case POS_SLEEPING:
        gain += get_curr_stat(ch, STAT_DEX);
        break;
      case POS_RESTING:
        gain += get_curr_stat(ch, STAT_DEX) / 2;
        break;
    }
  }

  gain = gain * ch->in_room->heal_rate / 100;
  if (ch->on != NULL && ch->on->item_type == ITEM_FURNITURE)
    gain = gain * ch->on->value[3] / 100;
  if (IS_AFFECTED(ch, AFF_POISON))
    gain /= 4;

  if (IS_AFFECTED(ch, AFF_PLAGUE))
    gain /= 8;
  if (IS_AFFECTED(ch, AFF_HASTE) || IS_AFFECTED(ch, AFF_SLOW))
    gain /= 2;
  return UMIN(gain, ch->max_move - ch->move);
}

void gain_condition(CHAR_DATA * ch, int iCond, int value)
{
  long condition;

  if (value == 0 || IS_NPC(ch) || ch->level >= 202)
    return;

  condition = ch->pcdata->condition[iCond];

  if (condition == -1)
    return;

  ch->pcdata->condition[iCond] = URANGE(0, condition + value, 48);
  if (ch->pcdata->condition[iCond] == 0)
  {
    switch (iCond)
    {
      case COND_DRUNK:
        if (condition != 0)
          send_to_char("You are sober.\n\r", ch);
        break;
    }
  }

  return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Merc cpu time.
 * -- Furey
 */
void mobile_update(void)
{
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  EXIT_DATA *pexit;
  int door;

  /* Examine all mobs. */
  for (ch = char_list; ch != NULL; ch = ch_next)
  {
    ch_next = ch->next;
    if (!IS_NPC(ch) || ch->in_room == NULL || IS_AFFECTED(ch, AFF_CHARM))
      continue;
    if (ch->in_room->area->empty && !IS_SET(ch->act, ACT_UPDATE_ALWAYS))
      continue;
    /* Examine call for special procedure */
    mp_percent_trigger(ch, NULL, NULL, NULL, TRIG_RANDOM);
    if (ch->spec_fun != 0)
    {
      if ((*ch->spec_fun) (ch))
        continue;
    }

    if (ch->pIndexData->pShop != NULL)  /* give him some platinum */
      if ((ch->platinum * 100 + ch->gold) < ch->pIndexData->wealth)
      {
        ch->platinum +=
          ch->pIndexData->wealth * number_range(1, 20) / 5000000;
        ch->gold += ch->pIndexData->wealth * number_range(1, 20) / 50000;
      }

    /* That's all for sleeping / busy monster, and empty zones */
    if (ch->position != POS_STANDING)
      continue;
    /* Scavenge */
    if (IS_SET(ch->act, ACT_SCAVENGER) && ch->in_room->contents != NULL
        && number_bits(6) == 0)
    {
      OBJ_DATA *obj;
      OBJ_DATA *obj_best;
      int max;

      max = 1;
      obj_best = 0;
      for (obj = ch->in_room->contents; obj; obj = obj->next_content)
      {
        if (CAN_WEAR(obj, ITEM_TAKE) && can_loot(ch, obj) &&
            obj->cost > max && obj->cost > 0)
        {
          obj_best = obj;
          max = obj->cost;
        }
      }

      if (obj_best)
      {
        obj_from_room(obj_best);
        obj_to_char(obj_best, ch);
        act("$n gets $p.", ch, obj_best, NULL, TO_ROOM);
      }
    }

    /* Wander */
    if (!IS_SET(ch->act, ACT_SENTINEL) && number_bits(3) == 0 &&
        (door = number_bits(6)) < MAX_DIR &&
        (pexit = ch->in_room->exit[door]) != NULL &&
        pexit->u1.to_room != NULL &&
        !IS_SET(pexit->exit_info, EX_CLOSED) &&
        !IS_SET(pexit->u1.to_room->room_flags, ROOM_NO_MOB) &&
        (!IS_SET(ch->act, ACT_STAY_AREA) ||
         pexit->u1.to_room->area == ch->in_room->area) &&
        (!IS_SET(ch->act, ACT_OUTDOORS) ||
         !IS_SET(pexit->u1.to_room->room_flags, ROOM_INDOORS)) &&
        (!IS_SET(ch->act, ACT_INDOORS) ||
         IS_SET(pexit->u1.to_room->room_flags, ROOM_INDOORS)) &&
        (IS_SET(ch->act2, ACT2_STAY_SECTOR) &&
         (pexit->u1.to_room->sector_type == ch->in_room->sector_type)))
    {
      move_char(ch, door, false, false);
    }
  }

  return;
}

/*
 * Update the weather.
 */
void weather_update(void)
{
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  int diff;

  buf[0] = '\0';

  switch (++time_info.hour)
  {
    case 5:
      weather_info.sunlight = SUN_LIGHT;
      strcat(buf, "{bThe {Yday{b has begun.{x\n\r");
      break;
    case 6:
      weather_info.sunlight = SUN_RISE;
      strcat(buf, "{yThe {Ysun{y rises in the east.{x\n\r");
      break;
    case 19:
      weather_info.sunlight = SUN_SET;
      strcat(buf, "{yThe {Ysun{y slowly disappears in the west.{x\n\r");
      break;
    case 20:
      weather_info.sunlight = SUN_DARK;
      strcat(buf, "{bThe {Dnight{B has begun.{x\n\r");
      break;
    case 24:
      time_info.hour = 0;
      time_info.day++;
      break;
  }

  if (time_info.day >= 35)
  {
    time_info.day = 0;
    time_info.month++;
  }

  if (time_info.month >= 17)
  {
    time_info.month = 0;
    time_info.year++;
  }

  /* 
   * Weather change.
   */
  if (time_info.month >= 9 && time_info.month <= 16)
    diff = weather_info.mmhg > 985 ? -2 : 2;
  else
    diff = weather_info.mmhg > 1015 ? -2 : 2;
  weather_info.change += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);

  weather_info.change = UMAX(weather_info.change, -12);
  weather_info.change = UMIN(weather_info.change, 12);
  weather_info.mmhg += weather_info.change;
  weather_info.mmhg = UMAX(weather_info.mmhg, 960);
  weather_info.mmhg = UMIN(weather_info.mmhg, 1040);
  switch (weather_info.sky)
  {
    default:
      bug("Weather_update: bad sky %d.", weather_info.sky);
      weather_info.sky = SKY_CLOUDLESS;
      break;
    case SKY_CLOUDLESS:
      if (weather_info.mmhg < 990 ||
          (weather_info.mmhg < 1010 && number_bits(2) == 0))
      {
        strcat(buf, "{wThe sky is getting cloudy.{x\n\r");
        weather_info.sky = SKY_CLOUDY;
      }
      break;
    case SKY_CLOUDY:
      if (weather_info.mmhg < 970 ||
          (weather_info.mmhg < 990 && number_bits(2) == 0))
      {
        strcat(buf, "{wIt starts to {Dr{wa{Di{wn{x.\n\r");
        weather_info.sky = SKY_RAINING;
      }

      if (weather_info.mmhg > 1030 && number_bits(2) == 0)
      {
        strcat(buf, "{WThe {Dc{wl{Do{wu{Dd{ws{c disappear.{x\n\r");
        weather_info.sky = SKY_CLOUDLESS;
      }
      break;
    case SKY_RAINING:
      if (weather_info.mmhg < 970 && number_bits(2) == 0)
      {
        strcat(buf,
               "{xL{Ci{xg{Dh{xt{Cn{xi{Dn{xg {Yf{wl{Ya{ws{Yh{we{Ys{x in the {csky.{x\n\r");
        weather_info.sky = SKY_LIGHTNING;
      }

      if (weather_info.mmhg > 1030 ||
          (weather_info.mmhg > 1010 && number_bits(2) == 0))
      {
        strcat(buf, "{DThe rain {cstopped.{x\n\r");
        weather_info.sky = SKY_CLOUDY;
      }
      break;
    case SKY_LIGHTNING:
      if (weather_info.mmhg > 1010 ||
          (weather_info.mmhg > 990 && number_bits(2) == 0))
      {
        strcat(buf, "{DThe {xl{Ci{xg{Ch{xt{Cn{xi{Cn{xg has stopped.{x\n\r");
        weather_info.sky = SKY_RAINING;
        break;
      }
      break;
  }

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING && IS_OUTSIDE(d->character) &&
        IS_AWAKE(d->character) && strlen(buf) > 1)
      send_to_char(buf, d->character);
  }
  return;
}

/*
 * Update the bank system
 * (C) 1996 The Maniac from Mythran Mud
 *
 * This updates the shares value (I hope)
 */
void bank_update(void)
{
  FILE *fp;

  if (time_info.hour < 9 || time_info.hour > 17)
    return;
  share_value += number_range(1, 10);
  if (share_value > 300)
    share_value -= 7;           /* Price stabilization */
  else
    share_value -= 3;
  if ((fp = file_open(BANK_FILE, "w")) == NULL)
  {
    bug("bank_update:  fopen of BANK_FILE failed", 0);
    file_close(fp);
    return;
  }
  fprintf(fp, "SHARE_VALUE %d\n", share_value);
  file_close(fp);
}

/*
 * Update all chars, including mobs.
 */
void char_update(void)
{
  int hour;
  int day;
  int month;
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;

  char tbuf[MSL];
  char rewbuf[MSL];
  char buf[MAX_STRING_LENGTH];
  char buftime[MAX_STRING_LENGTH];
  char bufinfo[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  FILE *fp;
  int countu;
  bool is_xmas = false;
  bool mobdeath = false;

  uptime_ticks++;

  randomize_entrances(ROOM_VNUM_CLANS);

  if (hour_ticks == 60)
  {
    hour_ticks = 0;
    logins_hour = 0;
  }
  else
    hour_ticks++;

  if (arena != FIGHT_OPEN)
    arenacount++;

  hour = localtime(&current_time)->tm_hour;
  day = localtime(&current_time)->tm_mday;
  month = localtime(&current_time)->tm_mon;

  if (hour == 12 && is_mid == true)
  {
    if (max_on > 0)
      max_on = (max_on / 2);
    is_mid = false;
  }
  else
  {
    is_mid = true;
  }

  if ((day == 24 || day == 25) && month == 11)  // Its actually 12
    is_xmas = true;

  if (hour == 5 || hour == 11 || hour == 17 || hour == 23 || is_xmas)
  {
    if (happy_hour == false)
    {
      if (is_xmas)
        do_gmessage
          ("{W[ {GM {RE {GR {RR {GY   {RC {GH {RR {GI {RS {GT {RM {GA {RS {W] {wHappy hour enabled for the 24th & 25th!\n\r");
      else
        do_gmessage
          ("{W[{RHAPPY HOUR{W] {YDouble {CXP {c& {CQP {w[{GON{w]\n\r");

      happy_hour = true;
    }
  }
  else
  {
    if (happy_hour == true)
    {
      do_gmessage
        ("{W[{RHAPPY HOUR{W] Aww.. {wHappy Hour{W is over.. *sniff*{x\n\r");
      happy_hour = false;
    }
  }

  if (uptime_ticks <= 2)
  {
    if (!(fp = file_open(MAX_ON_FILE, "r")))
    {
      file_close(fp);
      return;
    }

    for (;;)
    {
      char *word;
      char letter;

      do
      {
        letter = getc(fp);
        if (feof(fp))
        {
          file_close(fp);
          return;

        }
      }
      while (isspace(letter));
      ungetc(letter, fp);

      word = fread_word(fp);
      if (!str_cmp(word, "MAX_ON"))
      {
        int number;

        number = fread_number(fp);
        if (number > 2)
          max_on = number;
        file_close(fp);
        return;

      }
    }
  }
  else
  {
    if ((fp = file_open(MAX_ON_FILE, "w")) == NULL)
    {
      bug("max_on_update:  fopen of MAX_ON_FILE failed", 0);
      file_close(fp);
      return;
    }
    fprintf(fp, "MAX_ON %d\n", max_on);
    file_close(fp);
  }

  if (--copyover_countdown >= 0)
  {
    if (copyover_countdown == 0)
    {
      copyover_countdown = -1;
      do_copyover(NULL, "");
    }
    else
    {
      if (copyover_countdown == 5 || copyover_countdown == 1 ||
          copyover_countdown == 2)
      {
        if (copyover_countdown == 1)
        {
          do_aclear(NULL, NULL);
          sprintf(buf,
                  "\n\r{W[{RCOPYOVER{W] Login {YLOCKDOWN{x is now {D[{GON{D]\n\r");
          do_gmessage(buf);
        }
        else
        {
          if (copyover_countdown == 2)
          {
            sprintf(buf,
                    "\n\r{W[{RCOPYOVER{W] Command {YLOCKDOWN{W is now {D[{GON{D]\n\r");
            do_gmessage(buf);
          }
        }
        sprintf(buftime,
                "\n\r{W[{RCOPYOVER{W] Copyover will execute in {D[{R%d{D]{W minutes.{x\n\r",
                copyover_countdown);
        sprintf(bufinfo,
                "\n\r{W[{RCOPYOVER{W] Please see {RHELP COPYOVER{W for more information.{x\n\r");
        do_gmessage(buftime);
        do_gmessage(bufinfo);
      }
    }
  }
  if (!is_mass_arena)
    is_mass_arena_fight = false;

  if (arena == FIGHT_OPEN)
  {
    arenacount = 0;

    if (is_mass_arena)
      mass_arena_ticks++;
    else
    {
      mass_arena_ticks = 0;
      mass_arena_players_joined = 0;
      sprintf(lastwinner, "Nobody");
    }

    if (mass_arena_players < 3)
    {
      if (mass_arena_valid_ticks > 0)
        mass_arena_valid_ticks--;
    }
    else
    {
      if (mass_arena_valid_ticks < 30)
        mass_arena_valid_ticks++;
    }

    if (mass_arena_valid_ticks >= 1 && !is_mass_arena && !is_mass_arena_fight)
    {
      do_gmessage
        ("{W[{RARENA{W] The Mass-Arena is now {D[{GOPEN{D]\n\r{W[{RARENA{W] Type: {RMASSARENA JOIN{w. You have {w[{R2{w] minutes to do so.{x\n\r");
      mass_arena_ticks = 0;
      mass_arena_players_joined = 0;
      is_mass_arena = true;
    }
    if (mass_arena_ticks >= 4 && !is_mass_arena_fight && is_mass_arena)
    {
      do_gmessage
        ("{W[{RARENA{W] Not enough participants. Mass-Arena is now {D[{RCLOSED{D]\n\r");
      is_mass_arena = false;
      is_mass_arena_fight = false;
      mass_arena_valid_ticks = 0;
      mass_arena_players_joined = 0;
      mass_arena_ticks = 0;
      do_aclear(NULL, NULL);
    }
  }

  mass_arena_players = 0;
  mass_arena_players_joined = 0;

  sync_max_ever();

  if (arenacount > 20)
    arenacount = 0;


  for (ch = char_list; ch != NULL; ch = ch_next)
  {

    AFFECT_DATA *paf;
    AFFECT_DATA *paf_next;

    ch_next = ch->next;

    if (!IS_NPC(ch))
      if (ch->desc)
        if (ch->desc->connected != CON_PLAYING)
          continue;

    if (!IS_NPC(ch) && !IS_IMMORTAL(ch))
    {
      if (IS_SET(ch->act2, PLR2_MASS_ARENA))
        mass_arena_players++;

      if (IS_SET(ch->act2, PLR2_MASS_JOINER) &&
          IS_SET(ch->act2, PLR2_MASS_ARENA))
        mass_arena_players_joined++;
    }
    if (!IS_NPC(ch))
    {
      if (ch->level > 1)
      {
        save_char_obj(ch);
      }

      if (ch->blackjack_timer >= 1)
      ch->blackjack_timer--;

      if (!is_pkill(ch))
      {
        ch->pk_timer = 0;
      }

      if (!IS_SET(ch->act2, PLR2_MASS_ARENA))
        REMOVE_BIT(ch->act2, PLR2_MASS_JOINER);

      if (mass_arena_ticks >= 2 && is_mass_arena &&
          mass_arena_players_joined >= 4)
      {
        do_start_massarena();
      }
      if (mass_arena_ticks == 1 && is_mass_arena)
      {
        send_to_char
          ("{w[{RARENA{w] The mass arena will start in {R60 seconds{w. Last call!{x\n\r",
           ch);
      }
      if (mass_arena_players < 2 && is_mass_arena && is_mass_arena_fight
          && IS_SET(ch->in_room->room_flags, ROOM_ARENA) &&
          IS_SET(ch->act2, PLR2_MASS_JOINER) &&
          IS_SET(ch->act2, PLR2_MASS_ARENA) && !IS_IMMORTAL(ch) &&
          is_name(ch->name, lastwinner))
      {
        char wbuf[MSL];

        sprintf(wbuf,
                "{W[{RARENA{W] {D[{G%s{D] has won the Mass-Arena!\n\r",
                ch->name);
        do_gmessage(wbuf);
        ch->qps += 10;
        stop_fighting(ch, true);
        ch->hit = ch->max_hit;
        ch->mana = ch->max_mana;
        ch->move = ch->max_move;
        update_pos(ch);
        sprintf(lastwinner, "Nobody");
        send_to_char("You have been awarded 10iqp for your victory!\n\r", ch);
        REMOVE_BIT(ch->act2, PLR2_MASS_JOINER);
        REMOVE_BIT(ch->act2, PLR2_MASS_ARENA);
        is_mass_arena = false;
        is_mass_arena_fight = false;
        do_aclear(NULL, NULL);
        break;
      }

      if (ch->max_move < 100)
        ch->max_move = 100;

      if (ch->max_mana < 100)
        ch->max_mana = 100;

      if (ch->move < 1)
        ch->move = (ch->max_move / 4);

      if ((((ch->played + (int) (current_time - ch->logon)) / 3600) == 10) &&
          str_cmp(ch->pcdata->refer, "None") && !IS_SET(ch->act2, PLR2_REFER)
          && ch->pcdata->refer != NULL)
      {
        sprintf(rewbuf,
                "System Message: You are now 10 hours old. If the player ( %s ) on this mud refered you please notify an immortal so the player can receive their referrer reward. Otherwise, ignore this message.\n\r",
                ch->pcdata->refer);
        add_buf(ch->pcdata->buffer, rewbuf);
        ch->tells++;

        SET_BIT(ch->act2, PLR2_REFER);
      }

      if (ch->tells > 0 && !IS_SET(ch->comm, COMM_AFK))
      {
        sprintf(tbuf,
                "You have {R%d{x tells waiting.\n\rType 'replay' to see tells.\n\r",
                ch->tells);
        send_to_char(tbuf, ch);
      }

      if (++ch->timer >= 10 && !IS_IMMORTAL(ch))
      {
        SET_BIT(ch->comm, COMM_AFK);
        REMOVE_BIT(ch->act2, PLR2_MASS_ARENA);
      }

      if (ch->challenge_timer > 0)
        --ch->challenge_timer;

      if (ch->stunned > 0 && ch->position != POS_FIGHTING)
        ch->stunned = 0;

      if (ch->corner_timer >= 1 && ch->desc)
      {
        char cbuf[MSL];

        sprintf(cbuf,
                "You have %d minutes remaining until you are free.\n\r",
                (ch->corner_timer));
        send_to_char(cbuf, ch);
        --ch->corner_timer;
      }
      if (ch->corner_timer == 0 && !IS_NPC(ch))
      {
        do_uncorner(ch, "");
        ch->corner_timer = -1;
      }
      if (ch->timer > 30 && !IS_IMMORTAL(ch))
      {
        force_quit(ch, "");
      }
    }
    if (IS_NPC(ch) && ch->hastimer)
    {
      if (++ch->timer > 5)
      {
        act("$n decays into dust.", ch, NULL, NULL, TO_ROOM);
        extract_char(ch, true);
        ch = NULL;
        continue;
      }
    }

    if (ch->position >= POS_STUNNED)
    {
      /* check to see if we need to go home */
      if (IS_NPC(ch) && ch->zone != NULL &&
          ch->zone != ch->in_room->area && ch->desc == NULL &&
          ch->fighting == NULL && !IS_AFFECTED(ch, AFF_CHARM) &&
          number_percent() < 5)
      {
        act("$n wanders on home.", ch, NULL, NULL, TO_ROOM);
        extract_char(ch, true);
        ch = NULL;
        continue;
      }

      if (ch->hit < ch->max_hit)
        ch->hit += hit_gain(ch);
      else
        ch->hit = ch->max_hit;
      if (ch->mana < ch->max_mana)
        ch->mana += mana_gain(ch);
      else
        ch->mana = ch->max_mana;
      if (ch->move < ch->max_move)
        ch->move += move_gain(ch);
      else
        ch->move = ch->max_move;
    }

    if (!IS_NPC(ch) && ch->position == POS_STUNNED)
      update_pos(ch);
    if (!IS_NPC(ch) && ch->level < LEVEL_IMMORTAL)
    {
      OBJ_DATA *obj;

      if ((obj = get_eq_char(ch, WEAR_LIGHT)) != NULL &&
          obj->item_type == ITEM_LIGHT && obj->value[2] > 0)
      {
        if (--obj->value[2] == 0 && ch->in_room != NULL)
        {
          --ch->in_room->light;
          act("$p goes out.", ch, obj, NULL, TO_ROOM);
          act("$p flickers and goes out.", ch, obj, NULL, TO_CHAR);
          extract_obj(obj);
          obj = NULL;
        }
        else if (obj->value[2] <= 5 && ch->in_room != NULL)
          act("$p flickers.", ch, obj, NULL, TO_CHAR);
      }

      countu = 0;
      cur_on = 0;
      for (d = descriptor_list; d; d = d->next)
      {
        if (d->connected == CON_PLAYING)
        countu++;
      }
      cur_on = countu;

      if (arena == FIGHT_BUSY)
      {
        if (arenacount >= 15)
        {
          do_aclear(ch, NULL);
          arenacount = 0;
          ch->challenge_timer = 15;
        }
        if (arenacount == 10)
        {
          send_to_char
            ("{W[{RARENA{W] 5 minutes until the arena is forcefully closed.{x\n\r",
             ch);
        }
        if (arenacount == 14)
        {
          send_to_char
            ("{W[{RARENA{W] 1 minute until the arena is forcefully closed.{x\n\r",
             ch);
        }
      }

      if (arena == FIGHT_START)
      {
        if (arenacount >= 3)
        {
          do_aclear(ch, NULL);
          arenacount = 0;
        }

      }

      if (!IS_NPC(ch))
      {
        if (arena == FIGHT_OPEN)
        {
          clean_char_flags(ch);
        }

        if (ch->reroll_timer >= 0 && ch->reroll_timer != -1 &&
            ch->level <= 25)
        {
          --ch->reroll_timer;
        }
        else
        {
          ch->reroll_timer = -1;
        }

        if (ch->pk_timer == 1)
        {
          --ch->pk_timer;
          send_to_char("Your PK timer has run out.\n\r", ch);
        }

        if (ch->pk_timer > 0)
        {
          --ch->pk_timer;
        }
        if (ch->rps > ch->rpst)
          ch->rpst = ch->rps;

        if (!IS_NPC(ch) && !IS_IMMORTAL(ch))
          if (ch->timer >= 15)
          {
            if (!IS_IMMORTAL(ch))
            {
              if (ch->was_in_room == NULL && ch->in_room != NULL)
              {

                ch->was_in_room = ch->in_room;
                if (ch->fighting != NULL)
                  stop_fighting(ch, true);
                act("$n disappears into the void.", ch, NULL, NULL, TO_ROOM);
                SET_BIT(ch->comm, COMM_AFK);
                send_to_char("You disappear into the void.\n\r", ch);
                char_from_room(ch);
                char_to_room(ch, get_room_index(ROOM_VNUM_LIMBO));
              }
            }
          }

        if (!IS_NPC(ch))
        {
          gain_condition(ch, COND_DRUNK, -1);
        }
      }
    }
    for (paf = ch->affected; paf != NULL; paf = paf_next)
    {
      paf_next = paf->next;
      if (paf->duration > 0)
      {
        paf->duration--;
        if (number_range(0, 4) == 0 && paf->level > 0)
          paf->level--;         /* spell strength fades with time */
      }
      else
      {
        if (paf_next == NULL || paf_next->type != paf->type ||
            paf_next->duration > 0)
        {
          if (paf->type > 0 && skill_table[paf->type].msg_off[0] != '\0')
          {
            send_to_char(skill_table[paf->type].msg_off, ch);
            send_to_char("\n\r", ch);
          }
        }

        affect_remove(ch, paf);
      }
    }

    /* 
     * Careful with the damages here,
     *   MUST NOT refer to ch after damage taken,
     *   as it may be lethal damage (on NPC).
     */

    if (is_affected(ch, gsn_plague) && ch != NULL)
    {
      AFFECT_DATA *af, plague;
      CHAR_DATA *vch;
      int dam;

      if (ch->in_room == NULL)
        return;
      act("$n writhes in agony as plague sores erupt from $s skin.", ch,
          NULL, NULL, TO_ROOM);
      send_to_char("You writhe in agony from the plague.\n\r", ch);
      for (af = ch->affected; af != NULL; af = af->next)
      {
        if (af->type == gsn_plague)
          break;
      }

      if (af == NULL)
      {
        REMOVE_BIT(ch->affected_by, AFF_PLAGUE);
        return;
      }

      if (af->level == 1)
        return;
      plague.where = TO_AFFECTS;
      plague.type = gsn_plague;
      plague.level = af->level - 1;
      plague.duration = number_range(1, 2 * plague.level);
      plague.location = APPLY_STR;
      plague.modifier = -5;
      plague.bitvector = AFF_PLAGUE;
      for (vch = ch->in_room->people; vch != NULL; vch = vch->next_in_room)
      {
        if (!saves_spell(plague.level - 2, vch, DAM_DISEASE) &&
            !IS_IMMORTAL(vch) && !IS_AFFECTED(vch, AFF_PLAGUE) &&
            number_bits(4) == 0)
        {
          send_to_char("You feel hot and feverish.\n\r", vch);
          act("$n shivers and looks very ill.", vch, NULL, NULL, TO_ROOM);
          affect_join(vch, &plague);
        }
      }

      dam = UMIN(ch->level, af->level / 5 + 1);
      ch->mana -= dam;
      ch->move -= dam;
      xdamage(ch, ch, dam, gsn_plague, DAM_DISEASE, false, VERBOSE_STD,
              &mobdeath);
    }
    else if (IS_AFFECTED(ch, AFF_POISON) && ch != NULL &&
             !IS_AFFECTED(ch, AFF_SLOW))
    {
      AFFECT_DATA *poison;

      poison = affect_find(ch->affected, gsn_poison);
      if (poison != NULL)
      {
        act("$n shivers and suffers.", ch, NULL, NULL, TO_ROOM);
        send_to_char("You shiver and suffer.\n\r", ch);
        xdamage(ch, ch, poison->level / 10 + 1, gsn_poison,
                DAM_POISON, false, VERBOSE_STD, &mobdeath);
      }
    }

    else if (ch->position == POS_INCAP && number_range(0, 1) == 0)
    {
      xdamage(ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, false, VERBOSE_STD,
              &mobdeath);
    }
    else if (ch->position == POS_MORTAL)
    {
      xdamage(ch, ch, 1, TYPE_UNDEFINED, DAM_NONE, false, VERBOSE_STD,
              &mobdeath);
    }

    mobdeath = false;
  }

  return;
}

/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update(void)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  for (obj = object_list; obj != NULL; obj = obj_next)
  {
    CHAR_DATA *rch;
    char *message;

    obj_next = obj->next;

    if (obj->timer <= 0 || --obj->timer > 0)
      continue;
    switch (obj->item_type)
    {
      default:
        message = "$p crumbles into dust.";
        break;
      case ITEM_FOUNTAIN:
        message = "$p dries up.";
        break;
      case ITEM_CORPSE_NPC:
        message = "$p sinks from view into the gore.";
        break;
      case ITEM_CORPSE_PC:
        message = "$p sinks from view into the gore.";
        break;
      case ITEM_FOOD:
        message = "$p decomposes.";
        break;
      case ITEM_POTION:
        message = "$p has evaporated from disuse.";
        break;
      case ITEM_PORTAL:
        message = "$p fades out of existence.";
        break;
      case ITEM_CONTAINER:
      case ITEM_PIT:
        if (CAN_WEAR(obj, ITEM_WEAR_FLOAT))
          if (obj->contains)
            message =
              "$p flickers and vanishes, spilling its contents on the floor.";
          else
            message = "$p flickers and vanishes.";
        else
          message = "$p crumbles into dust.";
        break;
    }

    if (obj->carried_by != NULL)
    {
      if (IS_NPC(obj->carried_by) &&
          obj->carried_by->pIndexData->pShop != NULL)
        obj->carried_by->silver += obj->cost / 5;
      else
      {
        act(message, obj->carried_by, obj, NULL, TO_CHAR);
        if (obj->wear_loc == WEAR_FLOAT)
          act(message, obj->carried_by, obj, NULL, TO_ROOM);
      }
    }
    else if (obj->in_room != NULL && (rch = obj->in_room->people) != NULL)
    {
      if (!
          (obj->in_obj && obj->in_obj->pIndexData->item_type == ITEM_PIT
           && !CAN_WEAR(obj->in_obj, ITEM_TAKE)))
      {
        act(message, rch, obj, NULL, TO_ROOM);
        act(message, rch, obj, NULL, TO_CHAR);
      }
    }

    if ((obj->item_type == ITEM_CORPSE_PC || obj->wear_loc == WEAR_FLOAT)
        && obj->contains)
    {                           /* save the contents */
      OBJ_DATA *t_obj, *next_obj;

      for (t_obj = obj->contains; t_obj != NULL; t_obj = next_obj)
      {
        next_obj = t_obj->next_content;
        obj_from_obj(t_obj);
        if (obj->in_obj)        /* in another object */
          obj_to_obj(t_obj, obj->in_obj);
        else if (obj->carried_by) /* carried */
          if (obj->wear_loc == WEAR_FLOAT)
            if (obj->carried_by->in_room == NULL)
            {
              extract_obj(t_obj);
              t_obj = NULL;
            }
            else
              obj_to_room(t_obj, obj->carried_by->in_room);
          else
            obj_to_char(t_obj, obj->carried_by);
        else if (obj->in_room == NULL)  /* destroy it */
        {
          extract_obj(t_obj);
          t_obj = NULL;
        }
        else                    /* to a room */
          obj_to_room(t_obj, obj->in_room);
      }
    }

    extract_obj(obj);
    obj = NULL;
  }

  return;
}

/*
 * Aggress.
 *
 * for each mortal PC
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function takes 25% to 35% of ALL Merc cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 * -- Furey
 */
void aggr_update(void)
{
  CHAR_DATA *wch;
  CHAR_DATA *wch_next;
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  CHAR_DATA *vch;
  CHAR_DATA *vch_next;
  CHAR_DATA *victim;
  bool mobdeath = false;

  for (wch = char_list; wch != NULL; wch = wch_next)
  {
    wch_next = wch->next;
    if (IS_NPC(wch) || wch->level >= LEVEL_IMMORTAL ||
        wch->in_room == NULL || wch->in_room->area->empty)
      continue;
    for (ch = wch->in_room->people; ch != NULL; ch = ch_next)
    {
      int count;

      ch_next = ch->next_in_room;
      if (!IS_NPC(ch) || !IS_SET(ch->act, ACT_AGGRESSIVE) ||
          IS_SET(ch->in_room->room_flags, ROOM_SAFE) ||
          IS_AFFECTED(ch, AFF_CALM) || ch->fighting != NULL ||
          IS_AFFECTED(ch, AFF_CHARM) || !IS_AWAKE(ch) ||
          (IS_SET(ch->act, ACT_WIMPY) && IS_AWAKE(wch)) ||
          !can_see(ch, wch) || number_bits(1) == 0)
        continue;
      /* 
       * Ok we have a 'wch' player character and a 'ch' npc aggressor.
       * Now make the aggressor fight a RANDOM pc victim in the room,
       *   giving each 'vch' an equal chance of selection.
       */
      count = 0;
      victim = NULL;
      for (vch = wch->in_room->people; vch != NULL; vch = vch_next)
      {
        vch_next = vch->next_in_room;
        if (!IS_NPC(vch) && vch->level < LEVEL_IMMORTAL &&
            ch->level >= vch->level - 5 &&
            (!IS_SET(ch->act, ACT_WIMPY) || !IS_AWAKE(vch)) &&
            can_see(ch, vch))
        {
          if (number_range(0, count) == 0)
            victim = vch;
          count++;
        }
      }

      if (victim == NULL)
        continue;
      multi_hit(ch, victim, TYPE_UNDEFINED, &mobdeath);
    }
  }

  return;
}

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */

void update_handler(bool forced)
{
  static int pulse_area;
  static int pulse_mobile;
  static int pulse_violence;
  static int pulse_point;
  static int pulse_music;
  static int pulse_auction;

  // static int pulse_tele;
  static int pulse_quest;

  uptime = (current_time - boot_time);

  if (--pulse_area <= 0 || forced)
  {
    pulse_area = PULSE_AREA;
    quest_update();
    area_update();
  }
  /*
     if ( --pulse_tele <= 0 || forced )
     {
     pulse_tele = PULSE_TELEPORT;
     tele_update  ( );
     }    
   */
  if (--pulse_music <= 0 || forced)
  {
    pulse_music = PULSE_MUSIC;
    song_update();
  }

  if (--pulse_quest <= 0 || forced)
  {
    pulse_quest = PULSE_QUEST;
    quest_update();
  }

  if (--pulse_mobile <= 0 || forced)
  {
    pulse_mobile = PULSE_MOBILE;
    mobile_update();
  }

  if (--pulse_violence <= 0 || forced)
  {
    pulse_violence = PULSE_VIOLENCE;
    violence_update();
  }

  if (--pulse_auction <= 0 || forced)
  {
    pulse_auction = PULSE_AUCTION;
    update_auc();
  }

  if (--pulse_point <= 0 || forced)
  {
    wiznet("TICK!", NULL, NULL, WIZ_TICKS, 0, 0);
    pulse_point = PULSE_TICK;
    weather_update();
    char_update();
    obj_update();
    bank_update();
    randomize_entrances(ROOM_VNUM_CHAIN);
  }

  aggr_update();
  tail_chain();
  return;
}

/*

   void tele_update ( void ) 
   {
   CHAR_DATA  *ch;
   CHAR_DATA  *ch_next;
   ROOM_INDEX_DATA *pRoomIndex;


   for (ch = char_list ; ch != NULL; ch = ch_next )
   {
   ch_next = ch->next;
   if ( IS_SET(ch->in_room->room_flags, ROOM_TELEPORT ) )
   {

   if (IS_IMMORTAL(ch) && IS_SET(ch->act,PLR_NOSUMMON))
   {
   return;
   }
   do_look ( ch, "tele" );
   if ( ch->in_room->tele_dest == 0 )
   pRoomIndex = get_random_room (ch);
   else
   pRoomIndex = get_room_index(ch->in_room->tele_dest);

   send_to_char ("You have been teleported!!!\n\r", ch);
   act("$n vanishes!!!\n\r", ch, NULL, NULL, TO_ROOM);
   char_from_room(ch);
   char_to_room(ch, pRoomIndex);
   act("$n slowly fades into existence.\n\r", ch, NULL, NULL,TO_ROOM);
   do_look(ch, "auto");
   }
   }
   } */

void olcautosave(void)
{
  AREA_DATA *pArea;
  DESCRIPTOR_DATA *d;
  char buf[MAX_INPUT_LENGTH];

  /*  if you're still adding areas manually, you may need to remove the next line - Chance */
  save_area_list();
  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->editor)
      send_to_char("OLC Autosaving:\n\r", d->character);
  }
  sprintf(buf, "None.\n\r");
  for (pArea = area_first; pArea; pArea = pArea->next)
  {
    /* Save changed areas. */
    if (IS_SET(pArea->area_flags, AREA_CHANGED))
    {
      save_area(pArea);
      sprintf(buf, "%24s - '%s'", pArea->name, pArea->file_name);
      for (d = descriptor_list; d != NULL; d = d->next)
      {
        if (d->editor)
        {
          send_to_char(buf, d->character);
          send_to_char("\n\r", d->character);
        }
      }
      REMOVE_BIT(pArea->area_flags, AREA_CHANGED);
    }
  }
  if (!str_cmp(buf, "None.\n\r"))
  {
    for (d = descriptor_list; d != NULL; d = d->next)
    {
      if (d->editor)
        send_to_char(buf, d->character);
    }
  }
  return;
}
