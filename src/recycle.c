
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
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "merc.h"
#include "recycle.h"
#include "fd_property.h"

/* stuff for recycling ban structures */
BAN_DATA *new_ban(void)
{
  BAN_DATA *ban;

  ban = (BAN_DATA *) malloc(sizeof(*ban));
  memset(ban, 0, sizeof(*ban));

  VALIDATE(ban);
  ban->name = &str_empty[0];
  return ban;
}

void free_ban(BAN_DATA * ban)
{
  free_string(ban->name);
  INVALIDATE(ban);
  free(ban);
}

WIZ_DATA *new_wiz(void)
{
  WIZ_DATA *wiz;

  wiz = (WIZ_DATA *) malloc(sizeof(*wiz));
  memset(wiz, 0, sizeof(*wiz));

  VALIDATE(wiz);
  wiz->name = &str_empty[0];
  return wiz;
}

void free_wiz(WIZ_DATA * wiz)
{
  free_string(wiz->name);
  INVALIDATE(wiz);
  free(wiz);
}

CLN_DATA *new_cln(void)
{
  CLN_DATA *cln;

  cln = (CLN_DATA *) malloc(sizeof(*cln));
  memset(cln, 0, sizeof(*cln));

  VALIDATE(cln);
  cln->name = &str_empty[0];
  return cln;
}

void free_cln(CLN_DATA * cln)
{
  free_string(cln->name);
  INVALIDATE(cln);
  free(cln);
}

MBR_DATA *new_mbr(void)
{
  MBR_DATA *mbr;

  mbr = (MBR_DATA *) malloc(sizeof(*mbr));
  memset(mbr, 0, sizeof(*mbr));

  VALIDATE(mbr);
  mbr->name = &str_empty[0];
  return mbr;
}

void free_mbr(MBR_DATA * mbr)
{
  free_string(mbr->name);
  INVALIDATE(mbr);
  free(mbr);
}

DESCRIPTOR_DATA *new_descriptor(void)
{
  DESCRIPTOR_DATA *d;

  d = (DESCRIPTOR_DATA *) malloc(sizeof(*d));
  memset(d, 0, sizeof(*d));

  VALIDATE(d);

  d->connected = CON_GET_NAME;
  d->showstr_head = NULL;
  d->showstr_point = NULL;
  d->outsize = 2000;
  d->ansi = true;
  d->outbuf = malloc(d->outsize);
  memset(d->outbuf, 0, d->outsize);

  return d;
}

void free_descriptor(DESCRIPTOR_DATA * d)
{
  free_string(d->host);
  free(d->outbuf);
  INVALIDATE(d);
  free(d);
}

GEN_DATA *new_gen_data(void)
{
  GEN_DATA *gen;

  gen = (GEN_DATA *) malloc(sizeof(*gen));
  memset(gen, 0, sizeof(*gen));

  VALIDATE(gen);
  return gen;
}

void free_gen_data(GEN_DATA * gen)
{
  if (!IS_VALID(gen))
    return;

  INVALIDATE(gen);
  free(gen);
}

EXTRA_DESCR_DATA *new_extra_descr(void)
{
  EXTRA_DESCR_DATA *ed;

  ed = (EXTRA_DESCR_DATA *) malloc(sizeof(*ed));
  memset(ed, 0, sizeof(*ed));
  top_ed++;

  ed->keyword = &str_empty[0];
  ed->description = &str_empty[0];
  VALIDATE(ed);
  return ed;
}

void free_extra_descr(EXTRA_DESCR_DATA * ed)
{
  top_ed--;
  free_string(ed->keyword);
  free_string(ed->description);
  INVALIDATE(ed);
  free(ed);
}

AFFECT_DATA *new_affect(void)
{
  AFFECT_DATA *af;

  af = (AFFECT_DATA *) malloc(sizeof(*af));
  memset(af, 0, sizeof(*af));

  top_affect++;

  VALIDATE(af);
  return af;
}

void free_affect(AFFECT_DATA * af)
{
  top_affect--;
  INVALIDATE(af);
  free(af);
}

OBJ_DATA *new_obj(void)
{
  OBJ_DATA *obj;

  obj = (OBJ_DATA *) malloc(sizeof(*obj));
  memset(obj, 0, sizeof(*obj));

  VALIDATE(obj);

  return obj;
}

void free_obj(OBJ_DATA * obj)
{
  AFFECT_DATA *paf, *paf_next;
  EXTRA_DESCR_DATA *ed, *ed_next;
  PROPERTY *pProp, *pProp_next;

  if (!IS_VALID(obj))
    return;

  for (paf = obj->affected; paf != NULL; paf = paf_next)
  {
    paf_next = paf->next;
    free_affect(paf);
  }
  obj->affected = NULL;

  for (ed = obj->extra_descr; ed != NULL; ed = ed_next)
  {
    ed_next = ed->next;
    free_extra_descr(ed);
  }
  obj->extra_descr = NULL;

  free_string(obj->name);
  free_string(obj->description);
  free_string(obj->short_descr);
  free_string(obj->owner);
  free_string(obj->killer);

  pProp = obj->property;
  while (pProp)
  {
    pProp_next = pProp->next;
    free_property(pProp);
    pProp = pProp_next;
  }

  INVALIDATE(obj);
  free(obj);
}

CHAR_DATA *new_char(void)
{
  CHAR_DATA *ch;
  int i;

  ch = (CHAR_DATA *) malloc(sizeof(*ch));
  memset(ch, 0, sizeof(*ch));

  VALIDATE(ch);
  ch->name = &str_empty[0];
  ch->short_descr = &str_empty[0];
  ch->long_descr = &str_empty[0];
  ch->description = &str_empty[0];
  ch->prompt = &str_empty[0];
  ch->prefix = &str_empty[0];
  ch->die_descr = &str_empty[0];
  ch->say_descr = &str_empty[0];
  ch->material = &str_empty[0];
  ch->logon = current_time;
  ch->lines = PAGELEN;
  for (i = 0; i < 4; i++)
    ch->armor[i] = 100;
  ch->position = POS_STANDING;
  ch->hit = 100;
  ch->max_hit = 100;
  ch->mana = 100;
  ch->max_mana = 100;
  ch->move = 100;
  ch->max_move = 100;
  ch->nameauthed = 0;
  ch->namedenied = 0;
  for (i = 0; i < MAX_STATS; i++)
  {
    ch->perm_stat[i] = 13;
    ch->mod_stat[i] = 0;
  }

  return ch;
}

void free_char(CHAR_DATA * ch)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  AFFECT_DATA *paf;
  AFFECT_DATA *paf_next;
  PROPERTY *prop, *prop_next;

  if (IS_NPC(ch))
    mobile_count--;

  for (obj = ch->carrying; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    extract_obj(obj);
  }

  for (obj = ch->bankeditems; obj != NULL; obj = obj_next)
  {
    obj_next = obj->next_content;
    extract_obj(obj);
  }

  for (paf = ch->affected; paf != NULL; paf = paf_next)
  {
    paf_next = paf->next;
    affect_remove(ch, paf);
  }

  prop = ch->property;
  while (prop)
  {
    prop_next = prop->next;
    free_property(prop);
    prop = prop_next;
  }

  free_string(ch->name);
  free_string(ch->short_descr);
  free_string(ch->long_descr);
  free_string(ch->description);
  free_string(ch->prompt);
  free_string(ch->prefix);
  free_string(ch->die_descr);
  free_string(ch->say_descr);
  free_string(ch->material);
  if (ch->pcdata != NULL)
    free_pcdata(ch->pcdata);

  INVALIDATE(ch);
  free(ch);
  return;
}

PC_DATA *new_pcdata(void)
{
  int alias;
  PC_DATA *pcdata;

  pcdata = (PC_DATA *) malloc(sizeof(*pcdata));
  memset(pcdata, 0, sizeof(*pcdata));

  for (alias = 0; alias < MAX_ALIAS; alias++)
  {
    pcdata->alias[alias] = NULL;
    pcdata->alias_sub[alias] = NULL;
  }
  for (alias = 0; alias < MAX_FORGET; alias++)
  {
    pcdata->forget[alias] = NULL;
  }
  for (alias = 0; alias < MAX_DUPES; alias++)
  {
    pcdata->dupes[alias] = NULL;
  }

  pcdata->buffer = new_buf();

  VALIDATE(pcdata);
  return pcdata;
}

void free_pcdata(PC_DATA * pcdata)
{
  int alias;

  free_string(pcdata->pwd);
  free_string(pcdata->bamfin);
  free_string(pcdata->bamfout);
  free_string(pcdata->who_descr);
  free_string(pcdata->title);
  free_string(pcdata->spouse);
  free_string(pcdata->socket);
  free_buf(pcdata->buffer);

  for (alias = 0; alias < MAX_ALIAS; alias++)
  {
    free_string(pcdata->alias[alias]);
    free_string(pcdata->alias_sub[alias]);
  }
  for (alias = 0; alias < MAX_FORGET; alias++)
  {
    free_string(pcdata->forget[alias]);
  }
  for (alias = 0; alias < MAX_DUPES; alias++)
  {
    free_string(pcdata->dupes[alias]);
  }
  INVALIDATE(pcdata);
  free(pcdata);

  return;
}

/* stuff for setting ids */
long last_pc_id;
long last_mob_id;

long get_pc_id(void)
{
  unsigned long val;

  val = (number_range(1, 1000000000));
  last_pc_id = val;

  return val;
}

long get_mob_id(void)
{
  unsigned long val;

  val = (number_range(1, 1000000000));
  last_mob_id = val;

  return val;
}

MEM_DATA *new_mem_data(void)
{
  MEM_DATA *memory;

  memory = (MEM_DATA *) malloc(sizeof(*memory));
  memset(memory, 0, sizeof(*memory));

  memory->next = NULL;
  memory->id = 0;
  memory->reaction = 0;
  memory->when = 0;
  VALIDATE(memory);

  return memory;
}

void free_mem_data(MEM_DATA * memory)
{
  INVALIDATE(memory);
  free(memory);
}

/* procedures and constants needed for buffering */

/* buffer sizes */
const int buf_size[MAX_BUF_LIST] = {
  16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768, 65536
};

/* local procedure for finding the next acceptable size */
/* -1 indicates out-of-boundary error */
int get_size(int val)
{
  int i;

  for (i = 0; i < MAX_BUF_LIST; i++)
    if (buf_size[i] >= val)
    {
      return buf_size[i];
    }

  return -1;
}

BUFFER *new_buf()
{
  BUFFER *buffer;

  buffer = (BUFFER *) malloc(sizeof(*buffer));
  memset(buffer, 0, sizeof(*buffer));

  buffer->next = NULL;
  buffer->state = BUFFER_SAFE;
  buffer->size = get_size(BASE_BUF);

  buffer->string = malloc(buffer->size);
  memset(buffer->string, 0, buffer->size);
  buffer->string[0] = '\0';
  VALIDATE(buffer);

  return buffer;
}

BUFFER *new_buf_size(int size)
{
  BUFFER *buffer;

  buffer = (BUFFER *) malloc(sizeof(*buffer));
  memset(buffer, 0, sizeof(*buffer));

  buffer->next = NULL;
  buffer->state = BUFFER_SAFE;
  buffer->size = get_size(size);
  if (buffer->size == -1)
  {
    bug("new_buf: buffer size %d too large.", size);
    quit(1);
  }
  buffer->string = malloc(buffer->size);
  memset(buffer->string, 0, buffer->size);
  buffer->string[0] = '\0';
  VALIDATE(buffer);

  return buffer;
}

void free_buf(BUFFER * buffer)
{
  free(buffer->string);
  buffer->string = NULL;
  buffer->size = 0;
  buffer->state = BUFFER_FREED;
  INVALIDATE(buffer);
  free(buffer);
}

bool add_buf(BUFFER * buffer, char *string)
{
  int len;
  char *oldstr;
  int oldsize;

  oldstr = buffer->string;
  oldsize = buffer->size;

  if (buffer->state == BUFFER_OVERFLOW) /* don't waste time on bad strings! 
                                         */
    return false;

  len = strlen(buffer->string) + strlen(string) + 1;

  while (len >= buffer->size)   /* increase the buffer size */
  {
    buffer->size = get_size(buffer->size + 1);
    {
      if (buffer->size == -1)   /* overflow */
      {
        buffer->size = oldsize;
        buffer->state = BUFFER_OVERFLOW;
        bug("buffer overflow past size %d", buffer->size);
        return false;
      }
    }
  }

  if (buffer->size != oldsize)
  {
    buffer->string = malloc(buffer->size);
    memset(buffer->string, 0, buffer->size);

    strcpy(buffer->string, oldstr);
    free(oldstr);
  }

  strcat(buffer->string, string);
  return true;
}

void clear_buf(BUFFER * buffer)
{
  buffer->string[0] = '\0';
  buffer->state = BUFFER_SAFE;
}

char *buf_string(BUFFER * buffer)
{
  return buffer->string;
}

MPROG_LIST *new_mprog(void)
{
  MPROG_LIST *mp;

  mp = (MPROG_LIST *) malloc(sizeof(*mp));
  memset(mp, 0, sizeof(*mp));

  mp->vnum = 0;
  mp->trig_type = 0;
  mp->code = &str_empty[0];
  mp->trig_phrase = &str_empty[0];
  VALIDATE(mp);
  return mp;
}

void free_mprog(MPROG_LIST * mp)
{
  free_string(mp->code);
  free_string(mp->trig_phrase);
  INVALIDATE(mp);
  free(mp);
}

/* Stuff for recycling imm/auction shit */
AUCTION_DATA *new_auction(void)
{
  AUCTION_DATA *auction;

  auction = (AUCTION_DATA *) malloc(sizeof(*auction));
  memset(auction, 0, sizeof(*auction));

  VALIDATE(auction);
  return auction;
}

void free_auction(AUCTION_DATA * auction)
{
  auction->current_bid = 0;
  auction->platinum_held = 0;
  auction->high_bidder = NULL;
  auction->item = NULL;
  auction->min_bid = 0;
  auction->owner = NULL;
  auction->status = 0;
  INVALIDATE(auction);
  free(auction);
}

HELP_DATA *new_help(void)
{
  HELP_DATA *help;

  help = malloc(sizeof(HELP_DATA));
  memset(help, 0, sizeof(*help));

  help->id = 0;
  help->level = 0;
  help->keyword = str_dup("");
  help->text = str_dup("");

  return help;
}

void free_help(HELP_DATA * help)
{
  free_string(help->keyword);
  free_string(help->text);
  free(help);
}

//
// PROPERTY_INDEX
//
int property_index_allocated = 0;

PROPERTY_INDEX_TYPE *new_property_index(void)
{
  PROPERTY_INDEX_TYPE *pProp;

  pProp = (PROPERTY_INDEX_TYPE *) malloc(sizeof(PROPERTY_INDEX_TYPE));
  memset(pProp, 0, sizeof(*pProp));
  property_index_allocated++;

  VALIDATE(pProp);

  pProp->key = str_dup("");

  return pProp;
}

void free_property_index(PROPERTY_INDEX_TYPE * pProp)
{
  free_string(pProp->key);

  property_index_allocated--;
  INVALIDATE(pProp);
  free(pProp);
}


//
// PROPERTY
//
int property_allocated = 0;

PROPERTY *new_property(void)
{
  PROPERTY *property;

  property = (PROPERTY *) malloc(sizeof(PROPERTY));
  property_allocated++;

  memset((void *) property, 0, sizeof(PROPERTY));
  VALIDATE(property);

  property->sValue = str_dup("");

  return property;
}

void free_property(PROPERTY * property)
{
  free_string(property->sValue);

  property_allocated--;
  INVALIDATE(property);
  free(property);
}
