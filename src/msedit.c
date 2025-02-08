#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "merc.h"
#include "music.h"
#include "olc.h"
#include "recycle.h"

extern struct song_data song_table[MAX_SONGS];

void do_band_name args((CHAR_DATA * ch, char *argument));
void do_song_name args((CHAR_DATA * ch, char *argument));
void do_lyrics args((CHAR_DATA * ch, char *argument));
void do_save_music args((void));

CH_CMD(do_msedit)
{
  sh_int count = 0;
  int lines;
  char arg[MAX_INPUT_LENGTH];
  char edit_name[MAX_INPUT_LENGTH];
  char buf[MAX_STRING_LENGTH];

  argument = one_argument(argument, arg);
  strcpy(edit_name, argument);

  if (ch->pcdata->security < MAX_SECURITY)
  {
    send_to_char("You don't have enough security to edit songs.\n\r", ch);
    return;
  }
  if (arg[0] == '\0')
  {
    send_to_char
      ("MSEdit Syntax:\n\rMSEdit Create <song name>\n\rMSEdit Load <song name>\n\rMSEDIT Songname <song name>\n\rMSEDIT Group <group name>\n\rMSEdit Lyrics + <line>\n\rMSEdit Show\n\rMSEdit Save\n\r",
       ch);
    return;
  }

  if (!str_cmp(arg, "create"))
  {
    if (edit_name[0] == '\0')
    {
      send_to_char("You must include a song name.\n\r", ch);
      return;
    }
    if (ch->song_ed_number > 0)
    {
      send_to_char
        ("Finish the song you are working on then create a new one.\n\r", ch);
      return;
    }
    for (count = 0; count < MAX_SONGS; count++)
    {
      if (song_table[count].name != NULL &&
          is_name(edit_name, song_table[count].name))
      {
        send_to_char("That song already exists, try a new name.\n\r", ch);
        return;
      }
      if (song_table[count].name == NULL)
      {
        ch->song_ed_number = count;
        ch->llines = 0;
        song_table[ch->song_ed_number].name = str_dup(edit_name);
        song_table[ch->song_ed_number].group =
          str_dup("Place Band Name here");
        send_to_char("Ok.\n\r", ch);
        return;
      }
    }
  }

  if (!str_cmp(arg, "songname"))
  {
    if (ch->song_ed_number == 0)
    {
      send_to_char("you aren't currently editing any music.\n\r", ch);
      return;
    }
    do_song_name(ch, edit_name);
    return;
  }

  if (!str_cmp(arg, "lines"))
  {
    if (is_number(edit_name))
    {
      if (atoi(edit_name) < 0 || atoi(edit_name) > MAX_LINES)
      {
        sprintf(buf,
                "Value must be between 0 and %d.\n\rIf you play the song and get (null) reduce the number of lines./n/r",
                MAX_LINES);
        send_to_char(buf, ch);
        return;
      }
      ch->llines = atoi(edit_name);
      send_to_char("Ok.\n\r", ch);
      return;
    }
    else
    {
      send_to_char("Must be a Numeric value.\n\r", ch);
      return;
    }

  }

  if (!str_cmp(arg, "group"))
  {
    if (ch->song_ed_number == 0)
    {
      send_to_char("you aren't currently editing any music.\n\r", ch);
      return;
    }
    do_band_name(ch, edit_name);
    return;
  }

  if (!str_cmp(arg, "lyrics"))
  {
    if (ch->song_ed_number == 0)
    {
      send_to_char("you aren't currently editing any music.\n\r", ch);
      return;
    }
    do_lyrics(ch, edit_name);
    return;
  }

  if (!str_cmp(arg, "show"))
  {
    if (ch->song_ed_number == 0)
    {
      send_to_char("you aren't currently editing any music.\n\r", ch);
      return;
    }
    sprintf(buf,
            "-=-=-=-=-=-=-=-=-=-= %s -=-=-=-=-=-=-=-=-=-=\n\r"
            "Sung by: %s\n\r", song_table[ch->song_ed_number].name,
            song_table[ch->song_ed_number].group);
    send_to_char(buf, ch);
    send_to_char("Lyrics:\n\r", ch);
    for (lines = 0; lines < MAX_LINES; lines++)
    {
      if (song_table[ch->song_ed_number].lyrics[lines] == NULL)
        break;
      sprintf(buf, "%s\n\r", song_table[ch->song_ed_number].lyrics[lines]);
      send_to_char(buf, ch);
    }
    return;
  }

  if (!str_cmp(arg, "done"))
  {
    ch->song_ed_number = 0;
    ch->llines = 0;
    for (lines = 0; lines < MAX_LINES; lines++)
    {
      if (song_table[ch->song_ed_number].lyrics[lines] == NULL)
      {
        song_table[ch->song_ed_number].lines = lines;
        break;
      }
    }
    send_to_char("Ok.\n\r", ch);
    return;
  }

  if (!str_cmp(arg, "save"))
  {
    send_to_char("Music files saved.\n\r", ch);
    do_save_music();
    return;
  }
  if (!str_cmp(arg, "load"))
  {
    for (count = 0; count < MAX_SONGS; count++)
    {
      if (is_name(edit_name, song_table[count].name))
      {
        ch->song_ed_number = count;
        sprintf(buf, "Now editing existing file: %s\n\r",
                song_table[ch->song_ed_number].name);
        send_to_char(buf, ch);

        for (lines = 0; lines < MAX_LINES; lines++)
        {
          if (song_table[count].lyrics[lines] == NULL)
            break;

          song_table[ch->song_ed_number].lines = lines;
          ch->llines = lines;
          return;
        }
      }
    }
    if (ch->song_ed_number <= 0)
      send_to_char("No such Song.\n\r", ch);
  }
  send_to_char
    ("MSEdit Syntax:\n\rMSEdit Create <song name>\n\rMSEdit Load <song name>\n\rMSEDIT Songname <song name>\n\rMSEDIT Group <group name>\n\rMSEdit Lyrics + <line>\n\rMSEdit Show\n\rMSEdit Save\n\r",
     ch);
  return;
}

CH_CMD(do_band_name)
{
  char bandname[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];

  strcpy(bandname, argument);

  if (bandname[0] == '\0')
  {
    send_to_char("You must include the band name.\n\r", ch);
    return;
  }
  sprintf(buf, "%s", bandname);
  song_table[ch->song_ed_number].group = str_dup(buf);
  send_to_char("Ok.\n\r", ch);
  return;
}

CH_CMD(do_song_name)
{
  char songname[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];
  int count;

  strcpy(songname, argument);

  if (songname[0] == '\0')
  {
    send_to_char("You must include the song name.\n\r", ch);
    return;
  }
  for (count = 0; count < MAX_SONGS; count++)
  {
    if (is_name(songname, song_table[count].name))
    {
      send_to_char("That song already exists, try a new name.\n\r", ch);
      return;
    }
  }
  sprintf(buf, "%s", songname);
  song_table[ch->song_ed_number].name = str_dup(buf);
  send_to_char("Ok.\n\r", ch);
  return;
}

CH_CMD(do_lyrics)
{
  char arg[MAX_INPUT_LENGTH];
  char lyrics[MAX_INPUT_LENGTH];
  char buf[MAX_INPUT_LENGTH];

  argument = one_argument(argument, arg);
  strcpy(lyrics, argument);

  if (arg[0] == '\0')
  {
    send_to_char("Type MSEdit + <string> or MSEdit -\n\r", ch);
    return;
  }

  if (!str_cmp(arg, "+"))
  {
    if (arg[0] == '\0')
    {
      send_to_char("No Blank Lines.\n\r", ch);
      return;
    }
    sprintf(buf, "%s", lyrics);
    song_table[ch->song_ed_number].lyrics[ch->llines] = str_dup(buf);
    ch->llines += 1;
    song_table[ch->song_ed_number].lines = ch->llines;
    send_to_char("Ok.\n\r", ch);
    return;
  }
  if (!str_cmp(arg, "-"))
  {
    ch->llines -= 1;
    if (ch->llines < 0)
    {
      send_to_char("Lyrics Cleared.\n\r", ch);
      ch->llines = 0;
      return;
    }
    song_table[ch->song_ed_number].lyrics[ch->llines] = str_dup(" ");
    song_table[ch->song_ed_number].lines = ch->llines;
    send_to_char("Line Deleted.\n\r", ch);
    return;
  }
  send_to_char("Type MSEdit + <string> or MSEdit -\n\r", ch);
  return;
}

void do_save_music(void)
{
  FILE *fp;
  int count;
  int lines;

  lines = 0;

  if (!(fp = file_open("music.txt", "w")))
  {
    bug("Open_help: fopen", 0);

    perror("music.txt");
  }
  for (count = 0; count < MAX_SONGS; count++)
  {
    if (song_table[count].name == NULL)
      break;

    fprintf(fp, "%s~\n%s~\n", song_table[count].group,
            song_table[count].name);

    for (lines = 0; count < MAX_LINES; lines++)
    {
      if (song_table[count].lyrics[lines] == NULL)
      {
        fprintf(fp, "~\n");
        break;
      }
      fprintf(fp, "%s\n", song_table[count].lyrics[lines]);
    }
  }
  fprintf(fp, "#\n");
  file_close(fp);
  return;
}
