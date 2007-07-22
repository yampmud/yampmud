#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "str_util.h"
#include "lookup.h"

CH_CMD(do_rlist)
{
  ROOM_INDEX_DATA *pRoomIndex;
  AREA_DATA *pArea;
  char buf[MAX_STRING_LENGTH];
  BUFFER *buf1 = new_buf();
  char arg[MAX_INPUT_LENGTH];
  bool found = false;
  long vnum;
  long area_vnum = 0;
  int col = 0;

  one_argument(argument, arg);

  if (arg[0] != '\0')
  {
    if (isdigit(arg[0]))
    {
      area_vnum = atoi(arg);
      if (area_vnum <= 0 || area_vnum > top_area)
      {
        sprintf(buf, "Area vnum must be between 1 and %d.\n\r", top_area);
        send_to_char(buf, ch);
        return;
      }
    }
    else
    {
      if ((area_vnum = area_lookup(arg)) == 0)
      {
        sprintf(buf, "Unknown area: %s\n\r", arg);
        send_to_char(buf, ch);
        return;
      }
    }
    for (pArea = area_first; pArea != NULL; pArea = pArea->next)
    {
      if (pArea->vnum == area_vnum)
      {
        found = true;
        break;
      }
    }
  }

  if (!found)
    pArea = ch->in_room->area;

  for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
  {
    if ((pRoomIndex = get_room_index(vnum)))
    {
      found = true;
      sprintf(buf, "[%-6ld] %-17.16s ", vnum,
              strip_color(capitalize(pRoomIndex->name)));
      add_buf(buf1, buf);
      if (++col % 3 == 0)
        add_buf(buf1, "\n\r");
    }
  }

  if (!found)
  {
    send_to_char("Room(s) not found in this area.\n\r", ch);
    free_buf(buf1);
    return;
  }

  if (col % 3 != 0)
    add_buf(buf1, "\n\r");

  send_to_char(buf_string(buf1), ch);
  free_buf(buf1);
  return;
}

CH_CMD(do_mlist)
{
  MOB_INDEX_DATA *pMobIndex;
  AREA_DATA *pArea;
  char buf[MAX_STRING_LENGTH];
  BUFFER *buf1 = new_buf();
  char arg1[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  bool fAll, found = false;
  long vnum;
  long area_vnum = 0;
  int col = 0;

  argument = one_argument(argument, arg1);
  argument = one_argument(argument, arg2);

  if (arg1[0] == '\0')
  {
    send_to_char("Syntax:  mlist <all/name> [area vnum/name]\n\r", ch);
    return;
  }

  if (arg2[0] != '\0')
  {
    if (isdigit(arg2[0]))
    {

      area_vnum = atoi(arg2);
      if (area_vnum <= 0 || area_vnum > top_area)
      {
        sprintf(buf, "Area vnum must be between 1 and %d.\n\r", top_area);
        send_to_char(buf, ch);
        return;
      }
    }
    else
    {
      if ((area_vnum = area_lookup(arg2)) == 0)
      {
        sprintf(buf, "Unknown area: %s\n\r", arg1);
        send_to_char(buf, ch);
        return;
      }
    }
    for (pArea = area_first; pArea != NULL; pArea = pArea->next)
    {
      if (pArea->vnum == area_vnum)
      {
        found = true;
        break;
      }
    }
  }

  if (!found)
    pArea = ch->in_room->area;

  fAll = !str_cmp(arg1, "all");
  found = false;

  for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
  {
    if ((pMobIndex = get_mob_index(vnum)) != NULL)
    {
      if (fAll || is_name(arg1, pMobIndex->player_name))
      {
        found = true;
        sprintf(buf, "[%-6ld] %-17.16s ", pMobIndex->vnum,
                strip_color(capitalize(pMobIndex->short_descr)));
        add_buf(buf1, buf);
        if (++col % 3 == 0)
          add_buf(buf1, "\n\r");
      }
    }
  }

  if (!found)
  {
    send_to_char("Mobile(s) not found in this area.\n\r", ch);
    free_buf(buf1);
    return;
  }

  if (col % 3 != 0)
    add_buf(buf1, "\n\r");

  send_to_char(buf_string(buf1), ch);
  free_buf(buf1);
  return;
}

CH_CMD(do_olist)
{
  OBJ_INDEX_DATA *pObjIndex;
  AREA_DATA *pArea;
  char buf[MAX_STRING_LENGTH];
  BUFFER *buf1 = new_buf();
  char arg[MAX_INPUT_LENGTH];
  char arg2[MAX_INPUT_LENGTH];
  bool fAll, found = false;
  long vnum;
  long area_vnum = 0;
  int col = 0;

  argument = one_argument(argument, arg);
  argument = one_argument(argument, arg2);

  if (arg[0] == '\0')
  {
    send_to_char("Syntax:  olist <all/name/item_type> [area vnum/name]\n\r",
                 ch);
    return;
  }

  if (arg2[0] != '\0')
  {
    if (isdigit(arg2[0]))
    {
      area_vnum = atoi(arg2);
      if (area_vnum <= 0 || area_vnum > top_area)
      {
        sprintf(buf, "Area vnum must be between 1 and %d.\n\r", top_area);
        send_to_char(buf, ch);
        return;
      }
    }
    else
    {
      if ((area_vnum = area_lookup(arg2)) == 0)
      {
        sprintf(buf, "Unknown area: %s\n\r", arg2);
        send_to_char(buf, ch);
        return;
      }
    }
    for (pArea = area_first; pArea != NULL; pArea = pArea->next)
    {
      if (pArea->vnum == area_vnum)
      {
        found = true;
        break;
      }
    }
  }

  if (!found)
    pArea = ch->in_room->area;

  fAll = !str_cmp(arg, "all");
  found = false;

  for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
  {
    if ((pObjIndex = get_obj_index(vnum)))
    {
      if (fAll || is_name(arg, pObjIndex->name) ||
          flag_value(type_flags, arg) == pObjIndex->item_type)
      {
        found = true;
        sprintf(buf, "[%-6ld] %-17.16s", pObjIndex->vnum,
                strip_color(capitalize(pObjIndex->short_descr)));
        add_buf(buf1, buf);
        if (++col % 3 == 0)
          add_buf(buf1, "\n\r");
      }
    }
  }

  if (!found)
  {
    send_to_char("Object(s) not found in this area.\n\r", ch);
    free_buf(buf1);
    return;
  }

  if (col % 3 != 0)
    add_buf(buf1, "\n\r");

  send_to_char(buf_string(buf1), ch);
  free_buf(buf1);
  return;
}
