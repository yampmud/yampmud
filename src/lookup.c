
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
#include <string.h>
#include <sqlite3.h>
#include "merc.h"
#include "tables.h"
#include "lookup.h"
#include "sql_io.h"

int flag_lookup(const char *name, const struct flag_type *flag_table)
{
  int flag;

  for (flag = 0; flag_table[flag].name != NULL; flag++)
  {
    if (LOWER(name[0]) == LOWER(flag_table[flag].name[0]) &&
        !str_prefix(name, flag_table[flag].name))
      return flag_table[flag].bit;
  }

  return 0;
}

int clan_lookup(const char *name)
{
  int clan;

  for (clan = 0; clan < MAX_CLAN; clan++)
  {
    if (LOWER(name[0]) == LOWER(clan_table[clan].name[0]) &&
        !str_prefix(name, clan_table[clan].name))
      return clan;
  }

  return 0;
}

int position_lookup(const char *name)
{
  int pos;

  for (pos = 0; position_table[pos].name != NULL; pos++)
  {
    if (LOWER(name[0]) == LOWER(position_table[pos].name[0]) &&
        !str_prefix(name, position_table[pos].name))
      return pos;
  }

  return -1;
}

int sex_lookup(const char *name)
{
  int sex;

  for (sex = 0; sex_table[sex].name != NULL; sex++)
  {
    if (LOWER(name[0]) == LOWER(sex_table[sex].name[0]) &&
        !str_prefix(name, sex_table[sex].name))
      return sex;
  }

  return -1;
}

int size_lookup(const char *name)
{
  int size;

  for (size = 0; size_table[size].name != NULL; size++)
  {
    if (LOWER(name[0]) == LOWER(size_table[size].name[0]) &&
        !str_prefix(name, size_table[size].name))
      return size;
  }

  return -1;
}

HELP_DATA *help_lookup(char *keyword)
{
  sqlite3_stmt *stmt;
  char *sql;
  const char *tail;
  int rc;
  long long id;

  char temp[MIL], argall[MIL];

  argall[0] = '\0';

  while (keyword[0] != '\0')
  {
    keyword = one_argument(keyword, temp);
    if (argall[0] != '\0')
      strcat(argall, " ");
    strcat(argall, temp);
  }

  sql =
    sqlite3_mprintf
    ("SELECT id,keyword FROM helps WHERE keyword LIKE '%%%q%%'", keyword);
  rc = sqlite3_prepare(world_db, sql, strlen(sql), &stmt, &tail);

  if (rc != SQLITE_OK)
  {
    sprintf(log_buf, "SQL error: %s", sqlite3_errmsg(world_db));
    log_string(log_buf);
    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    return NULL;
  }

  rc = sqlite3_step(stmt);

  while (rc == SQLITE_ROW)
  {
    if (is_name(argall, (char *) sqlite3_column_text(stmt, 1)))
    {
      id = sqlite3_column_int64(stmt, 0);
      sqlite3_finalize(stmt);
      sqlite3_free(sql);
      return fetch_help(id);
    }
    rc = sqlite3_step(stmt);
  }

  sqlite3_finalize(stmt);
  sqlite3_free(sql);
  return NULL;
}

int area_lookup(const char *name)
{
  AREA_DATA *pArea = NULL;
  char buf[MAX_INPUT_LENGTH];

  snprintf(buf, MAX_INPUT_LENGTH, "%s.are", name);

  for (pArea = area_first; pArea != NULL; pArea = pArea->next)
  {
    if (!strcmp(buf, pArea->file_name))
      break;
  }

  if (pArea)
    return pArea->vnum;
  else
    return 0;
}
