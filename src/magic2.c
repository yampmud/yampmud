
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
#include "magic.h"
#include "tables.h"
#include "recycle.h"

extern char *target_name;

MAGIC(spell_farsight)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_AFFECTED(victim, AFF_FARSIGHT))
  {
    if (victim == ch)
      send_to_char("Your eyes are already as good as they get.\n\r", ch);
    else
      act("$N can see just fine.", ch, NULL, victim, TO_CHAR);
    return;
  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = AFF_FARSIGHT;
  affect_to_char(victim, &af);
  send_to_char("Your eyes jump into focus.\n\r", victim);
  if (ch != victim)
    send_to_char("Ok.\n\r", ch);
  return;
}

MAGIC(spell_protection_voodoo)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_SHIELDED(victim, SHD_PROTECT_VOODOO))
  {
    return;
  }
  af.where = TO_SHIELDS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = SHD_PROTECT_VOODOO;
  affect_to_char(victim, &af);
  return;
}

MAGIC(spell_portal)
{
  CHAR_DATA *victim;
  OBJ_DATA *portal, *stone;

  if ((victim = get_char_world(ch, target_name)) == NULL || victim == ch
      || victim->in_room == NULL || !can_see_room(ch, victim->in_room) ||
      IS_SET(victim->in_room->room_flags, ROOM_SAFE) ||
      IS_SET(victim->in_room->room_flags, ROOM_PRIVATE) ||
      IS_SET(victim->in_room->room_flags, ROOM_SOLITARY) ||
      IS_SET(victim->in_room->room_flags, ROOM_NO_RECALL) ||
      IS_SET(ch->in_room->room_flags, ROOM_NO_RECALL)
      //    || (IS_NPC(victim) && is_gqmob(ch, victim->pIndexData->vnum) != -1) 
      || victim->level >= level + 3 || (!IS_NPC(victim) && victim->level >= LEVEL_ANCIENT)  /* NOT 
                                                                                               trust 
                                                                                             */
      || (IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_SUMMON)) ||
      (IS_NPC(victim) && saves_spell(level, victim, DAM_NONE)) ||
      (is_clan(victim) &&
       (!is_same_clan(ch, victim) || clan_table[victim->clan].independent)))
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }
  if (strstr(victim->in_room->area->builders, "Unlinked"))
  {
    send_to_char("You can't gate to areas that aren't linked yet!\n\r", ch);
    return;
  }

  stone = get_eq_char(ch, WEAR_HOLD);
  if (!IS_IMMORTAL(ch) &&
      (stone == NULL || stone->item_type != ITEM_WARP_STONE))
  {
    send_to_char("You lack the proper component for this spell.\n\r", ch);
    return;
  }

  if (stone != NULL && stone->item_type == ITEM_WARP_STONE)
  {
    act("You draw upon the power of $p.", ch, stone, NULL, TO_CHAR);
    act("It flares brightly and vanishes!", ch, stone, NULL, TO_CHAR);
    extract_obj(stone);
  }

  portal = create_object(get_obj_index(OBJ_VNUM_PORTAL), 0);
  portal->timer = 2 + level / 25;
  portal->value[3] = victim->in_room->vnum;

  obj_to_room(portal, ch->in_room);

  act("$p rises up from the ground.", ch, portal, NULL, TO_ROOM);
  act("$p rises up before you.", ch, portal, NULL, TO_CHAR);
}

MAGIC(spell_nexus)
{
  CHAR_DATA *victim;
  OBJ_DATA *portal, *stone;
  ROOM_INDEX_DATA *to_room, *from_room;

  from_room = ch->in_room;

  if ((victim = get_char_world(ch, target_name)) == NULL || victim == ch
      || (to_room = victim->in_room) == NULL ||
      !can_see_room(ch, to_room) || !can_see_room(ch, from_room) ||
      IS_SET(to_room->room_flags, ROOM_SAFE) ||
      IS_SET(from_room->room_flags, ROOM_SAFE) ||
      IS_SET(to_room->room_flags, ROOM_PRIVATE) ||
      IS_SET(to_room->room_flags, ROOM_SOLITARY) ||
      IS_SET(to_room->room_flags, ROOM_NO_RECALL) ||
      IS_SET(from_room->room_flags, ROOM_NO_RECALL)
      //    || (IS_NPC(victim) && is_gqmob(ch, victim->pIndexData->vnum) != -1)  
      || victim->level >= level + 3 || (!IS_NPC(victim) && victim->level >= LEVEL_ANCIENT)  /* NOT 
                                                                                               trust 
                                                                                             */
      || (IS_NPC(victim) && IS_SET(victim->imm_flags, IMM_SUMMON)) ||
      (IS_NPC(victim) && saves_spell(level, victim, DAM_NONE)) ||
      (is_clan(victim) &&
       (!is_same_clan(ch, victim) || clan_table[victim->clan].independent)))
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }
  if (strstr(victim->in_room->area->builders, "Unlinked"))
  {
    send_to_char("You can't gate to areas that aren't linked yet!\n\r", ch);
    return;
  }

  stone = get_eq_char(ch, WEAR_HOLD);
  if (!IS_IMMORTAL(ch) &&
      (stone == NULL || stone->item_type != ITEM_WARP_STONE))
  {
    send_to_char("You lack the proper component for this spell.\n\r", ch);
    return;
  }

  if (stone != NULL && stone->item_type == ITEM_WARP_STONE)
  {
    act("You draw upon the power of $p.", ch, stone, NULL, TO_CHAR);
    act("It flares brightly and vanishes!", ch, stone, NULL, TO_CHAR);
    extract_obj(stone);
  }

  /* portal one */
  portal = create_object(get_obj_index(OBJ_VNUM_PORTAL), 0);
  portal->timer = 1 + level / 10;
  portal->value[3] = to_room->vnum;

  obj_to_room(portal, from_room);

  act("$p rises up from the ground.", ch, portal, NULL, TO_ROOM);
  act("$p rises up before you.", ch, portal, NULL, TO_CHAR);

  /* no second portal if rooms are the same */
  if (to_room == from_room)
    return;

  /* portal two */
  portal = create_object(get_obj_index(OBJ_VNUM_PORTAL), 0);
  portal->timer = 1 + level / 10;
  portal->value[3] = from_room->vnum;

  obj_to_room(portal, to_room);

  if (to_room->people != NULL)
  {
    act("$p rises up from the ground.", to_room->people, portal, NULL,
        TO_ROOM);
    act("$p rises up from the ground.", to_room->people, portal, NULL,
        TO_CHAR);
  }
}

MAGIC(spell_empower)
{
  OBJ_DATA *object;
  char buf[MAX_STRING_LENGTH];
  char *name;
  int new_sn;
  int mana;
  int newmana;
  int newtarget;

  if ((new_sn = find_spell(ch, target_name)) < 0 ||
      (!IS_NPC(ch) &&
       (ch->level < skill_table[new_sn].skill_level[ch->class] ||
        ch->pcdata->learned[new_sn] == 0)))
  {
    send_to_char("What spell do you wish to bind?\n\r", ch);
    return;
  }

  if (skill_table[new_sn].skill_level[ch->class] >= 201 && !IS_IMMORTAL(ch))
  {
    send_to_char("You may not empower that spell.\n\r", ch);
    return;
  }

  name = skill_table[new_sn].name;
  if (!str_cmp(name, "empower"))
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }
  newtarget = skill_table[new_sn].target;
  if (ch->level + 2 == skill_table[sn].skill_level[ch->class])
    mana = 50;
  else
    mana =
      UMAX(skill_table[sn].min_mana,
           100 / (2 + ch->level - skill_table[sn].skill_level[ch->class]));

  if (ch->level + 2 == skill_table[new_sn].skill_level[ch->class])
    newmana = 50;
  else
    newmana =
      UMAX(skill_table[new_sn].min_mana,
           100 / (2 + ch->level -
                  skill_table[new_sn].skill_level[ch->class]));

  if ((ch->mana - mana - newmana) < 0)
  {
    send_to_char("You do not have enough mana.\n\r", ch);
    return;
  }
  ch->mana -= newmana;

  if ((newtarget == TAR_CHAR_DEFENSIVE) || (newtarget == TAR_CHAR_SELF))
  {
    object = create_object(get_obj_index(OBJ_VNUM_POTION), 0);
  }
  else
  {
    object = create_object(get_obj_index(OBJ_VNUM_SCROLL), 0);
  }
  object->value[0] = ch->level - 20;
  object->value[1] = new_sn;
  object->level = ch->level - 20;

  sprintf(buf, "%s%s", object->short_descr, name);
  free_string(object->short_descr);
  object->short_descr = str_dup(buf);

  free_string(object->name);
  object->name = str_dup(name);

  if ((newtarget == TAR_CHAR_DEFENSIVE) || (newtarget == TAR_CHAR_SELF))
  {
    sprintf(buf, "$n has created a potion of %s!", name);
    act(buf, ch, object, NULL, TO_ROOM);
    sprintf(buf, "You create a potion of %s!\n\r", name);
    send_to_char(buf, ch);
  }
  else
  {
    sprintf(buf, "$n has created a scroll of %s!", name);
    act(buf, ch, object, NULL, TO_ROOM);
    sprintf(buf, "You create a scroll of %s!\n\r", name);
    send_to_char(buf, ch);
  }
  obj_to_char(object, ch);
  return;
}

/* *REPLACED*
   MAGIC ( spell_resurrect )
   {
   char buf[MAX_STRING_LENGTH];
   char arg[MAX_STRING_LENGTH];
   MOB_INDEX_DATA *pMobIndex;
   OBJ_DATA *obj;
   OBJ_DATA *cobj;
   OBJ_DATA *obj_next;
   CHAR_DATA *pet;
   int length;

   if ( ( obj = get_obj_here ( ch, "corpse" ) ) == NULL )
   {
   send_to_char ( "There's no corpse here.\n\r", ch );
   return;
   }
   if ( ch->pet != NULL )
   {
   send_to_char ( "You failed.\n\r", ch );
   return;
   }
   pMobIndex = get_mob_index ( MOB_VNUM_CORPSE );
   pet = create_mobile ( pMobIndex );
   if ( !IS_SET ( pet->act, ACT_PET ) )
   SET_BIT ( pet->act, ACT_PET );
   if ( !IS_SET ( pet->affected_by, AFF_CHARM ) )
   SET_BIT ( pet->affected_by, AFF_CHARM );
   pet->comm = COMM_NOTELL | COMM_NOSHOUT | COMM_NOCHANNELS;
   sprintf ( buf, "%s{GThe mark of %s is on it's forehead.{x.\n\r",
   pet->description, ch->name );
   free_string ( pet->description );
   pet->description = str_dup ( buf );
   free_string ( pet->short_descr );
   pet->short_descr =
   str_dup ( str_replace ( obj->short_descr, "corpse", "zombie" ) );
   sprintf ( buf, "%s", str_replace ( obj->description, "corpse", "zombie" ) );
   length = strlen ( buf ) - 12;
   strncpy ( arg, buf, length );
   arg[length] = '\0';
   sprintf ( buf, "%s standing here.\n\r", arg );
   free_string ( pet->long_descr );
   pet->long_descr = str_dup ( buf );
   char_to_room ( pet, ch->in_room );
   pet->ptype = 1;
   add_follower ( pet, ch );
   pet->leader = ch;
   ch->pet = pet;
   pet->alignment = ch->alignment;
   pet->level =
   UMAX ( 1, UMIN ( 109, ( ( ch->level / 2 ) + ( obj->level / 2 ) ) ) );
   pet->max_hit = pet->level * 15;
   pet->hit = pet->max_hit;
   pet->armor[0] = pet->level / 6;
   pet->armor[1] = pet->level / 6;
   pet->armor[2] = pet->level / 6;
   pet->armor[3] = pet->level / 8;
   for ( cobj = obj->contains; cobj != NULL; cobj = obj_next )
   {
   obj_next = cobj->next_content;
   obj_from_obj ( cobj );
   obj_to_room ( cobj, ch->in_room );
   }
   extract_obj ( obj );
   sprintf ( buf, "%s stands up and starts following you.\n\r",
   pet->short_descr );
   send_to_char ( buf, ch );
   sprintf ( buf, "%s stands up and starts following $n.", pet->short_descr );
   act ( buf, ch, NULL, NULL, TO_ROOM );
   return;
   } */

MAGIC(spell_conjure)
{
  char buf[MAX_STRING_LENGTH];
  MOB_INDEX_DATA *pMobIndex;
  OBJ_DATA *stone;
  CHAR_DATA *pet;

  if (IS_NPC(ch))
    return;

  stone = get_eq_char(ch, WEAR_HOLD);
  if (!IS_IMMORTAL(ch) &&
      (stone == NULL || stone->item_type != ITEM_DEMON_STONE))
  {
    send_to_char("You lack the proper component for this spell.\n\r", ch);
    return;
  }

  if (ch->pet != NULL)
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }
  /*    if ( stone != NULL && stone->item_type == ITEM_DEMON_STONE )
     {
     act ( "You draw upon the power of $p.", ch, stone, NULL,
     TO_CHAR );
     act ( "$n draws upon the power of $p.", ch, stone, NULL,
     TO_ROOM );
     act ( "It flares brightly and explodes into dust.", ch, stone,
     NULL, TO_CHAR );
     act ( "It flares brightly and explodes into dust.", ch, stone,
     NULL, TO_ROOM );
     extract_obj ( stone );
     return;
     } */

  if (ch->familiar != NULL)
  {
    send_to_char("You can only have one familiar.\n\r", ch);
    return;
  }

  pMobIndex = get_mob_index(MOB_VNUM_DEMON);
  pet = create_mobile(pMobIndex);
  if (!IS_SET(pet->act, ACT_PET))
    SET_BIT(pet->act, ACT_PET);
  if (!IS_SET(pet->affected_by, AFF_CHARM))
    SET_BIT(pet->affected_by, AFF_CHARM);
  pet->comm = COMM_NOTELL | COMM_NOSHOUT | COMM_NOCHANNELS;
  sprintf(buf, "%s{GThe mark of %s is on it's forehead.{x.\n\r",
          pet->description, ch->name);
  free_string(pet->description);
  pet->description = str_dup(buf);
  char_to_room(pet, ch->in_room);
  if (stone != NULL && stone->item_type == ITEM_DEMON_STONE)
  {
    act("You draw upon the power of $p.", ch, stone, NULL, TO_CHAR);
    act("$n draws upon the power of $p.", ch, stone, NULL, TO_ROOM);
    act("It flares brightly and $N appears.", ch, stone, pet, TO_CHAR);
    act("It flares brightly and $N appears.", ch, stone, pet, TO_ROOM);
  }
  else
  {
    act("$N suddenly appears in the room.", ch, NULL, pet, TO_CHAR);
    act("$N suddenly appears in the room.", ch, NULL, pet, TO_ROOM);
  }
  pet->ptype = 1;
  add_follower(pet, ch);
  pet->leader = ch;
  ch->familiar = pet;
  pet->alignment = ch->alignment;
  pet->level = ch->level;
  pet->max_hit = pet->level * 30;
  pet->hit = pet->max_hit;
  pet->armor[0] = pet->level / 2;
  pet->armor[1] = pet->level / 2;
  pet->armor[2] = pet->level / 2;
  pet->armor[3] = pet->level / 3;

  return;
}

MAGIC(spell_animate)
{
  char buf[MAX_STRING_LENGTH];
  char arg[MAX_STRING_LENGTH];
  MOB_INDEX_DATA *pMobIndex;
  OBJ_DATA *obj = (OBJ_DATA *) vo;
  CHAR_DATA *pet;
  CHAR_DATA *gch;
  int length;
  int part;

  part = 0;
  for (gch = char_list; gch != NULL; gch = gch->next)
  {
    if (is_same_group(gch, ch) && IS_NPC(gch))
    {
      if (gch->pIndexData->vnum == 3175)
        part++;
    }
  }
  if (part >= 6)
  {
    send_to_char("You can only control 6 animated parts at once.\n\r", ch);
    return;
  }

  if ((obj->pIndexData->vnum > 17) || (obj->pIndexData->vnum < 12))
  {
    send_to_char("That's not a body part!\n\r", ch);
    return;
  }

  pMobIndex = get_mob_index(MOB_VNUM_ANIMATE);
  pet = create_mobile(pMobIndex);
  SET_BIT(pet->affected_by, AFF_CHARM);
  pet->comm = COMM_NOTELL | COMM_NOSHOUT | COMM_NOCHANNELS;
  sprintf(buf, "%s{GIt's branded with the mark of %s.{x.\n\r",
          obj->description, ch->name);
  free_string(pet->description);
  pet->description = str_dup(buf);
  free_string(pet->short_descr);
  pet->short_descr = str_dup(obj->short_descr);
  free_string(pet->name);
  pet->name = str_dup(obj->name);
  sprintf(buf, "%s", obj->description);
  length = strlen(buf) - 12;
  strncpy(arg, buf, length);
  arg[length] = '\0';
  sprintf(buf, "%s floating here.\n\r", arg);
  free_string(pet->long_descr);
  pet->long_descr = str_dup(buf);
  char_to_room(pet, ch->in_room);
  pet->ptype = 1;
  add_follower(pet, ch);
  pet->leader = ch;
  obj_from_char(obj);
  sprintf(buf, "%s floats up and starts following you.\n\r",
          pet->short_descr);
  send_to_char(buf, ch);
  sprintf(buf, "%s floats up and starts following $n.", pet->short_descr);
  act(buf, ch, NULL, NULL, TO_ROOM);
  return;
}

MAGIC(spell_iceshield)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_SHIELDED(victim, SHD_ICE))
  {
    if (victim == ch)
      send_to_char
        ("You are already surrounded by an {Cicy{x shield.\n\r", ch);
    else
      act("$N is already surrounded by an {Cicy{x shield.", ch, NULL,
          victim, TO_CHAR);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }

  /*
     if ( (skill_table[sn].skill_level[victim->class] > LEVEL_ANCIENT)
     && (victim->level < LEVEL_IMMORTAL) )
     {
     send_to_char("You are surrounded by an {Cicy{x shield.\n\r", victim);
     act("$n is surrounded by an {Cicy{x shield.",victim, NULL,NULL,TO_ROOM);
     send_to_char("Your {Cicy{x shield quickly melts away.\n\r", victim);
     act("$n's {Cicy{x shield quickly melts away.",victim, NULL,NULL,TO_ROOM);
     return;
     }
   */

  af.where = TO_SHIELDS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = SHD_ICE;

  affect_to_char(victim, &af);
  send_to_char("You are surrounded by an {Cicy{x shield.\n\r", victim);
  act("$n is surrounded by an {Cicy{x shield.", victim, NULL, NULL, TO_ROOM);
  return;
}

MAGIC(spell_fireshield)
{

  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_SHIELDED(victim, SHD_FIRE))
  {
    if (victim == ch)
      send_to_char
        ("You are already surrounded by a {Rfirey{x shield.\r\n", ch);
    else
      act("$N is already surrounded by a {Rfiery{x shield.", ch, NULL,
          victim, TO_CHAR);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }

  /*
     if ( (skill_table[sn].skill_level[victim->class] > LEVEL_ANCIENT)
     && (victim->level < LEVEL_IMMORTAL) )
     {
     send_to_char("You are surrounded by a {Rfiery{x shield.\n\r", victim);
     act("$n is surrounded by a {Rfiery{x shield.",victim, NULL,NULL,TO_ROOM);
     send_to_char("Your {Rfirey{x shield gutters out.\n\r", victim);
     act("$n's {Rfirey{x shield gutters out.",victim, NULL,NULL,TO_ROOM);
     return;
     }
   */

  af.where = TO_SHIELDS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = SHD_FIRE;

  affect_to_char(victim, &af);
  send_to_char("You are surrounded by a {Rfiery{x shield.\n\r", victim);
  act("$n is surrounded by a {Rfiery{x shield.", victim, NULL, NULL, TO_ROOM);
  return;

}

MAGIC(spell_shockshield)
{

  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_SHIELDED(victim, SHD_SHOCK))
  {
    if (victim == ch)
      send_to_char
        ("You are already surrounded in a {Bcrackling{x shield.\n\r", ch);
    else
      act("$N is already surrounded by a {Bcrackling{x shield.", ch,
          NULL, victim, TO_CHAR);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }

  /*
     if ( (skill_table[sn].skill_level[victim->class] > LEVEL_ANCIENT)
     && (victim->level < LEVEL_IMMORTAL) )
     {
     send_to_char("You are surrounded by a {Bcrackling{x shield.\n\r", victim);
     act("$n is surrounded by a {Bcrackling{x shield.",victim, NULL,NULL,TO_ROOM);
     send_to_char("Your {Bcrackling{x shield sizzles and fades.\n\r", victim);
     act("$n's {Bcrackling{x shield sizzles and fades.",victim, NULL,NULL,TO_ROOM);
     return;
     }
   */

  af.where = TO_SHIELDS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = SHD_SHOCK;

  affect_to_char(victim, &af);
  send_to_char("You are surrounded by a {Bcrackling{x field.\n\r", victim);
  act("$n is surrounded by a {Bcrackling{x shield.", victim, NULL, NULL,
      TO_ROOM);
  return;
}

MAGIC(spell_acidshield)
{

  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_SHIELDED(victim, SHD_ACID))
  {
    if (victim == ch)
      send_to_char
        ("You are already surrounded in an {GA{gcidic{x shield.\n\r", ch);
    else
      act("$N is already surrounded by an {GA{gcidic{x shield.", ch,
          NULL, victim, TO_CHAR);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }

  af.where = TO_SHIELDS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = SHD_ACID;

  affect_to_char(victim, &af);
  send_to_char("You are surrounded by an {GA{gcidic{x field.\n\r", victim);
  act("$n is surrounded by an {GA{gcidic{x shield.", victim, NULL, NULL,
      TO_ROOM);
  return;
}

MAGIC(spell_poisonshield)
{

  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_SHIELDED(victim, SHD_POISON))
  {
    if (victim == ch)
      send_to_char
        ("You are already surrounded in a {MP{moisonous{x {CC{cl{Co{cu{Cd{x.\n\r",
         ch);
    else
      act("$N is already surrounded by a {MP{moisonous{x {CC{cl{Co{cu{Cd{x.",
          ch, NULL, victim, TO_CHAR);
    return;
  }
  if (IS_NPC(victim))
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }

  af.where = TO_SHIELDS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = SHD_POISON;

  affect_to_char(victim, &af);
  send_to_char
    ("You are surrounded by a {MP{moisonous{x {CC{cl{Co{cu{Cd{x.\n\r",
     victim);
  act("$n is surrounded by a {MP{moisonous{x {CC{cl{Co{cu{Cd{x.", victim,
      NULL, NULL, TO_ROOM);
  return;
}

MAGIC(spell_quest_pill)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;

  if (IS_NPC(victim))
    return;

  victim->qps++;
  send_to_char("{YYou've gained a {RQuest Point{Y!{x\n\r", victim);
  if (ch != victim)
    send_to_char("Ok.\n\r", ch);
  return;
}

MAGIC(spell_voodoo)
{
  char name[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *bpart;
  OBJ_DATA *doll;

  bpart = get_eq_char(ch, WEAR_HOLD);
  if ((bpart == NULL) || (bpart->pIndexData->vnum < 12) ||
      (bpart->pIndexData->vnum > 17))
  {
    send_to_char("You are not holding a body part.\n\r", ch);
    return;
  }
  if (bpart->value[4] == 0)
  {
    send_to_char("This body part is from a mobile.\n\r", ch);
    return;
  }
  one_argument(bpart->name, name);
  doll = create_object(get_obj_index(OBJ_VNUM_VOODOO), 0);
  sprintf(buf, doll->short_descr, name);
  free_string(doll->short_descr);
  doll->short_descr = str_dup(buf);
  sprintf(buf, doll->description, name);
  free_string(doll->description);
  doll->description = str_dup(buf);
  sprintf(buf, doll->name, name);
  free_string(doll->name);
  doll->name = str_dup(buf);
  act("$p morphs into a voodoo doll", ch, bpart, NULL, TO_CHAR);
  obj_from_char(bpart);
  obj_to_char(doll, ch);
  equip_char(ch, doll, WEAR_HOLD);
  act("$n has created $p!", ch, doll, NULL, TO_ROOM);
  return;
}

MAGIC(spell_napalm)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  dam = ch->level * 10 + dice(15, 30);
  if (saves_spell(level, victim, DAM_DISEASE))
    dam /= 1.5;
  else
    spell_plague(skill_lookup("plague"), level, ch, (void *) victim,
                 TARGET_CHAR);

  damage(ch, victim, dam + (dam / 2), sn, DAM_FIRE, true);
  damage(ch, victim, dam + (dam / 4), sn, DAM_DISEASE, true);
  damage(ch, victim, dam, sn, DAM_FIRE, true);
  return;
}

MAGIC(spell_dragon_skin)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn))
  {
    if (victim == ch)
      send_to_char("Your skin is already hard as a dragons.\n\r", ch);
    else
      act("$N's skin is already hard as a dragons.", ch, NULL, victim,
          TO_CHAR);
    return;
  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_AC;
  af.modifier = -(2 * level);
  af.bitvector = 0;
  affect_to_char(victim, &af);
  act("$n's skin is now hard as a dragons.", victim, NULL, NULL, TO_ROOM);
  send_to_char("Your skin is now hard as a dragons.\n\r", victim);
  return;
}

MAGIC(spell_dragon_wisdom)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn))
  {
    if (victim == ch)
      send_to_char("You are already as wise as a dragon!\n\r", ch);
    else
      act("$N can't get any wiser.", ch, NULL, victim, TO_CHAR);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_WIS;
  af.modifier = 1 + (level >= 18) + (level >= 25) + (level >= 32);
  af.bitvector = 0;
  affect_to_char(victim, &af);
  send_to_char("Your brain surges with heightened wisdom!\n\r", victim);
  act("$n's brain surges with heightened wisdom.", victim, NULL, NULL,
      TO_ROOM);
  return;
}

MAGIC(spell_briarshield)
{

  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (IS_SHIELDED(victim, SHD_BRIAR))
  {
    if (victim == ch)
      send_to_char("You are already surrounded in a thorny shield.\n\r", ch);
    else
      act("$N is already surrounded by a thorny shield.", ch, NULL,
          victim, TO_CHAR);
    return;
  }

  if (IS_NPC(victim))
  {
    send_to_char("You failed.\n\r", ch);
    return;
  }

  /*
     if ( (skill_table[sn].skill_level[victim->class] > LEVEL_ANCIENT)
     && (victim->level < LEVEL_IMMORTAL) )
     {
     send_to_char("You are surrounded by a {Bcrackling{x shield.\n\r", victim);
     act("$n is surrounded by a {Bcrackling{x shield.",victim, NULL,NULL,TO_ROOM);
     send_to_char("Your {Bcrackling{x shield sizzles and fades.\n\r", victim);
     act("$n's {Bcrackling{x shield sizzles and fades.",victim, NULL,NULL,TO_ROOM);
     return;
     }
   */

  af.where = TO_SHIELDS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = SHD_BRIAR;

  affect_to_char(victim, &af);
  send_to_char("You are surrounded by a thorny shield.\n\r", victim);
  act("$n is surrounded by a thorny shield.", victim, NULL, NULL, TO_ROOM);
  return;
}

MAGIC(spell_bloodbath)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;
  OBJ_DATA *blood;

  blood = get_obj_here(ch, "blood");
  if (blood == NULL)
  {
    send_to_char
      ("There is not enough {Rb{rloo{Rd{x here to fuel this spell.\n\r", ch);
    return;
  }

  if (is_affected(victim, sn))
  {
    send_to_char("Your body is already bathed in {Rb{rloo{Rd{x.\n\r", ch);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level / 3;
  af.bitvector = 0;

  af.location = APPLY_DAMROLL;
  af.modifier = level / 3.5;
  affect_to_char(victim, &af);

  af.location = APPLY_HITROLL;
  af.modifier = level / 3.5;
  affect_to_char(victim, &af);

  af.location = APPLY_AC;
  af.modifier = -100 - level;
  affect_to_char(victim, &af);

  act
    ("Fresh {=b{=l{=o{=o{=d{x swirls up from the ground and is absorbed into your skin.",
     ch, NULL, victim, TO_CHAR);
  act
    ("Fresh {=b{=l{=o{=o{=d{x swirls up from the ground and is absorbed into $n's skin.",
     ch, NULL, victim, TO_NOTVICT);

  extract_obj(blood);

  send_to_char
    ("Your body weakens as it assimilates the fresh {Rb{rloo{Rd{x.\n\r", ch);
  ch->hit -= level * 3;
  ch->move -= level * 2;

  return;
}

MAGIC(spell_camouflage)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn))
  {
    if (victim == ch)
      send_to_char("You are already hidden.\n\r", ch);
    else
      act("$N is already hidden.", ch, NULL, victim, TO_CHAR);
    return;
  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = ch->level / 6;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = AFF_SNEAK;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = ch->level / 6;
  af.location = APPLY_AC;
  af.modifier = 0 - (ch->level / 5);
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = ch->level / 6;
  af.location = APPLY_HITROLL;
  af.modifier = ch->level / 5;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = ch->level / 6;
  af.location = APPLY_DAMROLL;
  af.modifier = ch->level / 5;
  af.bitvector = 0;
  affect_to_char(victim, &af);
  act("$n {&fa{7de{8s in{6to the {3ba{6ck{2gr{6ou{3nd.{x", victim, NULL,
      NULL, TO_ROOM);
  send_to_char("You {7fa{8de {6into t{3he b{6ack{3gro{6und.{x\n\r", victim);
  return;
}

MAGIC(spell_divine_protection)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn))
  {
    if (victim == ch)
      send_to_char("You are already protected.\n\r", ch);
    else
      act("$N is already protected.", ch, NULL, victim, TO_CHAR);
    return;

  }
  af.where = TO_SHIELDS;
  af.type = sn;
  af.level = level;
  af.duration = level / 1.5;
  af.bitvector = SHD_DIVINE_PROTECTION;

  af.location = APPLY_DAMROLL;
  af.modifier = level / 6;
  affect_to_char(victim, &af);

  af.location = APPLY_HITROLL;
  af.modifier = level / 6;
  affect_to_char(victim, &af);

  af.location = APPLY_SAVES;
  af.modifier = 0 - level / 8;
  affect_to_char(victim, &af);

  af.location = APPLY_AC;
  af.modifier = 0 - level / 1.5;
  affect_to_char(victim, &af);

  send_to_char("The very essense of the gods protects you!\n\r", victim);
  if (ch != victim)
    act("$N is protected by your gods.", ch, NULL, victim, TO_CHAR);
  return;
}

MAGIC(spell_superior_healing)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int bonus = 1000 + level + 600 + dice(1, 25);

  victim->hit = UMIN(victim->hit + bonus, victim->max_hit);
  update_pos(victim);
  send_to_char("A warm feeling fills your body.\n\r", victim);
  if (ch != victim)
    send_to_char("Ok.\n\r", ch);
  return;
}

MAGIC(spell_master_healing)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int bonus = 1500 + level + 1500 + dice(1, 40);

  victim->hit = UMIN(victim->hit + bonus, victim->max_hit);
  update_pos(victim);
  send_to_char("A warm feeling fills your body.\n\r", victim);
  if (ch != victim)
    send_to_char("Ok.\n\r", ch);
  return;
}

MAGIC(spell_anger)
  /*1 */
{
  CHAR_DATA *victim = (CHAR_DATA *) vo; /* 2a */
  AFFECT_DATA af;               /* 2b */

  if (is_affected(ch, sn))      /* 3 */
  {
    if (victim == ch)
      send_to_char("You are already angry\n\r", ch);  /* 4 */
    else
      act("$N is filled with anger.", ch, NULL, victim, TO_CHAR); /* 5 
                                                                   */
    return;
  }

  af.where = TO_AFFECTS;        /* 6 */
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_DAMROLL;
  af.modifier = level / 4.5;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_HITROLL;
  af.modifier = level / 4.5;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_AC;
  af.modifier = level * .8;
  af.bitvector = 0;
  affect_to_char(victim, &af);
  act("$n is filled with anger.", victim, NULL, NULL, TO_ROOM);
  send_to_char("You are filled with anger.\n\r", ch);
  return;
}

MAGIC(spell_sate)
{

  CHAR_DATA *victim = (CHAR_DATA *) vo;

  victim->pcdata->condition[COND_HUNGER] = 24;
  update_pos(victim);
  send_to_char("Your hunger is sated.\n\r", victim);
  if (ch != victim)
    send_to_char("Ok.\n\r", ch);
  return;

}

MAGIC(spell_quench)
{

  CHAR_DATA *victim = (CHAR_DATA *) vo;

  victim->pcdata->condition[COND_THIRST] = 30;
  update_pos(victim);
  send_to_char("Your thirst is quenched.\n\r", victim);
  if (ch != victim)
    send_to_char("Ok.\n\r", ch);
  return;

}

MAGIC(spell_heroes_feast)
{
  CHAR_DATA *gch;
  int sate_num, quench_num;

  sate_num = skill_lookup("sate");
  quench_num = skill_lookup("quench");

  for (gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room)
  {
    if ((IS_NPC(ch) && IS_NPC(gch)) || (!IS_NPC(ch) && !IS_NPC(gch)))
    {
      spell_sate(sate_num, level, ch, (void *) gch, TARGET_CHAR);
      spell_quench(quench_num, level, ch, (void *) gch, TARGET_CHAR);
    }
  }
}

MAGIC(spell_cry)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn))
  {
    if (victim == ch)
      send_to_char("You are already crying.\n\r", ch);
    else
      act("$N is already crying.", ch, NULL, victim, TO_CHAR);
    return;
  }

  if (saves_spell(level * 2, victim, DAM_MENTAL))
  {
    act("$N isn't affect by your mental abuse.", ch, NULL, victim, TO_CHAR);
    return;
  }

  damage_old(ch, victim, dice(ch->level * 8, 10), sn, DAM_MENTAL, true);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = ch->level / 40;
  af.location = APPLY_HITROLL;
  af.modifier = 0 - ch->level / 10;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = ch->level / 40;
  af.location = APPLY_DAMROLL;
  af.modifier = 0 - ch->level / 15;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = ch->level / 40;
  af.location = APPLY_DEX;
  af.modifier = 0 - ch->level / 33;
  af.bitvector = 0;
  affect_to_char(victim, &af);
  send_to_char("You burst into tears.", victim);
  act("$n bursts into tears.", victim, NULL, NULL, TO_ROOM);

  return;
}

MAGIC(spell_blind_rage)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(ch, sn))
  {
    if (victim == ch)
      send_to_char("You are allready in a blind {RR{ra{RG{re{x\n\r", ch);
    else
      act("$N is allready in a blind {RR{ra{RG{re{x.", ch, NULL, victim,
          TO_CHAR);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_DAMROLL;
  af.modifier = level * .75;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_HITROLL;
  af.modifier = level * .75;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_AC;
  af.modifier = level / .75;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_STR;
  af.modifier = 1 + level / 25;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  act("$n is filled with {RRAGE{x.", victim, NULL, NULL, TO_ROOM);
  send_to_char("You are filled with {RRAGE{x.\n\r", ch);
  return;
}

MAGIC(spell_gods_armor)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn))
  {
    if (victim == ch)
      send_to_char("You allready have armor like the gods.\n\r", ch);
    else
      act("$N allready has armor like the guards", ch, NULL, victim, TO_CHAR);
    return;
  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = 20;
  af.modifier = 0 - level * 1.55;
  af.location = APPLY_AC;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  send_to_char("You feel the armor of the gods wrap around you.\n\r", victim);
  if (ch != victim)
    act("$N is now protected by the armor of the gods", ch, NULL, victim,
        TO_CHAR);
  return;
}

MAGIC(spell_blade_bless)
{
  OBJ_DATA *obj;
  AFFECT_DATA af;

  if (target == TARGET_OBJ)
  {
    obj = (OBJ_DATA *) vo;
    if (IS_OBJ_STAT(obj, ITEM_BLESS))
    {
      act("$p is already blessed.", ch, obj, NULL, TO_CHAR);
      return;
    }

    if (obj->item_type != ITEM_WEAPON)
    {
      send_to_char("That isn't a weapon.\n\r", ch);
      return;
    }
    /* if (IS_OBJ_STAT(obj,ITEM_EVIL)) { AFFECT_DATA *paf;

       paf = affect_find(obj->affected,gsn_curse); if
       (!saves_dispel(level,paf != NULL ? paf->level : obj->level,0)) { if
       (paf != NULL) affect_remove_obj(obj,paf); act("$p glows soft
       gold.",ch,obj,NULL,TO_ALL); REMOVE_BIT(obj->extra_flags,ITEM_EVIL);
       return; } else { act("The evil of $p resists your blessings.",
       ch,obj,NULL,TO_CHAR); return; } } */

    af.where = TO_OBJECT;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.bitvector = ITEM_BLESS;

    af.location = APPLY_HITROLL;
    af.modifier = level / 6;
    affect_to_obj(obj, &af);

    af.location = APPLY_DAMROLL;
    af.modifier = level / 6;
    affect_to_obj(obj, &af);

    act("$p glows a brilliant gold!", ch, obj, NULL, TO_ALL);
    return;
  }
}

MAGIC(spell_invigorate)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(ch, sn))
  {
    if (victim == ch)
      send_to_char("{WYou are allread invigorated! {x\n\r", ch);
    else
      act("$N is allready invigorated! ", ch, NULL, victim, TO_CHAR);
    return;
  }

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = 6;
  af.location = APPLY_HIT;
  af.modifier = level * 10;
  af.bitvector = 0;
  affect_to_char(victim, &af);

  act("$n is invigorated!", victim, NULL, NULL, TO_ROOM);
  /* send_to_char("You are invigorated!\n\r",ch); */

  //    ch->hit = ch->max_hit;
  return;
}

MAGIC(spell_create_bed)
{
  OBJ_DATA *bed;

  bed = create_object(get_obj_index(OBJ_VNUM_BED), 0);
  bed->value[3] = level * 3;    /* hp heal rate */
  bed->value[4] = level * 3;    /* mana heal rate */
  bed->timer = level / 5;
  obj_to_room(bed, ch->in_room);
  act("$n begin to wave $s hands.\n\r"
      "The wind starts to blow leaves around and a small cylone forms.\n\r"
      "As the cyclone dies down, a small bed of leaves is left on the ground.",
      ch, bed, NULL, TO_ROOM);
  act("You begin to wave your hands.\n\r"
      "The wind starts to blow leaves around and a small cylone begins to form.\n\r"
      "As the cyclone dies down a small bed of leaves is left on the ground.",
      ch, bed, NULL, TO_CHAR);
  return;
}

MAGIC(spell_immolation)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn))
  {
    if (victim == ch)
      send_to_char
        ("You have allready sacraficed your self to the {Rf{ri{Rr{re{Rs {xof {Rh{Ye{Rl{Yl{x!\n\r",
         ch);
    else
      act
        ("$N has allready sacraficed himself to the {Rf{ri{Rr{re{rs{x of {Rh{Ye{Rl{Yl{x!",
         ch, NULL, victim, TO_CHAR);
    return;
  }

  af.where = TO_RESIST;
  af.type = sn;
  af.level = level;
  af.duration = 4;

  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = RES_FIRE;
  affect_to_char(victim, &af);

  af.location = APPLY_DAMROLL;
  af.modifier = 4;

  affect_to_char(victim, &af);

  act("$n is surrounded by the {Rf{ri{Rr{re{Rs{x of {Rh{Ye{Rl{Yl{x! ",
      victim, NULL, NULL, TO_ROOM);
  send_to_char
    ("You become surrounded by the {Rf{ri{Rr{re{Rs{x of {Rh{Ye{Rl{Yl{x!\n\r",
     victim);

  send_to_char
    ("The {Rf{ri{Rr{re{Rs{x of {Rh{Ye{Rl{Yl{x scar and maim your body\n\r",
     ch);
  ch->hit -= level * 4;

  return;
}

MAGIC(spell_concentration)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int bonus = 800 + level + dice(10, 25);
  AFFECT_DATA af;

  if (IS_SHIELDED(victim, SHD_CONCENTRATION))
  {
    send_to_char("You are already concentrating as hard as you can.\n\r", ch);
    return;
  }

  if ((ch->move - (level * 5)) < 1)
  {
    send_to_char("You are too fatigued for that.", ch);
    return;
  }

  af.where = TO_SHIELDS;
  af.type = sn;
  af.level = level;
  af.duration = level;
  af.location = APPLY_NONE;
  af.modifier = 0;
  af.bitvector = SHD_CONCENTRATION;
  affect_to_char(victim, &af);

  victim->mana = UMIN(victim->mana + bonus, victim->max_hit);
  update_pos(victim);
  send_to_char("You feel the power of the magi flow within you.\n\r", victim);
  if (ch != victim)
    send_to_char("Ok.\n\r", ch);
  send_to_char("You are fatigued from concentrating so hard.\n\r", ch);
  ch->move -= level * 5;
  return;
}

MAGIC(spell_holy_mace)
{
  OBJ_DATA *mace;
  AFFECT_DATA *pAf;

  /* Make the mace, and have it decay after time */
  mace = create_object(get_obj_index(OBJ_VNUM_MACE), 0);
  mace->timer = level / 3;

  /* Add the hitroll based on level */
  pAf = new_affect();
  pAf->location = APPLY_HITROLL;
  pAf->modifier = (ch->level / 3);
  pAf->where = TO_OBJECT;
  pAf->type = -1;
  pAf->duration = -1;
  pAf->bitvector = 0;
  pAf->level = mace->level;
  pAf->next = mace->affected;
  mace->affected = pAf;
  mace->level = level;

  /* Now for the level based damage roll */
  pAf = new_affect();
  pAf->location = APPLY_DAMROLL;
  pAf->modifier = (ch->level / 3.5);
  pAf->where = TO_OBJECT;
  pAf->type = -1;
  pAf->duration = -1;
  pAf->bitvector = 0;
  pAf->level = mace->level;
  pAf->next = mace->affected;
  mace->affected = pAf;
  mace->level = level;

  /* Now for the Hitpoints of the weapon */
  pAf = new_affect();
  pAf->location = APPLY_HIT;
  pAf->modifier = ch->level + 100;
  pAf->where = TO_OBJECT;
  pAf->type = -1;
  pAf->duration = -1;
  pAf->bitvector = 0;
  pAf->level = mace->level;
  pAf->next = mace->affected;
  mace->affected = pAf;
  mace->level = level;

  /* Now for the weapons mana bonus */
  pAf = new_affect();
  pAf->location = APPLY_MANA;
  pAf->modifier = ch->level * 1.5;
  pAf->where = TO_OBJECT;
  pAf->type = -1;
  pAf->duration = -1;
  pAf->bitvector = 0;
  pAf->level = mace->level;
  pAf->next = mace->affected;
  mace->affected = pAf;
  mace->level = level;

  /* Now add the damage dice of the weapon */
  mace->value[1] = ch->level / 12;
  mace->value[2] = ch->level / 10;
  obj_to_char(mace, ch);

  /* Now drain the caster of Hp and mana */

  send_to_char("You attempt to form a holy mace.\n\r", ch);
  send_to_char("You feel weak, perhaps you should rest.\n\r", ch);
  act("$n constructs a divine weapon.", ch, mace, NULL, TO_ROOM);
  ch->hit *= .75;
  ch->move *= .50;
  return;
}

MAGIC(spell_summon_familiar)
{
  char buf[MAX_STRING_LENGTH];
  CHAR_DATA *gch;
  CHAR_DATA *familiar;
  AFFECT_DATA af;
  int i;

  if (is_affected(ch, sn))
  {
    send_to_char
      ("You lack the power to summon another familiar right now.\n\r", ch);
    return;
  }

  send_to_char("You attempt to summon a familiar.\n\r", ch);
  act("$n attempts to summon a demon.", ch, NULL, NULL, TO_ROOM);

  for (gch = char_list; gch != NULL; gch = gch->next)
  {
    if (IS_NPC(gch) && IS_AFFECTED(gch, AFF_CHARM) &&
        gch->master == ch && gch->pIndexData->vnum == MOB_VNUM_FAMILIAR)
    {
      send_to_char("Two familiars are more than you can control!\n\r", ch);
      return;
    }
  }

  familiar = create_mobile(get_mob_index(MOB_VNUM_FAMILIAR));

  for (i = 0; i < MAX_STATS; i++)
  {
    familiar->perm_stat[i] = ch->perm_stat[i];
  }
  /*
     familiar->max_hit    = IS_NPC(ch)? URANGE(ch->max_hit,1 * ch->max_hit,30000)
     : URANGE(ch->pcdata->perm_hit,ch->hit,30000);
     familiar->hit        = familiar->max_hit;
   */
  familiar->max_hit = IS_NPC(ch) ? ch->max_hit : ch->pcdata->perm_hit / 2;
  familiar->hit = familiar->max_hit;
  familiar->max_mana = IS_NPC(ch) ? ch->max_mana : ch->pcdata->perm_mana / 2;
  familiar->mana = familiar->max_mana;
  familiar->max_move = IS_NPC(ch) ? ch->max_move : ch->pcdata->perm_move;
  familiar->move = familiar->move;
  familiar->level = ch->level;

  for (i = 0; i < 3; i++)

    familiar->armor[i] = interpolate(familiar->level, 100, -100);
  familiar->armor[3] = interpolate(familiar->level, 100, 0);
  familiar->gold = 0;
  familiar->timer = 0;
  familiar->damage[DICE_NUMBER] = number_range(level / 15, level / 10);
  familiar->damage[DICE_TYPE] = number_range(level / 3, level / 2);
  familiar->damage[DICE_BONUS] = number_range(level / 8, level / 6);

  sprintf(buf, "%s's familiar", ch->name);
  free_string(familiar->short_descr);
  familiar->short_descr = str_dup(buf);

  sprintf(buf, "%s's familiar is here.\n\r", ch->name);
  free_string(familiar->long_descr);
  familiar->long_descr = str_dup(buf);

  char_to_room(familiar, ch->in_room);
  send_to_char("A familiar arrives through a {Dd{bark{x portal!\n\r", ch);
  act("A familiar arrives through a {Dd{bark{x portal!", ch, NULL, NULL,
      TO_ROOM);

  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = 24;
  af.bitvector = 0;
  af.modifier = 0;
  af.location = APPLY_NONE;
  affect_to_char(ch, &af);

  send_to_char("You feel weak, perhaps you should rest.\n\r", ch);
  ch->hit *= .75;
  ch->move *= .50;
  SET_BIT(familiar->affected_by, AFF_CHARM);
  familiar->master = familiar->leader = ch;
  return;
}

MAGIC(spell_vampiric_laceration)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  AFFECT_DATA af;

  if (is_affected(victim, sn))
  {
    if (victim == ch)
      send_to_char
        ("You are allready at one with your vampiric side.\n\r", ch);
    else
      act("$N is allready at one with thier vampiric side.", ch, NULL,
          victim, TO_CHAR);
    return;

  }
  af.where = TO_AFFECTS;
  af.type = sn;
  af.level = level;
  af.duration = 7;
  af.bitvector = AFF_WEAKEN;

  af.location = APPLY_DAMROLL;
  af.modifier = level / 10;
  affect_to_char(victim, &af);

  af.location = APPLY_HITROLL;
  af.modifier = level / 10;
  affect_to_char(victim, &af);

  af.location = APPLY_SAVES;
  af.modifier = 0 - level / 7;
  affect_to_char(victim, &af);

  af.location = APPLY_AC;
  af.modifier = 0 - level / 2;
  affect_to_char(victim, &af);

  send_to_char("The very essense of evil flows though you!\n\r", victim);
  if (ch != victim)
    act("$N", ch, NULL, victim, TO_CHAR);
  return;
}

MAGIC(spell_celestial_fury)
{
  CHAR_DATA *victim = (CHAR_DATA *) vo;
  int dam;

  act("You call upon the fury of the heavens!", ch, NULL, victim, TO_CHAR);
  act("$n calls upon the fury of the heavens!", ch, NULL, NULL, TO_ROOM);

  if ((ch->fighting == NULL) && (!IS_NPC(ch)) && (!IS_NPC(victim)))
  {
    ch->attacker = true;
    victim->attacker = false;
  }

  dam = dice(level * 2, 40);
  if (saves_spell(level, victim, DAM_HOLY))
    dam /= 1.5;

  damage_old(ch, victim, dam + dice(3, 10), sn, DAM_HOLY, true);
  damage_old(ch, victim, dam + dice(2, 15), sn, DAM_LIGHTNING, true);
  return;
}
