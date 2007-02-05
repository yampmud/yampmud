
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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "recycle.h"
#include "lookup.h"

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 */

void kill_swalk(CHAR_DATA * ch)
{
  if (IS_SHIELDED(ch, SHD_SWALK))
  {
    send_to_char
      ("\n\rYour actions have revealed your location!\n\rYou no longer walk in the shadows.\n\r",
       ch);
    affect_strip(ch, gsn_swalk);
  }
  if (IS_SHIELDED(ch, SHD_VANISH))
  {
    send_to_char("You fade into view.\n\r", ch);
    affect_strip(ch, gsn_vanish);
  }
  return;
}

bool nia(CHAR_DATA * ch)
{
  if (IS_SET(ch->in_room->room_flags, ROOM_ARENA))
  {
    send_to_char("Not in the arena.\n\r", ch);
    return true;
  }
  else
  {
    return false;
  }
}

bool WR(CHAR_DATA * ch, CHAR_DATA * victim)
{

  int ch_tier;
  int victim_tier;

  ch_tier = 0;
  victim_tier = 0;

  if (IS_NPC(victim))
    return true;

  if (IS_NPC(ch))
    return true;

  if (IS_SET(ch->in_room->room_flags, ROOM_ARENA) &&
      IS_SET(victim->in_room->room_flags, ROOM_ARENA) &&
      !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
    return true;

  if (IS_IMMORTAL(ch))
    return true;

  if (IS_IMMORTAL(victim))
    return true;

  if (!victim->desc)
    return false;

  if (!ch->desc)
    return false;

  if (IS_SET(victim->comm, COMM_AFK))
    return false;

  if (IS_SET(ch->comm, COMM_AFK))
    return false;

  if (ch->pk_timer > 0 || victim->pk_timer > 0)
    return false;

  if (IS_SET(class_table[ch->class].tier, TIER_01))
    ch_tier = 1;

  if (IS_SET(class_table[ch->class].tier, TIER_02))
    ch_tier = 2;

  if (IS_SET(class_table[ch->class].tier, TIER_03))
    ch_tier = 3;

  if (IS_SET(class_table[ch->class].tier, TIER_04))
    ch_tier = 4;

  if (IS_SET(class_table[victim->class].tier, TIER_01))
    victim_tier = 1;

  if (IS_SET(class_table[victim->class].tier, TIER_02))
    victim_tier = 2;

  if (IS_SET(class_table[victim->class].tier, TIER_03))
    victim_tier = 3;

  if (IS_SET(class_table[victim->class].tier, TIER_04))
    victim_tier = 4;

  if ((!IS_NPC(ch) && !IS_NPC(victim)) &&
      (is_pkill(ch) && is_pkill(victim)) &&
      (ch_tier == victim_tier) &&
      (abs(ch->level - victim->level) <= 5) && (!IS_IMMORTAL(ch) &&
                                                !IS_IMMORTAL
                                                (victim)) &&
      (ch->name != victim->name))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void violence_update(void)
{
  char buf[MSL];
  CHAR_DATA *ch;
  CHAR_DATA *ch_next;
  CHAR_DATA *victim;
  int dam = 0;
  int vdam = 0;
  bool mobdeath = false;

  for (ch = char_list; ch != NULL; ch = ch_next)
  {
    ch_next = ch->next;
    mobdeath = false;

    if ((victim = ch->fighting) == NULL || ch->in_room == NULL)
      continue;

    if (IS_AWAKE(ch) && ch->in_room == victim->in_room && WR(ch, victim))
    {
      kill_swalk(ch);
      multi_hit(ch, victim, TYPE_UNDEFINED, &mobdeath);
      if (mobdeath)
        continue;
      process_shields(ch, victim, &mobdeath);
      if (mobdeath)
        continue;
    }
    else
      stop_fighting(ch, false);

    if ((victim = ch->fighting) == NULL)
      continue;

    /* 
     * Fun for the whole family!
     */
    if (mobdeath)
      continue;
    check_assist(ch, victim, &mobdeath);
    if (mobdeath)
      continue;

    if (IS_NPC(ch))
    {
      if (HAS_TRIGGER(ch, TRIG_FIGHT))
        mp_percent_trigger(ch, victim, NULL, NULL, TRIG_FIGHT);
      if (HAS_TRIGGER(ch, TRIG_HPCNT))
        mp_hprct_trigger(ch, victim);
    }
    dam = ((ch->old_hit) - (ch->hit));
    vdam = ((victim->old_hit) - (victim->hit));

    if (dam > 0)
    {
      if (!IS_NPC(ch))
        if (IS_SET(ch->pcdata->verbose, VERBOSE_DAMAGE))
        {
          sprintf(buf,
                  "{RYou received {C%d{R damage last round from {C%s{R.{x\n\r",
                  dam, victim->name);
          send_to_char(buf, ch);

          sprintf(buf,
                  "{C%s{R recived {C%d{R damage from {C%s{Y last round.{x",
                  victim->name, vdam, ch->name);
          if (vdam > 0)
            act(buf, ch, NULL, NULL, TO_NOTVICT);

          sprintf(buf,
                  "{C%s{Y delt {C%d{Y damage to {C%s{Y last round.{x",
                  ch->name, dam, victim->name);
          if (dam > 0)
            act(buf, ch, NULL, NULL, TO_NOTVICT);

        }
      if (!IS_NPC(victim))
        if (IS_SET(victim->pcdata->verbose, VERBOSE_DAMAGE))
        {
          sprintf(buf,
                  "{YYou delt {C%d{Y damage last round to {C%s{Y.{x\n\r",
                  dam, ch->name);
          send_to_char(buf, victim);
        }
    }
    ch->old_hit = ch->hit;
    ch->old_mana = ch->mana;
  }

  return;
}

/* for auto assisting */
void check_assist(CHAR_DATA * ch, CHAR_DATA * victim, bool * mobdeath)
{
  CHAR_DATA *rch, *rch_next;

  for (rch = ch->in_room->people; rch != NULL; rch = rch_next)
  {
    rch_next = rch->next_in_room;

    if (IS_AWAKE(rch) && rch->fighting == NULL && can_see(rch, victim))
    {

      /* quick check for ASSIST_PLAYER */
      if (!IS_NPC(ch) && IS_NPC(rch) &&
          IS_SET(rch->off_flags, ASSIST_PLAYERS) &&
          rch->level + 6 > victim->level)
      {
        do_emote(rch, "{Rscreams and attacks!{x");
        multi_hit(rch, victim, TYPE_UNDEFINED, mobdeath);
        continue;
      }

      /* PCs next */
      if (!IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM))
      {
        if (((!IS_NPC(rch) &&
              IS_SET(rch->act, PLR_AUTOASSIST)) ||
             IS_AFFECTED(rch, AFF_CHARM)) &&
            is_same_group(ch, rch) && !is_safe(rch, victim))
          multi_hit(rch, victim, TYPE_UNDEFINED, mobdeath);

        continue;
      }

      /* now check the NPC cases */

      if (IS_NPC(ch) && !IS_AFFECTED(ch, AFF_CHARM))

      {
        if ((IS_NPC(rch) && IS_SET(rch->off_flags, ASSIST_ALL))
            || (IS_NPC(rch) && rch->group &&
                rch->group == ch->group) || (IS_NPC(rch) &&
                                             rch->race == ch->race
                                             && IS_SET(rch->
                                                       off_flags,
                                                       ASSIST_RACE))
            || (IS_NPC(rch) &&
                IS_SET(rch->off_flags, ASSIST_ALIGN) &&
                ((IS_GOOD(rch) && IS_GOOD(ch)) ||
                 (IS_EVIL(rch) && IS_EVIL(ch)) ||
                 (IS_NEUTRAL(rch) && IS_NEUTRAL(ch)))) ||
            (rch->pIndexData == ch->pIndexData &&
             IS_SET(rch->off_flags, ASSIST_VNUM)))

        {
          CHAR_DATA *vch;
          CHAR_DATA *target;
          int number;

          if (number_bits(1) == 0)
            continue;

          target = NULL;
          number = 0;
          for (vch = ch->in_room->people; vch; vch = vch->next)
          {
            if (can_see(rch, vch) &&
                is_same_group(vch, victim) && number_range(0, number) == 0)
            {
              target = vch;
              number++;
            }
          }

          if (target != NULL)
          {
            do_emote(rch, "{Rscreams and attacks!{x");
            multi_hit(rch, target, TYPE_UNDEFINED, mobdeath);
            if (*mobdeath)
              continue;
          }
        }
      }
    }
  }
}

/*
 * Do one group of attacks.
 */
void multi_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool * mobdeath)
{
  int chance, amount;
  char buf[MIL];

  /* decrement the wait */
  if (ch->desc == NULL)
    ch->wait = UMAX(0, ch->wait - PULSE_VIOLENCE);

  if (ch->desc == NULL)
    ch->daze = UMAX(0, ch->daze - PULSE_VIOLENCE);

  /* no attacks for stunnies -- just a check */
  if (ch->position < POS_RESTING)
    return;

  if (ch->stunned)
  {
    if (number_percent() <= (((get_curr_stat(ch, STAT_CON)) -
                              (ch->stunned)) * 2))
    {
      ch->stunned--;
      if (!ch->stunned)
      {
        send_to_char("You regain your equilibrium.\n\r", ch);
        act("$n regains $m equilibrium.", ch, NULL, NULL, TO_ROOM);
        return;
      }
    }
    ch->stunned--;
    if (!ch->stunned)
    {
      send_to_char("You regain your equilibrium.\n\r", ch);
      act("$n regains $m equilibrium.", ch, NULL, NULL, TO_ROOM);
      return;
    }
    else
    {
      send_to_char("{RYou are stunned.{x\n\r", ch);
    }
    return;
  }

  if (IS_AFFECTED(ch, AFF_TERROR))
  {
    send_to_char("You try to flee in {Dterror{x.\n\r", ch);
    act("$n tries to flee in {Dterror{x.", ch, NULL, NULL, TO_ROOM);
    do_flee(ch, "");
  }

  if (IS_SHIELDED(ch, SHD_LASTRITES) && (ch->hit <= ch->max_hit * .16))
  {
    affect_strip(ch, gsn_last_rites);
    REMOVE_BIT(ch->shielded_by, SHD_LASTRITES);
    if (number_percent() + (get_curr_stat(ch, STAT_WIS) / 4) > 45)
    {
      amount = ch->mana / dice(2, 4);
      ch->mana -= amount;
      ch->hit += amount;
      sprintf(buf, "{rYou invoke your {Dlast rites{r! {D({R%d{D){x\n\r",
              amount);
      act(buf, ch, NULL, NULL, TO_CHAR);
      act("$n invokes $s last rites.", ch, NULL, NULL, TO_ROOM);
    }
    else
      send_to_char("You fail to recite your last rites.\n\r", ch);
  }

  if (IS_NPC(ch))
  {
    mob_hit(ch, victim, dt, mobdeath);
    return;
  }

  one_hit(ch, victim, dt, false, mobdeath);
  if (*mobdeath)
    return;

  if (get_eq_char(ch, WEAR_SECONDARY))
  {
    chance = (get_skill(ch, gsn_dual_wield) / 3) * 2;
    chance += 33;
    if (number_percent() < chance)
    {
      one_hit(ch, victim, dt, true, mobdeath);
      if (get_skill(ch, gsn_dual_wield) != 0 &&
          (!IS_NPC(ch) &&
           ch->level >= skill_table[gsn_dual_wield].skill_level[ch->class]))
      {
        check_improve(ch, gsn_dual_wield, true, 1);
      }
      if (*mobdeath)
        return;
    }
    if (ch->fighting != victim)
      return;
  }

  if (ch->fighting != victim)
    return;

  if (dt == gsn_strike)
    return;

  if (IS_AFFECTED(ch, AFF_HASTE))
  {
    one_hit(ch, victim, dt, false, mobdeath);
    if (*mobdeath)
      return;
  }

  if (ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle ||
      dt == gsn_whirlwind || dt == gsn_strike || dt == gsn_assassinate)
    return;

  chance = get_skill(ch, gsn_second_attack) / 2;

  if (IS_AFFECTED(ch, AFF_SLOW))
    chance /= 2;

  if (number_percent() < chance)
  {
    one_hit(ch, victim, dt, false, mobdeath);
    check_improve(ch, gsn_second_attack, true, 5);
    if (*mobdeath || ch->fighting != victim)
      return;
  }
  else
  {
    return;
  }

  chance = get_skill(ch, gsn_third_attack) / 2;

  if (IS_AFFECTED(ch, AFF_SLOW))
    chance /= 2;

  if (number_percent() < chance)
  {
    one_hit(ch, victim, dt, false, mobdeath);
    check_improve(ch, gsn_third_attack, true, 6);
    if (!mobdeath || ch->fighting != victim)
      return;
  }
  else
  {
    return;
  }

  chance = get_skill(ch, gsn_fourth_attack) / 2;

  if (IS_AFFECTED(ch, AFF_SLOW))
    chance /= 3;

  if (number_percent() < chance)
  {
    one_hit(ch, victim, dt, false, mobdeath);
    check_improve(ch, gsn_fourth_attack, true, 6);
    if (*mobdeath || ch->fighting != victim)
      return;
  }
  else
  {
    return;
  }

  chance = get_skill(ch, gsn_fifth_attack) / 2;

  if (IS_AFFECTED(ch, AFF_SLOW))
    chance = 0;

  if (number_percent() < chance)
  {
    one_hit(ch, victim, dt, false, mobdeath);
    check_improve(ch, gsn_fifth_attack, true, 6);
    if (*mobdeath || ch->fighting != victim)
      return;
  }

  chance = get_skill(ch, gsn_sixth_attack) / 2;

  if (number_percent() < chance)
  {
    one_hit(ch, victim, dt, false, mobdeath);
    check_improve(ch, gsn_sixth_attack, true, 6);
    if (*mobdeath || ch->fighting != victim)
      return;
  }
  chance = get_skill(ch, gsn_seventh_attack) / 2;

  if (number_percent() < chance)
  {
    one_hit(ch, victim, dt, false, mobdeath);
    check_improve(ch, gsn_seventh_attack, true, 6);
    if (*mobdeath || ch->fighting != victim)
      return;
  }
  if (dt == gsn_ambush)
    return;
  return;
}

/* procedure for all mobile attacks */
void mob_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool * mobdeath)
{
  int chance, number;
  CHAR_DATA *vch, *vch_next;

  one_hit(ch, victim, dt, false, mobdeath);

  if (*mobdeath || ch->fighting != victim)
    return;

  if (ch->stunned)
    return;

  /* Area attack -- BALLS nasty! */

  if (IS_SET(ch->off_flags, OFF_AREA_ATTACK))
  {
    for (vch = ch->in_room->people; vch != NULL; vch = vch_next)
    {
      vch_next = vch->next;
      if ((vch != victim && vch->fighting == ch))
        one_hit(ch, vch, dt, false, mobdeath);
    }
  }

  if (*mobdeath || ch->fighting != victim)
    return;

  if (get_eq_char(ch, WEAR_SECONDARY))
  {
    chance = (get_skill(ch, gsn_dual_wield) / 3) * 2;
    chance += 33;
    if (number_percent() < chance)
    {
      one_hit(ch, victim, dt, true, mobdeath);
    }
    if (*mobdeath || ch->fighting != victim)
      return;
  }

  if (IS_AFFECTED(ch, AFF_HASTE) ||
      (IS_SET(ch->off_flags, OFF_FAST) && !IS_AFFECTED(ch, AFF_SLOW)))
  {
    one_hit(ch, victim, dt, false, mobdeath);
    if (*mobdeath)
      return;
  }

  if (ch->fighting != victim || dt == gsn_backstab || dt == gsn_circle ||
      dt == gsn_whirlwind || dt == gsn_strike || dt == gsn_assassinate)
    return;

  chance = get_skill(ch, gsn_second_attack) / 2;

  if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags, OFF_FAST))
    chance /= 2;

  if (number_percent() < chance)
  {
    one_hit(ch, victim, dt, false, mobdeath);
    if (*mobdeath || ch->fighting != victim)
      return;
    chance = get_skill(ch, gsn_third_attack) / 2;

    if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags, OFF_FAST))
      chance /= 2;

    if (number_percent() < chance)
    {
      one_hit(ch, victim, dt, false, mobdeath);
      if (*mobdeath || ch->fighting != victim)
        return;

      chance = get_skill(ch, gsn_fourth_attack) / 2;

      if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags, OFF_FAST))
        chance /= 3;

      if (number_percent() < chance)
      {
        one_hit(ch, victim, dt, false, mobdeath);
        if (*mobdeath || ch->fighting != victim)
          return;

        chance = get_skill(ch, gsn_fifth_attack) / 2;

        if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags, OFF_FAST))
          chance = 0;

        if (number_percent() < chance)
        {
          one_hit(ch, victim, dt, false, mobdeath);
          if (*mobdeath || ch->fighting != victim)
            return;
        }

        chance = get_skill(ch, gsn_sixth_attack) / 2;

        if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags, OFF_FAST))
          chance = 0;

        if (number_percent() < chance)
        {
          one_hit(ch, victim, dt, false, mobdeath);
          if (*mobdeath || ch->fighting != victim)
            return;
        }

        chance = get_skill(ch, gsn_seventh_attack) / 2;

        if (IS_AFFECTED(ch, AFF_SLOW) && !IS_SET(ch->off_flags, OFF_FAST))
          chance = 0;

        if (number_percent() < chance)
        {
          one_hit(ch, victim, dt, false, mobdeath);
          if (*mobdeath || ch->fighting != victim)
            return;
        }
      }
    }
  }

  /* oh boy! Fun stuff! */

  if (ch->wait > 0)
    return;

  number = number_range(0, 2);

  if (number == 1 && IS_SET(ch->act, ACT_MAGE))
  {
    /* { mob_cast_mage(ch,victim); return; } */ ;
  }

  if (number == 2 && IS_SET(ch->act, ACT_CLERIC))
  {
    /* { mob_cast_cleric(ch,victim); return; } */ ;
  }

  /* now for the skills */

  number = number_range(0, 8);

  switch (number)
  {
    case (0):
      if (IS_SET(ch->off_flags, OFF_BASH))
        do_bash(ch, "");
      break;

    case (1):
      if (IS_SET(ch->off_flags, OFF_BERSERK) && !IS_AFFECTED(ch, AFF_BERSERK))
        do_berserk(ch, "");
      break;

    case (2):
      if (IS_SET(ch->off_flags, OFF_DISARM) ||
          (get_weapon_sn(ch) != gsn_hand_to_hand &&
           (IS_SET(ch->act, ACT_WARRIOR) ||
            IS_SET(ch->act, ACT_VAMPIRE) || IS_SET(ch->act, ACT_THIEF))))
        do_disarm(ch, "");
      break;

    case (3):
      if (IS_SET(ch->off_flags, OFF_KICK))
        do_kick(ch, "");
      break;

    case (4):
      if (IS_SET(ch->off_flags, OFF_KICK_DIRT))
        do_dirt(ch, "");
      break;

    case (5):
      if (IS_SET(ch->off_flags, OFF_TAIL))
      {
        /* do_tail(ch,"") */ ;
      }
      break;

    case (6):
      if (IS_SET(ch->off_flags, OFF_TRIP))
        do_trip(ch, "");
      break;

    case (7):
      if (IS_SET(ch->off_flags, OFF_CRUSH))
      {
        /* do_crush(ch,"") */ ;
      }
      break;
    case (8):
      if (IS_SET(ch->off_flags, OFF_BACKSTAB))
      {
        do_backstab(ch, "");
      }
  }
}

/*
 * Hit one guy once.
 */
void one_hit(CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary,
             bool * mobdeath)
{
  OBJ_DATA *wield;
  int victim_ac;
  int thac0;
  int thac0_00;
  int thac0_32;
  int dam;
  int diceroll;
  int sn, skill;
  int dam_type;
  bool result;

  sn = -1;

  /* just in case */
  if (victim == ch || ch == NULL || victim == NULL)
    return;

  /* 
   * Can't beat a dead char!
   * Guard against weird room-leavings.
   */
  if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
    return;

  /* 
   * Figure out the type of damage message.
   * if secondary == true, use the second weapon.
   */
  if (!secondary)
    wield = get_eq_char(ch, WEAR_WIELD);
  else
    wield = get_eq_char(ch, WEAR_SECONDARY);
  if (dt == TYPE_UNDEFINED)
  {
    dt = TYPE_HIT;
    if (wield != NULL && wield->item_type == ITEM_WEAPON)
      dt += wield->value[3];
    else
      dt += ch->dam_type;
  }

  if (dt < TYPE_HIT)
    if (wield != NULL)
      dam_type = attack_table[wield->value[3]].damage;
    else
      dam_type = attack_table[ch->dam_type].damage;
  else
    dam_type = attack_table[dt - TYPE_HIT].damage;

  if (dam_type == -1)
    dam_type = DAM_BASH;

  /* get the weapon skill */
  sn = get_weapon_sn(ch);
  skill = 20 + get_weapon_skill(ch, sn);

  /* 
   * Calculate to-hit-armor-class-0 versus armor.
   */
  if (IS_NPC(ch))
  {
    thac0_00 = 20;
    thac0_32 = -4;              /* as good as a thief */

    if (IS_SET(ch->act, ACT_VAMPIRE))
      thac0_32 = -30;
    else if (IS_SET(ch->act, ACT_DRUID))
      thac0_32 = 0;
    else if (IS_SET(ch->act, ACT_RANGER))
      thac0_32 = -4;

    else if (IS_SET(ch->act, ACT_WARRIOR))
      thac0_32 = -10;
    else if (IS_SET(ch->act, ACT_THIEF))
      thac0_32 = -4;

    else if (IS_SET(ch->act, ACT_CLERIC))
      thac0_32 = 2;
    else if (IS_SET(ch->act, ACT_MAGE))
      thac0_32 = 6;
  }
  else
  {
    thac0_00 = class_table[ch->class].thac0_00;
    thac0_32 = class_table[ch->class].thac0_32;
  }
  thac0 = interpolate(ch->level, thac0_00, thac0_32);

  if (thac0 < 0)
    thac0 = thac0 / 2;

  if (thac0 < -5)
    thac0 = -5 + (thac0 + 5) / 2;

  thac0 -= GET_HITROLL(ch) * skill / 100;
  thac0 += 5 * (100 - skill) / 100;

  if (dt == gsn_backstab)
    thac0 -= 10 * (100 - get_skill(ch, gsn_backstab));

  if (dt == gsn_circle)
    thac0 -= 10 * (100 - get_skill(ch, gsn_circle));

  if (dt == gsn_whirlwind)
    thac0 -= 10 * (100 - get_skill(ch, gsn_whirlwind));

  if (dt == gsn_strike)
    thac0 -= 10 * (100 - get_skill(ch, gsn_strike));

  if (dt == gsn_ambush)
    thac0 -= 10 * (100 - get_skill(ch, gsn_ambush));
  if (dt == gsn_assassinate)
    thac0 -= 10 * (100 - get_skill(ch, gsn_assassinate));

  switch (dam_type)
  {
    case (DAM_PIERCE):
      victim_ac = GET_AC(victim, AC_PIERCE) / 10;
      break;
    case (DAM_BASH):
      victim_ac = GET_AC(victim, AC_BASH) / 10;
      break;
    case (DAM_SLASH):
      victim_ac = GET_AC(victim, AC_SLASH) / 10;
      break;
    default:
      victim_ac = GET_AC(victim, AC_EXOTIC) / 10;
      break;
  };

  if (victim_ac < -15)
    victim_ac = (victim_ac + 15) / 5 - 15;

  if (!can_see(ch, victim))
    victim_ac -= 4;

  if (victim->position < POS_FIGHTING)
    victim_ac += 4;

  if (victim->position < POS_RESTING)
    victim_ac += 6;

  /* 
   * The moment of excitement!
   */
  while ((diceroll = number_bits(5)) >= 20)
    ;

  if (diceroll == 0 || (diceroll != 19 && diceroll < thac0 - victim_ac))
  {
    /* Miss. */
    xdamage(ch, victim, 0, dt, dam_type, true, VERBOSE_STD, mobdeath);
    tail_chain();
    return;
  }

  /* 
   * Hit.
   * Calc damage.
   */
  if (IS_NPC(ch) && (wield == NULL))
    dam = dice(ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE]);

  else
  {
    if (sn != -1)
      check_improve(ch, sn, true, 5);
    if (wield != NULL)
    {
      if (wield->clan)
      {
        dam = dice(ch->level / 3, 3) * skill / 100;
      }
      else
      {
        dam = dice(wield->value[1], wield->value[2]) * skill / 100;
      }

      if (get_eq_char(ch, WEAR_SHIELD) == NULL) /* no shield = more 
                                                 */
        dam = dam * 11 / 10;

      /* sharpness! */
      if (IS_WEAPON_STAT(wield, WEAPON_SHARP))
      {
        int percent;

        if ((percent = number_percent()) <= (skill / 8))
          dam = 2 * dam + (dam * 2 * percent / 100);
      }
      if (IS_WEAPON_STAT(wield, WEAPON_VORPAL))
      {
        int percent;

        if ((percent = number_percent()) <= (skill / 8))
          dam = 3 * dam + (dam * 3 * percent / 105);
      }
    }

    else
      dam =
        number_range(1 + 4 * skill / 100, 2 * ch->level / 3 * skill / 100);
  }

  /* 
   * Bonuses.
   */
  if (get_skill(ch, gsn_enhanced_damage) > 0)
  {
    diceroll = number_percent();
    if (diceroll <= get_skill(ch, gsn_enhanced_damage))
    {
      check_improve(ch, gsn_enhanced_damage, true, 6);
      dam += 2 * (dam * diceroll / 300);
    }
  }

  if (dt == gsn_backstab && wield != NULL)
  {
    if (wield->value[0] != 2)
      dam *= 2 + (ch->level / 14);
    else
      dam *= 2 + (ch->level / 12);
  }
  if (dt == gsn_circle && wield != NULL)
  {
    if (wield->value[0] != 2)
      dam *= 5 + (ch->level / 16);
    else
      dam *= 5 + (ch->level / 14);
    if (dt == gsn_whirlwind && wield != NULL)
    {
      if (wield->value[0] != 2)
        dam *= 2 + (ch->level / 10);
      else
        dam *= 2 + (ch->level / 8);
    }
  }

  if (dt == gsn_assassinate && wield != NULL)
  {
    if (IS_SHIELDED(ch, SHD_SWALK))
    {
      send_to_char
        ("The essence of surprise strengthens your attack!\n\r", ch);
      dam *= 2;
    }
    if (chance(20))
    {
      act("Someone nearly ends $n's miserable life!", victim, NULL,
          NULL, TO_ROOM);
      act("$n barely misses piercing your vital organs!", ch, NULL,
          victim, TO_VICT);
      printf_to_char(ch,
                     "{xYour assassination attempt barely misses its mark{x!!\n\r");
      dam *= 2 + (ch->level / 12);
    }
    else
      dam *= 2 + (ch->level / 10);
  }

  kill_swalk(ch);

  if (dt == gsn_strike && wield != NULL)
    dam *= (ch->level / 33);    /* max * 3 */

  if (dt == gsn_ambush && wield != NULL)
    dam *= 2 + (ch->level / 50);  /* max * 4 */

  dam += GET_DAMROLL(ch) * UMIN(100, skill) / 100;

  if (dam <= 0)
    dam = 1;

  /*    result = damage( ch, victim, dam, dt, dam_type, true ); */

  if (!check_counter(ch, victim, dam, dt, mobdeath))
  {
    if (*mobdeath)
      return;
    result =
      xdamage(ch, victim, dam, dt, dam_type, true, VERBOSE_STD, mobdeath);
    if (*mobdeath)
      return;
  }
  else
    return;

  /* but do we have a funky weapon? */
  if (result && wield != NULL)
  {
    int dam;

    if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_POISON))
    {
      int level;
      AFFECT_DATA *poison, af;

      if ((poison = affect_find(wield->affected, gsn_poison)) == NULL)
        level = wield->level;
      else
        level = poison->level;

      if (!saves_spell(level / 2, victim, DAM_POISON))
      {
        send_to_char
          ("{cYou feel {ypoison{c coursing through your veins.{x", victim);
        act("$n is {ypoisoned{x by the venom on $p.", victim, wield,
            NULL, TO_ROOM);

        af.where = TO_AFFECTS;
        af.type = gsn_poison;
        af.level = level * 3 / 4;

        af.duration = level / 2;
        af.location = APPLY_STR;
        af.modifier = -1;
        af.bitvector = AFF_POISON;
        affect_join(victim, &af);
      }

      /* weaken the poison if it's temporary */
      if (poison != NULL)
      {
        poison->level = UMAX(0, poison->level - 2);
        poison->duration = UMAX(0, poison->duration - 1);

        if (poison->level == 0 || poison->duration == 0)
          act("The {ypoison{x on $p has worn off.", ch, wield, NULL, TO_CHAR);
      }
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_MANADRAIN))
    {
      dam = wield->level * 0.5;
      xact_new("{k$p draws mana from $n.{x", victim, wield, ch, TO_VICT,
               POS_RESTING, VERBOSE_FLAGS);
      xact_new("{iYou feel $p drawing your mana away.{x", victim, wield,
               ch, TO_VICT, POS_RESTING, VERBOSE_FLAGS);
      victim->mana = victim->mana - dam;
      ch->mana += dam / 2;
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_VAMPIRIC))
    {
      dam = wield->level * 1.50;
      xact_new("{k$p draws life from $n.{x", victim, wield, ch, TO_VICT,
               POS_RESTING, VERBOSE_FLAGS);
      xact_new("{iYou feel $p drawing your life away.{x", victim, wield,
               NULL, TO_CHAR, POS_RESTING, VERBOSE_FLAGS);
      ch->alignment = UMAX(-1000, ch->alignment - 1);
      if (ch->pet != NULL)
        ch->pet->alignment = ch->alignment;
      ch->hit += dam / 2;
      xdamage(ch, victim, dam, 0, DAM_NEGATIVE, false, VERBOSE_STD, mobdeath);
      if (*mobdeath)
        return;
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_FLAMING))
    {
      dam = number_range(1, wield->level / 4 + 1);

      xact_new("{k$n is {rburned{k by $p.{x", victim, wield, ch,
               TO_VICT, POS_RESTING, VERBOSE_FLAGS);
      xact_new("{i$p {rsears{i your flesh.{x", victim, wield, NULL,
               TO_CHAR, POS_RESTING, VERBOSE_FLAGS);
      fire_effect((void *) victim, wield->level / 2, dam, TARGET_CHAR);
      xdamage(ch, victim, dam, 0, DAM_FIRE, false, VERBOSE_STD, mobdeath);
      if (*mobdeath)
        return;
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_FROST))
    {
      dam = number_range(1, wield->level / 6 + 2);
      xact_new("{k$p {cfreezes{k $n.{x", victim, wield, ch, TO_VICT,
               POS_RESTING, VERBOSE_FLAGS);
      xact_new("{iThe {Ccold{i touch of $p surrounds you with {Cice.{x",
               victim, wield, NULL, TO_CHAR, POS_RESTING, VERBOSE_FLAGS);
      cold_effect(victim, wield->level / 2, dam, TARGET_CHAR);
      xdamage(ch, victim, dam, 0, DAM_COLD, false, VERBOSE_STD, mobdeath);
      if (*mobdeath)
        return;
    }

    if (ch->fighting == victim && IS_WEAPON_STAT(wield, WEAPON_SHOCKING))
    {
      dam = number_range(1, wield->level / 5 + 2);
      xact_new("{k$n is struck by {Ylightning{k from $p.{x", victim,
               wield, ch, TO_VICT, POS_RESTING, VERBOSE_FLAGS);
      xact_new("{iYou are {Yshocked{i by $p.{x", victim, wield, NULL,
               TO_CHAR, POS_RESTING, VERBOSE_FLAGS);
      shock_effect(victim, wield->level / 2, dam, TARGET_CHAR);
      xdamage(ch, victim, dam, 0, DAM_LIGHTNING, false, VERBOSE_STD,
              mobdeath);
      if (*mobdeath)
        return;
    }

  }

  tail_chain();
  return;
}

void process_shields(CHAR_DATA * ch, CHAR_DATA * victim, bool * mobdeath)
{
  int dam, dt;
  int count = 0, total = 0;

  if (IS_SHIELDED(victim, SHD_POISON))
  {
    if (ch->fighting != victim)
      return;
    if (!IS_SHIELDED(ch, SHD_POISON))
    {
      dt = skill_lookup("poisonshield");
      dam = number_range(5, 15);
      total +=
        xdamage(victim, ch, dam, dt, DAM_POISON, true, VERBOSE_SHIELD,
                mobdeath);
      count++;
    }
  }
  if (!*mobdeath && IS_SHIELDED(victim, SHD_ICE))
  {
    if (ch->fighting != victim)
      return;
    if (!IS_SHIELDED(ch, SHD_ICE))
    {
      dt = skill_lookup("iceshield");
      dam = number_range(15, 25);
      total +=
        xdamage(victim, ch, dam, dt, DAM_COLD, true, VERBOSE_SHIELD,
                mobdeath);
      count++;
    }
  }
  if (!*mobdeath && IS_SHIELDED(victim, SHD_FIRE))
  {
    if (ch->fighting != victim)
      return;
    if (!IS_SHIELDED(ch, SHD_FIRE))
    {
      dt = skill_lookup("fireshield");
      dam = number_range(25, 35);
      total +=
        xdamage(victim, ch, dam, dt, DAM_FIRE, true, VERBOSE_SHIELD,
                mobdeath);
      count++;

    }
  }
  if (!*mobdeath && IS_SHIELDED(victim, SHD_SHOCK))
  {
    if (ch->fighting != victim)
      return;
    if (!IS_SHIELDED(ch, SHD_SHOCK))
    {
      dt = skill_lookup("shockshield");

      dam = number_range(35, 45);
      total +=
        xdamage(victim, ch, dam, dt, DAM_POISON, true, VERBOSE_SHIELD,
                mobdeath);
      count++;
    }
  }
  if (!*mobdeath && IS_SHIELDED(victim, SHD_ACID))
  {
    if (ch->fighting != victim)
      return;
    if (!IS_SHIELDED(ch, SHD_ACID))
    {
      dt = skill_lookup("acidshield");
      dam = number_range(45, 85);
      total +=
        xdamage(victim, ch, dam, dt, DAM_POISON, true, VERBOSE_SHIELD,
                mobdeath);
      count++;
    }
  }
  if (!*mobdeath && IS_SHIELDED(victim, SHD_BRIAR))
  {
    if (ch->fighting != victim)
      return;
    if (!IS_SHIELDED(ch, SHD_BRIAR))
    {
      dt = skill_lookup("briarshield");
      dam = number_range(30, 55);
      total +=
        xdamage(victim, ch, dam, dt, DAM_POISON, true, VERBOSE_SHIELD,
                mobdeath);
      count++;
    }
  }

  if (*mobdeath || ch->fighting != victim)
    return;
  if (count > 1)
    dam_message(victim, ch, total, -1, false, VERBOSE_SHIELD_COMP, false);
  else if (count == 1)
    dam_message(victim, ch, total, -1, false, VERBOSE_SHIELD_COMP, true);

  return;
}

/*
 * Mock hit one guy once.
 */
void one_hit_mock(CHAR_DATA * ch, CHAR_DATA * victim, int dt, bool secondary)
{
  OBJ_DATA *wield;
  int victim_ac;
  int thac0;
  int thac0_00;
  int thac0_32;
  int dam;
  int diceroll;
  int sn, skill;
  int dam_type;
  bool result;

  sn = -1;

  /* just in case */
  if (ch == NULL || victim == NULL)
    return;

  /* 
   * Can't beat a dead char!
   * Guard against weird room-leavings.
   */
  if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
    return;

  /* 
   * Figure out the type of damage message.
   * if secondary == true, use the second weapon.
   */
  if (!secondary)
    wield = get_eq_char(ch, WEAR_WIELD);
  else
    wield = get_eq_char(ch, WEAR_SECONDARY);
  if (dt == TYPE_UNDEFINED)
  {
    dt = TYPE_HIT;
    if (wield != NULL && wield->item_type == ITEM_WEAPON)
      dt += wield->value[3];
    else
      dt += ch->dam_type;
  }

  if (dt < TYPE_HIT)
    if (wield != NULL)
      dam_type = attack_table[wield->value[3]].damage;
    else
      dam_type = attack_table[ch->dam_type].damage;
  else
    dam_type = attack_table[dt - TYPE_HIT].damage;

  if (dam_type == -1)
    dam_type = DAM_BASH;

  /* get the weapon skill */
  sn = get_weapon_sn(ch);
  skill = 20 + get_weapon_skill(ch, sn);

  /* 
   * Calculate to-hit-armor-class-0 versus armor.
   */
  if (IS_NPC(ch))
  {
    thac0_00 = 20;
    thac0_32 = -4;              /* as good as a thief */

    if (IS_SET(ch->act, ACT_VAMPIRE))
      thac0_32 = -30;
    else if (IS_SET(ch->act, ACT_DRUID))
      thac0_32 = 0;
    else if (IS_SET(ch->act, ACT_RANGER))
      thac0_32 = -4;

    else if (IS_SET(ch->act, ACT_WARRIOR))
      thac0_32 = -10;
    else if (IS_SET(ch->act, ACT_THIEF))
      thac0_32 = -4;

    else if (IS_SET(ch->act, ACT_CLERIC))
      thac0_32 = 2;
    else if (IS_SET(ch->act, ACT_MAGE))
      thac0_32 = 6;
  }
  else
  {
    thac0_00 = class_table[ch->class].thac0_00;
    thac0_32 = class_table[ch->class].thac0_32;
  }
  thac0 = interpolate(ch->level, thac0_00, thac0_32);

  if (thac0 < 0)
    thac0 = thac0 / 2;

  if (thac0 < -5)
    thac0 = -5 + (thac0 + 5) / 2;

  thac0 -= GET_HITROLL(ch) * skill / 100;
  thac0 += 5 * (100 - skill) / 100;

  if (dt == gsn_backstab)
    thac0 -= 10 * (100 - get_skill(ch, gsn_backstab));

  if (dt == gsn_strike)
    thac0 -= 10 * (100 - get_skill(ch, gsn_strike));

  if (dt == gsn_circle)
    thac0 -= 10 * (100 - get_skill(ch, gsn_circle));
  if (dt == gsn_whirlwind)
    thac0 -= 10 * (100 - get_skill(ch, gsn_whirlwind));
  if (dt == gsn_assassinate)
    thac0 -= 10 * (100 - get_skill(ch, gsn_assassinate));

  switch (dam_type)
  {
    case (DAM_PIERCE):
      victim_ac = GET_AC(victim, AC_PIERCE) / 10;
      break;
    case (DAM_BASH):
      victim_ac = GET_AC(victim, AC_BASH) / 10;
      break;
    case (DAM_SLASH):
      victim_ac = GET_AC(victim, AC_SLASH) / 10;
      break;
    default:
      victim_ac = GET_AC(victim, AC_EXOTIC) / 10;
      break;
  };

  if (victim_ac < -15)
    victim_ac = (victim_ac + 15) / 5 - 15;

  if (!can_see(ch, victim))
    victim_ac -= 4;

  if (victim->position < POS_FIGHTING)
    victim_ac += 4;

  if (victim->position < POS_RESTING)
    victim_ac += 6;

  /* 
   * The moment of excitement!
   */
  while ((diceroll = number_bits(5)) >= 20)
    ;

  if (diceroll == 0 || (diceroll != 19 && diceroll < thac0 - victim_ac))
  {
    /* Miss. */
    damage_mock(ch, victim, 0, dt, dam_type, true);
    tail_chain();
    return;
  }

  /* 
   * Hit.
   * Calc damage.
   */
  if (IS_NPC(ch) && (wield == NULL))
    dam = dice(ch->damage[DICE_NUMBER], ch->damage[DICE_TYPE]);

  else
  {
    if (sn != -1)
      check_improve(ch, sn, true, 5);
    if (wield != NULL)
    {
      if (wield->clan)
      {
        dam = dice(ch->level / 3, 3) * skill / 100;
      }
      else
      {
        dam = dice(wield->value[1], wield->value[2]) * skill / 100;
      }

      if (get_eq_char(ch, WEAR_SHIELD) == NULL) /* no shield = more 
                                                 */
        dam = dam * 11 / 10;

      /* sharpness! */
      if (IS_WEAPON_STAT(wield, WEAPON_SHARP))
      {
        int percent;

        if ((percent = number_percent()) <= (skill / 8))
          dam = 2 * dam + (dam * 2 * percent / 100);
      }
    }

    else
      dam =
        number_range(1 + 4 * skill / 100, 2 * ch->level / 3 * skill / 100);
  }

  /* 
   * Bonuses.
   */
  if (get_skill(ch, gsn_enhanced_damage) > 0)
  {
    diceroll = number_percent();
    if (diceroll <= get_skill(ch, gsn_enhanced_damage))
    {
      check_improve(ch, gsn_enhanced_damage, true, 6);
      dam += 2 * (dam * diceroll / 300);
    }
  }

  if (!IS_AWAKE(victim))
    dam *= 2;
  else if (victim->position < POS_FIGHTING)
    dam = dam * 3 / 2;

  if (dt == gsn_backstab && wield != NULL)
  {
    if (wield->value[0] != 2)
      dam *= 2 + (ch->level / 10);
    else
      dam *= 2 + (ch->level / 8);
  }
  if (dt == gsn_circle && wield != NULL)
  {
    if (wield->value[0] != 2)
      dam *= 2 + (ch->level / 12);
    else
      dam *= 2 + (ch->level / 10);
  }
  if (dt == gsn_whirlwind && wield != NULL)
  {
    if (wield->value[0] != 2)
      dam *= 2 + (ch->level / 12);
    else
      dam *= 2 + (ch->level / 7);
  }

  if (dt == gsn_assassinate && wield != NULL)
  {
    if (IS_SHIELDED(ch, SHD_SWALK))
    {
      send_to_char
        ("The essence of surprise strengthens your attack!\n\r", ch);
      dam *= 2;
    }
    if (chance(20))
    {
      act("Someone nearly ends $n's miserable life!", victim, NULL,
          NULL, TO_ROOM);
      act("$n barely misses piercing your vital organs!", ch, NULL,
          victim, TO_VICT);
      printf_to_char(ch,
                     "{xYour assassination attempt barely misses its mark{x!!\n\r");
      dam *= 2 + (ch->level / 9);
    }
    else
      dam *= 2 + (ch->level / 8);
  }

  kill_swalk(ch);

  if (dt == gsn_strike && wield != NULL)
    dam *= (ch->level / 33);    /* max * 3 */

  dam += GET_DAMROLL(ch) * UMIN(100, skill) / 100;

  if (dam <= 0)
    dam = 1;

  result = damage_mock(ch, victim, dam, dt, dam_type, true);

  tail_chain();
  return;
}

/*
 * By Gregor Stipicic for support of verbose 
 * Inflict damage from a hit.
 */
int xdamage(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt, int dam_type,
            bool show, int verbose, bool * mobdeath)
{
  ROOM_INDEX_DATA *location;
  char buf[MAX_STRING_LENGTH];
  char pkbuf[MAX_STRING_LENGTH];
  OBJ_DATA *corpse;
  bool immune;
  int absorb;
  char bufl[MSL];
  char bufg[MSL];
  char bufa[MIL];
  char bufb[MIL];
  char bufc[MIL];
  bool vicisnpc = false;
  bool vicisch = false;

  absorb = 0;

  *mobdeath = false;
  if (IS_NPC(victim))
    vicisnpc = true;

  if (victim == ch)
    vicisch = true;

  if (victim->position == POS_DEAD || !WR(ch, victim) || !WR(victim, ch))
    return 0;

  if (victim != ch)
  {
    /* 
     * Certain attacks are forbidden.
     * Most other attacks are returned.
     */
    if (is_safe(ch, victim))
      return 0;

    if (victim->position > POS_STUNNED)
    {
      if (victim->fighting == NULL)
      {
        set_fighting(victim, ch);
        if (IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_KILL))
          mp_percent_trigger(victim, ch, NULL, NULL, TRIG_KILL);
      }
      if (victim->timer <= 4)
        victim->position = POS_FIGHTING;
    }

    if (victim->position > POS_STUNNED)
    {
      if (ch->fighting == NULL)
        set_fighting(ch, victim);

    }

    /* 
     * More charm stuff.
     */
    if (victim->master == ch)
      stop_follower(victim);
  }
  /* 
   * Inviso attacks ... not.
   */
  if (IS_SHIELDED(ch, SHD_INVISIBLE))
  {
    affect_strip(ch, gsn_invis);
    affect_strip(ch, gsn_mass_invis);
    REMOVE_BIT(ch->shielded_by, SHD_INVISIBLE);
    act("$n fades into existence.", ch, NULL, NULL, TO_ROOM);
  }

  /* 
   * Damage modifiers.
   */

  if (dam > 1 && !IS_NPC(victim) &&
      victim->pcdata->condition[COND_DRUNK] > 10)
    dam = 9 * dam / 10;

  if (dam > 1 && IS_SHIELDED(victim, SHD_DIVINE_PROTECTION))
    dam /= 2;
  if (dam > 1 && IS_SHIELDED(victim, SHD_SANCTUARY))
    dam /= 2;

  if (dam > 1 &&
      ((IS_SHIELDED(victim, SHD_PROTECT_EVIL) && IS_EVIL(ch)) ||
       (IS_SHIELDED(victim, SHD_PROTECT_GOOD) && IS_GOOD(ch))))
    dam -= dam / 4;

  immune = false;

  /* 
   * Check for parry, and dodge.
   */
  if (dt >= TYPE_HIT && ch != victim)
  {
    if (check_parry(ch, victim))
      return 0;
    if (check_dodge(ch, victim))
      return 0;
    if (check_shield_block(ch, victim))
      return 0;
  }

  switch (check_immune(victim, dam_type))
  {

    case (IS_IMMUNE):
      immune = true;
      dam = 0;
      break;
    case (IS_RESISTANT):
      dam -= dam / 3;
      break;
    case (IS_VULNERABLE):
      dam += dam / 2;
      break;
  }

  /* damage reduction */
  if (dam > 35)
    dam = (dam - 35) / 2 + 35;
  if (dam > 80)
    dam = (dam - 80) / 2 + 80;

  if (check_critical(ch, victim) && dam > 400)
  {
    int cchan;

    cchan =
      ((get_skill(ch, gsn_critical) / 4) + ((ch->level) / 8) +
       (number_range(1, 50)));

    if (cchan <= 25)
    {
      dam *= 1.5;
    }
    else if (cchan <= 50)
    {
      dam *= 2;
    }
    else if (cchan <= 75)
    {
      dam *= 2.5;
    }
    else if (cchan <= 90)
    {
      dam *= 4;
    }
    else if (cchan >= 98)
    {
      dam *= 10;
      act("{YHOLY SHIT!!!{x", ch, NULL, victim, TO_NOTVICT);
      act("{YHOLY SHIT!!!{x", ch, NULL, victim, TO_CHAR);
      act("{YHOLY SHIT!!!{x", ch, NULL, victim, TO_VICT);
      act("{YHOLY SHIT!!!{x", ch, NULL, victim, TO_NOTVICT);
      act("{YHOLY SHIT!!!{x", ch, NULL, victim, TO_CHAR);
      act("{YHOLY SHIT!!!{x", ch, NULL, victim, TO_VICT);
      act("{YHOLY SHIT!!!{x", ch, NULL, victim, TO_NOTVICT);
      act("{YHOLY SHIT!!!{x", ch, NULL, victim, TO_CHAR);
      act("{YHOLY SHIT!!!{x", ch, NULL, victim, TO_VICT);

    }

    if (cchan > 100)
      cchan = 100;

    sprintf(bufa, "{Y$n CRITICALLY STRIKES $N! {D({w%c%d{D){x", '%', cchan);
    sprintf(bufb, "{YYou CRITICALLY STRIKE $N!  {D({w%c%d{D){x", '%', cchan);
    sprintf(bufc, "{Y$n CRITICALLY STRIKES YOU  {D({w%c%d{D){x", '%', cchan);
    show = true;
    act(bufa, ch, NULL, victim, TO_NOTVICT);
    act(bufb, ch, NULL, victim, TO_CHAR);
    act(bufc, ch, NULL, victim, TO_VICT);

  }

  if (IS_SHIELDED(victim, SHD_WARD) && show &&
      (dt >= TYPE_HIT || dt >= gsn_backstab) && (dam > 0))
  {
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,
            "{GYour ward absorbs the damage and vanishes in a bright flash! {D({WAbsorb{D:{W%d{D){x\n\r",
            dam);
    affect_strip(victim, skill_lookup("ward"));
    send_to_char(buf, victim);
    sprintf(buf,
            "{G$n{G's ward absorbs the damage and vanishes in a bright flash! {D({WAbsorb{D:{W%d{D){x",
            dam);
    act(buf, victim, NULL, NULL, TO_ROOM);
    return 0;
  }

  if (IS_SHIELDED(victim, SHD_MANA) && show && (dt >= TYPE_HIT) && (dam > 0))
  {
    if (chance(30))
    {
      char buf[MAX_STRING_LENGTH];
      int absorb = (dam / 10) * number_range(1, 4);

      if ((victim->mana - absorb >= 0) && (absorb > 0))
      {
        sprintf(buf,
                "Your mana shield absorbs some of the damage! {D({WAbsorb{D:{W%d{D){x\n\r",
                absorb);
        send_to_char(buf, victim);
        sprintf(buf,
                "$n's mana shield absorbs some of the damage! {D({WAbsorb{D:{W%d{D){x",
                absorb);
        act(buf, victim, NULL, NULL, TO_ROOM);
        dam -= absorb;
        victim->mana -= absorb;
      }
    }
  }

  if (IS_SHIELDED(victim, SHD_LIFE) && (dam > 0) && victim->mana > 0
      && victim->max_mana > 0 &&
      (((victim->mana * 100) / victim->max_mana) > 20) &&
      !str_cmp(class_table[victim->class].name, "Elder") &&
      victim->level >= 202)
  {
    char buf[MAX_STRING_LENGTH];

    sprintf(buf,
            "Your lifeforce absorbs the damage! {D({WAbsorb cost{D: {W%d{D){x\n\r",
            (dam / 2));
    send_to_char(buf, victim);
    sprintf(buf, "$n's lifeforce absorbs the damage!{x");
    act(buf, victim, NULL, NULL, TO_ROOM);

    victim->mana -= (dam / 2);
    dam = 0;
    return 0;
  }


  /* DAMCAP */
  /* If the victim is your level or higher, cap at 20% of victim's max hp. */
  if (ch->level - victim->level <= 0)
    dam = UMIN((victim->max_hit * 20 / 100), dam);
  else if (ch->level - victim->level == 1)  /* Victim is one level below, cap at 30% */
    dam = UMIN((victim->max_hit * 30 / 100), dam);
  else if (ch->level - victim->level == 2)  /* Victim is two levels below, cap at 45% */
    dam = UMIN((victim->max_hit * 45 / 100), dam);
  else if (ch->level - victim->level == 3)  /* Victim is three levels below, cap at 65% */
    dam = UMIN((victim->max_hit * 65 / 100), dam);
  else if (ch->level - victim->level == 4)  /* Victim is four levels below, cap at 90% */
    dam = UMIN((victim->max_hit * 90 / 100), dam);

  /* No cap if victim is 5 or more levels below the attacker. */


  if (show && dam > 0)
    dam_message(ch, victim, dam, dt, immune, verbose, true);

  if (dam == 0)
    return 0;

  /* 
   * Hurt the victim.
   * Inform the victim of his new state.
   */
  victim->hit -= dam;
  if (!IS_NPC(victim) && victim->level >= LEVEL_IMMORTAL && victim->hit < 1)
    victim->hit = 1;

  update_pos(victim);
  if (dt == gsn_feed)
  {
    ch->hit = UMIN(ch->hit + (dam * .8), ch->max_hit);
    update_pos(ch);
  }

  switch (victim->position)
  {
    case POS_MORTAL:
      act("{c$n is mortally wounded, and will die soon, if not aided.{x",
          victim, NULL, NULL, TO_ROOM);
      send_to_char
        ("{cYou are mortally wounded, and will die soon, if not aided.{x\n\r",
         victim);
      break;

    case POS_INCAP:
      act("{c$n is incapacitated and will slowly die, if not aided.{x",
          victim, NULL, NULL, TO_ROOM);
      send_to_char
        ("{cYou are incapacitated and will slowly {z{Rdie{x{c, if not aided.{x\n\r",
         victim);
      break;

    case POS_STUNNED:
      act("{c$n is stunned, but will probably recover.{x", victim, NULL,
          NULL, TO_ROOM);
      send_to_char
        ("{cYou are stunned, but will probably recover.{x\n\r", victim);
      break;

    case POS_DEAD:
      if ((IS_NPC(victim)) && (victim->die_descr[0] != '\0'))
      {
        act("{c$n $T{x", victim, 0, victim->die_descr, TO_ROOM);
      }
      else
      {
        act("{c$n is {CDEAD!!{x", victim, 0, 0, TO_ROOM);
      }
      send_to_char("{cYou have been {RKILLED!!{x\n\r\n\r", victim);
      break;

    default:
      if (dam > victim->max_hit / 4)
        if (!IS_NPC(victim))
          if (!IS_SET(victim->pcdata->verbose, VERBOSE_DAMAGE))
            send_to_char("{cThat really did {RHURT!{x\n\r", victim);
      if (victim->hit < victim->max_hit / 4)
        if (!IS_NPC(victim))
          if (!IS_SET(victim->pcdata->verbose, VERBOSE_DAMAGE))
            // send_to_char ( "{cYou sure are
            // {z{RBLEEDING!{x\n\r",
            // victim );
            break;
  }

  /* 
   * Sleep spells and extremely wounded folks.
   */
  if (!IS_AWAKE(victim))
    stop_fighting(victim, false);

  if (victim->position == POS_DEAD)
  {
    if (!IS_NPC(victim) && !IS_NPC(ch) &&
        IS_SET(victim->in_room->room_flags, ROOM_ARENA) &&
        IS_SET(ch->in_room->room_flags, ROOM_ARENA))
    {
      check_arena(ch, victim);
      return true;
    }
  }
  /* 
   * Payoff for killing things.
   */
  if (victim->position == POS_DEAD)
  {

    group_gain(ch, victim);

    if (!IS_NPC(victim))
    {
      sprintf(log_buf, "%s killed by %s at %ld", victim->name,
              (IS_NPC(ch) ? ch->short_descr : ch->name), ch->in_room->vnum);
      log_string(log_buf);

      /* 
       * Dying penalty:
       * 2/3 way back to previous level.
       */
      if (victim->exp >
          exp_per_level(victim, victim->pcdata->points) * victim->level)
        gain_exp(victim,
                 (5 *
                  (exp_per_level(victim, victim->pcdata->points)
                   * victim->level - victim->exp) / 6) + 50);

    }
    if (!IS_NPC(ch) && !IS_NPC(victim) && ch->name != victim->name)
    {
      sprintf(buf,
              "{w[{RPK{w] {R%s{w has murdered {Y%s{w at {Y%s{w!{x\n\r",
              ch->name, victim->name, ch->in_room->name);
      if (!WR(ch, victim) && !IS_IMMORTAL(ch) && !IS_IMMORTAL(victim))
      {
        sprintf(buf,
                "{W-=-={R{zPK{x{W-=>> {R%s{w has murdered {Y%s{w at %s{w. {R( {Y{zILLEGAL PK LOGGED!{x{R){x\n\r",
                ch->name, victim->name, ch->in_room->name);
        sprintf(pkbuf,
                "%s ( level %d tier %d ), murdered %s ( level %d tier %d ). Out of range PK!\n\r",
                ch->name, ch->level, ch->pcdata->ctier, victim->name,
                victim->level, victim->pcdata->ctier);

        if (ch->fighting != NULL)
          stop_fighting(ch, true);

        SET_BIT(ch->act, PLR_FREEZE);
        char_from_room(ch);
        location = find_location(ch, "3");
        char_to_room(ch, location);

        ch->corner_timer = 30;
        ch->pk_timer = 120;
        sprintf(bufl, "You have been cornered for %d minutes.\n\r",
                ch->corner_timer);
        send_to_char(bufl, ch);

        sprintf(bufl,
                "%s has been cornered for %d minutes for killing you out of range.\n\r",
                ch->name, ch->corner_timer);
        send_to_char(bufl, victim);

        sprintf(bufg,
                "{W[{RPENALTY{W] %s has been cornered for %d minutes for killing %s out of PK range.\n\r",
                ch->name, ch->corner_timer, victim->name);

        append_file(ch, NWRPK_FILE, pkbuf);
        do_gmessage(buf);
        do_gmessage(bufg);
      }
      else
      {

        do_gmessage(buf);
        victim->pk_timer = 2;
      }
    }
    sprintf(log_buf, "%s got toasted by %s at %s [room %ld]",
            (IS_NPC(victim) ? victim->short_descr : victim->name),
            (IS_NPC(ch) ? ch->short_descr : ch->name),
            ch->in_room->name, ch->in_room->vnum);

    if (IS_NPC(victim))
      wiznet(log_buf, NULL, NULL, WIZ_MOBDEATHS, 0, 0);
    else
      wiznet(log_buf, NULL, NULL, WIZ_DEATHS, 0, 0);

    /* 
     * Death trigger
     */
    if (IS_NPC(victim) && HAS_TRIGGER(victim, TRIG_DEATH))
    {
      victim->position = POS_STANDING;
      mp_percent_trigger(victim, ch, NULL, NULL, TRIG_DEATH);
    }

    /* dump the flags */
    if (ch != victim && !IS_NPC(ch) &&
        (!is_same_clan(ch, victim) || clan_table[victim->clan].independent))
    {
      if (IS_SET(victim->act, PLR_TWIT))
        REMOVE_BIT(victim->act, PLR_TWIT);
    }



    raw_kill(victim, ch, mobdeath);

    if (vicisnpc)
    {
      victim = NULL;
    }

    /* RT new auto commands */

    if (!IS_NPC(ch) && vicisnpc)
    {
      OBJ_DATA *coins;

      corpse = get_obj_list(ch, "corpse", ch->in_room->contents);

      if (IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains)  /* exists 
                                                                           and 
                                                                           not 
                                                                           empty 
                                                                         */
        do_get(ch, "all corpse");

      if (IS_SET(ch->act, PLR_AUTOGOLD) && corpse && corpse->contains &&  /* exists 
                                                                             and 
                                                                             not 
                                                                             empty 
                                                                           */
          !IS_SET(ch->act, PLR_AUTOLOOT))
        if ((coins = get_obj_list(ch, "gcash", corpse->contains)) != NULL)
          do_get(ch, "all.gcash corpse");

      if (IS_SET(ch->act, PLR_AUTOSAC))
      {
        if (IS_SET(ch->act, PLR_AUTOLOOT) && corpse && corpse->contains)
          return dam;           /* leave if corpse has treasure */
        else
          do_sacrifice(ch, "corpse");
      }
    }
    return dam;
  }

  if (vicisch)
    return dam;

  /* 
   * Take care of link dead people.
   */
  if (!vicisnpc && victim->desc == NULL)
  {
    if (number_range(0, victim->wait) == 0)
    {
      do_recall(victim, "");
      return dam;
    }
  }

  /* 
   * Wimp out?
   */

  if ((!IS_SET(ch->act2, PLR2_CHALLENGED) ||
       !IS_SET(ch->act2, PLR2_CHALLENGER)) && arena != FIGHT_BUSY)
  {
    if (vicisnpc && (*mobdeath != true) && dam > 0 &&
        victim->wait < PULSE_VIOLENCE / 2)
    {
      if ((IS_SET(victim->act, ACT_WIMPY) && number_bits(2) == 0
           && victim->hit < victim->max_hit / 5) ||
          (IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL
           && victim->master->in_room != victim->in_room))
        do_flee(victim, "");
    }

    if (!vicisnpc && victim->hit > 0 &&
        victim->hit <= victim->wimpy && victim->wait < PULSE_VIOLENCE / 2)
      do_flee(victim, "");

  }

  tail_chain();
  return dam;
}


/*
 * Show damage from a mock hit.
 */
bool damage_mock(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
                 int dam_type, bool show)
{
  long immdam;
  bool immune;
  char buf1[256], buf2[256], buf3[256];
  const char *attack;

  if (victim->position == POS_DEAD)
    return false;

  dam = dam / 2;
  if (is_safe_mock(ch, victim))
    return false;
  /* 
   * Damage modifiers.
   */

  if (dam > 1 && !IS_NPC(victim) &&
      victim->pcdata->condition[COND_DRUNK] > 10)
    dam = 9 * dam / 10;

  if (dam > 1 && IS_SHIELDED(victim, SHD_DIVINE_PROTECTION))
    dam /= 2;
  if (dam > 1 && IS_SHIELDED(victim, SHD_SANCTUARY))
    dam /= 2;

  if (dam > 1 &&
      ((IS_SHIELDED(victim, SHD_PROTECT_EVIL) && IS_EVIL(ch)) ||
       (IS_SHIELDED(victim, SHD_PROTECT_GOOD) && IS_GOOD(ch))))
    dam -= dam / 4;

  immune = false;

  switch (check_immune(victim, dam_type))
  {
    case (IS_IMMUNE):
      immune = true;
      dam = 0;
      break;
    case (IS_RESISTANT):
      dam -= dam / 3;
      break;
    case (IS_VULNERABLE):
      dam += dam / 2;
      break;
  }

  if (dt >= 0 && dt < MAX_SKILL)
    attack = skill_table[dt].noun_damage;
  else if (dt >= TYPE_HIT && dt <= TYPE_HIT + MAX_DAMAGE_MESSAGE)
    attack = attack_table[dt - TYPE_HIT].noun;
  else
  {
    bug("Dam_message: bad dt %d.", dt);
    dt = TYPE_HIT;
    attack = attack_table[0].name;
  }
  immdam = 0;
  if (ch->level == MAX_LEVEL)
  {
    immdam = dam * 63;
  }
  if (ch == victim)
  {
    sprintf(buf1,
            "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {y$mself{g.{x",
            attack, dam);
    sprintf(buf2,
            "{yYour {gmock {B%s{g would have done {R%d hp{g damage to {yyourself{g.{x",
            attack, dam);
    act(buf1, ch, NULL, NULL, TO_ROOM);
    act(buf2, ch, NULL, NULL, TO_CHAR);
  }
  else if (ch->level < MAX_LEVEL)
  {
    sprintf(buf1,
            "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {y$N{g.{x",
            attack, dam);
    sprintf(buf2,
            "{yYour {gmock {B%s{g would have done {R%d hp{g damage to {y$N{g.{x",
            attack, dam);
    sprintf(buf3,
            "{y$n's {gmock {B%s{g would have done {R%d hp{g damage to {yyou{g.{x",
            attack, dam);
    act(buf1, ch, NULL, victim, TO_NOTVICT);
    act(buf2, ch, NULL, victim, TO_CHAR);
    act(buf3, ch, NULL, victim, TO_VICT);
  }
  else
  {
    sprintf(buf1,
            "{y$n's {gmock {B%s{g would have done {R%lu hp{g damage to {y$N{g.{x",
            attack, immdam);
    sprintf(buf2,
            "{yYour {gmock {B%s{g would have done {R%lu hp{g damage to {y$N{g.{x",
            attack, immdam);
    sprintf(buf3,
            "{y$n's {gmock {B%s{g would have done {R%lu hp{g damage to {yyou{g.{x",
            attack, immdam);
    act(buf1, ch, NULL, victim, TO_NOTVICT);
    act(buf2, ch, NULL, victim, TO_CHAR);
    act(buf3, ch, NULL, victim, TO_VICT);
  }

  tail_chain();
  return true;
}

bool is_safe(CHAR_DATA * ch, CHAR_DATA * victim)
{

  if (victim->in_room == NULL || ch->in_room == NULL)
    return true;

  if (victim->fighting == ch || victim == ch)
    return false;

  if (!IS_NPC(ch) && IS_IMMORTAL(ch))
    return false;

  /* ARENA room checking */
  if (IS_SET(ch->in_room->room_flags, ROOM_ARENA) &&
      IS_SET(victim->in_room->room_flags, ROOM_ARENA))
    return false;

  /* killing mobiles */
  if (IS_NPC(victim))
  {

    /* safe room? */
    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
    {
      send_to_char("Not in this room.\n\r", ch);
      return true;
    }

    if (victim->pIndexData->pShop != NULL)
    {
      send_to_char("The shopkeeper wouldn't like that.\n\r", ch);
      return true;
    }

    /* no killing healers, trainers, etc */
    if (IS_SET(victim->act, ACT_TRAIN) ||
        IS_SET(victim->act, ACT_PRACTICE) ||
        IS_SET(victim->act, ACT_IS_HEALER) ||
        IS_SET(victim->act, ACT_IS_CHANGER) ||
        IS_SET(victim->act, ACT_IS_SATAN) ||
        IS_SET(victim->act, ACT_IS_PRIEST))
    {
      act("I don't think $G would approve.", ch, NULL, NULL, TO_CHAR);
      return true;
    }

    if (!IS_NPC(ch))
    {
      /* no pets */
      if (IS_SET(victim->act, ACT_PET))
      {
        act("But $N looks so cute and cuddly...", ch, NULL, victim, TO_CHAR);
        return true;
      }

      /* no charmed creatures unless owner */
      if (IS_AFFECTED(victim, AFF_CHARM) && ch != victim->master)
      {
        send_to_char("You don't own that monster.\n\r", ch);
        return true;
      }
    }
  }
  /* killing players */
  else
  {
    /* NPC doing the killing */
    if (IS_NPC(ch))
    {
      /* safe room check */
      if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
      {
        send_to_char("Not in this room.\n\r", ch);
        return true;
      }

      /* charmed mobs and pets cannot attack players while owned */
      if (IS_AFFECTED(ch, AFF_CHARM) && ch->master != NULL &&
          ch->master->fighting != victim)
      {
        send_to_char("Players are your friends!\n\r", ch);
        return true;
      }
    }
    /* player doing the killing */
    else
    {
      if (IS_SET(victim->act, PLR_TWIT))
        return false;

      if (((victim->level > 19) ||
           ((victim->class >= MAX_CLASS / 2) &&
            (victim->level > 14))) && (is_voodood(ch, victim)))
        return false;

      if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
      {
        send_to_char("Not in this room.\n\r", ch);
        return true;
      }
      if (ch->on_quest)
      {
        send_to_char("Not while you are on a quest.\n\r", ch);
        return true;
      }
      if (victim->on_quest)
      {
        send_to_char("They are on a quest, leave them alone.\n\r", ch);
        return true;
      }
      if (!is_clan(ch))
      {
        send_to_char("Join a clan if you want to fight players.\n\r", ch);
        return true;
      }

      if (!is_pkill(ch))
      {
        send_to_char("Your clan does not allow player fighting.\n\r", ch);
        return true;
      }

      if (!is_clan(victim))
      {
        send_to_char("They aren't in a clan, leave them alone.\n\r", ch);
        return true;
      }

      if (!is_pkill(victim))
      {
        send_to_char
          ("They are in a no pkill clan, leave them alone.\n\r", ch);
        return true;
      }

      if (is_same_clan(ch, victim))
      {
        send_to_char("You can't fight your own clan members.\n\r", ch);
        return true;
      }
    }
  }
  return false;
}

bool can_pk(CHAR_DATA * ch, CHAR_DATA * victim)
{
  int tvalue;
  int lvalue;
  int pkvalue;

  if (IS_SET(class_table[ch->class].tier, TIER_01))
    tvalue = 1;

  else if (IS_SET(class_table[ch->class].tier, TIER_02))
    tvalue = 2;

  else if (IS_SET(class_table[ch->class].tier, TIER_03))
    tvalue = 3;

  else
    tvalue = 4;

  if (ch->level >= LEVEL_ANCIENT)
    lvalue = ch->level / 11;
  else
    lvalue = ch->level / 10;
  pkvalue = tvalue + lvalue;

  if ((ch->pcdata->pkvalue + 10) > (victim->pcdata->pkvalue - 10))
  {
    send_to_char
      ("They are far to weak to be considered a worthy target by the likes of you!\n\r",
       ch);
    return false;
  }

  if ((ch->pcdata->pkvalue + 15) < (victim->pcdata->pkvalue - 15))
  {
    printf_to_char(ch,
                   "They are far to strong to be considered an actual target by the likes of you!\n\r");
    return false;
  }

  else
  {
    return true;
  }
  return false;
}

bool is_safe_mock(CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (victim->in_room == NULL || ch->in_room == NULL)
    return true;
  if (!IS_NPC(ch) && IS_IMMORTAL(ch))
    return false;
  if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
  {
    send_to_char("Not in this room.\n\r", ch);
    return true;
  }
  if (IS_NPC(victim))
  {
    send_to_char("{RYou can only use this on a player.{x\n\r", ch);
    return true;
  }
  return false;
}

bool is_voodood(CHAR_DATA * ch, CHAR_DATA * victim)
{
  OBJ_DATA *object;
  bool found;

  if (ch->level > HERO)
    return false;

  found = false;
  for (object = victim->carrying; object != NULL;
       object = object->next_content)
  {
    if (object->pIndexData->vnum == OBJ_VNUM_VOODOO)
    {
      char arg[MAX_INPUT_LENGTH];

      one_argument(object->name, arg);
      if (!str_cmp(arg, ch->name))
      {
        return true;
      }
    }
  }
  return false;
}

bool is_safe_spell(CHAR_DATA * ch, CHAR_DATA * victim, bool area)
{
  if (victim->in_room == NULL || ch->in_room == NULL)
    return true;

  if (victim == ch && area)
    return true;

  if (victim->fighting == ch || victim == ch)
    return false;

  /* ARENA room checking */
  if (IS_SET(ch->in_room->room_flags, ROOM_ARENA) &&
      IS_SET(victim->in_room->room_flags, ROOM_ARENA))
    return false;

  if (!IS_NPC(ch) && IS_IMMORTAL(ch))
    return false;

  /* killing mobiles */
  if (IS_NPC(victim))
  {
    /* safe room? */
    if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
      return true;

    if (victim->pIndexData->pShop != NULL)
      return true;

    /* no killing healers, trainers, etc */
    if (IS_SET(victim->act, ACT_TRAIN) ||
        IS_SET(victim->act, ACT_PRACTICE) ||
        IS_SET(victim->act, ACT_IS_HEALER) ||
        IS_SET(victim->act, ACT_IS_CHANGER) ||
        IS_SET(victim->act, ACT_IS_SATAN) ||
        IS_SET(victim->act, ACT_IS_PRIEST))
      return true;

    if (!IS_NPC(ch))
    {
      /* no pets */
      if (IS_SET(victim->act, ACT_PET))
        return true;

      /* no charmed creatures unless owner */
      if (IS_AFFECTED(victim, AFF_CHARM) && (area || ch != victim->master))
        return true;

      /* legal kill? -- cannot hit mob fighting non-group member */
      if (victim->fighting != NULL && !is_same_group(ch, victim->fighting))
        return true;
    }
    else
    {
      /* area effect spells do not hit other mobs */
      if (area && !is_same_group(victim, ch->fighting))
        return true;
    }
  }
  /* killing players */
  else
  {
    if (area && IS_IMMORTAL(victim) && victim->level > LEVEL_IMMORTAL)
      return true;

    /* NPC doing the killing */
    if (IS_NPC(ch))
    {
      /* charmed mobs and pets cannot attack players while owned */
      if (((IS_AFFECTED(ch, AFF_CHARM)) & (ch->master != NULL))
          && (ch->master->fighting != victim))
        return true;

      /* safe room? */
      if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
        return true;

      /* legal kill? -- mobs only hit players grouped with opponent */
      if (ch->fighting != NULL && !is_same_group(ch->fighting, victim))
        return true;
    }

    /* player doing the killing */
    else
    {
      if (IS_SET(victim->act, PLR_TWIT))
        return false;

      if (((victim->level > 19) ||
           ((victim->class >= MAX_CLASS / 2) &&
            (victim->level > 14))) && (is_voodood(ch, victim)))
        return false;

      if (!is_clan(ch))
        return true;

      if (!is_pkill(ch))
        return true;

      if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
        return true;

      if (ch->on_quest)
        return true;

      if (victim->on_quest)
        return true;

      if (!is_clan(victim))
        return true;

      if (!is_pkill(victim))
        return true;

      if (is_same_clan(ch, victim))
        return true;

      if (((ch->class < MAX_CLASS / 2) &&
           (victim->class < MAX_CLASS / 2)) ||
          ((ch->class >= MAX_CLASS / 2) && (victim->class >= MAX_CLASS / 2)))
      {
        if (ch->level > victim->level + 15)
        {
          return true;
        }
        if (ch->level < victim->level - 15)
        {
          return true;
        }
      }
      else
      {
        return true;
      }
    }
  }
  return false;
}

/*
 * Check for parry.
 */
bool check_parry(CHAR_DATA * ch, CHAR_DATA * victim)
{
  int chance;

  if (!IS_AWAKE(victim))
    return false;

  chance = get_skill(victim, gsn_parry) / 2;

  if (get_eq_char(victim, WEAR_WIELD) == NULL)
  {
    if (IS_NPC(victim))
      chance /= 2;
    else
      return false;
  }

  if (victim->stunned)
    return false;

  if (number_percent() >= chance + victim->level - ch->level)
    return false;

  xact_new("{iYou parry $n's attack.{x", ch, NULL, victim, TO_VICT,
           POS_RESTING, VERBOSE_DODGE);
  xact_new("{h$N parries your attack.{x", ch, NULL, victim, TO_CHAR,
           POS_RESTING, VERBOSE_DODGE);
  check_improve(victim, gsn_parry, true, 6);
  return true;
}

/*
 * Check for shield block.
 */
bool check_shield_block(CHAR_DATA * ch, CHAR_DATA * victim)
{
  bool levitate;
  int chance;

  if (!IS_AWAKE(victim))
    return false;

  levitate = (get_skill(victim, gsn_shield_levitation) != 0) &&
    (skill_table[gsn_shield_levitation].skill_level[ch->class] <=
     ch->level) && (get_eq_char(victim, WEAR_SHIELD) != NULL) &&
    ((get_eq_char(victim, WEAR_SECONDARY) != NULL) ||
     ((get_eq_char(victim, WEAR_WIELD) != NULL) &&
      (get_eq_char(victim, WEAR_SECONDARY) == NULL) &&
      IS_WEAPON_STAT(get_eq_char(victim, WEAR_WIELD), WEAPON_TWO_HANDS)));

  chance = get_skill(victim, gsn_shield_block) / 5 + 3;
  if (levitate)
    chance =
      (chance + (get_skill(victim, gsn_shield_levitation) / 5 + 3)) / 2;

  if (get_eq_char(victim, WEAR_SHIELD) == NULL)
    return false;

  if (number_percent() >= chance + victim->level - ch->level)
    return false;

  if (victim->stunned)
    return false;

  xact_new("{iYou block $n's attack with your shield.{x", ch, NULL, victim,
           TO_VICT, POS_RESTING, VERBOSE_DODGE);
  xact_new("{h$N blocks your attack with a shield.{x", ch, NULL, victim,
           TO_CHAR, POS_RESTING, VERBOSE_DODGE);
  check_improve(victim, gsn_shield_block, true, 6);
  if (levitate)
    check_improve(victim, gsn_shield_levitation, true, 6);
  return true;
}

/*
 * Check for dodge.
 */
bool check_dodge(CHAR_DATA * ch, CHAR_DATA * victim)
{
  int chance;

  if (!IS_AWAKE(victim))
    return false;

  chance = get_skill(victim, gsn_dodge) / 2;

  if (!can_see(victim, ch))
    chance /= 2;

  if (number_percent() >= chance + victim->level - ch->level)
    return false;

  if (victim->stunned)
    return false;

  xact_new("{iYou dodge $n's attack.{x", ch, NULL, victim, TO_VICT,
           POS_RESTING, VERBOSE_DODGE);
  xact_new("{h$N dodges your attack.{x", ch, NULL, victim, TO_CHAR,
           POS_RESTING, VERBOSE_DODGE);

  check_improve(victim, gsn_dodge, true, 6);
  return true;
}

/*
 * Set position of a victim.
 */
void update_pos(CHAR_DATA * victim)
{
  if (victim->hit > 0)
  {
    if (victim->position <= POS_STUNNED)
      victim->position = POS_STANDING;
    return;
  }

  if (IS_NPC(victim) && victim->hit < 1)
  {
    victim->position = POS_DEAD;
    return;
  }

  if (victim->hit <= -11)
  {
    victim->position = POS_DEAD;
    return;
  }

  if (victim->hit <= -6)
    victim->position = POS_MORTAL;
  else if (victim->hit <= -3)
    victim->position = POS_INCAP;
  else
    victim->position = POS_STUNNED;

  return;
}

/*
 * Start fights.
 */
void set_fighting(CHAR_DATA * ch, CHAR_DATA * victim)
{
  if (ch->fighting != NULL)
  {
    bug("Set_fighting: already fighting", 0);
    return;
  }

  if (IS_AFFECTED(ch, AFF_SLEEP))
    affect_strip(ch, gsn_sleep);

  ch->fighting = victim;
  ch->position = POS_FIGHTING;
  ch->stunned = 0;

  return;
}

/*
 * Stop fights.
 */
void stop_fighting(CHAR_DATA * ch, bool fBoth)
{
  CHAR_DATA *fch;
  char buf[MAX_STRING_LENGTH];

  for (fch = char_list; fch != NULL; fch = fch->next)
  {
    if (fch == ch || (fBoth && fch->fighting == ch))
    {
      fch->fighting = NULL;
      fch->position = IS_NPC(fch) ? fch->default_pos : POS_STANDING;
      fch->stunned = 0;
      update_pos(fch);
      if (IS_SET(fch->comm, COMM_STORE))
        if (fch->tells)
        {
          sprintf(buf, "{cYou have {R%d{c tells waiting.{x\n\r", fch->tells);
          send_to_char(buf, fch);
          send_to_char("{cType '{Rreplay{c' to see tells.{x\n\r", fch);
        }
    }
  }

  return;
}

/*
 * Make a corpse out of a character.
 */
void make_corpse(CHAR_DATA * ch, CHAR_DATA * killer)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *corpse;
  OBJ_DATA *pile;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  char *name;
  ROOM_INDEX_DATA *morgue;
  bool did_make_itempile = false;

  morgue = get_room_index(ROOM_VNUM_MORGUE);

  if (IS_NPC(ch))
  {
    if (IS_SET(ch->act, ACT_NO_BODY))
    {
      if (IS_SET(ch->act, ACT_NB_DROP))
      {
        for (obj = ch->carrying; obj != NULL; obj = obj_next)
        {
          obj_next = obj->next_content;
          obj_from_char(obj);
          if (obj->item_type == ITEM_POTION)
            obj->timer = number_range(500, 1000);
          if (obj->item_type == ITEM_SCROLL)
            obj->timer = number_range(1000, 2500);
          if (IS_SET(obj->extra_flags, ITEM_ROT_DEATH))
          {
            obj->timer = number_range(5, 10);
            REMOVE_BIT(obj->extra_flags, ITEM_ROT_DEATH);
          }
          REMOVE_BIT(obj->extra_flags, ITEM_VIS_DEATH);

          if (IS_SET(obj->extra_flags, ITEM_INVENTORY))
          {
            extract_obj(obj);
            obj = NULL;
            return;
          }

          act("$p falls to the floor.", ch, obj, NULL, TO_ROOM);
          obj_to_room(obj, ch->in_room);
        }
      }
      return;
    }
    name = ch->short_descr;
    corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC));
    corpse->timer = number_range(3, 6);

    pile = create_object(get_obj_index(OBJ_VNUM_ITEMPILE));
    pile->timer = number_range(25, 40);

    if (ch->gold > 0 || ch->platinum > 0 || ch->silver > 0)
    {
      obj_to_obj(create_money(ch->platinum, ch->gold, ch->silver), corpse);
      ch->platinum = 0;
      ch->gold = 0;
      ch->silver = 0;
    }
    corpse->cost = 0;
    pile->cost = 0;
  }
  else
  {
    name = ch->name;
    corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC));
    corpse->timer = number_range(25, 40);

    pile = create_object(get_obj_index(OBJ_VNUM_ITEMPILE));
    pile->timer = number_range(25, 40);

    REMOVE_BIT(ch->act, PLR_CANLOOT);
    if (!is_clan(ch))
    {
      free_string(corpse->owner);
      corpse->owner = str_dup(ch->name);
      corpse->killer = NULL;

      free_string(pile->owner);
      pile->owner = str_dup(ch->name);
      pile->killer = NULL;
    }
    else
    {
      free_string(corpse->owner);
      corpse->owner = str_dup(ch->name);
      free_string(corpse->killer);
      corpse->killer = str_dup(killer->name);

      free_string(pile->owner);
      pile->owner = str_dup(ch->name);
      free_string(pile->killer);
      pile->killer = str_dup(killer->name);

      if (ch->platinum > 1 || ch->gold > 1 || ch->silver > 1)
      {
        obj_to_obj(create_money
                   (ch->platinum / 2, ch->gold / 2, ch->silver / 2), corpse);
        ch->platinum -= ch->platinum / 2;
        ch->gold -= ch->gold / 2;
        ch->silver -= ch->silver / 2;
      }
    }

    corpse->cost = 0;
    pile->cost = 0;
  }

  corpse->level = ch->level;
  pile->level = ch->level;

  sprintf(buf, corpse->short_descr, name);
  free_string(corpse->short_descr);
  corpse->short_descr = str_dup(buf);

  sprintf(buf, pile->short_descr, name);
  free_string(pile->short_descr);
  pile->short_descr = str_dup(buf);

  sprintf(buf, corpse->description, name);
  free_string(corpse->description);
  corpse->description = str_dup(buf);

  sprintf(buf, pile->description, name);
  free_string(pile->description);
  pile->description = str_dup(buf);

  for (obj = ch->carrying; obj != NULL; obj = obj_next)
  {
    bool floating = false;

    obj_next = obj->next_content;
    if (obj->wear_loc == WEAR_FLOAT)
      floating = true;
    obj_from_char(obj);
    if (obj->item_type == ITEM_POTION)
      obj->timer = number_range(500, 1000);
    if (obj->item_type == ITEM_SCROLL)
      obj->timer = number_range(1000, 2500);
    if (IS_SET(obj->extra_flags, ITEM_ROT_DEATH) && !floating)
    {
      obj->timer = number_range(5, 10);
      REMOVE_BIT(obj->extra_flags, ITEM_ROT_DEATH);
    }
    REMOVE_BIT(obj->extra_flags, ITEM_VIS_DEATH);

    if (IS_SET(obj->extra_flags, ITEM_INVENTORY))
    {
      extract_obj(obj);
      obj = NULL;
    }
    else if (floating)
    {
      if (IS_OBJ_STAT(obj, ITEM_ROT_DEATH)) /* get rid of it! */
      {
        if (obj->contains != NULL)
        {
          OBJ_DATA *in, *in_next;

          act("$p evaporates,scattering its contents.", ch, obj,
              NULL, TO_ROOM);
          for (in = obj->contains; in != NULL; in = in_next)
          {
            in_next = in->next_content;
            obj_from_obj(in);
            obj_to_room(in, ch->in_room);
          }
        }
        else
          act("$p evaporates.", ch, obj, NULL, TO_ROOM);
        extract_obj(obj);
        obj = NULL;
      }

      else
      {
        act("$p falls to the floor.", ch, obj, NULL, TO_ROOM);
        obj_to_room(obj, ch->in_room);
      }
    }
    else if ((number_range(0, 100) >= 75 && is_pkill(ch)) ||
             IS_SET(ch->in_room->room_flags, ROOM_ARENA) ||
             (!is_pkill(ch) || IS_NPC(killer)))
    {
      obj_to_obj(obj, corpse);
    }
    else
    {
      obj_to_obj(obj, pile);
      did_make_itempile = true;
    }
  }
  if (did_make_itempile)
    obj_to_room(pile, ch->in_room);
  else
  {
    extract_obj(pile);
    pile = NULL;
  }

  obj_to_room(corpse, ch->in_room);
  return;
}

/*
 * Improved Death_cry contributed by Diavolo.
 */
void death_cry(CHAR_DATA * ch)
{
  ROOM_INDEX_DATA *was_in_room;
  char *msg;
  int door;
  int vnum;

  vnum = 0;
  msg = "You hear $n's death cry.";
  if (IS_NPC(ch))
  {
    if (!IS_SET(ch->act, ACT_NO_BODY))
    {
      switch (number_bits(4))
      {
        case 0:
          msg = "$n hits the ground ... DEAD.";
          vnum = OBJ_VNUM_BLOOD;
          break;
        case 1:
          msg = "$n splatters blood on your armor.";
          vnum = OBJ_VNUM_BLOOD;
          break;
        case 2:
          if (IS_SET(ch->parts, PART_GUTS))
          {
            msg = "$n spills $s guts all over the floor.";
            vnum = OBJ_VNUM_GUTS;
          }
          break;
        case 3:
          if (IS_SET(ch->parts, PART_HEAD))
          {
            msg = "$n's severed head plops on the ground.";
            vnum = OBJ_VNUM_SEVERED_HEAD;
          }
          break;
        case 4:
          if (IS_SET(ch->parts, PART_HEART))
          {
            msg = "$n's heart is torn from $s chest.";
            vnum = OBJ_VNUM_TORN_HEART;
          }
          break;
        case 5:
          if (IS_SET(ch->parts, PART_ARMS))
          {
            msg = "$n's arm is sliced from $s dead body.";
            vnum = OBJ_VNUM_SLICED_ARM;
          }
          break;
        case 6:
          if (IS_SET(ch->parts, PART_LEGS))
          {
            msg = "$n's leg is sliced from $s dead body.";
            vnum = OBJ_VNUM_SLICED_LEG;
          }
          break;
        case 7:
          if (IS_SET(ch->parts, PART_BRAINS))
          {
            msg =
              "$n's head is shattered, and $s brains splash all over you.";
            vnum = OBJ_VNUM_BRAINS;
          }
          break;
        case 8:
          msg = "$n hits the ground ... DEAD.";
          vnum = OBJ_VNUM_BLOOD;
          break;
        case 9:
          msg = "$n hits the ground ... DEAD.";
          vnum = OBJ_VNUM_BLOOD;
      }
    }
  }
  else if (ch->level > 19)
  {
    switch (number_bits(4))
    {
      case 0:
        msg = "$n hits the ground ... DEAD.";
        vnum = OBJ_VNUM_BLOOD;
        break;
      case 1:
        msg = "$n splatters blood on your armor.";
        vnum = OBJ_VNUM_BLOOD;
        break;
      case 2:
        if (IS_SET(ch->parts, PART_GUTS))
        {
          msg = "$n spills $s guts all over the floor.";
          vnum = OBJ_VNUM_GUTS;
        }
        break;
      case 3:
        if (IS_SET(ch->parts, PART_HEAD))
        {
          msg = "$n's severed head plops on the ground.";
          vnum = OBJ_VNUM_SEVERED_HEAD;
        }
        break;
      case 4:
        if (IS_SET(ch->parts, PART_HEART))
        {
          msg = "$n's heart is torn from $s chest.";
          vnum = OBJ_VNUM_TORN_HEART;
        }
        break;
      case 5:
        if (IS_SET(ch->parts, PART_ARMS))
        {
          msg = "$n's arm is sliced from $s dead body.";
          vnum = OBJ_VNUM_SLICED_ARM;
        }
        break;
      case 6:
        if (IS_SET(ch->parts, PART_LEGS))
        {
          msg = "$n's leg is sliced from $s dead body.";
          vnum = OBJ_VNUM_SLICED_LEG;
        }
        break;
      case 7:
        if (IS_SET(ch->parts, PART_BRAINS))
        {
          msg = "$n's head is shattered, and $s brains splash all over you.";
          vnum = OBJ_VNUM_BRAINS;
        }
        break;
      case 8:
        msg = "$n hits the ground ... DEAD.";
        vnum = OBJ_VNUM_BLOOD;
        break;
      case 9:
        msg = "$n hits the ground ... DEAD.";
        vnum = OBJ_VNUM_BLOOD;
        break;
      case 10:
        if (IS_SET(ch->parts, PART_HEAD))
        {
          msg = "$n's severed head plops on the ground.";
          vnum = OBJ_VNUM_SEVERED_HEAD;
        }
        break;
      case 11:
        if (IS_SET(ch->parts, PART_HEART))
        {
          msg = "$n's heart is torn from $s chest.";
          vnum = OBJ_VNUM_TORN_HEART;
        }
        break;
      case 12:
        if (IS_SET(ch->parts, PART_ARMS))
        {
          msg = "$n's arm is sliced from $s dead body.";
          vnum = OBJ_VNUM_SLICED_ARM;
        }
        break;
      case 13:
        if (IS_SET(ch->parts, PART_LEGS))
        {
          msg = "$n's leg is sliced from $s dead body.";
          vnum = OBJ_VNUM_SLICED_LEG;
        }
        break;
      case 14:
        if (IS_SET(ch->parts, PART_BRAINS))
        {
          msg = "$n's head is shattered, and $s brains splash all over you.";
          vnum = OBJ_VNUM_BRAINS;
        }
    }
  }

  act(msg, ch, NULL, NULL, TO_ROOM);

  if ((vnum == 0) && !IS_SET(ch->act, ACT_NO_BODY))
  {
    switch (number_bits(4))
    {
      case 0:
        vnum = 0;
        break;
      case 1:
        vnum = OBJ_VNUM_BLOOD;
        break;
      case 2:
        vnum = 0;
        break;
      case 3:
        vnum = OBJ_VNUM_BLOOD;
        break;
      case 4:
        vnum = 0;

        break;
      case 5:
        vnum = OBJ_VNUM_BLOOD;
        break;
      case 6:
        vnum = 0;
        break;
      case 7:
        vnum = OBJ_VNUM_BLOOD;
    }
  }

  if (vnum != 0)
  {
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    char *name;

    name = IS_NPC(ch) ? ch->short_descr : ch->name;
    obj = create_object(get_obj_index(vnum));
    obj->timer = number_range(4, 7);

    if (!IS_NPC(ch))
    {
      obj->timer = number_range(12, 18);
    }
    if (vnum == OBJ_VNUM_BLOOD)
    {
      obj->timer = number_range(1, 4);
    }

    sprintf(buf, obj->short_descr, name);
    free_string(obj->short_descr);
    obj->short_descr = str_dup(buf);

    sprintf(buf, obj->description, name);
    free_string(obj->description);
    obj->description = str_dup(buf);

    sprintf(buf, obj->name, name);
    free_string(obj->name);
    obj->name = str_dup(buf);

    if (obj->item_type == ITEM_FOOD)
    {
      if (IS_SET(ch->form, FORM_POISON))
        obj->value[3] = 1;
      else if (!IS_SET(ch->form, FORM_EDIBLE))
        obj->item_type = ITEM_TRASH;
    }

    if (IS_NPC(ch))
    {
      obj->value[4] = 0;
    }
    else
    {
      obj->value[4] = 1;
    }

    obj_to_room(obj, ch->in_room);
  }

  if (IS_NPC(ch))
    msg = "You hear something's death cry.";
  else
    msg = "You hear someone's death cry.";

  was_in_room = ch->in_room;
  for (door = 0; door < MAX_DIR; door++)
  {
    EXIT_DATA *pexit;

    if ((pexit = was_in_room->exit[door]) != NULL &&
        pexit->u1.to_room != NULL && pexit->u1.to_room != was_in_room)
    {
      ch->in_room = pexit->u1.to_room;
      act(msg, ch, NULL, NULL, TO_ROOM);
    }
  }
  ch->in_room = was_in_room;

  return;
}

extern bool castkill;

void raw_kill(CHAR_DATA * victim, CHAR_DATA * killer, bool * mobdeath)
{
  int i;

  OBJ_DATA *tattoo;
  OBJ_DATA *ctattoo;

  death_cry(victim);
  stop_fighting(victim, true);

  if ((tattoo = get_eq_char(victim, WEAR_TATTOO)) != NULL)  /* keep
                                                               tattoo */
    obj_from_char(tattoo);
  if ((ctattoo = get_eq_char(victim, WEAR_CTATTOO)) != NULL)  /* keep 
                                                                 tattoo 
                                                               */
    obj_from_char(ctattoo);

  make_corpse(victim, killer);

  if (!IS_NPC(killer) && !IS_NPC(victim))
  {
    victim->pcdata->pkdeaths++;
    killer->pcdata->pkkills++;
  }

  if (IS_NPC(victim))
  {
    victim->pIndexData->killed++;
    kill_table[URANGE(0, victim->level, MAX_LEVEL - 1)].killed++;
    extract_char(victim, true);
    victim = NULL;
    *mobdeath = true;
    castkill = true;
    return;
  }

  if (!IS_NPC(victim))
  {
    extract_char(victim, false);
  }
  while (victim->affected)
    affect_remove(victim, victim->affected);
  victim->affected_by = race_table[victim->race].aff;
  victim->shielded_by = race_table[victim->race].shd;
  for (i = 0; i < 4; i++)
    victim->armor[i] = 100;
  victim->position = POS_RESTING;
  victim->hit = UMAX(1, victim->hit);
  victim->mana = UMAX(1, victim->mana);
  victim->move = UMAX(1, victim->move);
  if (tattoo != NULL)
  {
    obj_to_char(tattoo, victim);
    equip_char(victim, tattoo, WEAR_TATTOO);
  }
  if (ctattoo != NULL)
  {
    obj_to_char(ctattoo, victim);
    equip_char(victim, tattoo, WEAR_CTATTOO);
  }
  /*  save_char_obj( victim ); we're stable enough to not need this :) */

  return;
}

void group_gain(CHAR_DATA * ch, CHAR_DATA * victim)
{
  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  CHAR_DATA *gch;
  AFFECT_DATA *paf;
  CHAR_DATA *lch;
  int xp;
  int members;
  int group_levels;
  bool using_levelup_pill;

  using_levelup_pill = false;

  /* 
   * Monsters don't get kill xp's or alignment changes.
   * P-killing doesn't help either.
   * Dying of mortal wounds or poison doesn't give xp to anyone!
   */
  if (victim == ch)
    return;

  members = 0;
  group_levels = 0;
  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
  {
    if (is_same_group(gch, ch))
    {
      members++;
      group_levels += IS_NPC(gch) ? gch->level / 2 : gch->level;
    }
  }

  if (members == 0)
  {
    bug("Group_gain: members.", members);
    members = 1;
    group_levels = ch->level;
  }

  lch = (ch->leader != NULL) ? ch->leader : ch;

  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
  {
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if (!is_same_group(gch, ch) || IS_NPC(gch))
      continue;

    if (gch->level - lch->level >= 15)
    {
      send_to_char("You are too high for this group.\n\r", gch);
      continue;
    }

    if (gch->level - lch->level <= -15)
    {
      send_to_char("You are too low for this group.\n\r", gch);
      continue;
    }

    xp = xp_compute(gch, victim, group_levels);

    if (gch->affected)
    {
      for (paf = gch->affected; paf != NULL; paf = paf->next)
      {

        if (paf->type && skill_table[paf->type].name &&
            str_cmp(skill_table[paf->type].name, "4x") == 0)
        {
          xp = xp * paf->modifier;
          using_levelup_pill = true;
          sprintf(buf2, "{MYou recieve {C%d{Wx {MExperience!{x\n\r",
                  paf->modifier);
          send_to_char(buf2, gch);
        }
      }
    }

    if ((double_exp || happy_hour) && !using_levelup_pill)
    {
      xp *= 2;

      if (xp > 2125)
      {
        xp = (number_range(2125, 2225));
      }
      sprintf(buf,
              "{BYou receive {W%d{B experience points. {y(Double xp) {x\n\r\n\r",
              xp);
      send_to_char(buf, gch);

    }
    else
    {
      if (xp > 2125)
      {
        xp = (number_range(2125, 2225));
      }
      sprintf(buf, "{BYou receive {W%d{B experience points.{x\n\r\n\r", xp);
      send_to_char(buf, gch);
    }

    if (IS_QUESTOR(ch) && IS_NPC(victim))
    {
      if (ch->pcdata->questmob == victim->pIndexData->vnum)
      {
        send_to_char
          ("{RYou have almost completed your {GQ{gU{GE{gS{GT{Y!{x\n\r", ch);
        send_to_char
          ("{RReturn to the {Bq{bu{Be{bs{Bt{bm{Ba{bs{Bt{be{Br{R before your time runs out{Y!{x\n\r",
           ch);
        ch->pcdata->questmob = -1;
      }
    }
    gain_exp(gch, xp);

    for (obj = gch->carrying; obj != NULL; obj = obj_next)
    {
      obj_next = obj->next_content;
      if (obj->wear_loc == WEAR_NONE)
        continue;

      if ((IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(gch)) ||
          (IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(gch)) ||
          (IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(gch)))
      {
        act("{cYou are {Wzapped{c by $p.{x", gch, obj, NULL, TO_CHAR);
        act("$n is {Wzapped{x by $p.", gch, obj, NULL, TO_ROOM);
        obj_from_char(obj);
        obj_to_room(obj, gch->in_room);
      }
    }
  }

  return;
}

/*
 * Compute xp for a kill.
 * Also adjust alignment of killer.
 * Edit this function to change xp computations.
 */
int xp_compute(CHAR_DATA * gch, CHAR_DATA * victim, int total_levels)
{
  int xp, xp90, base_exp;
  int align, level_range;
  int change;
  float bonus = 0.0;

  level_range = victim->level - gch->level;

  if (!IS_NPC(gch) && !IS_NPC(victim))
  {
    xp = 1;
    return xp;
  }

  /* compute the base exp */
  switch (level_range)
  {
    default:
      base_exp = 0;
      break;
    case -9:
      base_exp = 1;
      break;
    case -8:
      base_exp = 2;
      break;
    case -7:
      base_exp = 5;
      break;
    case -6:
      base_exp = 9;
      break;
    case -5:
      base_exp = 11;
      break;
    case -4:
      base_exp = 22;

      break;
    case -3:
      base_exp = 33;
      break;
    case -2:
      base_exp = 50;
      break;
    case -1:
      base_exp = 66;
      break;
    case 0:
      base_exp = 83;
      break;
    case 1:
      base_exp = 95;
      break;
    case 2:
      base_exp = 110;
      break;
    case 3:
      base_exp = 120;
      break;
    case 4:
      base_exp = 140;

      break;
  }

  if (level_range > 4)
    base_exp = 160 + 20 * (level_range - 4);

  if (IS_NPC(victim))           /* at max a mob with all worth 5 level above
                                   his own */
  {
    if (is_affected(victim, skill_lookup("sanctuary")))
      bonus += (base_exp * .30);
    if (is_affected(victim, skill_lookup("haste")))
      if (IS_SET(victim->off_flags, OFF_AREA_ATTACK))
        bonus += (base_exp * .20);
    if (IS_SET(victim->off_flags, OFF_BACKSTAB))
      bonus += (base_exp * .20);
    if (IS_SET(victim->off_flags, OFF_FAST))
      bonus += (base_exp * .20);
    if (IS_SET(victim->off_flags, OFF_DODGE))
      bonus += (base_exp * .10);
    if (IS_SET(victim->off_flags, OFF_PARRY))
      bonus += (base_exp * .10);

    if (victim->spec_fun != 0)
    {
      if (!str_cmp(spec_name(victim->spec_fun), "spec_breath_any")
          || !str_cmp(spec_name(victim->spec_fun),
                      "spec_breath_acid") ||
          !str_cmp(spec_name(victim->spec_fun), "spec_breath_fire")
          || !str_cmp(spec_name(victim->spec_fun),
                      "spec_breath_frost") ||
          !str_cmp(spec_name(victim->spec_fun), "spec_breath_gas")
          || !str_cmp(spec_name(victim->spec_fun), "spec_breath_lightning"))
        bonus += (base_exp * .25);

      else if (!str_cmp
               (spec_name(victim->spec_fun), "spec_cast_cleric") ||
               !str_cmp(spec_name(victim->spec_fun),
                        "spec_cast_mage") ||
               !str_cmp(spec_name(victim->spec_fun), "spec_cast_undead"))
        bonus += (base_exp * .20);

      else if (!str_cmp(spec_name(victim->spec_fun), "spec_poison"))
        bonus += (base_exp * .10);
    }
  }
  /* back to normal code -------------------- */

  /* do alignment computations */

  align = victim->alignment - gch->alignment;

  if (IS_SET(victim->act, ACT_NOALIGN))
  {
    /* no change */
  }

  else if (align > 500)         /* monster is more good than slayer */
  {
    change = (align - 500) * base_exp / 500 * gch->level / total_levels;
    change = UMAX(1, change);
    gch->alignment = UMAX(-1000, gch->alignment - change);
    if (gch->pet != NULL)
      gch->pet->alignment = gch->alignment;
  }

  else if (align < -500)        /* monster is more evil than slayer */
  {
    change = (-1 * align - 500) * base_exp / 500 * gch->level / total_levels;
    change = UMAX(1, change);
    gch->alignment = UMIN(1000, gch->alignment + change);
    if (gch->pet != NULL)
      gch->pet->alignment = gch->alignment;
  }

  else
    /* improve this someday */
  {
    change = gch->alignment * base_exp / 500 * gch->level / total_levels;
    gch->alignment -= change;
    if (gch->pet != NULL)
      gch->pet->alignment = gch->alignment;
  }

  /* calculate exp multiplier for align */
  if (IS_SET(victim->act, ACT_NOALIGN))
    xp = base_exp + bonus;

  else if (gch->alignment > 500)  /* for goodie two shoes */
  {
    if (victim->alignment < -750)
      xp = (base_exp * 4) / 3;

    else if (victim->alignment < -500)
      xp = (base_exp * 5) / 4;

    else if (victim->alignment > 750)
      xp = base_exp / 4;

    else if (victim->alignment > 500)
      xp = base_exp / 2;

    else if (victim->alignment > 250)
      xp = (base_exp * 3) / 4;

    else
      xp = base_exp;
  }

  else if (gch->alignment < -500) /* for baddies */
  {
    if (victim->alignment > 750)
      xp = (base_exp * 5) / 4;

    else if (victim->alignment > 500)
      xp = (base_exp * 11) / 10;

    else if (victim->alignment < -750)
      xp = base_exp / 2;

    else if (victim->alignment < -500)
      xp = (base_exp * 3) / 4;

    else if (victim->alignment < -250)
      xp = (base_exp * 9) / 10;

    else
      xp = base_exp;
  }

  else if (gch->alignment > 200)  /* a little good */
  {

    if (victim->alignment < -500)
      xp = (base_exp * 6) / 5;

    else if (victim->alignment > 750)
      xp = base_exp / 2;

    else if (victim->alignment > 0)
      xp = (base_exp * 3) / 4;

    else
      xp = base_exp;
  }

  else if (gch->alignment < -200) /* a little bad */
  {
    if (victim->alignment > 500)
      xp = (base_exp * 6) / 5;

    else if (victim->alignment < -750)
      xp = base_exp / 2;

    else if (victim->alignment < 0)
      xp = (base_exp * 3) / 4;

    else
      xp = base_exp;
  }

  else
    /* neutral */
  {

    if (victim->alignment > 500 || victim->alignment < -500)
      xp = (base_exp * 4) / 3;

    else if (victim->alignment < 200 && victim->alignment > -200)
      xp = base_exp / 2;

    else
      xp = base_exp;
  }

  xp = xp + bonus;

  if (gch->level > 99)
    xp = 90 * xp / gch->level;
  /* randomize the rewards */
  xp = number_range(xp * 8 / 5, xp * 9 / 5);

  xp90 = 90 * xp / 100;         /* 90% limit for one grouped player */

  /* adjust for grouping */
  xp = xp * gch->level / (UMAX(1, total_levels - 1));

  /* modified for groups of more than 1 -- Elrac */
  if (gch->level != total_levels)
  {
    xp = 120 * xp / 100;        /* everyone gets a 20% bonus for grouping */
    if (xp > xp90)
      xp = xp90;                /* but no more than the single grouped player
                                   limit */
  }

  while (xp > 2225)
  {
    xp = xp * .95;
  }

  return xp;
}

void dam_message(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
                 bool immune, int verbose, bool singular)
{
  char buf1[256], buf2[256], buf3[256];
  char pbuf[MSL];
  const char *vs;
  const char *vp;
  const char *attack;
  char punct;
  float painper = 0.00;
  float fdam = 0.00;

  fdam = dam;

  if (victim->max_hit > 0 && dam > 0)
    painper = ((fdam * 100) / victim->max_hit);
  else
    painper = 0;
  sprintf(pbuf, "{D[{R%c%.2f{D]", '%', painper);

  if (ch == NULL || victim == NULL)
    return;

  if (dam == 0)
  {
    vs = "{bmiss{x";
    vp = "{bmisses{x";
  }
  else if (painper <= 1)
  {
    vs = "{Wwound{x";
    vp = "{Wwounds{x";
  }
  else if (painper <= 2)
  {
    vs = "{Wmaul{x";
    vp = "{Wmauls{x";
  }
  else if (painper <= 3)
  {
    vs = "{Wdecimate{x";
    vp = "{Wdecimates{x";
  }
  else if (painper <= 4)
  {
    vs = "{Wdevastate{x";
    vp = "{Wdevastates{x";
  }
  else if (painper <= 5)
  {
    vs = "{ymaim{x";
    vp = "{ymaims{x";
  }
  else if (painper <= 10)
  {
    vs = "{DMUTILATE{x";
    vp = "{DMUTILATES{x";
  }
  else if (painper <= 15)
  {
    vs = "{mDISEMBOWEL{x";
    vp = "{mDISEMBOWELS{x";
  }
  else if (painper <= 20)
  {
    vs = "{gDISMEMBER{x";
    vp = "{gDISMEMBERS{x";
  }
  else if (painper <= 25)
  {
    vs = "{rMASSACRE{x";
    vp = "{rMASSACRES{x";
  }
  else if (painper <= 30)
  {
    vs = "{GMANGLE{x";
    vp = "{GMANGLES{x";
  }
  else if (painper <= 35)
  {
    vs = "{Y***{MDEMOLISH{Y***{x";
    vp = "{Y***{MDEMOLISHES{Y***{x";
  }
  else if (painper <= 40)
  {
    vs = "{Y***{rDEVASTATE{x{Y***{x";
    vp = "{Y***{rDEVASTATES{x{Y***{x";
  }
  else if (painper <= 45)
  {
    vs = "{Y==={x{MOBLITERATE{Y==={x{x";
    vp = "{Y==={x{MOBLITERATES{Y==={x{x";
  }
  else if (painper <= 50)
  {
    vs = "{Y>{R*{x{Y>{R*{x{Y>{MANNIHILATE{Y<{R*{x{Y<{R*{x{Y<{x";
    vp = "{Y>{R*{x{Y>{R*{x{Y>{MANNIHILATES{Y<{R*{x{Y<{R*{x{Y<{x";
  }
  else if (painper <= 55)
  {
    vs = "{R<>{W<>{B<>{Y ErAdIcAtPE{x{x{B<>{W<>{R<>{x";
    vp = "{R<>{W<>{B<>{Y ErAdIcAtEs{B{x <>{W<>{R<>{x";
  }
  else if (painper <= 60)
  {
    vs =
      "{Ddo {r*{R*{Y>{GU{gn{GS{gp{GE{ga{GK{ga{GB{gl{GE{Y<{R*{r*{D things to{x";
    vp =
      "{Ddoes {r*{R*{Y>{GU{gn{GS{gp{GE{ga{GK{ga{GB{gl{GE{Y<{R*{r* {Dthings to{x";
  }
  else if (painper <= 65)
  {
    vs = "{Y*{B*{Y*{WV{Ra{Wp{Ro{Wr{Ri{Wz{x{Re{x{Y*{B*{Y*";
    vp = "{Y*{B*{Y*{WV{Ra{Wp{Ro{Wr{Ri{Wz{Re{x{Ws{x{Y*{B*{Y*{x";
  }
  else if (painper <= 70)
  {
    vs = "{B>{G>{x{RE{Yv{Ri{Yc{Re{Yr{Ra{Yt{Re{x{G<{x{B<";
    vp = "{B>{G>{x{RE{Yv{Ri{Yc{Re{Yr{Ra{Yt{Re{Ys{G<{x{B<{x";
  }
  else if (painper <= 75)
  {
    vs = "{B({R*{B){rD{De{rs{Dt{rr{Do{r{B({R*{B){x";
    vp = "{B({R*{B){rD{De{rs{Dt{rr{Do{ry{Ds{B({R*{B){x";
  }
  else if (painper <= 80)
  {
    vs = "{r^{b^{r^{yT{Re{ya{Rr{ys {RI{yn{Rt{yo{r^{b^{r^{x";
    vp = "{r^{b^{r^{yT{Re{ya{Rr{ys {RI{yn{Rt{yo{r^{b^{r^{x";
  }
  else if (painper <= 85)
  {
    vs = "{B!{r*{B!{RR{Di{Rp{Ds {RO{Dp{{Re{Dn{B!{r*{B!{x";
    vp = "{B!{r*{B!{RR{Di{Rp{Ds {RO{Dp{Re{Dn{B!{r*{B!{x";
  }
  else if (painper <= 90)
  {
    vs =
      "{R+{x{Y+{R+{x{Y+{BE{cx{Bt{ce{Br{cm{Bi{cn{Ba{ct{Be{cs{Y+{R+{x{Y+{R+{x{x";
    vp =
      "{R+{x{Y+{R+{X{Y+{BE{cx{Bt{ce{Br{cm{Bi{cn{Ba{ct{Be{cs{Y+{R+{x{Y+{R+{x{x";
  }
  else if (painper <= 95)
  {
    vs = "{W**{R**{Ddo {WG{co{WD{cl{WI{ck{We {Ddamage to{R**{W**{x";
    vp = "{W**{R**{Ddoes {WG{co{WD{cl{WI{ck{We {Ddamage to{R**{W**{x";
  }
  else if (painper <= 99)
  {
    vs =
      "{R!!{x{Y!!{WT{GO{WT{GA{WL{GL{WY {BF{CU{BC{CK{BS{x {WU{GP{Y!!{R!!{x{x";
    vp =
      "{R!!{x{Y!!{WT{GO{WT{GA{WL{GL{WY {BF{CU{BC{CK{BS{x {WU{GP{Y!!{R!!{x{x";
  }
  else
  {
    vs =
      "{D..{w--{r=={R**{W( {Y!! {OI {ON {OS {OT {OA {ON {OT {OL {OY   {OK {OI {OL {OL {OS {Y!! {W){R**{r=={w--{D..{x";
    vp =
      "{D..{w--{r=={R**{W( {Y!! {OI {ON {OS {OT {OA {ON {OT {OL {OY   {OK {OI {OL {OL {OS {Y!! {W){R**{r=={w--{D..{x";
  }

  punct = (dam <= 24) ? '.' : '!';

  if (dt == TYPE_HIT)
  {
    if (ch == victim)
    {
      sprintf(buf1, "$n %s $melf for {G*{W%d{G*{x damage%c %s{x", vp,
              dam, punct, pbuf);
      sprintf(buf2,
              "You %s yourself for {G*{W%d{G*{x damage%c {D[{R%%d{D] %s{x",
              vs, dam, punct, pbuf);
    }
    else
    {
      sprintf(buf1, "$n %s $N for {g>{G>{Y%d{G<{g<{x damage%c %s{x", vp,
              dam, punct, pbuf);
      sprintf(buf2, "You %s $N for {g>{G>{B%d{G<{g<{x damage%c %s{x",
              vs, dam, punct, pbuf);
      sprintf(buf3, "$n %s you for {w>{W>{R%d{W<{w<{x damage%c %s{x",
              vp, dam, punct, pbuf);
    }
  }
  else
  {
    if (dt >= 0 && dt < MAX_SKILL)
      attack = skill_table[dt].noun_damage;
    else if (dt >= TYPE_HIT && dt < TYPE_HIT + MAX_DAMAGE_MESSAGE)
      attack = attack_table[dt - TYPE_HIT].noun;
    else if (dt == -1 && verbose == VERBOSE_SHIELD_COMP)
    {
      if (singular)
        attack = "shield";
      else
        attack = "shields";
    }
    else
    {
      bug("Dam_message: bad dt %d.", dt);
      dt = TYPE_HIT;
      attack = attack_table[0].name;
    }
    if (immune)
    {
      if (ch == victim)
      {
        sprintf(buf1, "$n is unaffected by $s own %s.{x", attack);
        sprintf(buf2, "Luckily, you are immune to that.{x");
      }
      else
      {
        sprintf(buf1, "$N is unaffected by $n's %s!{x", attack);
        sprintf(buf2, "$N is unaffected by your %s!{x", attack);
        sprintf(buf3, "$n's %s is powerless against you.{x", attack);
      }
    }
    else
    {
      if (ch == victim)
      {
        sprintf(buf1,
                "$n's %s %s $m for {g>{G>{W%d{G<{g<{x damage%c %s{x",
                attack, vp, dam, punct, pbuf);
        sprintf(buf2,
                "Your %s %s you for {w>{W>{R%d{W<{w<{x damage%c %s{x",
                attack, vp, dam, punct, pbuf);
      }
      else
      {
        sprintf(buf1,
                "$n's %s %s $N for {g>{G>{Y%d{G<{g<{x damage%c %s{x",
                attack, vp, dam, punct, pbuf);
        sprintf(buf2,
                "Your %s %s $N for {g>{G>{W%d{G<{g<{x damage%c %s{x",
                attack, vp, dam, punct, pbuf);
        sprintf(buf3,
                "$n's %s %s you for {w>{W>{R%d{W<{w<{x damage%c %s{x",
                attack, vp, dam, punct, pbuf);
      }
    }
  }

  if (ch == victim)
  {
    xact_new(buf1, ch, NULL, NULL, TO_ROOM, POS_RESTING, verbose);
    xact_new(buf2, ch, NULL, NULL, TO_CHAR, POS_RESTING, verbose);
  }
  else
  {
    xact_new(buf1, ch, NULL, victim, TO_NOTVICT, POS_RESTING, verbose);
    if (!IS_NPC(ch))
      if (!IS_SET(ch->pcdata->verbose, VERBOSE_DAMAGE))
        xact_new(buf2, ch, NULL, victim, TO_CHAR, POS_RESTING, verbose);
    if (!IS_NPC(victim))
      if (!IS_SET(victim->pcdata->verbose, VERBOSE_DAMAGE))
        xact_new(buf3, ch, NULL, victim, TO_VICT, POS_RESTING, verbose);
  }
  if (dt == -1 && verbose == VERBOSE_SHIELD_COMP)

    return;
}

/*
 * Disarm a creature.
 * Caller must check for successful attack.
 */
void disarm(CHAR_DATA * ch, CHAR_DATA * victim)
{
  OBJ_DATA *obj;

  if ((obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
    return;

  if (nia(ch))
    return;

  if (IS_OBJ_STAT(obj, ITEM_NOREMOVE))
  {
    act("$S weapon won't budge!{x", ch, NULL, victim, TO_CHAR);
    act("$n tries to disarm you, but your weapon won't budge!{x", ch,
        NULL, victim, TO_VICT);
    act("{k$n tries to disarm $N, but fails.{x", ch, NULL, victim,
        TO_NOTVICT);
    return;
  }

  act("{W$n {YDISARMS{W you and sends your weapon flying!{x", ch, NULL,
      victim, TO_VICT);
  act("{YYou disarm $N!{x", ch, NULL, victim, TO_CHAR);
  act("{Y$n disarms $N!{x", ch, NULL, victim, TO_NOTVICT);

  obj_from_char(obj);
  if (IS_OBJ_STAT(obj, ITEM_NODROP) || IS_OBJ_STAT(obj, ITEM_INVENTORY))
    obj_to_char(obj, victim);
  else
  {
    obj_to_room(obj, victim->in_room);
    if (IS_NPC(victim) && victim->wait == 0 && can_see_obj(victim, obj))
      get_obj(victim, obj, NULL, false);
  }

  return;
}

CH_CMD(do_berserk)
{
  int chance, hp_percent;

  chance = get_skill(ch, gsn_berserk);

  if ((chance == 0) || (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_BERSERK)) ||
      (!IS_NPC(ch) && (chance < 2)
       && ch->level < skill_table[gsn_berserk].skill_level[ch->class]))
  {
    send_to_char
      ("{hYou turn {rred{h in the face, but nothing happens.{x\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_BERSERK) || is_affected(ch, gsn_berserk) ||
      is_affected(ch, skill_lookup("frenzy")))
  {
    send_to_char("{hYou get a little madder.{x\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CALM))
  {
    send_to_char("{hYou're feeling too mellow to berserk.{x\n\r", ch);
    return;
  }

  if (ch->mana < 50)
  {
    send_to_char("{hYou can't get up enough energy.{x\n\r", ch);
    return;
  }

  /* modifiers */

  /* fighting */
  if (ch->position == POS_FIGHTING)
    chance += 10;

  /* damage -- below 50% of hp helps, above hurts */
  hp_percent = 100 * ch->hit / ch->max_hit;
  chance += 25 - hp_percent / 2;

  if (number_percent() < chance)
  {
    AFFECT_DATA af;

    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, PULSE_VIOLENCE);
    ch->mana -= 50;
    ch->move /= 2;

    /* heal a little damage */
    ch->hit += ch->level * 2;
    ch->hit = UMIN(ch->hit, ch->max_hit);

    send_to_char
      ("{hYour pulse races as you are consumed by {rrage!{x\n\r", ch);
    act("{k$n gets a {cw{gi{rl{yd{k look in $s eyes.{x", ch, NULL, NULL,
        TO_ROOM);
    check_improve(ch, gsn_berserk, true, 2);

    af.where = TO_AFFECTS;
    af.type = gsn_berserk;
    af.level = ch->level;
    af.duration = number_fuzzy(ch->level / 8);
    af.modifier = UMAX(1, ch->level / 5);
    af.bitvector = AFF_BERSERK;

    af.location = APPLY_HITROLL;
    affect_to_char(ch, &af);

    af.location = APPLY_DAMROLL;
    affect_to_char(ch, &af);

    af.modifier = UMAX(10, 10 * (ch->level / 5));
    af.location = APPLY_AC;
    affect_to_char(ch, &af);
  }

  else
  {
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, 3 * PULSE_VIOLENCE);
    ch->mana -= 25;
    ch->move /= 2;

    send_to_char("{hYour pulse speeds up, but nothing happens.{x\n\r", ch);
    check_improve(ch, gsn_berserk, false, 2);
  }
}

CH_CMD(do_voodoo)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *doll;

  if (IS_NPC(ch))
    return;

  doll = get_eq_char(ch, WEAR_HOLD);
  if (doll == NULL || (doll->pIndexData->vnum != OBJ_VNUM_VOODOO))
  {
    send_to_char("You are not holding a voodoo doll.\n\r", ch);
    return;
  }

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Syntax: voodoo <action>\n\r", ch);
    send_to_char("Actions: pin trip throw\n\r", ch);
    return;
  }

  if (!str_cmp(arg, "pin"))
  {
    do_vdpi(ch, doll->name);
    return;
  }

  if (!str_cmp(arg, "trip"))
  {
    do_vdtr(ch, doll->name);
    return;
  }

  if (!str_cmp(arg, "throw"))
  {
    do_vdth(ch, doll->name);
    return;
  }

  do_voodoo(ch, "");
}

CH_CMD(do_vdpi)
{
  char arg1[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *d;
  AFFECT_DATA af;
  bool found = false;

  argument = one_argument(argument, arg1);

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *wch;

    if (d->connected != CON_PLAYING || !can_see(ch, d->character))
      continue;

    wch = (d->original != NULL) ? d->original : d->character;

    if (!can_see(ch, wch))
      continue;

    if (!str_cmp(arg1, wch->name) && !found)
    {
      if (IS_NPC(wch))
        continue;

      if (IS_IMMORTAL(wch) && (wch->level > ch->level))
      {
        send_to_char("That's not a good idea.\n\r", ch);
        return;
      }

      if ((wch->level < 20) && !IS_IMMORTAL(ch))
      {
        send_to_char("They are a little too young for that.\n\r", ch);
        return;
      }

      if (IS_SHIELDED(wch, SHD_PROTECT_VOODOO))
      {
        send_to_char
          ("They are still realing from a previous voodoo.\n\r", ch);
        return;
      }

      found = true;

      send_to_char("You stick a pin into your voodoo doll.\n\r", ch);
      act("$n sticks a pin into a voodoo doll.", ch, NULL, NULL, TO_ROOM);
      send_to_char
        ("{RYou double over with a sudden pain in your gut!{x\n\r", wch);
      act("$n suddenly doubles over with a look of extreme pain!", wch,
          NULL, NULL, TO_ROOM);
      af.where = TO_SHIELDS;
      af.type = skill_lookup("protection voodoo");
      af.level = wch->level;
      af.duration = 1;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector = SHD_PROTECT_VOODOO;
      affect_to_char(wch, &af);
      return;
    }
  }
  send_to_char("Your victim doesn't seem to be in the realm.\n\r", ch);
  return;
}

CH_CMD(do_vdtr)
{
  char arg1[MAX_INPUT_LENGTH];
  DESCRIPTOR_DATA *d;
  AFFECT_DATA af;
  bool found = false;

  argument = one_argument(argument, arg1);

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *wch;

    if (d->connected != CON_PLAYING || !can_see(ch, d->character))
      continue;

    wch = (d->original != NULL) ? d->original : d->character;

    if (!can_see(ch, wch))
      continue;

    if (!str_cmp(arg1, wch->name) && !found)
    {
      if (IS_NPC(wch))
        continue;

      if (IS_IMMORTAL(wch) && (wch->level > ch->level))
      {
        send_to_char("That's not a good idea.\n\r", ch);
        return;
      }

      if ((wch->level < 20) && !IS_IMMORTAL(ch))
      {
        send_to_char("They are a little too young for that.\n\r", ch);
        return;
      }

      if (IS_SHIELDED(wch, SHD_PROTECT_VOODOO))
      {
        send_to_char
          ("They are still realing from a previous voodoo.\n\r", ch);
        return;
      }

      found = true;

      send_to_char("You slam your voodoo doll against the ground.\n\r", ch);
      act("$n slams a voodoo doll against the ground.", ch, NULL, NULL,
          TO_ROOM);
      send_to_char("{RYour feet slide out from under you!{x\n\r", wch);
      send_to_char("{RYou hit the ground face first!{x\n\r", wch);
      act("$n trips over $s own feet, and does a nose dive into the ground!",
          wch, NULL, NULL, TO_ROOM);
      af.where = TO_SHIELDS;
      af.type = skill_lookup("protection voodoo");
      af.level = wch->level;
      af.duration = 1;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector = SHD_PROTECT_VOODOO;
      affect_to_char(wch, &af);
      return;
    }
  }
  send_to_char("Your victim doesn't seem to be in the realm.\n\r", ch);
  return;
}

CH_CMD(do_vdth)
{
  char arg1[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  DESCRIPTOR_DATA *d;
  AFFECT_DATA af;
  ROOM_INDEX_DATA *was_in;
  ROOM_INDEX_DATA *now_in;
  bool found = false;
  int attempt;

  argument = one_argument(argument, arg1);

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    CHAR_DATA *wch;

    if (d->connected != CON_PLAYING || !can_see(ch, d->character))
      continue;

    wch = (d->original != NULL) ? d->original : d->character;

    if (!can_see(ch, wch))
      continue;

    if (!str_cmp(arg1, wch->name) && !found)
    {
      if (IS_NPC(wch))
        continue;

      if (IS_IMMORTAL(wch) && (wch->level > ch->level))
      {
        send_to_char("That's not a good idea.\n\r", ch);
        return;
      }

      if ((wch->level < 20) && !IS_IMMORTAL(ch))
      {
        send_to_char("They are a little too young for that.\n\r", ch);
        return;
      }

      if (IS_SHIELDED(wch, SHD_PROTECT_VOODOO))
      {
        send_to_char
          ("They are still realing from a previous voodoo.\n\r", ch);
        return;
      }

      found = true;

      send_to_char("You toss your voodoo doll into the air.\n\r", ch);
      act("$n tosses a voodoo doll into the air.", ch, NULL, NULL, TO_ROOM);
      af.where = TO_SHIELDS;
      af.type = skill_lookup("protection voodoo");
      af.level = wch->level;
      af.duration = 1;
      af.location = APPLY_NONE;
      af.modifier = 0;
      af.bitvector = SHD_PROTECT_VOODOO;
      affect_to_char(wch, &af);
      if ((wch->fighting != NULL) || (number_percent() < 25))
      {
        send_to_char
          ("{RA sudden gust of wind throws you through the air!{x\n\r", wch);
        send_to_char
          ("{RYou slam face first into the nearest wall!{x\n\r", wch);
        act("A sudden gust of wind picks up $n and throws $m into a wall!",
            wch, NULL, NULL, TO_ROOM);
        return;
      }
      wch->position = POS_STANDING;
      was_in = wch->in_room;
      for (attempt = 0; attempt < MAX_DIR; attempt++)
      {
        EXIT_DATA *pexit;
        int door;

        door = number_door();
        if ((pexit = was_in->exit[door]) == 0 ||
            pexit->u1.to_room == NULL ||
            IS_SET(pexit->exit_info, EX_CLOSED) || (IS_NPC(wch)
                                                    &&
                                                    IS_SET
                                                    (pexit->u1.
                                                     to_room->
                                                     room_flags,
                                                     ROOM_NO_MOB)))
          continue;

        move_char(wch, door, false, true);
        if ((now_in = wch->in_room) == was_in)
          continue;

        wch->in_room = was_in;
        sprintf(buf,
                "A sudden gust of wind picks up $n and throws $m to the %s.",
                dir_name[door]);
        act(buf, wch, NULL, NULL, TO_ROOM);
        send_to_char
          ("{RA sudden gust of wind throws you through the air!{x\n\r", wch);
        wch->in_room = now_in;
        act("$n sails into the room and slams face first into a wall!", wch,
            NULL, NULL, TO_ROOM);
        do_look(wch, "auto");
        send_to_char
          ("{RYou slam face first into the nearest wall!{x\n\r", wch);
        return;
      }
      send_to_char
        ("{RA sudden gust of wind throws you through the air!{x\n\r", wch);
      send_to_char("{RYou slam face first into the nearest wall!{x\n\r", wch);
      act("A sudden gust of wind picks up $n and throws $m into a wall!", wch,
          NULL, NULL, TO_ROOM);
      return;
    }
  }
  send_to_char("Your victim doesn't seem to be in the realm.\n\r", ch);
  return;
}

CH_CMD(do_bash)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int chance;
  int schance;
  bool mobdeath = false;

  one_argument(argument, arg);

  chance = get_skill(ch, gsn_bash);

  if ((chance == 0) || (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_BASH)) ||
      (!IS_NPC(ch) && chance < 2 &&
       ch->level < skill_table[gsn_bash].skill_level[ch->class]))
  {
    send_to_char("Bashing? What's that?\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    victim = ch->fighting;
    if (victim == NULL)
    {
      send_to_char("But you aren't fighting anyone!\n\r", ch);
      return;
    }
  }

  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (victim->position < POS_FIGHTING)
  {
    act("You'll have to let $M get back up first.", ch, NULL, victim,
        TO_CHAR);
    return;
  }

  if (victim == ch)
  {
    send_to_char("You try to bash your brains out, but fail.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
  {
    act("But $N is your friend!", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (!can_see(ch, victim))
  {
    send_to_char
      ("You get a running start, and slam right into a wall.\n\r", ch);
    return;
  }

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  /* modifiers */

  /* size and weight */
  chance += ch->carry_weight / 250;
  chance -= victim->carry_weight / 200;

  /* stats *//* curbash */
  chance += (get_curr_stat(ch, STAT_STR) * 5) / 2;
  chance += (get_curr_stat(ch, STAT_DEX) * 3) / 2;
  chance -= (get_curr_stat(victim, STAT_DEX) * 3) / 2;
  chance += GET_AC(victim, AC_BASH) / 200;

  /* speed */
  if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
    chance += 10;

  if (IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE))
    chance -= 30;

  /* level */
  chance += (ch->level - victim->level);

  if (!IS_NPC(victim) && chance < get_skill(victim, gsn_dodge))
  {
    act("{i$n tries to bash you, but you dodge it.{x", ch, NULL, victim,
        TO_VICT);
    act("{h$N dodges your bash, you fall flat on your face.{x", ch, NULL,
        victim, TO_CHAR);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_bash].beats);
    return;
    chance -= 3 * (get_skill(victim, gsn_dodge) - chance);
  }

  /* now the attack */
  if (number_percent() < chance)
  {

    act("{i$n sends you sprawling with a powerful bash!{x", ch, NULL,
        victim, TO_VICT);
    act("{hYou slam into $N, and send $M flying!{x", ch, NULL, victim,
        TO_CHAR);
    act("{k$n sends $N sprawling with a powerful bash.{x", ch, NULL,
        victim, TO_NOTVICT);
    check_improve(ch, gsn_bash, true, 1);

    DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_bash].beats);
    victim->position = POS_RESTING;
    xdamage(ch, victim, number_range(2, 2 + 2 * ch->size + chance / 20),
            gsn_bash, DAM_BASH, false, VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;
    chance = (get_skill(ch, gsn_stun) / 4);
    schance = (number_percent());
    if (schance < chance)
    {
      if (schance <= 1)
      {
        victim->stunned = 4;
        act("You have been almost knocked out!", ch, NULL, victim, TO_VICT);
        act("$N is virtually knocked out by your bash!", ch, NULL, victim,
            TO_CHAR);
        act("$N has been virtually knocked out cold!", ch, NULL, victim,
            TO_NOTVICT);
      }
      else if (schance < 10)
      {
        victim->stunned = 3;
        act("You have extremely stunned by a powerful bash!", ch, NULL,
            victim, TO_VICT);
        act("$N has been extremely stunned by your bash!", ch, NULL, victim,
            TO_CHAR);
        act("$N is reeling with glazed eyes from a powerful bash!", ch, NULL,
            victim, TO_NOTVICT);
      }
      else
      {
        victim->stunned = 2;
        act("You have been lightly stunned.", ch, NULL, victim, TO_VICT);
        act("$N is lightly stunned by your bash.", ch, NULL, victim, TO_CHAR);
        act("$N looks a little stunned by the bash.", ch, NULL, victim,
            TO_NOTVICT);
      }
      check_improve(ch, gsn_stun, true, 1);
    }
  }
  else
  {
    act("{hYou fall flat on your face!{x", ch, NULL, victim, TO_CHAR);
    act("{k$n falls flat on $s face.{x", ch, NULL, victim, TO_NOTVICT);
    act("{iYou evade $n's bash, causing $m to fall flat on $s face.{x",
        ch, NULL, victim, TO_VICT);
    check_improve(ch, gsn_bash, false, 1);
    ch->position = POS_RESTING;
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_bash].beats * 3 / 2);
    xdamage(ch, victim, 0, gsn_bash, DAM_BASH, false, VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;
  }
}

CH_CMD(do_dirt)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int chance;
  bool mobdeath = false;

  one_argument(argument, arg);

  chance = get_skill(ch, gsn_dirt);

  if ((chance == 0) || (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_KICK_DIRT))
      || (!IS_NPC(ch) && chance < 2 &&
          ch->level < skill_table[gsn_dirt].skill_level[ch->class]))
  {
    send_to_char("{hYou get your feet dirty.{x\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    victim = ch->fighting;
    if (victim == NULL)
    {
      send_to_char("But you aren't in combat!\n\r", ch);
      return;
    }
  }

  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_BLIND))
  {
    act("{h$E's already been blinded.{x", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (victim == ch)
  {
    send_to_char("Very funny.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
  {
    act("But $N is such a good friend!", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  /* modifiers */

  /* dexterity */
  chance += get_curr_stat(ch, STAT_DEX);
  chance -= 2 * get_curr_stat(victim, STAT_DEX);

  /* speed */
  if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
    chance += 10;
  if (IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE))
    chance -= 25;

  /* level */
  chance += (ch->level - victim->level) * 2;

  /* sloppy hack to prevent false zeroes */
  if (chance % 5 == 0)
    chance += 1;

  /* terrain */

  switch (ch->in_room->sector_type)
  {
    case (SECT_INSIDE):
      chance -= 20;
      break;
    case (SECT_CITY):
      chance -= 10;
      break;
    case (SECT_FIELD):
      chance += 5;
      break;
    case (SECT_FOREST):
      break;
    case (SECT_HILLS):
      break;
    case (SECT_MOUNTAIN):
      chance -= 10;
      break;
    case (SECT_WATER_SWIM):
      chance = 0;
      break;
    case (SECT_WATER_NOSWIM):
      chance = 0;
      break;
    case (SECT_AIR):
      chance = 0;
      break;
    case (SECT_DESERT):
      chance += 10;
      break;
  }

  if (chance == 0)
  {
    send_to_char("{hThere isn't any dirt to kick.{x\n\r", ch);
    return;
  }

  /* now the attack */
  if (number_percent() < chance)
  {
    AFFECT_DATA af;

    act("{k$n is blinded by the dirt in $s eyes!{x", victim, NULL, NULL,
        TO_ROOM);
    act("{i$n kicks dirt in your eyes!{x", ch, NULL, victim, TO_VICT);
    xdamage(ch, victim, number_range(2, 5), gsn_dirt, DAM_NONE, false,
            VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;
    send_to_char("{DYou can't see a thing!{x\n\r", victim);
    check_improve(ch, gsn_dirt, true, 2);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_dirt].beats);

    af.where = TO_AFFECTS;
    af.type = gsn_dirt;
    af.level = ch->level;
    af.duration = 0;
    af.location = APPLY_HITROLL;
    af.modifier = -4;

    af.bitvector = AFF_BLIND;

    affect_to_char(victim, &af);
  }
  else
  {
    check_improve(ch, gsn_dirt, false, 2);
    send_to_char("Your dirt kick misses.\n\r", ch);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_dirt].beats);
    xdamage(ch, victim, 0, gsn_dirt, DAM_NONE, true, VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;
  }
}

CH_CMD(do_gouge)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int chance;
  bool mobdeath = false;

  one_argument(argument, arg);

  chance = get_skill(ch, gsn_gouge);

  if (chance == 0 || (!IS_NPC(ch) && chance < 2 &&
                      ch->level <
                      skill_table[gsn_gouge].skill_level[ch->class]))
  {
    send_to_char("Gouge?  What's that?{x\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    victim = ch->fighting;
    if (victim == NULL)
    {
      send_to_char("But you aren't in combat!\n\r", ch);
      return;
    }
  }

  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_BLIND))
  {
    act("{h$E's already been blinded.{x", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (victim == ch)
  {
    send_to_char("Very funny.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
  {
    act("But $N is such a good friend!", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  /* modifiers */

  /* dexterity */
  chance += get_curr_stat(ch, STAT_DEX);
  chance -= 2 * get_curr_stat(victim, STAT_DEX);

  /* speed */
  if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
    chance += 10;
  if (IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE))
    chance -= 25;

  /* level */
  chance += (ch->level - victim->level) * 2;

  /* sloppy hack to prevent false zeroes */
  if (chance % 5 == 0)
    chance += 1;

  /* now the attack */
  if (number_percent() < chance)
  {
    AFFECT_DATA af;

    act("{k$n is blinded by a poke in the eyes!{x", victim, NULL, NULL,
        TO_ROOM);
    act("{i$n gouges at your eyes!{x", ch, NULL, victim, TO_VICT);
    send_to_char("{DYou see nothing but stars!{x\n\r", victim);
    check_improve(ch, gsn_gouge, true, 2);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_gouge].beats);
    xdamage(ch, victim, number_range(2, 5), gsn_gouge, DAM_NONE, false,
            VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;

    af.where = TO_AFFECTS;
    af.type = gsn_gouge;
    af.level = ch->level;
    af.duration = 0;
    af.location = APPLY_HITROLL;
    af.modifier = -4;

    af.bitvector = AFF_BLIND;

    affect_to_char(victim, &af);
  }
  else
  {
    send_to_char("Your gouge misses.\n\r", ch);
    check_improve(ch, gsn_gouge, false, 2);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_gouge].beats);
    xdamage(ch, victim, 0, gsn_gouge, DAM_NONE, true, VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;
  }
}

CH_CMD(do_trip)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int chance;
  bool mobdeath = false;

  one_argument(argument, arg);

  chance = get_skill(ch, gsn_trip);

  if ((chance == 0) || (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_TRIP)) ||
      (!IS_NPC(ch) && chance < 2 &&
       ch->level < skill_table[gsn_trip].skill_level[ch->class]))
  {
    send_to_char("Tripping?  What's that?\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    victim = ch->fighting;
    if (victim == NULL)
    {
      send_to_char("But you aren't fighting anyone!\n\r", ch);
      return;
    }
  }

  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(victim, AFF_FLYING))
  {
    act("{h$S feet aren't on the ground.{x", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (victim->position < POS_FIGHTING)
  {
    act("{h$N is already down.{c", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (victim == ch)
  {
    send_to_char("{hYou fall flat on your face!{x\n\r", ch);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, 2 * skill_table[gsn_trip].beats);
    act("{k$n trips over $s own feet!{x", ch, NULL, NULL, TO_ROOM);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
  {
    act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  /* modifiers */

  /* size */
  if (ch->size < victim->size)
    chance += (ch->size - victim->size) * 10; /* bigger = harder to trip */

  /* dex */
  chance += get_curr_stat(ch, STAT_DEX);
  chance -= get_curr_stat(victim, STAT_DEX) * 3 / 2;

  /* speed */
  if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
    chance += 10;
  if (IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE))
    chance -= 20;

  /* level */
  chance += (ch->level - victim->level) * 2;

  /* now the attack */
  if (number_percent() < chance)
  {
    act("{i$n trips you and you go down!{x", ch, NULL, victim, TO_VICT);
    act("{hYou trip $N and $N goes down!{x", ch, NULL, victim, TO_CHAR);
    act("{k$n trips $N, sending $M to the ground.{x", ch, NULL, victim,
        TO_NOTVICT);
    check_improve(ch, gsn_trip, true, 1);

    DAZE_STATE(victim, 2 * PULSE_VIOLENCE);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_trip].beats);
    victim->position = POS_RESTING;
    xdamage(ch, victim, number_range(2, 2 + 2 * victim->size), gsn_trip,
            DAM_BASH, true, VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;
  }
  else
  {
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_trip].beats * 2 / 3);
    check_improve(ch, gsn_trip, false, 1);
    xdamage(ch, victim, 0, gsn_trip, DAM_BASH, true, VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;
  }
}

CH_CMD(do_kill)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  bool mobdeath = false;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Kill whom?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (victim == ch)
  {
    send_to_char("{hYou hit yourself.  {z{COuch!{x\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (!IS_NPC(victim))
  {
    if (!IS_SET(victim->act, PLR_TWIT) &&
        !IS_SET(victim->in_room->room_flags, ROOM_ARENA))
    {
      send_to_char("You must MURDER a player.\n\r", ch);
      return;
    }
  }
  if (victim->fighting != NULL && !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
  {
    act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (ch->position == POS_FIGHTING)
  {
    send_to_char("You do the best you can!\n\r", ch);
    return;
  }

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
  multi_hit(ch, victim, TYPE_UNDEFINED, &mobdeath);
  return;
}

CH_CMD(do_mock)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Mock hit whom?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (is_safe_mock(ch, victim))
    return;

  if (victim->fighting != NULL)
  {
    send_to_char("{gThis player is busy at the moment.{x\n\r", ch);
    return;
  }

  if (ch->position == POS_FIGHTING)
  {
    send_to_char("{gYou've already got your hands full!{x\n\r", ch);
    return;
  }

  one_hit_mock(ch, victim, TYPE_UNDEFINED, false);

  return;
}

CH_CMD(do_murde)
{
  send_to_char("If you want to {RMURDER{x, spell it out.\n\r", ch);
  return;
}

CH_CMD(do_murder)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  bool mobdeath = false;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Murder whom?\n\r", ch);
    return;
  }

  if (IS_NPC(ch))
    return;

  if (IS_AFFECTED(ch, AFF_CHARM))
    return;

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    send_to_char("Suicide is a mortal sin.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
  {
    act("$N is your beloved master.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (!WR(ch, victim))
  {
    send_to_char("You are not allowed to harm them.\n\r", ch);
    return;
  }

  if (ch->position == POS_FIGHTING)
  {
    send_to_char("You do the best you can!\n\r", ch);
    return;
  }

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
  if (IS_NPC(ch))
    sprintf(buf, "Help! I am being attacked by %s!", ch->short_descr);
  else
    sprintf(buf, "Help!  I am being attacked by %s!", ch->name);
  do_yell(victim, buf);
  multi_hit(ch, victim, TYPE_UNDEFINED, &mobdeath);

  return;
}

CH_CMD(do_assassinate)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  bool mobdeath = false;

  char buf[MSL];

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Assassinate whom?\n\r", ch);
    return;
  }

  if (ch->fighting != NULL)
  {
    send_to_char("{hYou are already fighting!{x\n\r", ch);
    return;
  }

  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (!WR(ch, victim))
  {
    send_to_char("You are not allowed to harm them.\n\r", ch);
    return;
  }

  if (nia(ch))
    return;

  if (victim == ch)
  {
    send_to_char("You cannot assassinate yourself.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
  {
    send_to_char
      ("{hYou need to wield a primary weapon to backstab.{x\n\r", ch);
    return;
  }

  if (victim->hit < victim->max_hit / 1.5)
  {
    act("$N is hurt and suspicious ... you can't sneak up.", ch, NULL,
        victim, TO_CHAR);
    return;
  }

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_assassinate].beats);
  if (number_percent() < get_skill(ch, gsn_assassinate) ||
      (get_skill(ch, gsn_assassinate) >= 75 && !IS_AWAKE(victim)))
  {
    if (chance(2) && (!IS_SHIELDED(victim, SHD_WARD) && (!IS_NPC(victim))))
    {
      act("Someone has masterfully ASSASSINATED $n!{x\n\r", victim,
          NULL, NULL, TO_ROOM);
      act
        ("$n thrusts their weapon into your vitals, ending your pitiful existence!\n\r",
         ch, NULL, victim, TO_VICT);
      printf_to_char(ch,
                     "{xYou {RA{YSS{RA{YSSIN{RA{YTE{x your helpless victim{x!!");
      group_gain(ch, victim);
      sprintf(buf,
              "{w[{RPK{w] {R%s {whas {Yassassinated {R%s {wat {Y%s{w!{x",
              ch->name, victim->name, ch->in_room->name);
      do_gmessage(buf);
      victim->hit = -victim->hit - 1;
      victim->pk_timer = 2;
      raw_kill(victim, ch, &mobdeath);
      check_improve(ch, gsn_assassinate, true, 1);
    }
    else
    {
      check_improve(ch, gsn_assassinate, true, 1);
      multi_hit(ch, victim, gsn_assassinate, &mobdeath);
      if (mobdeath)
        return;
    }
  }
  else
  {
    check_improve(ch, gsn_assassinate, false, 1);
    xdamage(ch, victim, 0, gsn_assassinate, DAM_NONE, true, VERBOSE_STD,
            &mobdeath);
    if (mobdeath)
      return;
  }

  return;
}

CH_CMD(do_backstab)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  bool mobdeath = false;

  one_argument(argument, arg);

  if (arg[0] == '\0')
  {
    send_to_char("Backstab whom?\n\r", ch);
    return;
  }

  if (ch->fighting != NULL)
  {
    send_to_char("{hYou're facing the wrong end.{x\n\r", ch);
    return;
  }

  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (!WR(ch, victim))
  {
    send_to_char("You are not allowed to harm them.\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    send_to_char("How can you sneak up on yourself?\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
  {
    send_to_char
      ("{hYou need to wield a primary weapon to backstab.{x\n\r", ch);
    return;
  }

  if (victim->hit < victim->max_hit / 1.5)
  {
    act("$N is hurt and suspicious ... you can't sneak up.", ch, NULL,
        victim, TO_CHAR);
    return;
  }

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_backstab].beats);
  if (number_percent() < get_skill(ch, gsn_backstab) ||
      (get_skill(ch, gsn_backstab) >= 2 && !IS_AWAKE(victim)))
  {
    check_improve(ch, gsn_backstab, true, 1);
    multi_hit(ch, victim, gsn_backstab, &mobdeath);
    if (mobdeath)
      return;
  }
  else
  {
    check_improve(ch, gsn_backstab, false, 1);
    xdamage(ch, victim, 0, gsn_backstab, DAM_NONE, true, VERBOSE_STD,
            &mobdeath);
    if (mobdeath)
      return;
  }

  return;
}

CH_CMD(do_blackjack)
{
  char buf[MSL];
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int chance;

  one_argument(argument, arg);

  if ((chance = get_skill(ch, gsn_blackjack)) == 0)
  {
    send_to_char("You can't blackjack.\n\r", ch);
    return;
  }

  if (ch->fighting != NULL)
  {
    send_to_char("{hYou are already fighting!{x\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    victim = ch->fighting;
    if (victim == NULL)
    {
      send_to_char("You must specify a target!\n\r", ch);
      return;
    }
  }
  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (ch->blackjack_timer > 0 && !IS_NPC(victim))
  {
    sprintf(buf,
            "You must rest %d more minutes before trying to blackjack again.\n\r",
            ch->blackjack_timer);
    send_to_char(buf, ch);
    return;
  }

  kill_swalk(ch);

  if (!WR(ch, victim))
  {
    send_to_char("You are not allowed to harm them.\n\r", ch);
    return;
  }

  if (nia(ch))
    return;

  if (IS_AFFECTED(victim, AFF_SLEEP))
  {
    act("$E's already been blackjacked.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (victim == ch)
  {
    send_to_char("Very funny.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
  {
    act("But $N is such a good friend!", ch, NULL, victim, TO_CHAR);
    return;
  }

  /* level */
  chance += (ch->level - victim->level) * 1.15;

  /* sloppy hack to prevent false zeroes */
  if (chance % 5 == 0)
    chance += 1;

  if (chance == 0)
  {
    send_to_char("You failed horrbly!.\n\r", ch);
    return;
  }

  if (number_percent() < chance)
  {
    AFFECT_DATA af;

    act("$n suddenly passes out.", victim, NULL, NULL, TO_ROOM);
    act
      ("$n whacks you upside the head, a sharp pain tears through your skull, starbursts erupt in front of your eyes as the world fades from view!",
       ch, NULL, victim, TO_VICT);
    printf_to_char(ch,
                   "{xYou {Dwhack{x your target upside the head!! \n\rThey are knocked out {ccold{x!!");

    send_to_char("You are knocked out cold!\n\r", victim);
    ch->blackjack_timer = (number_range(2, 5));
    check_improve(ch, gsn_blackjack, true, 2);

    af.where = TO_AFFECTS;
    af.type = gsn_sleep;
    af.level = ch->level;
    af.duration = 1;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = AFF_SLEEP;

    affect_to_char(victim, &af);
    victim->position = POS_SLEEPING;

  }
  else
  {
    check_improve(ch, gsn_blackjack, false, 2);
  }
}

CH_CMD(do_circle)
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  bool mobdeath = false;

  if (get_skill(ch, gsn_circle) == 0 ||
      (!IS_NPC(ch) &&
       ch->level < skill_table[gsn_circle].skill_level[ch->class]))
  {
    send_to_char("Circle? What's that?\n\r", ch);
    return;
  }

  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }

  if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
  {
    send_to_char("You need to wield a primary weapon to circle.\n\r", ch);
    return;
  }

  if (victim->hit < victim->max_hit / 6)
  {
    act("$N is hurt and suspicious ... you can't sneak around.", ch, NULL,
        victim, TO_CHAR);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (!can_see(ch, victim))
  {
    send_to_char("You stumble blindly into a wall.\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_circle].beats / 2);
  if (number_percent() < get_skill(ch, gsn_circle) ||
      (get_skill(ch, gsn_circle) >= 2 && !IS_AWAKE(victim)))
  {
    check_improve(ch, gsn_circle, true, 1);
    act("{i$n circles around behind you.{x", ch, NULL, victim, TO_VICT);
    act("{hYou circle around $N.{x", ch, NULL, victim, TO_CHAR);
    act("{k$n circles around behind $N.{x", ch, NULL, victim, TO_NOTVICT);
    multi_hit(ch, victim, gsn_circle, &mobdeath);
    if (mobdeath)
      return;
  }
  else
  {
    check_improve(ch, gsn_circle, false, 1);
    act("{i$n tries to circle around you.{x", ch, NULL, victim, TO_VICT);
    act("{h$N circles with you.{x", ch, NULL, victim, TO_CHAR);
    act("{k$n tries to circle around $N.{x", ch, NULL, victim, TO_NOTVICT);
    xdamage(ch, victim, 0, gsn_circle, DAM_NONE, true, VERBOSE_STD,
            &mobdeath);
    if (mobdeath)
      return;
  }

  return;
}

bool check_counter(CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt,
                   bool * mobdeath)
{
  int chance;
  int dam_type;
  OBJ_DATA *wield;

  if (ch->stunned)
  {
    return false;
  }

  if ((get_eq_char(victim, WEAR_WIELD) == NULL) ||
      (!IS_AWAKE(victim)) || (!can_see(victim, ch)) ||
      (get_skill(victim, gsn_counter) < 1))
    return false;

  wield = get_eq_char(victim, WEAR_WIELD);

  chance = get_skill(victim, gsn_counter) / 6;
  chance += (victim->level - ch->level) / 2;
  chance +=
    2 * (get_curr_stat(victim, STAT_DEX) - get_curr_stat(ch, STAT_DEX));
  chance +=
    get_weapon_skill(victim,
                     get_weapon_sn(victim)) - get_weapon_skill(ch,
                                                               get_weapon_sn
                                                               (ch));
  chance += (get_curr_stat(victim, STAT_STR) - get_curr_stat(ch, STAT_STR));

  if (number_percent() >= chance)
    return false;

  dt = gsn_counter;

  if (dt == TYPE_UNDEFINED)
  {
    dt = TYPE_HIT;
    if (wield != NULL && wield->item_type == ITEM_WEAPON)
      dt += wield->value[3];
    else
      dt += ch->dam_type;
  }

  if (dt < TYPE_HIT)
    if (wield != NULL)
      dam_type = attack_table[wield->value[3]].damage;
    else
      dam_type = attack_table[ch->dam_type].damage;
  else
    dam_type = attack_table[dt - TYPE_HIT].damage;

  if (dam_type == -1)
    dam_type = DAM_BASH;

  act("You reverse $n's attack and counter with your own!", ch, NULL,
      victim, TO_VICT);
  act("$N reverses your attack!", ch, NULL, victim, TO_CHAR);

  xdamage(victim, ch, dam / 2, gsn_counter, dam_type, true, VERBOSE_STD, mobdeath); /* DAM MSG
                                                                                       NUMBER!! 
                                                                                     */

  check_improve(victim, gsn_counter, true, 6);

  return true;
}

bool check_critical(CHAR_DATA * ch, CHAR_DATA * victim)
{
  OBJ_DATA *obj;

  if ((number_range(1, 100) > 3))
    return false;

  obj = get_eq_char(ch, WEAR_WIELD);

  if ((get_eq_char(ch, WEAR_WIELD) == NULL) ||
      (get_skill(ch, gsn_critical) < 1))
    return false;

  check_improve(ch, gsn_critical, true, 6);
  return true;
}

CH_CMD(do_feed)
{
  CHAR_DATA *victim;
  int dam;
  bool mobdeath = false;

  if (get_skill(ch, gsn_feed) == 0 ||
      (!IS_NPC(ch) &&
       ch->level < skill_table[gsn_feed].skill_level[ch->class]))
  {
    send_to_char("Feed? What's that?\n\r", ch);
    return;
  }

  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }

  if (victim->hit < victim->max_hit / 16)
  {
    act("$N is hurt and suspicious ... you can't get close enough.", ch,
        NULL, victim, TO_CHAR);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_feed].beats);
  if (number_percent() < get_skill(ch, gsn_feed) ||
      (get_skill(ch, gsn_feed) >= 2 && !IS_AWAKE(victim)))
  {
    check_improve(ch, gsn_feed, true, 1);
    act("{i$n bites you.{x", ch, NULL, victim, TO_VICT);
    act("{hYou bite $N.{x", ch, NULL, victim, TO_CHAR);
    act("{k$n bites $N.{x", ch, NULL, victim, TO_NOTVICT);
    dam =
      number_range(((ch->level / 2) + (victim->level / 2) * 40),
                   ((ch->level / 2) + (victim->level / 2)) * 175);
    xdamage(ch, victim, dam, gsn_feed, DAM_PIERCE, true, VERBOSE_STD,
            &mobdeath);
    if (mobdeath)
      return;
  }
  else
  {
    check_improve(ch, gsn_feed, false, 1);
    act("{i$n tries to bite you, but hits only air.{x", ch, NULL, victim,
        TO_VICT);
    act("{hYou chomp a mouthfull of air.{x", ch, NULL, victim, TO_CHAR);
    act("{k$n tries to bite $N.{x", ch, NULL, victim, TO_NOTVICT);
    xdamage(ch, victim, 0, gsn_feed, DAM_NEGATIVE, true, VERBOSE_STD,
            &mobdeath);
    if (mobdeath)
      return;
  }

  return;
}

CH_CMD(do_flee)
{
  ROOM_INDEX_DATA *was_in;
  ROOM_INDEX_DATA *now_in;
  CHAR_DATA *victim;
  int attempt;

  if (IS_AFFECTED(ch, AFF_TERROR) || IS_SHIELDED(ch, SHD_RETREAT))
  {
    WAIT_STATE(ch, 4);
  }
  else
  {
    WAIT_STATE(ch, 8);
  }

  if ((victim = ch->fighting) == NULL)
  {
    if (ch->position == POS_FIGHTING)
      ch->position = POS_STANDING;
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  was_in = ch->in_room;
  for (attempt = 0; attempt < MAX_DIR; attempt++)
  {
    EXIT_DATA *pexit;
    int door;

    door = number_door();
    if ((pexit = was_in->exit[door]) == 0 || pexit->u1.to_room == NULL ||
        IS_SET(pexit->exit_info, EX_CLOSED) ||
        number_range(0, ch->daze) != 0 || (IS_NPC(ch) &&
                                           IS_SET(pexit->u1.to_room->
                                                  room_flags, ROOM_NO_MOB)))
      continue;

    move_char(ch, door, false, false);
    if ((now_in = ch->in_room) == was_in)
      continue;

    ch->in_room = was_in;
    act("$n has {Yfled{x!", ch, NULL, NULL, TO_ROOM);
    if (!IS_NPC(ch))
    {
      send_to_char("{BYou {Yflee{B from combat!{x\n\r", ch);
      if (((ch->class == 2) ||
           (ch->class == (MAX_CLASS / 2) + 1)) &&
          (number_percent() < 3 * (ch->level / 2)))
      {
        if (IS_NPC(victim) || ch->attacker == false)
        {
          send_to_char("You {Ysnuck away{x safely.\n\r", ch);
        }
        else
        {
          send_to_char
            ("You feel something singe your butt on the way out.\n\r", ch);
          act
            ("$n is nearly {Yzapped{x in the butt by a lightning bolt from above!",
             ch, NULL, NULL, TO_ROOM);
          ch->hit -= (ch->hit / 8);
        }
      }
      else
      {
        if (!IS_NPC(victim) && ch->attacker == true)
        {
          send_to_char
            ("The {RWrath of Thoth {YZAPS{x your butt on the way out!\n\r",
             ch);
          act("$n is {Yzapped{x in the butt by a lightning bolt from above!",
              ch, NULL, NULL, TO_ROOM);
          ch->hit -= (ch->hit / 4);
        }
        send_to_char("You lost 10 exp.\n\r", ch);
        gain_exp(ch, -10);
      }
    }
    ch->in_room = now_in;
    stop_fighting(ch, true);
    return;
  }

  send_to_char("{z{CPANIC!{x{B You couldn't escape!{x\n\r", ch);
  return;
}

CH_CMD(do_rescue)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  CHAR_DATA *fch;

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Rescue whom?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    send_to_char("What about {Yfleeing{x instead?\n\r", ch);
    return;
  }

  if (!IS_NPC(ch) && IS_NPC(victim) && !is_same_group(ch, victim))
  {
    send_to_char("Doesn't need your help!\n\r", ch);
    return;
  }

  if (ch->fighting == victim)
  {
    send_to_char("Too late.\n\r", ch);
    return;
  }

  if ((fch = victim->fighting) == NULL)
  {
    send_to_char("That person is not fighting right now.\n\r", ch);
    return;
  }

  if (IS_NPC(fch) && !is_same_group(ch, victim))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_rescue].beats);
  if (number_percent() > get_skill(ch, gsn_rescue))
  {
    send_to_char("You fail the rescue.\n\r", ch);
    check_improve(ch, gsn_rescue, false, 1);
    return;
  }

  act("{yYou rescue $N!{x", ch, NULL, victim, TO_CHAR);
  act("{y$n rescues you!{x", ch, NULL, victim, TO_VICT);
  act("{y$n rescues $N!{x", ch, NULL, victim, TO_NOTVICT);
  check_improve(ch, gsn_rescue, true, 1);

  stop_fighting(fch, false);
  stop_fighting(victim, false);

  set_fighting(ch, fch);
  set_fighting(fch, ch);
  return;
}

CH_CMD(do_howl)
{
  char arg[MSL];
  CHAR_DATA *victim;
  int dam;
  bool mobdeath = false;

  one_argument(argument, arg);

  if (!IS_NPC(ch) && get_skill(ch, gsn_howl) < 2 &&
      ch->level < skill_table[gsn_howl].skill_level[ch->class])
  {
    send_to_char("You are not in touch with your primal nature.\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (get_skill(ch, gsn_howl) < number_percent())
  {
    printf_to_char(ch,
                   "You try to howl, but only produce a faint screech.\n\r");
    check_improve(ch, gsn_howl, false, 1);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_howl].beats);

  if ((victim = ch->fighting) == NULL)
  {
    if (arg[0] == '\0')
    {
      send_to_char("Howl at whom?\n\r", ch);
      return;
    }
    else if ((victim = get_char_room(ch, arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
  }

  if (!WR(ch, victim))
  {
    send_to_char("You are not allowed to harm them.\n\r", ch);
    return;
  }

  if (nia(ch))
    return;

  if (victim == ch)
  {
    send_to_char("You are immune to your own howling.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  act("$n lets out an ear-piercing howl!", ch, NULL, NULL, TO_ROOM);
  act("You let out an ear-piercing howl!", ch, NULL, NULL, TO_CHAR);

  dam = 35 + ch->level;
  check_improve(ch, gsn_howl, true, 1);
  xdamage(ch, victim, number_range(dam, (ch->level * 4)), gsn_howl,
          DAM_SOUND, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  if (chance(55))
    dam = 30 + ch->level;
  xdamage(ch, victim, number_range(dam, (ch->level * 3)), gsn_howl,
          DAM_SOUND, true, VERBOSE_STD, &mobdeath);

  return;
}

CH_CMD(do_wspit)
{
  char arg[MSL];
  CHAR_DATA *victim;
  int dam;
  bool mobdeath = false;

  one_argument(argument, arg);

  if (!IS_NPC(ch) && get_skill(ch, gsn_wspit) < 2 &&
      ch->level < skill_table[gsn_wspit].skill_level[ch->class])
  {
    send_to_char("Your mouth is too dry.\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (get_skill(ch, gsn_wspit) < number_percent())
  {
    printf_to_char(ch, "You fail to produce sufficient saliva.\n\r");
    check_improve(ch, gsn_wspit, false, 1);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_wspit].beats);

  if ((victim = ch->fighting) == NULL)
  {
    if (arg[0] == '\0')
    {
      send_to_char("Spit on whom?\n\r", ch);
      return;
    }
    else if ((victim = get_char_room(ch, arg)) == NULL)
    {
      send_to_char("They aren't here.\n\r", ch);
      return;
    }
  }

  if (!WR(ch, victim))
  {
    send_to_char("You are not allowed to harm them.\n\r", ch);
    return;
  }

  if (nia(ch))
    return;

  if (victim == ch)
  {
    send_to_char("You drool on yourself.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  act("$n {-s{-p{-i{-t{-s{x on you!", ch, NULL, victim, TO_VICT);
  act("$n {-s{-p{-i{-t{-s{x on $N!", ch, NULL, victim, TO_NOTVICT);
  act("You {-s{-p{-i{-t{x on $N!", ch, NULL, victim, TO_CHAR);

  dam = 35 + ch->level;
  check_improve(ch, gsn_wspit, true, 1);
  xdamage(ch, victim, number_range(dam, (ch->level * 4)), gsn_wspit,
          DAM_WATER, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  if (chance(55))
    dam = 30 + ch->level;
  xdamage(ch, victim, number_range(dam, (ch->level * 3)), gsn_wspit,
          DAM_WATER, true, VERBOSE_STD, &mobdeath);

  return;
}

CH_CMD(do_kick)
{
  CHAR_DATA *victim;
  int dam;
  bool mobdeath = false;

  if (!IS_NPC(ch) && get_skill(ch, gsn_kick) < 2 &&
      ch->level < skill_table[gsn_kick].skill_level[ch->class])
  {
    send_to_char("You better leave the martial arts to fighters.\n\r", ch);
    return;
  }

  if (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_KICK))
    return;

  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (get_skill(ch, gsn_kick) < number_percent())
  {
    printf_to_char(ch,
                   "You try to kick your opponent but they are to fast!\n\r");
    check_improve(ch, gsn_kick, false, 1);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_kick].beats);

  dam = 35 + ch->level;
  check_improve(ch, gsn_kick, true, 1);
  xdamage(ch, victim, number_range(dam, (ch->level * 4)), gsn_kick,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  if (chance(55))
    dam = 30 + ch->level;
  xdamage(ch, victim, number_range(dam, (ch->level * 3)), gsn_kick,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);

  return;
}

CH_CMD(do_disarm)
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  int chance, hth, ch_weapon, vict_weapon, ch_vict_weapon;

  hth = 0;

  if ((chance = get_skill(ch, gsn_disarm)) == 0)
  {
    send_to_char("You don't know how to disarm opponents.\n\r", ch);
    return;
  }

  if (get_eq_char(ch, WEAR_WIELD) == NULL &&
      ((hth = get_skill(ch, gsn_hand_to_hand)) == 0 ||
       (IS_NPC(ch) && !IS_SET(ch->off_flags, OFF_DISARM))))
  {
    send_to_char("You must wield a weapon to disarm.\n\r", ch);
    return;
  }

  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if ((obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
  {
    send_to_char("{hYour opponent is not wielding a weapon.{x\n\r", ch);
    return;
  }

  /* find weapon skills */
  ch_weapon = get_weapon_skill(ch, get_weapon_sn(ch));
  vict_weapon = get_weapon_skill(victim, get_weapon_sn(victim));
  ch_vict_weapon = get_weapon_skill(ch, get_weapon_sn(victim));

  /* modifiers */

  /* skill */
  if (get_eq_char(ch, WEAR_WIELD) == NULL)
    chance = chance * hth / 150;
  else
    chance = chance * ch_weapon / 100;

  chance += (ch_vict_weapon / 2 - vict_weapon) / 2;

  /* dex vs. strength */
  chance += get_curr_stat(ch, STAT_DEX);
  chance -= 2 * get_curr_stat(victim, STAT_STR);

  /* level */
  chance += (ch->level - victim->level) * 2;

  chance /= 2;

  /* and now the attack */
  if (number_percent() < chance)
  {
    if (((chance = get_skill(victim, gsn_grip)) == 0) ||
        (!IS_NPC(victim) &&
         victim->level < skill_table[gsn_grip].skill_level[victim->class]))
    {
      if (!IS_IMMORTAL(ch))
        WAIT_STATE(ch, skill_table[gsn_disarm].beats);
      disarm(ch, victim);
      check_improve(ch, gsn_disarm, true, 1);
      return;
    }
    if (number_percent() > (chance / 5) * 4)
    {
      if (!IS_IMMORTAL(ch))
        WAIT_STATE(ch, skill_table[gsn_disarm].beats);
      disarm(ch, victim);
      check_improve(ch, gsn_disarm, true, 1);
      check_improve(victim, gsn_grip, false, 1);
      return;
    }
    check_improve(victim, gsn_grip, true, 1);
  }
  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_disarm].beats);
  act("{hYou fail to disarm $N.{x", ch, NULL, victim, TO_CHAR);
  act("{i$n tries to disarm you, but fails.{x", ch, NULL, victim, TO_VICT);
  act("{k$n tries to disarm $N, but fails.{x", ch, NULL, victim, TO_NOTVICT);
  check_improve(ch, gsn_disarm, false, 1);
  return;
}

CH_CMD(do_surrender)
{
  CHAR_DATA *mob;
  bool mobdeath = false;

  if ((mob = ch->fighting) == NULL)
  {
    send_to_char("But you're not fighting!\n\r", ch);
    return;
  }
  act("You surrender to $N!", ch, NULL, mob, TO_CHAR);
  act("$n surrenders to you!", ch, NULL, mob, TO_VICT);
  act("$n tries to surrender to $N!", ch, NULL, mob, TO_NOTVICT);
  stop_fighting(ch, true);

  if (!IS_NPC(ch) && IS_NPC(mob) &&
      (!HAS_TRIGGER(mob, TRIG_SURR) ||
       !mp_percent_trigger(mob, ch, NULL, NULL, TRIG_SURR)))
  {
    act("$N seems to ignore your cowardly act!", ch, NULL, mob, TO_CHAR);
    multi_hit(mob, ch, TYPE_UNDEFINED, &mobdeath);
    if (mobdeath)
      return;
  }
}

CH_CMD(do_sla)
{
  send_to_char("If you want to {RSLAY{x, spell it out.\n\r", ch);
  return;
}

CH_CMD(do_slay)
{
  CHAR_DATA *victim;
  char arg[MAX_INPUT_LENGTH];
  bool mobdeath = false;

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Slay whom?\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (ch == victim)
  {
    send_to_char("Suicide is a mortal sin.\n\r", ch);
    return;
  }

  if (!IS_NPC(victim) && victim->level >= get_trust(ch))
  {
    send_to_char("{hYou failed.{c\n\r", ch);
    return;
  }

  if (IS_NPC(victim) || get_trust(ch) >= CREATOR)
  {
    act("{hYou slay $M in cold blood!{x", ch, NULL, victim, TO_CHAR);
    act("{i$n slays you in cold blood!{x", ch, NULL, victim, TO_VICT);
    act("{k$n slays $N in cold blood!{x", ch, NULL, victim, TO_NOTVICT);
    raw_kill(victim, ch, &mobdeath);
  }
  else
  {
    act("{i$N wields a sword called '{z{RGodSlayer{i'!{x", ch, NULL,
        victim, TO_CHAR);
    act("{hYou wield a sword called '{z{RGodSlayer{h'!{x", ch, NULL,
        victim, TO_VICT);
    act("{k$N wields a sword called '{z{RGodSlayer{k'!{x", ch, NULL,
        victim, TO_NOTVICT);
    act("{i$N's slice takes off your left arm!{x", ch, NULL, victim, TO_CHAR);
    act("{hYour slice takes off $n's left arm!{x", ch, NULL, victim, TO_VICT);
    act("{k$N's slice takes off $n's left arm!{x", ch, NULL, victim,
        TO_NOTVICT);
    act("{i$N's slice takes off your right arm!{x", ch, NULL, victim,
        TO_CHAR);
    act("{hYour slice takes off $n's right arm!{x", ch, NULL, victim,
        TO_VICT);
    act("{k$N's slice takes off $n's right arm!{x", ch, NULL, victim,
        TO_NOTVICT);
    act("{i$N's slice cuts off both of your legs!{x", ch, NULL, victim,
        TO_CHAR);
    act("{hYour slice cuts off both of $n's legs!{x", ch, NULL, victim,
        TO_VICT);
    act("{k$N's slice cuts off both of $n's legs!{x", ch, NULL, victim,
        TO_NOTVICT);
    act("{i$N's slice beheads you!{x", ch, NULL, victim, TO_CHAR);
    act("{hYour slice beheads $n!{x", ch, NULL, victim, TO_VICT);
    act("{k$N's slice beheads $n!{x", ch, NULL, victim, TO_NOTVICT);
    act("{iYou are DEAD!!!{x", ch, NULL, victim, TO_CHAR);
    act("{h$n is DEAD!!!{x", ch, NULL, victim, TO_VICT);
    act("{k$n is DEAD!!!{x", ch, NULL, victim, TO_NOTVICT);
    act("A sword called '{z{RGodSlayer{x' vanishes.", ch, NULL, victim,
        TO_VICT);
    act("A sword called '{z{RGodSlayer{x' vanishes.", ch, NULL, victim,
        TO_NOTVICT);
    raw_kill(ch, victim, &mobdeath);
  }
  return;
}

CH_CMD(do_rub)
{
  int chance;

  if ((chance = get_skill(ch, gsn_rub)) == 0 ||
      (!IS_NPC(ch) && get_skill(ch, gsn_rub) < 2 &&
       ch->level < skill_table[gsn_rub].skill_level[ch->class]))
  {
    send_to_char("You nearly gouged your own eyes.\n\r", ch);
    return;
  }
  if ((!IS_AFFECTED(ch, gsn_dirt)) ||
      (!IS_AFFECTED(ch, skill_lookup("fire_breath"))) ||
      (!str_cmp(class_table[ch->class].name, "Knight") &&
       (!IS_AFFECTED(ch, gsn_gouge))))

  {
    send_to_char("There is nothing in your eyes!\n\r", ch);
    return;
  }
  if (chance % 5 == 0)
    chance += 1;
  if (number_percent() < chance)
  {
    send_to_char("You rub your eyes, and your vision clears up again.\n\r",
                 ch);
    act("$n rubs $s eyes until $s vision clears.", ch, NULL, NULL, TO_ROOM);
    if (is_affected(ch, gsn_dirt))
    {
      affect_strip(ch, gsn_dirt);
      check_improve(ch, gsn_rub, true, 5);
      if (!IS_IMMORTAL(ch))
        WAIT_STATE(ch, skill_table[gsn_rub].beats);
      return;
    }
    if (IS_AFFECTED(ch, gsn_gouge) &&
        (!str_cmp(class_table[ch->class].name, "Knight")))
    {
      affect_strip(ch, gsn_gouge);
      check_improve(ch, gsn_rub, true, 5);
      if (!IS_IMMORTAL(ch))
        WAIT_STATE(ch, skill_table[gsn_rub].beats);
      return;
    }
    if (is_affected(ch, skill_lookup("fire breath")))
    {
      affect_strip(ch, skill_lookup("fire breath"));
      check_improve(ch, gsn_rub, true, 5);
      if (!IS_IMMORTAL(ch))
        WAIT_STATE(ch, skill_table[gsn_rub].beats);
      return;
    }
  }
  else
  {
    send_to_char("You failed to remove the dirt from your eyes!\n\r", ch);
    check_improve(ch, gsn_rub, false, 6);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_rub].beats / 2);
    return;
  }
}

/* Coded by Shinji - mudnet.net:7000 */
CH_CMD(do_sharpen)
{
  char arg[MAX_INPUT_LENGTH];
  OBJ_DATA *obj;
  int chance;
  AFFECT_DATA af;
  bool mobdeath = false;

  one_argument(argument, arg);

  if (!IS_NPC(ch) && get_skill(ch, gsn_sharpen) < 2 &&
      ch->level < skill_table[gsn_sharpen].skill_level[ch->class])
  {
    send_to_char("You had best leave that skill to master warriors.\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    send_to_char("Sharpen what?\n\r", ch);
    return;
  }

  if (ch->fighting)
  {
    send_to_char("Quit fighting! Then try.\n\r", ch);
    return;
  }

  if (!(obj = get_obj_carry(ch, arg)))
  {
    send_to_char("You do not have that weapon.\n\r", ch);
    return;
  }

  if (obj->item_type != ITEM_WEAPON)
  {
    send_to_char("That item is not a weapon.\n\r", ch);
    return;
  }

  if (IS_WEAPON_STAT(obj, WEAPON_SHARP))
  {
    send_to_char("This weapon is allready as sharp as it can get!\n\r", ch);
    return;
  }
  if (IS_WEAPON_STAT(obj, WEAPON_VORPAL))
  {
    send_to_char("This weapon is allready as sharp as it can get!\n\r", ch);
    return;
  }

  chance = number_range(1, 15);

  if (chance <= 10)
  {
    extract_obj(obj);
    obj = NULL;
    send_to_char
      ("You failed miserably and dulled the weapon beyond repair!\n\r", ch);
    return;
  }

  if (chance >= 45)
  {
    send_to_char
      ("You slice your finger while trying to sharpen your weapon.\n\r", ch);
    xdamage(ch, ch, ch->hit / 10, gsn_sharpen, WEAR_NONE, DAM_SLASH,
            VERBOSE_STD, &mobdeath);
    return;
  }

  if (chance <= 46)
  {
    send_to_char
      ("You put all your concentration into improving your weapon.\n\r", ch);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, PULSE_VIOLENCE * 5);
    send_to_char("Your weapon is now complete.\n\r", ch);
    obj->value[1] += 1;
    obj->value[2] += 1;

    af.where = TO_WEAPON;
    af.type = gsn_sharpen;
    af.level = 0;
    af.duration = -1;
    af.location = 0;
    af.modifier = 0;
    af.bitvector = WEAPON_SHARP;
    affect_to_obj(obj, &af);
    return;
  }
}

CH_CMD(do_rampage)
{
  OBJ_DATA *obj;
  CHAR_DATA *victim;
  int dam;
  bool mobdeath = false;

  if (!IS_NPC(ch) && get_skill(ch, gsn_rampage) < 2 &&
      ch->level < skill_table[gsn_rampage].skill_level[ch->class])
  {
    send_to_char("You had best leave that skill to master warriors.\n\r", ch);
    return;
  }

  if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
  {
    send_to_char
      ("You need to wield a primary weapon to go on a rampage.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char
      ("How do you expect to lose your temper when your not in battle.\n\r",
       ch);
    return;
  }
  if (!can_see(ch, victim))
  {
    send_to_char("You stumble blindly into a wall.\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_rampage].beats / 2);

  if (get_skill(ch, gsn_rampage) < number_percent())
  {
    printf_to_char(ch,
                   "You prepare for a rampage but your opponent is too quick!\n\r");
    check_improve(ch, gsn_rampage, false, 1);
    return;
  }

  dam = ch->level * 15;

  check_improve(ch, gsn_rampage, true, 3);
  xdamage(ch, victim, number_range(dam, ch->level * 30), gsn_rampage,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  dam = ch->level * 20;

  xdamage(ch, victim, number_range(dam, ch->level * 30), gsn_rampage,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  dam = ch->level * 25;

  xdamage(ch, victim, number_range(dam, ch->level * 30), gsn_rampage,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  if (chance(30))
    return;

  dam = ch->level * 15;

  check_improve(ch, gsn_rampage, true, 2);
  xdamage(ch, victim, number_range(dam, ch->level * 30), gsn_rampage,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  if (chance(75))
    return;

  dam = ch->level * 15;

  xdamage(ch, victim, number_range(dam, ch->level * 30), gsn_rampage,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  if (chance(70))
  {
    printf_to_char(ch,
                   "{YYour opponent is stunned by your {Rf{ru{Rr{ri{Ro{ru{Rs{Y{x rampage!{x\n\r");
    victim->stunned = 1;
    check_improve(ch, gsn_stun, true, 1);
  }

  if (chance(35))
    return;

  dam = ch->level * 30;

  xdamage(ch, victim, number_range(dam, ch->level * 30), gsn_rampage,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  if (chance(10))
    return;

  dam = ch->level * 50;

  xdamage(ch, victim, number_range(dam, ch->level * 55), gsn_rampage,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);

  return;
}

CH_CMD(do_ambush)
{
  AFFECT_DATA af;
  CHAR_DATA *victim;
  int skill;
  char arg[MAX_INPUT_LENGTH];
  bool mobdeath = false;

  if (nia(ch))
    return;

  argument = one_argument(argument, arg);

  if ((skill = get_skill(ch, gsn_ambush)) == 0)
  {
    send_to_char("You don't know how to ambush.\n\r", ch);
    return;
  }
  if (ch->fighting != NULL)
  {
    send_to_char("{YThey would notice!{x\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    if (is_affected(ch, gsn_ambush))
    {
      send_to_char("You are already lying in ambush.\n\r", ch);
      return;
    }
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, 12);
    if (number_percent() < skill)
    {
      af.where = TO_AFFECTS;
      af.location = APPLY_NONE;
      af.type = gsn_ambush;
      af.modifier = 0;
      af.bitvector = 0;
      af.duration = ch->level / 5;
      af.level = ch->level;
      affect_to_char(ch, &af);
      check_improve(ch, gsn_ambush, true, 2);
      send_to_char("You hide and prepare an ambush.\n\r", ch);
      return;
    }
    send_to_char
      ("You look around but can not find a suitable hiding place.\n\r", ch);
    check_improve(ch, gsn_ambush, false, 2);
    return;
  }
  if (!is_affected(ch, gsn_ambush))
  {
    send_to_char("You haven't even prepared an ambush yet.\n\r", ch);
    return;
  }
  if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (!WR(ch, victim))
  {
    send_to_char("You are not allowed to harm them.\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    send_to_char("Ambush yourself? You'd probably notice.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (get_eq_char(ch, WEAR_WIELD) == NULL)
  {
    send_to_char("{hYou need to wield a primary weapon to ambush.{x\n\r", ch);
    return;
  }

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, 24);
  if (number_percent() < skill)
  {
    affect_strip(ch, gsn_ambush);
    check_improve(ch, gsn_ambush, true, 1);
    multi_hit(ch, victim, gsn_ambush, &mobdeath);
    if (mobdeath)
      return;
  }
  else
  {
    affect_strip(ch, gsn_ambush);
    check_improve(ch, gsn_ambush, false, 1);
    xdamage(ch, victim, 0, gsn_ambush, DAM_NONE, true, VERBOSE_STD,
            &mobdeath);
    if (mobdeath)
      return;
  }
  return;
}

/* STAKE skill by The Mage */
CH_CMD(do_stake)
{
  char arg[MAX_INPUT_LENGTH];
  CHAR_DATA *victim;
  int chance;
  bool mobdeath = false;

  one_argument(argument, arg);

  if ((chance = get_skill(ch, gsn_stake)) &&
      ch->level < skill_table[gsn_stake].skill_level[ch->class] &&
      !IS_NPC(ch))
  {
    send_to_char("Stake? What's that?\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    victim = ch->fighting;
    if (victim == NULL)
    {
      send_to_char("Stake What undead??\n\r", ch);
      return;
    }
  }

  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }
  if (!WR(ch, victim))
  {
    send_to_char("You are not allowed to harm them.\n\r", ch);
    return;
  }

  if (!IS_NPC(victim) &&
      (!str_cmp(class_table[victim->class].name, "Vampire") ||
       (!str_cmp(class_table[victim->class].name, "Cainite") ||
        (!str_cmp(class_table[victim->class].name, "Revenant") ||
         (!str_cmp(class_table[victim->class].name, "Lich"))))))
  {
    send_to_char("You cannot stake a non-vampire player.\n\r", ch);
    return;
  }
  if (IS_NPC(victim) &&
      (!is_name("vampire", victim->name) &&
       !is_name("undead", victim->name) &&
       !is_name("zombie", victim->name) && !is_name("corpse", victim->name)))
  {

    send_to_char("You cannot stake this mob.\n\r", ch);
    return;
  }

  if (victim == ch)
  {
    send_to_char("You aren't undead.. you cannot stake yourself.\n\r", ch);
    return;
  }

  if (is_safe(ch, victim))
    return;

  if (IS_NPC(victim) && victim->fighting != NULL &&
      !is_same_group(ch, victim->fighting))
  {
    send_to_char("Kill stealing is not permitted.\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
  {
    act("But $N is your friend!", ch, NULL, victim, TO_CHAR);
    return;
  }

  /* modifiers */

  if (ch->size < victim->size)
    chance += (ch->size - victim->size) * 15;
  else
    chance += (ch->size - victim->size) * 10;

  /* stats */
  chance -= GET_AC(victim, AC_PIERCE) / 25;
  /* speed */
  if (IS_SET(ch->off_flags, OFF_FAST) || IS_AFFECTED(ch, AFF_HASTE))
    chance += 10;
  if (IS_SET(victim->off_flags, OFF_FAST) || IS_AFFECTED(victim, AFF_HASTE))
    chance -= 10;

  /* level */
  chance += (ch->level - victim->level);

  /* now the attack */
  if (number_percent() < chance)
  {

    act("$n has stuck a stake in your heart!", ch, NULL, victim, TO_VICT);
    act("You slam a stake into $N!", ch, NULL, victim, TO_CHAR);
    act("$n shoves a stake into $N .", ch, NULL, victim, TO_NOTVICT);
    check_improve(ch, gsn_stake, true, 1);

    DAZE_STATE(victim, 3 * PULSE_VIOLENCE);
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_stake].beats);
    victim->position = POS_RESTING;

    xdamage(ch, victim,
            ((ch->level * (dice((int) ch->level / 4, 6))) +
             ch->level), gsn_stake, DAM_PIERCE, true, VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;

  }
  else
  {
    act("You fall flat on your face!", ch, NULL, victim, TO_CHAR);
    act("$n falls flat on $s face.", ch, NULL, victim, TO_NOTVICT);
    act("You evade $n's stake, causing $m to fall flat on $s face.", ch,
        NULL, victim, TO_VICT);
    check_improve(ch, gsn_stake, false, 1);
    ch->position = POS_RESTING;
    if (!IS_IMMORTAL(ch))
      WAIT_STATE(ch, skill_table[gsn_stake].beats * 3 / 2);
    xdamage(ch, victim, 0, gsn_stake, DAM_PIERCE, true, VERBOSE_STD,
            &mobdeath);
    if (mobdeath)
      return;
  }
}

CH_CMD(do_strike)
{
  CHAR_DATA *victim;
  OBJ_DATA *obj;
  bool mobdeath = false;

  if (get_skill(ch, gsn_strike) == 0 ||
      (!IS_NPC(ch) && get_skill(ch, gsn_strike) < 2 &&
       ch->level < skill_table[gsn_strike].skill_level[ch->class]))
  {
    send_to_char("Strike? What's that?\n\r", ch);
    return;
  }

  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }

  if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
  {
    send_to_char("You need to wield a primary weapon to strike.\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (!can_see(ch, victim))
  {
    send_to_char("You need to see your opponent in order to strike.\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_strike].beats);
  if (number_percent() < get_skill(ch, gsn_strike) ||
      (get_skill(ch, gsn_strike) >= 2 && !IS_AWAKE(victim)))
  {
    check_improve(ch, gsn_circle, true, 1);
    act("{i$n strikes you with speed and accuracy!{x", ch, NULL, victim,
        TO_VICT);
    act("{hYou strike $N with speed and accuracy!{x", ch, NULL, victim,
        TO_CHAR);
    act("{k$n strikes $N with speed and accuracy!{x", ch, NULL, victim,
        TO_NOTVICT);
    multi_hit(ch, victim, gsn_strike, &mobdeath);
    if (mobdeath)
      return;
  }
  else
  {
    check_improve(ch, gsn_strike, false, 1);
    act("{i$n fails to strike you.{x", ch, NULL, victim, TO_VICT);
    act("{h$N dodges your strike.{x", ch, NULL, victim, TO_CHAR);
    act("{k$n fails to strike $N.{x", ch, NULL, victim, TO_NOTVICT);
    xdamage(ch, victim, 0, gsn_strike, DAM_NONE, true, VERBOSE_STD,
            &mobdeath);
    if (mobdeath)
      return;
  }

  return;
}

CH_CMD(do_nervestrike)
{
  CHAR_DATA *victim;
  int dam;
  bool mobdeath = false;

  if (!IS_NPC(ch) && get_skill(ch, gsn_nervestrike) < 2 &&
      ch->level < skill_table[gsn_nervestrike].skill_level[ch->class])
  {
    send_to_char("You had best leave that skill to master thieves.\n\r", ch);
    return;
  }
  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("Now that just wouldn't be right!\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_nervestrike].beats);

  if (get_skill(ch, gsn_nervestrike) < number_percent())
  {
    printf_to_char(ch,
                   "You prepare for a nervestrike but your opponent is too quick!\n\r");
    check_improve(ch, gsn_nervestrike, false, 1);
    return;
  }

  dam = ch->level * 40;

  check_improve(ch, gsn_nervestrike, true, 3);
  xdamage(ch, victim, number_range(dam, ch->level * 46), gsn_nervestrike,
          DAM_BASH, true, VERBOSE_STD, &mobdeath);
  if (mobdeath)
    return;

  if (chance(55) && can_see(ch, victim))
  {
    printf_to_char(ch,
                   "{YYour opponent is stunned by your {Rf{ru{Rr{ri{Ro{ru{Rs{Y{x nervestrike!{x\n\r");
    victim->stunned = 3;
  }

  return;
}

CH_CMD(do_thrust)
{
  OBJ_DATA *obj;
  CHAR_DATA *victim;
  int dam;
  bool mobdeath = false;

  if (!IS_NPC(ch) && get_skill(ch, gsn_thrust) < 2 &&
      ch->level < skill_table[gsn_thrust].skill_level[ch->class])
  {
    send_to_char("You better leave the martial arts to monks.\n\r", ch);
    return;
  }

  if ((victim = ch->fighting) == NULL)
  {
    send_to_char("You aren't fighting anyone.\n\r", ch);
    return;
  }

  if (ch->stunned)
  {
    send_to_char("You're still a little woozy.\n\r", ch);
    return;
  }

  if (!can_see(ch, victim))
  {
    send_to_char("You stumble blindly into a wall.\n\r", ch);
    return;
  }

  if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL)
  {
    send_to_char("You need to wield a primary weapon to thrust.\n\r", ch);
    return;
  }

  if (!can_see(ch, victim))
  {
    send_to_char("You stumble blindly into a wall.\n\r", ch);
    return;
  }

  dam = 50 + dice(100, 250);
  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_thrust].beats / 2);
  if (get_skill(ch, gsn_thrust) > number_percent())
  {
    check_improve(ch, gsn_thrust, true, 1);
    xdamage(ch, victim, number_range(dam, (ch->level * 55)),
            gsn_thrust, DAM_PIERCE, true, VERBOSE_STD, &mobdeath);
    if (mobdeath)
      return;
  }
  else
  {
    xdamage(ch, victim, 0, gsn_thrust, DAM_PIERCE, true, VERBOSE_STD,
            &mobdeath);
    if (mobdeath)
      return;
    check_improve(ch, gsn_thrust, false, 1);
  }
  return;
}

CH_CMD(do_whirlwind)
{
  CHAR_DATA *pChar;
  CHAR_DATA *pChar_next;
  OBJ_DATA *wield;
  bool found = false;
  bool mobdeath = false;

  if (!IS_NPC(ch) && get_skill(ch, gsn_whirlwind) < 2 &&
      ch->level < skill_table[gsn_whirlwind].skill_level[ch->class])
  {
    send_to_char("You don't know how to do that...\n\r", ch);
    return;
  }

  if ((wield = get_eq_char(ch, WEAR_WIELD)) == NULL)
  {
    send_to_char("You need to wield a weapon first...\n\r", ch);
    return;
  }

  act("$n holds $p firmly, and starts spinning round...", ch, wield, NULL,
      TO_ROOM);
  act("You hold $p firmly, and start spinning round...", ch, wield, NULL,
      TO_CHAR);
  check_improve(ch, gsn_whirlwind, true, 1);
  pChar_next = NULL;
  for (pChar = ch->in_room->people; pChar; pChar = pChar_next)
  {
    pChar_next = pChar->next_in_room;
    if (IS_NPC(pChar))
    {
      found = true;
      act("$n turns towards YOU!", ch, NULL, pChar, TO_VICT);
      multi_hit(ch, pChar, gsn_whirlwind, &mobdeath);
      if (mobdeath)
        return;
    }
    if (!IS_NPC(pChar))
    {
      found = true;
      act("$n turns towards YOU!", ch, NULL, pChar, TO_VICT);
      multi_hit(ch, pChar, gsn_whirlwind, &mobdeath);
      if (mobdeath)
        return;
    }

  }

  if (!found)
  {
    act("$n looks dizzy, and a tiny bit embarassed.", ch, NULL, NULL,
        TO_ROOM);
    act("You feel dizzy, and a tiny bit embarassed.", ch, NULL, NULL,
        TO_CHAR);
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_whirlwind].beats);

  if (!found && number_percent() < 25)
  {
    act("$n loses $s balance and falls into a heap.", ch, NULL, NULL,
        TO_ROOM);
    act("You lose your balance and fall into a heap.", ch, NULL, NULL,
        TO_CHAR);
    ch->position = POS_STUNNED;
  }

  return;
}

CH_CMD(do_call_wild)
{
  AFFECT_DATA af;

  if (get_skill(ch, gsn_call_wild) == 0 ||
      (!IS_NPC(ch) && get_skill(ch, gsn_call_wild) < 2 &&
       ch->level < skill_table[gsn_call_wild].skill_level[ch->class]))
  {
    send_to_char("You scream at the top of your lungs!\n\r", ch);
    return;
  }

  if (IS_AFFECTED(ch, AFF_CALM))
  {
    send_to_char("{hYou're feeling to mellow.{x\n\r", ch);
    return;
  }

  if (is_affected(ch, gsn_call_wild))
  {
    send_to_char("You are already as wild as possible.\n\r", ch);
    return;
  }

  if (!IS_IMMORTAL(ch))
    WAIT_STATE(ch, skill_table[gsn_call_wild].beats);

  if (number_percent() > ch->pcdata->learned[gsn_call_wild])
  {
    act("$n throws back $s head and howls loudly.", ch, NULL, NULL, TO_ROOM);
    check_improve(ch, gsn_call_wild, true, 1);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = gsn_call_wild;
  af.level = ch->level;
  af.duration = ch->level / 5;
  af.location = APPLY_AC;
  af.modifier = 0 - ch->level;
  af.bitvector = 0;
  affect_to_char(ch, &af);

  af.where = TO_AFFECTS;
  af.type = gsn_call_wild;
  af.level = ch->level;
  af.duration = ch->level / 5;
  af.location = APPLY_HITROLL;
  af.modifier = ch->level / 3;
  af.bitvector = 0;
  affect_to_char(ch, &af);

  af.where = TO_AFFECTS;
  af.type = gsn_call_wild;
  af.level = ch->level;
  af.duration = ch->level / 5;
  af.location = APPLY_DAMROLL;
  af.modifier = ch->level / 3;
  af.bitvector = 0;
  affect_to_char(ch, &af);
  send_to_char("You focus and feel the beast within come forth!\n\r", ch);
  return;
}

CH_CMD(do_mend)
{
  char arg[MIL];
  CHAR_DATA *victim;
  int heal;

  one_argument(argument, arg);

  if (!IS_NPC(ch) && get_skill(ch, gsn_mend) < 2 &&
      ch->level < skill_table[gsn_mend].skill_level[ch->class])
  {
    send_to_char("You don't know how to mend wounds.\n\r", ch);
    return;
  }

  if (arg[0] == '\0')
  {
    send_to_char("Mend whom?\n\r", ch);
    return;
  }
  else if ((victim = get_char_room(ch, arg)) == NULL)
  {
    send_to_char("They aren't here.\n\r", ch);
    return;
  }

  if (victim != ch)
  {
    if (victim->position != POS_SITTING && victim->position != POS_RESTING)
    {
      send_to_char
        ("You may only mend those who are in a comfortable position.\n\r",
         ch);
      return;
    }
  }

  heal = dice(50, 10) + (ch->level * 3);
  victim->hit = UMIN(victim->hit + heal, victim->max_hit);
  update_pos(victim);
  ch->move -= 500;
  send_to_char("Your wounds are mended.\n\r", victim);
  if (ch != victim)
    send_to_char("Ok.\n\r", ch);
  return;
}

void check_arena(CHAR_DATA * ch, CHAR_DATA * victim)
{
  DESCRIPTOR_DATA *d;           /* needed for Arena bet checking */
  char buf[MAX_STRING_LENGTH];
  float odds;
  float lvl1, lvl2;
  int payoff;

  if (IS_NPC(ch) || IS_NPC(victim))
    return;

  if (!spar)
    sprintf(buf, "{W[{RARENA{W] {R%s{W has defeated {R%s{W!{x\n\r", ch->name,
            victim->name);
  else
    sprintf(buf, "{W[{RSPAR{W] {R%s{W has defeated {R%s{W!{x\n\r", ch->name,
            victim->name);

  sprintf(lastwinner, "%s", ch->name);

  REMOVE_BIT(victim->act2, PLR2_MASS_ARENA);
  REMOVE_BIT(victim->act2, PLR2_MASS_JOINER);

  ch->stunned = 0;
  ch->position = POS_STANDING;

  ch->challenge_timer = 5;
  victim->challenge_timer = 5;
  arenacount = 0;
  do_gmessage(buf);

  if (!is_mass_arena && !spar)
  {
    ch->pcdata->awins += 1;
    victim->pcdata->alosses += 1;
  }
  lvl1 = ch->hit;
  lvl2 = victim->hit;
  odds = (lvl2 / lvl1);

  if (!is_mass_arena && !spar)
    for (d = descriptor_list; d; d = d->next)
    {
      if (d->connected == CON_PLAYING)
      {
        if (d->character->gladiator == ch)
        {
          payoff = d->character->pcdata->plr_wager * (odds + 1);
          payoff = abs(payoff);
          sprintf(buf,
                  "{WYou won! Your wager: {D%d{W, payoff: {D%d{x\n\r",
                  d->character->pcdata->plr_wager, payoff);
          send_to_char(buf, d->character);
          d->character->platinum += payoff;
          /* reset the betting info */
          d->character->gladiator = NULL;
          d->character->pcdata->plr_wager = 0;
          payoff = 0;
        }
        if (d->character->gladiator != ch &&
            d->character->pcdata->plr_wager >= 1)
        {
          int tmp = 0;

          sprintf(buf, "{WYou lost! Your wager: {D%d{x\n\r",
                  d->character->pcdata->plr_wager);
          send_to_char(buf, d->character);
          if (d->character->pcdata->plr_wager > d->character->exp)
          {
            tmp = d->character->pcdata->plr_wager;
            d->character->pcdata->plr_wager -= tmp;
          }
          if (tmp > 0)
            /* d->character->pcdata->quest -= tmp; */
            d->character->platinum -= d->character->pcdata->plr_wager;
          /* reset the betting info */
          d->character->gladiator = NULL;
          d->character->pcdata->plr_wager = 0;
        }
      }
    }

  /* now move both fighters out of arena and back to the regular "world" be
     sure to define ROOM_VNUM_AWINNER and ROOM_VNUM_ALOSER */
  stop_fighting(victim, true);
  char_from_room(victim);
  char_to_room(victim, get_room_index(ROOM_VNUM_ALOSER));
  victim->hit = victim->max_hit;
  victim->mana = victim->max_mana;
  update_pos(victim);
  do_look(victim, "auto");

  if (!is_mass_arena)
  {
    stop_fighting(ch, true);
    char_from_room(ch);
    char_to_room(ch, get_room_index(ROOM_VNUM_AWINNER));
    ch->hit = ch->max_hit;
    ch->mana = ch->max_mana;
    ch->move = ch->max_move;
    update_pos(ch);
    do_look(ch, "auto");

    if (IS_SET(ch->act2, PLR2_CHALLENGER))
      REMOVE_BIT(ch->act2, PLR2_CHALLENGER);
    if (IS_SET(ch->act2, PLR2_CHALLENGED))
      REMOVE_BIT(ch->act2, PLR2_CHALLENGED);
  }

  if (IS_SET(victim->act2, PLR2_CHALLENGER))
    REMOVE_BIT(victim->act2, PLR2_CHALLENGER);
  if (IS_SET(victim->act2, PLR2_CHALLENGED))
    REMOVE_BIT(victim->act2, PLR2_CHALLENGED);

  ch->challenger = NULL;
  ch->challenged = NULL;
  victim->challenger = NULL;
  victim->challenged = NULL;

  arena = FIGHT_OPEN;           /* clear the arena */
  spar = false;

  return;
}
