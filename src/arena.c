#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "merc.h"

void do_start_massarena ( void )
{
    int char_room;
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;

    if ( !is_mass_arena_fight )
    {
        do_gmessage
            ( "{W[{RARENA{W] The mass arena is now active! Let the games begin!{x\n\r" );
        is_mass_arena_fight = true;
        for ( ch = char_list; ch != NULL; ch = ch_next )
        {
            ch_next = ch->next;
            if ( IS_SET ( ch->act2, PLR2_MASS_ARENA ) &&
                 IS_SET ( ch->act2, PLR2_MASS_JOINER ) )
            {
                char_room = number_range ( 50000, 50013 );
                char_from_room ( ch );
                char_to_room ( ch, get_room_index ( char_room ) );
                do_look ( ch, "auto" );
                do_visible ( ch, NULL );

                affect_strip ( ch, gsn_plague );
                affect_strip ( ch, gsn_poison );
                affect_strip ( ch, gsn_blindness );
                affect_strip ( ch, gsn_sleep );
                affect_strip ( ch, gsn_curse );

                ch->hit = ch->max_hit;
                ch->mana = ch->max_mana;
                ch->move = ch->max_move;
                update_pos ( ch );

            }
        }
    }
}

CH_CMD ( do_massarena )
{
    if ( IS_NPC ( ch ) || ch->desc == NULL )
        return;

    if ( copyover_countdown > 0 && copyover_countdown <= 5 )
    {
        send_to_char
            ( "Sorry, the mud is getting ready for a copyover.\n\rPlease try again after the copyover.\n\r",
              ch );
        return;
    }
    if ( IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) )
    {
        send_to_char ( "You are already in the arena.\n\r", ch );
        return;
    }
    if ( is_mass_arena_fight )
    {
        send_to_char ( "Sorry, its too late to join now.\n\r", ch );
        return;
    }
    if ( !str_cmp ( argument, "join" ) )
    {
        char buf[MSL];

        if ( IS_NPC ( ch ) )
            return;

        if ( !is_mass_arena )
        {
            send_to_char
                ( "There is not a mass arena to join at this time.\n\r", ch );
            return;
        }
        if ( is_mass_arena_fight )
        {
            send_to_char ( "It is too late to join now.\n\r", ch );
            return;
        }
        if ( !IS_SET ( ch->act2, PLR2_MASS_ARENA ) )
        {
            send_to_char
                ( "You must first turn on your mass arena flag.\n\rPlease type massarena to do so.\n\r",
                  ch );
            return;
        }
        if ( IS_SET ( ch->act2, PLR2_MASS_JOINER ) )
        {
            send_to_char ( "You have already joined the mass arena.\n\r", ch );
            return;
        }
        if ( !IS_SET ( ch->act2, PLR2_MASS_JOINER ) && is_mass_arena &&
             !is_mass_arena_fight )
        {
            sprintf ( buf,
                      "{W[{RARENA{W] {B%s{W has joined the mass arena!{x\n\r",
                      ch->name );
            do_gmessage ( buf );
            SET_BIT ( ch->act2, PLR2_MASS_JOINER );
            return;
        }
    }
    if ( IS_SET ( ch->act2, PLR2_MASS_ARENA ) &&
         !IS_SET ( ch->act2, PLR2_MASS_JOINER ) && !is_mass_arena_fight )
    {
        send_to_char ( "Your mass arena flag is now {ROFF{x.\n\r", ch );
        REMOVE_BIT ( ch->act2, PLR2_MASS_ARENA );
        return;
    }
    if ( !IS_SET ( ch->act2, PLR2_MASS_ARENA ) &&
         !IS_SET ( ch->act2, PLR2_MASS_JOINER ) && !is_mass_arena_fight )
    {
        send_to_char ( "Your mass arena flag is now {GON{x.\n\r", ch );
        SET_BIT ( ch->act2, PLR2_MASS_ARENA );
        return;
    }
}

/*=======================================================================*
 * function: do_challenge                                                *
 * purpose: sends initial arena match query                              *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96                *
 *=======================================================================*/

CH_CMD ( do_challenge )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    char arg1[MIL];
    char arg2[MIL];

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );
/* == First make all invalid checks == */
    if ( IS_NPC ( ch ) )
        return;

    if ( copyover_countdown > 0 && copyover_countdown <= 5 )
    {
        send_to_char
            ( "Sorry, the mud is getting ready for a copyover.\n\rPlease try again after the copyover.\n\r",
              ch );
        return;
    }
    if ( is_mass_arena )
    {
        send_to_char ( "There is currently a mass arena taking place.\n\r",
                       ch );
        return;
    }
    if ( ch->challenge_timer != 0 )
    {
        sprintf ( buf,
                  "You must wait %d more minutes before you can challange someone to the arena.\n\r",
                  ch->challenge_timer );
        send_to_char ( buf, ch );
        return;
    }
    arenacount = 0;
    if ( arena == FIGHT_START )
    {
        send_to_char
            ( "Sorry, some one else has already started a challenge, please try later.\n\r",
              ch );
        return;
    }
    if ( arena == FIGHT_BUSY )
    {
        send_to_char
            ( "Sorry, there is a fight in progress, please wait a few moments.\n\r",
              ch );
        return;
    }
    if ( arena == FIGHT_LOCK )
    {
        send_to_char ( "Sorry, the arena is currently locked from use.\n\r",
                       ch );
        return;
    }

    if ( IS_SET ( ch->comm, COMM_AFK ) )
    {
        send_to_char ( "I'm sorry but they are AFK.\n\r", ch );
        return;
    }

    if ( IS_SET ( ch->act2, PLR2_CHALLENGED ) )
    {
        send_to_char
            ( "You have already been challenged, either ACCEPT or DECLINE first.\n\r",
              ch );
        return;
    }
/*    if ( ch->hit < ch->max_hit )
    {
        send_to_char ( "You must be fully healed to fight in the arena.\n\r",
                       ch );
        return;
    } */
    if ( arg1[0] == '\0' )
    {
        send_to_char ( "You must specify whom you wish to challenge.\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They are not playing.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) || IS_IMMORTAL ( victim ) || victim == ch ||
         IS_IMMORTAL ( ch ) )
    {
        send_to_char
            ( "You may not challenge yourself, npcs, or immortals.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act2, PLR2_CHALLENGER ) )
    {
        send_to_char ( "They have already challenged someone else.\n\r", ch );
        return;
    }
    if ( victim->fighting != NULL )
    {
        send_to_char ( "That person is engaged in battle right now.\n\r", ch );
        return;
    }
    if ( victim->pk_timer != 0 )
    {
        send_to_char ( "Sorry, they have a PK timer still.\n\r", ch );
        return;
    }
    if ( ch->pk_timer != 0 )
    {
        send_to_char ( "You have a PK timer still.\n\r", ch );
        return;
    }
/*    if ( victim->hit < victim->max_hit )
    {
        send_to_char
            ( "That player is not healthy enough to fight right now.\n\r", ch );
        return;
    } */
    if ( victim->desc == NULL )
    {
        send_to_char
            ( "That player is linkdead at the moment, try them later.\n\r",
              ch );
        return;
    }

/* == Now for the challenge == */
    ch->challenged = victim;
    if ( victim->name == ch->name )
    {
        send_to_char ( "You can not challenge yourself.\n\r", ch );
        return;
    }
    if ( !str_cmp ( arg2, "spar" ) )
    spar = true;
    else
    spar = false;

    ch->challenge_timer = 5;
    SET_BIT ( ch->act2, PLR2_CHALLENGER );
    victim->challenger = ch;
    SET_BIT ( victim->act2, PLR2_CHALLENGED );
    arena = FIGHT_START;
    do_visible ( ch, NULL );

    if ( !spar )
    {
    send_to_char ( "Challenge has been sent\n\r", ch );
    act ( "$n has challenged you to a death match.", ch, NULL, victim,
          TO_VICT );
    sprintf ( buf,
              "{W[{RARENA{W] {R%s{W has challenged {R%s{W to a match in the arena.{x\n\r",
              ch->name, victim->name );
    }
    else
    {
    send_to_char ( "Sparing challenge has been sent\n\r", ch );
    act ( "$n has challenged you to a spar.", ch, NULL, victim,
          TO_VICT );
    sprintf ( buf,
              "{W[{RSPAR{W] {R%s{W has challenged {R%s{W to a spar in the arena.{x\n\r",
              ch->name, victim->name );
    }
    do_gmessage ( buf );
    do_visible ( ch, NULL );
    sprintf ( buf, "{Wtype: {RACCEPT %s{W to meet the challenge.{x\n\r",
              ch->name );
    send_to_char ( buf, victim );
    sprintf ( buf, "{Wtype: {RDECLINE %s{W to chicken out.{x\n\r", ch->name );
    send_to_char ( buf, victim );
    return;
}

/*=======================================================================*
 * function: do_accept                                                   *
 * purpose: to accept the arena match, and move the players to the arena *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96                *
 *=======================================================================*/
CH_CMD ( do_agree )
{
    float odd1, odd2;
    float lvl1, lvl2;
    CHAR_DATA *victim;

    int char_room;
    int vict_room;

 /*== the room VNUM's for our arena.are ==*/
    /* we use 1051 thru 1066 for a 4x4 arena */


    if ( !spar )
    {
        char_room = number_range ( 50000, 50013 );
        vict_room = number_range ( 50000, 50013 );
    }
    else
    {
        char_room = ROOM_VNUM_SPAR;
        vict_room = ROOM_VNUM_SPAR;
    }

/* == first make all invalid checks == */
    if ( IS_NPC ( ch ) )
        return;
    if ( !IS_SET ( ch->act2, PLR2_CHALLENGED ) )
    {
        send_to_char ( "You have not been challenged.\n\r", ch );
        return;
    }
    if ( arena == FIGHT_BUSY )
    {
        send_to_char
            ( "Sorry, there is a fight in progress, please wait a few moments.\n\r",
              ch );
        return;
    }
    if ( arena == FIGHT_LOCK )
    {
        send_to_char ( "Sorry, the arena is currently locked from use.\n\r",
                       ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char
            ( "You must specify whose challenge you wish to accept.\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They aren't logged in!\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char ( "You haven't challenged yourself!\n\r", ch );
        return;
    }
    if ( !IS_SET ( victim->act2, PLR2_CHALLENGER ) || victim != ch->challenger )
    {
        send_to_char ( "That player hasn't challenged you!\n\r", ch );
        return;
    }

    do_visible ( ch, NULL );
    do_visible ( victim, NULL );

/* == now get to business == */
    send_to_char ( "You have accepted the challenge!\n\r", ch );
    act ( "$n accepts your challenge!", ch, NULL, victim, TO_VICT );
    arenacount = 0;

    ch->hit = ch->max_hit;
    ch->move = ch->max_move;
    ch->mana = ch->max_mana;

    victim->hit = victim->max_hit;
    victim->move = victim->max_move;
    victim->mana = victim->max_mana;

/* == announce the upcoming event == */
    lvl1 = ch->hit;
    lvl2 = victim->hit;
    odd1 = ( lvl1 / lvl2 );
    odd2 = ( lvl2 / lvl1 );
/*
    sprintf ( buf1,
              "{W[{RARENA{W] {R%s{W: Wins: {R%d{W Losses: {R%d{W Odds: {R%.2f{x\n\r",
              victim->name, victim->pcdata->awins, victim->pcdata->alosses,
              odd1 );
    sprintf ( buf2,
              "{W[{RARENA{W] {R%s{W: Wins: {R%d{W Losses: {R%d{W Odds: {R%.2f{x\n\r",
              ch->name, ch->pcdata->awins, ch->pcdata->alosses, odd2 );
    strcpy ( buf3,
             "{W[{RARENA{W] To place a bet type: {RBET{W <{Rplatinum{W> <{Rplayer{W>{x\n\r" );

    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            send_to_char ( buf1, d->character );
            send_to_char ( buf2, d->character );
//            send_to_char ( buf3, d->character );
            d->character->gladiator = NULL;
        }
    } */
/* == now move them both to an arena for the fun == */
    send_to_char ( "You make your way into the arena.\n\r", ch );
    char_from_room ( ch );
    char_to_room ( ch, get_room_index ( char_room ) );
    do_look ( ch, "auto" );
    send_to_char ( "You make your way to the arena.\n\r", victim );
    char_from_room ( victim );
    char_to_room ( victim, get_room_index ( vict_room ) );
    do_look ( victim, "auto" );
    arena = FIGHT_BUSY;
    return;
}

/*=======================================================================*
 * function: do_decline                                                  *
 * purpose: to chicken out from a sent arena challenge                   *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96                *
 *=======================================================================*/
CH_CMD ( do_decline )
{
    CHAR_DATA *victim;
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

/*== make all invalid checks == */
    if ( IS_NPC ( ch ) )
        return;
    if ( !IS_SET ( ch->act2, PLR2_CHALLENGED ) )
    {
        send_to_char ( "You have not been challenged.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char
            ( "You must specify whose challenge you wish to decline.\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, argument ) ) == NULL )
    {
        send_to_char ( "They aren't logged in!\n\r", ch );
        return;
    }
    if ( !IS_SET ( victim->act2, PLR2_CHALLENGER ) || victim != ch->challenger )
    {
        send_to_char ( "That player hasn't challenged you.\n\r", ch );
        return;
    }
    if ( victim == ch )
        return;
/*== now actually decline == */
    victim->challenged = NULL;
    REMOVE_BIT ( victim->act2, PLR2_CHALLENGER );
    ch->challenger = NULL;
    REMOVE_BIT ( ch->act2, PLR2_CHALLENGED );
    arena = FIGHT_OPEN;
    send_to_char ( "Challenge declined!\n\r", ch );
    act ( "$n has declined your challenge.", ch, NULL, victim, TO_VICT );
    if ( !spar )
    sprintf ( buf,
              "{w[{RARENA{w] {R%s {whas declined {R%s's {Wchallenge.{x\n\r",
              ch->name, victim->name );
    else
    sprintf ( buf,
              "{w[{RSPAR{w] {R%s {whas declined {R%s's {Wchallenge.{x\n\r",
              ch->name, victim->name );
    spar = false;
    for ( d = descriptor_list; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            send_to_char ( buf, d->character );
        }
    }
    return;
}

/*======================================================================*
 * function: do_bet                                                     *
 * purpose: to allow players to wager on the outcome of arena battles   *
 * written by: Doug Araya (whiplash@tft.nacs.net) 6-10-96               *
 *======================================================================*/
CH_CMD ( do_bet )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *fighter;
    int wager;

    return;
    argument = one_argument ( argument, arg );
    if ( argument[0] == '\0' || !is_number ( arg ) )
    {
        send_to_char ( "Syntax: BET [amount in plat] [player]\n\r", ch );
        return;
    }
    if ( ch->gladiator != NULL )
    {
        send_to_char ( "You have already placed a bet on this fight.\n\r", ch );
        return;
    }
/*== disable the actual fighters from betting ==*/
    if ( IS_SET ( ch->act2, PLR2_CHALLENGER ) ||
         IS_SET ( ch->act2, PLR2_CHALLENGED ) )
    {
        send_to_char ( "You can't bet on this battle.\n\r", ch );
        return;
    }
    fighter = get_char_world ( ch, argument );
/*== make sure the choice is valid ==*/
    if ( fighter == NULL )
    {
        send_to_char ( "That player is not logged in.\n\r", ch );
        return;
    }
    if ( IS_NPC ( fighter ) )
    {
        send_to_char ( "Why bet on a mob? They aren't fighting...\n\r", ch );
        return;
    }
    if ( !IS_SET ( fighter->in_room->room_flags, ROOM_ARENA ) )
    {
        send_to_char ( "That player is not in the arena.\n\r", ch );
        return;
    }
/*== do away with the negative number trickery ==*/
    if ( !str_prefix ( "-", arg ) )
    {
        send_to_char ( "Error: Invalid argument!\n\r", ch );
        return;
    }
    wager = atoi ( arg );
    if ( wager > 300 || wager < 1 )
    {
        send_to_char ( "Wager range is between 1 and 300\n\r", ch );
        return;
    }
/*== make sure they have the cash ==*/
    if ( wager > ch->platinum )
    {
        send_to_char ( "You don't have that much platinum to wager!\n\r", ch );
        return;
    }
/*== now set the info ==*/
    ch->gladiator = fighter;
    ch->pcdata->plr_wager = wager;
    sprintf ( buf, "You have placed a %d platinum wager on %s\n\r", wager,
              fighter->name );
    send_to_char ( buf, ch );
    return;
}
