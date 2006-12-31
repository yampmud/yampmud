#ifndef GLOBALS_H
#define GLOBALS_H
#ifdef IN_DB_C
#ifdef GLOBAL
#undef GLOBAL
#endif
#ifdef GLOBAL_DEF
#undef GLOABL_DEF
#endif
#define	GLOBAL(str) str
#define GLOBAL_DEF(str, def) str = def

#else
#ifdef GLOBAL
#undef GLOBAL
#endif
#ifdef GLOBAL_DEF
#undef GLOABL_DEF
#endif
#define	GLOBAL(str) extern str
#define GLOBAL_DEF(str, def) extern str
#endif

GLOBAL_DEF(int mobile_count, 0);
GLOBAL(char str_empty[1]);

GLOBAL_DEF(AREA_DATA * area_first, NULL);
GLOBAL_DEF(AREA_DATA * area_last, NULL);
GLOBAL_DEF(AREA_DATA * current_area, NULL);
GLOBAL_DEF(SHOP_DATA * shop_last, NULL);

GLOBAL_DEF(int top_area, 0);
GLOBAL_DEF(int top_exit, 0);
GLOBAL_DEF(int top_help, 0);
GLOBAL_DEF(int top_reset, 0);
GLOBAL_DEF(int top_room, 0);
GLOBAL_DEF(int top_shop, 0);
GLOBAL_DEF(int top_mob_index, 0);
GLOBAL_DEF(int top_obj_index, 0);
GLOBAL_DEF(int top_affect, 0);
GLOBAL_DEF(int top_ed, 0);
GLOBAL_DEF(int top_mprog_index, 0);
GLOBAL_DEF(int top_vnum_mob, 0);
GLOBAL_DEF(int top_vnum_obj, 0);
GLOBAL_DEF(int top_vnum_room, 0);
GLOBAL_DEF(long top_vnum, 0);

GLOBAL(ROOM_INDEX_DATA * room_index_hash[MAX_KEY_HASH]);
GLOBAL(GQUEST gquest_info);

GLOBAL_DEF(HELP_DATA * help_free, NULL);

GLOBAL_DEF(int share_value, SHARE_VALUE);

GLOBAL_DEF(SHOP_DATA * shop_first, NULL);

GLOBAL_DEF(CHAR_DATA * char_list, NULL);
GLOBAL_DEF(DESCRIPTOR_DATA * descriptor_list, NULL);
GLOBAL_DEF(OBJ_DATA * object_list, NULL);

GLOBAL_DEF(MPROG_CODE * mprog_list, NULL);
GLOBAL_DEF(AUCTION_DATA * auction_list, NULL);

GLOBAL(char bug_buf[2 * MAX_INPUT_LENGTH]);

GLOBAL(time_t current_time);
GLOBAL_DEF(bool fLogAll, false);
GLOBAL_DEF(bool islogonly, false);
GLOBAL(KILL_DATA kill_table[MAX_LEVEL]);
GLOBAL(char log_buf[2 * MAX_INPUT_LENGTH]);

GLOBAL(TIME_INFO_DATA time_info);
GLOBAL(WEATHER_DATA weather_info);
GLOBAL_DEF(int reboot_counter, -1);

GLOBAL_DEF(bool MOBtrigger, true);

GLOBAL_DEF(bool global_quest, false);
GLOBAL_DEF(bool double_exp, false);
GLOBAL_DEF(bool double_qp, false);
GLOBAL_DEF(bool happy_hour, false);
GLOBAL_DEF(bool fuckedup, false);
GLOBAL_DEF(bool drugs, false);
GLOBAL_DEF(int titl, 0);
GLOBAL(char new_arg[MIL]);
GLOBAL(char new_argg[MIL]);
GLOBAL(char fuck_arg[MIL]);
GLOBAL(char clean_arg[MIL]);

GLOBAL_DEF(bool tctoggle, true);
GLOBAL_DEF(bool tctogglee, true);
GLOBAL(char last_command[MSL]);
GLOBAL(char last_command2[MSL]);
GLOBAL(char lastwinner[MSL]);
GLOBAL(char madmin_reroll[MSL]);
GLOBAL_DEF(int last_descriptor, 0);
GLOBAL_DEF(int arena, FIGHT_OPEN);

GLOBAL_DEF(bool is_mass_arena, false);
GLOBAL_DEF(bool spar, false);
GLOBAL_DEF(bool is_mass_arena_fight, false);
GLOBAL_DEF(int arenacount, 0);
GLOBAL_DEF(int mass_arena_players, 0);
GLOBAL_DEF(int mass_arena_players_joined, 0);
GLOBAL_DEF(int mass_arena_valid_ticks, 0);
GLOBAL_DEF(int mass_arena_ticks, 0);

GLOBAL_DEF(bool mass_arena_fticked, 0);

GLOBAL_DEF(bool dist_ent, false);
GLOBAL_DEF(int copyover_countdown, -1);
GLOBAL_DEF(long logins_hour, 1);
GLOBAL_DEF(long logins_today, 1);
GLOBAL_DEF(long logins_total, 1);
GLOBAL_DEF(float uptime_ticks, 0.00);
GLOBAL_DEF(long hour_ticks, 0);
GLOBAL_DEF(int uptime, 0);
GLOBAL_DEF(int boot_time, 0);
GLOBAL_DEF(int max_ever, 0);
GLOBAL_DEF(int cur_on, 0);
GLOBAL_DEF(int max_on, 0);

GLOBAL(int happy_pot);

GLOBAL_DEF(bool is_mid, true);
GLOBAL_DEF(bool is_copyover_countdown, false);
GLOBAL_DEF(AREA_DATA * area_first_sorted, NULL);
GLOBAL_DEF(bool newlock, false);
GLOBAL_DEF(bool wizlock, false);
GLOBAL_DEF(bool merc_down, false);

GLOBAL_DEF(int chain, 0);

GLOBAL_DEF(BAN_DATA * ban_list, NULL);
GLOBAL_DEF(NOTE_DATA * note_list, NULL);
GLOBAL_DEF(CLN_DATA * cln_list, NULL);

GLOBAL_DEF(char *help_greetinga, "Welcome to " mudname);
GLOBAL_DEF(char *help_greetingb, "Welcome to " mudname);
GLOBAL_DEF(char *help_greetingc, "Welcome to " mudname);
GLOBAL_DEF(char *help_greetingd, "Welcome to " mudname);
GLOBAL_DEF(char *help_greetinge, "Welcome to " mudname);
GLOBAL_DEF(char *help_authors, "");
GLOBAL_DEF(char *help_login, "What are you known as traveller?");

/* externals for counting purposes */
GLOBAL_DEF(OBJ_DATA * obj_free, NULL);
GLOBAL_DEF(CHAR_DATA * char_free, NULL);
GLOBAL_DEF(PC_DATA * pcdata_free, NULL);
GLOBAL_DEF(bool fBootDb, false);
GLOBAL(MOB_INDEX_DATA * mob_index_hash[MAX_KEY_HASH]);
GLOBAL(OBJ_INDEX_DATA * obj_index_hash[MAX_KEY_HASH]);
GLOBAL_DEF(FILE * fpArea, NULL);
GLOBAL(char strArea[MAX_INPUT_LENGTH]);

GLOBAL_DEF(int nAllocPerm, 0);
GLOBAL_DEF(int nAllocString, 0);

GLOBAL_DEF(int maxSocial, 0);   /* Max number of socials */

GLOBAL_DEF(NOTE_DATA * icn_list, NULL);
GLOBAL_DEF(NOTE_DATA * penalty_list, NULL);
GLOBAL_DEF(NOTE_DATA * news_list, NULL);
GLOBAL_DEF(NOTE_DATA * changes_list, NULL);
GLOBAL_DEF(NOTE_DATA * weddings_list, NULL);
GLOBAL(char str_boot_time[MAX_INPUT_LENGTH]);

#endif
