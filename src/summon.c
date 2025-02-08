// Written by Carl F Otto III ( Distortions, dist@bluehalo.homeunix.org )
// You may distribute this freely as long as the credits are intact.
// For non-profit use only. For for-profit use contact the author.


#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"
#include "db.h"

/* Adjust MAX_SUMMON in merc.h */

// No limits. Add/delete/reoder as needed..
// The "type" name is the 2nd word of the spell name like 'summon mephit'.

const struct summon_type summon_table[] = {
  /*  { "subtype",        "type",         vnum,lev, hp,  mv,  ma, lag}, */
  {"ice", "mephit", 100, 95, 25, 8, 25, 0},
  {"dust", "mephit", 101, 95, 20, 5, 20, 0},
  {"magma", "mephit", 102, 115, 30, 10, 30, 0},
  {"steam", "mephit", 103, 135, 35, 13, 35, 0},
  {"ooze", "mephit", 104, 155, 40, 15, 40, 0},
  {"smoke", "mephit", 105, 175, 45, 17, 45, 0},
  {"air", "elemental", 110, 75, 20, 5, 20, 0},
  {"water", "elemental", 111, 100, 30, 10, 30, 0},
  {"earth", "elemental", 112, 125, 40, 15, 40, 0},
  {"fire", "elemental", 113, 150, 50, 20, 50, 0},
  {"bear", "companion", 120, 150, 50, 20, 50, 0},
  {"panther", "companion", 121, 125, 40, 15, 40, 0},
  {"wolf", "companion", 122, 100, 30, 10, 30, 0},
  {"hawk", "companion", 123, 75, 20, 5, 20, 0},
  {"lantern", "archon", 130, 75, 20, 5, 20, 0},
  {"trumpet", "archon", 131, 100, 30, 10, 30, 0},
  {"hound", "archon", 132, 125, 40, 15, 40, 0},
  {"crown", "archon", 133, 150, 50, 20, 50, 0},
  {"sparrow", "songbird", 142, 75, 20, 5, 20, 0},
  {"mockingbird", "songbird", 143, 100, 30, 10, 30, 0},
  {"dove", "songbird", 140, 125, 40, 15, 40, 0},
  {"raven", "songbird", 141, 150, 50, 20, 50, 0},
  {"skeleton", "resurrect", 150, 100, 30, 10, 30, 0},
  {"zombie", "resurrect", 151, 75, 20, 5, 20, 0},
  {"wraith", "resurrect", 152, 150, 50, 20, 50, 0},
  {"spectre", "resurrect", 153, 125, 40, 15, 40, 0},
  {"north", "barrier", 160, 180, 45, 0, 45, 12},
  {"east", "barrier", 161, 180, 45, 0, 45, 12},
  {"south", "barrier", 162, 180, 45, 0, 45, 12},
  {"west", "barrier", 163, 180, 45, 0, 45, 12},
  {"up", "barrier", 164, 180, 45, 0, 45, 12},
  {"down", "barrier", 165, 180, 45, 0, 45, 12},
};

// This is where all the magic happens ( literally ).
void get_summon_type(CHAR_DATA * ch, char *argument, int sn)
{
  CHAR_DATA *victim;
  CHAR_DATA *gch;
  MOB_INDEX_DATA *pMobIndex;
  OBJ_DATA *corpse;


  int vnum, i, summon_id;
  int corpse_item_count;

  int animated;
  int follower;
  int summon;
  int resurrected;

  char buf[MSL];
  char arg1[MIL];
  char arg2[MIL];
  char arg3[MIL];
  char arg4[MIL];
  char sarg1[MIL];
  char type[MSL];
  char act_buf[MSL];
  char lev_buf[MSL];
  char noarg_buf[MSL];
  char spell_buf[MSL-256];
  char spell_temp_buf[MSL];
  char spell_name[MSL];
  char spell_name_tmp[MSL];
  char corpse_item_buf[MSL];

  bool found;
  bool corpse_found;
  bool is_bone;
  bool group_found;

  // Nice fresh slate before we start.
  found = false;
  corpse_found = false;
  is_bone = false;
  group_found = false;
  i = 0;
  vnum = 0;
  summon_id = 0;
  corpse_item_count = 0;
  summon = 0;
  follower = 0;
  animated = 0;
  resurrected = 0;
  buf[0] = '\0';
  arg1[0] = '\0';
  arg2[0] = '\0';
  arg3[0] = '\0';
  arg4[0] = '\0';
  sarg1[0] = '\0';
  type[0] = '\0';
  act_buf[0] = '\0';
  lev_buf[0] = '\0';
  noarg_buf[0] = '\0';
  spell_buf[0] = '\0';
  spell_temp_buf[0] = '\0';
  spell_name[0] = '\0';
  spell_name_tmp[0] = '\0';
  corpse_item_buf[0] = '\0';


  // Would you like one word, or two?
  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);
  argument = one_argument(argument, arg3);
  argument = one_argument(argument, arg4);

  // Quicker and safer than strcat / reset repeat.
  sprintf(spell_name_tmp, "%s", skill_table[sn].name);
  sprintf(spell_name_tmp, "%s", one_argument(spell_name_tmp, sarg1));
  one_argument(spell_name_tmp, spell_name);

  // If there is no second word, use the first. Such as for resurrect.
  if (strlen(spell_name) <= 1)
    sprintf(spell_name, "%s", skill_table[sn].name);

  // Search for the summon type and subtype.
  while (i < MAX_SUMMON)
  {

    if (!str_prefix(arg2, summon_table[i].name) &&  // Correct summon subtype?
        !str_cmp(summon_table[i].type, spell_name)) // Correct summon type?
    {
      if (strlen(arg2) > 1)
        found = true;           // WE GOT ONE!!!
      // *Hand slams on button*
      // *Fire alarm goes off*

      // Remember the info from the search
      vnum = summon_table[i].vnum;
      sprintf(type, "%s", summon_table[i].type);
      summon_id = i;
    }

    i++;                        // NEXT!
  }

  i = 0;                        // Clear so we count correctly!

  if (!found)                   // Damn we couldn't find that one.
  {
    while (i < MAX_SUMMON)
    {
      if (!str_cmp(summon_table[i].type, spell_name))
      {
        sprintf(spell_temp_buf, "%d: '%s' %s\n\r",
                summon_table[i].level_needed, skill_table[sn].name,
                summon_table[i].name);
        strcat(spell_buf, spell_temp_buf);
      }
      i++;
    }

    if (i == 0)
      // Send a list of *correct* syntax instead starring blankly and blinking slowly. 
      sprintf(noarg_buf, "There isn't anything by that name to summon!\n\r");
    else
      // Print out the spell name and the summon subtypes.
      sprintf(noarg_buf, "Syntax: cast:\n\rlevel: 'spell' type\n\r%s",
              spell_buf);

    send_to_char(noarg_buf, ch);
    return;
  }

  // Make sure they are allowed to cast it!
  if (summon_table[summon_id].level_needed > ch->level)
  {
    sprintf(lev_buf, "You must be level %d to cast %s %s.\n\r",
            summon_table[summon_id].level_needed, skill_table[sn].name,
            summon_table[summon_id].name);
    send_to_char(lev_buf, ch);
    return;                     // Stop, go directly to jail.
    // Do not pass GO or collect 200 mana.
  }

  for (gch = char_list; gch != NULL; gch = gch->next)
  {
    if (is_same_group(gch, ch) && IS_NPC(gch))
    {
      i = 0;
      while (i < MAX_SUMMON)
      {
        if (summon_table[i].vnum == gch->pIndexData->vnum)
        {
          summon++;
        }
        i++;
      }
    }
  }

  if (summon >= 2)
  {
    send_to_char
      ("You can not have any more summoned beings in your control.\n\r", ch);
    return;
  }

  // If we are resurrecting something we need a presentable corpse to use.    
  if (!str_cmp(skill_table[sn].name, "bone barrier"))
  {
    if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
    {
      send_to_char("You can not summon a bone barrier in a safe room.\n\r",
                   ch);
      return;
    }
    is_bone = true;
  }

  if (!str_cmp(skill_table[sn].name, "resurrect"))
  {

    //Hmm.. Let me see what I can dig up..
    corpse = get_obj_list(ch, arg3, ch->in_room->contents);
    if (corpse == NULL)
    {
      if (arg3[0] != '\0')
        act("I see no $T here.", ch, NULL, arg3, TO_CHAR);
      else
        send_to_char("You must specify a corpse to resurrect.\n\r", ch);

      return;
    }
    else
    {

      if (corpse->item_type == ITEM_CORPSE_NPC ||
          corpse->item_type == ITEM_CORPSE_PC)
      {
        corpse_found = true;    // This should do just fine            
      }
      else
      {
        send_to_char("Only corpses can be resurrected.\n\r", ch);
        corpse_found = false;
        return;
      }

    }
    // If it is a PC corpse, make sure it is lootable first.
    if (corpse_found)
    {
      if (corpse->item_type == ITEM_CORPSE_PC && !can_loot(ch, corpse))
      {
        send_to_char("That corpse is protected.\n\r", ch);
        return;
      }

      // If there are any contents, drop them to the floor.
      if (corpse->contains)
        obj_from_room(corpse);

      extract_obj(corpse);
      corpse = NULL;
    }
    else
    {
      send_to_char("The corpse was not found.\n\r", ch);
      return;
    }

  }

  // Better see if the mob actually exists before we go loading it..
  if ((pMobIndex = get_mob_index(vnum)) == NULL)
  {
    // Builder: What you say !! 
    // Coder: You have no chance to survive make your time.
    // Coder: HA HA HA HA ....

    send_to_char
      ("Sorry, an error has occurred. Please write a note to 'immortal'.\n\r",
       ch);
    bug("Summon: Tried to load a vnum that did not exist: %d!", vnum);
    return;
  }

  victim = create_mobile(pMobIndex);  // Time to make the mob data..
  // I made the mob data..

  // No, bad mob. No OOC and no cookie either!
  victim->comm = COMM_NOTELL | COMM_NOSHOUT | COMM_NOCHANNELS;

  // Mark their forehead :E.. DO IT NOW!!!

  sprintf(act_buf, "%s\n\r{w%s has the mark of %s on its forehead.\n\r",
          victim->description, victim->short_descr, ch->name);
  free_string(victim->description);
  victim->description = str_dup(act_buf);

  // Set up the mobs stuff before we pop them into the room.
  victim->alignment = ch->alignment;
  victim->level = ((ch->level / 2) + 1);

  // Give them stats based on the table data and the player level
  victim->max_hit =
    (ch->level * summon_table[summon_id].hp_usage + dice(36, 36) + 1);
  victim->max_mana =
    ((ch->level * summon_table[summon_id].mana_usage + dice(36, 36)) + 1 / 2);
  victim->max_move =
    (ch->level * summon_table[summon_id].move_usage + dice(36, 36) + 1);

  // Fill the mobs hp/mana/move up
  victim->hit = victim->max_hit;
  victim->mana = victim->max_mana;
  victim->move = victim->max_move;

  // Make sure they have enought of the stats to cast
  if (((summon_table[summon_id].hp_usage * 100) > ch->hit) ||
      ((summon_table[summon_id].move_usage * 100) > ch->move) ||
      ((summon_table[summon_id].mana_usage * 100) > ch->mana))
  {
    send_to_char("You do not have energy to do that.\n\r", ch);
    victim = NULL;
    pMobIndex = NULL;
    return;                     // I would suggest getting more sleep.
  }

  // SLUUuurrp *blech*
  ch->hit -= (summon_table[summon_id].hp_usage * 100);
  ch->move -= (summon_table[summon_id].move_usage * 100);
  ch->mana -= (summon_table[summon_id].mana_usage * 50);

  // By our powers combined..
  act("$n summons $N and channels their energy into it.", ch, NULL, victim,
      TO_ROOM);
  act("You summon $N and channel your energy into it.", ch, NULL, victim,
      TO_CHAR);

  // Ok, shes all ready to go. Beam us abord scotty.
  char_to_room(victim, ch->in_room);

  // Last minute adjustments
  if (!is_bone)
  {
    victim->ptype = 2;
    victim->master = ch;
    victim->leader = ch;
    if (!IS_SET(victim->affected_by, AFF_CHARM))
      SET_BIT(victim->affected_by, AFF_CHARM);
  }

  return;                       // "Summoning is complete." Jeez, finally..
}
