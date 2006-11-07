/***************************************************************************
 * c2004: nicoLe sativa kurlish & carl otto III.
 * Most of the spells have been created or heavily modified by us.
 * Credit has been given where it's due to the best of our ability.
 **************************************************************************/
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
*	ROM 2.4 is copyright 1993-1998 Russ Taylor			   *
*	ROM has been brought to you by the ROM consortium		   *
*	    Russ Taylor (rtaylor@hypercube.org)				   *
*	    Gabrielle Taylor (gtaylor@hypercube.org)			   *
*	    Brian Moore (zump@rom.org)					   *
*	By using this code, you have agreed to follow the terms of the	   *
*	ROM license, in the file Rom24/doc/rom.license			   *
***************************************************************************/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "magic.h"
#include "recycle.h"

MAGIC ( spell_entangle )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( IS_SET ( ch->in_room->room_flags, ROOM_ARENA ) )
    {
        send_to_char ( "Not in the arena you dont...\n\n", ch );
        return;
    }

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
        {
            send_to_char ( "Entangle yourself?\n\r", ch );
            return;
        }
        else
        {
            send_to_char ( "They are already entangled.\n\r", ch );
            return;
        }

    }
    if ( saves_spell ( level, victim, DAM_OTHER ) )
    {
        send_to_char ( "You failed!\n\r", ch );
        return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = number_range ( 2, 4 );
    af.location = APPLY_NONE;
    af.modifier = -1;
    af.bitvector = AFF_ENTANGLE;
    affect_to_char ( victim, &af );

    send_to_char ( "{gV{Gi{yn{ge{Gs{x grow up from the ground and bind you.", victim );
    act ( "{gV{Gi{yn{ge{Gs{x grow up from the ground and bind $n.", victim, NULL, NULL, TO_ROOM );
    return;
}

/* SPELLS (c) 2000 BY TAKA ( a_ghost_dancer@excite.com ) */

MAGIC ( spell_stone_meld )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin is already as hard as a rock.\n\r", ch );
        else
            act ( "$N is already as hard as can be.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = 0 - ( ch->level / 2) + dice ( 2, 10 );
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin melds into stone.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your skin melds into stone.\n\r", victim );
    return;
}

MAGIC ( spell_screen )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your are surrounded by a wall of smoke.\n\r", ch );
        else
            act ( "$N is already surrounded by smoke.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = 0 - ( 20 * ( level / 14 ) );
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n is hidden in smoke screen.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your are hidden in a smoke screen.\n\r", victim );
    return;
}

MAGIC ( spell_icerain )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    act ( "{RDrops of {Dice {Brain {Rshower down upon {W$N!{x", ch, NULL, victim, TO_ROOM );
    act ( "{RDrops of {Dice {Brain {Rshower down upon {W$N!{x", ch, NULL, victim, TO_CHAR );
    for ( i = 1; i < 8; i++ )
    {
        dam = dice ( ( level * 2 ), 8 );

        if ( saves_spell ( level, victim, DAM_COLD ) )
            dam /= 1.5;
        if ( victim->in_room == ch->in_room )
            damage ( ch, victim, dam, sn, DAM_COLD, TRUE );
    }
    return;
}

MAGIC ( spell_firerain )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 10; i++ )
    {
        dam = dice ( ( level * 2 ), 8 );

        if ( saves_spell ( level, victim, DAM_FIRE ) )
            dam /= 2;

        if ( victim->in_room == ch->in_room )
        {
            act ( "{RDrops of {rfire {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_ROOM );
            act ( "{RDrops of {rfire {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_FIRE, TRUE );
        }
    }
    return;
}

MAGIC ( spell_acidstorm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 4; i++ )
    {
        dam = dice ( ( level / 2 ), 8 );

        if ( saves_spell ( level, victim, DAM_ACID ) )
            dam /= 2;

        if ( victim->in_room == ch->in_room )
        {
            act ( "{RDrops of {Dacid {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_ROOM );
            act ( "{RDrops of {Dacid {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_ACID, TRUE );
        }
    }
    return;
}

MAGIC ( spell_firestorm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    for ( i = 1; i < 4; i++ )
    {
        dam = dice ( level, 10 );

        if ( saves_spell ( level, victim, DAM_FIRE ) )
            dam /= 2;

        if ( victim->in_room == ch->in_room )
        {
            act ( "{RDrops of {rfire {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_ROOM );
            act ( "{RDrops of {rfire {Brain {Rshower down upon {W$N!{x", ch,
                  NULL, victim, TO_CHAR );
            damage ( ch, victim, dam, sn, DAM_FIRE, TRUE );
        }
    }
    return;
}

MAGIC ( spell_bark_skin )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin is already covered in bark.\n\r", ch );
        else
            act ( "$N's skin is already the texture of bark.", ch, NULL, victim, TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level / 20;
    af.location = APPLY_AC;
    af.modifier = 0 - ( level / 20 );
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    af.where = TO_RESIST;
    af.type = sn;
    af.level = level;
    af.duration = level / 20;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = RES_WOOD;
    affect_to_char ( ch, &af );

    act ( "$n's skin is covered by bark.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your skin is covered by bark.\n\r", victim );
    return;
}

MAGIC ( spell_steel_skin )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "Your skin is already protected by steel.\n\r", ch );
        else
            act ( "$N is already protected by steel.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = level;
    af.location = APPLY_AC;
    af.modifier = 0 - ( level / 15 );
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    act ( "$n's skin becomes hard as steel.", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your skin becomes hard as steel.\n\r", victim );
    return;
}

/* End of Taka spells, as far as I can tell */

MAGIC ( spell_mystic_armor )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        send_to_char ( "They are already armored", ch );
        return;
    }

    af.type = sn;
    af.duration = 4 +( level / 3 );

    af.location = APPLY_AC;
    af.modifier = -10 - ( level / 8 );
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    if ( ch != victim )
    {
        act ( "$n is surrounded by $N's mystic armour.", victim, NULL, ch,
              TO_ROOM );
        act ( "$N is surrounded by your mystic armour.", ch, NULL, victim,
              TO_CHAR );
        act ( "You are surrounded by $N's mystic armour.", victim, NULL, ch,
              TO_CHAR );
    }
    else
    {
        act ( "$n is surrounded by a mystic armour.", victim, NULL, ch,
              TO_ROOM );
        send_to_char ( "You are protected by a mystic armor!", ch );
    }
    return;
}

MAGIC ( spell_meteor_storm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;

    if ( victim->in_room == ch->in_room )
    {
        act ( "{DM{re{Dt{re{Do{rr{Ds{x rain down upon {W$N!!!{x", ch, NULL, victim, TO_ROOM );
        act ( "{DM{re{Dt{re{Do{rr{Ds{x rain down upon {W$N!{x", ch, NULL, victim, TO_CHAR );
        damage ( ch, victim, dice ( level * 4, 45 ), sn, DAM_EARTH, TRUE );
        damage ( ch, victim, dice ( level * 4, 45 ), sn, DAM_HARM, TRUE );
        if ( number_percent ( ) <= get_curr_stat ( ch, STAT_INT ) )
        {
            damage ( ch, victim, dice ( level * 3, 45 ), sn, DAM_FIRE, TRUE );
            damage ( ch, victim, dice ( level * 2, 45 ), sn, DAM_HARM, TRUE );
        }
    }
    return;
}

MAGIC ( spell_call_darkness )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int dam, i;

    act ( "{D$n calls on the powers of the nights to attack $N!{x", ch, NULL, victim, TO_ROOM );
    act ( "{D$N is attacked by the powers of the night!!{x", ch, NULL, victim, TO_CHAR );

    for ( i = 1; i < 3; i++ )
    {
        dam = dice ( level, level / 2 );

        if ( victim->in_room == ch->in_room )
        {
            damage ( ch, victim, dam, sn, DAM_NEGATIVE, TRUE );

            if ( number_percent ( ) >= 91 )
            {
                if ( !IS_AFFECTED ( victim, AFF_BLIND ) || saves_spell ( level, victim, DAM_NEGATIVE ) )
                   return;

                 af.where = TO_AFFECTS;
                 af.type = sn;
                 af.level = level + 5;
                 af.location = APPLY_HITROLL;
                 af.modifier = -20;
                 af.duration = 2;
                 af.bitvector = AFF_BLIND;
                 affect_to_char ( victim, &af );
                 send_to_char ( "You are enshrouded in darkness!\n\r", victim );
                 act ( "$n is enshrouded in darkness.", victim, NULL, NULL, TO_ROOM );
                 return;
            }
        }
    }
    return;
}

MAGIC ( spell_ward )
{
    AFFECT_DATA af;

    if ( is_affected ( ch, sn ) )
    {
        send_to_char ( "You are already in a ward", ch );
        return;
    }

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level / 5;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = SHD_WARD;
    affect_to_char ( ch, &af );
    act ( "$n is warded.", ch, NULL, NULL, TO_ROOM );
    send_to_char ( "You are warded.\n\r", ch );
    return;
}

/*original writer unkown, out of paradox codebase RoT1.4/1.5 diriv*/
MAGIC ( spell_mana_shield )
{

    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( IS_SHIELDED ( victim, SHD_MANA ) )
    {
        if ( victim == ch )
            send_to_char ( "You are already surrounded by a mana shield.\n\r",
                           ch );
        else
            act ( "$N is already surrounded by a mana shield.", ch, NULL,
                  victim, TO_CHAR );
        return;
    }

    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level / 22;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = SHD_MANA;

    affect_to_char ( victim, &af );
    send_to_char ( "You are surrounded by a mana shield.\n\r", victim );
    act ( "$n is surrounded by a mana shield.", victim, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_soul_siphon )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    if ( victim == ch )
    {
        send_to_char ( "How would you do that?\n\r", ch );
        return;
    }

    if ( victim->hit < victim->max_hit / 16 )
    {
        act ( "$N is hurt and suspicious ... you can't get close enough.", ch,
              NULL, victim, TO_CHAR );
        return;
    }

    if ( ch->stunned )
    {
        send_to_char ( "You're still a little woozy.\n\r", ch );
        return;
    }

    dam = dice ( ch->level, 33 );

    if ( victim->perm_stat[STAT_WIS] > ch->perm_stat[STAT_WIS] )
        dam /= 1.25;
    if ( saves_spell ( level, victim, DAM_NEGATIVE ) )
        dam /= 2;

    act ( "You {Dsiphon{x off some of $N's {ds{do{du{dl{x.", ch, NULL, victim, TO_CHAR );
    act ( "$n {Dsiphons{x off some of your {ds{do{du{dl{x.", ch, NULL, victim, TO_VICT );

    victim->mana -= dam ;
    ch->hit += ( dam / 5 );
        
    return;
}
/*
MAGIC ( spell_banish )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;

    victim = ( CHAR_DATA * ) vo;

    if ( IS_AFFECTED ( victim, AFF_BANISH ) ||
         saves_spell ( level, victim, DAM_NEGATIVE ) )
        return;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 3 * level;
    af.location = APPLY_HITROLL;
    af.modifier = -1 * ( level / 5 );
    af.bitvector = AFF_BANISH;
    affect_to_char ( victim, &af );

    af.location = APPLY_SAVING_SPELL;
    af.modifier = level / 8;
    affect_to_char ( victim, &af );

    send_to_char ( "You have been banished.\n\r", victim );
    if ( ch != victim )
        act ( "$N has been banished.", ch, NULL, victim, TO_CHAR );
    return;
}*/

MAGIC ( spell_earthrise )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;

    if ( victim->in_room == ch->in_room )
    {
        act ( "Chunks of the {ge{ba{gr{bt{gh {Dr{ri{Ds{re {xup and come crashing down upon {W$N!!!{x", ch, NULL, victim, TO_ROOM );
        act ( "Chunks of the {ge{ba{gr{bt{gh {Dr{ri{Ds{re {xup and come crashing down upon {W$N!{x", ch, NULL, victim, TO_CHAR );
        damage ( ch, victim, dice ( level * 5, 40 ), sn, DAM_EARTH, TRUE );
        damage ( ch, victim, dice ( level * 5, 40 ), sn, DAM_EARTH, TRUE );
    }
    return;
}

/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

/* <<< SPELL SHOULD NOT BE USED IN SCROLLS OR ANYTHING THAT MORTALS MAY ACCESS!!!  Just for the IMMs to have Fun. >>> */

MAGIC ( spell_immortal_wrath )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;

    if ( victim->in_room == ch->in_room )
    {
        act ( "The {BImm{Wort{Bals {xrelease their {DW{RR{WA{RT{DH {xupon {c$N!!!{x", ch, NULL, victim, TO_ROOM );
        act ( "Using your {BImm{Wor{Btal {DP{Yo{yw{Ye{Dr{x, you relese your {DW{RR{WA{RT{DH {xupon {c$N!{x", ch, NULL, victim, TO_CHAR );
        send_to_char ( "Everything goes black..\n\r", victim );
        damage ( ch, victim, 1000000000, sn, DAM_OTHER, TRUE );
    }

    return;
}

/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

MAGIC ( spell_powersurge )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( ch, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "{WYour power is at its peak!{x\n\r", ch );
        else
            act ( "This spell can't be cast on others! ", ch, NULL, victim,
                  TO_CHAR );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_HIT;
    af.modifier = 2000;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_MANA;
    af.modifier = 6000;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_DAMROLL;
    af.modifier = level;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_HITROLL;
    af.modifier = level;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = 25;
    af.location = APPLY_AC;
    af.modifier = -500;
    af.bitvector = 0;
    affect_to_char ( victim, &af );

    act ( "$n gets a RUSH of power!", victim, NULL, NULL, TO_ROOM );
    send_to_char ( "Your power gets a huge BOOST!\n\r", ch );

    return;
}

/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

MAGIC ( spell_shadow_barrier )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( is_affected ( victim, sn ) )
    {
        if ( victim == ch )
            send_to_char ( "You are already protected by shadows.\n\r", ch );
        else
            act ( "$N is already protected by shadows.", ch, NULL, victim,
                  TO_CHAR );
        return;
    }
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = 2 + level;
    af.duration = level / 7;
    af.modifier = -50 - level * 2;
    af.location = APPLY_AC;
    af.bitvector = 0;
    affect_to_char ( victim, &af );
    send_to_char ( "The shadows surround you and create a barrier.\n\r",
                   victim );
    if ( ch != victim )
        act ( "$N is protected by a barrier of shadows.", ch, NULL, victim,
              TO_CHAR );
    return;
}

/**********************************
* Spell by Mangan of Death Rising *
* drmud.spaceservices.net 6000    *
* This Tag must stay w/ the Code  *
**********************************/

MAGIC ( spell_mistblend )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;

    victim = ( CHAR_DATA * ) vo;

    if ( IS_SHIELDED ( victim, SHD_MISTBLEND ) )
        return;

    act ( "$n blends into the mist.", victim, NULL, NULL, TO_ROOM );

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level + 98;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = SHD_MISTBLEND;
    affect_to_char ( victim, &af );

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level + 98;
    af.location = APPLY_DAMROLL;    /* high damroll since it goes away after
                                       first hit */
    af.modifier = 500;
    af.bitvector = SHD_MISTBLEND;
    affect_to_char ( victim, &af );

    send_to_char ( "You blend into the mist.\n\r", victim );
    return;
}

MAGIC ( spell_lifeforce )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( IS_SHIELDED ( victim, SHD_LIFE ) )
    {
        if ( victim == ch )
            send_to_char ( "You are already protected.\n\r", ch );
        else
            act ( "$n is already protected.", ch, NULL, victim, TO_CHAR );
        return;
    }
    if ( IS_NPC ( victim ) )
    {
        send_to_char ( "You failed.\n\r", ch );
        return;
    }

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level / 20;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = SHD_LIFE;

    affect_to_char ( victim, &af );
    send_to_char ( "You call upon your lifeforce to protect you.\n\r", victim );
    act ( "$n is surrounded by an elemental aura.", victim, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_downpour )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;
    int rds;

    rds = number_range ( 2, 2 + ( level / 40 ) );

    act ( "$n raises $s arms and prays for {cr{wa{Ci{cn{x.", ch, NULL, NULL, TO_ROOM );
    act ( "You raise your arms and pray for {cr{wa{Ci{cn{x.", ch, NULL, NULL, TO_CHAR );

    if ( saves_spell ( level, victim, DAM_WATER ) )
        dam /= 1.5;

    act ( "$N is caught in $n's downpour!", ch, NULL, victim, TO_NOTVICT );
    act ( "You are caught in $n's downpour!!", ch, NULL, victim, TO_VICT );
    act ( "$N is caught in your downpour!", ch, NULL, victim, TO_CHAR );

    while ( rds >= 0 )
    {
        dam = ( level * rds ) + dice ( 5, level );
        damage ( ch, victim, dam, sn, DAM_WATER, TRUE );
        rds--;
    }

    return;
}

MAGIC ( spell_dust_storm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, rds;

    rds = number_range ( 2, 2 + ( level / 40 ) );

    if ( saves_spell ( level, victim, DAM_EARTH ) )
        dam /= 1.5;

    act ( "$n summons a {yd{Du{Ys{yt st{Yo{Dr{ym{x to attack $N!", ch, NULL, victim, TO_NOTVICT );
    act ( "You are caught in $n's {yd{Du{Ys{yt st{Yo{Dr{ym{x!!", ch, NULL, victim, TO_VICT );
    act ( "You conjure a {yd{Du{Ys{yt st{Yo{Dr{ym{x to attack $N!", ch, NULL, victim, TO_CHAR );

    while ( rds >= 0 )
    {
        dam = ( level * rds ) + dice ( 7, level );
        damage ( ch, victim, dam, sn, DAM_EARTH, TRUE );
        rds--;
    }

    return;
}

MAGIC ( spell_ego_whip )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level * 5, 5 );
    if ( saves_spell ( level, victim, DAM_MENTAL ) )
        dam /= 1.5;

    if ( ( ch->perm_stat[STAT_INT] ) > ( victim->perm_stat[STAT_INT] ) )
        dam *= 3;
    else if ( ( ch->perm_stat[STAT_INT] ) == ( victim->perm_stat[STAT_INT] ) )
        dam *= 2;
    else if ( ( ch->perm_stat[STAT_INT] ) < ( victim->perm_stat[STAT_INT] ) )
        dam /= 2;

    act ( "$n prods your mind, looking for weaknesses.", ch, NULL, victim, TO_VICT );
    act ( "You prod $N's mind, looking for weaknesses.", ch, NULL, victim, TO_CHAR );

    damage_old ( ch, victim, dam, sn, DAM_MENTAL, TRUE );
    victim->mana -= ( dam / 4 );
    return;
}

MAGIC ( spell_sonic_blast )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = number_range ( 20, 50 );
    if ( saves_spell ( level, victim, DAM_SOUND ) )
        dam /= 1.5;
    if ( victim->in_room == ch->in_room )
    {
        act ( "$n emits a screeching blast at $N!", ch, NULL, victim, TO_NOTVICT );
        act ( "$n directs a sonic blast at you!", ch, NULL, victim, TO_VICT );
        act ( "You channel a loud sonic blast at $N!", ch, NULL, victim, TO_CHAR );
        damage ( ch, victim, dam, sn, DAM_SOUND, TRUE );
    }
    return;
}

MAGIC ( spell_death_chant )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    char buf[MSL];

    if ( victim->in_room == ch->in_room )
    {
        act ( "$n chants dark words, eyes grimly focused on $N.", ch, NULL, victim, TO_NOTVICT );
        act ( "$n chants dark words, eyes grimly focused on you!", ch, NULL, victim, TO_VICT );
        act ( "You begin your death chant, focusing your dark powers on $N.", ch, NULL, victim, TO_CHAR );
        damage ( ch, victim, dice ( level * 5, 20 ), sn, DAM_SOUND, TRUE );
        damage ( ch, victim, dice ( level * 5, 21 ), sn, DAM_NEGATIVE, TRUE );

        if ( !IS_SET ( victim->imm_flags, IMM_SOUND ) )
        {   // If my understanding is correct, this should be %0.25
            if ( number_range ( 1, 400 ) == number_range ( 1, 400 )  )
            {
                    act ( "$N's eyes roll back as $S lifeless body falls to the ground.\n\r", ch, NULL, victim, TO_NOTVICT );
                    act ( "You are pulled in by $n's death chant.  Everything goes dark...\n\r", ch, NULL, victim, TO_VICT );
                    act ( "$N collapses to the ground, surrending to your death chant.", ch, NULL, victim, TO_CHAR );
                    if ( !IS_NPC ( victim ) )
                    {
                        sprintf ( buf, "{w[{RPK{w] {R%s {xhas {Yfallen {xto {R%s's {Ddeath magic{x {wat {Y%s{w!{x",
                                 victim->name, ch->name, ch->in_room->name );
                        do_gmessage ( buf );
                    }
                    raw_kill ( victim, ch );
            }
            else
            {
                act ( "$n death chant fails.", ch, NULL, victim, TO_ROOM );
                send_to_char ( "Your death chant fails.\n\r", ch );
            }
        }
    }

    return;
}

MAGIC ( spell_terror )
{
    CHAR_DATA *victim;
    AFFECT_DATA af;

    victim = ( CHAR_DATA * ) vo;

    if ( IS_AFFECTED ( victim, AFF_TERROR ) ||
         saves_spell ( level, victim, DAM_MENTAL ) )
        return;
    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = ( level / 50 );
    af.location = APPLY_HITROLL;
    af.modifier = -1 * ( level / 6 );
    af.bitvector = AFF_TERROR;
    affect_to_char ( victim, &af );

    af.location = APPLY_SAVING_SPELL;
    af.modifier = level / 6;
    affect_to_char ( victim, &af );

    send_to_char ( "{DTerror{x consumes you!\n\r", victim );
    if ( ch != victim )
        act ( "$N is suddenly stricken with {Dterror{x!", ch, NULL, victim, TO_CHAR );

    return;
}

MAGIC ( spell_inspire )
{
    AFFECT_DATA af;
    CHAR_DATA *gch;

    for ( gch = ch->in_room->people; gch != NULL; gch = gch->next_in_room )
    {
        if ( !is_same_group ( gch, ch ) || IS_SHIELDED ( gch, SHD_INSPIRE ) )
            continue;
        act ( "$n is inspired by $N's song!", gch, NULL, ch, TO_ROOM );
        send_to_char ( "You are inspired and feel more prepared for battle.\n\r", gch );

        af.where = TO_SHIELDS;
        af.type = sn;
        af.level = level / 2;
        af.duration = level / 20;

        af.bitvector = SHD_INSPIRE;
        af.location = APPLY_AC;
        af.modifier = level;
        affect_to_char ( gch, &af );
        af.location = APPLY_HITROLL;
        af.modifier = level;
        affect_to_char ( gch, &af );
        af.location = APPLY_DAMROLL;
        af.modifier = level / 2;
        affect_to_char ( gch, &af );
        af.location = APPLY_SAVES;
        af.modifier = 0 - level / 4;
        affect_to_char ( gch, &af );


    }
    send_to_char ( "Ok.\n\r", ch );

    return;
}

MAGIC ( spell_lullaby )
{
    AFFECT_DATA af;
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int dam;

    dam = ch->level * ( number_range ( 1, 5 ) );

    send_to_char ( "You sing a soothing lullaby.\n\r", ch );
    act ( "$n sings a soothing lullaby.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( vch != ch && !is_safe_spell ( ch, vch, TRUE ) )
            {
                if ( saves_spell ( level, vch, DAM_CHARM ) )
                    dam /= 3;
                vch->move -= dam;
                if ( ( vch->position != POS_SLEEPING ) && ( number_range ( 1, 100 ) <= 10 ) )
                {
                    af.where = TO_AFFECTS;
                    af.type = sn;
                    af.level = level;
                    af.duration = 1;

                    af.location = APPLY_NONE;
                    af.modifier = 0;
                    af.bitvector = AFF_SLEEP;
                    affect_join ( vch, &af );

                    send_to_char ( "You are lulled to sleep.\n\r", vch );
                    act ( "$n drifts to sleep.", vch, NULL, NULL, TO_ROOM );
                    vch->position = POS_SLEEPING;
                 }
            }
            continue;
        }
    }

    return;
}

MAGIC ( spell_expeditious_retreat )
{
    AFFECT_DATA af;

    if ( IS_SHIELDED ( ch, SHD_RETREAT ) || IS_AFFECTED ( ch, AFF_TERROR ) )
    {
        send_to_char ( "You are already running as fast as you can.\n\r", ch );
        return;
    }

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = 1;
    af.location = APPLY_DEX;
    af.modifier = 3;
    af.bitvector = SHD_RETREAT;
    affect_to_char ( ch, &af );

    send_to_char ( "You are now able to run faster.\n\r", ch );
    return;
}

MAGIC ( spell_coldfire )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int cdam, fdam;

    cdam = dice ( ( level / 5 ), 25 );
    fdam = dice ( ( level / 5 ), 25 );

    if ( saves_spell ( level, victim, DAM_COLD ) )
        cdam /= 1.5;
    if ( saves_spell ( level, victim, DAM_FIRE ) )
        fdam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, cdam, sn, DAM_COLD, TRUE );
        damage ( ch, victim, fdam, sn, DAM_FIRE, TRUE );
    }
    return;
}

MAGIC ( spell_pandemonium )
{
    CHAR_DATA *ich;
    CHAR_DATA *ich_next;
    AFFECT_DATA af;
    int chance, mal;
    bool found;

    found = FALSE;

    act ( "$n performs a devastatingly {nc{=h{-a{po{dt{Oi{Ic{x song!", ch, NULL, NULL, TO_ROOM );
    send_to_char ( "You perform a devastatingly {nc{=h{-a{po{dt{Oi{Ic{x song!\n\r", ch );

    ich_next = NULL;
    for ( ich = ch->in_room->people; ich; ich = ich_next ) // Changed how we look for new players
    {
        ich_next = ich->next_in_room; // Next please

        if ( ich == ch ) // Dont hurt yourself!
            continue;

        found = TRUE; // There ARE people in the room other than me.
        chance = ( number_range ( 1, 5 ) );   // Randomize the second damage type.

        if ( chance == 1 )
        {
            act ( "Summoned {Wd{wa{Dgge{wr{Ws{x rain down upon $N!", ch, NULL, ich, TO_NOTVICT );
            act ( "Summoned {Wd{wa{Dgge{wr{Ws{x rain down upon $N!", ch, NULL, ich, TO_CHAR );
            send_to_char ( "Summoned {Wd{wa{Dgge{wr{Ws{x rain down upon you!\n\r", ich );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_SOUND, TRUE );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_PIERCE, TRUE );
        }
        else if ( chance == 2 ) // else if is faster and eliminates danger of mis-execution.
        {
            act ( "A stream of {qa{qc{qi{qd{x pours down on $N!", ch, NULL, ich, TO_NOTVICT );
            act ( "A stream of {qa{qc{qi{qd{x pours down on $N!", ch, NULL, ich, TO_CHAR );
            send_to_char ( "A stream of {qa{qc{qi{qd{x pours down on you!\n\r", ich );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_SOUND, TRUE );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_ACID, TRUE );
        }
        else if ( chance == 3 )
        {
            act ( "{pL{pi{pg{ph{pt{pn{pi{pn{pg{x strikes $N!", ch, NULL, ich, TO_NOTVICT );
            act ( "{pL{pi{pg{ph{pt{pn{pi{pn{pg{x strikes $N!", ch, NULL, ich, TO_CHAR );
            send_to_char ( "{pL{pi{pg{ph{pt{pn{pi{pn{pg{x strikes you!\n\r", ich );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_SOUND, TRUE );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_LIGHTNING, TRUE );
        }
        else if ( chance == 4 )
        {
            act ( "$N is hit by a ball of {ce{Mn{ce{Cr{cg{Gy{x!", ch, NULL, ich, TO_NOTVICT );
            act ( "$N is hit by a ball of {ce{Mn{ce{Cr{cg{Gy{x!", ch, NULL, ich, TO_CHAR );
            send_to_char ( "You are hit by a ball of {ce{Mn{ce{Cr{cg{Gy{x!\n\r", ich );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_SOUND, TRUE );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_ENERGY, TRUE );
        }
        else if ( chance == 5 )
        {
            act ( "$N seems to explode with {ol{oi{og{oh{ot{x!", ch, NULL, ich, TO_NOTVICT );
            act ( "$N seems to explode with {ol{oi{og{oh{ot{x!", ch, NULL, ich, TO_CHAR );
            send_to_char ( "{oL{oi{og{oh{ot{x explodes within you!\n\r", ich );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_SOUND, TRUE );
            damage ( ch, ich, ( dice ( ch->level * 4, 37 ) ), sn, DAM_LIGHT, TRUE );
        }

        mal = get_curr_stat ( ch, STAT_WIS );

        if ( saves_spell ( ch->level, ich, DAM_CHARM ) && mal > 0 )
            mal /= 1.5;

        if ( number_range ( 1, 100 ) < mal )  // ( consider lowering.. dosn't happen very often =/ )
        {
            chance = ( number_range ( 1, 5 ) );   // Give them a random maladiction.

            if ( ( chance == 1 ) && ( !IS_AFFECTED ( ich, AFF_BLIND ) ) )
            {
                af.where = TO_AFFECTS;
                af.type = sn;
                af.level = ch->level;
                af.duration = 2;
                af.location = APPLY_HITROLL;
                af.modifier = 0 - ch->level;
                af.bitvector = AFF_BLIND;
                affect_to_char ( ich, &af );

                act ( "$N is blinded by the pandemonium!", ch, NULL, ich, TO_NOTVICT );
                act ( "$N is blinded by the pandemonium!", ch, NULL, ich, TO_CHAR );
                send_to_char ( "You are suddenly blinded!", ich );
            }
            else if ( ( chance == 2 ) && ( !IS_AFFECTED ( ich, AFF_WEAKEN ) ) )
            {
                af.where = TO_AFFECTS;
                af.type = sn;
                af.level = ch->level;
                af.duration = 2;
                af.location = APPLY_STR;
                af.modifier = -8;
                af.bitvector = AFF_WEAKEN;
                affect_to_char ( ich, &af );

                act ( "$N seems weakened by the pandemonium.", ch, NULL, ich, TO_NOTVICT );
                act ( "$N seems weakened by the pandemonium.", ch, NULL, ich, TO_CHAR );
                send_to_char ( "You suddenly feel weaker.", ich );
            }
            else if ( ( chance == 3 ) && ( !IS_AFFECTED ( ich, AFF_SLOW ) ) )
            {
                af.where = TO_AFFECTS;
                af.type = sn;
                af.level = ch->level;
                af.duration = 2;
                af.location = APPLY_DEX;
                af.modifier = -8;
                af.bitvector = AFF_SLOW;
                affect_to_char ( ich, &af );

                act ( "$N is slowed by the pandemonium.", ch, NULL, ich, TO_NOTVICT );
                act ( "$N is slowed by the pandemonium.", ch, NULL, ich, TO_ROOM );
                send_to_char ( "You are slowed by the pandemonium.", ich );
            }
            else if ( ( chance == 4 ) && ( !IS_SHIELDED ( ich, SHD_SILENCE ) ) )
            {
                af.where = TO_SHIELDS;
                af.type = sn;
                af.level = ch->level;
                af.duration = 2;
                af.location = APPLY_NONE;
                af.modifier = 0;
                af.bitvector = SHD_SILENCE;
                affect_to_char ( ich, &af );

                act ( "$N is silenced by the pandemonium.", ch, NULL, ich, TO_NOTVICT );
                act ( "$N is silenced by the pandemonium.", ch, NULL, ich, TO_ROOM );
                send_to_char ( "You are silenced by the pandemonium.", ich );
            }
            else if ( ( chance == 5 ) && ( !IS_AFFECTED ( ich, AFF_FAERIE_FIRE ) ) )
            {
                af.where = TO_AFFECTS;
                af.type = sn;
                af.level = ch->level;
                af.duration = 2;
                af.location = APPLY_DEX;
                af.modifier = level * 3;
                af.bitvector = AFF_FAERIE_FIRE;
                affect_to_char ( ich, &af );

                act ( "$N is surrounded by a {Mpink{x aura.", ch, NULL, ich, TO_NOTVICT );
                act ( "$N is surrounded by a {Mpink{x aura.", ch, NULL, ich, TO_ROOM );
                send_to_char ( "You are surrounded by a {Mpink{x aura.", ich );
            }
            else
            {
                act ( "$n shrugs off the affects of the pandemonium.", ch, NULL, ich, TO_NOTVICT );
                act ( "$N shrugs off the affects of the pandemonium.", ch, NULL, ich, TO_CHAR );
                send_to_char ( "You shrug off the chaotic affects of the pandemonium.", ich );
            }
        }
        continue; // Next!
    }

    if ( !found ) // Nobody around..
        send_to_char ("You cease playing and hear the sound of crickets chirping..\n\r", ch );

    return; // This is a pretty cool spell! :) Nice coding too!
}

MAGIC ( spell_silence )
{        
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    
    if ( IS_SHIELDED ( victim, SHD_SILENCE ) ||
         saves_spell ( level, victim, DAM_OTHER ) )
        return;
    
    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level / 45;
    af.bitvector = SHD_SILENCE;
    af.location = APPLY_NONE;
    af.modifier = 0;
    affect_to_char ( victim, &af );
    send_to_char ( "You have been silenced!\n\r", victim );
    act ( "$n appears unable to speak.", victim, NULL, NULL, TO_ROOM );
    return;
}
 
MAGIC ( spell_whisper )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( ( 7 + level ) / 2, 10 );
    if ( saves_spell ( level, victim, DAM_CHARM ) )
        dam /= 1.5;
    damage_old ( ch, victim, ( dam + dice ( 5, 4 ) ), sn, DAM_SOUND, TRUE );
    damage_old ( ch, victim, dam, sn, DAM_CHARM, TRUE );
    return;
}

MAGIC ( spell_gust )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, chance;

    dam = dice ( ( 13 + level ) / 2, 10 );
    if ( saves_spell ( level, victim, DAM_LIGHTNING ) )
        dam /= 1.5;
    damage_old ( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );

    chance = 5 + ( get_curr_stat ( ch, STAT_DEX ) - get_curr_stat ( victim, STAT_DEX ) );
    if ( number_percent ( ) <= chance )
    {
        act ( "You are blown backwards by $n's {Wgust{x!", ch, NULL, victim, TO_VICT );
        act ( "$N is blown backwards by your {Wgust{x!", ch, NULL, victim, TO_CHAR );
        act ( "$N is blown backwards by $n's {Wgust{x!", ch, NULL, victim, TO_NOTVICT );
        DAZE_STATE ( victim, 2 * PULSE_VIOLENCE );
        victim->position = POS_RESTING;
    }
    return;
}

MAGIC ( spell_acid_arrow )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int pdam, adam;

    pdam = dice ( ( level / 5 ), 25 );
    adam = dice ( ( level / 5 ), 25 );

    if ( saves_spell ( level, victim, DAM_PIERCE ) )
        pdam /= 1.5;
    if ( saves_spell ( level, victim, DAM_ACID ) )
        adam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, pdam, sn, DAM_PIERCE, TRUE );
        damage ( ch, victim, adam, sn, DAM_ACID, TRUE );
    }
    return;
}

MAGIC ( spell_rumors )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    send_to_char ( "You begin to spread some nasty rumors.\n\r", ch );
    act ( "$n begins spreading nasty rumors.\n\r", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( vch != ch && !is_safe_spell ( ch, vch, TRUE ) )
            {
                if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
                     ( !IS_NPC ( vch ) ) )
                {
                    ch->attacker = TRUE;
                    vch->attacker = FALSE;
                }
                damage_old ( ch, vch, level + dice ( level, 3 ), sn, DAM_SOUND, TRUE );
                damage_old ( ch, vch, level + dice ( level, 3 ), sn, DAM_MENTAL, TRUE );
            }
            continue;
        }
    }
    return;
}

MAGIC ( spell_drowning_pool )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int mdam, wdam;

    mdam = dice ( ( level / 2.5 ), 23 );
    wdam = dice ( ( level / 2.5 ), 23 );

    if ( saves_spell ( level, victim, DAM_MENTAL ) )
        mdam /= 1.5;
    if ( saves_spell ( level, victim, DAM_WATER ) )
        wdam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, mdam, sn, DAM_MENTAL, TRUE );
        damage ( ch, victim, wdam, sn, DAM_WATER, TRUE );
    }
    return;
}

MAGIC ( spell_shatter )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;

    if ( victim->in_room == ch->in_room )
    {
        damage_old ( ch, victim, dice ( level, 10 ) + level, sn, DAM_SOUND, TRUE );
        cold_effect ( victim, level, dice ( level, 10 ) + level, TARGET_CHAR );
    }
    return;
}

MAGIC ( spell_energy_bolt )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;

    if ( victim->in_room == ch->in_room )
        damage_old ( ch, victim, dice ( level * 4, 5 ), sn, DAM_ENERGY, TRUE );

    return;
}

MAGIC ( spell_blight )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;

    if ( victim->in_room == ch->in_room )
    {
        damage_old ( ch, victim, dice ( level + 20, 7 ), sn, DAM_COLD, TRUE );
        damage_old ( ch, victim, dice ( level + 20, 7 ), sn, DAM_NEGATIVE, TRUE );
    }

    if ( saves_spell ( level, victim, DAM_POISON ) )
    {
        act ( "$n turns slightly green, but it passes.", victim, NULL, NULL, TO_ROOM );
        send_to_char ( "You feel momentarily ill, but it passes.\n\r", victim );
        return;
    }

    af.where = TO_AFFECTS;
    af.type = gsn_poison;
    af.level = level;
    af.duration = level;
    af.location = APPLY_STR;
    af.modifier = -2;
    af.bitvector = AFF_POISON;
    affect_join ( victim, &af );
    send_to_char ( "You feel very sick.\n\r", victim );
    act ( "$n looks very ill.", victim, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_nightmare )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;

    if ( victim->in_room == ch->in_room )
    {
        act ( "$n fills your head with {Dhorrific visions{x!", ch, NULL, victim, TO_VICT );
        act ( "You send {Dhorrific visions{x into $N's mind!", ch, NULL, victim, TO_CHAR );
        damage_old ( ch, victim, dice ( level + 13, 8 ) + dice ( 3, level ), sn, DAM_NEGATIVE, TRUE );
        damage_old ( ch, victim, dice ( level + 13, 8 ), sn, DAM_MENTAL, TRUE );
    }

    if ( !saves_spell ( level, victim, DAM_MENTAL ) && 
          number_percent ( ) < ( 13 + get_curr_stat ( ch, STAT_INT ) - get_curr_stat ( victim, STAT_WIS ) ) )
    {
        act ( "$n tries to flee in terror!", victim, NULL, NULL, TO_ROOM );
        send_to_char ( "You try to flee in terror!", victim );
        do_flee ( victim, "" );
    }

    return;
}

MAGIC ( spell_hymn )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level + 13, 8 );
    if ( saves_spell ( level, victim, DAM_HOLY ) )
        dam /= 1.5;
    act ( "$n sings a {og{ol{oo{or{oi{oo{ou{os{x hymn!", ch, NULL, victim, TO_ROOM );
    act ( "You sing a {og{ol{oo{or{oi{oo{ou{os{x hymn!", ch, NULL, victim, TO_CHAR );
    damage_old ( ch, victim, dam + dice ( 3, level ), sn, DAM_HOLY, TRUE );
    damage_old ( ch, victim, dam, sn, DAM_SOUND, TRUE );
    return;
}

MAGIC ( spell_ray_of_frost )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level + 13, 8 );
    if ( saves_spell ( level, victim, DAM_COLD ) )
        dam /= 1.5;
    damage_old ( ch, victim, dam + dice ( 3, level ), sn, DAM_COLD, TRUE );
    damage_old ( ch, victim, dam, sn, DAM_COLD, TRUE );
    damage_old ( ch, victim, dam / 2 , sn, DAM_COLD, TRUE );
    damage_old ( ch, victim, dam / 5, sn, DAM_COLD, TRUE );
    return;
}

MAGIC ( spell_hallowed_ground )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level * 5, 5 );
    if ( saves_spell ( level, victim, DAM_EARTH ) && saves_spell ( level, victim, DAM_HOLY ) )
        dam /= 1.5;
    damage_old ( ch, victim, ( dam / 2 ) + dice ( 8, 5 ), sn, DAM_HOLY, TRUE );
    damage_old ( ch, victim, ( dam / 2 ) + dice ( 8, 5 ), sn, DAM_EARTH, TRUE );
    return;
}

MAGIC ( spell_searing_light )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level * 5, 5 );
    if ( saves_spell ( level, victim, DAM_LIGHT ) )
        dam /= 1.5;
    else
        spell_blindness ( skill_lookup ( "blindness" ), level / 2, ch, ( void * ) victim, TARGET_CHAR );

    damage_old ( ch, victim, ( dam / 2 ) + dice ( 8, 5 ), sn, DAM_LIGHT, TRUE );
    return;
}

MAGIC ( spell_early_grave )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int mdam, edam;

    mdam = dice ( ( level / 4 ), 50 );
    edam = dice ( ( level / 4 ), 50 );

    if ( saves_spell ( level, victim, DAM_MENTAL ) )
        mdam /= 1.5;
    if ( saves_spell ( level, victim, DAM_EARTH ) )
        edam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, mdam, sn, DAM_MENTAL, TRUE );
        damage ( ch, victim, edam, sn, DAM_EARTH, TRUE );
    }
    return;
}


MAGIC ( spell_raindance )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    send_to_char ( "Storm clouds begin to gather!\n\r", ch );
    act ( "$n begins to chant as $e performs a ritualistic dance.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( vch != ch && !is_safe_spell ( ch, vch, TRUE ) )
            {
                if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
                     ( !IS_NPC ( vch ) ) )
                {
                    ch->attacker = TRUE;
                    vch->attacker = FALSE;
                }
                damage_old ( ch, vch, level + dice ( level * 4, 5 ), sn, DAM_WATER, TRUE );
            }
            continue;
        }

        if ( vch->in_room->area == ch->in_room->area ) // && vch->in_room ***NOT INDOORS***)
            send_to_char ( "The sky darkens as storm clouds begin to gather.\n\r", vch );
    }

    return;
}

MAGIC ( spell_cloudkill )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    AFFECT_DATA af;

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( vch != ch && !is_safe_spell ( ch, vch, TRUE ) )
            {
                if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) && ( !IS_NPC ( vch ) ) )
                {
                    ch->attacker = TRUE;
                    vch->attacker = FALSE;
                }
                damage_old ( ch, vch, level + dice ( level * 2, 5 ), sn, DAM_HARM, TRUE );
                damage_old ( ch, vch, level + dice ( level * 2, 5 ), sn, DAM_POISON, TRUE );

                if ( !IS_AFFECTED ( vch, AFF_POISON ) && !saves_spell ( level, vch, DAM_POISON )
                    && number_range ( 1, 35 ) > get_curr_stat ( vch, STAT_CON ) )
                {
                    af.where = TO_AFFECTS;
                    af.type = gsn_poison;
                    af.level = level;
                    af.duration = level;
                    af.location = APPLY_STR;
                    af.modifier = -2;
                    af.bitvector = AFF_POISON;
                    affect_join ( vch, &af );
                    send_to_char ( "You feel very sick.\n\r", vch );
                    act ( "$n looks very ill.", vch, NULL, NULL, TO_ROOM );
                }
            }
            continue;
        }

        if ( vch->in_room->area == ch->in_room->area )
            send_to_char ( "Foul smelling fumes pass through the area.\n\r", vch );
    }
    return;
}

MAGIC ( spell_icestorm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, i;

    act ( "{WS{wh{War{wd{Ws {gof {-i{-c{-e r{-a{-i{-n {gcalled by $n shower down upon {R$N{g!{x", ch, NULL, victim, TO_NOTVICT );
    act ( "{WS{wh{War{wd{Ws {gof {-i{-c{-e r{-a{-i{-n {gcalled by $n shower down upon {Ryou{g!{x", ch, NULL, victim, TO_VICT );
    act ( "{WS{wh{War{wd{Ws {gof {-i{-c{-e r{-a{-i{-n {gshower down upon {R$N{g!{x", ch, NULL, victim, TO_CHAR );
    i = 4;
    while ( i > 0 )
    {
        dam = dice ( level, level / 10 );
        if ( saves_spell ( level, victim, DAM_COLD ) )
            dam /= 1.5;
            damage ( ch, victim, dam, sn, DAM_COLD, TRUE );
            damage ( ch, victim, dam, sn, DAM_PIERCE, TRUE );
        i--;
    }
    return;
}

MAGIC ( spell_deceit )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int mdam, cdam;

    mdam = dice ( level, 20 );
    cdam = dice ( level, 50 );

    if ( saves_spell ( level, victim, DAM_MENTAL ) )
        mdam /= 1.5;
    if ( saves_spell ( level, victim, DAM_COLD ) )
        cdam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, mdam, sn, DAM_MENTAL, TRUE );
        damage ( ch, victim, cdam, sn, DAM_COLD, TRUE );
    }
    return;
}

MAGIC ( spell_astral_blast )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int mdam, hdam;

    mdam = dice ( level, 20 );
    hdam = dice ( level, 20 );

    if ( saves_spell ( level, victim, DAM_MENTAL ) )
        mdam /= 1.5;
    if ( saves_spell ( level, victim, DAM_HOLY ) )
        hdam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, mdam, sn, DAM_MENTAL, TRUE );
        damage ( ch, victim, hdam, sn, DAM_HOLY, TRUE );
    }
    return;
}

MAGIC ( spell_desecrate )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    send_to_char ( "You desecrate the area.\n\r", ch );
    act ( "$n {Ddesecrates{x the area!", ch, NULL, NULL, TO_ROOM );

    if ( IS_GOOD ( ch ) )
    {
         send_to_char ( "The {Ddarkness{x burns within you!\n\r", ch );
         damage_old ( ch, ch, level + dice ( level * 2, 5 ), sn, DAM_NEGATIVE, TRUE );
         damage_old ( ch, ch, level + dice ( level * 2, 5 ), sn, DAM_ENERGY, TRUE );
    }

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( IS_EVIL ( vch ) )
            {
                send_to_char ( "You feel empowered by the darkness!\n\r", vch );
                spell_frenzy ( skill_lookup ( "frenzy" ), level, ch, ( void * ) vch, TARGET_CHAR );
                spell_protection_good ( skill_lookup ( "protection good" ), level, ch, ( void * ) vch, TARGET_CHAR );
            }
            else if ( vch != ch && !is_safe_spell ( ch, vch, TRUE ) )
            {
                if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
                     ( !IS_NPC ( vch ) ) )
                {
                    ch->attacker = TRUE;
                    vch->attacker = FALSE;
                }
                damage_old ( ch, vch, level + dice ( level * 2, 7 ), sn, DAM_NEGATIVE, TRUE );
                damage_old ( ch, vch, level + dice ( level * 2, 7 ), sn, DAM_ENERGY, TRUE );
            }
            continue;
        }
    }
    return;
}

MAGIC ( spell_consecrate )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    send_to_char ( "You consecrate the area.\n\r", ch );
    act ( "$n {Wconsecrates{x the area!", ch, NULL, NULL, TO_ROOM );

    if ( IS_EVIL ( ch ) )
    {
         send_to_char ( "The {Wlight{x burns within you!\n\r", ch );
         damage_old ( ch, ch, level + dice ( level * 2, 5 ), sn, DAM_HOLY, TRUE );
         damage_old ( ch, ch, level + dice ( level * 2, 5 ), sn, DAM_ENERGY, TRUE );
    }

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( IS_GOOD ( vch ) )
            {
                send_to_char ( "You are protected by the light!\n\r", vch );
                spell_protection_evil ( skill_lookup ( "protection evil" ), level, ch, ( void * ) vch, TARGET_CHAR );
                spell_bless ( skill_lookup ( "bless" ), level, ch, ( void * ) vch, TARGET_CHAR );
            }
            else if ( vch != ch && !is_safe_spell ( ch, vch, TRUE ) )
            {
                if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
                     ( !IS_NPC ( vch ) ) )
                {
                    ch->attacker = TRUE;
                    vch->attacker = FALSE;
                }
                damage_old ( ch, vch, level + dice ( level * 2, 7 ), sn, DAM_HOLY, TRUE );
                damage_old ( ch, vch, level + dice ( level * 2, 7 ), sn, DAM_ENERGY, TRUE );
            }
            continue;
        }
    }
    return;
}

MAGIC ( spell_venom )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int dam;

    dam = dice ( level * 2, 20 );

    if ( saves_spell ( level, victim, DAM_ACID ) )
        dam /= 1.5;

    if ( !IS_AFFECTED ( victim, AFF_POISON ) && !saves_spell ( level, victim, DAM_POISON )
        && number_range ( 1, 40 ) > get_curr_stat ( victim, STAT_CON ) )
    {
        af.where = TO_AFFECTS;
        af.type = gsn_poison;
        af.level = level;
        af.duration = level;
        af.location = APPLY_STR;
        af.modifier = -2;
        af.bitvector = AFF_POISON;
        affect_join ( victim, &af );
        send_to_char ( "You feel very sick.\n\r", victim );
        act ( "$n looks very ill.", victim, NULL, NULL, TO_ROOM );
     }

    if ( victim->in_room == ch->in_room )
        damage ( ch, victim, dam, sn, DAM_ACID, TRUE );

    return;
}

MAGIC ( spell_holocaust )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    send_to_char ( "You call upon death itself to fell your enemies.\n\r", ch );
    act ( "The stench of decay fills the air as $n channels death itself.", ch, NULL, NULL, TO_ROOM );

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( vch != ch && !is_safe_spell ( ch, vch, TRUE ) )
            {
                if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
                     ( !IS_NPC ( vch ) ) )
                {
                    ch->attacker = TRUE;
                    vch->attacker = FALSE;
                }
                damage_old ( ch, vch, level + dice ( level * 4, 7 ), sn, DAM_NEGATIVE, TRUE );
                if ( !saves_spell ( level, vch, DAM_MENTAL ) && number_range ( 1, 35 ) > get_curr_stat ( vch, STAT_WIS ) )
                spell_curse ( skill_lookup ( "curse" ), level, ch, ( void * ) vch, TARGET_CHAR );
            }
            continue;
        }
    }

    return;
}

MAGIC ( spell_sleet )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int cdam, wdam;

    cdam = dice ( ( level + 10 ), 25 );
    wdam = dice ( ( level + 10 ), 25 );

    if ( saves_spell ( level, victim, DAM_COLD ) )
        cdam /= 1.5;
    if ( saves_spell ( level, victim, DAM_WATER ) )
        wdam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, cdam, sn, DAM_COLD, TRUE );
        damage ( ch, victim, wdam, sn, DAM_WATER, TRUE );
    }
    return;
}

MAGIC ( spell_disrupt )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, chance;

    dam = dice ( ( level + 10 ), 24 );
    chance = 15 + ( get_curr_stat ( ch, STAT_INT ) - get_curr_stat ( victim, STAT_INT ) );

    if ( saves_spell ( level, victim, DAM_HARM ) && saves_spell ( level, victim, DAM_ENERGY ) )
    {
        dam /= 1.5;
        chance -= 5;
    }

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, dam + dice ( 12, 6 ), sn, DAM_HARM, TRUE );
        damage ( ch, victim, dam, sn, DAM_ENERGY, TRUE );
    }

    if ( number_percent ( ) <= chance )
    {
        act ( "You are sent staggering by $n's {cr{ge{Gp{Bu{bl{Bs{Gi{go{cn{x!", ch, NULL, victim, TO_VICT );
        act ( "$N is sent staggering by your {cr{ge{Gp{Bu{bl{Bs{Gi{go{cn{x!", ch, NULL, victim, TO_CHAR );
        act ( "$N is sent staggering by $n's {cr{ge{Gp{Bu{bl{Bs{Gi{go{cn{x!", ch, NULL, victim, TO_NOTVICT );
        victim->stunned = 3;
    }
    return;
}

MAGIC ( spell_laughter )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int ldam, sdam;

    ldam = dice ( ( level + 10 ), 25 );
    sdam = dice ( ( level + 10 ), 25 );

    if ( saves_spell ( level, victim, DAM_LIGHT ) )
        ldam /= 1.5;
    if ( saves_spell ( level, victim, DAM_SOUND ) )
        sdam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, ldam, sn, DAM_LIGHT, TRUE );
        damage ( ch, victim, sdam, sn, DAM_SOUND, TRUE );
    }
    return;
}

MAGIC ( spell_earthsong )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level, 33 );
    if ( saves_spell ( level, victim, DAM_EARTH ) && saves_spell ( level, victim, DAM_SOUND ) )
        dam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage_old ( ch, victim, dam + dice ( 12, 6 ), sn, DAM_EARTH, TRUE );
        damage_old ( ch, victim, dam, sn, DAM_SOUND, TRUE );
    }
    return;
}

MAGIC ( spell_repulse )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, chance;

    dam = dice ( level, 30 ) + level;
    chance = 20 + ( get_curr_stat ( ch, STAT_INT ) - get_curr_stat ( victim, STAT_INT ) );

    if ( saves_spell ( level, victim, DAM_NEGATIVE ) && saves_spell ( level, victim, DAM_ENERGY ) )
    {
        dam /= 1.5;
        chance -= 5;
    }

    if ( victim->in_room == ch->in_room )
    {
        damage ( ch, victim, dam + dice ( 12, 6 ), sn, DAM_HARM, TRUE );
        damage ( ch, victim, dam, sn, DAM_ENERGY, TRUE );
    }

    if ( number_percent ( ) <= chance )
    {
        act ( "You are sent staggering by $n's {Dr{me{Mp{Bu{bl{Bs{Mi{mo{Dn{x!", ch, NULL, victim, TO_VICT );
        act ( "$N is sent staggering by your {Dr{me{Mp{Bu{bl{Bs{Mi{mo{Dn{x!", ch, NULL, victim, TO_CHAR );
        act ( "$N is sent staggering by $n's {Dr{me{Mp{Bu{bl{Bs{Mi{mo{Dn{x!", ch, NULL, victim, TO_NOTVICT );
        DAZE_STATE ( victim, 3 * PULSE_VIOLENCE );
        victim->position = POS_RESTING;
    }
    return;
}

MAGIC ( spell_corrode )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level * 2, 45 );
    if ( saves_spell ( level, victim, DAM_ACID ) )
        dam /= 1.5;

    if ( victim->in_room == ch->in_room )
    {
        damage_old ( ch, victim, dam, sn, DAM_ACID, TRUE );
        acid_effect ( victim, level, dam, TARGET_CHAR );
    }
    return;
}

MAGIC ( spell_cyclone )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, chance;

    dam = dice ( level * 2, 45 );
    chance = 20 + ( get_curr_stat ( ch, STAT_INT ) - get_curr_stat ( victim, STAT_DEX ) );
    if ( saves_spell ( level, victim, DAM_LIGHTNING ) )
    {
        dam /= 1.5;
        chance -= 5;
    }

    damage_old ( ch, victim, dam, sn, DAM_LIGHTNING, TRUE );

    if ( number_percent ( ) <= chance )
    {
        act ( "You are blown to the ground by $n's {Wcyclone{x!", ch, NULL, victim, TO_VICT );
        act ( "$N is blown to the ground by your {Wcyclone{x!", ch, NULL, victim, TO_CHAR );
        act ( "$N is blown to the ground by $n's {Wcyclone{x!", ch, NULL, victim, TO_NOTVICT );
        DAZE_STATE ( victim, 2 * PULSE_VIOLENCE );
        victim->position = POS_RESTING;
        if ( IS_AFFECTED ( victim, AFF_FLYING ) )
            affect_strip ( victim, AFF_FLYING );
    }
    return;
}

MAGIC ( spell_death_knell )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int dam;

    if ( victim->hit > ( victim->max_hit / 6 ) )
    {
        act ( "$N is not near enough to death...", ch, NULL, victim, TO_CHAR );
        return;
    }

    act ( "$n draws the last of your life from your body...", ch, NULL, victim, TO_VICT );
    act ( "You drain the last of $N's life from $S body and grow stronger!", ch, NULL, victim, TO_CHAR );
    act ( "$n drains the last of $N's life from $S body.", ch, NULL, victim, TO_NOTVICT );
    dam = victim->hit;
    damage_old ( ch, victim, dam * 10, sn, DAM_NEGATIVE, TRUE );
    ch->hit += dam;
   
    if ( IS_AFFECTED ( ch, sn ) )
        return;

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = dam / 100;
    af.location = APPLY_STR;
    af.modifier = 2;
    af.bitvector = 0;
    affect_to_char ( ch, &af );

    af.where = TO_AFFECTS;
    af.type = sn;
    af.level = level;
    af.duration = dam / 100;
    af.location = APPLY_LEVEL;
    af.modifier = victim->level / 40;
    af.bitvector = 0;
    affect_to_char ( ch, &af );
    send_to_char ( "You feel stronger!", ch );
    return;
}

MAGIC ( spell_hemorrhage )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int dam, chance;

    dam = dice ( level * 2, 20 );
    chance = level / 2;
    if ( saves_spell ( level, victim, DAM_DISEASE ) )
    {
        dam /= 1.5;
        chance /= 2;
    }

    damage_old ( ch, victim, dam + dice ( level, 3 ), sn, DAM_DISEASE, TRUE );
    damage_old ( ch, victim, dam, sn, DAM_HARM, TRUE );

    if ( !IS_SHIELDED ( victim, SHD_HEMORRHAGE ) && ( number_percent ( ) <= chance ) )
    {
        af.where = TO_SHIELDS;
        af.type = sn;
        af.level = level;
        af.duration = level / 25;
        af.location = APPLY_CON;
        af.modifier = level / 40;
        af.bitvector = SHD_HEMORRHAGE;
        affect_to_char ( victim, &af );   
    }
    return;
}

MAGIC ( spell_unearthly_beauty )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int dam, chance;

    dam = dice ( level * 2, 20 );
    chance = level / 3;
    if ( saves_spell ( level, victim, DAM_CHARM ) )
    {
        dam /= 1.5;
        chance /= 2;
    }

    damage_old ( ch, victim, dam + dice ( level, 3 ), sn, DAM_CHARM, TRUE );
    damage_old ( ch, victim, dam, sn, DAM_LIGHT, TRUE );

    if ( !IS_AFFECTED ( victim, AFF_BLIND ) && ( number_percent ( ) < chance ) )
    {
        af.where = TO_AFFECTS;
        af.type = gsn_blindness;
        af.level = level;
        af.duration = level / 40;
        af.location = APPLY_HITROLL;
        af.modifier = 0 - level;
        af.bitvector = AFF_BLIND;
        affect_to_char ( victim, &af );   
        act ( "You are blinded by $n's beauty!", ch, NULL, victim, TO_VICT );
        act ( "$N is blinded by your beauty!", ch, NULL, victim, TO_CHAR );
    }
    return;
}

MAGIC ( spell_freezing_sphere )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level * 2, 27 );
    if ( saves_spell ( level, victim, DAM_COLD ) )
        dam /= 1.5;

    damage_old ( ch, victim, dam + dice ( level, 3 ), sn, DAM_COLD, TRUE );
    damage_old ( ch, victim, dam, sn, DAM_WATER, TRUE );
    return;
}

MAGIC ( spell_serendipity )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level * 4, 27 );
    if ( saves_spell ( level, victim, DAM_HOLY ) )
        dam /= 1.5;

    damage_old ( ch, victim, dam, sn, DAM_HOLY, TRUE );
    ch->hit += dam / 25;
    return;
}

MAGIC ( spell_prismatic_spray )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, color, beams;

    dam = dice ( level * 6, 18 );
    color = number_range ( 1, 8 );
    beams = 1;

    if ( !saves_spell ( level * 1.5, victim, DAM_LIGHT ) )
        spell_blindness ( skill_lookup ( "blindness" ), level, victim, ( void * ) victim, TARGET_CHAR );

    while ( beams > 0 )
    {
        if ( color == 8 )
            beams = 3;
        if ( color == 7 )
        {
            act ( "$N is hit by a beam of {mviolet light{x and is sent away!", ch, NULL, victim, TO_NOTVICT );
            act ( "$N is hit by a beam of {mviolet light{x and is sent away!", ch, NULL, victim, TO_CHAR );
            act ( "You are hit by a beam of {mviolet light{x and are sent away!", ch, NULL, victim, TO_VICT );
            spell_teleport ( skill_lookup ( "teleport" ), level, victim, ( void * ) victim, TARGET_CHAR );
        }
        if ( color == 6 )
        {
            act ( "$N is hit by a beam of {bindigo light{x!", ch, NULL, victim, TO_NOTVICT );
            act ( "$N is hit by a beam of {bindigo light{x!", ch, NULL, victim, TO_CHAR );
            act ( "You feel drained as you are hit by a beam of {bindigo light{x!", ch, NULL, victim, TO_VICT );
            spell_energy_drain ( skill_lookup ( "energy drain" ), level, ch, ( void * ) victim, TARGET_CHAR );
            damage_old ( ch, victim, dam / 1.5, sn, DAM_LIGHT, TRUE );            
        }
        if ( color == 5 )
        {
            act ( "$N is hit by a beam of {Bblue light{x and $S skin begins to harden!", ch, NULL, victim, TO_NOTVICT );
            act ( "$N is hit by a beam of {Bblue light{x and $S skin begins to harden!", ch, NULL, victim, TO_CHAR );
            act ( "Your skin begins to harden as you are hit by a beam of {Bblue light{x!", ch, NULL, victim, TO_VICT );
            if ( number_percent ( ) > ( get_curr_stat ( victim, STAT_CON ) * 3 ) )
                victim->stunned = number_range ( 2, 4 );
            else
                send_to_char ( "You resist the petrifying effect of the light.\n\r", victim );
        }
        if ( color == 4 )
        {
            act ( "$N is hit by a beam of {Ggreen light{x and looks very ill.", ch, NULL, victim, TO_NOTVICT );
            act ( "$N is hit by a beam of {Ggreen light{x and looks very ill.", ch, NULL, victim, TO_CHAR );
            act ( "You feel sick as you are hit by a beam of {Ggreen light{x!", ch, NULL, victim, TO_VICT );
            if ( saves_spell ( level, victim, DAM_POISON ) )
                dam /= 2;
            damage_old ( ch, victim, dam, sn, DAM_POISON, TRUE );
            spell_poison ( skill_lookup ( "poison" ), level, ch, ( void * ) victim, TARGET_CHAR );
        }
        if ( color == 3 )
        {
            act ( "$N is hit by a beam of {Yyellow light{x!", ch, NULL, victim, TO_NOTVICT );
            act ( "$N is hit by a beam of {Yyellow light{x!", ch, NULL, victim, TO_CHAR );
            act ( "You are shocked by a beam of {Yyellow light{x!", ch, NULL, victim, TO_VICT );
            if ( saves_spell ( level, victim, DAM_ENERGY ) )
                dam /= 2;
            damage_old ( ch, victim, dam, sn, DAM_ENERGY, TRUE );
        }
        if ( color == 2 )
        {
            act ( "$N is hit by a beam of {yorange light{x!", ch, NULL, victim, TO_NOTVICT );
            act ( "$N is hit by a beam of {yorange light{x!", ch, NULL, victim, TO_CHAR );
            act ( "You are hit with acid from a beam of {yorange light{x!", ch, NULL, victim, TO_VICT );
            if ( saves_spell ( level, victim, DAM_ACID ) )
                dam /= 2;
            damage_old ( ch, victim, dam, sn, DAM_ACID, TRUE );
        }
        if ( color == 1 )
        {
            act ( "$N is hit by a beam of {Rred light{x!", ch, NULL, victim, TO_NOTVICT );
            act ( "$N is hit by a beam of {Rred light{x!", ch, NULL, victim, TO_CHAR );
            act ( "You are burned by a beam of {Rred light{x!", ch, NULL, victim, TO_VICT );
            if ( saves_spell ( level, victim, DAM_FIRE ) )
                dam /= 2;
            damage_old ( ch, victim, dam, sn, DAM_FIRE, TRUE );
        }
        beams--;
        color = number_range ( 1, 7 );
    }
    return;
}

MAGIC ( spell_sunburst )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int dam;

    dam = dice ( level * 6, 9 );
    if ( saves_spell ( level, victim, DAM_FIRE ) )
        dam /= 1.5;
    damage_old ( ch, victim, dam + dice ( 3, 10 ), sn, DAM_FIRE, TRUE );
    damage_old ( ch, victim, dam + dice ( 1, 30 ), sn, DAM_LIGHT, TRUE );

    if ( !IS_AFFECTED ( victim, AFF_BLIND ) && !saves_spell ( level, victim, DAM_LIGHT ) )
    {
        af.where = TO_AFFECTS;
        af.type = gsn_blindness;
        af.level = level;
        af.duration = level / 40;
        af.location = APPLY_HITROLL;
        af.modifier = 0 - level;
        af.bitvector = AFF_BLIND;
        affect_to_char ( victim, &af );   
        send_to_char ( "You are blinded by the light!\n\r", victim );
        act ( "$N is blinded by the light!", ch, NULL, victim, TO_CHAR );
    }
    return;
}

MAGIC ( spell_horrid_wilting )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level * 6, 9 );
    if ( !saves_spell ( level, victim, DAM_LIGHTNING ) )
        dam *= 1.5;
    damage_old ( ch, victim, dam + dice ( 3, 10 ), sn, DAM_NEGATIVE, TRUE );
    damage_old ( ch, victim, dam + dice ( 1, 30 ), sn, DAM_WATER, TRUE );
    return;
}

MAGIC ( spell_sound_burst )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level * 6, 6 );
    if ( saves_spell ( level, victim, DAM_SOUND ) )
        dam /= 1.5;
    damage_old ( ch, victim, dam + dice ( 3, 10 ), sn, DAM_SOUND, TRUE );
    damage_old ( ch, victim, dam + dice ( 2, 15 ), sn, DAM_HARM, TRUE );
    damage_old ( ch, victim, dam + dice ( 1, 30 ), sn, DAM_BASH, TRUE );

    if ( number_percent ( ) > get_curr_stat ( victim, STAT_DEX ) * 3 )
    {
        send_to_char ( "You are sent reeling!\n\r", victim );
        act ( "$N is stunned by your sound burst!\n\r", ch, NULL, victim, TO_CHAR );
        victim->stunned = number_range ( 1, 3 );
    }
    return;
}

MAGIC ( spell_elysium )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    dam = dice ( level * 5, 11 );
    if ( saves_spell ( level, victim, DAM_LIGHT ) )
        dam /= 1.5;

    act ( "You attempt to send $N into the paradise beyond death!", ch, NULL, victim, TO_CHAR );
    act ( "$n attempts to send you into the paradise beyond death!", ch, NULL, victim, TO_VICT );
    act ( "$n attempts to send $N into the paradise beyond death!", ch, NULL, victim, TO_NOTVICT );

    damage_old ( ch, victim, dam + dice ( 3, 10 ), sn, DAM_LIGHT, TRUE );
    damage_old ( ch, victim, dam + dice ( 2, 15 ), sn, DAM_EARTH, TRUE );
    return;
}

MAGIC ( spell_stinking_cloud )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    AFFECT_DATA af;

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( vch != ch && !is_safe_spell ( ch, vch, TRUE ) )
            {
                if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) && ( !IS_NPC ( vch ) ) )
                {
                    ch->attacker = TRUE;
                    vch->attacker = FALSE;
                }
                damage_old ( ch, vch, level + dice ( level, level / 5 ), sn, DAM_HARM, TRUE );
                damage_old ( ch, vch, level + dice ( level, level / 5 ), sn, DAM_DISEASE, TRUE );

                if ( !IS_AFFECTED ( vch, AFF_PLAGUE ) && !saves_spell ( level, vch, DAM_DISEASE )
                    && number_range ( 1, 35 ) > get_curr_stat ( vch, STAT_CON ) )
                {
                    af.where = TO_AFFECTS;
                    af.type = gsn_plague;
                    af.level = level;
                    af.duration = level;
                    af.location = APPLY_STR;
                    af.modifier = -5;
                    af.bitvector = AFF_PLAGUE;
                    affect_join ( vch, &af );
                    send_to_char ( "You scream in agony as plague sores erupt from your skin.\n\r", vch );
                    act ( "$n screams in agony as sores erupt all over $s skin.", vch, NULL, NULL, TO_ROOM );
                }
            }
            continue;
        }

        if ( vch->in_room->area == ch->in_room->area )
            send_to_char ( "Putrid fumes pass through the area.\n\r", vch );
    }
    return;
}

MAGIC ( spell_baptism )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    act ( "You baptize $N!", ch, NULL, victim, TO_CHAR );
    act ( "$n dunks you in the water and baptizes you!", ch, NULL, victim, TO_VICT );
    act ( "$n dunks $N in the water and baptizes $M!", ch, NULL, victim, TO_NOTVICT );

    if ( ( IS_EVIL ( ch ) && IS_EVIL ( victim ) ) || ( IS_NEUTRAL ( ch ) && IS_NEUTRAL ( victim ) )
          || ( IS_GOOD ( ch ) && IS_GOOD ( victim ) ) )
    {
        spell_bless ( skill_lookup ( "bless" ), level, ch, ( void * ) victim, TARGET_CHAR );
        send_to_char ( "You are blessed by the baptism!", victim );
        act ( "$N is blessed by the baptism!", ch, NULL, victim, TO_NOTVICT );
        act ( "$N is blessed by the baptism!", ch, NULL, victim, TO_CHAR );
        return;
    }

    dam = dice ( level * 5, 11 );
    if ( saves_spell ( level, victim, DAM_HOLY ) )
        dam /= 1.5;

    damage_old ( ch, victim, dam + dice ( 3, 10 ), sn, DAM_HOLY, TRUE );
    damage_old ( ch, victim, dam + dice ( 2, 15 ), sn, DAM_WATER, TRUE );
    return;
}

MAGIC ( spell_venomous_lies )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int dam;

    if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) &&
         ( !IS_NPC ( victim ) ) )
    {
        ch->attacker = TRUE;
        victim->attacker = FALSE;
    }

    dam = dice ( level * 2, 36 );
    if ( saves_spell ( level, victim, DAM_SOUND ) )
        dam /= 1.5;

    damage_old ( ch, victim, dam + dice ( 3, 10 ), sn, DAM_ACID, TRUE );
    damage_old ( ch, victim, dam + dice ( 2, 15 ), sn, DAM_SOUND, TRUE );

    if ( !IS_AFFECTED ( victim, AFF_POISON ) && !saves_spell ( level, victim, DAM_POISON ) 
        && number_range ( 1, 40 ) > get_curr_stat ( victim, STAT_CON ) )
    {
        af.where = TO_AFFECTS;
        af.type = gsn_poison;
        af.level = level;
        af.duration = level;
        af.location = APPLY_STR;
        af.modifier = -2;
        af.bitvector = AFF_POISON;
        affect_join ( victim, &af );
        send_to_char ( "You feel very sick.\n\r", victim );
        act ( "$n looks very ill.", victim, NULL, NULL, TO_ROOM );
    }

    return;
}

MAGIC ( spell_pollution )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    AFFECT_DATA af;
    int dam;

    act ( "You conjure a thick cloud of {Dpo{gl{Gl{Dut{Gi{go{yn{x to surround $N!", ch, NULL, victim, TO_CHAR );
    act ( "$n conjures a thick cloud of {Dpo{gl{Gl{Dut{Gi{go{yn{x to surround you!", ch, NULL, victim, TO_VICT );
    act ( "$n conjures a thick cloud of {Dpo{gl{Gl{Dut{Gi{go{yn{x to surround $N!", ch, NULL, victim, TO_NOTVICT );

    dam = dice ( level * 2, 36 );
    if ( saves_spell ( level, victim, DAM_NEGATIVE ) )
        dam /= 1.5;

    damage_old ( ch, victim, dam + dice ( 3, 10 ), sn, DAM_ACID, TRUE );
    damage_old ( ch, victim, dam + dice ( 2, 15 ), sn, DAM_NEGATIVE, TRUE );

    if ( !IS_AFFECTED ( victim, AFF_POISON ) && !saves_spell ( level, victim, DAM_POISON ) 
        && number_range ( 1, 40 ) > get_curr_stat ( victim, STAT_CON ) )
    {
        af.where = TO_AFFECTS;
        af.type = gsn_poison;
        af.level = level;
        af.duration = level;
        af.location = APPLY_STR;
        af.modifier = -2;
        af.bitvector = AFF_POISON;
        affect_join ( victim, &af );
        send_to_char ( "You feel very sick.\n\r", victim );
        act ( "$n looks very ill.", victim, NULL, NULL, TO_ROOM );
    }

    return;
}

MAGIC ( spell_phantasmal_killer )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    act ( "You dig into $N's mind and summon $S worst nightmare!", ch, NULL, victim, TO_CHAR );
    act ( "The most horrific being you could imagine appears before you and attacks!", ch, NULL, victim, TO_VICT );
    act ( "$N cries out in utter horror!", ch, NULL, victim, TO_NOTVICT );

    dam = dice ( level * 2, 40 );
    if ( saves_spell ( level, victim, DAM_MENTAL ) )
        dam /= 1.5;

    damage_old ( ch, victim, dam + dice ( 3, 10 ), sn, DAM_ENERGY, TRUE );
    damage_old ( ch, victim, dam + dice ( 2, 15 ), sn, DAM_NEGATIVE, TRUE );
    return;
}

MAGIC ( spell_wail_of_the_banshee )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    act ( "A horrible wailing fills the room.", ch, NULL, victim, TO_ROOM );

    dam = ( number_range ( level * 4, 2500 ) * 8 );
    if ( saves_spell ( level, victim, DAM_SOUND ) )
        dam /= 1.5;

    damage_old ( ch, victim, dam + number_range ( 11, 123 ), sn, DAM_HARM, TRUE );
    damage_old ( ch, victim, dam, sn, DAM_SOUND, TRUE );
    return;
}

MAGIC ( spell_vermin_swarm )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam;

    act ( "You summon a swarm of rats and insects to attack $N!", ch, NULL, victim, TO_CHAR );
    act ( "Rats and insects summoned by $n rapidly overtake you!", ch, NULL, victim, TO_VICT );
    act ( "$N is viciously attacked by a swarm of rats and insects summoned by $N.", ch, NULL, victim, TO_NOTVICT );

    dam = ( number_range ( level * 4, 2500 ) * 8 );
    if ( saves_spell ( level, victim, DAM_DISEASE ) )
        dam /= 1.5;

    damage_old ( ch, victim, dam + number_range ( 11, 123 ), sn, DAM_DISEASE, TRUE );
    damage_old ( ch, victim, dam, sn, DAM_HARM, TRUE );
    return;
}

MAGIC ( spell_incendiary_cloud )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    int chance, dam, hits;

    act ( "$n summons a cloud of flaming embers!", ch, NULL, NULL, TO_ROOM );

    chance = get_curr_stat ( ch, STAT_WIS ) * 3;
    hits = 0;

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
        vch_next = vch->next;
        if ( vch->in_room == NULL )
            continue;
        if ( vch->in_room == ch->in_room )
        {
            if ( vch != ch && !is_safe_spell ( ch, vch, TRUE ) )
            {
                if ( ( ch->fighting == NULL ) && ( !IS_NPC ( ch ) ) && ( !IS_NPC ( vch ) ) )
                {
                    ch->attacker = TRUE;
                    vch->attacker = FALSE;
                }
                dam = number_range ( level * 4, 2500 ) * 8;
                if ( saves_spell ( level, vch, DAM_FIRE ) )
                    dam /= 1.5;
                act ( "The {Oe{Om{Ob{Oe{Or{Os{x spread to engulf you!", ch, NULL, vch, TO_VICT );
                act ( "The {Oe{Om{Ob{Oe{Or{Os{x spread to engulf $N!", NULL, NULL, vch, TO_NOTVICT );
                damage_old ( ch, vch, dam + dice ( 4, 17), sn, DAM_FIRE, TRUE );
                damage_old ( ch, vch, dam, sn, DAM_HARM, TRUE );
                hits++;
            }
            if ( number_percent ( ) >= chance && hits > 0 )
                return;
            chance -= number_range ( 5, 25 );
            continue;
        }
    }
    return;
}

MAGIC ( spell_storm_of_vengeance )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int chance;

    chance = get_curr_stat ( ch, STAT_WIS ) + number_range ( 1, 10 );
    act ( "A large {Dstorm cloud{x forms above {W$N{x, thundering loudly.", ch, NULL, victim, TO_NOTVICT );
    act ( "A large {Dstorm cloud{x forms above {W$N{x, thundering loudly.", ch, NULL, victim, TO_CHAR );
    send_to_char ( "A large {Dstorm cloud{x forms above you, thundering loudly.\n\r", victim );
    damage ( ch, victim, dice ( level * 4, 22 ), sn, DAM_SOUND, TRUE );
    if ( chance < 26 )
        return; 
    act ( "{qA{qc{qi{qd {qr{qa{qi{qn{x streams down upon $N!", ch, NULL, victim, TO_NOTVICT );
    act ( "{qA{qc{qi{qd {qr{qa{qi{qn{x streams down upon $N!", ch, NULL, victim, TO_CHAR );
    send_to_char ( "{qA{qc{qi{qd {qr{qa{qi{qn{x streams down upon you!\n\r", victim );
    damage ( ch, victim, dice ( level * 4, 22 ), sn, DAM_ACID, TRUE );
    if ( chance < 28 )
        return;
    act ( "A bolt of {ol{oi{og{oh{ot{on{oi{on{og{x strikes $N!", ch, NULL, victim, TO_NOTVICT );
    act ( "A bolt of {ol{oi{og{oh{ot{on{oi{on{og{x strikes $N!", ch, NULL, victim, TO_CHAR );
    send_to_char ( "A bolt of {ol{oi{og{oh{ot{on{oi{on{og{x strikes you!\n\r", victim );
    damage ( ch, victim, dice ( level * 4, 22 ), sn, DAM_LIGHTNING, TRUE );
    if ( chance < 31 )
        return;
    act ( "$N is bombarded with large {-h{-a{-i{-l{x stones!", ch, NULL, victim, TO_NOTVICT );
    act ( "$N is bombarded with large {-h{-a{-i{-l{x stones!", ch, NULL, victim, TO_CHAR );
    send_to_char ( "You are bombarded with large {-h{-a{-i{-l{x stones!\n\r", victim );
    damage ( ch, victim, dice ( level * 4, 22 ), sn, DAM_COLD, TRUE );
    damage ( ch, victim, dice ( level * 4, 22 ), sn, DAM_BASH, TRUE );
    if ( chance < 33 )
        return;
    act ( "Torrential rains {cf{Cl{Bo{co{Cd{x down upon $N!", ch, NULL, victim, TO_NOTVICT );
    act ( "Torrential rains {cf{Cl{Bo{co{Cd{x down upon $N!", ch, NULL, victim, TO_CHAR );
    send_to_char ( "Torrential rains {cf{Cl{Bo{co{Cd{x down upon you!\n\r", victim );
    damage ( ch, victim, dice ( level * 4, 22 ), sn, DAM_WATER, TRUE );
    return;
}

MAGIC ( spell_disintegrate )
{
    CHAR_DATA *victim = ( CHAR_DATA * ) vo;
    int dam, rds;

    dam = dice ( 303, 45 );
    rds = ( get_curr_stat ( ch, STAT_INT ) / 8 ) + number_range ( 0, 2 );
    if ( victim->in_room == ch->in_room )
    {
        act ( "$n shoots a {gd{Gi{gs{Gi{gntegrat{Gi{gon {Gb{gea{gm{x at $N!!!", ch, NULL, victim, TO_ROOM );
        act ( "You shoot a {gd{Gi{gs{Gi{gntegrat{Gi{gon {Gb{gea{gm{x at $N!", ch, NULL, victim, TO_CHAR );
        while ( rds >= 0 )
        {
            damage ( ch, victim, dam + dice ( 3, 13 ), sn, DAM_ENERGY, TRUE );
            damage ( ch, victim, dam, sn, DAM_HARM, TRUE );
            rds--;
        }
        fire_effect ( victim, level, dam, TARGET_CHAR );
        acid_effect ( victim, level, dam, TARGET_CHAR );
    }
    return;
}

MAGIC ( spell_diamond_aura )
{
    AFFECT_DATA af;

    if ( IS_SET ( ch->res_flags, RES_COLD ) )
    {
        send_to_char ( "You are already protected against the cold.\n\r", ch );
        return;
    }
    if ( IS_SET ( ch->res_flags, RES_FIRE ) )
    {
        send_to_char ( "Your body is too accustomed to the heat to bar itself against the cold.\n\r", ch );
        return;
    }

    af.where = TO_RESIST;
    af.type = sn;
    af.level = level;
    af.duration = 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = RES_COLD;
    affect_to_char ( ch, &af );
    send_to_char ( "You are surrounded by a glittering white aura.\n\r", ch );
    act ( "$n is surrounded by a glittering white aura.", ch, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_ruby_aura )
{
    AFFECT_DATA af;

    if ( IS_SET ( ch->res_flags, RES_FIRE ) )
    {
        send_to_char ( "You are already protected against the heat.\n\r", ch );
        return;
    }
    if ( IS_SET ( ch->res_flags, RES_COLD ) )
    {
        send_to_char ( "Your body is too accustomed to the cold to bar itself against the heat.\n\r", ch );
        return;
    }

    af.where = TO_RESIST;
    af.type = sn;
    af.level = level;
    af.duration = 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = RES_FIRE;
    affect_to_char ( ch, &af );
    send_to_char ( "You are surrounded by a glittering red aura.\n\r", ch );
    act ( "$n is surrounded by a glittering red aura.", ch, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_sapphire_aura )
{
    AFFECT_DATA af;

    if ( IS_SET ( ch->res_flags, RES_WATER ) )
    {
        send_to_char ( "You are already protected against water.\n\r", ch );
        return;
    }
    if ( IS_SET ( ch->res_flags, RES_ACID ) )
    {
        send_to_char ( "You are too aligned with acid to guard against water.\n\r", ch );
        return;
    }

    af.where = TO_RESIST;
    af.type = sn;
    af.level = level;
    af.duration = 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = RES_WATER;
    affect_to_char ( ch, &af );
    send_to_char ( "You are surrounded by a sparkling blue aura.\n\r", ch );
    act ( "$n is surrounded by a sparkling blue aura.", ch, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_emerald_aura )
{
    AFFECT_DATA af;

    if ( IS_SET ( ch->res_flags, RES_ACID ) )
    {
        send_to_char ( "You are already protected against acid.\n\r", ch );
        return;
    }
    if ( IS_SET ( ch->res_flags, RES_WATER ) )
    {
        send_to_char ( "You are too aligned with water to guard against acid.\n\r", ch );
        return;
    }

    af.where = TO_RESIST;
    af.type = sn;
    af.level = level;
    af.duration = 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = RES_ACID;
    affect_to_char ( ch, &af );
    send_to_char ( "You are surrounded by a sparkling green aura.\n\r", ch );
    act ( "$n is surrounded by a sparkling green aura.", ch, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_topaz_aura )
{
    AFFECT_DATA af;

    if ( IS_SET ( ch->res_flags, RES_LIGHTNING ) )
    {
        send_to_char ( "You are already protected against lightning.\n\r", ch );
        return;
    }
    if ( IS_SET ( ch->res_flags, RES_EARTH ) )
    {
        send_to_char ( "You are too aligned with earth to guard against lightning.\n\r", ch );
        return;
    }

    af.where = TO_RESIST;
    af.type = sn;
    af.level = level;
    af.duration = 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = RES_LIGHTNING;
    affect_to_char ( ch, &af );
    send_to_char ( "You are surrounded by a glowing amber aura.\n\r", ch );
    act ( "$n is surrounded by a glowing amber aura.", ch, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_obsidian_aura )
{
    AFFECT_DATA af;

    if ( IS_SET ( ch->res_flags, RES_EARTH ) )
    {
        send_to_char ( "You are already protected against earth.\n\r", ch );
        return;
    }
    if ( IS_SET ( ch->res_flags, RES_LIGHTNING ) )
    {
        send_to_char ( "You are too aligned with lightning to guard against earth.\n\r", ch );
        return;
    }

    af.where = TO_RESIST;
    af.type = sn;
    af.level = level;
    af.duration = 10;
    af.location = APPLY_NONE;
    af.modifier = 0;
    af.bitvector = RES_EARTH;
    affect_to_char ( ch, &af );
    send_to_char ( "You are surrounded by a glowing black aura.\n\r", ch );
    act ( "$n is surrounded by a glowing black aura.", ch, NULL, NULL, TO_ROOM );
    return;
}

MAGIC ( spell_last_rites )
{
    AFFECT_DATA af;

    if ( IS_SHIELDED ( ch, SHD_LASTRITES ) )
    {
        send_to_char ( "You have already memorized your last rites.\n\r", ch );
        return;
    }

    af.where = TO_SHIELDS;
    af.type = sn;
    af.level = level;
    af.duration = level / 5;
    af.location = APPLY_SAVES;
    af.modifier = 0 - ( level / 7 );
    af.bitvector = SHD_LASTRITES;
    affect_to_char ( ch, &af );
    send_to_char ( "You memorize your last rites.\n\r", ch );
    act ( "$n recites some arcane scripture.", ch, NULL, NULL, TO_ROOM );
    return;
}
