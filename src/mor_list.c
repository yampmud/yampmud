#include <stdio.h>
#include <time.h>

#include "merc.h"
#include "recycle.h"
#include "tables.h"
#include "str_util.h"

CH_CMD(do_rlist)
{
  ROOM_INDEX_DATA *pRoomIndex;
  AREA_DATA *pArea;
  char buf[MAX_STRING_LENGTH];
  BUFFER *buf1;
  char arg[MAX_INPUT_LENGTH];
  bool found;
  long vnum;
  int col = 0;

  one_argument(argument, arg);

  pArea = ch->in_room->area;
  buf1 = new_buf();
  /*    buf1[0] = '\0'; */
  found = false;

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

  page_to_char(buf_string(buf1), ch);
  free_buf(buf1);
  return;
}

CH_CMD(do_mlist)
{
  MOB_INDEX_DATA *pMobIndex;
  AREA_DATA *pArea;
  char buf[MAX_STRING_LENGTH];
  BUFFER *buf1;
  char arg[MAX_INPUT_LENGTH];
  bool fAll, found;
  long vnum;
  int col = 0;

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Syntax:  mlist <all/name>\n\r", ch);
    return;
  }

  buf1 = new_buf();
  pArea = ch->in_room->area;
  /*    buf1[0] = '\0'; */
  fAll = !str_cmp(arg, "all");
  found = false;

  for (vnum = pArea->min_vnum; vnum <= pArea->max_vnum; vnum++)
  {
    if ((pMobIndex = get_mob_index(vnum)) != NULL)
    {
      if (fAll || is_name(arg, pMobIndex->player_name))
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

  page_to_char(buf_string(buf1), ch);
  free_buf(buf1);
  return;
}

CH_CMD(do_olist)
{
  OBJ_INDEX_DATA *pObjIndex;
  AREA_DATA *pArea;
  char buf[MAX_STRING_LENGTH];
  BUFFER *buf1;
  char arg[MAX_INPUT_LENGTH];
  bool fAll, found;
  long vnum;
  int col = 0;

  one_argument(argument, arg);
  if (arg[0] == '\0')
  {
    send_to_char("Syntax:  olist <all/name/item_type>\n\r", ch);
    return;
  }

  pArea = ch->in_room->area;
  buf1 = new_buf();
  /*    buf1[0] = '\0'; */
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

  page_to_char(buf_string(buf1), ch);
  free_buf(buf1);
  return;
}
