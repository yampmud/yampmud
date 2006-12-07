/* SQLite3 Data Backend Storage and Retrieval */

void init_sqlite3(void);
void close_sqlite3(void);

#define DATA_DIR "../data/"
#define WORLD_DB_FILE DATA_DIR "world.db"

AREA_DATA *fetch_area(long long id);
int store_area(AREA_DATA * pArea);

RESET_DATA *fetch_reset(long long id);
void store_reset(RESET_DATA * pReset);

HELP_DATA *fetch_help(long long id);
int store_help(HELP_DATA * pHelp);
