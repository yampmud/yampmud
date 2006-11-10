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

#if defined(macintosh)
#include <types.h>
#include <time.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "tables.h"
#include "stdlib.h"

void do_htmlout (  )
{
    char buf[MSL];
    FILE *fp;
    DESCRIPTOR_DATA *d;
    int count;

    count = 0;
    if ( ( fp = file_open ( HTML_FILE, "w" ) ) == NULL )
    {
        bug ( "HTML_FILE: fopen", 0 );
        perror ( HTML_FILE );
    }
    else
    {
        fprintf ( fp,
                  "<html><head><title>Mud Stats</title></head><body bgcolor=003366 text=FFFFFF alink=666666 vlink=666666 link=888888>\n\r" );
        for ( d = descriptor_list; d; d = d->next )
        {
            CHAR_DATA *wch;

            if ( d->connected == CON_PLAYING )
            {
                count++;
                wch = ( d->original != NULL ) ? d->original : d->character;
                if ( !IS_SHIELDED ( wch, SHD_SWALK ) &&
                     !IS_SHIELDED ( wch, SHD_VANISH ) &&
                     wch->incog_level <= LEVEL_ANCIENT &&
                     wch->invis_level <= LEVEL_ANCIENT )
                {
                    sprintf ( buf, "%d %s %s<br>\n\r", wch->level, wch->name,
                              IS_SET ( wch->comm, COMM_AFK ) ? "(AFK)" : "" );
                    fprintf ( fp, buf );
                }
            }
        }

        sprintf ( buf, "<br>%d total players online.", count );
        fprintf ( fp, buf );

        sprintf ( buf, "<br>%d players on was the most today.", max_on );
        fprintf ( fp, buf );

        sprintf ( buf, "<br>There was a copyover %.2f hours ago.",
                  ( uptime_ticks / 60.00 ) );
        fprintf ( fp, buf );

        fprintf ( fp, "</body></html>\n\r" );
    }
    file_close ( fp );

    return;
}

CH_CMD ( do_redeem )
{
    OBJ_DATA *reward;
    int len, i;
    char buf[MSL];
    char out[MSL];
    char fin[MSL];

    i = 0;
    len = 0;
    buf[0] = '\0';
    out[0] = '\0';
    fin[0] = '\0';

    if ( IS_NPC ( ch ) || ch->redeem > 0 )
        return;

    sprintf ( buf, ch->name );

    len = strlen ( buf );
    while ( i < len && i < 9 )
    {
        sprintf ( out, "%d", ( ( LOWER ( buf[i] ) - 'a' ) / 3 ) );
        strcat ( fin, out );
        i++;
    }
    if ( !str_cmp ( fin, argument ) )
    {
        send_to_char ( "Key is correct!\n\r", ch );
        reward = create_object ( get_obj_index ( 523 ), 0 );
        obj_to_room ( reward, ch->in_room );
        do_get ( ch, "back" );
        ch->redeem++;
        return;
    }
    else
    {
        send_to_char ( "Key is incorrect!\n\r", ch );
        return;
    }

    return;
}

CH_CMD ( do_drag )
{
    int door = 0;
    char buf[MSL];
    char arg1[MIL];
    char arg2[MIL];
    bool item = true;
    CHAR_DATA *victim;
    OBJ_DATA *obj;

    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) )
    {
        send_to_char ( "Not in the arena you dont...\n\n", ch );
        return;
    }

    if ( !IS_IMMORTAL ( ch ) )
        WAIT_STATE ( ch, 24 );

    if ( ( ( ( !str_cmp ( class_table[ch->class].name, "Knight" ) ||
               !str_cmp ( class_table[ch->class].name, "Strider" ) ) &&
             ch->level >= 202 ) || IS_IMMORTAL ( ch ) ) )
    {
        if ( ch->position == POS_FIGHTING )
        {
            send_to_char ( "Maybe you should finish fighting first.\n\r", ch );
            return;
        }

        if ( argument[0] == '\0' )
        {
            send_to_char
                ( "Drag what where? ( drag <item/person> <direction> )\n\r",
                  ch );
            return;
        }

        argument = one_argument ( argument, arg1 );
        argument = one_argument ( argument, arg2 );

        if ( !str_cmp ( arg2, "n" ) || !str_cmp ( arg2, "north" ) )
            door = 0;
        else if ( !str_cmp ( arg2, "e" ) || !str_cmp ( arg2, "east" ) )
            door = 1;
        else if ( !str_cmp ( arg2, "s" ) || !str_cmp ( arg2, "south" ) )
            door = 2;
        else if ( !str_cmp ( arg2, "w" ) || !str_cmp ( arg2, "west" ) )
            door = 3;
        else if ( !str_cmp ( arg2, "u" ) || !str_cmp ( arg2, "up" ) )
            door = 4;

        else if ( !str_cmp ( arg2, "d" ) || !str_cmp ( arg2, "down" ) )
            door = 5;
        else
        {
            send_to_char ( "Drag in what direction?\n\r", ch );
            return;
        }

        obj = get_obj_list ( ch, arg1, ch->in_room->contents );

        if ( ( victim = get_char_room ( ch, arg1 ) ) == NULL )
        {
            if ( obj == NULL )
            {
                send_to_char ( "Drag what?\n\r", ch );
                return;
            }
        }
        else
        {
            if ( IS_NPC ( victim ) && !IS_IMMORTAL ( ch ) )
            {
                send_to_char ( "You can't drag NPCs.", ch );
                return;
            }
            item = false;
        }

        if ( item && obj != NULL )
            if ( ( !IS_SET ( obj->wear_flags, ITEM_TAKE ) ||
                   IS_SET ( obj->in_room->room_flags, ROOM_NODRAG ) ) &&
                 !IS_IMMORTAL ( ch ) )
            {
                send_to_char ( "It wont budge.\n\r", ch );
                return;
            }

        if ( item && obj != NULL )
            if ( obj->weight > ( 2 * can_carry_w ( ch ) ) &&
                 !IS_IMMORTAL ( ch ) )
            {
                send_to_char ( "It is too heavy.\n\r", ch );
                return;
            }

        if ( item )
        {
            sprintf ( buf, "You drag the %s behind you.\n\r",
                      obj->short_descr );
            send_to_char ( buf, ch );
            act ( "$n drags $p out of the room with $m.", ch, obj, NULL,
                  TO_ROOM );
            move_char ( ch, door, false, false );
            obj_from_room ( obj );
            obj_to_room ( obj, ch->in_room );
        }
        else
        {
            if ( victim->position == POS_FIGHTING && !IS_IMMORTAL ( ch ) )
            {
                send_to_char ( "You cant seem to grab a hold on them.\n\r",
                               ch );
                return;
            }
            if ( IS_AFFECTED ( victim, AFF_ENTANGLE ) )
            {
                if ( number_range ( 1, 100 ) >= 90 )
                {
                    act ( "$n drags your entangled body with $m!", ch, NULL,
                          victim, TO_VICT );
                    act ( "$n drags $N's entangled body with $m!", ch, NULL,
                          victim, TO_NOTVICT );
                    act ( "You drag $N's entangled body with you!", ch, NULL,
                          victim, TO_CHAR );
                }
                else
                {
                    act ( "$n tries to drag your from the room, but the web of mana holds.", ch, NULL, victim, TO_VICT );
                    act ( "$n tries to drag $N from the room, but the web of mana holds.", ch, NULL, victim, TO_NOTVICT );
                    act ( "You try to drag $N from the room, but the web of mana holds.", ch, NULL, victim, TO_CHAR );
                    return;
                }
            }
            if ( victim == ch )
            {
                send_to_char ( "Drag yourself??\n\r", ch );
                return;
            }
            if ( IS_SET ( ch->in_room->room_flags, ROOM_NODRAG ) &&
                 !IS_IMMORTAL ( ch ) )
            {
                send_to_char ( "You can not drag people from this room.\n\r",
                               ch );
                return;
            }
            if ( ( ch->move / 3 ) > 0 || IS_IMMORTAL ( ch ) )
            {
                if ( !IS_IMMORTAL ( ch ) )
                    ch->move = ch->move / 3;
                sprintf ( buf, "You drag %s with you.\n\r", victim->name );
                send_to_char ( buf, ch );
                act ( "$n drags you with $m.", ch, NULL, victim, TO_VICT );
                act ( "$n drags $N out of the room with $m.", ch, NULL, victim,
                      TO_NOTVICT );
                WAIT_STATE ( ch, 24 );
                move_char ( ch, door, false, false );
                char_from_room ( victim );
                char_to_room ( victim, ch->in_room );
            }
            else
            {
                sprintf ( buf,
                          "You are too exhausted to drag %s any farther.\n\r",
                          victim->name );
                send_to_char ( buf, ch );
                return;
            }
        }

        ch->movement_timer = 0;
    }
    return;
}

void move_char ( CHAR_DATA * ch, int door, bool follow, bool quiet )
{
    CHAR_DATA *fch;
    CHAR_DATA *fch_next;
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    EXIT_DATA *pexit;
    int track;

    if ( door < 0 || door >= MAX_DIR )
    {
        bug ( "Do_move: bad door %d.", door );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_ENTANGLE ) )
    {
        send_to_char ( "You strugle against the web but you cant get out!!\n\r",
                       ch );
        act ( "$n strugles against a web of mana, but cant get out!!", ch, NULL,
              NULL, TO_ROOM );
        if ( ch->move - ( ch->max_move / 20 ) > 0 )
            ch->move -= ( ch->max_move / 20 );
        return;
    }

    /* 
     * Exit trigger, if activated, bail out. Only PCs are triggered. */
    if ( !IS_NPC ( ch ) && mp_exit_trigger ( ch, door ) )
        return;

    in_room = ch->in_room;

    if ( ( pexit = in_room->exit[door] ) == NULL )
    {
        if ( !quiet )
        {
            OBJ_DATA *portal;

            portal = get_obj_list ( ch, dir_name[door], ch->in_room->contents );
            if ( portal != NULL )
            {
                enter_exit ( ch, dir_name[door] );
                return;
            }
        }
    }

    if ( ( pexit = in_room->exit[door] ) == NULL ||
         ( to_room = pexit->u1.to_room ) == NULL ||
         !can_see_room ( ch, pexit->u1.to_room ) )
    {
        if ( !quiet )
            send_to_char ( "Alas, you cannot go that way.\n\r", ch );
        return;
    }

    if ( IS_SET ( pexit->exit_info, EX_CLOSED ) &&
         ( !IS_AFFECTED ( ch, AFF_PASS_DOOR ) ||
           IS_SET ( pexit->exit_info, EX_NOPASS ) ) &&
         !IS_TRUSTED ( ch, KNIGHT ) )
    {
        if ( !quiet )
            act ( "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
        return;
    }

    if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master != NULL &&
         in_room == ch->master->in_room )
    {
        if ( !quiet )
            send_to_char ( "What?  And leave your beloved master?\n\r", ch );
        return;
    }

    if ( !is_room_owner ( ch, to_room ) && room_is_private ( ch, to_room ) )
    {
        if ( !quiet )
            send_to_char ( "That room is private right now.\n\r", ch );
        return;
    }

    if ( !IS_NPC ( ch ) )
    {
        int move;

/* We don't need a guild test.  It's messing up my vnums and we don't use it.
        int iClass, iGuild;
        bool ts1;
        bool ts2;

        ts1 = false;
        ts2 = false;

        for ( iClass = 0; iClass < MAX_CLASS; iClass++ )
        {
            for ( iGuild = 0; iGuild < MAX_GUILD; iGuild++ )
            {
                if ( iClass != ch->class &&
                     to_room->vnum == class_table[iClass].guild[iGuild] )
                {
                    ts1 = true;
                }
                if ( iClass == ch->class &&
                     to_room->vnum == class_table[iClass].guild[iGuild] )
                {
                    ts2 = true;
                }
            }
        }

        if ( ts2 )
        {
            ts1 = false;
        }
        if ( ts1 )
        {
            if ( !quiet )
                send_to_char ( "You aren't allowed in there.\n\r", ch );
            return;
        }   
        End of Guild check  */

        if ( in_room->sector_type == SECT_AIR ||
             to_room->sector_type == SECT_AIR )
        {
            if ( !IS_AFFECTED ( ch, AFF_FLYING ) && !IS_IMMORTAL ( ch ) )
            {
                if ( !quiet )
                    send_to_char ( "You can't fly.\n\r", ch );
                return;
            }
        }

        if ( ( in_room->sector_type == SECT_WATER_NOSWIM ||
               to_room->sector_type == SECT_WATER_NOSWIM ) &&
             !IS_AFFECTED ( ch, AFF_FLYING ) )
        {
            OBJ_DATA *obj;
            bool found;

            /* 
             * Look for a boat.
             */
            found = false;

            if ( IS_IMMORTAL ( ch ) )
                found = true;

            for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
            {
                if ( obj->item_type == ITEM_BOAT )
                {
                    found = true;
                    break;
                }
            }
            if ( !found )
            {
                if ( !quiet )
                    send_to_char ( "You need a boat to go there.\n\r", ch );
                return;
            }
        }

        move =
            movement_loss[UMIN ( SECT_MAX - 1, in_room->sector_type )] +
            movement_loss[UMIN ( SECT_MAX - 1, to_room->sector_type )];

        move /= 2;              /* i.e. the average */

        /* conditional effects */
        if ( IS_AFFECTED ( ch, AFF_FLYING ) || IS_AFFECTED ( ch, AFF_HASTE ) )
            move /= 2;

        if ( IS_AFFECTED ( ch, AFF_SLOW ) )
            move *= 2;

        if ( ch->move < move )
        {
            if ( !quiet )
                send_to_char ( "You are too exhausted.\n\r", ch );
            return;
        }

        if ( !IS_IMMORTAL ( ch ) )
            WAIT_STATE ( ch, 1 );
        ch->move -= move;
    }

    if ( ( !IS_AFFECTED ( ch, AFF_SNEAK ) && !IS_AFFECTED ( ch, SHD_SWALK ) ) &&
         ch->invis_level <= LEVEL_ANCIENT && ch->ghost_level <= LEVEL_ANCIENT &&
         !IS_SHIELDED ( ch, SHD_VANISH ) )
        if ( !quiet )
            act ( "$n leaves $T.", ch, NULL, dir_name[door], TO_ROOM );

    char_from_room ( ch );
    char_to_room ( ch, to_room );
    if ( IS_NPC ( ch ) || !IS_IMMORTAL ( ch ) )
    {
        for ( track = MAX_TRACK - 1; track > 0; track-- )
        {
            ch->track_to[track] = ch->track_to[track - 1];
            ch->track_from[track] = ch->track_from[track - 1];
        }
        if ( IS_AFFECTED ( ch, AFF_FLYING ) )
        {
            ch->track_from[0] = 0;
            ch->track_to[0] = 0;
        }
        else
        {
            ch->track_from[0] = in_room->vnum;
            ch->track_to[0] = to_room->vnum;
        }
    }
    if ( ( !IS_AFFECTED ( ch, AFF_SNEAK ) && !IS_AFFECTED ( ch, SHD_SWALK ) ) &&
         ch->invis_level <= LEVEL_ANCIENT && ch->ghost_level <= LEVEL_ANCIENT &&
         !IS_SHIELDED ( ch, SHD_VANISH ) )
    {
        if ( !quiet )
            act ( "$n has arrived.", ch, NULL, NULL, TO_ROOM );
        if ( IS_NPC ( ch ) )
        {
            if ( ch->say_descr[0] != '\0' && ch->say_descr != NULL )
            {
                if ( !quiet )
                    act ( "$n says '{a$T{x'", ch, NULL, ch->say_descr,
                          TO_ROOM );
            }
        }
    }
    if ( !quiet )
        do_look ( ch, "auto" );

    if ( in_room == to_room )   /* no circular follows */
        return;

    for ( fch = in_room->people; fch != NULL; fch = fch_next )
    {
        fch_next = fch->next_in_room;

        if ( fch->master == ch && IS_AFFECTED ( fch, AFF_CHARM ) &&
             fch->position < POS_STANDING )
            do_stand ( fch, "" );

        if ( fch->master == ch && fch->position == POS_STANDING &&
             can_see_room ( fch, to_room ) )
        {

            if ( IS_SET ( ch->in_room->room_flags, ROOM_LAW ) &&
                 ( IS_NPC ( fch ) && IS_SET ( fch->act, ACT_AGGRESSIVE ) ) )
            {
                act ( "You can't bring $N into the city.", ch, NULL, fch,
                      TO_CHAR );
                act ( "You aren't allowed in the city.", fch, NULL, NULL,
                      TO_CHAR );
                continue;
            }

            act ( "You follow $N.", fch, NULL, ch, TO_CHAR );
            move_char ( fch, door, true, false );
        }
    }

    /* 
     * If someone is following the char, these triggers get activated
     * for the followers before the char, but it's safer this way...
     */
    if ( IS_NPC ( ch ) && HAS_TRIGGER ( ch, TRIG_ENTRY ) )
        mp_percent_trigger ( ch, NULL, NULL, NULL, TRIG_ENTRY );
    if ( !IS_NPC ( ch ) )
        mp_greet_trigger ( ch );

    return;
}

/* RW Enter movable exits */
void enter_exit ( CHAR_DATA * ch, char *arg )
{
    ROOM_INDEX_DATA *location;
    int track;

    if ( ch->fighting != NULL )
        return;

    /* nifty portal stuff */
    if ( arg[0] != '\0' )
    {
        ROOM_INDEX_DATA *old_room;
        OBJ_DATA *portal;
        CHAR_DATA *fch, *fch_next;

        old_room = ch->in_room;

        portal = get_obj_list ( ch, arg, ch->in_room->contents );

        if ( portal == NULL )
        {
            send_to_char ( "Alas, you cannot go that way.\n\r", ch );
            return;
        }

        if ( portal->item_type != ITEM_EXIT )
        {
            send_to_char ( "Alas, you cannot go that way.\n\r", ch );
            return;
        }

        location = get_room_index ( portal->value[0] );

        if ( location == NULL || location == old_room ||
             !can_see_room ( ch, location ) ||
             ( room_is_private ( ch, location ) &&
               !IS_TRUSTED ( ch, IMPLEMENTOR ) ) )
        {
            send_to_char ( "Alas, you cannot go that way.\n\r", ch );
            return;
        }

        if ( IS_AFFECTED ( ch, AFF_CHARM ) && ch->master != NULL &&
             old_room == ch->master->in_room )
        {
            send_to_char ( "What?  And leave your beloved master?\n\r", ch );
            return;
        }

        if ( IS_NPC ( ch ) && IS_SET ( ch->act, ACT_AGGRESSIVE ) &&
             IS_SET ( location->room_flags, ROOM_LAW ) )
        {
            send_to_char ( "You aren't allowed in the city.\n\r", ch );
            return;
        }

        if ( !IS_NPC ( ch ) )
        {
            int move;

            if ( old_room->sector_type == SECT_AIR ||
                 location->sector_type == SECT_AIR )
            {
                if ( !IS_AFFECTED ( ch, AFF_FLYING ) && !IS_IMMORTAL ( ch ) )
                {
                    send_to_char ( "You can't fly.\n\r", ch );
                    return;
                }
            }

            if ( ( old_room->sector_type == SECT_WATER_NOSWIM ||
                   location->sector_type == SECT_WATER_NOSWIM ) &&
                 !IS_AFFECTED ( ch, AFF_FLYING ) )
            {
                OBJ_DATA *obj;
                bool found;

                /* 
                 * Look for a boat. */
                found = false;

                if ( IS_IMMORTAL ( ch ) )
                    found = true;

                for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
                {
                    if ( obj->item_type == ITEM_BOAT )
                    {
                        found = true;
                        break;
                    }
                }
                if ( !found )
                {
                    send_to_char ( "You need a boat to go there.\n\r", ch );
                    return;
                }
            }

            move =
                movement_loss[UMIN ( SECT_MAX - 1, old_room->sector_type )] +
                movement_loss[UMIN ( SECT_MAX - 1, location->sector_type )];

            move /= 2;          /* i.e. the average */

            /* conditional effects */
            if ( IS_AFFECTED ( ch, AFF_FLYING ) ||
                 IS_AFFECTED ( ch, AFF_HASTE ) )
                move /= 2;

            if ( IS_AFFECTED ( ch, AFF_SLOW ) )
                move *= 2;

            if ( ch->move < move )
            {
                send_to_char ( "You are too exhausted.\n\r", ch );
                return;
            }

            if ( !IS_IMMORTAL ( ch ) )
                WAIT_STATE ( ch, 1 );
            ch->move -= move;
        }

        if ( ( !IS_AFFECTED ( ch, AFF_SNEAK ) && !IS_AFFECTED ( ch, SHD_SWALK )
               && !IS_SHIELDED ( ch, SHD_VANISH ) ) &&
             ch->invis_level <= LEVEL_ANCIENT &&
             ch->ghost_level <= LEVEL_ANCIENT )
        {
            act ( "$n leaves $p.", ch, portal, NULL, TO_ROOM );
        }

        char_from_room ( ch );
        char_to_room ( ch, location );
        if ( IS_NPC ( ch ) || !IS_IMMORTAL ( ch ) )
        {
            for ( track = MAX_TRACK - 1; track > 0; track-- )
            {
                ch->track_to[track] = ch->track_to[track - 1];
                ch->track_from[track] = ch->track_from[track - 1];
            }
            if ( IS_AFFECTED ( ch, AFF_FLYING ) )
            {
                ch->track_from[0] = 0;
                ch->track_to[0] = 0;
            }
            else
            {
                ch->track_from[0] = old_room->vnum;
                ch->track_to[0] = location->vnum;
            }
        }

        if ( ( !IS_AFFECTED ( ch, AFF_SNEAK ) && !IS_AFFECTED ( ch, SHD_SWALK )
               && !IS_SHIELDED ( ch, SHD_VANISH ) ) &&
             ch->invis_level <= LEVEL_ANCIENT &&
             ch->ghost_level <= LEVEL_ANCIENT )
        {
            act ( "$n has arrived.", ch, NULL, NULL, TO_ROOM );
            if ( IS_NPC ( ch ) )
            {
                if ( ch->say_descr[0] != '\0' && ch->say_descr != NULL )
                {
                    act ( "$n says '{a$T{x'", ch, NULL, ch->say_descr,
                          TO_ROOM );
                }
            }
        }

        do_look ( ch, "auto" );

        /* protect against circular follows */
        if ( old_room == location )
            return;

        for ( fch = old_room->people; fch != NULL; fch = fch_next )
        {
            fch_next = fch->next_in_room;

            if ( portal == NULL )
                continue;

            if ( fch->master == ch && IS_AFFECTED ( fch, AFF_CHARM ) &&
                 fch->position < POS_STANDING )
                do_stand ( fch, "" );

            if ( fch->master == ch && fch->position == POS_STANDING &&
                 can_see_room ( fch, location ) )
            {

                if ( IS_SET ( ch->in_room->room_flags, ROOM_LAW ) &&
                     ( IS_NPC ( fch ) && IS_SET ( fch->act, ACT_AGGRESSIVE ) ) )
                {
                    act ( "You can't bring $N into the city.", ch, NULL, fch,
                          TO_CHAR );
                    act ( "You aren't allowed in the city.", fch, NULL, NULL,
                          TO_CHAR );
                    continue;
                }

                act ( "You follow $N.", fch, NULL, ch, TO_CHAR );
                enter_exit ( fch, arg );
            }
        }
        return;
    }

    send_to_char ( "Alas, you cannot go that way.\n\r", ch );
    return;
}

CH_CMD ( do_north )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_NORTH, false, false );
    return;
}

CH_CMD ( do_east )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_EAST, false, false );
    return;
}

CH_CMD ( do_south )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_SOUTH, false, false );
    return;
}

CH_CMD ( do_west )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_WEST, false, false );
    return;
}

CH_CMD ( do_up )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_UP, false, false );
    return;
}

CH_CMD ( do_down )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_DOWN, false, false );
    return;
}

CH_CMD ( do_northeast )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_NORTHEAST, false, false );
    return;
}

CH_CMD ( do_southeast )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_SOUTHEAST, false, false );
    return;
}

CH_CMD ( do_southwest )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_SOUTHWEST, false, false );
    return;
}

CH_CMD ( do_northwest )
{
    ch->movement_timer = 0;
    move_char ( ch, DIR_NORTHWEST, false, false );
    return;
}

int find_door ( CHAR_DATA * ch, char *arg )
{
    EXIT_DATA *pexit;
    int door;

    if ( !str_cmp ( arg, "n" ) || !str_cmp ( arg, "north" ) )
        door = 0;
    else if ( !str_cmp ( arg, "e" ) || !str_cmp ( arg, "east" ) )
        door = 1;
    else if ( !str_cmp ( arg, "s" ) || !str_cmp ( arg, "south" ) )
        door = 2;
    else if ( !str_cmp ( arg, "w" ) || !str_cmp ( arg, "west" ) )
        door = 3;
    else if ( !str_cmp ( arg, "u" ) || !str_cmp ( arg, "up" ) )
        door = 4;
    else if ( !str_cmp ( arg, "d" ) || !str_cmp ( arg, "down" ) )
        door = 5;
    else if ( !str_cmp ( arg, "ne" ) || !str_cmp ( arg, "northeast" ) )
        door = 6;
    else if ( !str_cmp ( arg, "se" ) || !str_cmp ( arg, "southeast" ) )
        door = 7;
    else if ( !str_cmp ( arg, "sw" ) || !str_cmp ( arg, "southwest" ) )
        door = 8;
    else if ( !str_cmp ( arg, "nw" ) || !str_cmp ( arg, "northwest" ) )
        door = 9;
    else
    {
        for ( door = 0; door < MAX_DIR; door++ )
        {
            if ( ( pexit = ch->in_room->exit[door] ) != NULL &&
                      IS_SET ( pexit->exit_info, EX_ISDOOR ) &&
                      pexit->keyword != NULL &&
                      is_name ( arg, pexit->keyword ) )
            {
                return door;
            }
        }
        act ( "I see no $T here.", ch, NULL, arg, TO_CHAR );
        return -1;
    }

    if ( ( pexit = ch->in_room->exit[door] ) == NULL )
    {
        act ( "I see no door $T here.", ch, NULL, arg, TO_CHAR );
        return -1;
    }

    if ( !IS_SET ( pexit->exit_info, EX_ISDOOR ) )
    {
        send_to_char ( "You can't do that.\n\r", ch );
        return -1;
    }

    return door;
}

CH_CMD ( do_open )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Open what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_here ( ch, arg ) ) != NULL )
    {
        /* open portal */
        if ( obj->item_type == ITEM_PORTAL )
        {
            if ( !IS_SET ( obj->value[1], EX_ISDOOR ) )
            {
                send_to_char ( "You can't do that.\n\r", ch );
                return;
            }

            if ( !IS_SET ( obj->value[1], EX_CLOSED ) )
            {
                send_to_char ( "It's already open.\n\r", ch );
                return;
            }

            if ( IS_SET ( obj->value[1], EX_LOCKED ) )
            {
                send_to_char ( "It's locked.\n\r", ch );
                return;
            }

            REMOVE_BIT ( obj->value[1], EX_CLOSED );
            act ( "You open $p.", ch, obj, NULL, TO_CHAR );
            act ( "$n opens $p.", ch, obj, NULL, TO_ROOM );
            return;
        }

        /* 'open object' */
        if ( ( obj->item_type != ITEM_CONTAINER ) &&
             ( obj->item_type != ITEM_PIT ) )
        {
            send_to_char ( "That's not a container.\n\r", ch );
            return;
        }
        if ( !IS_SET ( obj->value[1], CONT_CLOSED ) )
        {
            send_to_char ( "It's already open.\n\r", ch );
            return;
        }
        if ( !IS_SET ( obj->value[1], CONT_CLOSEABLE ) )
        {
            send_to_char ( "You can't do that.\n\r", ch );
            return;
        }
        if ( IS_SET ( obj->value[1], CONT_LOCKED ) )
        {
            send_to_char ( "It's locked.\n\r", ch );
            return;
        }

        REMOVE_BIT ( obj->value[1], CONT_CLOSED );
        act ( "You open $p.", ch, obj, NULL, TO_CHAR );
        act ( "$n opens $p.", ch, obj, NULL, TO_ROOM );
        return;
    }

    if ( ( door = find_door ( ch, arg ) ) >= 0 )
    {
        /* 'open door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if ( !IS_SET ( pexit->exit_info, EX_CLOSED ) )
        {
            send_to_char ( "It's already open.\n\r", ch );
            return;
        }
        if ( IS_SET ( pexit->exit_info, EX_LOCKED ) )
        {
            send_to_char ( "It's locked.\n\r", ch );
            return;
        }

        REMOVE_BIT ( pexit->exit_info, EX_CLOSED );
        act ( "$n opens the $d.", ch, NULL, pexit->keyword, TO_ROOM );
        send_to_char ( "Ok.\n\r", ch );

        /* open the other side */
        if ( ( to_room = pexit->u1.to_room ) != NULL &&
             ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL &&
             pexit_rev->u1.to_room == ch->in_room )
        {
            CHAR_DATA *rch;

            REMOVE_BIT ( pexit_rev->exit_info, EX_CLOSED );
            for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
                act ( "The $d opens.", rch, NULL, pexit_rev->keyword, TO_CHAR );
        }
    }

    return;
}

CH_CMD ( do_close )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Close what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_here ( ch, arg ) ) != NULL )
    {
        /* portal stuff */
        if ( obj->item_type == ITEM_PORTAL )
        {

            if ( !IS_SET ( obj->value[1], EX_ISDOOR ) ||
                 IS_SET ( obj->value[1], EX_NOCLOSE ) )
            {
                send_to_char ( "You can't do that.\n\r", ch );
                return;
            }

            if ( IS_SET ( obj->value[1], EX_CLOSED ) )
            {
                send_to_char ( "It's already closed.\n\r", ch );
                return;
            }

            SET_BIT ( obj->value[1], EX_CLOSED );
            act ( "You close $p.", ch, obj, NULL, TO_CHAR );
            act ( "$n closes $p.", ch, obj, NULL, TO_ROOM );
            return;
        }

        /* 'close object' */
        if ( ( obj->item_type != ITEM_CONTAINER ) &&
             ( obj->item_type != ITEM_PIT ) )
        {
            send_to_char ( "That's not a container.\n\r", ch );
            return;
        }
        if ( IS_SET ( obj->value[1], CONT_CLOSED ) )
        {
            send_to_char ( "It's already closed.\n\r", ch );
            return;
        }
        if ( !IS_SET ( obj->value[1], CONT_CLOSEABLE ) )
        {
            send_to_char ( "You can't do that.\n\r", ch );
            return;
        }

        SET_BIT ( obj->value[1], CONT_CLOSED );
        act ( "You close $p.", ch, obj, NULL, TO_CHAR );
        act ( "$n closes $p.", ch, obj, NULL, TO_ROOM );
        return;
    }

    if ( ( door = find_door ( ch, arg ) ) >= 0 )
    {
        /* 'close door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if ( IS_SET ( pexit->exit_info, EX_CLOSED ) )
        {
            send_to_char ( "It's already closed.\n\r", ch );
            return;
        }

        SET_BIT ( pexit->exit_info, EX_CLOSED );
        act ( "$n closes the $d.", ch, NULL, pexit->keyword, TO_ROOM );
        send_to_char ( "Ok.\n\r", ch );

        /* close the other side */
        if ( ( to_room = pexit->u1.to_room ) != NULL &&
             ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0 &&
             pexit_rev->u1.to_room == ch->in_room )
        {
            CHAR_DATA *rch;

            SET_BIT ( pexit_rev->exit_info, EX_CLOSED );
            for ( rch = to_room->people; rch != NULL; rch = rch->next_in_room )
                act ( "The $d closes.", rch, NULL, pexit_rev->keyword,
                      TO_CHAR );
        }
    }

    return;
}

bool has_key ( CHAR_DATA * ch, long key )
{
    OBJ_DATA *obj;

    for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
    {
        if ( obj->pIndexData->vnum == key )
            return true;
    }

    return false;
}

CH_CMD ( do_lock )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Lock what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_here ( ch, arg ) ) != NULL )
    {
        /* portal stuff */
        if ( obj->item_type == ITEM_PORTAL )
        {
            if ( !IS_SET ( obj->value[1], EX_ISDOOR ) ||
                 IS_SET ( obj->value[1], EX_NOCLOSE ) )
            {
                send_to_char ( "You can't do that.\n\r", ch );
                return;
            }
            if ( !IS_SET ( obj->value[1], EX_CLOSED ) )
            {
                send_to_char ( "It's not closed.\n\r", ch );
                return;
            }

            if ( obj->value[4] < 0 || IS_SET ( obj->value[1], EX_NOLOCK ) )
            {
                send_to_char ( "It can't be locked.\n\r", ch );
                return;
            }

            if ( !has_key ( ch, obj->value[4] ) )
            {
                send_to_char ( "You lack the key.\n\r", ch );
                return;
            }

            if ( IS_SET ( obj->value[1], EX_LOCKED ) )
            {
                send_to_char ( "It's already locked.\n\r", ch );
                return;
            }

            SET_BIT ( obj->value[1], EX_LOCKED );
            act ( "You lock $p.", ch, obj, NULL, TO_CHAR );
            act ( "$n locks $p.", ch, obj, NULL, TO_ROOM );
            return;
        }

        /* 'lock object' */
        if ( ( obj->item_type != ITEM_CONTAINER ) &&
             ( obj->item_type != ITEM_PIT ) )
        {
            send_to_char ( "That's not a container.\n\r", ch );
            return;
        }
        if ( !IS_SET ( obj->value[1], CONT_CLOSED ) )
        {
            send_to_char ( "It's not closed.\n\r", ch );
            return;
        }
        if ( obj->value[2] < 0 )
        {
            send_to_char ( "It can't be locked.\n\r", ch );
            return;
        }
        if ( !has_key ( ch, obj->value[2] ) )
        {
            send_to_char ( "You lack the key.\n\r", ch );
            return;
        }
        if ( IS_SET ( obj->value[1], CONT_LOCKED ) )
        {
            send_to_char ( "It's already locked.\n\r", ch );
            return;
        }

        SET_BIT ( obj->value[1], CONT_LOCKED );
        act ( "You lock $p.", ch, obj, NULL, TO_CHAR );
        act ( "$n locks $p.", ch, obj, NULL, TO_ROOM );
        return;
    }

    if ( ( door = find_door ( ch, arg ) ) >= 0 )
    {
        /* 'lock door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if ( !IS_SET ( pexit->exit_info, EX_CLOSED ) )
        {
            send_to_char ( "It's not closed.\n\r", ch );
            return;
        }
        if ( pexit->key < 0 )
        {
            send_to_char ( "It can't be locked.\n\r", ch );
            return;
        }
        if ( !has_key ( ch, pexit->key ) )
        {
            send_to_char ( "You lack the key.\n\r", ch );
            return;
        }
        if ( IS_SET ( pexit->exit_info, EX_LOCKED ) )
        {
            send_to_char ( "It's already locked.\n\r", ch );
            return;
        }

        SET_BIT ( pexit->exit_info, EX_LOCKED );
        send_to_char ( "*Click*\n\r", ch );
        act ( "$n locks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

        /* lock the other side */
        if ( ( to_room = pexit->u1.to_room ) != NULL &&
             ( pexit_rev = to_room->exit[rev_dir[door]] ) != 0 &&
             pexit_rev->u1.to_room == ch->in_room )
        {
            SET_BIT ( pexit_rev->exit_info, EX_LOCKED );
        }
    }

    return;
}

CH_CMD ( do_unlock )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int door;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Unlock what?\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_here ( ch, arg ) ) != NULL )
    {
        /* portal stuff */
        if ( obj->item_type == ITEM_PORTAL )
        {
            if ( !IS_SET ( obj->value[1], EX_ISDOOR ) )
            {
                send_to_char ( "You can't do that.\n\r", ch );
                return;
            }

            if ( !IS_SET ( obj->value[1], EX_CLOSED ) )
            {
                send_to_char ( "It's not closed.\n\r", ch );
                return;
            }

            if ( obj->value[4] < 0 )
            {
                send_to_char ( "It can't be unlocked.\n\r", ch );
                return;
            }

            if ( !has_key ( ch, obj->value[4] ) )
            {
                send_to_char ( "You lack the key.\n\r", ch );
                return;
            }

            if ( !IS_SET ( obj->value[1], EX_LOCKED ) )
            {
                send_to_char ( "It's already unlocked.\n\r", ch );
                return;
            }

            REMOVE_BIT ( obj->value[1], EX_LOCKED );
            act ( "You unlock $p.", ch, obj, NULL, TO_CHAR );
            act ( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
            return;
        }

        /* 'unlock object' */
        if ( ( obj->item_type != ITEM_CONTAINER ) &&
             ( obj->item_type != ITEM_PIT ) )
        {
            send_to_char ( "That's not a container.\n\r", ch );
            return;
        }
        if ( !IS_SET ( obj->value[1], CONT_CLOSED ) )
        {
            send_to_char ( "It's not closed.\n\r", ch );
            return;
        }
        if ( obj->value[2] < 0 )
        {
            send_to_char ( "It can't be unlocked.\n\r", ch );
            return;
        }
        if ( !has_key ( ch, obj->value[2] ) )
        {
            send_to_char ( "You lack the key.\n\r", ch );
            return;
        }
        if ( !IS_SET ( obj->value[1], CONT_LOCKED ) )
        {
            send_to_char ( "It's already unlocked.\n\r", ch );
            return;
        }

        REMOVE_BIT ( obj->value[1], CONT_LOCKED );
        act ( "You unlock $p.", ch, obj, NULL, TO_CHAR );
        act ( "$n unlocks $p.", ch, obj, NULL, TO_ROOM );
        return;
    }

    if ( ( door = find_door ( ch, arg ) ) >= 0 )
    {
        /* 'unlock door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if ( !IS_SET ( pexit->exit_info, EX_CLOSED ) )
        {
            send_to_char ( "It's not closed.\n\r", ch );
            return;
        }
        if ( pexit->key < 0 )
        {
            send_to_char ( "It can't be unlocked.\n\r", ch );
            return;
        }
        if ( !has_key ( ch, pexit->key ) )
        {
            send_to_char ( "You lack the key.\n\r", ch );
            return;
        }
        if ( !IS_SET ( pexit->exit_info, EX_LOCKED ) )
        {
            send_to_char ( "It's already unlocked.\n\r", ch );
            return;
        }

        REMOVE_BIT ( pexit->exit_info, EX_LOCKED );
        send_to_char ( "*Click*\n\r", ch );
        act ( "$n unlocks the $d.", ch, NULL, pexit->keyword, TO_ROOM );

        /* unlock the other side */
        if ( ( to_room = pexit->u1.to_room ) != NULL &&
             ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL &&
             pexit_rev->u1.to_room == ch->in_room )
        {
            REMOVE_BIT ( pexit_rev->exit_info, EX_LOCKED );
        }
    }

    return;
}

CH_CMD ( do_pick )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *gch;
    OBJ_DATA *obj;
    int door;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Pick what?\n\r", ch );
        return;
    }

    if ( !IS_IMMORTAL ( ch ) )
        WAIT_STATE ( ch, skill_table[gsn_pick_lock].beats );

    /* look for guards */
    for ( gch = ch->in_room->people; gch; gch = gch->next_in_room )
    {
        if ( IS_NPC ( gch ) && IS_AWAKE ( gch ) && ch->level + 5 < gch->level )
        {
            act ( "$N is standing too close to the lock.", ch, NULL, gch,
                  TO_CHAR );
            return;
        }
    }

    if ( !IS_NPC ( ch ) &&
         number_percent (  ) > get_skill ( ch, gsn_pick_lock ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        check_improve ( ch, gsn_pick_lock, false, 2 );
        return;
    }

    if ( ( obj = get_obj_here ( ch, arg ) ) != NULL )
    {
        /* portal stuff */
        if ( obj->item_type == ITEM_PORTAL )
        {
            if ( !IS_SET ( obj->value[1], EX_ISDOOR ) )
            {
                send_to_char ( "You can't do that.\n\r", ch );
                return;
            }

            if ( !IS_SET ( obj->value[1], EX_CLOSED ) )
            {
                send_to_char ( "It's not closed.\n\r", ch );
                return;
            }

            if ( obj->value[4] < 0 )
            {
                send_to_char ( "It can't be unlocked.\n\r", ch );
                return;
            }

            if ( IS_SET ( obj->value[1], EX_PICKPROOF ) )
            {
                send_to_char ( "You failed.\n\r", ch );
                return;
            }

            REMOVE_BIT ( obj->value[1], EX_LOCKED );
            act ( "You pick the lock on $p.", ch, obj, NULL, TO_CHAR );
            act ( "$n picks the lock on $p.", ch, obj, NULL, TO_ROOM );
            check_improve ( ch, gsn_pick_lock, true, 2 );
            return;
        }

        /* 'pick object' */
        if ( ( obj->item_type != ITEM_CONTAINER ) &&
             ( obj->item_type != ITEM_PIT ) )
        {
            send_to_char ( "That's not a container.\n\r", ch );
            return;
        }
        if ( !IS_SET ( obj->value[1], CONT_CLOSED ) )
        {
            send_to_char ( "It's not closed.\n\r", ch );
            return;
        }
        if ( obj->value[2] < 0 )
        {
            send_to_char ( "It can't be unlocked.\n\r", ch );
            return;
        }
        if ( !IS_SET ( obj->value[1], CONT_LOCKED ) )
        {
            send_to_char ( "It's already unlocked.\n\r", ch );
            return;
        }
        if ( IS_SET ( obj->value[1], CONT_PICKPROOF ) )
        {
            send_to_char ( "You failed.\n\r", ch );
            return;
        }

        REMOVE_BIT ( obj->value[1], CONT_LOCKED );
        act ( "You pick the lock on $p.", ch, obj, NULL, TO_CHAR );
        act ( "$n picks the lock on $p.", ch, obj, NULL, TO_ROOM );
        check_improve ( ch, gsn_pick_lock, true, 2 );
        return;
    }

    if ( ( door = find_door ( ch, arg ) ) >= 0 )
    {
        /* 'pick door' */
        ROOM_INDEX_DATA *to_room;
        EXIT_DATA *pexit;
        EXIT_DATA *pexit_rev;

        pexit = ch->in_room->exit[door];
        if ( !IS_SET ( pexit->exit_info, EX_CLOSED ) && !IS_IMMORTAL ( ch ) )
        {
            send_to_char ( "It's not closed.\n\r", ch );
            return;
        }
        if ( pexit->key < 0 && !IS_IMMORTAL ( ch ) )
        {
            send_to_char ( "It can't be picked.\n\r", ch );
            return;
        }
        if ( !IS_SET ( pexit->exit_info, EX_LOCKED ) )
        {
            send_to_char ( "It's already unlocked.\n\r", ch );
            return;
        }
        if ( IS_SET ( pexit->exit_info, EX_PICKPROOF ) && !IS_IMMORTAL ( ch ) )
        {
            send_to_char ( "You failed.\n\r", ch );
            return;
        }

        REMOVE_BIT ( pexit->exit_info, EX_LOCKED );
        send_to_char ( "*Click*\n\r", ch );
        act ( "$n picks the $d.", ch, NULL, pexit->keyword, TO_ROOM );
        check_improve ( ch, gsn_pick_lock, true, 2 );

        /* pick the other side */
        if ( ( to_room = pexit->u1.to_room ) != NULL &&
             ( pexit_rev = to_room->exit[rev_dir[door]] ) != NULL &&
             pexit_rev->u1.to_room == ch->in_room )
        {
            REMOVE_BIT ( pexit_rev->exit_info, EX_LOCKED );
        }
    }

    return;
}

CH_CMD ( do_stand )
{
    OBJ_DATA *obj = NULL;

    if ( argument[0] != '\0' )
    {
        if ( ch->position == POS_FIGHTING )
        {
            send_to_char ( "Maybe you should finish fighting first?\n\r", ch );
            return;
        }
        obj = get_obj_list ( ch, argument, ch->in_room->contents );
        if ( obj == NULL )
        {
            send_to_char ( "You don't see that here.\n\r", ch );
            return;
        }
        if ( obj->item_type != ITEM_FURNITURE ||
             ( !IS_SET ( obj->value[2], STAND_AT ) &&
               !IS_SET ( obj->value[2], STAND_ON ) &&
               !IS_SET ( obj->value[2], STAND_IN ) ) )
        {
            send_to_char ( "You can't seem to find a place to stand.\n\r", ch );
            return;
        }
        if ( ch->on != obj && count_users ( obj ) >= obj->value[0] )
        {
            act_new ( "There's no room to stand on $p.", ch, obj, NULL, TO_CHAR,
                      POS_DEAD );
            return;
        }
        ch->on = obj;
    }

    switch ( ch->position )
    {
        case POS_SLEEPING:
            if ( IS_AFFECTED ( ch, AFF_SLEEP ) )
            {
                send_to_char ( "You can't wake up!\n\r", ch );
                return;
            }

            if ( obj == NULL )
            {
                send_to_char ( "You wake and stand up.\n\r", ch );
                act ( "$n wakes and stands up.", ch, NULL, NULL, TO_ROOM );
                ch->on = NULL;
            }
            else if ( IS_SET ( obj->value[2], STAND_AT ) )
            {
                act_new ( "You wake and stand at $p.", ch, obj, NULL, TO_CHAR,
                          POS_DEAD );
                act ( "$n wakes and stands at $p.", ch, obj, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], STAND_ON ) )
            {
                act_new ( "You wake and stand on $p.", ch, obj, NULL, TO_CHAR,
                          POS_DEAD );
                act ( "$n wakes and stands on $p.", ch, obj, NULL, TO_ROOM );
            }
            else
            {
                act_new ( "You wake and stand in $p.", ch, obj, NULL, TO_CHAR,
                          POS_DEAD );
                act ( "$n wakes and stands in $p.", ch, obj, NULL, TO_ROOM );
            }
            ch->position = POS_STANDING;
            do_look ( ch, "auto" );
            break;

        case POS_RESTING:
        case POS_SITTING:
            if ( obj == NULL )
            {
                send_to_char ( "You stand up.\n\r", ch );
                act ( "$n stands up.", ch, NULL, NULL, TO_ROOM );
                ch->on = NULL;
            }
            else if ( IS_SET ( obj->value[2], STAND_AT ) )
            {
                act ( "You stand at $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n stands at $p.", ch, obj, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], STAND_ON ) )
            {
                act ( "You stand on $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n stands on $p.", ch, obj, NULL, TO_ROOM );
            }
            else
            {
                act ( "You stand in $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n stands on $p.", ch, obj, NULL, TO_ROOM );
            }
            ch->position = POS_STANDING;
            break;

        case POS_STANDING:
            send_to_char ( "You are already standing.\n\r", ch );
            break;

        case POS_FIGHTING:
            send_to_char ( "You are already fighting!\n\r", ch );
            break;
    }

    return;
}

CH_CMD ( do_rest )
{
    OBJ_DATA *obj = NULL;

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char ( "You are already fighting!\n\r", ch );
        return;
    }

    /* okay, now that we know we can rest, find an object to rest on */
    if ( argument[0] != '\0' )
    {
        obj = get_obj_list ( ch, argument, ch->in_room->contents );
        if ( obj == NULL )
        {
            send_to_char ( "You don't see that here.\n\r", ch );
            return;
        }
    }
    else
        obj = ch->on;

    if ( obj != NULL )
    {
        if ( !IS_SET ( obj->item_type, ITEM_FURNITURE ) ||
             ( !IS_SET ( obj->value[2], REST_ON ) &&
               !IS_SET ( obj->value[2], REST_IN ) &&
               !IS_SET ( obj->value[2], REST_AT ) ) )
        {
            send_to_char ( "You can't rest on that.\n\r", ch );
            return;
        }

        if ( obj != NULL && ch->on != obj &&
             count_users ( obj ) >= obj->value[0] )
        {
            act_new ( "There's no more room on $p.", ch, obj, NULL, TO_CHAR,
                      POS_DEAD );
            return;
        }

        ch->on = obj;
    }

    switch ( ch->position )
    {
        case POS_SLEEPING:

/* MUSKA */
            if ( IS_AFFECTED ( ch, AFF_SLEEP ) )
            {
                send_to_char ( "You can't wake up!\n\r", ch );
                return;
            }

            else if ( obj == NULL )
            {
                send_to_char ( "You wake up and start resting.\n\r", ch );
                act ( "$n wakes up and starts resting.", ch, NULL, NULL,
                      TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], REST_AT ) )
            {
                act_new ( "You wake up and rest at $p.", ch, obj, NULL, TO_CHAR,
                          POS_SLEEPING );
                act ( "$n wakes up and rests at $p.", ch, obj, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], REST_ON ) )
            {
                act_new ( "You wake up and rest on $p.", ch, obj, NULL, TO_CHAR,
                          POS_SLEEPING );
                act ( "$n wakes up and rests on $p.", ch, obj, NULL, TO_ROOM );
            }
            else
            {
                act_new ( "You wake up and rest in $p.", ch, obj, NULL, TO_CHAR,
                          POS_SLEEPING );
                act ( "$n wakes up and rests in $p.", ch, obj, NULL, TO_ROOM );
            }
            ch->position = POS_RESTING;
            break;

        case POS_RESTING:
            send_to_char ( "You are already resting.\n\r", ch );
            break;

        case POS_STANDING:
            if ( obj == NULL )
            {
                send_to_char ( "You rest.\n\r", ch );
                act ( "$n sits down and rests.", ch, NULL, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], REST_AT ) )
            {
                act ( "You sit down at $p and rest.", ch, obj, NULL, TO_CHAR );
                act ( "$n sits down at $p and rests.", ch, obj, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], REST_ON ) )
            {
                act ( "You sit on $p and rest.", ch, obj, NULL, TO_CHAR );
                act ( "$n sits on $p and rests.", ch, obj, NULL, TO_ROOM );
            }
            else
            {
                act ( "You rest in $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n rests in $p.", ch, obj, NULL, TO_ROOM );
            }
            ch->position = POS_RESTING;
            break;

        case POS_SITTING:
            if ( obj == NULL )
            {
                send_to_char ( "You rest.\n\r", ch );
                act ( "$n rests.", ch, NULL, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], REST_AT ) )
            {
                act ( "You rest at $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n rests at $p.", ch, obj, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], REST_ON ) )
            {
                act ( "You rest on $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n rests on $p.", ch, obj, NULL, TO_ROOM );
            }
            else
            {
                act ( "You rest in $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n rests in $p.", ch, obj, NULL, TO_ROOM );
            }
            ch->position = POS_RESTING;
            break;
    }

    return;
}

CH_CMD ( do_sit )
{
    OBJ_DATA *obj = NULL;

    if ( ch->position == POS_FIGHTING )
    {
        send_to_char ( "Maybe you should finish this fight first?\n\r", ch );
        return;
    }

    /* okay, now that we know we can sit, find an object to sit on */
    if ( argument[0] != '\0' )
    {
        obj = get_obj_list ( ch, argument, ch->in_room->contents );
        if ( obj == NULL )
        {
            send_to_char ( "You don't see that here.\n\r", ch );
            return;
        }
    }
    else
        obj = ch->on;

    if ( obj != NULL )
    {
        if ( !IS_SET ( obj->item_type, ITEM_FURNITURE ) ||
             ( !IS_SET ( obj->value[2], SIT_ON ) &&
               !IS_SET ( obj->value[2], SIT_IN ) &&
               !IS_SET ( obj->value[2], SIT_AT ) ) )
        {
            send_to_char ( "You can't sit on that.\n\r", ch );
            return;
        }

        if ( obj != NULL && ch->on != obj &&
             count_users ( obj ) >= obj->value[0] )
        {
            act_new ( "There's no more room on $p.", ch, obj, NULL, TO_CHAR,
                      POS_DEAD );
            return;
        }

        ch->on = obj;
    }
    switch ( ch->position )
    {
        case POS_SLEEPING:
/* MUSKA */
            if ( IS_AFFECTED ( ch, AFF_SLEEP ) )
            {
                send_to_char ( "You can't wake up!\n\r", ch );
                return;
            }

            if ( obj == NULL )
            {
                send_to_char ( "You wake and sit up.\n\r", ch );
                act ( "$n wakes and sits up.", ch, NULL, NULL, TO_ROOM );
            }

            else if ( IS_SET ( obj->value[2], SIT_AT ) )
            {
                act_new ( "You wake and sit at $p.", ch, obj, NULL, TO_CHAR,
                          POS_DEAD );
                act ( "$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], SIT_ON ) )
            {
                act_new ( "You wake and sit on $p.", ch, obj, NULL, TO_CHAR,
                          POS_DEAD );
                act ( "$n wakes and sits at $p.", ch, obj, NULL, TO_ROOM );
            }
            else
            {
                act_new ( "You wake and sit in $p.", ch, obj, NULL, TO_CHAR,
                          POS_DEAD );
                act ( "$n wakes and sits in $p.", ch, obj, NULL, TO_ROOM );
            }

            ch->position = POS_SITTING;
            break;
        case POS_RESTING:
            if ( obj == NULL )
                send_to_char ( "You stop resting.\n\r", ch );
            else if ( IS_SET ( obj->value[2], SIT_AT ) )
            {
                act ( "You sit at $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n sits at $p.", ch, obj, NULL, TO_ROOM );
            }

            else if ( IS_SET ( obj->value[2], SIT_ON ) )
            {
                act ( "You sit on $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n sits on $p.", ch, obj, NULL, TO_ROOM );
            }
            ch->position = POS_SITTING;
            break;
        case POS_SITTING:
            send_to_char ( "You are already sitting down.\n\r", ch );
            break;
        case POS_STANDING:
            if ( obj == NULL )
            {
                send_to_char ( "You sit down.\n\r", ch );
                act ( "$n sits down on the ground.", ch, NULL, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], SIT_AT ) )
            {
                act ( "You sit down at $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n sits down at $p.", ch, obj, NULL, TO_ROOM );
            }
            else if ( IS_SET ( obj->value[2], SIT_ON ) )
            {
                act ( "You sit on $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n sits on $p.", ch, obj, NULL, TO_ROOM );
            }
            else
            {
                act ( "You sit down in $p.", ch, obj, NULL, TO_CHAR );
                act ( "$n sits down in $p.", ch, obj, NULL, TO_ROOM );
            }
            ch->position = POS_SITTING;
            break;
    }
    return;
}

CH_CMD ( do_sleep )
{
    OBJ_DATA *obj = NULL;

    switch ( ch->position )
    {
        case POS_SLEEPING:
            send_to_char ( "You are already sleeping.\n\r", ch );
            break;

        case POS_RESTING:
        case POS_SITTING:
        case POS_STANDING:
            if ( argument[0] == '\0' && ch->on == NULL )
            {
                send_to_char ( "You go to sleep.\n\r", ch );
                act ( "$n goes to sleep.", ch, NULL, NULL, TO_ROOM );
                ch->position = POS_SLEEPING;
            }
            else                /* find an object and sleep on it */
            {
                if ( argument[0] == '\0' )
                    obj = ch->on;
                else
                    obj = get_obj_list ( ch, argument, ch->in_room->contents );

                if ( obj == NULL )
                {
                    send_to_char ( "You don't see that here.\n\r", ch );
                    return;
                }
                if ( obj->item_type != ITEM_FURNITURE ||
                     ( !IS_SET ( obj->value[2], SLEEP_ON ) &&
                       !IS_SET ( obj->value[2], SLEEP_IN ) &&
                       !IS_SET ( obj->value[2], SLEEP_AT ) ) )
                {
                    send_to_char ( "You can't sleep on that!\n\r", ch );
                    return;
                }

                if ( ch->on != obj && count_users ( obj ) >= obj->value[0] )
                {
                    act_new ( "There is no room on $p for you.", ch, obj, NULL,
                              TO_CHAR, POS_DEAD );
                    return;
                }

                ch->on = obj;
                if ( IS_SET ( obj->value[2], SLEEP_AT ) )
                {
                    act ( "You go to sleep at $p.", ch, obj, NULL, TO_CHAR );
                    act ( "$n goes to sleep at $p.", ch, obj, NULL, TO_ROOM );
                }
                else if ( IS_SET ( obj->value[2], SLEEP_ON ) )
                {
                    act ( "You go to sleep on $p.", ch, obj, NULL, TO_CHAR );
                    act ( "$n goes to sleep on $p.", ch, obj, NULL, TO_ROOM );
                }
                else
                {
                    act ( "You go to sleep in $p.", ch, obj, NULL, TO_CHAR );
                    act ( "$n goes to sleep in $p.", ch, obj, NULL, TO_ROOM );
                }
                ch->position = POS_SLEEPING;
            }
            break;

        case POS_FIGHTING:
            send_to_char ( "You are already fighting!\n\r", ch );
            break;
    }

    return;
}

CH_CMD ( do_wake )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );
    if ( arg[0] == '\0' )
    {
        do_stand ( ch, argument );
        return;
    }

    if ( !IS_AWAKE ( ch ) )
    {
        send_to_char ( "You are asleep yourself!\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_AWAKE ( victim ) )
    {
        act ( "$N is already awake.", ch, NULL, victim, TO_CHAR );
        return;
    }

    /* if ( IS_AFFECTED(victim, AFF_SLEEP) ) { act( "You can't wake $M!", ch,
       NULL, victim, TO_CHAR ); return; } */
    act_new ( "$n wakes you.", ch, NULL, victim, TO_VICT, POS_SLEEPING );
    do_stand ( victim, "" );
    return;
}

CH_CMD ( do_sneak )
{
    AFFECT_DATA af;

    if ( IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) )
    {
        send_to_char ( "Not while in the arena you dont.\n\r", ch );
        return;
    }

    send_to_char ( "You attempt to move silently.\n\r", ch );
    affect_strip ( ch, gsn_sneak );

    if ( IS_AFFECTED ( ch, AFF_SNEAK ) )
        return;

    if ( number_percent (  ) < get_skill ( ch, gsn_sneak ) )
    {
        check_improve ( ch, gsn_sneak, true, 3 );
        af.where = TO_AFFECTS;
        af.type = gsn_sneak;
        af.level = ch->level;
        af.duration = ch->level;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_SNEAK;
        affect_to_char ( ch, &af );
    }
    else
        check_improve ( ch, gsn_sneak, false, 3 );

    return;
}

CH_CMD ( do_swalk )
{
    AFFECT_DATA af;

    if ( nia ( ch ) )
        return;

    send_to_char ( "You attempt to blend into the shadows.\n\r", ch );
    affect_strip ( ch, gsn_swalk );

    if ( IS_SHIELDED ( ch, SHD_SWALK ) )
    {
        send_to_char ( "You are already there!\n\r", ch );
        return;
    }

    if ( ( number_percent (  ) < get_skill ( ch, gsn_swalk ) ) &&
         ( ( ( ch->hit * 100 ) / ch->max_hit ) >= 90 ) )
    {
        check_improve ( ch, gsn_swalk, true, 3 );
        af.where = TO_SHIELDS;
        af.type = gsn_swalk;
        af.level = ch->level;
        af.duration = ch->level / 20;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = SHD_SWALK;
        affect_to_char ( ch, &af );
    }
    else
    {
        if ( ( ( ch->hit * 100 ) / ch->max_hit ) <= 90 )
            printf_to_char ( ch,
                             "You attempt to melt into the shadows but your injuries give you away." );
        else
            printf_to_char ( ch,
                             "You attempt to meld into the shadows but fail." );

        check_improve ( ch, gsn_swalk, false, 3 );
    }

    return;
}

CH_CMD ( do_hide )
{
    AFFECT_DATA af;

    if ( IS_AFFECTED ( ch, AFF_HIDE ) )
    {
        REMOVE_BIT ( ch->affected_by, AFF_HIDE );
        send_to_char ( "You were allready hidden!\n\r", ch );
        return;
    }

    if ( number_percent (  ) < get_skill ( ch, gsn_hide ) )
    {
        affect_strip ( ch, gsn_hide );
        send_to_char ( "You are now hidden!\n\r", ch );
        af.where = TO_AFFECTS;
        af.type = gsn_hide;
        af.level = ch->level;
        af.duration = ch->level / 5;
        af.location = APPLY_NONE;
        af.modifier = 0;
        af.bitvector = AFF_HIDE;
        affect_to_char ( ch, &af );
        check_improve ( ch, gsn_hide, true, 3 );
    }
    else
        check_improve ( ch, gsn_hide, false, 3 );

    return;
}

/*
 * Contributed by Alander.
 */
CH_CMD ( do_visible )
{
    affect_strip ( ch, gsn_invis );
    affect_strip ( ch, gsn_vanish );
    affect_strip ( ch, gsn_mass_invis );
    affect_strip ( ch, gsn_sneak );
    kill_swalk ( ch );
    REMOVE_BIT ( ch->affected_by, AFF_HIDE );
    REMOVE_BIT ( ch->shielded_by, SHD_INVISIBLE );
    REMOVE_BIT ( ch->shielded_by, SHD_SWALK );
    REMOVE_BIT ( ch->shielded_by, SHD_VANISH );
    REMOVE_BIT ( ch->affected_by, AFF_SNEAK );
    send_to_char ( "Ok.\n\r", ch );
    return;
}

CH_CMD ( do_recall )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    int track;
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *och;
    CHAR_DATA *och_next;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( IS_NPC ( ch ) && !IS_SET ( ch->act, ACT_PET ) )
    {
        send_to_char ( "Only players can recall.\n\r", ch );
        return;
    }

    WAIT_STATE ( ch, 8 );

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    if ( !str_cmp ( "here", arg1 ) )
    {
        if ( IS_SET ( ch->in_room->room_flags, ROOM_RECALL_HERE ) ||
             IS_IMMORTAL ( ch ) )
        {
            send_to_char ( "Your recall has been set.\n\r", ch );
            ch->recall_room = ch->in_room->vnum;
        }
        else
        {
            send_to_char ( "You can not recall to this spot.\n\r", ch );
            return;
        }
        return;
    }

    act ( "$n prays for transportation!", ch, 0, 0, TO_ROOM );

    if ( ( location = get_room_index ( ROOM_VNUM_TEMPLE ) ) == NULL )
    {
        send_to_char ( "You are completely lost.\n\r", ch );
        return;
    }

    if ( ch->in_room == location )
        return;

    if ( ( IS_SET ( ch->in_room->room_flags, ROOM_NO_RECALL ) ||
           IS_AFFECTED ( ch, AFF_CURSE ) ) && ( ch->level <= ANCIENT ) )
    {
        act ( "$G has forsaken you.", ch, NULL, NULL, TO_CHAR );
        return;
    }

    if ( ( victim = ch->fighting ) != NULL )
    {
        int lose, skill;

        skill = get_skill ( ch, gsn_recall );

        if ( number_percent (  ) < 80 * skill / 100 )
        {
            check_improve ( ch, gsn_recall, false, 6 );
            if ( !IS_IMMORTAL ( ch ) )
                WAIT_STATE ( ch, 4 );

            sprintf ( buf, "You failed!.\n\r" );
            send_to_char ( buf, ch );
            return;
        }

        lose = ( ch->desc != NULL ) ? 25 : 50;
        gain_exp ( ch, 0 - lose );
        check_improve ( ch, gsn_recall, true, 4 );
        sprintf ( buf, "You recall from combat!  You lose %d exps.\n\r", lose );
        send_to_char ( buf, ch );
        if ( !IS_NPC ( ch ) )
        {
            if ( !IS_NPC ( victim ) && ch->attacker == true )
            {
                send_to_char
                    ( "The {RWrath of Samoth {YZAPS{x your butt on the way out!\n\r",
                      ch );
                act ( "$n is {Yzapped{x in the butt by a lightning bolt from above!", ch, NULL, NULL, TO_ROOM );
                ch->hit -= ( ch->hit / 4 );
            }
        }
        stop_fighting ( ch, true );
    }

    if ( is_clan ( ch ) && ( clan_table[ch->clan].hall != ROOM_VNUM_ALTAR ) &&
         !IS_SET ( ch->act, PLR_TWIT ) )
        location = get_room_index ( clan_table[ch->clan].hall );

    if ( IS_NPC ( ch ) && IS_SET ( ch->act, ACT_PET ) && is_clan ( ch->master )
         && ( clan_table[ch->master->clan].hall != ROOM_VNUM_ALTAR ) &&
         !IS_SET ( ch->master->act, PLR_TWIT ) )
        location = get_room_index ( clan_table[ch->master->clan].hall );

    ch->move *= .75;
    act ( "$n disappears.", ch, NULL, NULL, TO_ROOM );
    if ( IS_NPC ( ch ) || !IS_IMMORTAL ( ch ) )
    {
        for ( track = MAX_TRACK - 1; track > 0; track-- )
        {
            ch->track_to[track] = ch->track_to[track - 1];
            ch->track_from[track] = ch->track_from[track - 1];
        }
        ch->track_from[0] = ch->in_room->vnum;
        ch->track_to[0] = 0;
    }

    if ( !str_cmp ( arg1, "altar" ) && !IS_SET ( ch->act, PLR_TWIT ) )
    {
        send_to_char ( "\n\rYou recall to the temple altar.\n\r\n\r", ch );
        location = get_room_index ( ROOM_VNUM_ALTAR );
    }
    else if ( !str_cmp ( arg1, "clan" ) )
    {
        if ( !is_clan ( ch ) )
        {
            printf_to_char ( ch, "You are not even in a clan!" );
            return;
        }
        else if ( is_clan ( ch ) &&
                  ( clan_table[ch->clan].hall != ROOM_VNUM_ALTAR ) &&
                  !IS_SET ( ch->act, PLR_TWIT ) )
            location = get_room_index ( clan_table[ch->clan].hall );
    }

    if ( ch->recall_room != 0 )
    {
        location = get_room_index ( ch->recall_room );
    }
    else
    {
        ch->recall_room = 0;
    }

    for ( och = ch->in_room->people; och != NULL; och = och_next )
    {
        och_next = och->next_in_room;
        if ( IS_AFFECTED ( och, AFF_CHARM ) && och->master == ch )
        {

            char_from_room ( och );
            char_to_room ( och, location );
        }
    }

    affect_strip ( ch, gsn_entangle );

        if ( ch->pet != NULL )
        if ( ch->pet->in_room == ch->in_room )
        {
            char_from_room ( ch->pet );
            char_to_room ( ch->pet, location );
        }
            
        if ( ch->familiar != NULL )
        if ( ch->familiar->in_room == ch->in_room )
        {
            char_from_room ( ch->familiar );
            char_to_room ( ch->familiar, location );
        } 
    char_from_room ( ch );
    char_to_room ( ch, location );
    act ( "$n appears in the room.", ch, NULL, NULL, TO_ROOM );
    do_look ( ch, "auto" );
    return;
}

CH_CMD ( do_train )
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *mob;
    sh_int stat = -1;
    char *pOutput = NULL;
    int cost;
    int aloop;
    int train_loop;
    int train_amount;
    bool do_train_all;
    bool train_is_num;

    do_train_all = false;
    train_is_num = false;
    train_loop = 0;
    train_amount = 0;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( IS_NPC ( ch ) )
        return;

    if ( !str_cmp ( arg2, "all" ) )
    {
        do_train_all = true;
        train_is_num = false;
    }

    if ( is_number ( arg2 ) )
    {
        train_is_num = true;
        do_train_all = true;
        train_amount = ( atoi ( arg2 ) );
        if ( train_amount < 1 )
        {
            send_to_char ( "What? Train it less than 1 time!?!?\n\r", ch );
            return;
        }
    }

    /* 
     * Check for trainer.
     */
    for ( mob = ch->in_room->people; mob; mob = mob->next_in_room )
    {
        if ( IS_NPC ( mob ) && IS_SET ( mob->act, ACT_TRAIN ) )
            break;
    }

    if ( mob == NULL )
    {
        send_to_char ( "You can't do that here.\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0' )
    {
        sprintf ( buf, "You have %d training sessions.\n\r", ch->train );
        send_to_char ( buf, ch );
        argument = "foo";
    }

    cost = 1;

    if ( !str_cmp ( arg1, "str" ) )
    {
        if ( class_table[ch->class].attr_prime == STAT_STR )
            cost = 1;
        stat = STAT_STR;
        pOutput = "strength";
    }

    else if ( !str_cmp ( arg1, "int" ) )
    {
        if ( class_table[ch->class].attr_prime == STAT_INT )
            cost = 1;
        stat = STAT_INT;
        pOutput = "intelligence";
    }

    else if ( !str_cmp ( arg1, "wis" ) )
    {
        if ( class_table[ch->class].attr_prime == STAT_WIS )
            cost = 1;
        stat = STAT_WIS;
        pOutput = "wisdom";
    }

    else if ( !str_cmp ( arg1, "dex" ) )
    {
        if ( class_table[ch->class].attr_prime == STAT_DEX )
            cost = 1;
        stat = STAT_DEX;
        pOutput = "dexterity";
    }

    else if ( !str_cmp ( arg1, "con" ) )
    {
        if ( class_table[ch->class].attr_prime == STAT_CON )
            cost = 1;
        stat = STAT_CON;
        pOutput = "constitution";
    }

    else if ( !str_cmp ( arg1, "hp" ) )
        cost = 1;

    else if ( !str_cmp ( arg1, "mana" ) )
        cost = 1;

    else if ( !str_cmp ( arg1, "move" ) )
        cost = 1;

    else
    {
        strcpy ( buf, "You can train:" );
        if ( ch->perm_stat[STAT_STR] < get_max_train ( ch, STAT_STR ) )
            strcat ( buf, " str" );
        if ( ch->perm_stat[STAT_INT] < get_max_train ( ch, STAT_INT ) )
            strcat ( buf, " int" );
        if ( ch->perm_stat[STAT_WIS] < get_max_train ( ch, STAT_WIS ) )
            strcat ( buf, " wis" );
        if ( ch->perm_stat[STAT_DEX] < get_max_train ( ch, STAT_DEX ) )
            strcat ( buf, " dex" );
        if ( ch->perm_stat[STAT_CON] < get_max_train ( ch, STAT_CON ) )
            strcat ( buf, " con" );
        strcat ( buf, " hp mana move" );
        strcat ( buf,
                 "\n\r\n\rSyntax: train < hp / mana / move > < number or ALL > \n\r" );
        if ( buf[strlen ( buf ) - 1] != ':' )
        {
            send_to_char ( buf, ch );
        }
        else
        {
            /* 
             * This message dedicated to Jordan ... you big stud!
             */
            act ( "You have nothing left to train, you $T!", ch, NULL,
                  ch->sex == SEX_MALE ? "big stud" : ch->sex ==
                  SEX_FEMALE ? "hot babe" : "wild thing", TO_CHAR );
        }

        return;
    }

/* hpargs */

    if ( !str_cmp ( "hp", arg1 ) )
    {

        if ( cost > ch->train )
        {
            send_to_char ( "You don't have enough training sessions.\n\r", ch );
            return;
        }

        aloop = 0;
        train_loop = train_amount;

        if ( do_train_all == true )
        {
            while ( cost <= ch->train )
            {
                if ( train_loop < 1 && train_is_num )
                {
                    sprintf ( buf, "You gain %d health points!\n\r",
                              ( aloop * 5 ) );
                    send_to_char ( buf, ch );
                    return;
                }
                train_loop--;
                aloop++;
                ch->train -= cost;
                ch->pcdata->perm_hit += 5;
                ch->max_hit += 5;
                ch->hit += 5;

            }
            sprintf ( buf, "You gain %d health points!\n\r", ( aloop * 5 ) );
            send_to_char ( buf, ch );
            return;
        }
        else
        {
            ch->train -= cost;
            ch->pcdata->perm_hit += 5;
            ch->max_hit += 5;
            ch->hit += 5;
            act ( "You gain 5 health points", ch, NULL, NULL, TO_CHAR );
            return;
        }
    }

    if ( !str_cmp ( "mana", arg1 ) )
    {
        if ( cost > ch->train )
        {
            send_to_char ( "You don't have enough training sessions.\n\r", ch );
            return;
        }

        aloop = 0;
        train_loop = train_amount;

        if ( do_train_all == true )
        {
            while ( cost <= ch->train )
            {
                if ( train_loop < 1 && train_is_num )
                {
                    sprintf ( buf, "You gain %d mana points!\n\r",
                              ( aloop * 10 ) );
                    send_to_char ( buf, ch );
                    return;
                }
                train_loop--;
                aloop++;
                ch->train -= cost;
                ch->pcdata->perm_mana += 10;
                ch->max_mana += 10;
                ch->mana += 10;
            }
            sprintf ( buf, "You gain %d mana points!\n\r", ( aloop * 10 ) );
            send_to_char ( buf, ch );
            return;

        }
        else
        {
            ch->train -= cost;
            ch->pcdata->perm_mana += 10;
            ch->max_mana += 10;
            ch->mana += 10;
            act ( "Your gain 10 mana points!", ch, NULL, NULL, TO_CHAR );
            return;
        }
    }

    if ( !str_cmp ( "move", arg1 ) )
    {
        if ( cost > ch->train )
        {
            send_to_char ( "You don't have enough training sessions.\n\r", ch );
            return;
        }

        aloop = 0;
        train_loop = train_amount;

        if ( do_train_all == true )
        {
            while ( cost <= ch->train )
            {
                if ( train_loop < 1 && train_is_num )
                {
                    sprintf ( buf, "You gain %d move points!\n\r",
                              ( aloop * 20 ) );
                    send_to_char ( buf, ch );
                    return;
                }
                train_loop--;
                aloop++;
                ch->train -= cost;
                ch->pcdata->perm_move += 20;
                ch->max_move += 20;
                ch->move += 20;
            }
            sprintf ( buf, "You gain %d move points!\n\r", ( aloop * 20 ) );
            send_to_char ( buf, ch );
            return;

        }
        else
        {
            ch->train -= cost;
            ch->pcdata->perm_move += 20;
            ch->max_move += 20;
            ch->move += 20;

            act ( "Your gain 20 move points!", ch, NULL, NULL, TO_CHAR );
            return;
        }
    }

    if ( ch->perm_stat[stat] >= get_max_train ( ch, stat ) )
    {
        act ( "Your $T is already at maximum.", ch, NULL, pOutput, TO_CHAR );
        return;
    }

    if ( cost > ch->train )
    {
        send_to_char ( "You don't have enough training sessions.\n\r", ch );
        return;
    }

    ch->train -= cost;

    ch->perm_stat[stat] += 1;
    act ( "Your $T increases!", ch, NULL, pOutput, TO_CHAR );
    return;
}
