#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "merc.h"
#include "tables.h"
#include "olc.h"
#include "lookup.h"
#include "recycle.h"

#define IS_NULLSTR(str) ((str) == NULL || (str)[0] == '\0')

#define HEDIT( fun )           bool fun(CHAR_DATA *ch, char*argument)
#define EDIT_HELP(ch, help)	( help = (HELP_DATA *) ch->desc->pEdit )

const struct olc_cmd_type hedit_table[] =
{
/*	{	command		function	}, */

	{	"keyword",	hedit_keyword	},
	{	"text",	hedit_text	},
	{	"new",		hedit_new	},
	{	"level",	hedit_level	},
	{	"commands",	show_commands	},
	{	"delete",	hedit_delete	},
	{	"list",		hedit_list	},
	{	"show",		hedit_show	},
	{	"?",		show_help	},

	{	NULL,		0		}
};

HEDIT(hedit_show)
{
	HELP_DATA * help;
	char buf[MSL*2];

	EDIT_HELP(ch, help);

	sprintf( buf, "Keyword : [%s]\n\r"
		      "Level   : [%d]\n\r"
		      "Text   :\n\r"
		      "%s-END-\n\r",
		      help->keyword,
		      help->level,
		      help->text );
	send_to_char( buf, ch );

	return false;
}

HEDIT(hedit_level)
{
	HELP_DATA *help;
	int lev;

	EDIT_HELP(ch, help);

	if ( IS_NULLSTR(argument) || !is_number(argument) )
	{
		send_to_char( "Syntax : level [-1..MAX_LEVEL]\n\r", ch );
		return false;
	}

	lev = atoi(argument);

	if ( lev < -1 || lev > MAX_LEVEL )
	{
		printf_to_char( ch, "HEdit : levels between -1 and %d only.\n\r", MAX_LEVEL );
		return false;
	}

	help->level = lev;
	send_to_char( "Ok.\n\r", ch );
	return true;
}

HEDIT(hedit_keyword)
{
	HELP_DATA *help;

	EDIT_HELP(ch, help);

	if ( IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax : keyword [keywords]\n\r", ch );
		return false;
	}

	free_string(help->keyword);
	help->keyword = str_dup(argument);

	send_to_char( "Ok.\n\r", ch );
	return true;
}

HEDIT(hedit_new)
{
	char arg[MIL], fullarg[MIL];
	HELP_DATA *help;

	if ( IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax : new [name]\n\r", ch );
		send_to_char( "           new [area] [name]\n\r", ch );
		return false;
	}

	strcpy( fullarg, argument );
	argument = one_argument( argument, arg );

	if ( help_lookup(argument) )
	{
		send_to_char( "HEdit : help already exists.\n\r", ch );
		return false;
	}

	help		= new_help();
	help->level	= 0;
	help->keyword	= str_dup(argument);
	help->text	= str_dup( "" );

	if (help_last)
		help_last->next	= help;

	if (help_first == NULL)
		help_first = help;

	help_last	= help;
	help->next	= NULL;

	ch->desc->pEdit		= (HELP_DATA *) help;
	ch->desc->editor	= ED_HELP;

	send_to_char( "Ok.\n\r", ch );
	return false;
}

HEDIT( hedit_text )
{
	HELP_DATA *help;

	EDIT_HELP(ch, help);

	if ( !IS_NULLSTR(argument) )
	{
		send_to_char( "Syntax : text\n\r", ch );
		return false;
	}

	string_append( ch, &help->text );

	return true;
}

void hedit( CHAR_DATA *ch, char *argument)
{
    HELP_DATA * pHelp;
    char arg[MAX_INPUT_LENGTH];
    char command[MAX_INPUT_LENGTH];
    int cmd;

    smash_tilde(argument);
    strcpy(arg, argument);
    argument = one_argument( argument, command);

    EDIT_HELP(ch, pHelp);

    if (ch->pcdata->security < 9)
    {
        send_to_char("HEdit: Insufficient security to edit helps.\n\r",ch);
	edit_done(ch);
	return;
    }

    if (command[0] == '\0')
    {
	hedit_show(ch, argument);
        return;
    }

    if (!str_cmp(command, "done") )
    {
        edit_done(ch);
        return;
    }

    for (cmd = 0; hedit_table[cmd].name != NULL; cmd++)
    {
	if (!str_prefix(command, hedit_table[cmd].name) )
	{
		(*hedit_table[cmd].olc_fun)(ch, argument);
		return;
	}
    }

    interpret(ch, arg);
    return;
}

/*
void do_hedit(CHAR_DATA *ch, char *argument)
{
	HELP_DATA * pHelp;

	if ( IS_NPC(ch) )
		return;

	if ( (pHelp = help_lookup( argument )) == NULL )
	{
		send_to_char( "HEdit : Nonexistent Help.\n\r", ch );
		return;
	}

	ch->desc->pEdit		= (void *) pHelp;
	ch->desc->editor	= ED_HELP;

	return;
}
*/

void do_hedit( CHAR_DATA *ch, char *argument )
{
	HELP_DATA *pHelp;
	char arg1[MIL];
	char argall[MAX_INPUT_LENGTH],argone[MAX_INPUT_LENGTH];
	bool found = false;

	strcpy(arg1,argument);

	if(argument[0] != '\0' && str_cmp(arg1,"new"))
	{
		/* Taken from do_help */
		argall[0] = '\0';
		while (argument[0] != '\0' )
		{
			argument = one_argument(argument,argone);
			if (argall[0] != '\0')
				strcat(argall," ");
			strcat(argall,argone);
		}
		for ( pHelp = help_first; pHelp != NULL; pHelp = pHelp->next )
		{
			if ( is_name( argall, pHelp->keyword ) )
			{
				ch->desc->pEdit=(void *)pHelp;
				ch->desc->editor= ED_HELP;
				found = true;
				return;
			}
		}
	}
	if(!found)
	{
		argument = one_argument(arg1, arg1);

		if(!str_cmp(arg1,"new"))
		{
			if (argument[0] == '\0')
			{
				send_to_char("Syntax: edit help new [topic]\n\r",ch);
				return;
			}
			if (hedit_new(ch, argument) )
				ch->desc->editor = ED_HELP;
			return;
		}
	}
	send_to_char( "HEdit:  There is no default help to edit.\n\r", ch );
	return;
}

HEDIT(hedit_delete)
{
	HELP_DATA * pHelp, * temp;
	DESCRIPTOR_DATA *d;

	EDIT_HELP(ch, pHelp);

	for ( d = descriptor_list; d; d = d->next )
		if ( d->editor == ED_HELP && pHelp == (HELP_DATA *) d->pEdit )
			edit_done(d->character);

	if (help_first == pHelp)
		help_first = help_first->next;
	else
	{
		for ( temp = help_first; temp; temp = temp->next )
			if ( temp->next == pHelp )
				break;

		if ( !temp )
		{
			bugf( "hedit_delete : help %s not found in help_first", pHelp->keyword );
			return false;
		}

		temp->next = pHelp->next;
	}

	free_help(pHelp);

	send_to_char( "Ok.\n\r", ch );
	return true;
}

HEDIT(hedit_list)
{
	char buf[MIL];
	int cnt = 0;
	HELP_DATA *pHelp;
	BUFFER *buffer;

	EDIT_HELP(ch, pHelp);

		buffer = new_buf();

		for ( pHelp = help_first; pHelp; pHelp = pHelp->next )
		{
			sprintf( buf, "%3d. %-14.14s%s", cnt, pHelp->keyword,
				cnt % 4 == 3 ? "\n\r" : " " );
			add_buf( buffer, buf );
			cnt++;
		}

		if ( cnt % 4 )
			add_buf( buffer, "\n\r" );

		page_to_char( buf_string(buffer), ch );
    free_buf(buffer);
		return false;
}

