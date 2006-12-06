/* SQLite3 Data Backend Storage and Retrieval */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sqlite3.h>

#include "merc.h"

#define DATA_DIR "../data/"
#define WORLD_DB_FILE DATA_DIR "world.db"

AREA_DATA *fetch_area(long long id)
{
  AREA_DATA *pArea;

  sqlite3 *db;
  sqlite3_stmt *stmt;
  int rc;
  char *sql;
  const char *tail;

  rc = sqlite3_open(WORLD_DB_FILE, &db);

  if (rc)
  {
    sprintf(log_buf, "Can't open database: %s", sqlite3_errmsg(db));
    log_string(log_buf);
    sqlite3_close(db);
    return NULL;
  }

  pArea = new_area();

  sql =
    sqlite3_mprintf
    ("SELECT age, area_flags, builders, credits, empty, file_name, high_range, low_range, max_vnum, min_vnum, name, nplayer, repop_message, security FROM areas WHERE id=%d",
     id);
  rc = sqlite3_prepare(db, sql, strlen(sql), &stmt, &tail);

  if (rc != SQLITE_OK)
  {
    sprintf(log_buf, "SQL error: %s", sqlite3_errmsg(db));
    log_string(log_buf);
    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    sqlite3_close(db);
    return NULL;
  }

  rc = sqlite3_step(stmt);

  if (rc == SQLITE_ROW)
  {
    pArea->age = sqlite3_column_int(stmt, 0);
    pArea->area_flags = 1;
    free_string(pArea->builders);
    pArea->builders = str_dup((char *) sqlite3_column_text(stmt, 2));
    free_string(pArea->credits);
    pArea->credits = str_dup((char *) sqlite3_column_text(stmt, 3));
    pArea->empty = sqlite3_column_int(stmt, 4);
    free_string(pArea->file_name);
    pArea->file_name = str_dup((char *) sqlite3_column_text(stmt, 5));
    pArea->high_range = sqlite3_column_int(stmt, 6);
    pArea->low_range = sqlite3_column_int(stmt, 7);
    pArea->max_vnum = sqlite3_column_int(stmt, 8);
    pArea->min_vnum = sqlite3_column_int(stmt, 9);
    free_string(pArea->name);
    pArea->name = str_dup((char *) sqlite3_column_text(stmt, 10));
    pArea->nplayer = sqlite3_column_int(stmt, 11);
    free_string(pArea->repop_msg);
    pArea->repop_msg = str_dup((char *) sqlite3_column_text(stmt, 12));
    pArea->security = sqlite3_column_int(stmt, 13);

    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    sqlite3_close(db);
    return pArea;
  }


  sqlite3_finalize(stmt);
  sqlite3_free(sql);
  sqlite3_close(db);
  return NULL;
}

int store_area(AREA_DATA * pArea)
{
  sqlite3 *db;
  char *sql;
  char *zErr;
  int rc;

  rc = sqlite3_open(WORLD_DB_FILE, &db);

  if (rc)
  {
    sprintf(log_buf, "Can't open database: %s", sqlite3_errmsg(db));
    log_string(log_buf);
    sqlite3_close(db);
    return rc;
  }

  sql =
    sqlite3_mprintf
    ("INSERT OR REPLACE INTO areas (age, area_flags, builders, credits, empty, file_name, high_range, low_range, max_vnum, min_vnum, name, nplayer, repop_message, security) VALUES (%d, %Q, %Q, %Q, %d, %Q, %d, %d, %d, %d, %Q, %d, %Q, %d)",
     pArea->age, pArea->area_flags, pArea->builders, pArea->credits,
     pArea->empty, pArea->file_name, pArea->high_range, pArea->low_range,
     pArea->max_vnum, pArea->min_vnum, pArea->name, pArea->nplayer,
     pArea->repop_msg, pArea->security);
  rc = sqlite3_exec(db, sql, NULL, NULL, &zErr);

  if (rc != SQLITE_OK)
    if (zErr != NULL)
    {
      sprintf(log_buf, "SQL error: %s\n", zErr);
      log_string(log_buf);
      sqlite3_free(zErr);
    }

  if (!pArea->id)
    pArea->id = sqlite3_last_insert_rowid(db);

  sqlite3_free(sql);
  sqlite3_close(db);
  return 0;
}

RESET_DATA *fetch_reset(long long id)
{
  return NULL;
}

void store_reset(RESET_DATA * pReset)
{
  return;
}
