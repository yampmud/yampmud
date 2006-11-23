/*
 * String Utilities
 *
 * str_util.c
 *
 * Copyright (c) 2006 Bobby Bailey (a.k.a. Pwyll)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
 * associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT 
 * OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "merc.h"
#include "str_util.h"

int strlen_color(char *argument)
{
	int length;

	if (!argument || argument[0] == '\0')
		return 0;

	length = 0;

	while (*argument != '\0')
	{
		if (*argument != '{')
		{
			argument++;
			length++;
			continue;
		}

		if (*(++argument) == '{')
			length++;

		argument++;
	}

	return length;
}

/* CENTER Function
 * Syntax: center( "argument", width, "fill" )
 *
 * This function will center "argument" in a string <width> characters
 * wide, using the "fill" character for padding on either end of the
 * string when centering. If "argument" is longer than <width>, the
 * "argument" is returned unmodified.
 *
 * Example usage:
 *  printf( "%s", center( "TEST", 10, "*" ) );
 * will return:
 *      ***TEST***
 */

char *center(char *argument, int width, char *fill)
{
	char buf[MSL];
	static char buf2[MSL];
	int length;
	int lead_chrs;
	int trail_chrs;

	if (!argument)
	{
		sprintf(log_buf,
						"ERROR! Please note an imm if you see this message.\n\rPlease include EXACTLY what you did before you got this message.\n\r");
		return log_buf;
	}

  if (!fill || !str_cmp(fill,""))
    fill = " ";

	length = strlen_color(argument);

	if (length >= width)
		return argument;

	lead_chrs = (int) ((width / 2) - (length / 2) + .5);
	memset(buf2, *fill, lead_chrs);
	buf2[lead_chrs] = '\0';
	strcat(buf2, argument);
	trail_chrs = width - lead_chrs - length;

	if (trail_chrs > 0)
	{
		memset(buf, *fill, trail_chrs);
		buf[trail_chrs] = '\0';
		strcat(buf2, buf);
	}

	return buf2;
}

char *str_to_len(const char *string, int length)
{
	char buf[MSL];
	char *newstr;
	char temp;
	int count = 0;
	int pos = 0;
	newstr = buf;
	while (*string && count != length)
	{
		temp = *string++;
		buf[pos++] = temp;
		if (temp == '{')
		{
			buf[pos] = *string++;
			if (buf[pos] == '{')
				count++;
			pos++;
			continue;
		}
		count++;
	}
	while (count++ < length)
		buf[pos++] = ' ';
	buf[pos] = '\0';
	return (newstr);
}

char *strip_color(const char *string)
{
	char buf[MSL];
	char *newstr;
	int count = 0;
	char temp;
	newstr = buf;
	while (*string && count != MSL)
	{
		temp = *string++;
		if (temp == '{')
		{
			temp = *string++;
			if (temp == '{')
			{
				buf[count++] = '{';
				buf[count++] = temp;
			}
			continue;
		}
		buf[count++] = temp;
	}
	buf[count] = '\0';
	return (newstr);
}
