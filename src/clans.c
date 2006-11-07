/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.   *
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
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "lookup.h"

DECLARE_DO_FUN ( do_clanlist );

const struct clan_type clan_table[MAX_CLAN] = {
/*	name,		who entry,
	death-transfer room,	clan room entrance,
	clan pit vnum,
	independent,	pkill?		ext name

independent should be FALSE if is a real clan
pkill should be TRUE if pkilling is allowed

!!!DO NOT erase the first clan, all non-clanned players are
   placed in this first entry!!!

*/
    {"", "{x[    {bN{BP{bK{W     {x] ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, FALSE, "NPK"},

    {"Loner", "    {rI{RP{rK{x     ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, TRUE, "IPK"},

    {"Sages", "{mT{Dw{wi{Wli{wg{Dh{mt{WS{ma{wg{We{x",
     70101, 70100,
     OBJ_VNUM_PIT,
     FALSE, FALSE, "Sages of Twilight Grove"},

    {"DoD", "{C  D{ce{Dc{Wei{Dv{ce{Cr  {x",
     70400, 70406,
     OBJ_VNUM_PIT,
     FALSE, FALSE, "Disciples of Deception"},

    {"Void", "  {mN{bi{Dh{wil{Di{bs{mt  ",
     70700, 70750,
     OBJ_VNUM_PIT,
     FALSE, TRUE, "Followers of the Void"},

    {"Watch", " {RN{ri{Dg{wh{Wtw{wa{Dt{rc{Rh {x",
     70000, 70000,
     OBJ_VNUM_PIT,
     FALSE, TRUE, "Gaalstrom Nightwatch"},

    {"KoT", "   {YK{ye{we{ype{Yr{x   ",
     70300, 70300,
     OBJ_VNUM_PIT,
     FALSE, TRUE, "Keepers of the Tain"},

/*    {"NStalker", "{WN{wi{Dght{WS{wt{Dalker{x",
     70015, 70015,
     OBJ_VNUM_PIT,
     FALSE, TRUE, "NightStalkers"},

    {"HoS", " {MH{ma{Drb{win{Dge{mr{Ms{x ",
     70200, 70200,
     OBJ_VNUM_PIT,
     FALSE, TRUE, "Harbingers of Skulls"},

    {"TCC", " {YC{ye{Yl{ye{Ys{yt{Yi{ya{Yl{ys{x ",
     70600, 70600,
     OBJ_VNUM_PIT,
     FALSE, TRUE, "Celestial Chorus"}, */

    {"clan7", " none ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     FALSE, TRUE, "Unused"},

    {"clan8", " none ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     FALSE, TRUE, "Unused"},

    {"clan9", "  {rI{RP{rK{x   ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, TRUE, "Unused"},

    {"clan10", "  {rI{RP{rK{x   ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, TRUE, "Unused"},

    {"clan11", "  {rI{RP{rK{x   ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, TRUE, "Unused"},

    {"clan12", "  {rI{RP{rK{x   ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, TRUE, "Unused"},

    {"clan13", "  {rI{RP{rK{x   ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, TRUE, "Unused"},

    {"clan14", "  {rI{RP{rK{x   ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, TRUE, "Unused"},

    {"clan15", "  {rI{RP{rK{x   ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, TRUE, "Unused"},

    {"clan16", "  {rI{RP{rK{x   ",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, TRUE, "Unused"},

    {"rot", "ROT",
     ROOM_VNUM_ALTAR, ROOM_VNUM_ALTAR,
     OBJ_VNUM_PIT,
     TRUE, FALSE, "Unused"},

};

CH_CMD ( do_clead )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument ( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Syntax: clead <char>\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They aren't playing.\n\r", ch );
        return;
    }

    if ( !is_clan ( victim ) )
    {
        send_to_char ( "This person is not in a clan.\n\r", ch );
        return;
    }

    if ( clan_table[victim->clan].independent )
    {
        sprintf ( buf, "This person is a %s.\n\r",
                  clan_table[victim->clan].name );
        send_to_char ( buf, ch );
        return;
    }

    if ( is_clead ( victim ) )
    {
        sprintf ( buf, "They are no longer leader of clan %s.\n\r",
                  capitalize ( clan_table[victim->clan].name ) );
        send_to_char ( buf, ch );
        sprintf ( buf, "You are no longer leader of clan %s.\n\r",
                  capitalize ( clan_table[victim->clan].name ) );
        send_to_char ( buf, victim );
        update_clanlist ( victim, victim->clan, FALSE, TRUE );
        victim->clead = 0;
        victim->clan_rank = 1;
    }
    else
    {
        sprintf ( buf, "They are now leader of clan %s.\n\r",
                  capitalize ( clan_table[victim->clan].name ) );
        send_to_char ( buf, ch );
        sprintf ( buf, "You are now leader of clan %s.\n\r",
                  capitalize ( clan_table[victim->clan].name ) );
        send_to_char ( buf, victim );
        update_clanlist ( victim, victim->clan, TRUE, TRUE );
        victim->clan_rank = 10;
        victim->clead = victim->clan;
    }
}

CH_CMD ( do_guild )
{
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int clan;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Syntax: guild <char> <cln name>\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL ||
         ( victim->level > ch->level && victim->level == MAX_LEVEL ) )
    {
        send_to_char ( "They aren't playing.\n\r", ch );
        return;
    }

    if ( !str_prefix ( arg2, "none" ) )
    {
        send_to_char ( "They are now clanless.\n\r", ch );
        send_to_char ( "You are now a member of no clan!\n\r", victim );
        if ( is_clead ( victim ) )
            update_clanlist ( victim, victim->clead, FALSE, TRUE );
        if ( is_clan ( victim ) )
            update_clanlist ( victim, victim->clan, FALSE, FALSE );
        victim->clan_rank = 0;
        victim->clead = 0;
        victim->clan = 0;
        return;
    }

    if ( ( clan = clan_lookup ( arg2 ) ) == 0 )
    {
        send_to_char ( "No such clan exists.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act, PLR_NOCLAN ) && clan_table[clan].pkill )
    {
        send_to_char ( "This player is banned from pkill clans.\n\r", ch );
        return;
    }

    if ( clan_table[clan].independent )
    {
        sprintf ( buf, "They are now a %s.\n\r", clan_table[clan].name );
        send_to_char ( buf, ch );
        sprintf ( buf, "You are now a %s.\n\r", clan_table[clan].name );
        send_to_char ( buf, victim );
    }
    else
    {
        sprintf ( buf, "They are now a member of clan %s.\n\r",
                  capitalize ( clan_table[clan].name ) );
        send_to_char ( buf, ch );
        sprintf ( buf, "You are now a member of clan %s.\n\r",
                  capitalize ( clan_table[clan].name ) );
        send_to_char ( buf, victim );
    }

    if ( is_clead ( victim ) )
    {
        update_clanlist ( victim, victim->clead, FALSE, TRUE );
        victim->clead = 0;
        victim->clan_rank = 10;
    }
    if ( is_clan ( victim ) )
    {
        update_clanlist ( victim, victim->clan, FALSE, FALSE );
        victim->clan = 0;
        victim->clan_rank = 0;
    }
    update_clanlist ( victim, clan, TRUE, FALSE );
    victim->clan = clan;
    victim->clan_rank = 1;
}

CH_CMD ( do_member )
{
    char arg1[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument ( argument, arg1 );

    if ( IS_NPC ( ch ) )
    {
        return;
    }

    if ( !is_clead ( ch ) )
    {
        if ( !ch->invited )
        {
            send_to_char ( "You have not been invited to join a clan.\n\r",
                           ch );
            return;
        }
        if ( !str_cmp ( arg1, "accept" ) )
        {
            sprintf ( buf, "{RYou are now a member of clan {x[{%s%s{x]\n\r",
                      clan_table[ch->invited].pkill ? "B" : "M",
                      clan_table[ch->invited].who_name );
            send_to_char ( buf, ch );
            ch->clan = ch->invited;
            update_clanlist ( ch, ch->invited, TRUE, FALSE );
            ch->invited = 0;
            ch->clan_rank = 1;
            return;
        }
        if ( !str_cmp ( arg1, "deny" ) )
        {
            send_to_char ( "You turn down the invitation.\n\r", ch );
            ch->invited = 0;
            return;
        }
        send_to_char ( "Syntax: member <accept|deny>\n\r", ch );
        return;
    }

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Syntax: member <char>\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They aren't playing.\n\r", ch );
        return;
    }
    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "NPC's cannot join clans.\n\r", ch );
        return;
    }

    if ( is_pkill ( ch ) && is_pkill ( victim ) )
    {
        send_to_char ( "Ok.\n\r", ch );
    }
    else
    {
        if ( !is_pkill ( ch ) && !is_pkill ( victim ) )
        {
            send_to_char ( "Ok.\n\r", ch );
        }
        else
        {
            send_to_char
                ( "Sorry, PK players can not join NPK clans, and vice-versa. Rerolling bypasses this.\n\r",
                  ch );
            return;
        }
    }

    if ( !is_pkill ( ch ) && !is_pkill ( victim ) )

        if ( IS_SET ( victim->act, PLR_NOCLAN ) && clan_table[ch->clan].pkill )
        {
            send_to_char ( "This player is banned from pkill clans.\n\r", ch );
            return;
        }
    if ( victim == ch )
    {
        send_to_char ( "You're stuck...only a god can help you now!\n\r", ch );
        return;
    }

    if ( ( is_clan ( victim ) && !is_same_clan ( ch, victim ) ) &&
         !( clan_table[victim->clan].independent ) )
    {
        send_to_char ( "They are in another clan already.\n\r", ch );
        return;
    }

    if ( is_clan ( victim ) && ch->clan == victim->clan )
    {
        if ( is_clead ( victim ) )
        {
            send_to_char ( "You can't kick out another clan leader.\n\r", ch );
            return;
        }
        send_to_char ( "They are now clanless.\n\r", ch );
        send_to_char ( "Your clan leader has kicked you out!\n\r", victim );
        if ( is_pkill ( victim ) )
        {
            victim->clan = 0;
            victim->clan = clan_lookup ( "loner" );
        }
        else
        {
            victim->clan = 0;
        }
        victim->clan_rank = 0;
        update_clanlist ( victim, victim->clan, FALSE, FALSE );
        return;
    }
    if ( victim->invited )
    {
        send_to_char ( "They have already been invited to join a clan.\n\r",
                       ch );
        return;
    }
    if ( victim->level < 50 || victim->level > 202 )
    {
        send_to_char ( "They must be between levels 50 -> 201.\n\r", ch );
        return;
    }
    if ( victim->level < 50 || victim->level > 202 )
    {
        send_to_char ( "They must be between levels 50 -> 201.\n\r", ch );
        return;
    }
    sprintf ( buf, "%s has been invited to join your clan.\n\r", victim->name );
    send_to_char ( buf, ch );
    sprintf ( buf, "{RYou have been invited to join clan {x[{%s%s{x]\n\r",
              clan_table[ch->clan].pkill ? "B" : "M",
              clan_table[ch->clan].who_name );
    send_to_char ( buf, victim );
    send_to_char ( "{YUse {Gmember accept{Y to join this clan,{x\n\r", victim );
    send_to_char ( "{Yor {Gmember deny{Y to turn down the invitation.{x\n\r",
                   victim );
    victim->invited = ch->clan;
}

CH_CMD ( do_cgossip )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int wtime;

    if ( !is_clan ( ch ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "You are not in a clan!\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOCGOSSIP ) )
        {
            send_to_char ( "Clan gossip channel is now ON.\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOCGOSSIP );
        }
        else
        {
            send_to_char ( "Clan gossip channel is now OFF.\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOCGOSSIP );
        }
    }
    else                        /* cgossip message sent, turn cgossip on if it
                                   isn't already */
    {
        if ( IS_SET ( ch->comm, COMM_QUIET ) )
        {
            send_to_char ( "You must turn off quiet mode first.\n\r", ch );
            return;
        }

        if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
        {
            send_to_char ( "The gods have revoked your channel priviliges.\n\r",
                           ch );
            return;
        }

        REMOVE_BIT ( ch->comm, COMM_NOCGOSSIP );

        sprintf ( buf,
                  "{r-{R={gYou{R={r- {W({CC{clan{CG{cossip{W){x '{W%s{x'\n\r",
                  argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;
            int pos;
            bool found = FALSE;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 !IS_SET ( victim->comm, COMM_NOCGOSSIP ) &&
                 !IS_SET ( victim->comm, COMM_QUIET ) && ( is_clan ( victim ) ||
                                                           IS_IMMORTAL
                                                           ( victim ) ) )
            {
                for ( pos = 0; pos < MAX_FORGET; pos++ )
                {
                    if ( victim->pcdata->forget[pos] == NULL )
                        break;
                    if ( !str_cmp ( ch->name, victim->pcdata->forget[pos] ) )
                        found = TRUE;
                }
                if ( !found )
                {
                    act_new
                        ( "{r-{R={g$n{R={r- {W({CC{clan{CG{cossip{W){x '{W$t{x'",
                          ch, argument, d->character, TO_VICT, POS_SLEEPING );
                }
            }
        }
    }
    wtime = UMAX ( 2, 9 - ( ch->level ) );
    if ( !IS_IMMORTAL ( ch ) )
        WAIT_STATE ( ch, wtime );
}

CH_CMD ( do_clantalk )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( !str_prefix ( argument, "list" ) && argument[0] != '\0' )
    {
        argument = one_argument ( argument, arg );
        do_clans ( ch, argument );
        return;
    }

    if ( ( !is_clan ( ch ) || clan_table[ch->clan].independent ) && !IS_IMMORTAL ( ch ) )
    {
        send_to_char ( "You aren't in a clan.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        if ( IS_SET ( ch->comm, COMM_NOCLAN ) )
        {
            send_to_char ( "Clan channel is now ON\n\r", ch );
            REMOVE_BIT ( ch->comm, COMM_NOCLAN );
        }
        else
        {
            send_to_char ( "Clan channel is now OFF\n\r", ch );
            SET_BIT ( ch->comm, COMM_NOCLAN );
        }
        return;
    }

    if ( IS_SET ( ch->comm, COMM_NOCHANNELS ) )
    {
        send_to_char ( "The gods have revoked your channel priviliges.\n\r",
                       ch );
        return;
    }

    REMOVE_BIT ( ch->comm, COMM_NOCLAN );
    argument = makedrunk ( argument, ch );
    pcolor ( ch, argument, 0 );

    sprintf ( buf,
              "{w[{oC{ol{oa{on{w] {w-{W={YYou to %s{W{w=- {W'{w%s{W'{x\n\r",
              clan_table[ch->clan].exname, argument );
    send_to_char ( buf, ch );
    sprintf ( buf, "{w[{oC{ol{oa{on{w] {w-{W={Y%s to %s{W{w=- {W'{w%s{W'{x\n\r",
              ch->name, clan_table[ch->clan].exname, argument );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {
        int pos;
        bool found = FALSE;

        if ( d->connected == CON_PLAYING && d->character != ch &&
             ( is_same_clan ( ch, d->character ) ||
               IS_IMMORTAL ( d->character ) ) &&
             !IS_SET ( d->character->comm, COMM_NOCLAN ) &&
             !IS_SET ( d->character->act, PLR_IMMNOCLAN ) &&
             !IS_SET ( d->character->comm, COMM_QUIET ) )
        {
            for ( pos = 0; pos < MAX_FORGET; pos++ )
            {
                if ( d->character->pcdata->forget[pos] == NULL )
                    break;
                if ( !str_cmp ( ch->name, d->character->pcdata->forget[pos] ) )
                    found = TRUE;
            }
            if ( !found )
            {
                act_new ( buf, ch, argument, d->character, TO_VICT, POS_DEAD );
            }
        }
    }

    return;
}

CH_CMD ( do_clans )
{
    char buf[MAX_STRING_LENGTH];
    BUFFER *output;
    int e;

    output = new_buf (  );
    sprintf ( buf,
              "{x\n\r{R[] {x= {RP{rkill {WClan    {M[]{x = {MN{mon{x-{RP{rkill {WClan{x\n\r" );
    add_buf ( output, buf );
    for ( e = 0; e < MAX_CLAN; e++ )
    {
        if ( str_cmp ( clan_table[e].exname, "Unused" ) )
        {
            sprintf ( buf, "{D-------------------------------{x\n\r" );
            add_buf ( output, buf );
            sprintf ( buf, "{D| {WName{W: {W%s{x\n\r", clan_table[e].name );
            add_buf ( output, buf );
            if ( IS_IMMORTAL ( ch ) )
            {
                sprintf ( buf, "{D| {WRecall: %ld, Entrance: %ld{x\n\r",
                          clan_table[e].hall, clan_table[e].entrance );
                add_buf ( output, buf );
            }
            if ( clan_table[e].pkill )
                sprintf ( buf,
                          "{D| {WWho Tag: {R[%s{R]{x\n\r{D| {WDesc{W: {g%s{x\n\r",
                          clan_table[e].who_name, clan_table[e].exname );

            else
                sprintf ( buf,
                          "{D| {WWho Tag: {M[%s{M]{x\n\r{D| {WDesc{W: {g%s{x\n\r",
                          clan_table[e].who_name, clan_table[e].exname );
            add_buf ( output, buf );
        }
    }
    sprintf ( buf, "{D-------------------------------{x\n\r" );
    add_buf ( output, buf );
    page_to_char ( buf_string ( output ), ch );
    free_buf ( output );
    return;
}

CH_CMD ( do_noclan )
{
    char arg[MAX_INPUT_LENGTH], buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;

    one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Noclan whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world ( ch, arg ) ) == NULL )
    {
        send_to_char ( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust ( victim ) >= get_trust ( ch ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET ( victim->act, PLR_NOCLAN ) )
    {
        REMOVE_BIT ( victim->act, PLR_NOCLAN );
        send_to_char ( "NOCLAN removed.\n\r", ch );
        sprintf ( buf, "$N allows %s to join pkill clans.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }
    else
    {
        SET_BIT ( victim->act, PLR_NOCLAN );
        if ( is_clead ( victim ) )
            update_clanlist ( victim, victim->clead, FALSE, TRUE );
        if ( is_clan ( victim ) )
            update_clanlist ( victim, victim->clan, FALSE, FALSE );
        victim->clan = 0;
        victim->clead = 0;
        send_to_char ( "NOCLAN set.\n\r", ch );
        sprintf ( buf, "$N forbids %s to join pkill clans.", victim->name );
        wiznet ( buf, ch, NULL, WIZ_PENALTIES, WIZ_SECURE, 0 );
    }

    save_char_obj ( victim );

    return;
}

void save_clanlist ( int clannum )
{
    char buf[MAX_STRING_LENGTH];
    CLN_DATA *pcln;
    MBR_DATA *pmbr;
    FILE *fp;
    bool found;

    if ( !str_cmp ( clan_table[clannum].exname, "Unused" ) )
    {
        return;
    }
    sprintf ( buf, "%s%s.cln", CLAN_DIR, clan_table[clannum].name );
    if ( ( fp = file_open ( buf, "w" ) ) == NULL )
    {
        perror ( buf );
    }
    found = FALSE;
    for ( pcln = cln_list; pcln != NULL; pcln = pcln->next )
    {
        if ( pcln->clan == clannum )
        {
            found = TRUE;
            if ( sizeof ( pcln->members ) < 1 )
                return;

            fprintf ( fp, "%d\n", pcln->members );
            for ( pmbr = pcln->list; pmbr != NULL; pmbr = pmbr->next )
            {
                fprintf ( fp, "%s\n", pmbr->name );
            }
        }
    }
    if ( !found )
    {
        fprintf ( fp, "0\n" );
    }
    file_close ( fp );
}

void load_clanlist ( void )
{
    char buf[MAX_STRING_LENGTH];
    FILE *fp;
    MBR_DATA *mbr_last;
    int clannum;

    strcat ( boot_buf, "- the Powe" );
    for ( clannum = 0; clannum < MAX_CLAN; clannum++ )
    {
        if ( str_cmp ( clan_table[clannum].exname, "Unused" ) )
        {
            CLN_DATA *pcln;

            pcln = new_cln (  );
            pcln->clan = clannum;
            pcln->name = str_dup ( clan_table[clannum].name );
            sprintf ( buf, "%s%s.cln", CLAN_DIR, clan_table[clannum].name );
            if ( ( fp = file_open ( buf, "r" ) ) == NULL )
            {
                pcln->members = 0;
            }
            else
            {
                pcln->members = fread_number ( fp );
                fread_to_eol ( fp );
                mbr_last = NULL;
                for ( ;; )
                {
                    MBR_DATA *pmbr;

                    if ( feof ( fp ) )
                    {
                        break;
                    }

                    pmbr = new_mbr (  );

                    pmbr->name = str_dup ( fread_word ( fp ) );
                    fread_to_eol ( fp );

                    if ( pcln->list == NULL )
                        pcln->list = pmbr;
                    else
                        mbr_last->next = pmbr;
                    mbr_last = pmbr;
                }
                file_close ( fp );
            }
            pcln->next = cln_list;
            cln_list = pcln;
        }
    }
    strcat ( boot_buf, "rs that Be." );
    return;
}

void update_clanlist ( CHAR_DATA * ch, int clannum, bool add, bool clead )
{
//    MBR_DATA *prev;
    MBR_DATA *curr;
    CLN_DATA *pcln;

    if ( IS_NPC ( ch ) )
    {
        return;
    }
    else
    {
        return;
    }

    for ( pcln = cln_list; pcln != NULL; pcln = pcln->next )
    {
        if ( pcln->clan == clannum )
        {
            if ( clead )
            {
                if ( !add )
                {
/*                    prev = NULL;
                    for ( curr = pcln->list; curr != NULL;
                          prev = curr, curr = curr->next )
                    {
                        if ( !str_cmp ( ch->name, curr->name ) )
                        {
                            if ( prev == NULL )
                                pcln->list = pcln->list->next;
                            else
                                prev->next = curr->next;

                            free_mbr ( curr );
                            save_clanlist ( clannum ); 
                        }
                    }
                    return; */
                }
                else
                {
                    curr = new_mbr (  );
                    curr->name = str_dup ( ch->name );
                    curr->next = pcln->list;
                    pcln->list = curr;
                    save_clanlist ( clannum );
                    return;
                }
            }
            if ( add )
                pcln->members++;
            else
            {
                if ( pcln->members > 0 )
                    pcln->members--;
            }
            if ( pcln->members < 0 )
                pcln->members = 0;
            save_clanlist ( clannum );
        }
    }
    return;
}

char *player_clanwho ( CHAR_DATA * ch )
{
    if ( ch->clan == 0 )
        return '\0';
    return clan_table[ch->clan].who_name;
}

/* do_clanwho: hack of do_who to return everyone in your clan */
CH_CMD ( do_clanwho )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    BUFFER *output;
    DESCRIPTOR_DATA *d;
    int iClan;
    int nNumber;
    int nMatch;
    bool fClan = FALSE;
    bool fClanRestrict = FALSE;
    bool rgfClan[MAX_CLAN];

    if ( !is_clan ( ch ) )
    {
        send_to_char ( "You are not a member of a clan.", ch );
        return;
    }

    nNumber = 0;
    fClan = TRUE;
    iClan = clan_lookup ( clan_table[ch->clan].name );
    if ( iClan )
    {

        fClanRestrict = TRUE;
        rgfClan[iClan] = TRUE;
    }

    /* 
     * Now
     show matching chars.
     */
    nMatch = 0;
    buf[0] = '\0';
    output = new_buf (  );
    sprintf ( buf, "\n\r{c-----------%-11s{c----------\n\r",
              player_clanwho ( ch ) );
    add_buf ( output, buf );
    sprintf ( buf, "{CR{cank       {c| {CN{came{x                \n\r" );
    add_buf ( output, buf );
    sprintf ( buf, "{c------------------------------{x\n\r" );
    add_buf ( output, buf );
    for ( d = descriptor_list; d != NULL; d = d->next )
    {

        CHAR_DATA *wch;
        char const *class;

        /* 
         * Check
         for match against restrictions.
         * Don't use trust as that exposes
         trusted mortals.
         */
        if ( d->connected != CON_PLAYING || !can_see ( ch, d->character ) )
            continue;

        wch = ( d->original != NULL ) ? d->original : d->character;

        if ( !can_see ( ch, wch ) )
            continue;

        if ( ( fClan && !is_clan ( wch ) ) ||
             ( fClanRestrict && !rgfClan[wch->clan] ) ||
             ( wch->clan != ch->clan ) )
            continue;

        nMatch++;

        /* 

         * Figure out what to print for class. */
        class = class_table[wch->class].who_name;

        /* 
         * Format it up.
         */
        sprintf ( buf, "%-15s{c| {x%s%s%s\n\r",
                  is_clan ( wch ) ? clan_rank_table[wch->clan_rank].
                  title_of_rank[wch->sex] : "", IS_SET ( wch->comm,
                                                         COMM_AFK ) ?
                  "{W[{RAFK{W]{x " : "", wch->name,
                  IS_NPC ( wch ) ? "" : wch->pcdata->title );
        add_buf ( output, buf );

    }

    sprintf ( buf2, "\n\r{CC{clan {CM{cembers {CF{cound{x: {&%d{x\n\r",
              nMatch );
    add_buf ( output, buf2 );
    page_to_char ( buf_string ( output ), ch );

    free_buf ( output );
    return;
}

CH_CMD ( do_promote )
{
    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( IS_NPC ( ch ) )
    {
        send_to_char ( "NPC's can not promote someone.\n\r", ch );
        return;
    }

    if ( ( ch->clan_rank < 9 ) && ( !IS_IMMORTAL ( ch ) ) )
    {
        send_to_char
            ( "You must be a clan Leader or Magistrate to promote someone.\n\r",
              ch );
        return;
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' || atoi ( arg2 ) < 1 ||
         atoi ( arg2 ) > 10 )
    {
        send_to_char ( "Syntax: promote <char> <rank 1-10>\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room ( ch, arg1 ) ) == NULL )
    {
        send_to_char ( "They must be present to be promoted.\n\r", ch );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "You must be mad.\n\r", ch );
        return;
    }

    if ( ( victim->clan != ch->clan ) && ( !IS_IMMORTAL ( ch ) ) )
    {
        send_to_char
            ( "You can not promote a player who is not in your clan.\n\r", ch );
        return;
    }

    /* if ( !is_clead(ch)) { send_to_char("This player is not qualified to
       lead.\n\r",ch); return; } */

    victim->clan_rank = atoi ( arg2 );

    send_to_char ( "Rank ceremony complete\n\r", ch );
    send_to_char ( "Rank ceremony complete\n\r", victim );

    return;
}

CH_CMD ( do_cdonate )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    long value;

    smash_tilde ( argument );
    argument = one_argument ( argument, arg1 );
    strcpy ( arg2, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char ( "Syntax:\n\r", ch );
        send_to_char ( "cdonate <field> <value>\n\r", ch );
        send_to_char ( "\n\rField being one of the following:\n\r", ch );
        send_to_char ( "iqp aqp\n\r", ch );
        return;
    }

    value = is_number ( arg2 ) ? atol ( arg2 ) : -1;

    if ( !str_prefix ( arg1, "iqp" ) )
    {
        if ( !is_clan ( ch ) )
        {
            send_to_char ( "You aren't in a clan.\n\r", ch );
            return;
        }
        {
            if ( clan_table[ch->clan].independent )
            {
                send_to_char ( "Loner clans don't have clan balances!\n\r",
                               ch );
                return;
            }
            ch->pcdata->clan_data->cbalance = +value;
            ch->qps = -value;
            printf_to_char ( ch,
                             "You have donated %ld to your clan balance, the clan's balance is now %ld",
                             value, ch->pcdata->clan_data->cbalance );
            return;
        }
    }

    if ( !str_prefix ( arg1, "aqp" ) )

    {
        if ( !is_clan ( ch ) )
        {
            send_to_char ( "You aren't in a clan.\n\r", ch );
            return;
        }

        if ( clan_table[ch->clan].independent )
        {
            send_to_char ( "Loner clans don't have clan balances!\n\r", ch );
            return;
        }
        if ( value % 20 == 0 )
        {
            ch->pcdata->questpoints -= value;
            ch->pcdata->clan_data->cbalance = +value / 20;
            printf_to_char ( ch,
                             "You donate %ld AQP. Your clans balance is now %ld.\n\r",
                             value, ch->pcdata->clan_data->cbalance );
            return;
        }
        else
        {
            printf_to_char ( ch, "Amount MUST be divisible by 20" );
            return;
        }
    }
    do_cdonate ( ch, "" );
    return;
}
