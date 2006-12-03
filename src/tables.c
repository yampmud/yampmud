
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

#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include "merc.h"
#include "tables.h"

/*
 * Log Off Quotes
 */

const struct quote_type quote_table[MAX_QUOTES] = {
  {"He who laughs last, thinks slowest.", "Unknown"},
  {"Imagine if there were no hypothetical situations...", "John Mendosa"},
  {"Better to remain silent and be thought a fool, than to speak and remove all doubt.", "Abraham Lincoln"},
  {"Never interrupt your enemy when he is making a mistake.",
   "Napoleon Bonaparte"},
};

/* for position */
const struct position_type position_table[] = {
  {"dead", "dead"},
  {"mortally wounded", "mort"},
  {"incapacitated", "incap"},
  {"stunned", "stun"},
  {"sleeping", "sleep"},
  {"resting", "rest"},
  {"sitting", "sit"},
  {"fighting", "fight"},
  {"standing", "stand"},
  {NULL, NULL}
};

/* for sex */
const struct sex_type sex_table[] = {
  {"none"},
  {"male"},
  {"female"},
  {"either"},
  {NULL}
};

/* for sizes */
const struct size_type size_table[] = {
  {"tiny"},
  {"small"},
  {"medium"},
  {"large"},
  {"huge",},
  {"giant"},
  {NULL}
};

/* various flag tables */
const struct flag_type act_flags[] = {
  {"npc", A, false},
  {"sentinel", B, true},
  {"scavenger", C, true},
  {"key", D, false},
  {"ranger", E, false},
  {"aggressive", F, true},
  {"stay_area", G, true},
  {"wimpy", H, true},
  {"pet", I, true},
  {"train", J, true},
  {"practice", K, true},
  {"druid", L, false},
  {"no_body", M, true},
  {"nobd_drop", N, true},
  {"undead", O, true},
  {"vampire", P, false},
  {"cleric", Q, false},
  {"mage", R, false},
  {"thief", S, false},
  {"warrior", T, false},
  {"noalign", U, true},
  {"nopurge", V, true},
  {"outdoors", W, true},
  {"is_satan", X, true},
  {"indoors", Y, true},
  {"is_priest", Z, true},
  {"healer", aa, true},
  {"gain", bb, true},
  {"update_always", cc, true},
  {"changer", dd, true},
  {"dealer", L, true},
  {NULL, 0, false}
};

/* various flag tables */
const struct flag_type act2_flags[] = {
  {"forger", A, true},
  {"dealer", B, true},
  {"banker", C, true},
  {NULL, 0, false}
};

const struct flag_type plr_flags[] = {
  {"npc", A, false},
  {"color", B, false},
  {"autoassist", C, false},
  {"autoexit", D, false},
  {"autoloot", E, false},
  {"autosac", F, false},
  {"autogold", G, false},
  {"autosplit", H, false},
  {"notran", I, false},
  {"norestore", L, false},
  {"notitle", M, false},
  {"holylight", N, false},
  {"can_loot", P, false},
  {"nosummon", Q, false},
  {"nofollow", R, false},
  {"permit", U, true},
  {"log", W, false},
  {"deny", X, false},
  {"freeze", Y, false},
  {"twit", Z, false},
  {"key", bb, false},
  {"noclan", cc, false},
  {"questor", S, true},
  // {"gquest", ee, true},
  {NULL, 0, 0}
};

const struct clan_titles clan_rank_table[] = {
  {
   {"{GW{gildthing ", "{CF{creeman ", "{MF{mreewoman "}
   },
  {
   {"{GR{gecruit ", "{CR{cecruit ", "{MR{mecruit "}
   },
  {
   {"{GA{gcolyte ", "{CA{ccolyte ", "{MA{mcolyte "}
   },
  {
   {"{GA{gdept ", "{CA{cdept ", "{MA{mdept "}
   },
  {
   {"{GO{gfficer ", "{COf{cficer ", "{MO{mfficer "}
   },
  {
   {"{GL{gieutenant ", "{CL{cieutenant ", "{ML{mieutenant "}
   },
  {
   {"{GV{gassal ", "{CV{cassal ", "{MV{massal "}
   },
  {
   {"{GA{gmbassador ", "{CA{cmbassador ", "{MA{mmbassadress "}
   },
  {
   {"{GM{gagistrate ", "{CM{cagistrate ", "{MM{magistrate "}
   },
  {
   {"{GL{gord ", "{CL{cord ", "{ML{mady "}
   },
  {
   {"{GL{geader ", "{CL{ceader ", "{ML{meader "}
   },
  {
   {"{GP{gatron ", "{CP{catron ", "{MP{matron "}
   },
  {
   {NULL, NULL, NULL}
   }
};

const struct flag_type plr2_flags[] = {
  {"challenged", A, false},
  {"challenger", B, false},
  {"consent", C, false},
  {"true_trust", D, true},
  {"wiped", E, false},
  {NULL, 0, 0}
};

const struct flag_type affect_flags[] = {
  {"blind", A, true},
  {"entangle", B, true},
  {"detect_evil", C, true},
  {"detect_invis", D, true},
  {"detect_magic", E, true},
  {"detect_hidden", F, true},
  {"detect_good", G, true},
  {"unused_H", H, false},
  {"faerie_fire", I, true},
  {"infrared", J, true},
  {"curse", K, true},
  {"farsight", L, true},
  {"poison", M, true},
  {"sneak", P, true},
  {"hide", Q, true},
  {"sleep", R, true},
  {"charm", S, true},
  {"flying", T, true},
  {"pass_door", U, true},
  {"haste", V, true},
  {"calm", W, true},
  {"plague", X, true},
  {"weaken", Y, true},
  {"dark_vision", Z, true},
  {"berserk", aa, true},
  {"swim", bb, true},
  {"regeneration", cc, true},
  {"slow", dd, true},
  {"terror", ee, true},
  {NULL, 0, 0}
};

const struct flag_type shield_flags[] = {
  {"protect_voodoo", A, true},
  {"invisible", B, true},
  {"ice", C, true},
  {"fire", D, true},
  {"shock", E, true},
  {"acid", F, true},
  {"poison", G, true},
  {"briar", H, true},
  {"sanctuary", I, true},
  {"protect_evil", J, true},
  {"protect_good", K, true},
  {"mana_shield", L, true},
  {"swalk", M, true},
  {"ward", N, true},
  {"divine_prot", O, true},
  {"vanish", P, true},
  {"stone_skin", Q, true},
  {"bless", R, true},
  {"frenzy", S, true},
  {"lifeforce", T, true},
  {"mistblend", U, true},
  {"inspire", V, true},
  {"expeditious_retreat", W, true},
  {"silence", X, true},
  {"concentration", Y, true},
  {"hemorrhage", Z, true},
  {"last_rites", aa, true},
  {"unused_bb", bb, false},
  {"unused_cc", cc, false},
  {"unused_dd", dd, false},
  {"unused_ee", ee, false},

  {NULL, 0, 0}
};

const struct flag_type off_flags[] = {
  {"area_attack", A, true},
  {"backstab", B, true},
  {"bash", C, true},
  {"berserk", D, true},
  {"disarm", E, true},
  {"dodge", F, true},
  {"fade", G, true},
  {"fast", H, true},
  {"kick", I, true},
  {"dirt_kick", J, true},
  {"parry", K, true},
  {"rescue", L, true},
  {"tail", M, true},
  {"trip", N, true},
  {"crush", O, true},
  {"assist_all", P, true},
  {"assist_align", Q, true},
  {"assist_race", R, true},
  {"assist_players", S, true},
  {"assist_guard", T, true},
  {"assist_vnum", U, true},
  {NULL, 0, 0}
};

const struct flag_type imm_flags[] = {
  {"summon", A, true},
  {"charm", B, true},
  {"magic", C, true},
  {"weapon", D, true},
  {"bash", E, true},
  {"pierce", F, true},
  {"slash", G, true},
  {"fire", H, true},
  {"cold", I, true},
  {"lightning", J, true},
  {"acid", K, true},
  {"poison", L, true},
  {"negative", M, true},
  {"holy", N, true},
  {"energy", O, true},
  {"mental", P, true},
  {"disease", Q, true},
  {"water", R, true},
  {"light", S, true},
  {"sound", T, true},
  {"wood", X, true},
  {"silver", Y, true},
  {"iron", Z, true},
  {"nature", aa, true},
  {"earth", bb, true},
  {NULL, 0, 0}
};

const struct flag_type item_extra[] = {
  {"glowing", A, true},
  {"humming", B, true},
  {"dark", C, true},
  {"lock", D, false},
  {"evil", E, true},
  {"invisible", F, true},
  {"magic", G, true},
  {"nodrop", H, true},
  {"bless", I, true},
  {"anti_good", J, true},
  {"anti_evil", K, true},
  {"anti_neutral", L, true},
  {"noremove", M, true},
  {"inventory", N, true},
  {"nopurge", O, true},
  {"rot_death", P, true},
  {"vis_death", Q, true},
  {"nosacrifice", R, true},
  {"nonmetal", S, false},
  {"nolocate", T, true},
  {"melt_drop", U, true},
  {"had_timer", U, false},
  {"sell_extract", W, true},
  {"burn_proof", Y, true},
  {"nouncurse", Z, true},
  {"quest", aa, true},
  {"forced", bb, true},
  {"questpoint", cc, true},
  {NULL, 0, 0}
};

const struct flag_type item_wear[] = {
  {"take", A, true},
  {"finger", B, true},
  {"neck", C, true},
  {"head", E, true},
  {"legs", F, true},
  {"feet", G, true},
  {"hands", H, true},
  {"arms", I, true},
  {"shield", J, true},
  {"body", K, true},
  {"waist", L, true},
  {"wrist", M, true},
  {"wield", N, true},
  {"hold", O, true},
  {"no_sac", P, true},
  {"float", Q, true},
  {"face", R, true},
  {"ankle", S, true},
  {"tattoo", T, true},
  {"aura", U, true},
  {"ear", V, true},
  {"ctattoo", W, true},
  {NULL, 0, 0}
};

const struct flag_type item_weapon[] = {
  {"flaming", A, true},
  {"frost", B, true},
  {"vampiric", C, true},
  {"sharp", D, true},
  {"vorpal", E, true},
  {"two-handed", F, true},
  {"shocking", G, true},
  {"poisoned", H, true},
  {"manadrain", I, true},
  {NULL, 0, 0}
};

const struct flag_type area_room[] = {
  {"dark", A, true},
  {"no_mobs", C, true},
  {"indoors", D, true},
  {"private", J, true},
  {"safe", K, true},
  {"solitary", L, true},
  {"pet_shop", M, false},
  {"no_recall", N, true},
  {"imp", O, false},
  {"gods", P, false},
  {"heroes", Q, false},
  {"newbies", R, false},
  {"law", S, true},
  {"nowhere", T, false},
  {"locked", X, false},
  {NULL, 0, 0}
};

const struct flag_type form_flags[] = {
  {"edible", FORM_EDIBLE, true},
  {"poison", FORM_POISON, true},
  {"magical", FORM_MAGICAL, true},
  {"instant_decay", FORM_INSTANT_DECAY, true},
  {"other", FORM_OTHER, true},
  {"animal", FORM_ANIMAL, true},
  {"sentient", FORM_SENTIENT, true},
  {"undead", FORM_UNDEAD, true},
  {"construct", FORM_CONSTRUCT, true},
  {"mist", FORM_MIST, true},
  {"intangible", FORM_INTANGIBLE, true},
  {"biped", FORM_BIPED, true},
  {"centaur", FORM_CENTAUR, true},
  {"insect", FORM_INSECT, true},
  {"spider", FORM_SPIDER, true},
  {"crustacean", FORM_CRUSTACEAN, true},
  {"worm", FORM_WORM, true},
  {"blob", FORM_BLOB, true},
  {"mammal", FORM_MAMMAL, true},
  {"bird", FORM_BIRD, true},
  {"reptile", FORM_REPTILE, true},
  {"snake", FORM_SNAKE, true},
  {"dragon", FORM_DRAGON, true},
  {"amphibian", FORM_AMPHIBIAN, true},
  {"fish", FORM_FISH, true},
  {"cold_blood", FORM_COLD_BLOOD, true},
  {NULL, 0, 0}
};

const struct flag_type part_flags[] = {
  {"head", PART_HEAD, true},
  {"arms", PART_ARMS, true},
  {"legs", PART_LEGS, true},
  {"heart", PART_HEART, true},
  {"brains", PART_BRAINS, true},
  {"guts", PART_GUTS, true},
  {"hands", PART_HANDS, true},
  {"feet", PART_FEET, true},
  {"fingers", PART_FINGERS, true},
  {"ear", PART_EAR, true},
  {"eye", PART_EYE, true},
  {"long_tongue", PART_LONG_TONGUE, true},
  {"eyestalks", PART_EYESTALKS, true},
  {"tentacles", PART_TENTACLES, true},
  {"fins", PART_FINS, true},
  {"wings", PART_WINGS, true},
  {"tail", PART_TAIL, true},
  {"claws", PART_CLAWS, true},
  {"fangs", PART_FANGS, true},
  {"horns", PART_HORNS, true},
  {"scales", PART_SCALES, true},
  {"tusks", PART_TUSKS, true},
  {NULL, 0, 0}
};

const struct flag_type comm_flags[] = {
  {"quiet", COMM_QUIET, true},
  {"deaf", COMM_DEAF, true},
  {"nowiz", COMM_NOWIZ, true},
  {"noclangossip", COMM_NOOOC, true},
  {"nogossip", COMM_NOGOSSIP, true},
  {"nocgossip", COMM_NOCGOSSIP, true},
  {"noqgossip", COMM_NOQGOSSIP, true},
  {"noask", COMM_NOASK, true},
  {"nomusic", COMM_NOMUSIC, true},
  {"noclan", COMM_NOCLAN, true},
  {"nosocial", COMM_NOSOCIAL, true},
  {"noquote", COMM_NOQUOTE, true},
  /* { "shoutsoff", COMM_SHOUTSOFF, true }, */
  {"noarena", COMM_NOARENA, true},
  /* { "true_trust", COMM_true_TRUST, true }, */
  {"compact", COMM_COMPACT, true},
  {"brief", COMM_BRIEF, true},
  {"noannounce", COMM_NOANNOUNCE, true},
  {"prompt", COMM_PROMPT, true},
  {"long", COMM_LONG, true},
  {"store", COMM_STORE, true},
  {"combine", COMM_COMBINE, true},
  {"telnet_ga", COMM_TELNET_GA, true},
  {"show_affects", COMM_SHOW_AFFECTS, true},
  {"nograts", COMM_NOGRATS, true},
  /* { "wiped", COMM_WIPED, false }, */
  {"noemote", COMM_NOEMOTE, false},
  {"noshout", COMM_NOSHOUT, false},
  {"notell", COMM_NOTELL, false},
  {"nochannels", COMM_NOCHANNELS, false},
  {"snoop_proof", COMM_SNOOP_PROOF, false},
  {"afk", COMM_AFK, false},
  {"norace", COMM_NORACE, true},
  {"pray", COMM_PRAY, true},
  {NULL, 0, 0}
};

const struct flag_type mprog_flags[] = {
  {"act", TRIG_ACT, true},
  {"bribe", TRIG_BRIBE, true},
  {"death", TRIG_DEATH, true},
  {"entry", TRIG_ENTRY, true},
  {"fight", TRIG_FIGHT, true},
  {"give", TRIG_GIVE, true},
  {"greet", TRIG_GREET, true},
  {"grall", TRIG_GRALL, true},
  {"kill", TRIG_KILL, true},
  {"hpcnt", TRIG_HPCNT, true},
  {"random", TRIG_RANDOM, true},
  {"speech", TRIG_SPEECH, true},
  {"exit", TRIG_EXIT, true},
  {"exall", TRIG_EXALL, true},
  {"delay", TRIG_DELAY, true},
  {"surr", TRIG_SURR, true},
  {NULL, 0, true}
};

const struct flag_type area_flags[] = {
  {"none", AREA_NONE, false},
  {"changed", AREA_CHANGED, true},
  {"added", AREA_ADDED, true},
  {"loading", AREA_LOADING, true},
  {NULL, 0, 0}
};

const struct flag_type sex_flags[] = {
  {"male", SEX_MALE, true},
  {"female", SEX_FEMALE, true},
  {"neutral", SEX_NEUTRAL, true},
  {"random", 3, true},          /* ROM */
  {"none", SEX_NEUTRAL, true},
  {NULL, 0, 0}
};

const struct flag_type exit_flags[] = {
  {"door", EX_ISDOOR, true},
  {"closed", EX_CLOSED, true},
  {"locked", EX_LOCKED, true},
  {"pickproof", EX_PICKPROOF, true},
  {"nopass", EX_NOPASS, true},
  {"easy", EX_EASY, true},
  {"hard", EX_HARD, true},
  {"infuriating", EX_INFURIATING, true},
  {"noclose", EX_NOCLOSE, true},
  {"nolock", EX_NOLOCK, true},
  {NULL, 0, 0}
};

const struct flag_type door_resets[] = {
  {"open and unlocked", 0, true},
  {"closed and unlocked", 1, true},
  {"closed and locked", 2, true},
  {NULL, 0, 0}
};

const struct flag_type room_flags[] = {
  {"dark", ROOM_DARK, true},
  {"no_mob", ROOM_NO_MOB, true},
  {"indoors", ROOM_INDOORS, true},
  {"private", ROOM_PRIVATE, true},
  {"safe", ROOM_SAFE, true},
  {"solitary", ROOM_SOLITARY, true},
  {"pet_shop", ROOM_PET_SHOP, true},
  {"no_recall", ROOM_NO_RECALL, true},
  {"imp_only", ROOM_IMP_ONLY, true},
  {"gods_only", ROOM_GODS_ONLY, true},
  {"heroes_only", ROOM_HEROES_ONLY, true},
  {"newbies_only", ROOM_NEWBIES_ONLY, true},
  {"law", ROOM_LAW, true},
  {"arena", ROOM_ARENA, true},
  {"distalt", ROOM_DISTALT, true},
  {"nodrag", ROOM_NODRAG, true},
  {"recall_here", ROOM_RECALL_HERE, true},
  {"nowhere", ROOM_NOWHERE, true},
  {"home", ROOM_HOME, true},
  {"teleport", ROOM_TELEPORT, true},
  {NULL, 0, 0}
};

const struct flag_type sector_flags[] = {
  {"inside", SECT_INSIDE, true},
  {"city", SECT_CITY, true},
  {"field", SECT_FIELD, true},
  {"forest", SECT_FOREST, true},
  {"hills", SECT_HILLS, true},
  {"mountain", SECT_MOUNTAIN, true},
  {"swim", SECT_WATER_SWIM, true},
  {"noswim", SECT_WATER_NOSWIM, true},
  {"unused", SECT_UNUSED, true},
  {"air", SECT_AIR, true},
  {"desert", SECT_DESERT, true},
  {NULL, 0, 0}
};

const struct flag_type type_flags[] = {
  {"light", ITEM_LIGHT, true},
  {"scroll", ITEM_SCROLL, true},
  {"wand", ITEM_WAND, true},
  {"staff", ITEM_STAFF, true},
  {"weapon", ITEM_WEAPON, true},
  {"treasure", ITEM_TREASURE, true},
  {"armor", ITEM_ARMOR, true},
  {"potion", ITEM_POTION, true},
  {"furniture", ITEM_FURNITURE, true},
  {"trash", ITEM_TRASH, true},
  {"container", ITEM_CONTAINER, true},
  {"drinkcontainer", ITEM_DRINK_CON, true},
  {"key", ITEM_KEY, true},
  {"food", ITEM_FOOD, true},
  {"money", ITEM_MONEY, true},
  {"boat", ITEM_BOAT, true},
  {"npccorpse", ITEM_CORPSE_NPC, true},
  {"pc corpse", ITEM_CORPSE_PC, false},
  {"itempile", ITEM_ITEMPILE, true},
  {"fountain", ITEM_FOUNTAIN, true},
  {"pill", ITEM_PILL, true},
  /* { "protect", ITEM_PROTECT, true }, */
  {"map", ITEM_MAP, true},
  {"portal", ITEM_PORTAL, true},
  {"warpstone", ITEM_WARP_STONE, true},
  {"roomkey", ITEM_ROOM_KEY, true},
  {"gem", ITEM_GEM, true},
  {"jewelry", ITEM_JEWELRY, true},
  {"jukebox", ITEM_JUKEBOX, true},
  {"demonstone", ITEM_DEMON_STONE, true},
  {"slotmachine", ITEM_SLOT_MACHINE, true},
  {"pit", ITEM_PIT, true},
  {"instrument", ITEM_INSTRUMENT, true},
  {NULL, 0, 0}
};

const struct flag_type extra_flags[] = {
  {"glow", ITEM_GLOW, true},
  {"hum", ITEM_HUM, true},
  {"dark", ITEM_DARK, true},
  {"lock", ITEM_LOCK, true},
  {"evil", ITEM_EVIL, true},
  {"invis", ITEM_INVIS, true},
  {"magic", ITEM_MAGIC, true},
  {"nodrop", ITEM_NODROP, true},
  {"bless", ITEM_BLESS, true},
  {"antigood", ITEM_ANTI_GOOD, true},
  {"antievil", ITEM_ANTI_EVIL, true},
  {"antineutral", ITEM_ANTI_NEUTRAL, true},
  {"noremove", ITEM_NOREMOVE, true},
  {"inventory", ITEM_INVENTORY, true},
  {"nopurge", ITEM_NOPURGE, true},
  {"rotdeath", ITEM_ROT_DEATH, true},
  {"visdeath", ITEM_VIS_DEATH, true},
  {"nonmetal", ITEM_NONMETAL, true},
  {"meltdrop", ITEM_MELT_DROP, true},
  {"hadtimer", ITEM_HAD_TIMER, true},
  {"sellextract", ITEM_SELL_EXTRACT, true},
  {"burnproof", ITEM_BURN_PROOF, true},
  {"nouncurse", ITEM_NOUNCURSE, true},
  {"quest", ITEM_QUEST, true},
  {NULL, 0, 0}
};

const struct flag_type wear_flags[] = {
  {"take", ITEM_TAKE, true},
  {"finger", ITEM_WEAR_FINGER, true},
  {"neck", ITEM_WEAR_NECK, true},
  {"body", ITEM_WEAR_BODY, true},
  {"head", ITEM_WEAR_HEAD, true},
  {"legs", ITEM_WEAR_LEGS, true},
  {"feet", ITEM_WEAR_FEET, true},
  {"hands", ITEM_WEAR_HANDS, true},
  {"arms", ITEM_WEAR_ARMS, true},
  {"shield", ITEM_WEAR_SHIELD, true},
  {"about", ITEM_WEAR_ABOUT, true},
  {"waist", ITEM_WEAR_WAIST, true},
  {"wrist", ITEM_WEAR_WRIST, true},
  {"wield", ITEM_WIELD, true},
  {"hold", ITEM_HOLD, true},
  {"nosac", ITEM_NO_SAC, true},
  {"wearfloat", ITEM_WEAR_FLOAT, true},
  /*    {   "twohands",            ITEM_TWO_HANDS,         true    }, */
  {"face", ITEM_WEAR_FACE, true},
  {"ankle", ITEM_WEAR_ANKLE, true},
  {"tattoo", ITEM_WEAR_TATTOO, true},
  {"aura", ITEM_WEAR_AURA, true},
  {"ear", ITEM_WEAR_EAR, true},
  {"ctattoo", ITEM_WEAR_CTATTOO, true},
  {"back", ITEM_WEAR_BACK, true},
  {NULL, 0, 0}
};

/*
 * Used when adding an affect to tell where it goes.
 * See addaffect and delaffect in act_olc.c
 */
const struct flag_type apply_flags[] = {
  {"none", APPLY_NONE, true},
  {"strength", APPLY_STR, true},
  {"dexterity", APPLY_DEX, true},
  {"intelligence", APPLY_INT, true},
  {"wisdom", APPLY_WIS, true},
  {"constitution", APPLY_CON, true},
  {"sex", APPLY_SEX, true},
  {"class", APPLY_CLASS, true},
  {"level", APPLY_LEVEL, true},
  {"age", APPLY_AGE, true},
  {"height", APPLY_HEIGHT, true},
  {"weight", APPLY_WEIGHT, true},
  {"mana", APPLY_MANA, true},
  {"hp", APPLY_HIT, true},
  {"move", APPLY_MOVE, true},
  {"gold", APPLY_GOLD, true},
  {"experience", APPLY_EXP, true},
  {"ac", APPLY_AC, true},
  {"hitroll", APPLY_HITROLL, true},
  {"damroll", APPLY_DAMROLL, true},
  {"saves", APPLY_SAVES, true},
  {"savingpara", APPLY_SAVING_PARA, true},
  {"savingrod", APPLY_SAVING_ROD, true},
  {"savingpetri", APPLY_SAVING_PETRI, true},
  {"savingbreath", APPLY_SAVING_BREATH, true},
  {"savingspell", APPLY_SAVING_SPELL, true},
  {"spellaffect", APPLY_SPELL_AFFECT, false},
  {NULL, 0, 0}
};

/*
 * What is seen.
 */
const struct flag_type wear_loc_strings[] = {
  {"in the inventory", WEAR_NONE, true},
  {"as a light", WEAR_LIGHT, true},
  {"on the left finger", WEAR_FINGER_L, true},
  {"on the right finger", WEAR_FINGER_R, true},
  {"around the neck (1)", WEAR_NECK_1, true},
  {"around the neck (2)", WEAR_NECK_2, true},
  {"on the body", WEAR_BODY, true},
  {"over the head", WEAR_HEAD, true},
  {"on the legs", WEAR_LEGS, true},
  {"on the feet", WEAR_FEET, true},
  {"on the hands", WEAR_HANDS, true},
  {"on the arms", WEAR_ARMS, true},
  {"as a shield", WEAR_SHIELD, true},
  {"about the shoulders", WEAR_ABOUT, true},
  {"around the waist", WEAR_WAIST, true},
  {"on the left wrist", WEAR_WRIST_L, true},
  {"on the right wrist", WEAR_WRIST_R, true},
  {"wielded", WEAR_WIELD, true},
  {"held in the hands", WEAR_HOLD, true},
  {"floating nearby", WEAR_FLOAT, true},
  {"on the face", WEAR_FACE, true},
  {"on the left ankle", WEAR_ANKLE_L, true},
  {"on the right ankle", WEAR_ANKLE_R, true},
  {"as a tattoo", WEAR_TATTOO, true},
  {"as aura", WEAR_AURA, true},
  {"worn on left ear", WEAR_EAR_L, true},
  {"worn on right ear", WEAR_EAR_R, true},
  {"worn as clan tattoo", WEAR_CTATTOO, true},
  {"on the back", WEAR_BACK, true},
  {NULL, 0, 0}
};

const struct flag_type wear_loc_flags[] = {
  {"none", WEAR_NONE, true},
  {"light", WEAR_LIGHT, true},
  {"lfinger", WEAR_FINGER_L, true},
  {"rfinger", WEAR_FINGER_R, true},
  {"neck1", WEAR_NECK_1, true},
  {"neck2", WEAR_NECK_2, true},
  {"body", WEAR_BODY, true},
  {"head", WEAR_HEAD, true},
  {"legs", WEAR_LEGS, true},
  {"feet", WEAR_FEET, true},
  {"hands", WEAR_HANDS, true},
  {"arms", WEAR_ARMS, true},
  {"shield", WEAR_SHIELD, true},
  {"about", WEAR_ABOUT, true},
  {"waist", WEAR_WAIST, true},
  {"lwrist", WEAR_WRIST_L, true},
  {"rwrist", WEAR_WRIST_R, true},
  {"wielded", WEAR_WIELD, true},
  {"hold", WEAR_HOLD, true},
  {"floating", WEAR_FLOAT, true},
  {"face", WEAR_FACE, true},
  {"lankle", WEAR_ANKLE_L, true},
  {"rankle", WEAR_ANKLE_R, true},
  {"tattoo", WEAR_TATTOO, true},
  {"aura", WEAR_AURA, true},
  {"lear", WEAR_EAR_L, true},
  {"rear", WEAR_EAR_R, true},
  {"clan tattoo", WEAR_CTATTOO, true},
  {"back", WEAR_BACK, true},
  {NULL, 0, 0}

};

const struct flag_type container_flags[] = {
  {"closeable", 1, true},
  {"pickproof", 2, true},
  {"closed", 4, true},
  {"locked", 8, true},
  {"puton", 16, true},
  {NULL, 0, 0}
};

/*****************************************************************************
                      ROM - specific tables:
 ****************************************************************************/

const struct flag_type ac_type[] = {
  {"pierce", AC_PIERCE, true},
  {"bash", AC_BASH, true},
  {"slash", AC_SLASH, true},
  {"exotic", AC_EXOTIC, true},
  {NULL, 0, 0}
};

const struct flag_type size_flags[] = {
  {"tiny", SIZE_TINY, true},
  {"small", SIZE_SMALL, true},
  {"medium", SIZE_MEDIUM, true},
  {"large", SIZE_LARGE, true},
  {"huge", SIZE_HUGE, true},
  {"giant", SIZE_GIANT, true},
  {NULL, 0, 0},
};

const struct flag_type weapon_class[] = {
  {"exotic", WEAPON_EXOTIC, true},
  {"sword", WEAPON_SWORD, true},
  {"dagger", WEAPON_DAGGER, true},
  {"spear", WEAPON_SPEAR, true},
  {"mace", WEAPON_MACE, true},
  {"axe", WEAPON_AXE, true},
  {"flail", WEAPON_FLAIL, true},
  {"whip", WEAPON_WHIP, true},
  {"polearm", WEAPON_POLEARM, true},
  {NULL, 0, 0}
};

const struct flag_type weapon_type2[] = {
  {"flaming", WEAPON_FLAMING, true},
  {"frost", WEAPON_FROST, true},
  {"vampiric", WEAPON_VAMPIRIC, true},
  {"sharp", WEAPON_SHARP, true},
  {"vorpal", WEAPON_VORPAL, true},
  {"twohands", WEAPON_TWO_HANDS, true},
  {"shocking", WEAPON_SHOCKING, true},
  {"poison", WEAPON_POISON, true},
  {"manadrain", WEAPON_MANADRAIN, true},
  {NULL, 0, 0}
};

const struct flag_type res_flags[] = {
  {"summon", RES_SUMMON, true},
  {"charm", RES_CHARM, true},
  {"magic", RES_MAGIC, true},
  {"weapon", RES_WEAPON, true},
  {"nature", RES_NATURE, true},
  {"bash", RES_BASH, true},
  {"pierce", RES_PIERCE, true},
  {"slash", RES_SLASH, true},
  {"fire", RES_FIRE, true},
  {"cold", RES_COLD, true},
  {"lightning", RES_LIGHTNING, true},
  {"acid", RES_ACID, true},
  {"poison", RES_POISON, true},
  {"negative", RES_NEGATIVE, true},
  {"holy", RES_HOLY, true},
  {"energy", RES_ENERGY, true},
  {"mental", RES_MENTAL, true},
  {"disease", RES_DISEASE, true},
  {"water", RES_WATER, true},
  {"earth", RES_EARTH, true},
  {"light", RES_LIGHT, true},
  {"sound", RES_SOUND, true},
  {"wood", RES_WOOD, true},
  {"silver", RES_SILVER, true},
  {"iron", RES_IRON, true},
  {NULL, 0, 0}
};

const struct flag_type vuln_flags[] = {
  {"summon", VULN_SUMMON, true},
  {"charm", VULN_CHARM, true},
  {"magic", VULN_MAGIC, true},
  {"weapon", VULN_WEAPON, true},
  {"nature", VULN_NATURE, true},
  {"bash", VULN_BASH, true},
  {"pierce", VULN_PIERCE, true},
  {"slash", VULN_SLASH, true},
  {"fire", VULN_FIRE, true},
  {"cold", VULN_COLD, true},
  {"lightning", VULN_LIGHTNING, true},
  {"acid", VULN_ACID, true},
  {"poison", VULN_POISON, true},
  {"negative", VULN_NEGATIVE, true},
  {"holy", VULN_HOLY, true},
  {"energy", VULN_ENERGY, true},
  {"mental", VULN_MENTAL, true},
  {"disease", VULN_DISEASE, true},
  {"water", VULN_WATER, true},
  {"earth", VULN_EARTH, true},
  {"light", VULN_LIGHT, true},
  {"sound", VULN_SOUND, true},
  {"wood", VULN_WOOD, true},
  {"silver", VULN_SILVER, true},
  {"iron", VULN_IRON, true},
  {NULL, 0, 0}
};

const struct flag_type position_flags[] = {
  {"dead", POS_DEAD, false},
  {"mortal", POS_MORTAL, false},
  {"incap", POS_INCAP, false},
  {"stunned", POS_STUNNED, false},
  {"sleeping", POS_SLEEPING, true},
  {"resting", POS_RESTING, true},
  {"sitting", POS_SITTING, true},
  {"fighting", POS_FIGHTING, false},
  {"standing", POS_STANDING, true},
  {NULL, 0, 0}
};

const struct flag_type portal_flags[] = {
  {"normal_exit", GATE_NORMAL_EXIT, true},
  {"no_curse", GATE_NOCURSE, true},
  {"go_with", GATE_GOWITH, true},
  {"buggy", GATE_BUGGY, true},
  {"random", GATE_RANDOM, true},
  {NULL, 0, 0}
};

const struct flag_type furniture_flags[] = {
  {"stand_at", STAND_AT, true},
  {"stand_on", STAND_ON, true},
  {"stand_in", STAND_IN, true},
  {"sit_at", SIT_AT, true},
  {"sit_on", SIT_ON, true},
  {"sit_in", SIT_IN, true},
  {"rest_at", REST_AT, true},
  {"rest_on", REST_ON, true},
  {"rest_in", REST_IN, true},
  {"sleep_at", SLEEP_AT, true},
  {"sleep_on", SLEEP_ON, true},
  {"sleep_in", SLEEP_IN, true},
  {"put_at", PUT_AT, true},
  {"put_on", PUT_ON, true},
  {"put_in", PUT_IN, true},
  {"put_inside", PUT_INSIDE, true},
  {NULL, 0, 0}
};

const struct flag_type apply_types[] = {
  {"affects", TO_AFFECTS, true},
  {"object", TO_OBJECT, true},
  {"immune", TO_IMMUNE, true},
  {"resist", TO_RESIST, true},
  {"vuln", TO_VULN, true},
  {"weapon", TO_WEAPON, true},
  {NULL, 0, true}
};

const struct bit_type bitvector_type[] = {
  {affect_flags, "affect"},
  {apply_flags, "apply"},
  {imm_flags, "imm"},
  {res_flags, "res"},
  {vuln_flags, "vuln"},
  {weapon_type2, "weapon"}
};
