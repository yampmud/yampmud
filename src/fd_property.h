//
// $Id: fd_property.h,v 1.12 2002/11/03 17:41:16 jodocus Exp $
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

#ifndef INCLUDED_FD_PROPERTY_H
#define INCLUDED_FD_PROPERTY_H

struct property_index_type
{
  bool valid;
  PROPERTY_INDEX_TYPE *next;
  char *key;
  int type;
};

struct property_type
{
  PROPERTY_INDEX_TYPE *propIndex;
  bool valid;
  long iValue;
  char *sValue;
  PROPERTY *next;               /* next property */
};

#define PROPERTY_UNDEF	-1
#define PROPERTY_INT	1
#define PROPERTY_LONG	2
#define	PROPERTY_BOOL	3
#define PROPERTY_STRING	4
#define PROPERTY_CHAR	5

int SetRoomProperty(ROOM_INDEX_DATA * room, int type, char *key, void *value);
int SetObjectProperty(OBJ_DATA * obj, int type, char *key, void *value);
int SetCharProperty(CHAR_DATA * ch, int type, char *key, void *value);
int SetDObjectProperty(OBJ_INDEX_DATA * obj, int type, char *key,
                       void *value);
int SetDCharProperty(MOB_INDEX_DATA * ch, int type, char *key, void *value);

int GetRoomProperty(ROOM_INDEX_DATA * room, int object_type, char *key,
                    void *value);
int GetObjectProperty(OBJ_DATA * obj, int object_type, char *key,
                      void *value);
int GetCharProperty(CHAR_DATA * ch, int object_type, char *key, void *value);


int DeleteRoomProperty(ROOM_INDEX_DATA * room, int object_type, char *key);
int DeleteObjectProperty(OBJ_DATA * obj, int object_type, char *key);
int DeleteCharProperty(CHAR_DATA * ch, int object_type, char *key);
int DeleteDObjectProperty(OBJ_INDEX_DATA * obj, int object_type, char *key);
int DeleteDCharProperty(MOB_INDEX_DATA * ch, int object_type, char *key);

bool add_property_index_to_list(PROPERTY_INDEX_TYPE ** listhead,
                                PROPERTY_INDEX_TYPE * propIndex);
bool add_property_index(PROPERTY_INDEX_TYPE * propIndex);
bool remove_property_index_from_list(PROPERTY_INDEX_TYPE ** listhead,
                                     PROPERTY_INDEX_TYPE * propIndex);
bool remove_property_index(PROPERTY_INDEX_TYPE * propIndex);
void load_properties(void);
void show_properties(CHAR_DATA * ch, PROPERTY * prop, char *prefix);
void save_property(FILE * fp, char *prefix, PROPERTY * prop, char *function,
                   char *name);
bool does_property_exist_in_list(PROPERTY_INDEX_TYPE * propIndex, char *key,
                                 int type);
bool does_property_exist(char *key, int type);
bool does_property_exist_s(char *key, char *type);

PROPERTY_INDEX_TYPE *get_property_index_from_list(PROPERTY_INDEX_TYPE *
                                                  listhead, char *key,
                                                  int type);
PROPERTY_INDEX_TYPE *get_property_index(char *key, int type);

#endif // INCLUDED_FD_PROPERTY_H
