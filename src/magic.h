
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *									   *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
 *  Chastain, Michael Quan, and Mitchell Tse.				   *
 *									   *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc	   *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.						   *
 *									   *
 *  Much time and thought has gone into this software and you are	   *
 *  benefitting.  We hope that you share your changes too.  What goes	   *
 *  around, comes around.						   *
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

/*
 * Spell functions.
 * Defined in magic.c.
 */
DECLARE_SPELL_FUN(spell_null);
DECLARE_SPELL_FUN(spell_summon);
DECLARE_SPELL_FUN(spell_sate);
DECLARE_SPELL_FUN(spell_quench);
DECLARE_SPELL_FUN(spell_heroes_feast);
DECLARE_SPELL_FUN(spell_holy_mace);
DECLARE_SPELL_FUN(spell_acid_rain);
DECLARE_SPELL_FUN(spell_4x);
DECLARE_SPELL_FUN(spell_acid_blast);
DECLARE_SPELL_FUN(spell_animate);
DECLARE_SPELL_FUN(spell_anger);
DECLARE_SPELL_FUN(spell_armor);
DECLARE_SPELL_FUN(spell_blade_bless);
DECLARE_SPELL_FUN(spell_bless);
DECLARE_SPELL_FUN(spell_blindness);
DECLARE_SPELL_FUN(spell_silence);
DECLARE_SPELL_FUN(spell_burning_hands);
DECLARE_SPELL_FUN(spell_call_lightning);
DECLARE_SPELL_FUN(spell_calm);
DECLARE_SPELL_FUN(spell_call_darkness);
DECLARE_SPELL_FUN(spell_camouflage);
DECLARE_SPELL_FUN(spell_cancellation);
DECLARE_SPELL_FUN(spell_cause_critical);
DECLARE_SPELL_FUN(spell_cause_light);
DECLARE_SPELL_FUN(spell_cause_serious);
DECLARE_SPELL_FUN(spell_change_sex);
DECLARE_SPELL_FUN(spell_chain_lightning);
DECLARE_SPELL_FUN(spell_charm_person);
DECLARE_SPELL_FUN(spell_chill_touch);
DECLARE_SPELL_FUN(spell_colour_spray);
DECLARE_SPELL_FUN(spell_concentration);
DECLARE_SPELL_FUN(spell_conjure);
DECLARE_SPELL_FUN(spell_continual_light);
DECLARE_SPELL_FUN(spell_control_weather);
DECLARE_SPELL_FUN(spell_create_bed);
DECLARE_SPELL_FUN(spell_create_food);
DECLARE_SPELL_FUN(spell_create_rose);
DECLARE_SPELL_FUN(spell_create_spring);
DECLARE_SPELL_FUN(spell_create_water);
DECLARE_SPELL_FUN(spell_cry);
DECLARE_SPELL_FUN(spell_cure_blindness);
DECLARE_SPELL_FUN(spell_cure_critical);
DECLARE_SPELL_FUN(spell_cure_disease);
DECLARE_SPELL_FUN(spell_cure_light);
DECLARE_SPELL_FUN(spell_cure_poison);
DECLARE_SPELL_FUN(spell_cure_serious);
DECLARE_SPELL_FUN(spell_curse);
DECLARE_SPELL_FUN(spell_entangle);
DECLARE_SPELL_FUN(spell_bloodbath);
DECLARE_SPELL_FUN(spell_demonfire);
DECLARE_SPELL_FUN(spell_detect_evil);
DECLARE_SPELL_FUN(spell_detect_good);
DECLARE_SPELL_FUN(spell_detect_hidden);
DECLARE_SPELL_FUN(spell_invigorate);
DECLARE_SPELL_FUN(spell_detect_invis);
DECLARE_SPELL_FUN(spell_detect_magic);
DECLARE_SPELL_FUN(spell_detect_poison);
DECLARE_SPELL_FUN(spell_dispel_evil);
DECLARE_SPELL_FUN(spell_dispel_good);
DECLARE_SPELL_FUN(spell_dispel_magic);
DECLARE_SPELL_FUN(spell_celestial_fury);
DECLARE_SPELL_FUN(spell_divine_protection);
DECLARE_SPELL_FUN(spell_dragon_skin);
DECLARE_SPELL_FUN(spell_dragon_wisdom);
DECLARE_SPELL_FUN(spell_earthquake);
DECLARE_SPELL_FUN(spell_empower);
DECLARE_SPELL_FUN(spell_enchant_armor);
DECLARE_SPELL_FUN(spell_enchant_weapon);
DECLARE_SPELL_FUN(spell_energy_drain);
DECLARE_SPELL_FUN(spell_faerie_fire);
DECLARE_SPELL_FUN(spell_faerie_fog);
DECLARE_SPELL_FUN(spell_farsight);
DECLARE_SPELL_FUN(spell_fireball);
DECLARE_SPELL_FUN(spell_fireproof);
DECLARE_SPELL_FUN(spell_fireshield);
DECLARE_SPELL_FUN(spell_flamestrike);
DECLARE_SPELL_FUN(spell_floating_disc);
DECLARE_SPELL_FUN(spell_fly);
DECLARE_SPELL_FUN(spell_frenzy);
DECLARE_SPELL_FUN(spell_gate);
DECLARE_SPELL_FUN(spell_gods_armor);
DECLARE_SPELL_FUN(spell_giant_strength);
DECLARE_SPELL_FUN(spell_harm);
DECLARE_SPELL_FUN(spell_divinewrath);
DECLARE_SPELL_FUN(spell_haste);
DECLARE_SPELL_FUN(spell_heal);
DECLARE_SPELL_FUN(spell_fullheal);
DECLARE_SPELL_FUN(spell_heat_metal);
DECLARE_SPELL_FUN(spell_prayer);
DECLARE_SPELL_FUN(spell_iceshield);
DECLARE_SPELL_FUN(spell_identify);
DECLARE_SPELL_FUN(spell_immolation);
DECLARE_SPELL_FUN(spell_infravision);
DECLARE_SPELL_FUN(spell_invis);
DECLARE_SPELL_FUN(spell_vanish);
DECLARE_SPELL_FUN(spell_know_alignment);
DECLARE_SPELL_FUN(spell_lightning_bolt);
DECLARE_SPELL_FUN(spell_locate_object);
DECLARE_SPELL_FUN(spell_soul_siphon);
DECLARE_SPELL_FUN(spell_magic_missile);
DECLARE_SPELL_FUN(spell_mana_shield);
DECLARE_SPELL_FUN(spell_mass_healing);
DECLARE_SPELL_FUN(spell_mass_invis);
DECLARE_SPELL_FUN(spell_master_healing);
DECLARE_SPELL_FUN(spell_meteor_storm);
DECLARE_SPELL_FUN(spell_napalm);
DECLARE_SPELL_FUN(spell_nexus);
DECLARE_SPELL_FUN(spell_pass_door);
DECLARE_SPELL_FUN(spell_plague);
DECLARE_SPELL_FUN(spell_poison);
DECLARE_SPELL_FUN(spell_portal);
DECLARE_SPELL_FUN(spell_protection_evil);
DECLARE_SPELL_FUN(spell_protection_good);
DECLARE_SPELL_FUN(spell_protection_voodoo);
DECLARE_SPELL_FUN(spell_quest_pill);
DECLARE_SPELL_FUN(spell_ray_of_truth);
DECLARE_SPELL_FUN(spell_recharge);
DECLARE_SPELL_FUN(spell_refresh);
DECLARE_SPELL_FUN(spell_remove_curse);
DECLARE_SPELL_FUN(spell_restore_mana);
DECLARE_SPELL_FUN(spell_resurrect);
DECLARE_SPELL_FUN(spell_sanctuary);
DECLARE_SPELL_FUN(spell_shocking_grasp);
DECLARE_SPELL_FUN(spell_shockshield);
DECLARE_SPELL_FUN(spell_acidshield);
DECLARE_SPELL_FUN(spell_poisonshield);
DECLARE_SPELL_FUN(spell_shield);
DECLARE_SPELL_FUN(spell_sleep);
DECLARE_SPELL_FUN(spell_slow);
DECLARE_SPELL_FUN(spell_stone_skin);
DECLARE_SPELL_FUN(spell_summon_familiar);
DECLARE_SPELL_FUN(spell_superior_healing);
DECLARE_SPELL_FUN(spell_teleport);
DECLARE_SPELL_FUN(spell_transport);
DECLARE_SPELL_FUN(spell_ventriloquate);
DECLARE_SPELL_FUN(spell_voodoo);
DECLARE_SPELL_FUN(spell_blind_rage);
DECLARE_SPELL_FUN(spell_weaken);
DECLARE_SPELL_FUN(spell_word_of_recall);
DECLARE_SPELL_FUN(spell_vampiric_laceration);
DECLARE_SPELL_FUN(spell_acid_breath);
DECLARE_SPELL_FUN(spell_fire_breath);
DECLARE_SPELL_FUN(spell_frost_breath);
DECLARE_SPELL_FUN(spell_gas_breath);
DECLARE_SPELL_FUN(spell_lightning_breath);
DECLARE_SPELL_FUN(spell_general_purpose);
DECLARE_SPELL_FUN(spell_high_explosive);
DECLARE_SPELL_FUN(spell_briarshield);

DECLARE_SPELL_FUN(spell_drain_blade);
DECLARE_SPELL_FUN(spell_shocking_blade);
DECLARE_SPELL_FUN(spell_flame_blade);
DECLARE_SPELL_FUN(spell_frost_blade);
DECLARE_SPELL_FUN(spell_sharp_blade);
DECLARE_SPELL_FUN(spell_vorpal_blade);
DECLARE_SPELL_FUN(spell_ward);
DECLARE_SPELL_FUN(spell_lifeforce);
DECLARE_SPELL_FUN(spell_elemental_fury);

/* spells added by TAKA */
DECLARE_SPELL_FUN(spell_stone_meld);
DECLARE_SPELL_FUN(spell_screen);
DECLARE_SPELL_FUN(spell_acidrain);
DECLARE_SPELL_FUN(spell_icerain);
DECLARE_SPELL_FUN(spell_firerain);
DECLARE_SPELL_FUN(spell_firestorm);
DECLARE_SPELL_FUN(spell_acidstorm);
DECLARE_SPELL_FUN(spell_icestorm);
DECLARE_SPELL_FUN(spell_bark_skin);
DECLARE_SPELL_FUN(spell_steel_skin);

DECLARE_SPELL_FUN(spell_mystic_armor);
DECLARE_SPELL_FUN(spell_banish);
DECLARE_SPELL_FUN(spell_earthrise);
DECLARE_SPELL_FUN(spell_immortal_wrath);
DECLARE_SPELL_FUN(spell_powersurge);
DECLARE_SPELL_FUN(spell_shadow_barrier);
DECLARE_SPELL_FUN(spell_mistblend);
DECLARE_SPELL_FUN(spell_downpour);
DECLARE_SPELL_FUN(spell_dust_storm);
DECLARE_SPELL_FUN(spell_ego_whip);
DECLARE_SPELL_FUN(spell_sonic_blast);
DECLARE_SPELL_FUN(spell_summon_elemental);
DECLARE_SPELL_FUN(spell_summon_mephit);
DECLARE_SPELL_FUN(spell_summon_companion);
DECLARE_SPELL_FUN(spell_summon_archon);
DECLARE_SPELL_FUN(spell_summon_songbird);
DECLARE_SPELL_FUN(spell_summon_resurrect);
DECLARE_SPELL_FUN(spell_bone_barrier);
DECLARE_SPELL_FUN(spell_death_chant);
DECLARE_SPELL_FUN(spell_terror);
DECLARE_SPELL_FUN(spell_inspire);
DECLARE_SPELL_FUN(spell_lullaby);
DECLARE_SPELL_FUN(spell_expeditious_retreat);
DECLARE_SPELL_FUN(spell_coldfire);
DECLARE_SPELL_FUN(spell_pandemonium);

DECLARE_SPELL_FUN(spell_angeldust);
DECLARE_SPELL_FUN(spell_whisper);
DECLARE_SPELL_FUN(spell_acid_arrow);
DECLARE_SPELL_FUN(spell_gust);
DECLARE_SPELL_FUN(spell_rumors);
DECLARE_SPELL_FUN(spell_drowning_pool);
DECLARE_SPELL_FUN(spell_shatter);
DECLARE_SPELL_FUN(spell_energy_bolt);
DECLARE_SPELL_FUN(spell_blight);
DECLARE_SPELL_FUN(spell_nightmare);
DECLARE_SPELL_FUN(spell_hymn);
DECLARE_SPELL_FUN(spell_hallowed_ground);
DECLARE_SPELL_FUN(spell_searing_light);
DECLARE_SPELL_FUN(spell_early_grave);
DECLARE_SPELL_FUN(spell_raindance);
DECLARE_SPELL_FUN(spell_cloudkill);
DECLARE_SPELL_FUN(spell_deceit);
DECLARE_SPELL_FUN(spell_astral_blast);
DECLARE_SPELL_FUN(spell_desecrate);
DECLARE_SPELL_FUN(spell_consecrate);
DECLARE_SPELL_FUN(spell_venom);
DECLARE_SPELL_FUN(spell_holocaust);
DECLARE_SPELL_FUN(spell_sleet);
DECLARE_SPELL_FUN(spell_laughter);
DECLARE_SPELL_FUN(spell_disrupt);
DECLARE_SPELL_FUN(spell_earthsong);
DECLARE_SPELL_FUN(spell_repulse);
DECLARE_SPELL_FUN(spell_blasphemy);
DECLARE_SPELL_FUN(spell_corrode);
DECLARE_SPELL_FUN(spell_cyclone);
DECLARE_SPELL_FUN(spell_death_knell);
DECLARE_SPELL_FUN(spell_hemorrhage);
DECLARE_SPELL_FUN(spell_unearthly_beauty);
DECLARE_SPELL_FUN(spell_freezing_sphere);
DECLARE_SPELL_FUN(spell_serendipity);
DECLARE_SPELL_FUN(spell_prismatic_spray);
DECLARE_SPELL_FUN(spell_sunburst);
DECLARE_SPELL_FUN(spell_horrid_wilting);
DECLARE_SPELL_FUN(spell_sound_burst);
DECLARE_SPELL_FUN(spell_elysium);
DECLARE_SPELL_FUN(spell_stinking_cloud);
DECLARE_SPELL_FUN(spell_baptism);
DECLARE_SPELL_FUN(spell_venomous_lies);
DECLARE_SPELL_FUN(spell_pollution);
DECLARE_SPELL_FUN(spell_phantasmal_killer);
DECLARE_SPELL_FUN(spell_wail_of_the_banshee);
DECLARE_SPELL_FUN(spell_vermin_swarm);
DECLARE_SPELL_FUN(spell_incendiary_cloud);
DECLARE_SPELL_FUN(spell_storm_of_vengeance);
DECLARE_SPELL_FUN(spell_disintegrate);
DECLARE_SPELL_FUN(spell_diamond_aura);
DECLARE_SPELL_FUN(spell_ruby_aura);
DECLARE_SPELL_FUN(spell_sapphire_aura);
DECLARE_SPELL_FUN(spell_emerald_aura);
DECLARE_SPELL_FUN(spell_topaz_aura);
DECLARE_SPELL_FUN(spell_obsidian_aura);
DECLARE_SPELL_FUN(spell_ray_of_frost);
DECLARE_SPELL_FUN(spell_last_rites);
