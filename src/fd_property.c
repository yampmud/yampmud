//
// Copyright (c) 2000
//      The staff of Fatal Dimensions.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY FATAL DIMENSIONS AND CONTRIBUTORS ``AS
// IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
// STAFF OF FATAL DIMENSIONS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "merc.h"
#include "fd_property.h"
#include "recycle.h"
#include "tables.h"
#include "str_util.h"

#define PROPERTY_FILE "../config/properties.txt"

PROPERTY_INDEX_TYPE coded_properties[] = {
  {0, NULL, NULL, 0}
};

PROPERTY_INDEX_TYPE *property_head = NULL;

int table_find_name(const char *name, const struct table_type *table)
{
  int i;

  for (i = 0; table[i].name != NULL; i++)
    if (str_cmp(name, table[i].name) == 0)
      return table[i].value;
  return NO_FLAG;
}

char *table_find_value(int value, const struct table_type *table)
{
  int i;

  for (i = 0; table[i].name != NULL; i++)
    if (table[i].value == value)
      return table[i].name;
  return "(unknown)";
}


//
// Return an number of spaces with a maximum of 80
//
char *spaces(int len)
{
  static char *spacearray =
    "                                        "
    "                                        ";

  if (len < 0)
    len = 0;
  if (len > 80)
    len = 80;
  return spacearray + 80 - len;
}

void do_propertylist(CHAR_DATA * ch, char *argument)
{
  BUFFER *output;
  PROPERTY_INDEX_TYPE *propIndex;
  char s[MSL];
  int i;
  char *p;

  output = new_buf();
  i = 1;
  propIndex = property_head;
  while (propIndex)
  {
    p = table_find_value(propIndex->type, property_table);
    sprintf(s, "%3d. %s / %s%s", i,
            propIndex->key, p,
            spaces(39 - 5 - strlen(p) - 3 - strlen(propIndex->key)));
    add_buf(output, s);
    if (i % 2 == 0)
      add_buf(output, "\n\r");
    i++;
    propIndex = propIndex->next;
  }

  if (i % 2 == 0)
    add_buf(output, "\n\r");

  send_to_char(buf_string(output), ch);
  free_buf(output);
  printf_to_char(ch, "Done.\r\n");
}

PROPERTY_INDEX_TYPE *get_property_index_from_list(PROPERTY_INDEX_TYPE *
                                                  listhead, char *key,
                                                  int type)
{
  PROPERTY_INDEX_TYPE *prop = listhead;

  while (prop)
  {
    if (type == prop->type && !str_cmp(key, prop->key))
      return prop;
    prop = prop->next;
  }

  {
    bugf("Get_property_index: Unknown property index requested. "
         "Key: %s, type: %d\n", key, type);
    return NULL;
  }
}
PROPERTY_INDEX_TYPE *get_property_index(char *key, int type)
{
  return get_property_index_from_list(property_head, key, type);
}

bool does_property_exist_in_list(PROPERTY_INDEX_TYPE * propIndex, char *key,
                                 int type)
{
  while (propIndex)
  {
    if (type == propIndex->type && !str_cmp(key, propIndex->key))
      return true;
    propIndex = propIndex->next;
  }
  return false;
}

bool does_property_exist(char *key, int type)
{
  return does_property_exist_in_list(property_head, key, type);
}


bool does_property_exist_s(char *key, char *type)
{

  if (!str_cmp(type, "string"))
  {
    return does_property_exist(key, PROPERTY_STRING);
  }
  else if (!str_cmp(type, "long"))
  {
    return does_property_exist(key, PROPERTY_LONG);
  }
  else if (!str_cmp(type, "int"))
  {
    return does_property_exist(key, PROPERTY_INT);
  }
  else if (!str_cmp(type, "bool"))
  {
    return does_property_exist(key, PROPERTY_BOOL);
  }
  else if (!str_cmp(type, "char"))
  {
    return does_property_exist(key, PROPERTY_CHAR);
  }
  return false;
}

/*****************************************************************************/

void set_property(PROPERTY * prop, int type, char *key, void *value)
{
  PROPERTY_INDEX_TYPE *propIndex = get_property_index(key, type);

  if (propIndex == NULL)
    return;

  prop->propIndex = get_property_index(key, type);

  if (type == PROPERTY_CHAR)
  {
    prop->iValue = (long) *(char *) value;
    return;
  }
  if (type == PROPERTY_STRING)
  {
    prop->sValue = str_dup((char *) value);
    return;
  }
  if (type == PROPERTY_BOOL)
  {
    prop->iValue = (long) *(bool *) value;
    return;
  }
  if (type == PROPERTY_LONG)
  {
    prop->iValue = (long) *(long *) value;
    return;
  }
  if (type == PROPERTY_INT)
  {
    prop->iValue = (long) *(int *) value;
    return;
  }

  prop->sValue = str_dup(key);
  return;
}

/*****************************************************************************/

int SetRoomProperty(ROOM_INDEX_DATA * room, int type, char *key, void *value)
{
  PROPERTY *prop;

  if (room == NULL)
    return 0;
  if (key == NULL || key[0] == 0)
    return 1;

  if (!does_property_exist(key, type))
  {
    bugf("[0] Unknown property requested: SetRoomProperty(%s,%d)", key, type);
    return 1;
  }

  DeleteRoomProperty(room, type, key);

  prop = new_property();
  set_property(prop, type, key, value);
  prop->next = room->property;
  room->property = prop;

  return 0;
}

int SetDObjectProperty(OBJ_INDEX_DATA * obj, int type, char *key, void *value)
{
  PROPERTY *prop;

  if (obj == NULL)
    return 0;
  if (key == NULL || key[0] == 0)
    return 1;

  if (!does_property_exist(key, type))
  {
    bugf("[0] Unknown property requested: SetDObjectProperty(%s,%d)",
         key, type);
    return 1;
  }

  DeleteDObjectProperty(obj, type, key);

  prop = new_property();
  set_property(prop, type, key, value);
  prop->next = obj->property;
  obj->property = prop;

  return 0;
}

int SetObjectProperty(OBJ_DATA * obj, int type, char *key, void *value)
{
  PROPERTY *prop;

  if (obj == NULL)
    return 0;
  if (key == NULL || key[0] == 0)
    return 1;

  if (!does_property_exist(key, type))
  {
    bugf("[0] Unknown property requested: SetObjectProperty(%s,%d)",
         key, type);
    return 1;
  }

  DeleteObjectProperty(obj, type, key);

  prop = new_property();
  set_property(prop, type, key, value);
  prop->next = obj->property;
  obj->property = prop;

  return 0;
}

int SetDCharProperty(MOB_INDEX_DATA * ch, int type, char *key, void *value)
{
  PROPERTY *prop;

  if (ch == NULL)
    return 0;
  if (key == NULL || key[0] == 0)
    return 1;

  if (!does_property_exist(key, type))
  {
    bugf("[0] Unknown property requested: SetDCharProperty(%s,%d)",
         key, type);
    return 1;
  }

  DeleteDCharProperty(ch, type, key);

  prop = new_property();
  set_property(prop, type, key, value);
  prop->next = ch->property;
  ch->property = prop;

  return 0;
}

int SetCharProperty(CHAR_DATA * ch, int type, char *key, void *value)
{
  PROPERTY *prop;

  if (ch == NULL)
    return 0;
  if (key == NULL || key[0] == 0)
    return 1;

  if (!does_property_exist(key, type))
  {
    bugf("[0] Unknown property requested: SetCharProperty(%s,%d)", key, type);
    return 1;
  }

  DeleteCharProperty(ch, type, key);

  prop = new_property();
  set_property(prop, type, key, value);
  prop->next = ch->property;
  ch->property = prop;

  return 0;
}

/*****************************************************************************/
int get_property(PROPERTY * property, int object_type, char *key, void *value)
{
  while (property)
  {
    if (!str_cmp(property->propIndex->key, key) &&
        property->propIndex->type == object_type)
    {

      if (value == NULL)
        return 1;               // found the property, but got nowhere to put it.

      if (object_type == PROPERTY_CHAR)
      {
        *(char *) value = property->iValue;
        return 1;
      }
      if (object_type == PROPERTY_STRING)
      {
        strcpy(value, property->sValue);
        return 1;
      }
      if (object_type == PROPERTY_BOOL)
      {
        *(bool *) value = property->iValue;
        return 1;
      }
      if (object_type == PROPERTY_LONG)
      {
        *(long *) value = property->iValue;
        return 1;
      }
      if (object_type == PROPERTY_INT)
      {
        *(int *) value = property->iValue;
        return 1;
      }
    }
    property = property->next;
  }
  return 0;
}

int GetRoomProperty(ROOM_INDEX_DATA * room, int object_type, char *key,
                    void *value)
{
  if (room == NULL)
    return 0;
  if (get_property(room->property, object_type, key, value))
    return 1;
  return 0;
}

int GetObjectProperty(OBJ_DATA * obj, int object_type, char *key, void *value)
{
  if (obj == NULL)
    return 0;
  if (get_property(obj->property, object_type, key, value))
    return 1;
  if (obj->pIndexData &&
      get_property(obj->pIndexData->property, object_type, key, value))
    return 1;
  return 0;
}

int GetCharProperty(CHAR_DATA * ch, int object_type, char *key, void *value)
{
  if (ch == NULL)
    return 0;
  if (get_property(ch->property, object_type, key, value))
    return 1;
  if (ch->pIndexData &&
      get_property(ch->pIndexData->property, object_type, key, value))
    return 1;
  return 0;
}

/*****************************************************************************/

int DeleteRoomProperty(ROOM_INDEX_DATA * room, int object_type, char *key)
{
  PROPERTY *prop, *tempprop;

  if (room == NULL || room->property == NULL)
    return 0;

  prop = room->property;
  if (prop->propIndex->type == object_type &&
      !str_cmp(prop->propIndex->key, key))
  {
    room->property = prop->next;
    free_property(prop);
    return 0;
  }

  while (prop->next)
  {
    tempprop = prop->next;
    if (tempprop->propIndex->type == object_type &&
        !str_cmp(tempprop->propIndex->key, key))
    {
      prop->next = tempprop->next;
      free_property(tempprop);
      return 0;
    }
    prop = prop->next;
  }

  return 0;
}

int DeleteObjectProperty(OBJ_DATA * obj, int object_type, char *key)
{
  PROPERTY *prop, *tempprop;

  if (obj == NULL || obj->property == NULL)
    return 0;

  prop = obj->property;
  if (prop->propIndex->type == object_type &&
      !str_cmp(prop->propIndex->key, key))
  {
    obj->property = prop->next;
    free_property(prop);
    return 0;
  }

  while (prop->next)
  {
    tempprop = prop->next;
    if (tempprop->propIndex->type == object_type &&
        !str_cmp(tempprop->propIndex->key, key))
    {
      prop->next = tempprop->next;
      free_property(tempprop);
      return 0;
    }
    prop = prop->next;
  }

  return 0;
}

int DeleteCharProperty(CHAR_DATA * ch, int object_type, char *key)
{
  PROPERTY *prop, *tempprop;

  if (ch == NULL || ch->property == NULL)
    return 0;

  prop = ch->property;
  if (prop->propIndex->type == object_type &&
      !str_cmp(prop->propIndex->key, key))
  {
    ch->property = prop->next;
    free_property(prop);
    return 0;
  }

  while (prop->next)
  {
    tempprop = prop->next;
    if (tempprop->propIndex->type == object_type &&
        !str_cmp(tempprop->propIndex->key, key))
    {
      prop->next = tempprop->next;
      free_property(tempprop);
      return 0;
    }
    prop = prop->next;
  }

  return 0;
}

int DeleteDObjectProperty(OBJ_INDEX_DATA * obj, int object_type, char *key)
{
  PROPERTY *prop, *tempprop;

  if (obj == NULL || obj->property == NULL)
    return 0;

  prop = obj->property;
  if (prop->propIndex->type == object_type &&
      !str_cmp(prop->propIndex->key, key))
  {
    obj->property = prop->next;
    free_property(prop);
    return 0;
  }

  while (prop->next)
  {
    tempprop = prop->next;
    if (tempprop->propIndex->type == object_type &&
        !str_cmp(tempprop->propIndex->key, key))
    {
      prop->next = tempprop->next;
      free_property(tempprop);
      return 0;
    }
    prop = prop->next;
  }

  return 0;
}

int DeleteDCharProperty(MOB_INDEX_DATA * ch, int object_type, char *key)
{
  PROPERTY *prop, *tempprop;

  if (ch == NULL || ch->property == NULL)
    return 0;

  prop = ch->property;
  if (prop->propIndex->type == object_type &&
      !str_cmp(prop->propIndex->key, key))
  {
    ch->property = prop->next;
    free_property(prop);
    return 0;
  }

  while (prop->next)
  {
    tempprop = prop->next;
    if (tempprop->propIndex->type == object_type &&
        !str_cmp(tempprop->propIndex->key, key))
    {
      prop->next = tempprop->next;
      free_property(tempprop);
      return 0;
    }
    prop = prop->next;
  }

  return 0;
}

/*****************************************************************************/


void show_properties(CHAR_DATA * ch, PROPERTY * prop, char *prefix)
{
  while (prop)
  {
    printf_to_char(ch, "{y%s-property: {x%-20s %-6s",
                   prefix,
                   prop->propIndex->key,
                   table_find_value(prop->propIndex->type, property_table));
    switch (prop->propIndex->type)
    {
      default:
        send_to_char("(undef)", ch);
        break;
      case PROPERTY_INT:
        printf_to_char(ch, " %d\n\r", (int) prop->iValue);
        break;
      case PROPERTY_LONG:
        printf_to_char(ch, " %ld\n\r", (long) prop->iValue);
        break;
      case PROPERTY_BOOL:
        printf_to_char(ch, " %s\n\r", prop->iValue == 0 ? "false" : "true");
        break;
      case PROPERTY_STRING:
        printf_to_char(ch, " '%s'\n\r", prop->sValue);
        break;
      case PROPERTY_CHAR:
        printf_to_char(ch, " %c\n\r", (char) prop->iValue);
        break;
    }
    prop = prop->next;
  }
}

void save_property(FILE * fp, char *prefix, PROPERTY * prop, char *function,
                   char *name)
{
  if (!prop)
    return;
  save_property(fp, prefix, prop->next, function, name);

  switch (prop->propIndex->type)
  {
    case PROPERTY_INT:
      fprintf(fp, "%s %s~ int~ %d~\n",
              prefix, prop->propIndex->key, (int) prop->iValue);
      break;
    case PROPERTY_LONG:
      fprintf(fp, "%s %s~ long~ %ld~\n",
              prefix, prop->propIndex->key, prop->iValue);
      break;
    case PROPERTY_BOOL:
      if (prop->iValue)
        fprintf(fp, "%s %s~ bool~ true~\n", prefix, prop->propIndex->key);
      else
        fprintf(fp, "%s %s~ bool~ false~\n", prefix, prop->propIndex->key);
      break;
    case PROPERTY_STRING:
      smash_tilde(prop->sValue);
      fprintf(fp, "%s %s~ string~ %s~\n",
              prefix, prop->propIndex->key, prop->sValue);
      break;
    case PROPERTY_CHAR:
      fprintf(fp, "%s %s~ char~ %c~\n",
              prefix, prop->propIndex->key, (char) prop->iValue);
      break;
    case PROPERTY_UNDEF:
    default:
      {
        bugf("%s: undef property for writing %s on %s.",
             function, name, prop->propIndex->key);
        break;
      }
  }
}

bool add_property_index_to_list(PROPERTY_INDEX_TYPE ** listhead,
                                PROPERTY_INDEX_TYPE * propIndex)
{
  if (does_property_exist(propIndex->key, propIndex->type))
    return false;
  propIndex->next = *listhead;
  *listhead = propIndex;
  return true;
}

bool add_property_index(PROPERTY_INDEX_TYPE * propIndex)
{
  return add_property_index_to_list(&property_head, propIndex);
}

bool remove_property_index_from_list(PROPERTY_INDEX_TYPE ** listhead,
                                     PROPERTY_INDEX_TYPE * propIndex)
{
  PROPERTY_INDEX_TYPE *prop;
  if (*listhead == propIndex)
  {
    *listhead = propIndex->next;
    propIndex->next = NULL;
    return true;
  }
  for (prop = *listhead; prop->next != propIndex; prop = prop->next)
  {
    if (!prop->next)
      return false;
  }
  prop->next = prop->next->next;
  propIndex->next = NULL;
  return true;
}

bool remove_property_index(PROPERTY_INDEX_TYPE * propIndex)
{
  return remove_property_index_from_list(&property_head, propIndex);
}

void load_properties(void)
{
  FILE *fin;
  PROPERTY_INDEX_TYPE *propIndex;
  char *word;
  int n;

  // first read the hardcoded properties

  for (n = 0; coded_properties[n].key; n++)
  {
    coded_properties[n].valid = true;
    add_property_index(&coded_properties[n]);
  }

  // and now the soft-coded properties

  if ((fin = fopen(PROPERTY_FILE, "rt")) == NULL)
  {
    mudlogf("[0] load_properties(): fopen(%s): %s",
            PROPERTY_FILE, strerror(errno));
    bugf("Can't open %s", PROPERTY_FILE);
    return;
  }
  while (!feof(fin))
  {
    word = fread_word(fin);
    if (word[0] == '#')
    {
      fread_to_eol(fin);
      continue;
    }

    propIndex = new_property_index();
    propIndex->next = NULL;
    propIndex->key = str_dup(word);

    word = fread_word(fin);
    if (!strcmp(word, "bool"))
    {
      propIndex->type = PROPERTY_BOOL;
    }
    else if (!strcmp(word, "long"))
    {
      propIndex->type = PROPERTY_LONG;
    }
    else if (!strcmp(word, "int"))
    {
      propIndex->type = PROPERTY_INT;
    }
    else if (!strcmp(word, "char"))
    {
      propIndex->type = PROPERTY_CHAR;
    }
    else if (!strcmp(word, "string"))
    {
      propIndex->type = PROPERTY_STRING;
    }
    else
    {
      bugf("Unknown property-type found: '%s'\n", word);
      exit(0);
    }
    fread_to_eol(fin);

    if (!add_property_index(propIndex))
    {
      bugf("Duplicate property %s type %s", propIndex->key, word);
      free_property_index(propIndex);
    }
  }
  fclose(fin);
}

void save_properties(void);

void do_propedit(CHAR_DATA * ch, char *argument)
{
  char op[MAX_STRING_LENGTH];
  char stype[MAX_STRING_LENGTH];
  char key[MAX_STRING_LENGTH];

  int action, type;
  PROPERTY_INDEX_TYPE *pProp;


  argument = one_argument(argument, op);
  argument = one_argument(argument, key);
  argument = one_argument(argument, stype);

  action = which_keyword(op, "add", "delete", NULL);
  type = table_find_name(stype, property_table);

  if (action < 1 || action > 2)
  {
    send_to_char("Usage: propedit add|delete <key> <type>\r\n", ch);
    return;
  }

  if (type == NO_FLAG)
  {
    printf_to_char(ch, "%s is not a valid property type.\r\n", stype);
    return;
  }

  switch (action)
  {
    case 1:                    // add
      if (!does_property_exist(key, type))
      {
        pProp = new_property_index();
        pProp->type = type;
        pProp->key = str_dup(key);
        add_property_index(pProp);
        printf_to_char(ch, "Property %s added with type %s.\r\n", key, stype);
      }
      break;
    case 2:                    // delete
      pProp = get_property_index(key, type);
      remove_property_index(pProp);
      free_property_index(pProp);
      break;
    default:
      send_to_char("You shouldn't see this.", ch);
  }

  save_properties();
  return;
}


void save_properties(void)
{
  FILE *fp;

  PROPERTY_INDEX_TYPE *pProp;

  if ((fp = fopen(PROPERTY_FILE, "w")) == NULL)
  {
    mudlogf("save_properties(): fopen(%s): %s",
            PROPERTY_FILE, strerror(errno));
    bugf("Can't open %s", PROPERTY_FILE);
    return;
  }

  fprintf(fp, "#PROPERTIES\n");

  for (pProp = property_head; pProp; pProp = pProp->next)
  {
    fprintf(fp, "%s %s\n", pProp->key,
            table_find_value(pProp->type, property_table));
  }

  fclose(fp);
  return;
}
