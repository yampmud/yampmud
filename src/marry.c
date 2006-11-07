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
*	ROM 2.4 is copyright 1993-1996 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@efn.org)				   *
*	    Gabrielle Taylor						   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/
/***************************************************************************
*	MARRY.C written by Ryouga for Vilaross Mud (baby.indstate.edu 4000)*
*	Please leave this and all other credit include in this package.    *
*	Email questions/comments to ryouga@jessi.indstate.edu		   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "merc.h"

CH_CMD ( do_marry )
{

    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *victim2;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( ( ch->level >= LEVEL_IMMORTAL ) )
    {
        if ( arg1[0] == '\0' || arg2[0] == '\0' )
        {
            send_to_char ( "Syntax: marry <char1> <char2>\n\r", ch );
            return;
        }
        if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
        {
            send_to_char ( "The first person mentioned isn't playing.\n\r",
                           ch );
            return;
        }

        if ( ( victim2 = get_char_world ( ch, arg2 ) ) == NULL )
        {
            send_to_char ( "The second person mentioned isn't playing.\n\r",
                           ch );
            return;
        }

        if ( IS_NPC ( victim ) || IS_NPC ( victim2 ) )
        {
            send_to_char
                ( "I don't think they want to be Married to the Mob.\n\r", ch );
            return;
        }

        if ( !IS_SET ( victim->act2, PLR2_CONSENT ) ||
             !IS_SET ( victim2->act2, PLR2_CONSENT ) )
        {
            send_to_char ( "They do not give consent.\n\r", ch );
            return;
        }

        if ( victim->pcdata->spouse > 0 || victim2->pcdata->spouse > 0 )
        {
            send_to_char ( "They are already married! \n\r", ch );
            return;
        }

        if ( victim->level < 12 || victim2->level < 12 )
        {
            send_to_char ( "They are not of the proper level to marry.\n\r",
                           ch );
            return;
        }

        send_to_char ( "You pronounce them man and wife!\n\r", ch );
        send_to_char ( "You say the big 'I do.'\n\r", victim );
        send_to_char ( "You say the big 'I do.'\n\r", victim2 );
        victim->pcdata->spouse = victim2->name;
        victim2->pcdata->spouse = victim->name;
        return;

    }
    else
    {
        send_to_char ( "You do not have marrying power.\n\r", ch );
        return;
    }
}

CH_CMD ( do_divorce )
{

    char arg1[MAX_INPUT_LENGTH], arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    CHAR_DATA *victim2;

    argument = one_argument ( argument, arg1 );
    argument = one_argument ( argument, arg2 );

    if ( ch->level >= LEVEL_IMMORTAL )
    {
        if ( arg1[0] == '\0' || arg2[0] == '\0' )
        {
            send_to_char ( "Syntax: divorce <char1> <char2>\n\r", ch );
            return;
        }
        if ( ( victim = get_char_world ( ch, arg1 ) ) == NULL )
        {
            send_to_char ( "The first person mentioned isn't playing.\n\r",
                           ch );
            return;
        }

        if ( ( victim2 = get_char_world ( ch, arg2 ) ) == NULL )
        {
            send_to_char ( "The second person mentioned isn't playing.\n\r",
                           ch );
            return;
        }

        if ( IS_NPC ( victim ) || IS_NPC ( victim2 ) )
        {
            send_to_char ( "I don't think they're Married to the Mob...\n\r",
                           ch );
            return;
        }

        if ( !IS_SET ( victim->act2, PLR2_CONSENT ) ||
             !IS_SET ( victim2->act2, PLR2_CONSENT ) )
        {
            send_to_char ( "They do not give consent.\n\r", ch );
            return;
        }

        if ( victim->pcdata->spouse != victim2->name )
        {
            send_to_char ( "They aren't even married!!\n\r", ch );
            return;
        }

        send_to_char ( "You hand them their papers.\n\r", ch );
        send_to_char ( "Your divorce is final.\n\r", victim );
        send_to_char ( "Your divorce is final.\n\r", victim2 );
        victim->pcdata->spouse = NULL;
        victim2->pcdata->spouse = NULL;
        return;

    }
    else
    {
        send_to_char ( "You do not have divorcing power.\n\r", ch );
        return;
    }
}

void do_vow ( CHAR_DATA * ch, char *argument )
{
    if ( IS_NPC ( ch ) )
        return;

    if ( IS_SET ( ch->act2, PLR2_CONSENT ) )
    {
        send_to_char ( "Yoy no longer give consent to be married.\n\r", ch );
        REMOVE_BIT ( ch->act2, PLR2_CONSENT );
        return;
    }

    send_to_char ( "You now give consent to Married!\n\r", ch );
    SET_BIT ( ch->act2, PLR2_CONSENT );
    return;
}

CH_CMD ( do_spousetalk )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
        send_to_char ( "What do you wish to tell your other half?\n\r", ch );
        return;
    }
    else                        /* message sent */
    {

        sprintf ( buf, "{c[You {gspoucetalk to %s{c], {x'{y%s{x'\n\r",
                  ch->pcdata->spouse, argument );
        send_to_char ( buf, ch );
        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            CHAR_DATA *victim;

            victim = d->original ? d->original : d->character;

            if ( d->connected == CON_PLAYING && d->character != ch &&
                 d->character->name == ch->pcdata->spouse )
            {
                act_new ( "{c[{x$n {yspoucetalks{x, '{g$t{x'{c]", ch, argument,
                          d->character, TO_VICT, POS_SLEEPING );
            }
            else
            {
                send_to_char ( "Your spouse is not here.\n\r", ch );
                return;
            }
        }
    }
}
