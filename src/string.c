
/***************************************************************************
 *  File: string.c                                                         *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 *                                                                         *
 *  This code was freely distributed with the The Isles 1.1 source code,   *
 *  and has been used here for OLC - OLC would not be what it is without   *
 *  all the previous coders who released their source code.                *
 *                                                                         *
 ***************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"
#include "olc.h"

char *string_linedel(char *, int);
char *string_lineadd(char *, char *, int);
char *numlines(char *);

/*****************************************************************************
 Name:        string_edit
 Purpose:    Clears string and puts player into editing mode.
 Called by:    none
 ****************************************************************************/
void string_edit(CHAR_DATA * ch, char **pString)
{
  send_to_char("{b-========- {WEntering EDIT Mode {b-=========-{x\n\r", ch);
  send_to_char("    {wType {C.h {won a new line for help.{x\n\r", ch);
  send_to_char(" {wTerminate with a {C@ {won a blank line.{x\n\r", ch);
  send_to_char("{b-=======================================-{x\n\r", ch);

  if (*pString == NULL)
  {
    *pString = str_dup("");
  }
  else
  {
    **pString = '\0';
  }

  ch->desc->pString = pString;

  return;
}


bool ftp_push(DESCRIPTOR_DATA * d);

/*****************************************************************************
 Name:        string_append
 Purpose:    Puts player into append mode for given string.
 Called by:    (many)olc_act.c
 ****************************************************************************/
void string_append(CHAR_DATA * ch, char **pString)
{
  send_to_char("{b-=======- {WEntering APPEND Mode {b-========-{x\n\r", ch);
  send_to_char("    {wType {C.h {won a new line for help.{x\n\r", ch);
  send_to_char(" {wTerminate with a {C@ {won a blank line.{x\n\r", ch);
  send_to_char("{b-=======================================-{x\n\r", ch);

  if (*pString == NULL)
  {
    *pString = str_dup("");
  }

  send_to_char(*pString, ch);

  if (*(*pString + strlen(*pString) - 1) != '\r')
    send_to_char("\n\r", ch);

  ch->desc->pString = pString;

  return;
}



/*****************************************************************************
 Name:        string_replace
 Purpose:    Substitutes one string for another.
 Called by:    string_add(string.c) (aedit_builder)olc_act.c.
 ****************************************************************************/
char *string_replace(char *origstr, char *oldstr, char *newstr)
{
  char xbuf[MAX_STRING_LENGTH];
  int i;

  xbuf[0] = '\0';
  strcpy(xbuf, origstr);
  if (strstr(origstr, oldstr) != NULL)
  {
    i = strlen(origstr) - strlen(strstr(origstr, oldstr));
    xbuf[i] = '\0';
    strcat(xbuf, newstr);
    strcat(xbuf, &origstr[i + strlen(oldstr)]);
    free_string(origstr);
  }

  return str_dup(xbuf);
}



/*****************************************************************************
 Name:        string_add
 Purpose:    Interpreter for string editing.
 Called by:    game_loop_xxxx(comm.c).
 ****************************************************************************/
extern const char *szFinishPrompt;

void string_add(CHAR_DATA * ch, char *argument)
{
  char buf[MAX_STRING_LENGTH];

  /*
   * Thanks to James Seng
   */
  smash_tilde(argument);

  if (*argument == '.')
  {
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    char tmparg3[MAX_INPUT_LENGTH];

    argument = one_argument(argument, arg1);
    argument = first_arg(argument, arg2, false);
    strcpy(tmparg3, argument);
    argument = first_arg(argument, arg3, false);

    if (!str_cmp(arg1, ".c"))
    {
      send_to_char("String cleared.\n\r", ch);
      free_string(*ch->desc->pString);
      *ch->desc->pString = str_dup("");
      return;
    }

    if (!str_cmp(arg1, ".s"))
    {
      send_to_char("String so far:\n\r", ch);
      send_to_char(numlines(*ch->desc->pString), ch);
      return;
    }

    if (!str_cmp(arg1, ".r"))
    {
      if (arg2[0] == '\0')
      {
        send_to_char("usage:  .r \"old string\" \"new string\"\n\r", ch);
        return;
      }

      *ch->desc->pString = string_replace(*ch->desc->pString, arg2, arg3);
      sprintf(buf, "'%s' replaced with '%s'.\n\r", arg2, arg3);
      send_to_char(buf, ch);
      return;
    }

    if (!str_cmp(arg1, ".f"))
    {
      *ch->desc->pString = format_string(*ch->desc->pString);
      send_to_char("String formatted.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, ".d"))
    {
      *ch->desc->pString = string_linedel(*ch->desc->pString, atoi(arg2));
      send_to_char("Line deleted.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, ".i"))
    {
      *ch->desc->pString =
        string_lineadd(*ch->desc->pString, tmparg3, atoi(arg2));
      send_to_char("Line inserted.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, ".lr"))
    {
      *ch->desc->pString = string_linedel(*ch->desc->pString, atoi(arg2));
      *ch->desc->pString =
        string_lineadd(*ch->desc->pString, tmparg3, atoi(arg2));
      send_to_char("Line replaced.\n\r", ch);
      return;
    }

    if (!str_cmp(arg1, ".h"))
    {
      send_to_char
        ("{WUse any of these commands on a blank line:  {x\n\r", ch);
      send_to_char("{C.r 'old' 'new'   {c- replace a substring {x\n\r", ch);
      send_to_char
        ("                   {c(requires {C''{c, {C\"\"{c) {x\n\r", ch);
      send_to_char("{C.h               {c- get help (this info){x\n\r", ch);
      send_to_char("{C.s               {c- show string so far  {x\n\r", ch);
      send_to_char("{C.f               {c- (word wrap) string  {x\n\r", ch);
      send_to_char("{C.c               {c- clear string so far {x\n\r", ch);
      send_to_char
        ("{C.d <num>         {c- delete line number <num>{x\n\r", ch);
      send_to_char
        ("{C.i <num> <str>   {c- insert <str> at line <num>{x\n\r", ch);
      send_to_char
        ("{C.lr <num> <str>  {c- {creplace line <num> with <str>{x\n\r", ch);
      send_to_char("{C@                {c- end string          {x\n\r", ch);
      return;
    }

    send_to_char("Invalid dot command.\n\r", ch);
    return;
  }

  if (*argument == '~' || *argument == '@')
  {

    ch->desc->pString = NULL;
    if (!IS_NPC(ch) && ch->pcdata->in_progress) /* This a note we're editing? */
    {
      write_to_buffer(ch->desc, "\n\r\n\r", 0);
      write_to_buffer(ch->desc, szFinishPrompt, 0);
      write_to_buffer(ch->desc, "\n\r", 0);
    }
    return;
  }

  strcpy(buf, *ch->desc->pString);

  /*
   * Truncate strings to MAX_STRING_LENGTH.
   * --------------------------------------
   * Edwin strikes again! Fixed avoid adding to a too-long
   * note. JR -- 10/15/00
   */
  if (strlen(*ch->desc->pString) + strlen(argument) >=
      (MAX_STRING_LENGTH - 4))
  {
    send_to_char("String too long, last line skipped.\n\r", ch);

    /* Force character out of editing mode. */
    ch->desc->pString = NULL;
    return;
  }

  /*
   * Ensure no tilde's inside string.
   * --------------------------------
   */
  smash_tilde(argument);

  strcat(buf, argument);
  strcat(buf, "\n\r");
  free_string(*ch->desc->pString);
  *ch->desc->pString = str_dup(buf);
  return;
}



/*
 * Thanks to Kalgen for the new procedure (no more bug!)
 * Original wordwrap() written by Surreality.
 * Modifications for color codes and blank lines by Geoff.
 */

/*****************************************************************************
 Name:		format_string
 Purpose:	Special string formating and word-wrapping.
 Called by:	string_add(string.c) (many)olc_act.c
 ****************************************************************************/
char *format_string(char *oldstring /*, bool fSpace */ )
{
  char xbuf[MAX_STRING_LENGTH];
  char xbuf2[MAX_STRING_LENGTH];
  char *rdesc;
  int i = 0;
  int end_of_line;
  bool cap = true;
  bool bFormat = true;

  xbuf[0] = xbuf2[0] = 0;

  i = 0;

  for (rdesc = oldstring; *rdesc; rdesc++)
  {

    if (*rdesc != '{')
    {
      if (bFormat)
      {
        if (*rdesc == '\n')
        {
          if (*(rdesc + 1) == '\r' && *(rdesc + 2) == ' ' &&
              *(rdesc + 3) == '\n' && xbuf[i - 1] != '\r')
          {
            xbuf[i] = '\n';
            xbuf[i + 1] = '\r';
            xbuf[i + 2] = '\n';
            xbuf[i + 3] = '\r';
            i += 4;
            rdesc += 2;
          }
          else if (*(rdesc + 1) == '\r' && *(rdesc + 2) == ' ' &&
                   *(rdesc + 2) == '\n' && xbuf[i - 1] == '\r')
          {
            xbuf[i] = '\n';
            xbuf[i + 1] = '\r';
            i += 2;
          }
          else if (*(rdesc + 1) == '\r' && *(rdesc + 2) == '\n' &&
                   xbuf[i - 1] != '\r')
          {
            xbuf[i] = '\n';
            xbuf[i + 1] = '\r';
            xbuf[i + 2] = '\n';
            xbuf[i + 3] = '\r';
            i += 4;
            rdesc += 1;
          }
          else if (*(rdesc + 1) == '\r' && *(rdesc + 2) == '\n' &&
                   xbuf[i - 1] == '\r')
          {
            xbuf[i] = '\n';
            xbuf[i + 1] = '\r';
            i += 2;
          }
          else if (xbuf[i - 1] != ' ' && xbuf[i - 1] != '\r')
          {
            xbuf[i] = ' ';
            i++;
          }
        }
        else if (*rdesc == 'i' && *(rdesc + 1) == '.' &&
                 *(rdesc + 2) == 'e' && *(rdesc + 3) == '.')
        {
          xbuf[i] = 'i';
          xbuf[i + 1] = '.';
          xbuf[i + 2] = 'e';
          xbuf[i + 3] = '.';
          i += 4;
          rdesc += 3;
        }
        else if (*rdesc == ' ')
        {
          if (xbuf[i - 1] != ' ')
          {
            xbuf[i] = ' ';
            i++;
          }
        }
        else if (*rdesc == ')')
        {
          if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' '
              && (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' ||
                  xbuf[i - 3] == '!'))
          {
            xbuf[i - 2] = *rdesc;
            xbuf[i - 1] = ' ';
            xbuf[i] = ' ';
            i++;
          }
          else if (xbuf[i - 1] == ' ' &&
                   (xbuf[i - 2] == ',' || xbuf[i - 2] == ';'))
          {
            xbuf[i - 1] = *rdesc;
            xbuf[i] = ' ';
            i++;
          }
          else
          {
            xbuf[i] = *rdesc;
            i++;
          }
        }
        else if (*rdesc == ',' || *rdesc == ';')
        {
          if (xbuf[i - 1] == ' ')
          {
            xbuf[i - 1] = *rdesc;
            xbuf[i] = ' ';
            i++;
          }
          else
          {
            xbuf[i] = *rdesc;
            if (*(rdesc + 1) != '\"')
            {
              xbuf[i + 1] = ' ';
              i += 2;
            }
            else
            {
              xbuf[i + 1] = '\"';
              xbuf[i + 2] = ' ';
              i += 3;
              rdesc++;
            }
          }

        }
        else if (*rdesc == '.' || *rdesc == '?' || *rdesc == '!')
        {
          if (xbuf[i - 1] == ' ' && xbuf[i - 2] == ' '
              && (xbuf[i - 3] == '.' || xbuf[i - 3] == '?' ||
                  xbuf[i - 3] == '!'))
          {
            xbuf[i - 2] = *rdesc;
            if (*(rdesc + 1) != '\"')
            {
              xbuf[i - 1] = ' ';
              xbuf[i] = ' ';
              i++;
            }
            else
            {
              xbuf[i - 1] = '\"';
              xbuf[i] = ' ';
              xbuf[i + 1] = ' ';
              i += 2;
              rdesc++;
            }
          }
          else
          {
            xbuf[i] = *rdesc;
            if (*(rdesc + 1) != '\"')
            {
              xbuf[i + 1] = ' ';
              xbuf[i + 2] = ' ';
              i += 3;
            }
            else
            {
              xbuf[i + 1] = '\"';
              xbuf[i + 2] = ' ';
              xbuf[i + 3] = ' ';
              i += 4;
              rdesc++;
            }
          }
          cap = true;
        }
        else
        {
          xbuf[i] = *rdesc;
          if (cap)
          {
            cap = false;
            xbuf[i] = UPPER(xbuf[i]);
          }
          i++;
        }
      }
      else
      {
        xbuf[i] = *rdesc;
        i++;
      }
    }
    else
    {
      if (*(rdesc + 1) == 'Z')
        bFormat = !bFormat;
      xbuf[i] = *rdesc;
      i++;
      rdesc++;
      xbuf[i] = *rdesc;
      i++;
    }
  }
  xbuf[i] = 0;
  strcpy(xbuf2, xbuf);

  rdesc = xbuf2;

  xbuf[0] = 0;

  for (;;)
  {
    end_of_line = 77;
    for (i = 0; i < end_of_line; i++)
    {
      if (*(rdesc + i) == '{')
      {
        end_of_line += 2;
        i++;
      }

      if (!*(rdesc + i))
        break;

      if (*(rdesc + i) == '\r')
        end_of_line = i;
    }
    if (i < end_of_line)
    {
      break;
    }
    if (*(rdesc + i - 1) != '\r')
    {
      for (i = (xbuf[0] ? (end_of_line - 1) : (end_of_line - 4)); i; i--)
      {
        if (*(rdesc + i) == ' ')
          break;
      }
      if (i)
      {
        *(rdesc + i) = 0;
        strcat(xbuf, rdesc);
        strcat(xbuf, "\n\r");
        rdesc += i + 1;
        while (*rdesc == ' ')
          rdesc++;
      }
      else
      {
        bug("`5Wrap_string: `@No spaces``", 0);
        *(rdesc + (end_of_line - 2)) = 0;
        strcat(xbuf, rdesc);
        strcat(xbuf, "-\n\r");
        rdesc += end_of_line - 1;
      }
    }
    else
    {
      *(rdesc + i - 1) = 0;
      strcat(xbuf, rdesc);
      strcat(xbuf, "\r");
      rdesc += i;
      while (*rdesc == ' ')
        rdesc++;
    }
  }
  while (*(rdesc + i) && (*(rdesc + i) == ' ' ||
                          *(rdesc + i) == '\n' || *(rdesc + i) == '\r'))
    i--;
  *(rdesc + i + 1) = 0;
  strcat(xbuf, rdesc);
  if (xbuf[strlen(xbuf) - 2] != '\n')
    strcat(xbuf, "\n\r");

  free_string(oldstring);
  return (str_dup(xbuf));
}

/*
 * Used above in string_add.  Because this function does not
 * modify case if fCase is false and because it understands
 * parenthesis, it would probably make a nice replacement
 * for one_argument.
 */

/*****************************************************************************
 Name:        first_arg
 Purpose:    Pick off one argument from a string and return the rest.
         Understands quates, parenthesis (barring ) ('s) and
         percentages.
 Called by:    string_add(string.c)
 ****************************************************************************/
char *first_arg(char *argument, char *arg_first, bool fCase)
{
  char cEnd;

  while (*argument == ' ')
    argument++;

  cEnd = ' ';
  if (*argument == '\'' || *argument == '"'
      || *argument == '%' || *argument == '(')
  {
    if (*argument == '(')
    {
      cEnd = ')';
      argument++;
    }
    else
      cEnd = *argument++;
  }

  while (*argument != '\0')
  {
    if (*argument == cEnd)
    {
      argument++;
      break;
    }
    if (fCase)
      *arg_first = LOWER(*argument);
    else
      *arg_first = *argument;
    arg_first++;
    argument++;
  }
  *arg_first = '\0';

  while (*argument == ' ')
    argument++;

  return argument;
}




/*
 * Used in olc_act.c for aedit_builders.
 */
char *string_unpad(char *argument)
{
  char buf[MAX_STRING_LENGTH];
  char *s;

  s = argument;

  while (*s == ' ')
    s++;

  strcpy(buf, s);
  s = buf;

  if (*s != '\0')
  {
    while (*s != '\0')
      s++;
    s--;

    while (*s == ' ')
      s--;
    s++;
    *s = '\0';
  }

  free_string(argument);
  return str_dup(buf);
}



/*
 * Same as capitalize but changes the pointer's data.
 * Used in olc_act.c in aedit_builder.
 */
char *string_proper(char *argument)
{
  char *s;

  s = argument;

  while (*s != '\0')
  {
    if (*s != ' ')
    {
      *s = UPPER(*s);
      while (*s != ' ' && *s != '\0')
        s++;
    }
    else
    {
      s++;
    }
  }

  return argument;
}

char *string_linedel(char *string, int line)
{
  char *strtmp = string;
  char buf[MAX_STRING_LENGTH];
  int cnt = 1, tmp = 0;

  buf[0] = '\0';

  for (; *strtmp != '\0'; strtmp++)
  {
    if (cnt != line)
      buf[tmp++] = *strtmp;

    if (*strtmp == '\n')
    {
      if (*(strtmp + 1) == '\r')
      {
        if (cnt != line)
          buf[tmp++] = *(++strtmp);
        else
          ++strtmp;
      }

      cnt++;
    }
  }

  buf[tmp] = '\0';

  free_string(string);
  return str_dup(buf);
}

char *string_lineadd(char *string, char *newstr, int line)
{
  char *strtmp = string;
  int cnt = 1, tmp = 0;
  bool done = false;
  char buf[MAX_STRING_LENGTH];

  buf[0] = '\0';

  for (; *strtmp != '\0' || (!done && cnt == line); strtmp++)
  {
    if (cnt == line && !done)
    {
      strcat(buf, newstr);
      strcat(buf, "\n\r");
      tmp += strlen(newstr) + 2;
      cnt++;
      done = true;
    }

    buf[tmp++] = *strtmp;

    if (done && *strtmp == '\0')
      break;

    if (*strtmp == '\n')
    {
      if (*(strtmp + 1) == '\r')
        buf[tmp++] = *(++strtmp);

      cnt++;
    }

    buf[tmp] = '\0';
  }

  free_string(string);
  return str_dup(buf);
}

/* buf queda con la line sin \n\r */
char *olcgetline(char *str, char *buf)
{
  int tmp = 0;
  bool found = false;

  while (*str)
  {
    if (*str == '\n')
    {
      found = true;
      break;
    }

    buf[tmp++] = *(str++);
  }

  if (found)
  {
    if (*(str + 1) == '\r')
      str += 2;
    else
      str += 1;
  }

  buf[tmp] = '\0';

  return str;
}

char *numlines(char *string)
{
  int cnt = 1;
  static char buf[MAX_STRING_LENGTH * 2];
  char buf2[MAX_STRING_LENGTH], tmpb[MAX_STRING_LENGTH-256];

  buf[0] = '\0';

  while (*string)
  {
    string = olcgetline(string, tmpb);
    sprintf(buf2, "%2d. %s\n\r", cnt++, tmpb);
    strcat(buf, buf2);
  }

  return buf;
}
