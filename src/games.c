/*
01234567890123456789012345678901234567890123456789012345678901234567890123456789
Game Code v2 for ROM based muds. Robert Schultz, Sembiance  -  bert@ncinter.net
Snippets of mine can be found at http://www.ncinter.net/~bert/mud/
This file (games.c) contains all the game functions.
*/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "merc.h"
#include "recycle.h"
#include "games.h"

CH_CMD ( do_game )
{
    char arg1[MAX_INPUT_LENGTH];
    int whichGame;

    argument = one_argument ( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char ( "Type 'help games' for more information on games.\n\r",
                       ch );
        return;
    }

    if ( IS_NPC ( ch ) )
    {
        send_to_char ( "Sorry, only player characters may play games.\n\r",
                       ch );
        return;
    }

    if ( !str_cmp ( arg1, "slot" ) || !str_cmp ( arg1, "slots" ) )
        whichGame = GAME_SLOTS;
    else if ( !str_cmp ( arg1, "highdice" ) )
        whichGame = GAME_HIGH_DICE;
    else
        whichGame = GAME_NONE;

    switch ( whichGame )
    {
        case GAME_SLOTS:
            do_slots ( ch, argument );
            break;
        case GAME_HIGH_DICE:
            do_high_dice ( ch, argument );
            break;
        default:
            send_to_char
                ( "Thats not a game. Type 'help games' for a list.\n\r", ch );
            break;
    }

    return;
}

CH_CMD ( do_slots )
{
    OBJ_DATA *slotMachine;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    int counter, winArray[11];
    int cost, jackpot, bars, winnings, numberMatched;
    int bar1, bar2, bar3, bar4, bar5;
    bool partial, won, wonJackpot, frozen, foundSlot;

    char *bar_messages[] = {
        "<------------>",
        "{YGold Coin{x",        /* 1 */
        "{rRedDragon{x",
        "{DVengeance{x",        /* 3 */
        "{RCherries!{x",
        "{CPlatemail{x",        /* 5 */
        "{yTangerine{x",
        "{mMagicWand{x",
        "{GElemental{x",
        "{WLongsword{x",
        "{MPinkPixie{x",        /* 10 */
    };

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char ( "Syntax: game slots <which machine>\n\r", ch );
        return;
    }

    foundSlot = false;

    for ( slotMachine = ch->in_room->contents; slotMachine != NULL;
          slotMachine = slotMachine->next_content )
    {
        if ( ( slotMachine->item_type == ITEM_SLOT_MACHINE ) &&
             ( can_see_obj ( ch, slotMachine ) ) )
        {
            if ( is_name ( arg, slotMachine->name ) )
            {
                foundSlot = true;
                break;
            }
            else
            {
                foundSlot = false;
            }
        }
    }

    if ( foundSlot == false )
    {
        send_to_char ( "That slot machine is not here.\n\r", ch );
        return;
    }

    cost = slotMachine->value[0];
    if ( cost <= 0 )
    {
        send_to_char ( "This slot machine seems to be broken.\n\r", ch );
        return;
    }

    if ( cost > ( ch->gold + ( ch->platinum * 100 ) ) )
    {
        sprintf ( buf, "This slot machine costs %d gold to play.\n\r", cost );
        send_to_char ( buf, ch );
        return;
    }

    deduct_cost ( ch, cost, VALUE_GOLD );

    jackpot = slotMachine->value[1];
    bars = slotMachine->value[2];

    if ( slotMachine->value[3] == 1 )
        partial = true;
    else
        partial = false;

    if ( slotMachine->value[4] == 1 )
        frozen = true;

    else
        frozen = false;

    bar1 = number_range ( 1, 10 );
    bar2 = number_range ( 1, 10 );
    bar3 = number_range ( 1, 10 );
    if ( bars > 3 )
    {
        bar4 = number_range ( 1, 10 );
        bar5 = number_range ( 1, 10 );
    }
    else
    {
        bar4 = 0;
        bar5 = 0;
    }

    if ( bars == 3 )
    {
        send_to_char
            ( "{g////------------{MSlot Machine{g------------\\\\\\\\{x\n\r",
              ch );
        sprintf ( buf, "{g|{C{{}{g|{x  %s  %s  %s  {h|{C{{}{g|{x\n\r",
                  bar_messages[bar1], bar_messages[bar2], bar_messages[bar3] );
        send_to_char ( buf, ch );
        send_to_char
            ( "{g\\\\\\\\------------------------------------////{x\n\r", ch );
    }
    else
    {
        send_to_char
            ( "{g////-----------------------{MSlot Machine{g----------------------\\\\\\\\{x\n\r",
              ch );
        sprintf ( buf, "{g|{C{{}{g|{x  %s  %s  %s  %s  %s  {g|{C{{}{g|{x\n\r",
                  bar_messages[bar1], bar_messages[bar2], bar_messages[bar3],
                  bar_messages[bar4], bar_messages[bar5] );
        send_to_char ( buf, ch );
        send_to_char
            ( "{g\\\\\\\\---------------------------------------------------------////{x\n\r",
              ch );
    }

    wonJackpot = false;
    winnings = 0;
    won = false;
    numberMatched = 0;

    if ( bars == 3 )
    {
        if ( ( bar1 == bar2 ) && ( bar2 == bar3 ) )
        {
            winnings = jackpot; /* they won the jackpot, make it */
            won = true;         /* worth their while! */
            slotMachine->value[1] = cost * 75;  /* put it back to something */
            wonJackpot = true;
        }
        else
        {
            if ( !frozen )
                slotMachine->value[1] += cost;
        }
    }
    else if ( bars == 5 )
    {
        if ( ( bar1 == bar2 ) && ( bar2 == bar3 ) && ( bar3 == bar4 ) &&
             ( bar4 == bar5 ) )
        {
            winnings = jackpot; /* if no partial, better have a */
            won = true;         /* kick butt jackpot for them */
            slotMachine->value[1] = cost * 125;
            wonJackpot = true;
        }
        else
        {
            if ( !frozen )
                slotMachine->value[1] += cost;
        }
    }
    else
    {
        send_to_char
            ( "This is a bad slot machine. Contact casino administration.\n\r",
              ch );
        return;
    }
    if ( !frozen )
        if ( slotMachine->value[1] >= 32000 )
            slotMachine->value[1] = 31000;

    for ( counter = 0; counter <= 12; counter++ )
    {
        winArray[counter] = 0;
    }

    if ( !won && partial )
    {
        if ( bars == 3 )
        {
            if ( bar1 == bar2 )
            {
                winnings += cost / 2;
                won = true;
                numberMatched++;
            }
            if ( bar1 == bar3 )
            {
                numberMatched++;
                if ( won )
                    winnings += cost;
                else
                {
                    winnings += cost / 2;
                    won = true;
                }
            }
            if ( bar2 == bar3 )
            {
                numberMatched++;
                if ( won )
                    winnings += cost;
                else
                {
                    winnings += cost / 2;
                    won = true;
                }
            }
            if ( !frozen )
            {
                if ( !won )
                    slotMachine->value[1] += cost;
                else
                    slotMachine->value[1] -= winnings;
            }
        }
        if ( bars == 5 )
        {
            winArray[bar1]++;
            winArray[bar2]++;
            winArray[bar3]++;
            winArray[bar4]++;
            winArray[bar5]++;

            for ( counter = 0; counter <= 12; counter++ )
            {
                if ( winArray[counter] > 1 )
                    numberMatched += winArray[counter];
            }

            if ( numberMatched == 5 )
            {
                if ( !frozen )
                    slotMachine->value[1] -= ( cost * 7 ) / 2;
                winnings += cost * 7;
            }
            if ( numberMatched == 4 )
            {
                if ( !frozen )
                    slotMachine->value[1] -= ( cost * 5 ) / 2;
                winnings += cost * 5;
            }
            if ( numberMatched == 3 )
            {
                winnings += cost / 2;
                if ( !frozen )
                    slotMachine->value[1] += cost / 2;
            }
            if ( numberMatched == 2 )
            {
                if ( !frozen )
                    slotMachine->value[1] += cost - 1;
                winnings = 1;
            }
            if ( numberMatched == 0 )
            {
                winnings = 0;
                if ( !frozen )
                    slotMachine->value[1] += cost;
            }
            if ( winnings > 0 )
                won = true;
        }
    }
    // had to change cost to winnings here- Markanth
    add_cost ( ch, winnings, VALUE_GOLD );

    if ( won && wonJackpot )
    {
        sprintf ( buf,
                  "You won the jackpot worth %d gold!! The jackpot now stands at %ld gold.\n\r",
                  winnings, slotMachine->value[1] );
        send_to_char ( buf, ch );
    }
    if ( won && !wonJackpot )
    {
        sprintf ( buf,
                  "You matched %d bars and won %d gold! The jackpot is now worth %ld gold.\n\r",
                  numberMatched, winnings, slotMachine->value[1] );
        send_to_char ( buf, ch );
    }
    if ( !won )
    {
        sprintf ( buf,
                  "Sorry you didn't win anything. The jackpot is now worth %ld gold.\n\r",
                  slotMachine->value[1] );
        send_to_char ( buf, ch );
    }

    if ( slotMachine->value[1] >= 32000 )
        slotMachine->value[1] = 31000;

    return;
}

CH_CMD ( do_high_dice )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *dealer;
    int die, dealerDice, playerDice;
    int bet;

    argument = one_argument ( argument, arg );

    if ( arg[0] == '\0' || !is_number ( arg ) )
    {
        send_to_char ( "Syntax is: game highdice <bet>\n\r", ch );
        return;
    }

    bet = atoi ( arg );
    if ( bet < 10 )
    {
        send_to_char ( "Minimum bet is 10 gold coins.\n\r", ch );
        return;
    }

    if ( bet > 1000 )
    {
        send_to_char ( "Maximum bet is 1,000 gold coins.\n\r", ch );
        return;
    }

    for ( dealer = ch->in_room->people; dealer; dealer = dealer->next_in_room )
    {
        if ( IS_NPC ( dealer ) && IS_SET ( dealer->act2, ACT2_DEALER ) &&
             can_see ( ch, dealer ) )
            break;
    }

    if ( dealer == NULL )
    {
        send_to_char ( "You do not see any dice dealer here.\n\r", ch );
        return;
    }
// changed by Markanth
    if ( bet > ch->gold + ( ch->platinum * 100 ) )
    {
        send_to_char ( "You can not afford to bet that much!\n\r", ch );
        return;
    }

    dealerDice = 0;
    playerDice = 0;

    die = number_range ( 1, 6 );
    dealerDice += die;
    die = number_range ( 1, 6 );
    dealerDice += die;

    die = number_range ( 1, 6 );
    playerDice += die;
    die = number_range ( 1, 6 );
    playerDice += die;

    sprintf ( buf, "{c%s{g rolled two dice with a total of {W%d!{x\n\r",
              dealer->short_descr, dealerDice );
    send_to_char ( buf, ch );
    sprintf ( buf, "{gYou rolled two dice with a total of {W%d!{x\n\r",
              playerDice );
    send_to_char ( buf, ch );

    if ( dealerDice > playerDice )
    {
        sprintf ( buf,
                  "{RYou lost! {c%s{g takes your bet of {y%d gold{g.{x\n\r",
                  dealer->short_descr, bet );
        send_to_char ( buf, ch );
        deduct_cost ( ch, bet, VALUE_GOLD );
    }

    if ( dealerDice < playerDice )
    {
        sprintf ( buf,
                  "{GYou won! {c%s {ggives you your winnings of {y%d gold{g.{x\n\r",
                  dealer->short_descr, bet );
        send_to_char ( buf, ch );
        add_cost ( ch, bet, VALUE_GOLD );
    }

    if ( dealerDice == playerDice )
    {
        sprintf ( buf,
                  "{RYou lost! {gThe dealer always wins in a tie. You lose {y%d gold{g.{x\n\r",
                  bet );
        send_to_char ( buf, ch );
        deduct_cost ( ch, bet, VALUE_GOLD );
    }

    return;
}
