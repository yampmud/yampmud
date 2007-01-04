
/****************************************************************************
*  Automated Quest code written by Vassago of MOONGATE, moongate.ams.com    *
*  4000. Copyright (c) 1996 Ryan Addams, All Rights Reserved. Use of this   *
*  code is allowed provided you add a credit line to the effect of:         *
*  "Quest Code (c) 1996 Ryan Addams" to your logon screen with the rest     *
*  of the standard diku/rom credits. If you use this or a modified version  *
*  of this code, let me know via email: moongate@moongate.ams.com. Further  *
*  updates will be posted to the rom mailing list. If you'd like to get     *
*  the latest version of quest.c, please send a request to the above add-   *
*  ress. Quest Code v2.03. Please do not remove this notice from this file. *
****************************************************************************/

/********************************************************************************
 * Updated Quest Code copyright 1999-2001                                       *
 * Markanth : markanth@hotpop.com or rjenn78@home.com                           *
 * Devil's Lament : spaceservices.net port 3778                                 *
 * Web Page : http://spaceservices.net/~markanth/                               *
 *                                                                              *
 * Quest objects are now updated by owner's level, code is more dynamic.        *
 *                                                                              *
 * All I ask in return is that you give me credit on your mud somewhere         *
 * or email me if you use it.                                                   *
 ********************************************************************************/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "merc.h"
#include "recycle.h"
#include "magic.h"
#include "tables.h"
#include "lookup.h"
#include "olc.h"

struct quest_type
{
  char *name;
  char *who_name;
  long vnum;
  int cost;
};

/* Quest Prizes */
#define QUEST_BLADE       501   /* Blade */
#define QUEST_ARMOR       502   /* Armor */
#define QUEST_LEGGING     505   /* Leggings */
#define QUEST_GAUNTLET    503   /* Gauntlets */
#define QUEST_BOOT        512   /* Boots */
#define QUEST_HELM        514   /* Helm */
#define QUEST_RING        506   /* Ring */
#define QUEST_ORB         516   /* Orb */
#define QUEST_NECKLACE    517   /* Necklace */
#define QUEST_MASK        508   /* Mask */
#define QUEST_BELT        511   /* Belt */
#define QUEST_CLOAK       507   /* Cloak */
#define QUEST_BRACER      509   /* Bracelet */
#define QUEST_EARRING     518   /* Earring */
#define QUEST_ANKLET      513   /* Anklet */
#define QUEST_SHIELD      510   /* Shield */
#define QUEST_AURA        515   /* Aura */
#define QUEST_TATTOO      504   /* Tattoo */
#define QUEST_SLEEVE      519   /* Sleeves */
#define QUEST_FLAME       520   /* Flame */
#define QUEST_CREST       521   /* Crest */
#define QUEST_STAFF       522   /* Staff */
#define QUEST_BACKPACK    523   /* Backpack */
/*#define QUEST_STRINGTICK  548   Restring Ticket */
/*#define QUEST_WHOTICK     549   Whostring Ticket */
/*#define QUEST_TRIVIA      OBJ_VNUM_TRIVIA_PILL */

/*
 * Quest Reward Table.
 */
const struct quest_type quest_table[] = {
  {"blade", "{BB{blad{Be{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_BLADE,
   20000},
  {"protection", "{RP{rrotectio{Rn {xof the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_ARMOR, 15000},
  {"gauntlets", "{GG{gauntlet{Gs{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys",
   QUEST_GAUNTLET, 9000},
  {"tattoo", "{RT{ratto{Ro{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_TATTOO, 5000},
  {"leggings", "{WL{Degging{Ws{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_LEGGING, 5000},
  {"boots", "{DB{moot{Ds {xof the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_BOOT,
   5000},
  {"mask", "{CM{cas{Ck{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_MASK,
   5000},
  {"helm", "{YH{yel{Ym{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_HELM,
   5000},
  {"belt", "{BB{gel{Bt{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_BELT,
   5000},
  {"orb", "{CO{Br{Cb{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_ORB,
   5000},
  {"earring", "{ME{Darrin{Mg{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_EARRING, 5000},
  {"shield", "{DS{Ghiel{Dd{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_SHIELD, 5000},
  {"ring", "{MR{min{Mg{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_RING,
   5000},
  {"cloak", "{DC{gloa{Dk {xof the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_CLOAK,
   5000},
  {"neckguard", "{WN{Deckguar{Wd {xof the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_NECKLACE, 5000},
  {"anklet", "{gA{cnkle{gt{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_ANKLET, 5000},
  {"bracer", "{BB{Drace{Br{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_BRACER, 5000},
  {"aura", "{CA{Wu{Cr{Wa{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_AURA,
   5000},
  {"sleeves", "{RS{cl{Re{ce{Rv{ce{Rs{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_SLEEVE, 5000},
  {"flame", "{RF{Yl{Ra{Ym{Re{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_FLAME, 5000},
  {"crest", "{DC{Cres{Dt{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x", QUEST_CREST,
   5000},
  {"backpack", "{GB{yackpac{Gk{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_BACKPACK,
   5000},
  {"staff", "{yS{Dt{ya{Df{yf{x of the {YA{yn{Yc{yi{Ye{yn{Yt{ys{x",
   QUEST_STAFF,
   4000},
  /*    {"wticket", "{wA {WW{ghostrin{Wg{G Ticket (Quest Buy WTicket){x",
     QUEST_WHOTICK, 3000},
     {"sticket", "{wA {WR{gestrin{Wg {GTicket (Quest Buy STicket){x",
     QUEST_STRINGTICK, 2000}, */
  {NULL, 0, 0}
};

/* Quests to find objects */
#define QUEST_OBJQUEST1 3315
#define QUEST_OBJQUEST2 3316
#define QUEST_OBJQUEST3 3317
#define QUEST_OBJQUEST4 3318

/*
 * CHANCE function. I use this everywhere in my code, very handy :> 
 */
bool chance(int num)
{
  if (number_range(1, 100) <= num)
    return true;
  else
    return false;
}

/* is object in quest table? */
int is_qobj(OBJ_DATA * obj)
{
  int i;

  if (!obj || !obj->pIndexData)
    return -1;

  for (i = 0; quest_table[i].name != NULL; i++)
  {
    if (obj->pIndexData->vnum == quest_table[i].vnum)
      return i;
  }
  return -1;
}

/* get the cost of an object in questpoints */
int qobj_cost(OBJ_DATA * obj)
{
  int i;

  if (!obj || !obj->pIndexData)
    return 0;

  for (i = 0; quest_table[i].name != NULL; i++)
  {
    if (obj->pIndexData->vnum == quest_table[i].vnum)
      return quest_table[i].cost;
  }
  return 0;
}

/*
 * * Add or enhance an obj affect. 
 */
void affect_join_obj(OBJ_DATA * obj, AFFECT_DATA * paf)
{
  AFFECT_DATA *paf_old;
  bool found;

  found = false;
  for (paf_old = obj->affected; paf_old != NULL; paf_old = paf_old->next)
  {
    if (paf_old->location == paf->location)
    {
      paf_old->level = paf->level;
      paf_old->modifier = paf->modifier;
      found = true;
    }
  }
  if (!found)
    affect_to_obj(obj, paf);
  return;
}

/* Nice little addapply function */
void add_apply(OBJ_DATA * obj, int loc, int mod, int where, int type, int dur,
               long bit, int level)
{
  AFFECT_DATA pAf;

  if (obj == NULL)
    return;

  pAf.location = loc;
  pAf.modifier = mod;
  pAf.where = where;
  pAf.type = type;
  pAf.duration = dur;
  pAf.bitvector = bit;
  pAf.level = level;
  affect_join_obj(obj, &pAf);

  return;
}

/* Update a questobject */
void update_questobjs(CHAR_DATA * ch, OBJ_DATA * obj)
{
  int bonus, pbonus, cost;

  if (obj == NULL || obj->pIndexData == NULL)
  {
    bug("update_questobjs: NULL obj", 0);
    return;
  }
  if (ch == NULL)
  {
    bug("update_questobjs: NULL ch", 0);
    return;
  }

  if (!IS_OBJ_STAT(obj, ITEM_QUEST) && is_qobj(obj) == -1)
    return;

  bonus = UMAX(5, ch->level / 2);
  pbonus = UMAX(5, ch->level / 2);
  cost = qobj_cost(obj);

  if (obj->level != ch->level)
    obj->level = ch->level;
  if (obj->condition != -1)
    obj->condition = -1;
  if (obj->cost != cost)
    obj->cost = cost;
  if (!CAN_WEAR(obj, ITEM_NO_SAC))
    SET_BIT(obj->wear_flags, ITEM_NO_SAC);
  if (!IS_OBJ_STAT(obj, ITEM_BURN_PROOF))
    SET_BIT(obj->extra_flags, ITEM_BURN_PROOF);

  if (obj->pIndexData->vnum == QUEST_ARMOR)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level * 7), TO_OBJECT, 0,
              -1, 0, ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level * 7), TO_OBJECT, 0,
              -1, 0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level * 7), TO_OBJECT, 0,
              -1, 0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_SHIELD)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_TATTOO)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_MASK)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_EARRING)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_AURA)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_BRACER)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_NECKLACE)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_ORB)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_ANKLET)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_BELT)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_CLOAK)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_RING)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_HELM)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_GAUNTLET)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_BOOT)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->pIndexData->vnum == QUEST_LEGGING)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }

  else if (obj->pIndexData->vnum == QUEST_SLEEVE)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }

  else if (obj->pIndexData->vnum == QUEST_FLAME)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level / 2);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level / 2);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level * 2);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level * 2);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level * 2);
  }

  else if (obj->pIndexData->vnum == QUEST_CREST)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level / 2);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level / 2);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level * 2);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level * 2);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level * 2);
  }
  else if (obj->pIndexData->vnum == QUEST_STAFF)
  {
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level / 2);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level / 2);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level * 2);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level * 2);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level * 2);
  }

  else if (obj->item_type == ITEM_CONTAINER)
  {
    obj->weight = -1 * (50 + (ch->level * 1.5));
    obj->value[0] = 1000 + (20 * ch->level);
    obj->value[3] = 1000 + (20 * ch->level);
  }

  if (obj->item_type == ITEM_WEAPON)
  {
    obj->value[1] = UMAX(15, ch->level / 8);
    obj->value[2] = ch->level < 80 ? 6 : 40;
    add_apply(obj, APPLY_DAMROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HITROLL, pbonus, TO_OBJECT, 0, -1, 0, ch->level);
    add_apply(obj, APPLY_HIT, UMAX(50, ch->level), TO_OBJECT, 0, -1, 0,
              ch->level);
    add_apply(obj, APPLY_MANA, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
    add_apply(obj, APPLY_MOVE, UMAX(50, ch->level), TO_OBJECT, 0, -1,
              0, ch->level);
  }
  else if (obj->item_type == ITEM_ARMOR)
  {
    obj->value[0] = UMAX(20, ch->level * 4 / 3);
    obj->value[1] = UMAX(20, ch->level * 4 / 3);
    obj->value[2] = UMAX(20, ch->level * 4 / 3);
    obj->value[3] = (5 * UMAX(20, ch->level * 4 / 3)) / 6;
  }
  /*    else if ( obj->item_type == ITEM_STAFF )
     obj->value[0] = UMAX ( 50, ch->level * 3 ); */
  return;
}

/* Usage info on the QUEST commands */
/* Keep this in line with the do_quest function's keywords */
void quest_usage(CHAR_DATA * ch)
{
  send_to_char
    ("{YQUEST commands: POINTS INFO TIME REQUEST COMPLETE LIST BUY SPELL QUIT SELL REPLACE.{x\n\r",
     ch);
  send_to_char("{YFor more information, type 'HELP QUEST'.{x\n\r", ch);
  return;
}

/* Obtain additional location information about sought item/mob */
void quest_where(CHAR_DATA * ch, char *what)
{
  char buf[MAX_INPUT_LENGTH];
  ROOM_INDEX_DATA *room;

  if (!ch || IS_NPC(ch))
    return;

  if (ch->pcdata->questloc <= 0)
  {
    bug("QUEST INFO: ch->questloc = %ld", ch->pcdata->questloc);
    return;
  }
  if (ch->in_room == NULL)
    return;

  room = get_room_index(ch->pcdata->questloc);
  if (room->area == NULL)
  {
    bug("QUEST INFO: room(%ld)->area == NULL", ch->pcdata->questloc);
    return;
  }
  if (room->area->name == NULL)
  {
    bug("QUEST INFO: area->name == NULL", 0);
    return;
  }
  sprintf(buf,
          "{RRumor has it this %s was last seen in the area known as %s,{x\n\r",
          what, room->area->name);
  send_to_char(buf, ch);
  if (room->name == NULL)
  {
    bug("QUEST INFO: room(%ld)->name == NULL", ch->pcdata->questloc);
    return;
  }
  sprintf(buf, "{Rnear %s.{x\n\r", room->name);
  send_to_char(buf, ch);

}                               /* end quest_where() */

/* Does player have thier target object? */
OBJ_DATA *has_questobj(CHAR_DATA * ch)
{
  OBJ_DATA *obj;
  OBJ_INDEX_DATA *pObj;
  OBJ_DATA *obj2;
  if (!ch || IS_NPC(ch) || ch->pcdata->questobj <= 0)
    return NULL;

  if ((pObj = get_obj_index(ch->pcdata->questobj)) == NULL)
    return NULL;

  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    if (obj != NULL && obj->pIndexData == pObj)
      return obj;
  /* its not in their regular inventory, check their bags */
  for (obj = ch->carrying; obj != NULL; obj = obj->next_content)
    if (obj != NULL && (obj->contains != NULL))
      for (obj2 = obj->contains; obj2 != NULL; obj2 = obj2->next)
        if (obj2 != NULL && obj2->pIndexData == pObj)
          return obj2;

  return NULL;
}

/*
 * The main quest function 
 */
CH_CMD(do_quest)
{
  CHAR_DATA *questman;
  OBJ_DATA *obj = NULL;

  OBJ_INDEX_DATA *questinfoobj;
  MOB_INDEX_DATA *questinfo;
  char buf[MSL];
  char arg1[MAX_STRING_LENGTH];
  char arg2[MAX_STRING_LENGTH];
  char arg3[MAX_STRING_LENGTH];
  int i = 0;
  int people_online;

  people_online = 0;

  if (!ch)
    return;
  if (IS_NPC(ch))
  {
    send_to_char("I'm sorry, you can't quest.\n\r", ch);
    return;
  }

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);

  if (arg1[0] == '\0')
  {
    quest_usage(ch);
    return;
  }

  if (!str_cmp(arg1, "info"))
  {
    MOB_INDEX_DATA *qm_mobindex;

    if (!IS_QUESTOR(ch))
    {
      send_to_char("You aren't currently on a quest.\n\r", ch);
      return;
    }
    send_to_char("\n\r", ch);
    if (ch->pcdata->questgiver <= 0)
    {
      bug("QUEST INFO: quest giver = %d", ch->pcdata->questgiver);
      send_to_char
        ("{RIt seems your quest master died of old age waiting for you.{x\n\r",
         ch);
      end_quest(ch, 5);

      return;
    }
    qm_mobindex = get_mob_index(ch->pcdata->questgiver);
    if (qm_mobindex == NULL)
    {
      bug("QUEST INFO: quest giver %d has no MOB_INDEX_DATA!",
          ch->pcdata->questgiver);
      send_to_char
        ("{RYour quest master has fallen very ill. Please contact an imm!{x\n\r",
         ch);
      end_quest(ch, 5);

      return;
    }
    if (ch->pcdata->questmob == -1 || has_questobj(ch)) /* killed
                                                           target
                                                           mob */
    {
      send_to_char("{RYour quest is {fALMOST{x{R complete!{x\n\r", ch);
      sprintf(buf,
              "{RGet back to %s{R before your time runs out!{x\n\r",
              (qm_mobindex->short_descr ==
               NULL ? "your quest master" : qm_mobindex->short_descr));
      send_to_char(buf, ch);
      return;
    }
    else if (ch->pcdata->questobj > 0)  /* questing for an object */
    {
      questinfoobj = get_obj_index(ch->pcdata->questobj);
      if (questinfoobj != NULL)
      {
        printf_to_char(ch,
                       "{RYou recall the quest which the %s{R gave you.{x\n\r",
                       (qm_mobindex->short_descr ==
                        NULL ? "your quest master" : qm_mobindex->
                        short_descr));
        sprintf(buf,
                "{RYou are on a quest to recover the fabled %s{R!{x\n\r",
                questinfoobj->name);
        send_to_char(buf, ch);
        quest_where(ch, "treasure");
        return;
      }
      /* quest object not found! */
      bug("No info for quest object %d", ch->pcdata->questobj);
      ch->pcdata->questobj = 0;
      REMOVE_BIT(ch->act, PLR_QUESTOR);
      /* no RETURN -- fall thru to 'no quest', below */
    }
    else if (ch->pcdata->questmob > 0)  /* questing for a mob */
    {
      questinfo = get_mob_index(ch->pcdata->questmob);
      if (questinfo != NULL)
      {
        sprintf(buf,
                "{RYou are on a quest to slay the dreaded %s{R!{x\n\r",
                questinfo->short_descr);
        send_to_char(buf, ch);
        quest_where(ch, "fiend");
        return;
      }
      /* quest mob not found! */
      bug("No info for quest mob %d", ch->pcdata->questmob);
      ch->pcdata->questmob = 0; /* Changed to mob instead of obj -
                                   Lotus */
      REMOVE_BIT(ch->act, PLR_QUESTOR);
      /* no RETURN -- fall thru to 'no quest', below */
    }
    /* we shouldn't be here */
    bug("QUEST INFO: Questor with no kill, mob or obj", 0);
    return;
  }
  if (!str_cmp(arg1, "points"))
  {
    sprintf(buf,
            "{WYou have {r%-8ld{W AQP{x\n\r{WYou have {r%-8d {WIQP{x\n\r",
            ch->pcdata->questpoints, ch->qps);
    send_to_char(buf, ch);
    return;
  }
  else if (!str_cmp(arg1, "time"))
  {
    if (!IS_SET(ch->act, PLR_QUESTOR))
    {
      send_to_char("You aren't currently on a quest.\n\r", ch);
      if (ch->pcdata->nextquest > 0)
      {
        sprintf(buf,
                "There are %d minutes remaining until you can go on another quest.\n\r",
                ch->pcdata->nextquest);
        send_to_char(buf, ch);
      }
      else if (ch->pcdata->nextquest == 1)
      {
        sprintf(buf,
                "There is less than a minute remaining until you can go on another quest.\n\r");
        send_to_char(buf, ch);
      }
    }
    else if (ch->pcdata->countdown > 0)
    {
      sprintf(buf, "Time left for current quest: %d\n\r",
              ch->pcdata->countdown);
      send_to_char(buf, ch);
    }
    return;
  }

  /* 
   * Checks for a character in the room with spec_questmaster set. This
   * special procedure must be defined in special.c. You could instead use
   * an ACT_QUESTMASTER flag instead of a special procedure. 
   */

  for (questman = ch->in_room->people; questman != NULL;
       questman = questman->next_in_room)
  {
    if (!IS_NPC(questman))
      continue;
    if (questman->spec_fun == spec_lookup("spec_questmaster"))
      break;
  }

  if (questman == NULL ||
      questman->spec_fun != spec_lookup("spec_questmaster"))
  {
    send_to_char("You can't do that here.\n\r", ch);
    return;
  }

  if (questman->fighting != NULL)
  {
    send_to_char("Wait until the fighting stops.\n\r", ch);
    return;
  }

  /* 
   * And, of course, you will need to change the following lines for YOUR
   * quest item information. Quest items on Moongate are unbalanced, very
   * very nice items, and no one has one yet, because it takes awhile to
   * build up quest points :> Make the item worth their while. 
   */

  if (!str_cmp(arg1, "list"))
  {
    act("$n asks $N for a list of quest items.", ch, NULL, questman, TO_ROOM);
    send_to_char("\t{YCurrent Quest Items available for Purchase:{x\n\r", ch);
    for (i = 0; quest_table[i].who_name != NULL; i++)
      printf_to_char(ch, "\t%-5dqp ........ %s{x\n\r",
                     quest_table[i].cost, quest_table[i].who_name);
    send_to_char("\tTo buy an item, type 'QUEST BUY <item>'.\n\r", ch);
    send_to_char("\tFor more info on quest items type 'help qitems'\n\r", ch);
    return;
  }

  else if (!str_cmp(arg1, "buy"))
  {

    if (arg2[0] == '\0')
    {
      send_to_char("To buy an item, type 'QUEST BUY <item>'.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, "iqp"))
    {
      if (ch->pcdata->questpoints >= 500)
      {
        ch->pcdata->questpoints -= 500;
        ch->qps += 25;
      }
      else
      {
        sprintf(buf,
                "Sorry, %s, but you don't have enough quest points for that.",
                ch->name);
        do_mob_tell(ch, questman, buf);
        return;
      }
    }

    for (i = 0; quest_table[i].name != NULL; i++)
    {
      if (is_name(arg2, quest_table[i].name))
      {
        if (ch->pcdata->questpoints >= quest_table[i].cost)
        {
          if (quest_table[i].vnum == 0)
          {
            ch->pcdata->questpoints -= quest_table[i].cost;
            ch->pcdata->condition[COND_FULL] = -51;
            ch->pcdata->condition[COND_THIRST] = -51;
            act
              ("$N calls upon the power of the gods to relieve your mortal burdens.",
               ch, NULL, questman, TO_CHAR);
            act
              ("$N calls upon the power of the gods to relieve $n' mortal burdens.",
               ch, NULL, questman, TO_ROOM);
            return;
          }
          else if ((obj = create_object(get_obj_index(quest_table[i].vnum)))
                   == NULL)
          {
            send_to_char
              ("That object could not be found, contact an immortal.\n\r",
               ch);
            return;
          }
          else
          {
            ch->pcdata->questpoints -= quest_table[i].cost;
            if (!IS_IMMORTAL(ch))
            {
              sprintf(buf, "Bought %s for %d questpoints.",
                      quest_table[i].name, quest_table[i].cost);
              append_file(ch, QUEST_FILE, buf);
            }
          }

          if (!IS_SET(obj->pIndexData->extra_flags, ITEM_QUEST))
          {
            SET_BIT(obj->pIndexData->extra_flags, ITEM_QUEST);
            SET_BIT(obj->extra_flags, ITEM_QUEST);
            SET_BIT(obj->pIndexData->area->area_flags, AREA_CHANGED);
          }
          act("$N gives $p to $n.", ch, obj, questman, TO_ROOM);
          act("$N gives you $p.", ch, obj, questman, TO_CHAR);
          obj_to_char(obj, ch);
          save_char_obj(ch);
          return;
        }
        else
        {
          sprintf(buf,
                  "Sorry, %s, but you need %d quest points for that.",
                  ch->name, quest_table[i].cost);
          do_mob_tell(ch, questman, buf);
          return;
        }
      }
    }
    sprintf(buf, "I don't have that item, %s.", ch->name);
    do_mob_tell(ch, questman, buf);
    return;
  }

  else if (!str_cmp(arg1, "sell"))
  {
    if (arg2[0] == '\0')
    {
      send_to_char("To sell an item, type 'QUEST SELL <item>'.\n\r", ch);
      return;
    }
    if ((obj = get_obj_carry(ch, arg2)) == NULL)
    {
      send_to_char("Which item is that?\n\r", ch);
      return;
    }

    if (!IS_OBJ_STAT(obj, ITEM_QUEST))
    {
      sprintf(buf, "That is not a quest item, %s.", ch->name);
      do_mob_tell(ch, questman, buf);
      return;
    }

    for (i = 0; quest_table[i].name != NULL; i++)
    {
      if (quest_table[i].vnum <= 0)
        continue;
      if (quest_table[i].vnum == obj->pIndexData->vnum)
      {
        ch->pcdata->questpoints += (quest_table[i].cost / 1.5);
        act("$N takes $p from $n.", ch, obj, questman, TO_ROOM);
        sprintf(buf, "$N takes $p from you for %d quest points.",
                (int) (quest_table[i].cost / 1.5));
        act(buf, ch, obj, questman, TO_CHAR);
        extract_obj(obj);
        obj = NULL;
        save_char_obj(ch);
        return;
      }
    }
    sprintf(buf, "I only take items I sell, %s.", ch->name);
    do_mob_tell(ch, questman, buf);
    return;
  }
  else if (!str_cmp(arg1, "replace"))
  {
    if (arg2[0] == '\0')
    {
      send_to_char
        ("To replace an item, type 'QUEST REPLACE <item>'.\n\r", ch);
      return;
    }
    if ((obj = get_obj_carry(ch, arg2)) == NULL)
    {
      send_to_char("Which item is that?\n\r", ch);
      return;
    }

    if (!IS_OBJ_STAT(obj, ITEM_QUEST))
    {
      sprintf(buf, "That is not a quest item, %s.", ch->name);
      do_mob_tell(ch, questman, buf);
      return;
    }
    if (ch->pcdata->questpoints < 75)
    {
      do_mob_tell(ch, questman,
                  "It costs 75 questpoints to replace an item.");
      return;
    }
    for (i = 0; quest_table[i].name != NULL; i++)
    {
      if (quest_table[i].vnum <= 0)
        continue;
      if (quest_table[i].vnum == obj->pIndexData->vnum)
      {
        OBJ_DATA *newobj;

        if ((newobj =
             create_object(get_obj_index(quest_table[i].vnum))) == NULL)
        {
          sprintf(buf,
                  "I could not find a new quest item for you %s.", ch->name);
          do_mob_tell(ch, questman, buf);
        }
        else
        {
          obj_to_char(newobj, ch);
          ch->pcdata->questpoints -= 75;
          act("$N takes $p from $n and gives $m a new one.", ch,
              obj, questman, TO_ROOM);
          act("$N replaces $p with a new one for 75 quest points.",
              ch, obj, questman, TO_CHAR);
          extract_obj(obj);
          obj = NULL;
          save_char_obj(ch);
        }
        return;
      }
    }
    sprintf(buf, "I only replace items I sell, %s.", ch->name);
    do_mob_tell(ch, questman, buf);
    return;
  }

  else if (!str_cmp(arg1, "request"))
  {
    if (copyover_countdown > 0 && copyover_countdown < 3)
    {
      send_to_char
        ("Sorry, the mud is getting ready for a copyover.\n\rPlease try again later.\n\r",
         ch);
      return;
    }
    act("$n asks $N for a quest.", ch, NULL, questman, TO_ROOM);
    act("You ask $N for a quest.", ch, NULL, questman, TO_CHAR);
    if (IS_SET(ch->act, PLR_QUESTOR))
    {
      do_mob_tell(ch, questman, "But you're already on a quest!");
      return;
    }
    if (ch->pcdata->nextquest > 0)
    {
      sprintf(buf,
              "You're very brave, %s, but let someone else have a chance.",
              ch->name);
      do_mob_tell(ch, questman, buf);
      do_mob_tell(ch, questman, "Come back later.");
      return;
    }

    sprintf(buf, "Thank you, brave %s!", ch->name);
    do_mob_tell(ch, questman, buf);
    ch->pcdata->questmob = 0;
    ch->pcdata->questobj = 0;

    generate_quest(ch, questman);
    return;
  }
  else if (!str_cmp(arg1, "complete"))
  {
    if (ch->pcdata->questgiver != questman->pIndexData->vnum)
    {
      do_mob_tell(ch, questman,
                  "I never sent you on a quest! Perhaps you're thinking of someone else.");
      return;
    }

    if (IS_SET(ch->act, PLR_QUESTOR))
    {
      int reward, pracreward, points;

      if (ch->pcdata->questmob == -1 && ch->pcdata->countdown > 0)
      {
        reward = number_range(10000, 15000);
        points = number_range(25, 50);

        act("$n informs $N $e has completed $s quest.", ch, NULL,
            questman, TO_ROOM);
        act("You inform $N you have completed $s quest.", ch, NULL,
            questman, TO_CHAR);

        do_mob_tell(ch, questman,
                    "Congratulations on completing your quest!");
        if (double_qp || happy_hour)
        {
          points *= 2;
          sprintf(buf,
                  "As a reward, I am giving you %d quest points, and %d gold. ( Double QP )",
                  points, reward);
        }
        else
        {
          sprintf(buf,
                  "As a reward, I am giving you %d quest points, and %d gold.",
                  points, reward);
        }
        do_mob_tell(ch, questman, buf);

        if (chance(50))
        {
          pracreward = number_range(3, 12);
          sprintf(buf, "You gain %d practices!\n\r", pracreward);
          send_to_char(buf, ch);
          ch->practice += pracreward;
        }
        end_quest(ch, 5);
        add_cost(ch, reward, VALUE_GOLD);
        ch->pcdata->questpoints += points;
        return;
      }
      else if (ch->pcdata->questobj > 0 && ch->pcdata->countdown > 0)
      {
        if ((obj = has_questobj(ch)) != NULL)
        {
          reward = number_range(10000, 15000);
          points = number_range(25, 50);

          act("$n informs $N $e has completed $s quest.", ch, NULL,
              questman, TO_ROOM);
          act("You inform $N you have completed $s quest.", ch,
              NULL, questman, TO_CHAR);

          act("You hand $p to $N.", ch, obj, questman, TO_CHAR);
          act("$n hands $p to $N.", ch, obj, questman, TO_ROOM);

          do_mob_tell(ch, questman,
                      "Congratulations on completing your quest!");
          if (double_qp || happy_hour)
          {
            points *= 2;
            sprintf(buf,
                    "As a reward, I am giving you %d quest points, and %d gold. ( Double QP )",
                    points, reward);
          }
          else
          {
            sprintf(buf,
                    "As a reward, I am giving you %d quest points, and %d gold.",
                    points, reward);
          }
          do_mob_tell(ch, questman, buf);

          if (chance(50))
          {
            pracreward = number_range(3, 12);
            sprintf(buf, "You gain %d practices!\n\r", pracreward);
            send_to_char(buf, ch);
            ch->practice += pracreward;
          }
          end_quest(ch, 5);
          add_cost(ch, reward, VALUE_GOLD);
          ch->pcdata->questpoints += points;
          extract_obj(obj);
          obj = NULL;
          return;
        }
        else
        {
          do_mob_tell(ch, questman,
                      "You haven't completed the quest yet, but there is still time!");
          return;
        }
        return;
      }
      else if ((ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0)
               && ch->pcdata->countdown > 0)
      {
        do_mob_tell(ch, questman,
                    "You haven't completed the quest yet, but there is still  time!");
        return;
      }
    }
    if (ch->pcdata->nextquest > 0)
      sprintf(buf, "But you didn't complete your quest in time!");
    else
      sprintf(buf, "You have to REQUEST a quest first, %s.", ch->name);
    do_mob_tell(ch, questman, buf);
    return;
  }

  else if (!str_cmp(arg1, "quit") || !str_cmp(arg1, "fail"))
  {
    act("$n informs $N $e wishes to quit $s quest.", ch, NULL, questman,
        TO_ROOM);
    act("You inform $N you wish to quit $s quest.", ch, NULL, questman,
        TO_CHAR);
    if (ch->pcdata->questgiver != questman->pIndexData->vnum)
    {
      do_mob_tell(ch, questman,
                  "I never sent you on a quest! Perhaps you're thinking of someone else.");
      return;
    }

    if (IS_SET(ch->act, PLR_QUESTOR))
    {
      end_quest(ch, 5);
      do_mob_tell(ch, questman,
                  "Your quest is over, but for your cowardly behavior, you may not quest again for 5 minutes.");

      return;
    }
    else
    {
      send_to_char("You aren't on a quest!", ch);
      return;
    }
  }

  send_to_char
    ("Quest commands: Info, Time, Request, Complete, Quit, List, Sell and Buy.\n\r",
     ch);
  send_to_char("For more information, type 'Help Quest'.\n\r", ch);
  return;
}

#define MAX_QMOB_COUNT mobile_count /* do all mobs what the heck */

void generate_quest(CHAR_DATA * ch, CHAR_DATA * questman)
{
  CHAR_DATA *victim = NULL;
  ROOM_INDEX_DATA *room = NULL;
  CHAR_DATA *mobs[MAX_QMOB_COUNT];
  size_t mob_count;
  OBJ_DATA *questitem = NULL;
  char buf[MSL];
  int mrange;

  /* 
   * * find MAX_QMOB_COUNT quest mobs and store their vnums in mob_buf 
   */

  mob_count = 0;
  for (victim = char_list; victim; victim = victim->next)
  {

    if (!IS_NPC(victim) || !quest_level_diff(ch, victim) ||
        victim->pIndexData == NULL || victim->in_room == NULL ||
        victim->pIndexData->pShop != NULL || victim->pIndexData->vnum < 100
        || IS_SET(victim->imm_flags, IMM_WEAPON | IMM_MAGIC | IMM_NATURE) ||
        IS_SET(victim->act,
               ACT_TRAIN | ACT_PRACTICE | ACT_IS_HEALER | ACT_PET |
               ACT_IS_PRIEST | ACT_GAIN) ||
        IS_SET(victim->affected_by, AFF_CHARM) ||
        questman->pIndexData == victim->pIndexData ||
        IS_SET(victim->in_room->room_flags,
               ROOM_PRIVATE | ROOM_SOLITARY | ROOM_SAFE) ||
        strstr(victim->in_room->area->builders, "Unlinked"))

      continue;
    mobs[mob_count++] = victim;
    if (mob_count >= MAX_QMOB_COUNT)
      break;
  }

  if (mob_count == 0)
  {
    do_mob_tell(ch, questman,
                "I'm sorry, but I don't have any quests for you at this time.");
    do_mob_tell(ch, questman, "Try again later.");
    end_quest(ch, 5);
    return;
  }

  /* 
   * at this point the player is sure to get a quest 
   */
  ch->pcdata->questgiver = questman->pIndexData->vnum;
  mrange = number_range(0, mob_count - 1);
  while ((victim = mobs[mrange]) == NULL)
    mrange = number_range(0, mob_count - 1);
  room = victim->in_room;
  ch->pcdata->questloc = room->vnum;

  ch->pcdata->countdown = number_range(12, 30);

  /* 
   * 20% chance it will send the player on a 'recover item' quest. Changed
   * to 15 by Markanth 
   */

  if (chance(20))
  {
    long objvnum = 0;

    switch (number_range(0, 3))
    {
      case 0:
        objvnum = QUEST_OBJQUEST1;
        break;

      case 1:
        objvnum = QUEST_OBJQUEST2;
        break;

      case 2:
        objvnum = QUEST_OBJQUEST3;
        break;

      case 3:
        objvnum = QUEST_OBJQUEST4;
        break;
    }

    questitem = create_object(get_obj_index(objvnum));
    obj_to_room(questitem, room);
    REMOVE_BIT(ch->act, PLR_CANLOOT);
    free_string(questitem->owner);
    questitem->owner = str_dup(ch->name);
    questitem->cost = 0;
    questitem->timer = (4 * ch->pcdata->countdown + 10) / 3;
    ch->pcdata->questobj = questitem->pIndexData->vnum;

    switch (number_range(0, 1))
    {
      default:
      case 0:
        sprintf(buf,
                "Vile pilferers have stolen %s {Wfrom the royal treasury!",
                questitem->short_descr);
        do_mob_tell(ch, questman, buf);
        do_mob_tell(ch, questman,
                    "My court wizardess, with her magic mirror, has pinpointed its location.");
        break;
      case 1:
        sprintf(buf,
                "A powerful wizard has stolen %s {Wfor his personal power!",
                questitem->short_descr);
        do_mob_tell(ch, questman, buf);
        break;
    }

    if (room->name != NULL)
    {
      sprintf(buf, "Look for %s {Wsomewhere in the vicinity of %s!",
              questitem->short_descr, room->name);
      do_mob_tell(ch, questman, buf);
      sprintf(buf, "That location is in the general area of %s.",
              room->area->name);
      do_mob_tell(ch, questman, buf);
    }
  }

  /* 
   * Quest to kill a mob 
   */

  else
  {
    switch (number_range(0, 3))
    {
      default:
      case 0:
        sprintf(buf,
                "An enemy of mine, %s, {Wis making vile threats against the crown.",
                victim->short_descr);
        do_mob_tell(ch, questman, buf);
        do_mob_tell(ch, questman,
                    "King Garm has authorized an award to the one who eliminates this threat.");
        break;
      case 1:
        sprintf(buf,
                "Gaalstrom's most heinous criminal, %s, {Whas escaped from the dungeon!",
                victim->short_descr);
        do_mob_tell(ch, questman, buf);
        sprintf(buf,
                "Since the escape, %s {Whas murdered %d civilians!",
                victim->short_descr, number_range(2, 20));
        do_mob_tell(ch, questman, buf);
        do_mob_tell(ch, questman,
                    "The penalty for this crime is death, and you are to deliver the sentence!");
        break;
      case 2:
        sprintf(buf,
                "A citizen of Gaalstrom has recently been attacked by %s{W.  This is an act of war!",
                victim->short_descr);
        do_mob_tell(ch, questman, buf);
        sprintf(buf,
                "King Garm has authorized a reward to whoever punishes %s {Wwith death.",
                victim->short_descr);
        do_mob_tell(ch, questman, buf);
        break;
      case 3:
        sprintf(buf,
                "%s {Whas been stealing valuables from the citizens of Gaalstrom.",
                victim->short_descr);
        do_mob_tell(ch, questman, buf);
        sprintf(buf,
                "King Garm will reward anyone who makes sure that %s {Wnever has the chance to steal again.",
                victim->short_descr);
        do_mob_tell(ch, questman, buf);
        break;
    }

    if (room->name != NULL)
    {
      sprintf(buf, "Seek %s {Wout somewhere in the vicinity of %s!",
              victim->short_descr, room->name);
      do_mob_tell(ch, questman, buf);
      sprintf(buf, "That location is in the general area of %s.",
              room->area->name);
      do_mob_tell(ch, questman, buf);
    }
    ch->pcdata->questmob = victim->pIndexData->vnum;
  }
  if (ch->pcdata->questmob > 0 || ch->pcdata->questobj > 0)
  {
    SET_BIT(ch->act, PLR_QUESTOR);
    sprintf(buf, "You have %d minutes to complete this quest.",
            ch->pcdata->countdown);
    do_mob_tell(ch, questman, buf);
    do_mob_tell(ch, questman, "May the gods go with you!");
  }
  else
    end_quest(ch, 5);
  return;
}

bool quest_level_diff(CHAR_DATA * ch, CHAR_DATA * mob)
{
  if (IS_IMMORTAL(ch))
    return true;

  if ((ch->level + 20) < mob->level)
    return false;

  if ((mob->level + 50) < ch->level)
    return false;

  return true;
}

void quest_update(void)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *ch;

  for (d = descriptor_list; d != NULL; d = d->next)
  {
    if (d->connected == CON_PLAYING &&
        (ch = (d->original ? d->original : d->character)) != NULL)
    {

      if (ch->pcdata->nextquest > 0)
      {
        ch->pcdata->nextquest--;
        if (ch->pcdata->nextquest == 0)
        {
          send_to_char("You may now quest again.\n\r", ch);
          return;
        }
      }
      else if (IS_SET(ch->act, PLR_QUESTOR))
      {
        if (--ch->pcdata->countdown <= 0)
        {
          printf_to_char(ch,
                         "You have run out of time for your quest!\n\rYou may quest again in %d minutes.\n\r",
                         ch->pcdata->nextquest);
          end_quest(ch, 5);
        }
        if (ch->pcdata->countdown > 0 && ch->pcdata->countdown < 6)
        {
          send_to_char
            ("Better hurry, you're almost out of time for your quest!\n\r",
             ch);
          return;
        }
      }
    }
  }
  return;
}

void end_quest(CHAR_DATA * ch, int time)
{
  REMOVE_BIT(ch->act, PLR_QUESTOR);
  ch->pcdata->questgiver = 0;
  ch->pcdata->countdown = 0;
  ch->pcdata->questmob = 0;
  ch->pcdata->questobj = 0;
  ch->pcdata->questloc = 0;
  ch->pcdata->nextquest = (time);
}

void do_mob_tell(CHAR_DATA * ch, CHAR_DATA * victim, char *argument)
{
  char buf[MSL];

  sprintf(buf, "{R%s{R tells you '{W%s{R'{x\n\r", victim->short_descr,
          argument);
  send_to_char(buf, ch);
  return;
}

CH_CMD(do_qpgive)
{
  CHAR_DATA *victim;
  char arg[MIL], buf[MIL];
  int amount;

  if (!ch || IS_NPC(ch))
    return;
  argument = one_argument(argument, arg);

  if (argument[0] == '\0' || !is_number(arg))
  {
    send_to_char("Syntax: qpgive [amount] [person]\n\r", ch);
    return;
  }

  if ((amount = atoi(arg)) <= 0)
  {
    send_to_char("Give how many questpoints?\n\r", ch);
    return;
  }

  if (amount > ch->pcdata->questpoints)
  {
    send_to_char("You don't have that many questpoints to give.\n\r", ch);
    return;
  }

  if ((victim = get_char_room(ch, argument)) == NULL)
  {
    send_to_char("That person is not here.\n\r", ch);
    return;
  }

  if (IS_NPC(ch))
    return;

  if (IS_NPC(victim))
    return;

  ch->pcdata->questpoints -= amount;
  victim->pcdata->questpoints += amount;
  sprintf(buf, "%d", amount);
  act("$n gives you $t questpoints.", ch, buf, victim, TO_VICT);
  act("You give $N $t questpoints.", ch, buf, victim, TO_CHAR);
  act("$n gives $N $t questpoints.", ch, buf, victim, TO_NOTVICT);
  return;
}
