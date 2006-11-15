/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku vMud improvments copyright (C) 1992, 1993 by Michael         *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Thanks to abaddon for proof-reading our comm.c and pointing out bugs.  *
 *  Any remaining bugs are, of course, our work, not his.  :)              *
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

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
#include "merc.h"
#include "recycle.h"
#include "tables.h"

/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern int malloc_debug args ( ( int ) );
extern int malloc_verify args ( ( void ) );
#endif

/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix)
#include <signal.h>
#endif

#if defined(apollo)
#undef __attribute
#endif

/*
 * Socket and TCP/IP stuff.
 */
#if	defined(macintosh) || defined(MSDOS)
const char echo_off_str[] = { '\0' };
const char echo_on_str[] = { '\0' };
const char go_ahead_str[] = { '\0' };
#endif

#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "telnet.h"
const char echo_off_str[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const char go_ahead_str[] = { IAC, GA, '\0' };
#endif

/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
/*int	accept		args( ( int s, struct sockaddr *addr, int *addrlen
) );*/
/*int	bind		args( ( int s, struct sockaddr *name, int namelen
) );*/
void bzero args ( ( char *b, int length ) );
int getpeername args ( ( int s, struct sockaddr * name, int *namelen ) );
int getsockname args ( ( int s, struct sockaddr * name, int *namelen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );
int listen args ( ( int s, int backlog ) );
int setsockopt
args ( ( int s, int level, int optname, void *optval, int optlen ) );
int socket args ( ( int domain, int type, int protocol ) );
#endif

#if	defined(apollo)
#include <unistd.h>
void bzero args ( ( char *b, int length ) );
#endif

#if	defined(__hpux)
/*int	accept		args( ( int s, void *addr, int *addrlen ) );*/
/*int	bind		args( ( int s, const void *addr, int addrlen )
);*/
void bzero args ( ( char *b, int length ) );
int getpeername args ( ( int s, void *addr, int *addrlen ) );
int getsockname args ( ( int s, void *name, int *addrlen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );
int listen args ( ( int s, int backlog ) );
int setsockopt
args ( ( int s, int level, int optname, const void *optval, int optlen ) );
int socket args ( ( int domain, int type, int protocol ) );
#endif

#if	defined(interactive)
#include <net/errno.h>
#include <sys/fnctl.h>
#endif
/*
#if	defined(linux)
//int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
//int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif
*/
#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct timeval
{
    time_t tv_sec;
    time_t tv_usec;
};

#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static long theKeys[4];

int gettimeofday args ( ( struct timeval * tp, void *tzp ) );
#endif

#if	defined(MIPS_OS)
extern int errno;
#endif

#if	defined(MSDOS)
int gettimeofday args ( ( struct timeval * tp, void *tzp ) );
int kbhit args ( ( void ) );
#endif

#if	defined(NeXT)
int close args ( ( int fd ) );
int fcntl args ( ( int fd, int cmd, int arg ) );

#if	!defined(htons)
u_short htons args ( ( u_short hostshort ) );
#endif
#if	!defined(ntohl)
u_long ntohl args ( ( u_long hostlong ) );
#endif
int read args ( ( int fd, char *buf, int nbyte ) );
int select
args ( ( int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
         struct timeval * timeout ) );
int write args ( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(sequent)
/*int	accept		args( ( int s, struct sockaddr *addr, int *addrlen
) );*/
/*int	bind		args( ( int s, struct sockaddr *name, int namelen
) );*/
int close args ( ( int fd ) );
int fcntl args ( ( int fd, int cmd, int arg ) );
int getpeername args ( ( int s, struct sockaddr * name, int *namelen ) );
int getsockname args ( ( int s, struct sockaddr * name, int *namelen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );

#if	!defined(htons)
u_short htons args ( ( u_short hostshort ) );
#endif
int listen args ( ( int s, int backlog ) );

#if	!defined(ntohl)
u_long ntohl args ( ( u_long hostlong ) );
#endif
int read args ( ( int fd, char *buf, int nbyte ) );
int select
args ( ( int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
         struct timeval * timeout ) );
int setsockopt
args ( ( int s, int level, int optname, caddr_t optval, int optlen ) );
int socket args ( ( int domain, int type, int protocol ) );
int write args ( ( int fd, char *buf, int nbyte ) );
#endif

/* This includes Solaris Sys V as well */
#if defined(sun)
int accept args ( ( int s, struct sockaddr * addr, int *addrlen ) );
int bind args ( ( int s, struct sockaddr * name, int namelen ) );
void bzero args ( ( char *b, int length ) );
int close args ( ( int fd ) );
int getpeername args ( ( int s, struct sockaddr * name, int *namelen ) );
int getsockname args ( ( int s, struct sockaddr * name, int *namelen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );
int listen args ( ( int s, int backlog ) );
int read args ( ( int fd, char *buf, int nbyte ) );
int select
args ( ( int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
         struct timeval * timeout ) );
#if defined(SYSV)
int setsockopt
args ( ( int s, int level, int optname, const char *optval, int optlen ) );
#else
int setsockopt
args ( ( int s, int level, int optname, void *optval, int optlen ) );
#endif
int socket args ( ( int domain, int type, int protocol ) );
int write args ( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int accept args ( ( int s, struct sockaddr * addr, int *addrlen ) );
int bind args ( ( int s, struct sockaddr * name, int namelen ) );
void bzero args ( ( char *b, int length ) );
int close args ( ( int fd ) );
int getpeername args ( ( int s, struct sockaddr * name, int *namelen ) );
int getsockname args ( ( int s, struct sockaddr * name, int *namelen ) );
int gettimeofday args ( ( struct timeval * tp, struct timezone * tzp ) );
int listen args ( ( int s, int backlog ) );
int read args ( ( int fd, char *buf, int nbyte ) );
int select
args ( ( int width, fd_set * readfds, fd_set * writefds, fd_set * exceptfds,
         struct timeval * timeout ) );
int setsockopt
args ( ( int s, int level, int optname, void *optval, int optlen ) );
int socket args ( ( int domain, int type, int protocol ) );
int write args ( ( int fd, char *buf, int nbyte ) );
#endif

/*
 * Global variables.
 */
DESCRIPTOR_DATA *d_next;        /* Next descriptor in loop */
bool god;                       /* All new chars are gods! */
char clcode[MAX_INPUT_LENGTH];

/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos args ( ( void ) );
bool read_from_descriptor args ( ( DESCRIPTOR_DATA * d ) );
bool write_to_descriptor args ( ( int desc, char *txt, int length ) );
#endif

#if defined(unix)
void game_loop_unix args ( ( int control ) );
int init_socket args ( ( int port ) );
void init_descriptor args ( ( int control ) );
bool read_from_descriptor args ( ( DESCRIPTOR_DATA * d ) );
bool write_to_descriptor args ( ( int desc, char *txt, int length ) );
#endif

/* Needs to be global because of do_copyover */
int port, control;
volatile sig_atomic_t crashed = 0;

static void sigalrm ( sig )
int sig;
{
    time_t tm;

    time(&tm);

    if ( ( tm - current_time ) > 15 )
    {
        fprintf ( stderr, "Looping - Last Command: %s\r", last_command );
        // this requires you to add an "if (ch)" before the send_to_char
        // statements in do_copyover.
        do_copyover ( NULL, "" );
        exit ( 1 );
    }
    alarm ( 15 );
}

int main ( int argc, char **argv )
{
    struct timeval now_time;
    bool fCopyOver = false;
    struct sigaction halt_action, ignore_action, alarm_action;

    halt_action.sa_handler = halt_mud;
    sigemptyset ( &halt_action.sa_mask );
    halt_action.sa_flags = SA_NOMASK;

    ignore_action.sa_handler = SIG_IGN;
    sigemptyset ( &ignore_action.sa_mask );
    ignore_action.sa_flags = 0;

    alarm_action.sa_handler = sigalrm;
    sigemptyset ( &alarm_action.sa_mask );
    alarm_action.sa_flags = SA_NOMASK;

    sigaction ( SIGPIPE, &ignore_action, NULL );    /* who cares about pipes? */
    sigaction ( SIGHUP, &ignore_action, NULL ); /* stay alive if user quits */
    sigaction ( SIGINT, &halt_action, NULL );   /* interrupted at keyboard */
    sigaction ( SIGQUIT, &halt_action, NULL );  /* quit at keyboard */
    sigaction ( SIGILL, &halt_action, NULL );   /* illegal instruction */
    sigaction ( SIGFPE, &halt_action, NULL );   /* floating point error */
    sigaction ( SIGSEGV, &halt_action, NULL );  /* invalid memory reference */
    sigaction ( SIGTERM, &halt_action, NULL );  /* terminate */
    sigaction ( SIGBUS, &halt_action, NULL );   /* out of memory?? */
    sigaction ( SIGALRM, &alarm_action, NULL ); /* endless loop check */

    alarm ( 300 );

    /* 
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug ( 2 );
#endif

    /* 
     * Init time.
     */
    gettimeofday ( &now_time, NULL );
    current_time = ( time_t ) now_time.tv_sec;
    strcpy ( str_boot_time, ctime ( &current_time ) );

    boot_time = current_time;

    /* 
     * Macintosh console initialization.
     */
#if defined(macintosh)
    console_options.nrows = 31;
    cshow ( stdout );
    csetmode ( C_RAW, stdin );
    cecho2file ( "log file", 1, stderr );
#endif

    /* 
     * Reserve one channel for our use.
     */

    if ( atoi ( argv[1] ) == 7202 )
    {
        IS_DEVMUD = true;
    }

    if ( atoi ( argv[1] ) == 7201 )
    {
        IS_BLDMUD = true;
    }

    if ( atoi ( argv[1] ) == 7200 )
    {
        IS_LIVEMUD = true;
    }


    /* 
     * Get the port number. */
    // Default Port
    port = 7200;
    if ( argc > 1 )
    {
        if ( !is_number ( argv[1] ) )
        {
            fprintf ( stderr, "Usage: %s [port ##]\n", argv[0] );
            quit ( 1 );
        }
        else if ( ( port = atoi ( argv[1] ) ) <= 1024 )
        {
            fprintf ( stderr, "Port numbers must be above 1024.\n" );
            quit ( 1 );
        }
    }
    if ( argc > 3 )
    {
        if ( !str_cmp ( argv[3], "copyover" ) && is_number ( argv[4] ) &&
             is_number ( argv[5] ) )
        {
            fCopyOver = true;
            control = atoi ( argv[4] );
        }
        else
            fCopyOver = false;
    }

    /* 
     * Run the game.
     */

    log_string ( "Initializing Telnet Port." );
    if ( fCopyOver == false )
        control = init_socket ( port );

    boot_db (  );
    arena = FIGHT_OPEN;
    sprintf ( madmin_reroll, "someone" );
    sprintf ( lastwinner, "Nobody" );
    sprintf ( log_buf, "MUD Booted: Telnet: %d", port );
    log_string ( log_buf );
    
    if ( fCopyOver == true )
    {
        log_string ( "Initiating CopyOver Recovery" );
        copyover_recover (  );
        log_string ( "CopyOver Recovery Completed." );
        do_aclear ( NULL, NULL );
    }

    game_loop_unix ( control );
    close ( control );

    /* 
     * That's all, folks.
     */
    log_string ( "Normal termination of game." );
    quit ( 0 );
    return 0;
}

#if defined(unix)
int init_socket ( int port )
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;
    int fd;

    if ( ( fd = socket ( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror ( "Init_socket: socket" );
        quit ( 1 );
    }

    if ( setsockopt
         ( fd, SOL_SOCKET, SO_REUSEADDR, ( char * ) &x, sizeof ( x ) ) < 0 )
    {
        perror ( "Init_socket: SO_REUSEADDR" );
        close ( fd );
        quit ( 1 );
    }

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
        struct linger ld;

        ld.l_onoff = 1;
        ld.l_linger = 1000;

        if ( setsockopt
             ( fd, SOL_SOCKET, SO_DONTLINGER, ( char * ) &ld,
               sizeof ( ld ) ) < 0 )
        {
            perror ( "Init_socket: SO_DONTLINGER" );
            close ( fd );
            quit ( 1 );
        }
    }
#endif

    sa = sa_zero;
    sa.sin_family = AF_INET;
    sa.sin_port = htons ( port );

    if ( bind ( fd, ( struct sockaddr * ) &sa, sizeof ( sa ) ) < 0 )
    {
        perror ( "Init socket: bind" );
        close ( fd );
        quit ( 1 );
    }

    if ( listen ( fd, 3 ) < 0 )
    {
        perror ( "Init socket: listen" );
        close ( fd );
        quit ( 1 );
    }

    return fd;
}
#endif

#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos ( void )
{
    struct timeval last_time;
    struct timeval now_time;
    static DESCRIPTOR_DATA dcon;

    gettimeofday ( &last_time, NULL );
    current_time = ( time_t ) last_time.tv_sec;

    /* 
     * New_descriptor analogue.
     */
    dcon.descriptor = 0;
    dcon.connected = CON_GET_NAME;
    dcon.host = str_dup ( "localhost" );
    dcon.outsize = 2000;
    dcon.outbuf = alloc_mem ( dcon.outsize );
    dcon.next = descriptor_list;
    dcon.showstr_head = NULL;
    dcon.showstr_point = NULL;
    dcon.pEdit = NULL;          /* OLC */
    dcon.pString = NULL;        /* OLC */
    dcon.editor = 0;            /* OLC */
    dcon.ansi = true;
    descriptor_list = &dcon;

    /* 
     * Send the greeting.
     */
    {
        extern char *help_greetinga;
        extern char *help_authors;
        extern char *help_login;

        write_to_buffer ( &dcon, help_authors, 0 );
        write_to_buffer ( &dcon, help_login, 0 );
        write_to_buffer ( &dcon, help_greetinga, 0 );
    }

    /* Main loop */
    while ( !merc_down )
    {
        DESCRIPTOR_DATA *d;

        /* 
         * Process input.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            d->fcommand = false;

#if defined(MSDOS)
            if ( kbhit (  ) )
#endif
            {
                if ( d->character != NULL )
                    d->character->timer = 0;
                if ( !read_from_descriptor ( d ) )
                {
/*                    if ( d->character != NULL )
                        save_char_obj ( d->character ); */
                    d->outtop = 0;
                    close_socket ( d );
                    continue;
                }
            }

            if ( d->character != NULL && d->character->daze > 0 )
                --d->character->daze;

            if ( d->character != NULL && d->character->wait > 0 )
            {
                --d->character->wait;
                continue;
            }

            read_from_buffer ( d );
            if ( d->incomm[0] != '\0' )
            {
                d->fcommand = true;
                stop_idling ( d->character );

                /* OLC */
                if ( d->showstr_point )
                    show_string ( d, d->incomm );
                else if ( d->pString )
                    string_add ( d->character, d->incomm );
                else
                    switch ( d->connected )
                    {
                        case CON_PLAYING:
                            if ( !run_olc_editor ( d ) )
                                substitute_alias ( d, d->incomm );
                            break;
                        default:
                            nanny ( d, d->incomm );
                            break;
                    }

                d->incomm[0] = '\0';
            }
        }

        /* 
         * Autonomous game motion.
         */
        update_handler ( false );

        /* 
         * Output.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;

            if ( ( d->fcommand || d->outtop > 0 ) )
            {
                if ( !process_output ( d, true ) )
                {
//                    if ( d->character != NULL && d->character->level > 1 )
//                        save_char_obj ( d->character );
                    d->outtop = 0;
                    close_socket ( d );
                }
            }
        }

        /* 
         * Synchronize to a clock.
         * Busy wait (blargh).
         */
        now_time = last_time;
        for ( ;; )
        {
            int delta;

#if defined(MSDOS)
            if ( kbhit (  ) )
#endif
            {
                if ( dcon.character != NULL )
                    dcon.character->timer = 0;
                if ( !read_from_descriptor ( &dcon ) )
                {
//                    if ( dcon.character != NULL && d->character->level > 1 )
//                        save_char_obj ( d->character );
                    dcon.outtop = 0;
                    close_socket ( &dcon );
                }
#if defined(MSDOS)
                break;
#endif
            }

            gettimeofday ( &now_time, NULL );
            delta =
                ( now_time.tv_sec - last_time.tv_sec ) * 1000 * 1000 +
                ( now_time.tv_usec - last_time.tv_usec );
            if ( delta >= 1000000 / PULSE_PER_SECOND )
                break;
        }
        last_time = now_time;
        current_time = ( time_t ) last_time.tv_sec;
    }

    return;
}
#endif

#if defined(unix)
void game_loop_unix ( int control )
{
    static struct timeval null_time;
    struct timeval last_time;

    signal ( SIGPIPE, SIG_IGN );
    gettimeofday ( &last_time, NULL );
    current_time = ( time_t ) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
        fd_set in_set;
        fd_set out_set;
        fd_set exc_set;
        DESCRIPTOR_DATA *d;
        int maxdesc;

#if defined(MALLOC_DEBUG)
        if ( malloc_verify (  ) != 1 )
            abort (  );
#endif

        /* 
         * Poll all active descriptors.
         */
        FD_ZERO ( &in_set );
        FD_ZERO ( &out_set );
        FD_ZERO ( &exc_set );
        FD_SET ( control, &in_set );
        maxdesc = control;
        for ( d = descriptor_list; d; d = d->next )
        {
            maxdesc = UMAX ( maxdesc, d->descriptor );
            FD_SET ( d->descriptor, &in_set );
            FD_SET ( d->descriptor, &out_set );
            FD_SET ( d->descriptor, &exc_set );
        }

        if ( select ( maxdesc + 1, &in_set, &out_set, &exc_set, &null_time ) <
             0 )
        {
            perror ( "Game_loop: select: poll" );
            quit ( 1 );
        }

        /* 
         * New connection?
         */
        if ( FD_ISSET ( control, &in_set ) )
            init_descriptor ( control );

        FD_ZERO ( &in_set );
        FD_ZERO ( &out_set );
        FD_ZERO ( &exc_set );
        for ( d = descriptor_list; d; d = d->next )
        {
            maxdesc = UMAX ( maxdesc, d->descriptor );
            FD_SET ( d->descriptor, &in_set );
            FD_SET ( d->descriptor, &out_set );
            FD_SET ( d->descriptor, &exc_set );
        }

        if ( select ( maxdesc + 1, &in_set, &out_set, &exc_set, &null_time ) <
             0 )
        {
            perror ( "Game_loop: select: poll" );
            quit ( 1 );
        }

        /* 
         * Kick out the freaky folks.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            if ( FD_ISSET ( d->descriptor, &exc_set ) )
            {
                FD_CLR ( d->descriptor, &in_set );
                FD_CLR ( d->descriptor, &out_set );
//                if ( d->character && d->character->level > 1 )
//                    save_char_obj ( d->character );
                d->outtop = 0;
                close_socket ( d );
            }
        }

        /* 
         * Process input.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            d->fcommand = false;

            if ( FD_ISSET ( d->descriptor, &in_set ) )
            {
                if ( d->character != NULL )
                    d->character->timer = 0;
                if ( !read_from_descriptor ( d ) )
                {
                    FD_CLR ( d->descriptor, &out_set );
//                    if ( d->character != NULL && d->character->level > 1 )
//                        save_char_obj ( d->character );
                    d->outtop = 0;
                    close_socket ( d );
                    continue;
                }
            }

            if ( d->character != NULL && d->character->daze > 0 )
                --d->character->daze;

            if ( d->character != NULL && d->character->wait > 0 )
            {
                --d->character->wait;
                continue;
            }

            read_from_buffer ( d );
            if ( d->incomm[0] != '\0' )
            {
                d->fcommand = true;
                stop_idling ( d->character );

                /* OLC */
                if ( d->showstr_point )
                    show_string ( d, d->incomm );
                else if ( d->pString )
                    string_add ( d->character, d->incomm );
                else
                    switch ( d->connected )
                    {
                        case CON_PLAYING:
                            if ( !run_olc_editor ( d ) )
                                substitute_alias ( d, d->incomm );
                            break;
                        default:

                            nanny ( d, d->incomm );
                            break;
                    }

                d->incomm[0] = '\0';
            }
        }

        /* 
         * Autonomous game motion.
         */
        update_handler ( false );

        /* 
         * Output.
         */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;

            if ( ( d->fcommand || d->outtop > 0 ) &&
                 FD_ISSET ( d->descriptor, &out_set ) )
            {
                if ( !process_output ( d, true ) )
                {
 //                   if ( d->character != NULL && d->character->level > 1 )
 //                       save_char_obj ( d->character );
                    d->outtop = 0;
                    close_socket ( d );
                }
            }
        }

        /* 
         * Synchronize to a clock.
         * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
         * Careful here of signed versus unsigned arithmetic.
         */
        {
            struct timeval now_time;
            long secDelta;
            long usecDelta;

            gettimeofday ( &now_time, NULL );
            usecDelta =
                ( ( int ) last_time.tv_usec ) - ( ( int ) now_time.tv_usec ) +
                1000000 / PULSE_PER_SECOND;
            secDelta =
                ( ( int ) last_time.tv_sec ) - ( ( int ) now_time.tv_sec );
            while ( usecDelta < 0 )
            {
                usecDelta += 1000000;
                secDelta -= 1;
            }

            while ( usecDelta >= 1000000 )
            {
                usecDelta -= 1000000;
                secDelta += 1;
            }

            if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
            {
                struct timeval stall_time;

                stall_time.tv_usec = usecDelta;
                stall_time.tv_sec = secDelta;
                if ( select ( 0, NULL, NULL, NULL, &stall_time ) < 0 )
                {
                    if ( errno != EINTR )
                    {
                        perror ( "Game_loop: select: stall" );
                        quit ( 1 );
                    }
                }
            }
        }

        gettimeofday ( &last_time, NULL );
        current_time = ( time_t ) last_time.tv_sec;
    }

    return;
}
#endif

/*void game_loop_unix( int control )
{
    static struct timeval null_time;
    struct timeval last_time;

    signal( SIGPIPE, SIG_IGN );
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

                       *//* Main loop *//* 
                       while ( !merc_down ) { fd_set in_set; fd_set out_set;
                       fd_set exc_set; DESCRIPTOR_DATA *d; int maxdesc;

                       #if defined(MALLOC_DEBUG) if ( malloc_verify( ) != 1 )
                       abort( ); #endif

    *//* 
    * Poll all active descriptors.
    *//* 
      FD_ZERO( &in_set ); FD_ZERO( &out_set ); FD_ZERO( &exc_set ); FD_SET(
      control, &in_set ); maxdesc = control; for ( d = descriptor_list; d; d =
      d->next ) { maxdesc = UMAX( maxdesc, d->descriptor ); FD_SET(
      d->descriptor, &in_set ); FD_SET( d->descriptor, &out_set ); FD_SET(
      d->descriptor, &exc_set ); }

      if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 ) {
      perror( "Game_loop: select: poll" ); exit( 1 ); } */
    /* 
     * New connection?
    *//* 
      if ( FD_ISSET( control, &in_set ) ) init_descriptor( control );

      FD_ZERO( &in_set ); FD_ZERO( &out_set ); FD_ZERO( &exc_set ); FD_SET(
      wwwcontrol, &in_set ); maxdesc = wwwcontrol; for ( d = descriptor_list;
      d; d = d->next ) { maxdesc = UMAX( maxdesc, d->descriptor ); FD_SET(
      d->descriptor, &in_set ); FD_SET( d->descriptor, &out_set ); FD_SET(
      d->descriptor, &exc_set ); }

      if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 ) {
      perror( "Game_loop: select: poll" ); exit( 1 ); }

      if ( FD_ISSET( wwwcontrol, &in_set ) ) init_descriptor_www( wwwcontrol ); */
    /* 
     * Kick out the freaky folks.
    *//* 
      for ( d = descriptor_list; d != NULL; d = d_next ) { d_next = d->next;
      if ( FD_ISSET( d->descriptor, &exc_set ) ) { FD_CLR( d->descriptor,
      &in_set ); FD_CLR( d->descriptor, &out_set ); if ( d->character &&
      d->character->level > 1) save_char_obj( d->character ); d->outtop = 0;
      close_socket( d ); } } */
    /* 
     * Process input.
    *//* 
      for ( d = descriptor_list; d != NULL; d = d_next ) { d_next = d->next;
      d->fcommand = false;

      if ( FD_ISSET( d->descriptor, &in_set ) ) { if ( d->character != NULL )
      d->character->timer = 0; if ( !read_from_descriptor( d ) ) { FD_CLR(
      d->descriptor, &out_set ); if ( d->character != NULL &&
      d->character->level > 1) save_char_obj( d->character ); d->outtop = 0;
      close_socket( d ); continue; } }

      if (d->character != NULL && d->character->daze > 0) --d->character->daze;

      if ( d->character != NULL && d->character->wait > 0 ) {
      --d->character->wait; continue; }

      read_from_buffer( d ); if ( d->incomm[0] != '\0' ) { d->fcommand = true;
      stop_idling( d->character );

             *//* OLC *//* 
               if ( d->showstr_point ) show_string( d, d->incomm ); else if (
               d->pString ) string_add( d->character, d->incomm ); else switch
               ( d->connected ) { case CON_PLAYING: if ( !run_olc_editor( d ) )
               substitute_alias( d, d->incomm ); break; default:

               nanny( d, d->incomm ); break; }

               d->incomm[0] = '\0'; } }

             */
    /* 
     * Autonomous game motion.
    *//* 
      update_handler( false );

    */
    /* 
     * Output.
    *//* 
      for ( d = descriptor_list; d != NULL; d = d_next ) { d_next = d->next;

      if ( ( d->fcommand || d->outtop > 0 ) && FD_ISSET(d->descriptor,
      &out_set) ) { if ( !process_output( d, true ) ) { if ( d->character !=
      NULL && d->character->level > 1) save_char_obj( d->character ); d->outtop 
      = 0; close_socket( d ); } } }

    *//* 
    * Synchronize to a clock.
    * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
    * Careful here of signed versus unsigned arithmetic.
    *//* 
      { struct timeval now_time; long secDelta; long usecDelta;

      gettimeofday( &now_time, NULL ); usecDelta = ((int) last_time.tv_usec) -
      ((int) now_time.tv_usec) + 1000000 / PULSE_PER_SECOND; secDelta = ((int)
      last_time.tv_sec ) - ((int) now_time.tv_sec ); while ( usecDelta < 0 ) {
      usecDelta += 1000000; secDelta -= 1; }

      while ( usecDelta >= 1000000 ) { usecDelta -= 1000000; secDelta += 1; }

      if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) ) { struct
      timeval stall_time;

      stall_time.tv_usec = usecDelta; stall_time.tv_sec = secDelta; if (
      select( 0, NULL, NULL, NULL, &stall_time ) < 0 ) { perror( "Game_loop:
      select: stall" ); exit( 1 ); } } }

      gettimeofday( &last_time, NULL ); current_time = (time_t)
      last_time.tv_sec; }

      return; } #endif */

#if defined(unix)
void init_descriptor ( int control )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
    struct hostent *from;
    int desc;
    int size;

    size = sizeof ( sock );
    getsockname ( control, ( struct sockaddr * ) &sock, (socklen_t *) &size );
    if ( ( desc = accept ( control, ( struct sockaddr * ) &sock, (socklen_t *) &size ) ) < 0 )
    {
        perror ( "New_descriptor: accept" );
        return;
    }

#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

    if ( fcntl ( desc, F_SETFL, FNDELAY ) == -1 )
    {
        perror ( "New_descriptor: fcntl: FNDELAY" );
        return;
    }

    /* 
     * Cons a new descriptor.
     */
    dnew = new_descriptor (  );
    dnew->descriptor = desc;
    dnew->pEdit = NULL;         /* OLC */
    dnew->pString = NULL;       /* OLC */
    dnew->editor = 0;           /* OLC */
    dnew->ansi = true;
    size = sizeof ( sock );
    if ( getpeername ( desc, ( struct sockaddr * ) &sock, (socklen_t *) &size ) < 0 )
    {
        perror ( "New_descriptor: getpeername" );
        dnew->host = str_dup ( "(unknown)" );
    }
    else
    {
        /* 
         * Would be nice to use inet_ntoa here but it takes a struct arg,
         * which ain't very compatible between gcc and system libraries.
         */
        int addr;

        addr = ntohl ( sock.sin_addr.s_addr );
        sprintf ( buf, "%d.%d.%d.%d", ( addr >> 24 ) & 0xFF,
                  ( addr >> 16 ) & 0xFF, ( addr >> 8 ) & 0xFF,
                  ( addr ) & 0xFF );
        sprintf ( log_buf, "Sock.sinaddr:  %s", buf );
        log_string ( log_buf );
        from =
            gethostbyaddr ( ( char * ) &sock.sin_addr, sizeof ( sock.sin_addr ),
                            AF_INET );
        dnew->host = str_dup ( from ? from->h_name : buf );
    }

    /* 
     * Swiftest: I added the following to ban sites.  I don't
     * endorse banning of sites, but Copper has few descriptors now
     * and some people from certain sites keep abusing access by
     * using automated 'autodialers' and leaving connections hanging.
     *
     * Furey: added suffix check by request of Nickel of HiddenWorlds.
     */
    if ( check_ban ( dnew->host, BAN_ALL ) )
    {
        write_to_descriptor ( desc,
                              "Your site has been banned from this mud.\n\r",
                              0 );
        close ( desc );
        free_descriptor ( dnew );
        return;
    }
    /* 
     * Init descriptor data.
     */
    dnew->next = descriptor_list;
    descriptor_list = dnew;

    /* 
     * Send the greeting.
     */
    {
            write_to_buffer ( dnew, help_authors, 0 );
            write_to_buffer ( dnew, help_greetinga, 0 );
            if ( happy_hour )
                write_to_buffer ( dnew, "{R** {YH A P P Y  H O U R ! {R**{w\n\r", 0 );
            write_to_buffer ( dnew, help_login, 0 );

    }
    return;
}
#endif

void close_socket ( DESCRIPTOR_DATA * dclose )
{
    CHAR_DATA *ch;
    char buf[MSL];

    if ( dclose->outtop > 0 )
        process_output ( dclose, false );

    if ( dclose->snoop_by != NULL )
    {
        write_to_buffer ( dclose->snoop_by,
                          "Your victim has left the game.\n\r", 0 );
    }

    {
        DESCRIPTOR_DATA *d;

        for ( d = descriptor_list; d != NULL; d = d->next )
        {
            if ( d->snoop_by == dclose )
                d->snoop_by = NULL;
        }
    }

    if ( ( ch = dclose->character ) != NULL )
    {
        sprintf ( log_buf, "Closing link to %s.", ch->name );
        log_string ( log_buf );
        if ( dclose->connected == CON_PLAYING )
        {
            sprintf ( buf, "%s's link has been lost.\n\r", ch->name );
            wiznet ( buf, NULL, NULL, WIZ_LINKS, 0, 0 );
            ch->desc = NULL;
        }
        else
        {
            free_char ( dclose->original ? dclose->original : dclose->
                        character );
        }
    }

    if ( d_next == dclose )
        d_next = d_next->next;

    if ( dclose == descriptor_list )
    {
        descriptor_list = descriptor_list->next;
    }
    /* if ( IS_SET(ch->act2,PLR2_CHALLENGER) && arena == 0 ) { char
       buf[MAX_STRING_LENGTH];

       REMOVE_BIT(ch->act2,PLR2_CHALLENGER);
       REMOVE_BIT(ch->challenged->act2,PLR2_CHALLENGED);
       ch->challenged->challenger = NULL; ch->challenged = NULL; arena =
       FIGHT_OPEN; sprintf(buf, "[Arena] %s has lost $s link. Arena is OPEN.",
       ch->name); return; } */
    else
    {
        DESCRIPTOR_DATA *d;

        for ( d = descriptor_list; d && d->next != dclose; d = d->next )
            ;
        if ( d != NULL )
            d->next = dclose->next;
        else
            bug ( "Close_socket: dclose not found.", 0 );
    }

    close ( dclose->descriptor );
    free_descriptor ( dclose );
#if defined(MSDOS) || defined(macintosh)
    quit ( 1 );
#endif
    return;
}

bool read_from_descriptor ( DESCRIPTOR_DATA * d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
        return true;

    /* Check for overflow. */
    iStart = strlen ( d->inbuf );
    if ( iStart >= sizeof ( d->inbuf ) - 15 )
    {
        sprintf ( log_buf, "%s input overflow!", d->host );
        log_string ( log_buf );
        write_to_descriptor ( d->descriptor,
                              "\n\r*** Spam kick warning ***\n\r", 0 );
        return false;
    }

    /* Snarf input. */
#if defined(macintosh)
    for ( ;; )
    {
        int c;

        c = getc ( stdin );
        if ( c == '\0' || c == EOF )
            break;
        putc ( c, stdout );
        if ( c == '\r' )
            putc ( '\n', stdout );
        d->inbuf[iStart++] = c;
        if ( iStart > sizeof ( d->inbuf ) - 10 )
            break;
    }
#endif

#if defined(MSDOS) || defined(unix)
    for ( ;; )
    {
        int nRead;

        nRead =
            read ( d->descriptor, d->inbuf + iStart,
                   sizeof ( d->inbuf ) - 10 - iStart );
        if ( nRead > 0 )
        {
            iStart += nRead;
            if ( d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r' )
                break;
        }
        else if ( nRead == 0 )
        {
            log_string ( "EOF encountered on read." );
            return false;
        }
        else if ( errno == EWOULDBLOCK )
            break;
        else
        {
            perror ( "Read_from_descriptor" );
            return false;
        }
    }
#endif

    d->inbuf[iStart] = '\0';
    return true;
}

/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer ( DESCRIPTOR_DATA * d )
{
    int i, j, k;

    /* 
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
        return;

    /* 
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
        if ( d->inbuf[i] == '\0' )
            return;
    }

    /* 
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
        if ( k >= MAX_INPUT_LENGTH - 2 )
        {
            write_to_descriptor ( d->descriptor, "Line too long.\n\r", 0 );

            /* skip the rest of the line */
            for ( ; d->inbuf[i] != '\0'; i++ )
            {
                if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
                    break;
            }
            d->inbuf[i] = '\n';
            d->inbuf[i + 1] = '\0';
            break;
        }

        if ( d->inbuf[i] == '\b' && k > 0 )
            --k;
        else if ( isascii ( d->inbuf[i] ) && isprint ( d->inbuf[i] ) )
            d->incomm[k++] = d->inbuf[i];
    }

    /* 
     * Finish off the line.
     */
    if ( k == 0 )
        d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /* 
     * Deal with bozos with #repeat 1000 ...
     */

    if ( k > 1 || d->incomm[0] == '!' )
    {
        if ( d->incomm[0] != '!' && str_cmp ( d->incomm, d->inlast ) )
        {
            d->repeat = 0;
        }
        else
        {
            d->repeat++;
            if ( d->repeat == 25 && d->character &&
                 d->connected == CON_PLAYING )
            {
                sprintf ( log_buf, "%s is spamming '%s'!", d->character->name, d->incomm );
                wiznet ( log_buf, d->character, NULL, WIZ_SPAM, 0, 0 );
                log_string ( log_buf );

                write_to_descriptor ( d->descriptor,
                                      "\n\r\n\r*** STOP SPAMMING ***\n\r\n\r", 0 );

            }
            else if ( d->repeat == 35 && d->character &&
                      d->connected == CON_PLAYING )
            {
                sprintf ( log_buf, "%s is STILL spamming '%s'!", d->character->name, d->incomm );
                wiznet ( log_buf, d->character, NULL, WIZ_SPAM, 0, 0 );
                log_string ( log_buf );

                write_to_descriptor ( d->descriptor,
                                      "\n\r\n\r*** ! STOP SPAMMING ! ***\n\r\n\r", 0 );
            }
            else if ( d->repeat >= 45 && d->character &&
                      d->connected == CON_PLAYING )
            {
                d->repeat = 0;

                sprintf ( log_buf, "%s KICKED for spamming '%s'!", d->character->name, d->incomm );
                wiznet ( log_buf, d->character, NULL, WIZ_SPAM, 0, 0 );
                log_string ( log_buf );

                write_to_descriptor ( d->descriptor,
                                      "\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r*** !!! I WARNED YOU !!! ***\n\r", 0 );

                strcpy ( d->incomm, "quit force" );
            }
        }
    }

    /* 
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
        strcpy ( d->incomm, d->inlast );
    else
        strcpy ( d->inlast, d->incomm );

    /* 
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
        i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i + j] ) != '\0'; j++ )
        ;
    return;
}

/** Function: process_ansi_output
* Descr   : Sends buffered output to descriptor, converting any ANSI codes
*         : along the way if character has selected ANSI.
* Returns : true/false based on success of output to descriptor
* Syntax  : (N/A)
* Written : v1.0 5/98
* Author  : Lope, updated by Gary McNickle <gary@tarmongaidon.org>
*/
bool process_ansi_output ( DESCRIPTOR_DATA * d )
{
    CHAR_DATA *ch;
    char *counter;
    char output[MSL];
    char clcode[MSL];
    char *work;
    bool success = true;
    char *i;
    int rcol;

    /* 
     * No sense in dealing with a null descriptor 
     */
    if ( d == NULL )
        return false;

    /* 
     * Initialize output buffer, counter and working string 
     */
    memset ( output, 0, MSL );
    counter = output;
    work = d->outbuf;

    while ( *work != '\0' && ( work - d->outbuf ) < d->outtop )
    {
        if ( ( int ) ( counter - output ) >= MSL - 32 )
        {                       /* 
                                 * have a full buffer 
                                 */
            *counter++ = '\0';

            if ( !
                 ( success =
                   write_to_descriptor ( d->descriptor, output,
                                         strlen ( output ) ) ) )
                break;          /* problems...  */

            memset ( output, 0, MSL );
            counter = output;   /* increment counter */
        }

        if ( *work != '{' )
        {
            *counter++ = *work++;
            continue;
        }

        /* 
         * Ok, we have an ansi request... process it 
         */
        /* 
         * Pass the '{' code, next should be actual color code itself 
         */
        work++;

        clcode[0] = '\0';       /* Initialize buffer */

        ch = d->character;

        if ( d->ansi == true )
            switch ( *work )
            {
                default:
                    strcpy ( clcode, colour_clear ( ch ) );
                    break;
                case 'x':
                    strcpy ( clcode, colour_clear ( ch ) );
                    break;
                case '0':
                    strcpy ( clcode, colour_clear ( ch ) );
                    break;
                case 'z':
                    strcpy ( clcode, BLINK );
                    break;
                case 'b':
                    strcpy ( clcode, C_BLUE );
                    break;
                case '4':
                    strcpy ( clcode, C_BLUE );
                    break;
                case 'c':
                    strcpy ( clcode, C_CYAN );
                    break;
                case '6':
                    strcpy ( clcode, C_CYAN );
                    break;
                case 'g':
                    strcpy ( clcode, C_GREEN );
                    break;
                case '2':
                    strcpy ( clcode, C_GREEN );
                    break;
                case 'm':
                    strcpy ( clcode, C_MAGENTA );
                    break;
                case '5':
                    strcpy ( clcode, C_MAGENTA );
                    break;
                case 'r':
                    strcpy ( clcode, C_RED );
                    break;
                case '1':
                    strcpy ( clcode, C_RED );
                    break;
                case 'w':
                    strcpy ( clcode, C_WHITE );
                    break;
                case '7':
                    strcpy ( clcode, C_WHITE );
                    break;
                case 'y':
                    strcpy ( clcode, C_YELLOW );
                    break;
                case '3':
                    strcpy ( clcode, C_YELLOW );
                    break;
                case 'B':
                    strcpy ( clcode, C_B_BLUE );
                    break;
                case '$':
                    strcpy ( clcode, FLASHING );
                    break;
                case 'C':
                    strcpy ( clcode, C_B_CYAN );
                    break;
                case '^':
                    strcpy ( clcode, C_B_CYAN );
                    break;
                case 'G':
                    strcpy ( clcode, C_B_GREEN );
                    break;
                case '@':
                    strcpy ( clcode, C_B_GREEN );
                    break;
                case 'M':
                    strcpy ( clcode, C_B_MAGENTA );
                    break;
                case '%':
                    strcpy ( clcode, C_B_MAGENTA );
                    break;
                case 'R':
                    strcpy ( clcode, C_B_RED );
                    break;
                case '!':
                    strcpy ( clcode, C_B_RED );
                    break;
                case 'W':
                    strcpy ( clcode, C_B_WHITE );
                    break;
                case '&':
                    strcpy ( clcode, C_B_WHITE );
                    break;
                case 'Y':
                    strcpy ( clcode, C_B_YELLOW );
                    break;
                case '#':
                    strcpy ( clcode, C_B_YELLOW );
                    break;
                case 'D':
                    strcpy ( clcode, C_D_GREY );
                    break;
                case '8':
                    strcpy ( clcode, C_D_GREY );
                    break;
                case '-':
                    switch ( number_range ( 1, 6 ) )
                    {
                        case 1:
                            strcpy ( clcode, C_B_CYAN );
                            break;
                        case 2:
                            strcpy ( clcode, C_B_CYAN );
                            break;

                        case 3:
                            strcpy ( clcode, C_CYAN );
                            break;
                            case 4:strcpy ( clcode, C_CYAN );

                            break;
                        case 5:
                            strcpy ( clcode, C_CYAN );
                            break;

                        case 6:
                            strcpy ( clcode, C_B_WHITE );
                            break;
                    }
                    break;

                case '+':
                    switch ( number_range ( 1, 6 ) )
                    {
                        case 1:
                            strcpy ( clcode, C_D_GREY );
                            break;
                        case 2:
                            strcpy ( clcode, C_D_GREY );
                            break;
                        case 3:
                            strcpy ( clcode, C_D_GREY );
                            break;

                            case 4:strcpy ( clcode, C_WHITE );

                            break;
                        case 5:
                            strcpy ( clcode, C_WHITE );
                            break;

                        case 6:
                            strcpy ( clcode, C_B_WHITE );
                            break;
                    }
                    break;

                case 'O':
                    switch ( number_range ( 1, 6 ) )
                    {
                        case 1:
                            strcpy ( clcode, C_B_RED );
                            break;
                        case 2:
                            strcpy ( clcode, C_B_RED );
                            break;

                        case 3:
                            strcpy ( clcode, C_RED );
                            break;
                            case 4:strcpy ( clcode, C_RED );

                            break;

                        case 5:
                            strcpy ( clcode, C_B_YELLOW );
                            break;
                        case 6:
                            strcpy ( clcode, C_YELLOW );
                            break;
                    }
                    break;

                case 'p':
                    switch ( number_range ( 1, 6 ) )
                    {
                        case 1:
                            strcpy ( clcode, C_YELLOW );
                            break;
                        case 2:
                            strcpy ( clcode, C_YELLOW );
                            break;
                        case 3:
                            strcpy ( clcode, C_YELLOW );
                            break;

                            case 4:strcpy ( clcode, C_B_YELLOW );

                            break;
                        case 5:
                            strcpy ( clcode, C_B_YELLOW );
                            break;

                        case 6:
                            strcpy ( clcode, C_D_GREY );
                            break;
                    }
                    break;

                case '=':
                    switch ( number_range ( 1, 6 ) )
                    {
                        case 1:
                            strcpy ( clcode, C_RED );
                            break;
                        case 2:
                            strcpy ( clcode, C_RED );
                            break;
                        case 3:
                            strcpy ( clcode, C_RED );
                            break;

                            case 4:strcpy ( clcode, C_B_RED );

                            break;
                        case 5:
                            strcpy ( clcode, C_B_RED );
                            break;

                        case 6:
                            strcpy ( clcode, C_D_GREY );
                            break;
                    }
                    break;

                case 'I':
                    switch ( number_range ( 1, 6 ) )
                    {
                        case 1:
                            strcpy ( clcode, C_BLUE );
                            break;
                        case 2:
                            strcpy ( clcode, C_BLUE );
                            break;
                        case 3:
                            strcpy ( clcode, C_BLUE );
                            break;

                            case 4:strcpy ( clcode, C_B_BLUE );

                            break;
                        case 5:
                            strcpy ( clcode, C_B_BLUE );
                            break;

                        case 6:
                            strcpy ( clcode, C_D_GREY );
                            break;
                    }
                    break;

                case 'q':
                    switch ( number_range ( 1, 6 ) )
                    {
                        case 1:
                            strcpy ( clcode, C_GREEN );
                            break;
                        case 2:
                            strcpy ( clcode, C_GREEN );
                            break;
                        case 3:
                            strcpy ( clcode, C_GREEN );
                            break;

                            case 4:strcpy ( clcode, C_B_GREEN );

                            break;
                        case 5:
                            strcpy ( clcode, C_B_GREEN );
                            break;

                        case 6:
                            strcpy ( clcode, C_D_GREY );
                            break;
                    }
                    break;

                case 'd':
                    switch ( number_range ( 1, 6 ) )
                    {
                        case 1:
                            strcpy ( clcode, C_MAGENTA );
                            break;
                        case 2:
                            strcpy ( clcode, C_MAGENTA );
                            break;
                        case 3:
                            strcpy ( clcode, C_MAGENTA );
                            break;

                            case 4:strcpy ( clcode, C_B_MAGENTA );

                            break;
                        case 5:
                            strcpy ( clcode, C_B_MAGENTA );
                            break;

                        case 6:
                            strcpy ( clcode, C_D_GREY );
                            break;
                    }
                    break;

                case 'o':
                    switch ( number_range ( 1, 3 ) )
                    {

                        case 1:
                            strcpy ( clcode, C_WHITE );
                            break;

                        case 2:
                            strcpy ( clcode, C_B_YELLOW );
                            break;

                        case 3:
                            strcpy ( clcode, C_B_WHITE );
                            break;
                    }
                    break;

                case 'n':
                    switch ( number_range ( 1, 8 ) )
                    {
                        case 1:
                            strcpy ( clcode, C_GREEN );
                            break;
                        case 2:
                            strcpy ( clcode, C_GREEN );
                            break;
                        case 3:
                            strcpy ( clcode, C_GREEN );
                            break;

                            case 4:strcpy ( clcode, C_B_BLUE );

                            break;
                        case 5:
                            strcpy ( clcode, C_B_BLUE );
                            break;

                        case 6:
                            strcpy ( clcode, C_B_GREEN );
                            break;
                        case 7:
                            strcpy ( clcode, C_B_GREEN );
                            break;

                        case 8:
                            strcpy ( clcode, C_BLUE );
                            break;
                    }
                    break;

                case '*':
                    if ( 1 == 1 )
                    {
                        rcol = number_range ( 1, 100 );
                        if ( rcol < 10 )
                            strcpy ( clcode, C_B_WHITE );

                        if ( rcol > 10 && rcol <= 25 )
                            strcpy ( clcode, C_WHITE );

                        if ( rcol > 25 && rcol <= 60 )
                            strcpy ( clcode, C_D_GREY );

                        if ( rcol > 60 && rcol <= 80 )
                            strcpy ( clcode, C_MAGENTA );

                        if ( rcol > 80 && rcol <= 100 )
                            strcpy ( clcode, C_BLUE );
                    }
                    break;

                case 'A':      /* Auction Channel */
                    if ( ch && ch->color_auc )
                        strcpy ( clcode, colour_channel ( ch->color_auc, ch ) );
                    else
                        strcpy ( clcode, C_B_GREEN );
                    break;
                case 'E':      /* Clan Gossip Channel */
                    if ( ch && ch->color_cgo )
                        strcpy ( clcode, colour_channel ( ch->color_cgo, ch ) );
                    else
                        strcpy ( clcode, C_B_RED );
                    break;
                case 'F':      /* Clan Talk Channel */
                    if ( ch && ch->color_cla )
                        strcpy ( clcode, colour_channel ( ch->color_cla, ch ) );
                    else
                        strcpy ( clcode, C_B_MAGENTA );
                    break;
                case 'H':      /* Gossip Channel */
                    if ( ch && ch->color_gos )
                        strcpy ( clcode, colour_channel ( ch->color_gos, ch ) );
                    else
                        strcpy ( clcode, C_B_BLUE );
                    break;
                case 'J':      /* Grats Channel */
                    if ( ch && ch->color_gra )
                        strcpy ( clcode, colour_channel ( ch->color_gra, ch ) );
                    else
                        strcpy ( clcode, C_YELLOW );
                    break;
                case 'K':      /* Group Tell Channel */
                    if ( ch && ch->color_gte )
                        strcpy ( clcode, colour_channel ( ch->color_gte, ch ) );
                    else
                        strcpy ( clcode, C_CYAN );
                    break;
                case 'L':      /* Immortal Talk Channel */
                    if ( ch && ch->color_imm )
                        strcpy ( clcode, colour_channel ( ch->color_imm, ch ) );
                    else
                        strcpy ( clcode, C_B_WHITE );
                    break;
                case 'N':      /* Music Channel */
                    if ( ch && ch->color_mus )
                        strcpy ( clcode, colour_channel ( ch->color_mus, ch ) );
                    else
                        strcpy ( clcode, C_B_CYAN );
                    break;
                case 'P':      /* Question+Answer Channel */
                    if ( ch && ch->color_que )
                        strcpy ( clcode, colour_channel ( ch->color_que, ch ) );
                    else
                        strcpy ( clcode, C_B_YELLOW );
                    break;
                case 'Q':      /* Quote Channel */
                    if ( ch && ch->color_quo )
                        strcpy ( clcode, colour_channel ( ch->color_quo, ch ) );
                    else
                        strcpy ( clcode, C_GREEN );
                    break;
                case 'S':      /* Say Channel */
                    if ( ch && ch->color_say )
                        strcpy ( clcode, colour_channel ( ch->color_say, ch ) );
                    else
                        strcpy ( clcode, C_MAGENTA );
                    break;
                case 'T':      /* Shout+Yell Channel */
                    if ( ch && ch->color_sho )
                        strcpy ( clcode, colour_channel ( ch->color_sho, ch ) );
                    else
                        strcpy ( clcode, C_RED );
                    break;
                case 'U':      /* Tell+Reply Channel */
                    if ( ch && ch->color_tel )
                        strcpy ( clcode, colour_channel ( ch->color_tel, ch ) );
                    else
                        strcpy ( clcode, C_CYAN );
                    break;
                case 'V':      /* Wiznet Messages */
                    if ( ch && ch->color_wiz )
                        strcpy ( clcode, colour_channel ( ch->color_wiz, ch ) );
                    else
                        strcpy ( clcode, C_WHITE );
                    break;
                case 'a':      /* Mobile Talk */
                    if ( ch && ch->color_mob )
                        strcpy ( clcode, colour_channel ( ch->color_mob, ch ) );
                    else
                        strcpy ( clcode, C_MAGENTA );
                    break;
                case 'e':      /* Room Title */
                    if ( ch && ch->color_roo )
                        strcpy ( clcode, colour_channel ( ch->color_roo, ch ) );
                    else
                        strcpy ( clcode, C_B_BLUE );
                    break;
                case 'f':      /* Opponent Condition */
                    if ( ch && ch->color_con )
                        strcpy ( clcode, colour_channel ( ch->color_con, ch ) );
                    else
                        strcpy ( clcode, C_B_RED );
                    break;
                case 'h':      /* Fight Actions */
                    if ( ch && ch->color_fig )
                        strcpy ( clcode, colour_channel ( ch->color_fig, ch ) );
                    else
                        strcpy ( clcode, C_B_BLUE );
                    break;
                case 'i':      /* Opponents Fight Actions */
                    if ( ch && ch->color_opp )
                        strcpy ( clcode, colour_channel ( ch->color_opp, ch ) );
                    else
                        strcpy ( clcode, C_CYAN );
                    break;
                case 'j':      /* Disarm Messages */
                    if ( ch && ch->color_dis )
                        strcpy ( clcode, colour_channel ( ch->color_dis, ch ) );
                    else
                        strcpy ( clcode, C_B_YELLOW );
                    break;
                case 'k':      /* Witness Messages */
                    if ( ch && ch->color_wit )
                        strcpy ( clcode, colour_channel ( ch->color_wit, ch ) );
                    else
                        strcpy ( clcode, colour_clear ( ch ) );
                    break;
                case 'l':      /* Quest Gossip */
                    if ( ch && ch->color_qgo )
                        strcpy ( clcode, colour_channel ( ch->color_qgo, ch ) );
                    else
                        strcpy ( clcode, C_B_CYAN );
                    break;

                case '{':
                    sprintf ( clcode, "%c", '{' );
                    break;
            }
        /* 
         * end: switch 
         */
        work++;                 /* pass the ansi request code */

        /* 
         * increment counters 
         */
        i = clcode;
        while ( ( *counter = *i ) != '\0' )
            ++counter, ++i;

    }

    success = success &&
        ( write_to_descriptor ( d->descriptor, output, strlen ( output ) ) );

    d->outtop = 0;
    return success;

}

/*
 * Low level output function.
 */
bool process_output ( DESCRIPTOR_DATA * d, bool fPrompt )
{
    /* 
     * Bust a prompt.
     */
    if ( !merc_down )
    {
        if ( d->showstr_point )
            write_to_buffer ( d, "[Hit Return to continue]\n\r", 0 );
        else if ( fPrompt && d->pString && d->connected == CON_PLAYING )
            write_to_buffer ( d, "> ", 2 );
        else if ( fPrompt && d->connected == CON_PLAYING )
        {
            CHAR_DATA *ch;

            CHAR_DATA *victim;

            ch = d->character;

            /* battle prompt */
            if ( ( victim = ch->fighting ) != NULL && can_see ( ch, victim ) )
            {
                float percent = 0.00;
                char meter[100];
                char buf[MAX_STRING_LENGTH];
                float victhit = 0.00;
                float victhitmax = 0.00;
                float chhit = 0.00;
                float chhitmax = 0.00;

                victhit = victim->hit;
                victhitmax = victim->max_hit;
                if ( victhit > 0 && victhitmax > 0 )
                    percent = ( ( victhit * 100 ) / victhitmax );
                else
                    percent = 0;

                if ( percent == 100 )
                    sprintf ( meter, "{c[{D+++{r++++{R+++{c]" );
                else if ( percent >= 90 )
                    sprintf ( meter, "{c[{D+++{r++++{r++ {c]" );
                else if ( percent >= 80 )
                    sprintf ( meter, "{c[{D+++{r++++{R+  {c]" );
                else if ( percent >= 70 )
                    sprintf ( meter, "{c[{D+++{r++++   {c]" );
                else if ( percent >= 60 )
                    sprintf ( meter, "{c[{D+++{r+++    {c]" );
                else if ( percent >= 50 )
                    sprintf ( meter, "{c[{D+++{r++     {c]" );
                else if ( percent >= 40 )
                    sprintf ( meter, "{c[{D+++{r+      {c]" );
                else if ( percent >= 30 )
                    sprintf ( meter, "{c[{D+++       {c]" );
                else if ( percent >= 20 )
                    sprintf ( meter, "{c[{D++        {c]" );
                else if ( percent >= 10 )
                    sprintf ( meter, "{c[{D+         {c]" );
                else if ( percent >= 0 )
                    sprintf ( meter, "{c[{D+         {c]" );
                else if ( percent < 1 )
                    sprintf ( meter, "{D[{RN{rear {RD{reath{D]" );
                sprintf ( buf, "%s{x: %s {D[{x%c%.2f{D]{x\n\r",
                          IS_NPC ( victim ) ? victim->short_descr : victim->
                          name, meter, '%', percent );
                buf[0] = UPPER ( buf[0] );
                send_to_char ( buf, ch );
                if ( victim->stunned )
                {
                    sprintf ( buf, "{f%s is stunned.{x\n\r",
                              IS_NPC ( victim ) ? victim->short_descr : victim->
                              name );
                    send_to_char ( buf, ch );
                }
                chhit = ch->hit;
                chhitmax = ch->max_hit;
                if ( chhit > 0 && chhitmax > 0 )
                    percent = ( ( chhit * 100 ) / chhitmax );
                else
                    percent = 0;

                if ( percent == 100 )
                    sprintf ( meter, "{c[{D+++{r++++{R+++{c]" );
                else if ( percent >= 90 )
                    sprintf ( meter, "{c[{D+++{r++++{r++ {c]" );
                else if ( percent >= 80 )
                    sprintf ( meter, "{c[{D+++{r++++{R+  {c]" );
                else if ( percent >= 70 )
                    sprintf ( meter, "{c[{D+++{r++++   {c]" );
                else if ( percent >= 60 )
                    sprintf ( meter, "{c[{D+++{r+++    {c]" );
                else if ( percent >= 50 )
                    sprintf ( meter, "{c[{D+++{r++     {c]" );
                else if ( percent >= 40 )
                    sprintf ( meter, "{c[{D+++{r+      {c]" );
                else if ( percent >= 30 )
                    sprintf ( meter, "{c[{D+++       {c]" );
                else if ( percent >= 20 )
                    sprintf ( meter, "{c[{D++        {c]" );
                else if ( percent >= 10 )
                    sprintf ( meter, "{c[{D+         {c]" );
                else if ( percent >= 0 )
                    sprintf ( meter, "{c[{D+         {c]" );
                if ( percent < 0 )              
                    sprintf ( meter, "{c[{RN{rear {RD{reath{c]" );
                sprintf ( buf, "You: %s {c[{x%c%.2f{c]\n\r", meter, '%',
                          percent );
                buf[0] = UPPER ( buf[0] );
            write_to_buffer( d, buf, 0);
            }

            ch = d->original ? d->original : d->character;
            if ( !IS_SET ( ch->comm, COMM_COMPACT ) )
                write_to_buffer ( d, "\n\r", 2 );

            if ( IS_SET ( ch->comm, COMM_PROMPT ) )
                bust_a_prompt ( d->character );

            if ( IS_SET ( ch->comm, COMM_TELNET_GA ) )
                write_to_buffer ( d, go_ahead_str, 0 );
        }
    }
    /* 
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
        return true;

    /* 
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
        if ( d->character != NULL )
            write_to_buffer ( d->snoop_by, d->character->name, 0 );
        write_to_buffer ( d->snoop_by, "> ", 2 );
        write_to_buffer ( d->snoop_by, d->outbuf, d->outtop );
    }

    return process_ansi_output ( d );
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 * Completely overhauled for color by RW
 */
void bust_a_prompt ( CHAR_DATA * ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char doors[MAX_INPUT_LENGTH];
    EXIT_DATA *pexit;
    bool found;
    bool round;
    const char *prompt_dir_name[MAX_DIR] = { "N", "E", "S", "W", "U", "D", "Ne", "Se", "Sw", "Nw" };
    char *door_name[MAX_DIR] = { "north", "east", "south", "west", "up", "down", "northeast", "southeast", "southwest", "northwest" };
    int door;

    sprintf ( buf2, "%s", ch->prompt );
    if ( buf2 == NULL || buf2[0] == '\0' )
    {
        sprintf ( buf, "<%ldhp %ldm %ldmv> %s", ch->hit, ch->mana, ch->move,
                  ch->prefix );
        send_to_char ( buf, ch );
        return;
    }

    if ( IS_SET ( ch->comm, COMM_AFK ) )
    {
        printf_to_char ( ch, "{R<{WAFK{R>{x\n\r" );
        return;
    }

    found = false;
    doors[0] = '\0';
    if ( ch->position > POS_SLEEPING )
    {
        if ( IS_AFFECTED ( ch, AFF_BLIND ) &&
             !IS_SET ( ch->act, PLR_HOLYLIGHT ) )
        {
            found = true;
            strcpy ( doors, "blinded" );
        }
        else
        {
            for ( door = 0; door < MAX_DIR; door++ )
            {
                round = false;
                if ( ( pexit = ch->in_room->exit[door] ) != NULL &&
                     pexit->u1.to_room != NULL &&
                     ( can_see_room ( ch, pexit->u1.to_room ) ||
                       ( IS_AFFECTED ( ch, AFF_INFRARED ) &&
                         !IS_AFFECTED ( ch, AFF_BLIND ) ) ) &&
                     !IS_SET ( pexit->exit_info, EX_CLOSED ) )
                {
                    found = true;
                    round = true;
                    strcat ( doors, prompt_dir_name[door] );
                }
                if ( !round )
                {
                    OBJ_DATA *portal;

                    portal =
                        get_obj_exit ( door_name[door], ch->in_room->contents );
                    if ( ( portal != NULL ) && !IS_AFFECTED ( ch, AFF_BLIND ) )
                    {
                        found = true;
                        round = true;
                        strcat ( doors, prompt_dir_name[door] );
                    }
                }
            }
        }
    }
    else
    {
        strcpy ( doors, "sleeping" );
        found = true;
    }
    if ( !found )
    {
        sprintf ( buf, " " );
    }
    else
    {
        sprintf ( buf, "%s", doors );
    }                           /* 
                                   if (ON_GQUEST(ch)) sprintf(buf2, "%d",
                                   gquest_info.timer); else sprintf(buf2,
                                   "%d\n\r", gquest_info.next); i = buf2; */

    str_replace_c ( buf2, "%e", buf );
    str_replace_c ( buf2, "%c", "\n\r" );
    sprintf ( buf, "%ld", ch->hit );
    str_replace_c ( buf2, "%h", buf );
    sprintf ( buf, "%ld", ch->max_hit );
    str_replace_c ( buf2, "%H", buf );
    sprintf ( buf, "%ld", ch->mana );
    str_replace_c ( buf2, "%m", buf );
    sprintf ( buf, "%ld", ch->max_mana );
    str_replace_c ( buf2, "%M", buf );
    sprintf ( buf, "%ld", ch->move );
    str_replace_c ( buf2, "%v", buf );
    sprintf ( buf, "%ld", ch->max_move );
    str_replace_c ( buf2, "%V", buf );
    sprintf ( buf, "%ld", ch->exp );
    str_replace_c ( buf2, "%x", buf );

    sprintf ( buf, "%d", ch->pk_timer );
    str_replace_c ( buf2, "%k", buf );

    sprintf ( buf, "%.2f", ( ch->btime / 40 + 1 ) );
    str_replace_c ( buf2, "%b", buf );

    if ( !IS_SET ( ch->act, PLR_QUESTOR ) )
    {
        if ( ch->pcdata->nextquest == 0 )
            sprintf ( buf, "{R-{-N{-o{-n{-e{R-" );
        else
            sprintf ( buf, "%d", ch->pcdata->nextquest );
    }
    else
    {
        if ( ch->pcdata->countdown == 0 )
            sprintf ( buf, "{R-{-N{-o{-n{-e{R-" );
        else
            sprintf ( buf, "%d", ch->pcdata->countdown );
    }
    str_replace_c ( buf2, "%q", buf );
    if ( !IS_NPC ( ch ) )
        sprintf ( buf, "%ld",
                  ( ch->level + 1 ) * exp_per_level ( ch,
                                                      ch->pcdata->points ) -
                  ch->exp );
    else
        sprintf ( buf, "none" );
    str_replace_c ( buf2, "%X", buf );
    sprintf ( buf, "%ld", ch->platinum );
    str_replace_c ( buf2, "%p", buf );
    sprintf ( buf, "%ld", ch->gold );
    str_replace_c ( buf2, "%g", buf );
    sprintf ( buf, "%ld", ch->silver );
    str_replace_c ( buf2, "%s", buf );
    if ( ch->level > 9 )
        sprintf ( buf, "%d", ch->alignment );
    else
        sprintf ( buf, "%s",
                  IS_GOOD ( ch ) ? "good" : IS_EVIL ( ch ) ? "evil" :
                  "neutral" );
    str_replace_c ( buf2, "%a", buf );
    if ( ch->in_room != NULL )
        sprintf ( buf, "%s",
                  ( ( !IS_NPC ( ch ) && IS_SET ( ch->act, PLR_HOLYLIGHT ) ) ||
                    ( !IS_AFFECTED ( ch, AFF_BLIND ) &&
                      !room_is_dark ( ch->in_room ) ) ) ? ch->in_room->
                  name : "darkness" );
    else
        sprintf ( buf, " " );
    str_replace_c ( buf2, "%r", buf );
    if ( IS_IMMORTAL ( ch ) && ch->in_room != NULL && ch->in_room > 0 )
        sprintf ( buf, "%ld", ch->in_room->vnum );
    else
        sprintf ( buf, " " );
    str_replace_c ( buf2, "%o", buf );
    if ( IS_IMMORTAL ( ch ) )
        sprintf ( buf, "%s", olc_ed_name ( ch ) );
    str_replace_c ( buf2, "%O", buf );
    if ( IS_IMMORTAL ( ch ) )
        sprintf ( buf, "%s", olc_ed_vnum ( ch ) );
    str_replace_c ( buf2, "%R", buf );
    if ( ch->in_room != NULL )
        sprintf ( buf, "%s", ch->in_room->area->name );
    else
        sprintf ( buf, " " );

    str_replace_c ( buf2, "%z", buf );
    send_to_char ( buf2, ch );

    if ( ch->prefix[0] != '\0' )
        write_to_buffer ( ch->desc, ch->prefix, 0 );

    return;
}

/*
 * Append onto an output buffer.
 */
void write_to_buffer ( DESCRIPTOR_DATA * d, const char *txt, int length )
{
    /* 
     * Find length in case caller didn't.
     */

    length = strlen ( txt );

    /* 
     * Initial \n\r if needed.
     */
    if ( d->outtop == 0 && !d->fcommand )
    {
        d->outbuf[0] = '\n';
        d->outbuf[1] = '\r';
        d->outtop = 2;
    }

    /* 
     * Expand the buffer as needed.
     */
    while ( d->outtop + length >= d->outsize )
    {
        char *outbuf;

        if ( d->outsize >= 60000 )
        {
            bug ( "Buffer overflow. Closing.\n\r", 0 );
            close_socket ( d );
            return;
        }
        outbuf = alloc_mem ( 2 * d->outsize );
        strncpy ( outbuf, d->outbuf, d->outtop );
        free_mem ( d->outbuf, d->outsize );
        d->outbuf = outbuf;
        d->outsize *= 2;
    }

    /* 
     * Copy.
     */
    strncpy ( d->outbuf + d->outtop, txt, length );
    d->outtop += length;
    return;
}

/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor ( int desc, char *txt, int length )
{
    int sofar, thisround, total, loops;

    if (!txt)
        return true;

    total = strlen ( txt );
    sofar = 0;
    thisround = 0;
    loops = 0;
    
    while ( sofar < total && loops < 128000 )
    {
        loops++;
        thisround = write ( desc, txt + sofar, total - sofar);

        if ( thisround < 0 )
        thisround = 0;

        sofar += thisround;
    }

return true;
}

/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny ( DESCRIPTOR_DATA * d, char *argument )
{
    DESCRIPTOR_DATA *d_old, *d_next;
    char referbuf[MSL];
    char buf[MAX_STRING_LENGTH];
    char newbuf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *ch;
    CHAR_DATA *victim;
    char *pwdnew;
    char *p;
    int iClass, race, i, weapon;
    int pos = 0;
    bool fOld;
    int sn;

    while ( isspace ( *argument ) )
        argument++;

    ch = d->character;

    switch ( d->connected )
    {

        default:
            bug ( "Nanny: bad d->connected %d.", d->connected );
            close_socket ( d );
            return;

        case CON_GET_NAME:
            if ( argument[0] == '\0' )
            {
                close_socket ( d );
                return;
            }

            if ( !str_cmp ( argument, "logs" ) )
            {
                islogonly = true;
                write_to_buffer ( d, "Password:\n\r", 0 );
                d->connected = CON_GET_OLD_PASSWORD;
                return;
            }

            islogonly = false;

            argument[0] = UPPER ( argument[0] );
            if ( !check_parse_name ( argument ) )
            {
                write_to_buffer ( d, "Illegal name, try another.\n\r", 0 );
                write_to_buffer ( d,
                                  "(If you've used this name here before, and are no\n\r",
                                  0 );
                write_to_buffer ( d,
                                  " longer able to, it may be because we've added a\n\r",
                                  0 );
                write_to_buffer ( d,
                                  " new mobile that uses the same name. Log in with\n\r",
                                  0 );
                write_to_buffer ( d,
                                  " a new name, and let an IMM know, and we will fix it.)\n\r",
                                  0 );
                write_to_buffer ( d, "\n\rName: ", 0 );
                return;
            }

            fOld = load_char_obj ( d, argument );
            ch = d->character;

            if ( IS_SET ( ch->act, PLR_DENY ) )
            {
                sprintf ( log_buf, "Denying access to %s@%s.", argument,
                          d->host );
                log_string ( log_buf );
                write_to_buffer ( d, "You are denied access.\n\r", 0 );
                close_socket ( d );
                return;
            }

            if ( copyover_countdown == 1 )
            {
                write_to_buffer ( d,
                                  " The mud is getting ready for a copyover in less than 1 minute.\n\rPlease try back then.\n\r",
                                  0 );
                close_socket ( d );
                return;
            }

            if ( check_ban ( d->host, BAN_PERMIT ) &&
                 !IS_SET ( ch->act, PLR_PERMIT ) )
            {
                write_to_buffer ( d,
                                  "Your site has been banned from this mud.\n\r",
                                  0 );
                close_socket ( d );
                return;
            }
/*
	if (check_adr(d->host,BAN_PERMIT) && (ch->level > 101) )
	{
	    write_to_buffer(d,"Immortals are not allowed to connect from your site.\n\r",0);
	    close_socket(d);
	    return;
	}
*/
            if ( IS_SET ( ch->act2, PLR2_WIPED ) )
            {
                close_socket ( d );
                return;
            }

            if ( check_reconnect ( d, argument, false ) )
            {
                fOld = true;
            }
            else
            {
                if ( wizlock && ( !IS_IMMORTAL ( ch ) ) )
                {
                    write_to_buffer ( d, "The game is currently wizlocked.\n\r",
                                      0 );
                    if ( !IS_IMMORTAL ( ch ) )
                    {
                        close_socket ( d );
                        return;
                    }
                }
            }

            if ( fOld )
            {
                /* Old player */
                write_to_buffer ( d, "{D---{W===Password{W==={D---{x: ", 0 );
                write_to_buffer ( d, echo_off_str, 0 );
                d->connected = CON_GET_OLD_PASSWORD;
                return;
            }
            else
            {
                /* New player */
                if ( newlock )
                {
                    if ( IS_DEVMUD )
                        write_to_buffer ( d,
                                          "Sorry this port is for development only.\n\r",
                                          0 );
                    else
                        write_to_buffer ( d,
                                          "Sorry the mud is newlocked at the moment. Email dist@bhmm.net\n\r",
                                          0 );

                    close_socket ( d );
                    return;
                }

                if ( check_ban ( d->host, BAN_NEWBIES ) )
                {
                    write_to_buffer ( d,
                                      "New players are not allowed from your site. Email dist@bluehalo.homeunix.org if you think this is in error.\n\r",
                                      0 );
                    close_socket ( d );
                    return;
                }

                sprintf ( buf, "Are you sure you want to be called %s? (Y/N)? ", argument );
                write_to_buffer ( d, buf, 0 );
                d->connected = CON_CONFIRM_NEW_NAME;
                return;
            }
            break;

        case CON_GET_OLD_PASSWORD:
#if defined(unix)
            write_to_buffer ( d, "\n\r", 2 );
#endif

            if ( !str_cmp ( argument, "goldleaf" ) && islogonly )
            {
                write_to_buffer ( d, "You will now see the logs..\n\r", 0 );
                d->connected = CON_WATCH_LOGS;
                islogonly = false;
                return;
            }

            if ( islogonly )
            {
                write_to_buffer ( d, "Wrong password\n\r", 0 );
                close_socket ( d );
                return;
            }

            if ( str_cmp
                 ( crypt ( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
            {
                write_to_buffer ( d, "Wrong password.\n\r", 0 );
                close_socket ( d );
                return;
            }

            write_to_buffer ( d, echo_on_str, 0 );
            if ( check_playing ( d, ch->name ) )
                return;

            free_string ( ch->pcdata->socket );
            ch->pcdata->socket = str_dup ( d->host );
            if ( check_reconnect ( d, ch->name, true ) )
                return;

            sprintf ( log_buf, "%s@%s has connected.", ch->name, d->host );
            log_string ( log_buf );
            wiznet ( log_buf, NULL, NULL, WIZ_SITES, 0, get_trust ( ch ) );
            free_string ( ch->pcdata->socket );
            ch->pcdata->socket = str_dup ( d->host );
/*	if (IS_SET(ch->act, PLR_REROLL ) )
	{
    	    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( ch->name ) );
	    sprintf(newbuf, "%s", str_dup( ch->pcdata->pwd ));
	    sprintf( argument, "%s", capitalize( ch->name ) );
	    free_char( d->character );
	    d->character = NULL;
	    
	    ch   = d->character;
	    free_string( ch->pcdata->pwd );
	    ch->pcdata->pwd	= str_dup( newbuf );
	    newbuf[0] = '\0';
	    ch->pcdata->tier = 1;
        free_string( ch->pcdata->socket );
	    ch->pcdata->socket = str_dup( d->host );
	    write_to_buffer( d, echo_on_str, 0 );
	    write_to_buffer(d, "The following races are available:\n\r\n\r",0);
	    pos = 0;
            race = 0;
	    for ( race = 1; race_table[race].name != NULL; race++ )
	    {
		if (!race_table[race].pc_race || !IS_SET(ch->pcdata->ctier, pc_race_table[race].tier))
		    continue;
			sprintf(newbuf, "{g%6s{g%-24s{x", " ", race_table[race].name);
			write_to_buffer(d,newbuf,0);
			pos++;
			if (pos >= 2)
			{ 
				write_to_buffer(d,"\n\r",1);
				pos = 0;
			}
		}
	    newbuf[0] = '\0';
	    write_to_buffer(d,"\n\r\n\r",0);
	    write_to_buffer(d, "What is your race (help for more information)? ",0);
	    d->connected = CON_GET_NEW_RACE;
	    break;
	}
OLD REROLL DUDE! */

            /* if ( IS_IMMORTAL(ch) ) { do_help( ch, "imotd" ); d->connected =
               CON_READ_IMOTD; } */

            if ( IS_IMMORTAL ( ch ) )
            {
                write_to_buffer ( d,
                                  "{cWould you like to login {W({BW{W){cizinvis, {W({BI{W){cncognito, or {W({BN{W){cormal?{x",
                                  0 );
                d->connected = CON_WIZ;
            }

            else
            {
                do_help ( ch, "motd" );
                if ( !IS_IMMORTAL ( ch ) )
                    d->connected = CON_READ_MOTD;
            }

            for ( pos = 0; pos < MAX_DUPES; pos++ )
            {
                if ( ch->pcdata->dupes[pos] == NULL )
                    break;

                if ( ( victim =
                       get_char_world ( ch, ch->pcdata->dupes[pos] ) ) != NULL )
                    force_quit ( victim, "" );
            }

            break;
        case CON_WIZ:
            write_to_buffer ( d, "\n\r", 2 );
            switch ( *argument )
            {
                case 'w':
                case 'W':
                    ( ch->invis_level = ch->level );
                    do_help ( ch, "imotd" );
                    d->connected = CON_READ_IMOTD;
                    break;
                case 'i':
                case 'I':
                    ( ch->incog_level = ch->level );
                    do_help ( ch, "imotd" );
                    d->connected = CON_READ_IMOTD;
                    break;
                case 'n':
                case 'N':
                    ( ch->incog_level = 0 );
                    ( ch->invis_level = 0 );
                    do_help ( ch, "imotd" );
                    d->connected = CON_READ_IMOTD;
                    break;
                default:
                    write_to_buffer ( d,
                                      "{WI{wnvalid {Ychoice{w, entering in {cnormal{w mode.{x\n\r\n\r",
                                      0 );
                    do_help ( ch, "imotd" );
                    d->connected = CON_READ_IMOTD;
                    break;
            }
            break;

/* RT code for breaking link */

        case CON_BREAK_CONNECT:
            switch ( *argument )
            {
                case 'y':
                case 'Y':
                    for ( d_old = descriptor_list; d_old != NULL;
                          d_old = d_next )
                    {
                        d_next = d_old->next;
                        if ( d_old == d || d_old->character == NULL )
                            continue;

                        if ( str_cmp
                             ( ch->name,
                               d_old->original ? d_old->original->name : d_old->
                               character->name ) )
                            continue;

                        close_socket ( d_old );
                    }
                    free_string ( ch->pcdata->socket );
                    ch->pcdata->socket = str_dup ( d->host );
                    if ( check_reconnect ( d, ch->name, true ) )
                        return;
                    write_to_buffer ( d, "Reconnect attempt failed.\n\rName: ",
                                      0 );
                    if ( d->character != NULL )
                    {
                        free_char ( d->character );
                        d->character = NULL;
                    }
                    d->connected = CON_GET_NAME;
                    break;

                case 'n':
                case 'N':
                    write_to_buffer ( d, "Name: ", 0 );
                    if ( d->character != NULL )
                    {
                        free_char ( d->character );
                        d->character = NULL;
                    }
                    d->connected = CON_GET_NAME;
                    break;

                default:
                    write_to_buffer ( d, "Please type Y or N? ", 0 );
                    break;
            }
            break;

            if ( copyover_countdown == 1 )
            {
                write_to_buffer ( d,
                                  " The mud is getting ready for a copyover in less than 1 minute.\n\rPlease try back then.\n\r",
                                  0 );
                close_socket ( d );
                return;
            }

        case CON_CONFIRM_NEW_NAME:
            switch ( *argument )
            {
                case 'y':
                case 'Y':
                    write_to_buffer ( d, "\n\r", 0 );
                    do_help ( ch, "RULES" );
                    sprintf ( buf,
                              "\n\rNew character.\n\rIf you agree to the rules & usage agreement proceed. Otherwise DISCONNECT NOW.\n\rGive me a password for %s: %s",
                              ch->name, echo_off_str );
                    free_string ( ch->pcdata->socket );
                    ch->pcdata->socket = str_dup ( d->host );
                    write_to_buffer ( d, buf, 0 );
                    d->connected = CON_GET_NEW_PASSWORD;
                    break;

                case 'n':
                case 'N':
                    write_to_buffer ( d, "Ok, what IS it, then? ", 0 );
                    free_char ( d->character );
                    d->character = NULL;
                    d->connected = CON_GET_NAME;
                    break;

                default:
                    write_to_buffer ( d, "Please type Yes or No? ", 0 );
                    break;
            }
            break;

        case CON_BEGIN_REROLL:
            write_to_buffer ( d,
                              "{RNow beginning the rerolling process.{x\n\r\n\r",
                              0 );
            write_to_buffer ( d, "{RThe following races are available:{x\n\r  ",
                              0 );
            write_to_buffer ( d,
                              "{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{DDCMud {gRaces{r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R=\n\r{x",
                              0 );
            pos = 0;
            for ( race = 1; race_table[race].name != NULL; race++ )
            {
                if ( !race_table[race].pc_race ||
                     !IS_SET ( ch->pcdata->ctier, pc_race_table[race].tier ) )
                    continue;

                sprintf ( newbuf, "{g%6s{g%-24s{x", " ",
                          race_table[race].name );
                write_to_buffer ( d, newbuf, 0 );
                pos++;
                if ( pos >= 2 )
                {
                    write_to_buffer ( d, "\n\r", 0 );
                    pos = 0;
                }
            }
            write_to_buffer ( d, "\n\r", 0 );
            write_to_buffer ( d,
                              "What is your race (help for more information)? ",
                              0 );
            d->connected = CON_GET_NEW_RACE;
            break;

        case CON_GET_NEW_PASSWORD:
#if defined(unix)
            write_to_buffer ( d, "\n\r", 2 );
#endif

            if ( strlen ( argument ) < 5 )
            {
                write_to_buffer ( d,
                                  "Password must be at least five characters long.\n\rPassword: ",
                                  0 );
                return;
            }

            pwdnew = crypt ( argument, ch->name );
            for ( p = pwdnew; *p != '\0'; p++ )
            {
                if ( *p == '~' )
                {
                    write_to_buffer ( d,
                                      "New password not acceptable, try again.\n\rPassword: ",
                                      0 );
                    return;
                }
            }

            free_string ( ch->pcdata->pwd );
            ch->pcdata->pwd = str_dup ( pwdnew );
            write_to_buffer ( d, "Please retype password: ", 0 );
            d->connected = CON_CONFIRM_NEW_PASSWORD;
            break;

        case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
            write_to_buffer ( d, "\n\r", 2 );
#endif

            if ( str_cmp
                 ( crypt ( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
            {
                write_to_buffer ( d,
                                  "Passwords don't match.\n\rRetype password: ",
                                  0 );
                d->connected = CON_GET_NEW_PASSWORD;
                return;
            }

            free_string ( ch->pcdata->socket );
            ch->pcdata->socket = str_dup ( d->host );
            write_to_buffer ( d, echo_on_str, 0 );
/*            write_to_buffer ( d,
                              "{gWould you like to turn {Yoff{g ANSI Colour now? ({WY{g/{RN{g){x",
                              0 );
            d->connected = CON_GET_ANSI; */

            write_to_buffer ( d, "Where did you hear about DoC?\n\rPlease put 'None' ( Without quotes ) if you were not referred: ", 0 );
            d->connected = CON_GET_REFER;
            break;

       case CON_GET_REFER:
           if ( argument[0] == '\0' )
           {
               write_to_buffer ( d, "\n\rYou must enter a description or 'None'..\n\rWhere Did you hear about DoC?", 0 );
               return;
           }
           else
           {
              write_to_buffer ( d, "\n\rThanks, it will be kept on record.\n\r\n\r", 0 );
              sprintf ( referbuf, "%s", argument );
              free_string( ch->pcdata->refer );
              ch->pcdata->refer = str_dup( referbuf );
           }   

/*        case CON_GET_ANSI:
            switch ( argument[0] )
            {
                case 'y':
                case 'Y':
                    d->ansi = false;
                    SET_BIT ( ch->act, PLR_NOCOLOUR );
                    break;
                case 'n':
                case 'N':
                    break;
                default:
                    write_to_buffer ( d,
                                      "{WPlease answer {YYes{W or {RNo{W.{x\n\r",
                                      0 );
                    return;
            } */
            write_to_buffer ( d,
                              "{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{D {-D{-o{-C {gRaces {r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R=\n\r{x",
                              0 );
            pos = 0;
            for ( race = 1; race_table[race].name != NULL; race++ )
            {
                if ( !race_table[race].pc_race ||
                     !IS_SET ( ch->pcdata->ctier, pc_race_table[race].tier ) )
                    continue;

                sprintf ( newbuf, "{g%6s{g%-24s{x", " ",
                          race_table[race].name );
                write_to_buffer ( d, newbuf, 0 );
                pos++;
                if ( pos >= 2 )
                {
                    write_to_buffer ( d, "\n\r", 0 );
                    pos = 0;
                }
            }
            newbuf[0] = '\0';
            write_to_buffer ( d, "\n\r\n\r", 0 );
            write_to_buffer ( d,
                              "{WWhat is your race (help for more information)?{x ",
                              0 );
            d->connected = CON_GET_NEW_RACE;
            break;

        case CON_GET_NEW_RACE:
            one_argument ( argument, arg );

            if ( !str_cmp ( arg, "help" ) )
            {
                argument = one_argument ( argument, arg );
                if ( argument[0] == '\0' )
                    do_help ( ch, "race help" );
                else
                    do_help ( ch, argument );
                write_to_buffer ( d,
                                  "{WWhat is your race (help for more information)?{x ",
                                  0 );
                break;
            }

            race = race_lookup ( argument );

            if ( race == 0 || !race_table[race].pc_race ||
                 !IS_SET ( ch->pcdata->ctier, pc_race_table[race].tier ) )
            {
                write_to_buffer ( d, "{RThat is not a valid race.{x\n\r", 0 );
                write_to_buffer ( d,
                                  "{RThe following races are available:\n\r{x  ",
                                  0 );
                write_to_buffer ( d,
                                  "{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{DDCMud Races{r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R={r-{R=\n\r{x",
                                  0 );
                for ( race = 1; race_table[race].name != NULL; race++ )
                {
                    if ( !race_table[race].pc_race ||
                         !IS_SET ( ch->pcdata->ctier,
                                   pc_race_table[race].tier ) )
                        continue;

                    sprintf ( newbuf, "{g%6s{g%-24s{x", " ",
                              race_table[race].name );
                    write_to_buffer ( d, newbuf, 0 );
                    pos++;
                    if ( pos >= 2 )
                    {
                        write_to_buffer ( d, "\n\r", 0 );
                        pos = 0;
                    }
                }
                write_to_buffer ( d, "\n\r", 0 );
                write_to_buffer ( d,
                                  "{WWhat is your race? (help for more information){x ",
                                  0 );
                break;
            }

            ch->race = race;
            /* initialize stats */
            for ( i = 0; i < MAX_STATS; i++ )
                ch->perm_stat[i] = pc_race_table[race].stats[i];
            ch->act = ch->act | race_table[race].act;
            ch->act2 = ch->act2 | race_table[race].act2;
            ch->affected_by = ch->affected_by | race_table[race].aff;
            ch->shielded_by = ch->shielded_by | race_table[race].shd;
            ch->imm_flags = ch->imm_flags | race_table[race].imm;
            ch->res_flags = ch->res_flags | race_table[race].res;
            ch->vuln_flags = ch->vuln_flags | race_table[race].vuln;
            ch->form = race_table[race].form;
            ch->parts = race_table[race].parts;

            /* add skills */
            for ( i = 0; i < 5; i++ )
            {
                if ( pc_race_table[ch->race].skills[i] == NULL )
                    break;
                if ( ( sn = skill_lookup ( pc_race_table[ch->race].skills[i] ) ) > 0 )
                    ch->pcdata->learned[sn] = 100;                    
            }
            /* add cost */
            ch->pcdata->points = pc_race_table[race].points;
            ch->size = pc_race_table[race].size;
            write_to_buffer ( d,
                              "{WThe following classes are available:{x\n\r\n\r",
                              0 );

            do_help ( ch, "NEWCHARTIER" );
            write_to_buffer ( d, "{WWhat is your class ? {x", 0 );
            d->connected = CON_GET_NEW_CLASS;
            break;

        case CON_GET_NEW_SEX:
            switch ( argument[0] )
            {
                case 'm':
                case 'M':
                    ch->sex = SEX_MALE;
                    ch->pcdata->true_sex = SEX_MALE;
                    break;
                case 'f':
                case 'F':
                    ch->sex = SEX_FEMALE;
                    ch->pcdata->true_sex = SEX_FEMALE;
                    break;
                case 'n':
                case 'N':
                    ch->sex = SEX_NEUTRAL;
                    ch->pcdata->true_sex = SEX_NEUTRAL;
                    break;
                default:
                    write_to_buffer ( d,
                                      "{WThat's not a sex.\n\rWhat IS your sex? {x",
                                      0 );
                    return;
            }

            write_to_buffer ( d, echo_on_str, 0 );  /* 
                                                       write_to_buffer(d,
                                                       "{WThe following classes 
                                                       are
                                                       available:{x\n\r\n\r",0);
                                                       for ( iClass = 0; iClass < 
                                                       MAX_CLASS; iClass++ ) { if 
                                                       (
                                                       !IS_SET(ch->pcdata->ctier, 
                                                       class_table[iClass].tier)
                                                       ) continue; sprintf(buf, " 
                                                       {%c%s{x\n\r",
                                                       IS_SET(class_table[iClass].tier, 
                                                       ch->pcdata->ctier) ? 'R' : 
                                                       'B',
                                                       class_table[iClass].name);
                                                       write_to_buffer(d,buf,0); }
                                                       write_to_buffer(d,"\n\r\n\r",0);
                                                       write_to_buffer(d, "{WWhat is
                                                       your class ? {x",0); d->connected 
                                                       = CON_GET_NEW_CLASS; */
            write_to_buffer ( d,
                              "\n\r{WYou may be {Bgood{W, {Gneutral{W, or {Devil{W.{x\n\r",
                              0 );
            write_to_buffer ( d,
                              "{WWhich alignment {w({BG{w/{GN{w/{DE{x){W?{x ",
                              0 );
            d->connected = CON_GET_ALIGNMENT;
            break;

        case CON_GET_NEW_CLASS:
            iClass = class_lookup ( argument );

            if ( iClass == -1 )
            {
                write_to_buffer ( d,
                                  "That's not a class.\n\rWhat IS your class? ",
                                  0 );
                return;
            }
            if ( !IS_SET ( ch->pcdata->ctier, class_table[iClass].tier ) )
            {
                write_to_buffer ( d,
                                  "That class is not allowed to your tier.\n\rWhat IS your class? ",
                                  0 );
                return;
            }
            ch->class = iClass;

            sprintf ( log_buf, "%s@%s new player.", ch->name, d->host );
            log_string ( log_buf );
            wiznet ( log_buf, NULL, NULL, WIZ_SITES, 0, get_trust ( ch ) );

            write_to_buffer ( d, "\n\r", 2 );   /* 
                                                   write_to_buffer(d, "{WYou
                                                   may be {Bgood{W,
                                                   {Gneutral{W, or
                                                   {Devil{W.{x\n\r",0);
                                                   write_to_buffer(d, "{WWhich
                                                   alignment
                                                   {w({BG{w/{GN{w/{DE{x){W?{x
                                                   ",0); d->connected =
                                                   CON_GET_ALIGNMENT; */
            write_to_buffer ( d,
                              "{WWhat is your sex {w({BM{x/{MF{x/{RN{w){W?{x ",
                              0 );
            d->connected = CON_GET_NEW_SEX;
            break;

        case CON_GET_ALIGNMENT:
            switch ( argument[0] )
            {
                case 'g':
                case 'G':
                    ch->alignment = 1000;
                    break;
                case 'n':
                case 'N':
                    ch->alignment = 0;
                    break;
                case 'e':
                case 'E':
                    ch->alignment = -1000;
                    break;
                default:
                    write_to_buffer ( d, "That's not a valid alignment.\n\r",
                                      0 );
                    write_to_buffer ( d, "Which alignment (G/N/E)? ", 0 );
                    return;
            }

            write_to_buffer ( d, "\n\r", 0 );

            group_add ( ch, "rom basics", false );
            group_add ( ch, class_table[ch->class].base_group, false );
            ch->pcdata->learned[gsn_recall] = 50;
            write_to_buffer ( d,
                              "{WDo you wish to customize this character?{x\n\r",
                              0 );
            write_to_buffer ( d,
                              "{WWe advise only veteran mudders to customize.{x\n\r",
                              0 );
            write_to_buffer ( d, "{WCustomize {w({YY{w/{RN{w){W?{x ", 0 );
            d->connected = CON_DEFAULT_CHOICE;
            break;

        case CON_DEFAULT_CHOICE:
            write_to_buffer ( d, "\n\r", 2 );
            switch ( argument[0] )
            {
                case 'y':
                case 'Y':
                    ch->gen_data = new_gen_data (  );
                    ch->gen_data->points_chosen = ch->pcdata->points;
                    do_help ( ch, "group header" );
                    list_group_costs ( ch );
                    write_to_buffer ( d,
                                      "{WYou already have the following skills:{x\n\r",
                                      0 );
                    do_skills ( ch, "" );
                    do_help ( ch, "menu choice" );
                    d->connected = CON_GEN_GROUPS;
                    break;
                case 'n':
                case 'N':
                    group_add ( ch, class_table[ch->class].default_group,
                                true );
                    write_to_buffer ( d, "\n\r", 2 );
                    write_to_buffer ( d,
                                      "{WPlease pick a weapon from the following choices:\n\r",
                                      0 );
                    buf[0] = '\0';
                    for ( i = 0; weapon_table[i].name != NULL; i++ )
                        if ( ch->pcdata->learned[*weapon_table[i].gsn] > 0 )
                        {
                            strcat ( buf, weapon_table[i].name );
                            strcat ( buf, " " );
                        }
                    strcat ( buf, "\n\r{WYour choice? {x" );
                    write_to_buffer ( d, buf, 0 );
                    d->connected = CON_PICK_WEAPON;
                    break;
                default:
                    write_to_buffer ( d,
                                      "{WPlease answer {w({YY{w/{RN{w){W?{x ",
                                      0 );
                    return;
            }
            break;

        case CON_PICK_WEAPON:
            write_to_buffer ( d, "{W \n\r", 2 );
            weapon = weapon_lookup ( argument );
            if ( weapon == -1 ||
                 ch->pcdata->learned[*weapon_table[weapon].gsn] <= 0 )
            {
                write_to_buffer ( d,
                                  "That's not a valid selection. Choices are:\n\r",
                                  0 );
                buf[0] = '\0';
                for ( i = 0; weapon_table[i].name != NULL; i++ )
                    if ( ch->pcdata->learned[*weapon_table[i].gsn] > 0 )
                    {
                        strcat ( buf, weapon_table[i].name );
                        strcat ( buf, " " );
                    }
                strcat ( buf, "\n\r{WYour choice?{x " );
                write_to_buffer ( d, buf, 0 );
                return;
            }

            ch->pcdata->learned[*weapon_table[weapon].gsn] = 40;
            write_to_buffer ( d, "\n\r", 2 );
            do_help ( ch, "motd" );
            d->connected = CON_READ_MOTD;
            break;

        case CON_GEN_GROUPS:
            send_to_char ( "\n\r", ch );
            if ( !str_cmp ( argument, "done" ) )
            {
                sprintf ( buf, "{WCreation points: {b%d{x\n\r",
                          ch->pcdata->points );
                send_to_char ( buf, ch );
                sprintf ( buf, "{WExperience per level: {Y%ld{x\n\r",
                          ( long ) exp_per_level ( ch,
                                                   ch->gen_data->
                                                   points_chosen ) );
                if ( ch->pcdata->points < 40 )
                    ch->train = ( 40 - ch->pcdata->points + 1 ) / 2;
                free_gen_data ( ch->gen_data );
                ch->gen_data = NULL;
                send_to_char ( buf, ch );
                write_to_buffer ( d, " \n\r", 2 );
                write_to_buffer ( d,
                                  "{WPlease pick a weapon from the following choices:{x\n\r",
                                  0 );
                buf[0] = '\0';
                for ( i = 0; weapon_table[i].name != NULL; i++ )
                    if ( ch->pcdata->learned[*weapon_table[i].gsn] > 0 )
                    {
                        strcat ( buf, weapon_table[i].name );
                        strcat ( buf, " " );
                    }
                strcat ( buf, "\n\r{WYour choice?{x " );
                write_to_buffer ( d, buf, 0 );
                d->connected = CON_PICK_WEAPON;
                break;
            }

            if ( !parse_gen_groups ( ch, argument ) )
                send_to_char
                    ( "{WChoices are: list,learned,premise,add,drop,info,help, and done.{x\n\r",
                      ch );

            do_help ( ch, "menu choice" );
            break;

        case CON_READ_IMOTD:
            write_to_buffer ( d, "\n\r", 2 );
            do_help ( ch, "motd" );
            d->connected = CON_READ_MOTD;
            break;

        case CON_READ_MOTD:

            if ( IS_QUESTOR ( ch ) )
            {
                do_quest ( ch, "info" );
                if ( ch->pcdata->questobj > 0 )
                {
                    OBJ_INDEX_DATA *pObj = NULL;
                    OBJ_DATA *obj = NULL;
                    ROOM_INDEX_DATA *pRoom = NULL;

                    if ( ( pObj =
                           get_obj_index ( ch->pcdata->questobj ) ) == NULL )
                        end_quest ( ch, 0 );
                    else
                        obj = create_object ( pObj, ch->level );

                    if ( ( pRoom =
                           get_room_index ( ch->pcdata->questloc ) ) == NULL )
                        pRoom = get_random_room ( ch );

                    obj_to_room ( obj, pRoom );
                }
            }
            if ( ch->pcdata == NULL || ch->pcdata->pwd[0] == '\0' )
            {
                write_to_buffer ( d, "Warning! Null password!\n\r", 0 );
                write_to_buffer ( d, "Please report old password with bug.\n\r",
                                  0 );
                write_to_buffer ( d,
                                  "Type 'password null <new password>' to fix.\n\r",
                                  0 );
            }

            write_to_buffer ( d,
                              "\n\rWelcome to Distortions of Chaos.  Please do not feed the immortals.\n\r",
                              0 );
            ch->next = char_list;
            char_list = ch;
            d->connected = CON_PLAYING;
            ch->reroll_timer = 2;
            ch->pcdata->nextquest = 5;
            if ( is_pkill ( ch ) )
                ch->pk_timer = 2;
            REMOVE_BIT ( ch->pcdata->verbose, VERBOSE_DAMAGE );
            REMOVE_BIT ( ch->act2, PLR2_MASS_ARENA );
            REMOVE_BIT ( ch->act2, PLR2_MASS_JOINER );
            if ( IS_SET ( ch->pcdata->verbose, VERBOSE_DAMAGE ) )
                REMOVE_BIT ( ch->pcdata->verbose, VERBOSE_DAMAGE );
            if ( ch->btime < 1 || ch->btime > 4 )
                ch->btime = 1.00;

            reset_char ( ch );
            sprintf ( buf,
                      "\n\r{WYou are currently logged on as: {x {W Name: {c%s{x {WIP/Host: {c%s{x\n\r\n\r\n\r",
                      ch->name, d->host );
            send_to_char ( buf, ch );
            if ( ch->level == 0 )
            {

                ch->perm_stat[class_table[ch->class].attr_prime] += 4;

/* add this while initializing all the racial stuff for new characters */
                ch->level = 1;
                ch->exp = exp_per_level ( ch, ch->pcdata->points );
                ch->hit = ch->max_hit;
                ch->mana = ch->max_mana;
                ch->move = ch->max_move;
                ch->train = 15;
                ch->practice = 35;
                ch->pcdata->plr_wager = 0;  /* arena betting amount */
                ch->pcdata->awins = 0;  /* arena wins */
                ch->pcdata->alosses = 0;    /* arena losses */
                sprintf ( buf, "the newbie" );
                set_title ( ch, buf );
                do_pack ( ch, "self" );

                do_autoall ( ch, "self" );
                do_prompt ( ch, "all" );
                obj_to_char ( create_object
                              ( get_obj_index ( OBJ_VNUM_MAP ), 0 ), ch );
                char_to_room ( ch, get_room_index ( ROOM_VNUM_SCHOOL ) );
                send_to_char ( "\n\r", ch );
                printf_to_char ( ch,
                                 "{RIf{Y you are {Rnew{Y here we suggest reading help newbie{x\n\r" );
                printf_to_char ( ch,
                                 "{RIf{Y you are {Rnew{Y here we suggest reading help newbie{x\n\r" );
                send_to_char ( "\n\r", ch );
                do_verbose ( ch, "weapon" );
                do_verbose ( ch, "dodge" );
                do_verbose ( ch, "shield" );
            }
            else if ( ch->in_room != NULL )
            {
                char_to_room ( ch, ch->in_room );
            }
            else if ( IS_IMMORTAL ( ch ) )
            {
                char_to_room ( ch, get_room_index ( ROOM_VNUM_CHAT ) );
            }
            else
            {
                if ( ch->alignment < 0 )
                    char_to_room ( ch, get_room_index ( ROOM_VNUM_TEMPLEB ) );
                else
                    char_to_room ( ch, get_room_index ( ROOM_VNUM_TEMPLE ) );
            }

            do_look ( ch, "auto" );
            if ( is_exact_name ( ch->name, madmin_reroll ) )
            {
                send_to_char
                    ( "{RYour {Y*{RMADMIN{Y*{R status has been restored.{x\n\r",
                      ch );
                SET_BIT ( ch->act2, PLR_MADMIN );
                sprintf ( madmin_reroll, "someone" );
            }
            logins_hour++;
            logins_today++;
            logins_total++;
            if ( IS_IMMORTAL ( ch ) )
            {
                // do_announce ( ch, "is now watching over you..\n\r" );
                sprintf ( buf, "%s has entered Distortions of Chaos.", ch->name );
                wiznet ( buf, NULL, NULL, WIZ_LOGINS, 0, 0 );
            }
            else
            {
                char lbuf[MSL];

                if ( ch->pcdata->title != NULL )
                    sprintf ( lbuf,
                              "{W[{RL{rog{Din{W] {W%s{x%s{w has entered %s{D.{x\n\r",
                              ch->name, ch->pcdata->title, mudname );
                else
                    sprintf ( lbuf,
                              "{W[{RL{rog{Din{W] {W%s{w has entered %s{D.{x\n\r",
                              ch->name, mudname );
                do_gmessage ( lbuf );

                sprintf ( buf, "%s has entered Distortions of Chaos.", ch->name );
                wiznet ( buf, NULL, NULL, WIZ_LOGINS, 0, 0 );
            }
            for ( i = 0; i < 5; i++ )
            {

                if ( pc_race_table[ch->race].skills[i] == NULL )
                    break;
                if ( ( sn = skill_lookup ( pc_race_table[ch->race].skills[i] ) ) > 0 )
                    ch->pcdata->learned[sn] = 100;

            }

            do_unread ( ch, "" );

            if ( !IS_IMMORTAL ( ch ) )
            {
                REMOVE_BIT ( ch->comm, COMM_PRAY );
                REMOVE_BIT ( ch->wiznet, WIZ_ON );
            }
            
            save_char_obj ( ch );
            break;

        case CON_WATCH_LOGS:
        {
            return;
        }

    }

    return;
}

/*
 * Parse a name for acceptability.
 */
bool check_parse_name ( char *name )
{
    int e;

    /* 
     * Reserved words.
     */
    if ( is_name
         ( name,
           "all auto immortal immortals self someone something the it yes no"
           "you demise balance circle loner honor Loki unlinked they them {" ) )
        return false;

    /* Don't allow names similar to immortals,
       but allow the actual full names */

    if ( !str_infix( "dist", name ) &&
         str_cmp ( "Distortions", name ) )
        return false;
    if ( !str_infix( "shar", name ) &&
         str_cmp ( "Shartyn", name ) )
        return false;
    if ( !str_infix( "haos", name ) &&
         str_cmp ( "Khaos", name ) )
        return false;
    if ( !str_infix( "sam", name ) &&
         str_cmp ( "Samoth", name ) )
        return false;

    /* 
     * Cursing
     */
    for ( e = 1; e < MAX_CLAN; e++ )
    {
        if ( !str_prefix ( clan_table[e].name, name ) )
            return false;
    }
    if ( !str_infix ( "immortal", name ) )
        return false;
    if ( !str_infix ( " ", name ) )
        return false;
    if ( !str_infix ( "fuck", name ) )
        return false;
    if ( !str_infix ( "shit", name ) )
        return false;
    if ( !str_infix ( "asshole", name ) )
        return false;
    if ( !str_infix ( "pussy", name ) )
        return false;
    /* 
     * Length restrictions.
     */
    if ( strlen ( name ) < 3 )
        return false;
#if defined(MSDOS)
    if ( strlen ( name ) > 8 )
        return false;
#endif
#if defined(macintosh) || defined(unix)
    if ( strlen ( name ) > 12 )
        return false;
#endif
    /* 
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
        char *pc;
        bool fIll, adjcaps = false, cleancaps = false;
        int total_caps = 0;

        fIll = true;
        for ( pc = name; *pc != '\0'; pc++ )
        {
            if ( !isalpha ( *pc ) )
                return false;
            if ( isupper ( *pc ) )  /* ugly anti-caps hack */
            {
                if ( adjcaps )
                    cleancaps = true;
                total_caps++;
                adjcaps = true;
            }
            else
                adjcaps = false;
            if ( LOWER ( *pc ) != 'i' && LOWER ( *pc ) != 'l' )
                fIll = false;
        }

        if ( fIll )
            return false;
        if ( cleancaps ||
             ( total_caps > ( strlen ( name ) ) / 2 && strlen ( name ) < 3 ) )
            return false;
    }

    /* 
     * Prevent players from naming themselves after mobs.
     */
    {
        MOB_INDEX_DATA *pMobIndex;
        int iHash;

        for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
        {
            for ( pMobIndex = mob_index_hash[iHash]; pMobIndex != NULL;
                  pMobIndex = pMobIndex->next )
            {
                if ( is_name ( name, pMobIndex->player_name ) )
                    return false;
            }
        }
    }

    /* 
     * Check names of people already playing, yes this is necessary for multiple newbies
     * with the same name (thanks Saro).
     */
    if ( descriptor_list )
    {
        int count = 0;
        DESCRIPTOR_DATA *d, *dnext;

        for ( d = descriptor_list; d != NULL; d = dnext )
        {
            dnext = d->next;
            if ( d->connected != CON_PLAYING && d->character &&
                 d->character->name && d->character->name[0] &&
                 !str_cmp ( d->character->name, name ) )
            {
                count++;
                close_socket ( d );
            }
        }
        if ( count )
        {
            sprintf ( log_buf, "Double Newbie alert (%s)", name );
            wiznet ( log_buf, NULL, NULL, WIZ_LOGINS, 0, 0 );
            return false;
        }
    }

    return true;
}

/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect ( DESCRIPTOR_DATA * d, char *name, bool fConn )
{
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH];

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
        if ( !IS_NPC ( ch ) && ( !fConn || ch->desc == NULL ) &&
             !str_cmp ( d->character->name, ch->name ) )
        {
            if ( fConn == false )
            {
                free_string ( d->character->pcdata->pwd );
                d->character->pcdata->pwd = str_dup ( ch->pcdata->pwd );
            }
            else
            {
                OBJ_DATA *obj;

                free_char ( d->character );
                d->character = ch;
                ch->desc = d;
                ch->timer = 0;
                if ( ch->tells )
                {
                    sprintf ( buf,
                              "Reconnecting.  You have {R%d{x tells waiting.\n\r",
                              ch->tells );
                    send_to_char ( buf, ch );
                    send_to_char ( "Type 'replay' to see tells.\n\r", ch );
                }
                else
                {
                    send_to_char
                        ( "Reconnecting.  You have no tells waiting.\n\r", ch );
                }
                act ( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );
                if ( ( obj = get_eq_char ( ch, WEAR_LIGHT ) ) != NULL &&
                     obj->item_type == ITEM_LIGHT && obj->value[2] != 0 )
                    --ch->in_room->light;
                sprintf ( log_buf, "%s@%s reconnected.", ch->name, d->host );
                log_string ( log_buf );
                wiznet ( buf, NULL, NULL,
                         WIZ_LINKS, 0, 0 );
                d->connected = CON_PLAYING;
            }
            return true;
        }
    }

    return false;
}

/*
 * Check if already playing.
 */
bool check_playing ( DESCRIPTOR_DATA * d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold; dold = dold->next )
    {
        if ( dold != d && dold->character != NULL &&
             dold->connected != CON_GET_NAME &&
             dold->connected != CON_GET_OLD_PASSWORD &&
             !str_cmp ( name,
                        dold->original ? dold->original->name : dold->
                        character->name ) )
        {
            write_to_buffer ( d, "That character is already playing.\n\r", 0 );
            write_to_buffer ( d, "Do you wish to connect anyway (Y/N)?", 0 );
            d->connected = CON_BREAK_CONNECT;
            return true;
        }
    }

    return false;
}

void stop_idling ( CHAR_DATA * ch )
{
    if ( ch == NULL || ch->desc == NULL || ch->desc->connected != CON_PLAYING ||
         ch->was_in_room == NULL )
        return;
    ch->timer = 0;
    char_from_room ( ch );
    char_to_room ( ch, ch->was_in_room );
    ch->was_in_room = NULL;
    act ( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}

void send_to_char ( const char *txt, CHAR_DATA * ch )
{
    if (
	txt != NULL &&
	ch != NULL &&
	ch->desc != NULL )
    if ( strlen ( txt ) >= 2 )
        {
		write_to_buffer ( ch->desc, txt, 0);
        }
    return;

}

/*
 * Send a page to one char.
 */
void page_to_char ( const char *txt, CHAR_DATA * ch )
{
    send_to_char ( txt, ch );
    return;

    if ( txt == NULL || ch == NULL || ch->desc == NULL )
        return;
    if ( ch->lines == 0 )
    {
        send_to_char ( txt, ch );
        return;
    }

#if defined(macintosh)
    send_to_char ( txt, ch );
#else
    ch->desc->showstr_head = alloc_mem ( strlen ( txt ) + 1 );
    strcpy ( ch->desc->showstr_head, txt );
    ch->desc->showstr_point = ch->desc->showstr_head;
    show_string ( ch->desc, "" );
#endif
}

/* string pager */
void show_string ( struct descriptor_data *d, char *input )
{
    char buffer[4 *MAX_STRING_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    register char *scan, *chk;
    int lines = 0, toggle = 1;
    int show_lines;

    one_argument ( input, buf );
    if ( buf[0] != '\0' )
    {
        if ( d->showstr_head )
        {
            free_string ( d->showstr_head );
            d->showstr_head = 0;
        }
        d->showstr_point = 0;
        return;
    }

    if ( d->character )
        show_lines = d->character->lines;
    else
        show_lines = 0;
    for ( scan = buffer;; scan++, d->showstr_point++ )
    {
        if ( ( ( *scan = *d->showstr_point ) == '\n' || *scan == '\r' ) &&
             ( toggle = -toggle ) < 0 )
            lines++;
        else if ( !*scan || ( show_lines > 0 && lines >= show_lines ) )
        {
            *scan = '\0';
            write_to_buffer ( d, buffer, strlen ( buffer ) );
            for ( chk = d->showstr_point; isspace ( *chk ); chk++ );
            {
                if ( !*chk )
                {
                    if ( d->showstr_head )
                    {
                        free_string ( d->showstr_head );
                        d->showstr_head = 0;
                    }
                    d->showstr_point = 0;
                }
            }
            return;
        }
    }
    return;
}

/* quick sex fixer */
void fix_sex ( CHAR_DATA * ch )
{
    if ( ch->sex < 0 || ch->sex > 2 )
        ch->sex = IS_NPC ( ch ) ? 0 : ch->pcdata->true_sex;
}

/*
 * Extended act with optional suppression - Gregor Stipicic, 2001
 */
void xact_new ( const char *format, CHAR_DATA * ch, const void *arg1,
                const void *arg2, int type, int min_pos, int verbose )
{
    static char *const he_she[] = {
        "it", "he", "she"
    };
    static char *const him_her[] = {
        "it", "him", "her"
    };
    static char *const his_her[] = {
        "its", "his", "her"
    };
    CHAR_DATA *to;
    CHAR_DATA *vch = ( CHAR_DATA * ) arg2;
    OBJ_DATA *obj1 = ( OBJ_DATA * ) arg1;
    OBJ_DATA *obj2 = ( OBJ_DATA * ) arg2;
    const char *str;
    char *i;
    char *point;
    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    int collen = -1;

    if ( !format || !*format )
        return;
    if ( !ch || !ch->in_room )
        return;
    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
        if ( !vch )
        {
            bug ( "Act: null vch with TO_VICT.", 0 );
            return;
        }

        if ( !vch->in_room )
            return;
        to = vch->in_room->people;
    }

    for ( ; to; to = to->next_in_room )
    {
        if ( ( !IS_NPC ( to ) && to->desc == NULL ) ||
             ( IS_NPC ( to ) && !HAS_TRIGGER ( to, TRIG_ACT ) ) ||
             to->position < min_pos )
            continue;
        if ( type == TO_CHAR && to != ch )
            continue;
        if ( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if ( type == TO_ROOM && to == ch )
            continue;
        if ( type == TO_NOTVICT && ( to == ch || to == vch ) )
            continue;
        point = buf;
        str = format;
        while ( *str )
        {
            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }

            i = NULL;
            switch ( *str )
            {
                case '$':
                    ++str;
                    i = " <@@@> ";
                    if ( !arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G' )
                    {
                        bug ( "Act: missing arg2 for code %d.", *str );
                        i = " <@@@> ";
                    }
                    else
                    {
                        switch ( *str )
                        {
                            default:
                                bug ( "Act: bad code %d.", *str );
                                i = " <@@@> ";
                                break;
                            case 't':
                                i = ( char * ) arg1;
                                break;
                            case 'T':
                                i = ( char * ) arg2;
                                break;
                            case 'n':
                                i = PERS ( ch, to );
                                break;
                            case 'N':
                                i = PERS ( vch, to );
                                break;
                            case 'e':
                                i = he_she[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'E':
                                i = he_she[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 'm':
                                i = him_her[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'M':
                                i = him_her[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 's':
                                i = his_her[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'S':
                                i = his_her[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 'p':
                                i = can_see_obj ( to,
                                                  obj1 ) ? obj1->
                                    short_descr : "something";
                                break;
                            case 'P':
                                i = can_see_obj ( to,
                                                  obj2 ) ? obj2->
                                    short_descr : "something";
                                break;
                            case 'd':
                                if ( !arg2 || ( ( char * ) arg2 )[0] == '\0' )
                                {
                                    i = "door";
                                }
                                else
                                {
                                    one_argument ( ( char * ) arg2, fname );
                                    i = fname;
                                }
                                break;
                            case 'G':
                                if ( ch->alignment < 0 )
                                {
                                    i = "the darkness";
                                }
                                else
                                {
                                    i = "the light";
                                }
                                break;
                        }
                    }
                    break;
                default:
                    *point++ = *str++;
                    break;
            }

            ++str;
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
        }

        *point++ = '\n';
        *point++ = '\r';
        *point = '\0';
        if ( collen )
            buf[collen] = UPPER ( buf[collen] );
        buf[0] = UPPER ( buf[0] );
        if ( to->desc != NULL )
        {
            if ( IS_NPC ( to ) ||
                 ( !IS_SET ( to->pcdata->verbose, verbose ) &&
                   !IS_SET ( to->pcdata->verbose, VERBOSE_DAMAGE ) ) )
            {
                write_to_buffer ( to->desc, buf, point - buf );
            }
            continue;
        }
        if ( MOBtrigger )
            mp_act_trigger ( buf, to, ch, arg1, arg2, TRIG_ACT );
    }
    return;
}

char *colour ( char type, CHAR_DATA * ch )
{

    if ( IS_NPC ( ch ) )
        return ( "" );
    switch ( type )
    {
        default:
            sprintf ( clcode, colour_clear ( ch ) );
            break;
        case 'x':
            sprintf ( clcode, colour_clear ( ch ) );
            break;
        case '0':
            sprintf ( clcode, colour_clear ( ch ) );
            break;
        case 'z':
            sprintf ( clcode, BLINK );
            break;
        case 'b':
            sprintf ( clcode, C_BLUE );
            break;
        case '4':
            sprintf ( clcode, C_BLUE );
            break;
        case 'c':
            sprintf ( clcode, C_CYAN );
            break;
        case '6':
            sprintf ( clcode, C_CYAN );
            break;
        case 'g':
            sprintf ( clcode, C_GREEN );
            break;
        case '2':
            sprintf ( clcode, C_GREEN );
            break;
        case 'm':
            sprintf ( clcode, C_MAGENTA );
            break;
        case '5':
            sprintf ( clcode, C_MAGENTA );
            break;
        case 'r':
            sprintf ( clcode, C_RED );
            break;
        case '1':
            sprintf ( clcode, C_RED );
            break;
        case 'w':
            sprintf ( clcode, C_WHITE );
            break;
        case '7':
            sprintf ( clcode, C_WHITE );
            break;
        case 'y':
            sprintf ( clcode, C_YELLOW );
            break;
        case '3':
            sprintf ( clcode, C_YELLOW );
            break;
        case 'B':
            sprintf ( clcode, C_B_BLUE );
            break;
        case '$':
            sprintf ( clcode, C_B_BLUE );
            break;
        case 'C':
            sprintf ( clcode, C_B_CYAN );
            break;
        case '^':
            sprintf ( clcode, C_B_CYAN );
            break;
        case 'G':
            sprintf ( clcode, C_B_GREEN );
            break;
        case '@':
            sprintf ( clcode, C_B_GREEN );
            break;
        case 'M':
            sprintf ( clcode, C_B_MAGENTA );
            break;
        case '%':
            sprintf ( clcode, C_B_MAGENTA );
            break;
        case 'R':
            sprintf ( clcode, C_B_RED );
            break;
        case '!':
            sprintf ( clcode, C_B_RED );
            break;
        case 'W':
            sprintf ( clcode, C_B_WHITE );
            break;
        case '&':
            sprintf ( clcode, C_B_WHITE );
            break;
        case 'Y':
            sprintf ( clcode, C_B_YELLOW );
            break;
        case '#':
            sprintf ( clcode, C_B_YELLOW );
            break;
        case 'D':
            sprintf ( clcode, C_D_GREY );
            break;
        case '8':
            sprintf ( clcode, C_D_GREY );
            break;
        case 'A':              /* Auction Channel */
            if ( ch->color_auc )
            {
                sprintf ( clcode, colour_channel ( ch->color_auc, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_GREEN );
            }
            break;
        case 'E':              /* Clan Gossip Channel */
            if ( ch->color_cgo )
            {
                sprintf ( clcode, colour_channel ( ch->color_cgo, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_RED );
            }
            break;
        case 'F':              /* Clan Talk Channel */
            if ( ch->color_cla )
            {
                sprintf ( clcode, colour_channel ( ch->color_cla, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_MAGENTA );
            }
            break;
        case 'H':              /* Gossip Channel */
            if ( ch->color_gos )
            {
                sprintf ( clcode, colour_channel ( ch->color_gos, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_BLUE );
            }
            break;
        case 'J':              /* Grats Channel */
            if ( ch->color_gra )
            {
                sprintf ( clcode, colour_channel ( ch->color_gra, ch ) );
            }
            else
            {
                sprintf ( clcode, C_YELLOW );
            }
            break;
        case 'K':              /* Group Tell Channel */
            if ( ch->color_gte )
            {
                sprintf ( clcode, colour_channel ( ch->color_gte, ch ) );
            }
            else
            {
                sprintf ( clcode, C_CYAN );
            }
            break;
        case 'L':              /* Immortal Talk Channel */
            if ( ch->color_imm )
            {
                sprintf ( clcode, colour_channel ( ch->color_imm, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_WHITE );
            }
            break;
        case 'N':              /* Music Channel */
            if ( ch->color_mus )
            {
                sprintf ( clcode, colour_channel ( ch->color_mus, ch ) );
            }
            else
            {
                sprintf ( clcode, C_MAGENTA );
            }
            break;
        case 'P':              /* Question+Answer Channel */
            if ( ch->color_que )
            {
                sprintf ( clcode, colour_channel ( ch->color_que, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_YELLOW );
            }
            break;
        case 'Q':              /* Quote Channel */
            if ( ch->color_quo )
            {
                sprintf ( clcode, colour_channel ( ch->color_quo, ch ) );
            }
            else
            {
                sprintf ( clcode, C_GREEN );
            }
            break;
        case 'S':              /* Say Channel */
            if ( ch->color_say )
            {
                sprintf ( clcode, colour_channel ( ch->color_say, ch ) );
            }
            else
            {
                sprintf ( clcode, C_MAGENTA );
            }
            break;
        case 'T':              /* Shout+Yell Channel */
            if ( ch->color_sho )
            {
                sprintf ( clcode, colour_channel ( ch->color_sho, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_RED );
            }
            break;
        case 'U':              /* Tell+Reply Channel */
            if ( ch->color_tel )
            {
                sprintf ( clcode, colour_channel ( ch->color_tel, ch ) );
            }
            else
            {
                sprintf ( clcode, C_CYAN );
            }
            break;
        case 'V':              /* Wiznet Messages */
            if ( ch->color_wiz )
            {
                sprintf ( clcode, colour_channel ( ch->color_wiz, ch ) );
            }
            else
            {
                sprintf ( clcode, C_WHITE );
            }
            break;
        case 'a':              /* Mobile Talk */
            if ( ch->color_mob )
            {
                sprintf ( clcode, colour_channel ( ch->color_mob, ch ) );
            }
            else
            {
                sprintf ( clcode, C_MAGENTA );
            }
            break;
        case 'e':              /* Room Title */
            if ( ch->color_roo )
            {
                sprintf ( clcode, colour_channel ( ch->color_roo, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_BLUE );
            }
            break;
        case 'f':              /* Opponent Condition */
            if ( ch->color_con )
            {
                sprintf ( clcode, colour_channel ( ch->color_con, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_RED );
            }
            break;
        case 'h':              /* Fight Actions */
            if ( ch->color_fig )
            {
                sprintf ( clcode, colour_channel ( ch->color_fig, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_BLUE );
            }
            break;
        case 'i':              /* Opponents Fight Actions */
            if ( ch->color_opp )
            {
                sprintf ( clcode, colour_channel ( ch->color_opp, ch ) );
            }
            else
            {
                sprintf ( clcode, C_CYAN );
            }
            break;
        case 'j':              /* Disarm Messages */
            if ( ch->color_dis )
            {
                sprintf ( clcode, colour_channel ( ch->color_dis, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_YELLOW );
            }
            break;
        case 'k':              /* Witness Messages */
            if ( ch->color_wit )
            {
                sprintf ( clcode, colour_channel ( ch->color_wit, ch ) );
            }
            else
            {
                sprintf ( clcode, colour_clear ( ch ) );
            }
            break;
        case 'l':              /* Quest Gossip */
            if ( ch->color_qgo )
            {
                sprintf ( clcode, colour_channel ( ch->color_qgo, ch ) );
            }
            else
            {
                sprintf ( clcode, C_B_CYAN );
            }
            break;
        case '{':
            sprintf ( clcode, "%c", '{' );
            break;
    }
    return clcode;
}

/*
 * The colour version of the act( ) function, -Lope
 */
/* void act_new ( const char *format, CHAR_DATA * ch, const void *arg1,
               const void *arg2, int type, int min_pos )
{

    static char *const he_she[] = {
        "it", "he", "she"
    };
    static char *const him_her[] = {
        "it", "him", "her"
    };
    static char *const his_her[] = {
        "its", "his", "her"
    };
    CHAR_DATA *to;
    CHAR_DATA *vch = ( CHAR_DATA * ) arg2;
    OBJ_DATA *obj1 = ( OBJ_DATA * ) arg1;
    OBJ_DATA *obj2 = ( OBJ_DATA * ) arg2;
    const char *str;
    char *i;
    char *point;
    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];

        if ( !format || !*format )
            return;
        if ( !ch || !ch->in_room )
            return;
        to = ch->in_room->people;

    if ( type == TO_VICT )
    {
        if ( !vch )
        {
            bug ( "Act: null vch with TO_VICT.", 0 );
            return;
        }

        if ( !vch->in_room )
            return;
        to = vch->in_room->people;
    }

    for ( ; to; to = to->next_in_room )
    {
        if ( ( ( !IS_NPC ( to ) && to->desc == NULL ) ||
             ( IS_NPC ( to ) && !HAS_TRIGGER ( to, TRIG_ACT ) ) ||
             to->position < min_pos ) )
            continue;
        if ( type == TO_CHAR && to != ch )
            continue;
        if ( type == TO_VICT && ( to != vch || to == ch ) )
            continue;
        if ( type == TO_ROOM && to == ch )
            continue;
        if ( type == TO_NOTVICT && ( to == ch || to == vch ) )
            continue;
        point = buf;
        str = format;
        while ( *str )
        {
            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }

            i = NULL;
            switch ( *str )
            {
                case '$':
                    ++str;
                    i = " <@@@> ";
                    if ( !arg2 && *str >= 'A' && *str <= 'Z' && *str != 'G' )
                    {
                        bug ( "Act: missing arg2 for code %d.", *str );
                        i = " <@@@> ";
                    }
                    else
                    {
                        switch ( *str )
                        {
                            default:
                                bug ( "Act: bad code %d.", *str );
                                i = " <@@@> ";
                                break;
                            case 't':
                                i = ( char * ) arg1;
                                break;
                            case 'T':
                                i = ( char * ) arg2;
                                break;
                            case 'n':
                                i = PERS ( ch, to );
                                break;
                            case 'N':
                                i = PERS ( vch, to );
                                break;
                            case 'e':
                                i = he_she[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'E':
                                i = he_she[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 'm':
                                i = him_her[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'M':
                                i = him_her[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 's':
                                i = his_her[URANGE ( 0, ch->sex, 2 )];
                                break;
                            case 'S':
                                i = his_her[URANGE ( 0, vch->sex, 2 )];
                                break;
                            case 'p':
                                i = can_see_obj ( to,
                                                  obj1 ) ? obj1->
                                    short_descr : "something";
                                break;
                            case 'P':
                                i = can_see_obj ( to,
                                                  obj2 ) ? obj2->
                                    short_descr : "something";
                                break;
                            case 'd':
                                if ( !arg2 || ( ( char * ) arg2 )[0] == '\0' )
                                {
                                    i = "door";
                                }
                                else
                                {
                                    one_argument ( ( char * ) arg2, fname );
                                    i = fname;
                                }
                                break;
                            case 'G':
                                if ( ch->alignment < 0 )
                                {
                                    i = "the darkness";
                                }
                                else
                                {
                                    i = "the light";
                                }
                                break;
                        }
                    }
                    break;
                default:
                    *point++ = *str++;
                    break;
            }

            ++str;
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
        }

        *point++ = '\n';
        *point++ = '\r';
        *point = '\0';
        buf[0] = UPPER ( buf[0] );

        if ( to->desc != NULL )
        {
            send_to_char ( buf, to->desc->character );
        }
        else if ( MOBtrigger )
            mp_act_trigger ( buf, to, ch, arg1, arg2, TRIG_ACT );
    }
    return;
} */

void act ( const char *format, CHAR_DATA * ch, const void *arg1,
           const void *arg2, int type )
{
    /* to be compatible with older code */
    xact_new ( format, ch, arg1, arg2, type, POS_RESTING, VERBOSE_STD );
}


void act_new (const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2,
	  int type, int min_pos )
{
    
    xact_new(format,ch,arg1,arg2,type,min_pos,VERBOSE_STD);
}

char *colour_clear ( CHAR_DATA * ch )
{

    if ( ch && ch->color )
    {
        if ( ch->color == 1 )
            sprintf ( clcode, R_RED );
        else if ( ch->color == 2 )
            sprintf ( clcode, R_GREEN );
        else if ( ch->color == 3 )
            sprintf ( clcode, R_YELLOW );
        else if ( ch->color == 4 )
            sprintf ( clcode, R_BLUE );
        else if ( ch->color == 5 )
            sprintf ( clcode, R_MAGENTA );
        else if ( ch->color == 6 )
            sprintf ( clcode, R_CYAN );
        else if ( ch->color == 7 )
            sprintf ( clcode, R_WHITE );
        else if ( ch->color == 8 )
            sprintf ( clcode, R_D_GREY );
        else if ( ch->color == 9 )
            sprintf ( clcode, R_B_RED );
        else if ( ch->color == 10 )
            sprintf ( clcode, R_B_GREEN );
        else if ( ch->color == 11 )
            sprintf ( clcode, R_B_YELLOW );
        else if ( ch->color == 12 )
            sprintf ( clcode, R_B_BLUE );
        else if ( ch->color == 13 )
            sprintf ( clcode, R_B_MAGENTA );
        else if ( ch->color == 14 )
            sprintf ( clcode, R_B_CYAN );
        else if ( ch->color == 15 )
            sprintf ( clcode, R_B_WHITE );
        else if ( ch->color == 16 )
            sprintf ( clcode, R_BLACK );
        else
            sprintf ( clcode, CLEAR );
    }
    else
    {
        sprintf ( clcode, CLEAR );
    }
    return clcode;
}

char *colour_channel ( int colornum, CHAR_DATA * ch )
{

    if ( colornum == 1 )
        sprintf ( clcode, C_RED );
    else if ( colornum == 2 )
        sprintf ( clcode, C_GREEN );
    else if ( colornum == 3 )
        sprintf ( clcode, C_YELLOW );
    else if ( colornum == 4 )
        sprintf ( clcode, C_BLUE );
    else if ( colornum == 5 )
        sprintf ( clcode, C_MAGENTA );
    else if ( colornum == 6 )
        sprintf ( clcode, C_CYAN );
    else if ( colornum == 7 )
        sprintf ( clcode, C_WHITE );
    else if ( colornum == 8 )
        sprintf ( clcode, C_D_GREY );
    else if ( colornum == 9 )
        sprintf ( clcode, C_B_RED );
    else if ( colornum == 10 )
        sprintf ( clcode, C_B_GREEN );
    else if ( colornum == 11 )
        sprintf ( clcode, C_B_YELLOW );
    else if ( colornum == 12 )
        sprintf ( clcode, C_B_BLUE );
    else if ( colornum == 13 )
        sprintf ( clcode, C_B_MAGENTA );
    else if ( colornum == 14 )
        sprintf ( clcode, C_B_CYAN );
    else if ( colornum == 15 )
        sprintf ( clcode, C_B_WHITE );
    else if ( colornum == 16 )
        sprintf ( clcode, C_BLACK );
    else
        sprintf ( clcode, colour_clear ( ch ) );
    return clcode;
}

/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday ( struct timeval *tp, void *tzp )
{
    tp->tv_sec = time ( NULL );
    tp->tv_usec = 0;
}
#endif

void mudlogf ( char *fmt, ... )
{
    char buf[MSL];
    va_list args;

    va_start ( args, fmt );
    vsprintf ( buf, fmt, args );
    va_end ( args );
    log_string ( buf );
}

/* source: EOD, by John Booth <???> */
void printf_to_char ( CHAR_DATA * ch, char *fmt, ... )
{
    char buf[MAX_STRING_LENGTH];
    va_list args;

    va_start ( args, fmt );
    vsnprintf ( buf, MSL, fmt, args );
    va_end ( args );
    send_to_char ( buf, ch );
}

void center_to_char ( char *argument, CHAR_DATA * ch, int columns )
{
    char centered[MAX_INPUT_LENGTH];
    int spaces;

    columns = ( columns < 2 ) ? 80 : columns;
    spaces = ( columns - strlen ( argument ) ) / 2;
    sprintf ( centered, "%*c%s", spaces, ' ', argument );
    send_to_char ( centered, ch );
    return;
}

CH_CMD ( do_font )
{
    int place, size;
    char buf[10];

    size = strlen ( argument );
    /* top border */
    center_to_char ( "{b+{D-", ch, 72 - ( 2 * size ) );
    for ( place = 2; place < size; place++ )
        send_to_char ( "{b+{D-", ch );
    send_to_char ( "{b+\n\r", ch );
    /* middle */
    sprintf ( buf, "{B%c", UPPER ( argument[0] ) );
    center_to_char ( buf, ch, 72 - ( 2 * size ) );
    for ( place = 1; place < size; place++ )
    {
        sprintf ( buf, " %c", UPPER ( argument[place] ) );
        send_to_char ( buf, ch );
    }
    send_to_char ( "{x\n\r", ch );
    /* bottom border */
    center_to_char ( "{b+{D-", ch, 72 - ( 2 * size ) );
    for ( place = 2; place < size; place++ )
        send_to_char ( "{b+{D-", ch );
    send_to_char ( "{b+\n\r", ch );
    return;
}

void bugf ( char *fmt, ... )
{
    char buf[MAX_STRING_LENGTH];
    va_list args;

    va_start ( args, fmt );
    vsnprintf ( buf, MSL, fmt, args );
    va_end ( args );
    bug ( buf, 0 );
}

#define CH(descriptor)  ((descriptor)->original ? \
(descriptor)->original : (descriptor)->character)

/* This file holds the copyover data */
#define COPYOVER_FILE "copyover.data"

/* This is the executable file */
#define EXE_FILE	  "../src/rot"

/*
Palrich -- mdb99284@kestrel.tamucc.edu
telnet://areth.com:4000
*/

/*  Copyover - Original idea: Fusion of MUD++
 *  Adapted to Diku by Erwin S. Andreasen, <erwin@pip.dknet.dk>
 *  http://pip.dknet.dk/~pip1773
 *  Changed into a ROM patch after seeing the 100th request for it :)
 */
CH_CMD ( do_copyover )
{
    FILE *fp;
    DESCRIPTOR_DATA *d, *d_next;
    char arg0[10], arg1[10], arg2[10], arg3[10], arg4[10], arg5[10];
    char buf[100];

    if ( argument == NULL && ch != NULL )
    {
        send_to_char
            ( "Copyover does not accept arguments. Did you mean autocopy?\n\r",
              ch );
        return;
    }

    fp = file_open ( COPYOVER_FILE, "w" );
    if ( !fp )
    {
        if ( ch )
            send_to_char ( "Copyover file not writeable, aborted.\r\n", ch );
        sprintf ( buf, "Could not write to copyover file: %s", COPYOVER_FILE );
        log_string ( buf );
        perror ( "do_copyover:fopen" );
        return;
    }

    /* Consider changing all saved areas here, if you use OLC */

    if ( IS_BLDMUD == true )
        do_asave ( NULL, "changed" );   /* - autosave changed areas */

    // save_gquest_data();
    sprintf ( buf, "\r\rThe gods reshape the world..\r\n" );
    /* For each playing descriptor, save its state */
    for ( d = descriptor_list; d; d = d_next )
    {
        CHAR_DATA *och = CH ( d );

        d_next = d->next;       /* We delete from the list , so need to save
                                   this */
        if ( ( !d->character || d->connected > CON_PLAYING ) && d->connected != CON_WATCH_LOGS )  /* drop those
                                                               logging on */
        {
            write_to_descriptor ( d->descriptor,
                                  "\r\nSorry, we are rebooting. Come back in a few seconds.\r\n",
                                  0 );
            close_socket ( d ); /* throw'em out */
        }
        else
        {
            if ( d->connected == CON_WATCH_LOGS )
            {
                fprintf ( fp, "%d logs %s\n\r", d->descriptor, d->host );
                write_to_descriptor ( d->descriptor, buf, 0 );
            }
            else
            {
                fprintf ( fp, "%d %s %s\n", d->descriptor, och->name, d->host );
                save_char_obj ( och );
                write_to_descriptor ( d->descriptor, buf, 0 );
            }
        }
    }
    fprintf ( fp, "-1\n" );
    file_close ( fp );
    /* Close reserve and other always-open files and release other resources */
    /* exec - descriptors are inherited */
    sprintf ( arg0, "%s", "rot" );
    sprintf ( arg1, "%d", port );
    sprintf ( arg2, "%d", 0 );
    sprintf ( arg3, "%s", "copyover" );
    sprintf ( arg4, "%d", control );
    sprintf ( arg5, "%d", 0 );
    execl ( EXE_FILE, arg0, arg1, arg2, arg3, arg4, arg5, ( char * ) NULL );
    /* Failed - sucessful exec will not return */
    perror ( "do_copyover: execl" );
    if ( ch )
        send_to_char ( "Copyover FAILED!\r\n", ch );
}

/* Recover from a copyover - load players */
void copyover_recover (  )
{
    DESCRIPTOR_DATA *d;
    FILE *fp;
    char name[100];
    char host[MSL];
    int desc;
    bool fOld;

    mudlogf ( "Copyover recovery initiated" );
    fp = file_open ( COPYOVER_FILE, "r" );
    if ( !fp )                  /* there are some descriptors open which will
                                   hang forever then ? */
    {
        perror ( "copyover_recover:fopen" );
        mudlogf ( "Copyover file not found. Exitting.\n\r" );
        quit ( 1 );
    }

    unlink ( COPYOVER_FILE );   /* In case something crashes - doesn't prevent
                                   reading */
    for ( ;; )
    {
        fscanf ( fp, "%d %s %s\n", &desc, name, host );
        if ( desc == -1 )
            break;
        /* Write something, and check if it goes error-free */
        if ( !write_to_descriptor
             ( desc, "\n\rThe world shivers and trembles..\n\r", 0 ) )
        {
            close ( desc );     /* nope */
            continue;
        }

        d = new_descriptor (  );
        d->descriptor = desc;
        d->host = str_dup ( host );
        d->next = descriptor_list;
        descriptor_list = d;
        d->connected = CON_COPYOVER_RECOVER;    /* -15, so close_socket frees
                                                   the char */
        /* Now, find the pfile */
        
        fOld = load_char_obj ( d, name );
        if ( !fOld && str_cmp ( name, "logs" ) )            /* Player file not found?! */
        {
            write_to_descriptor ( desc,
                                  "\n\rSomehow, your character was lost in the copyover. Sorry.\n\r",
                                  0 );
            close_socket ( d );
        }
        else                    /* ok! */
        {
            write_to_descriptor ( desc,
                                  "\n\rThe world will never be the same..\n\r( Copyover complete. )\n\r",
                                  0 );

            if ( !str_cmp ( name, "logs" ) )
            {
                d->connected = CON_WATCH_LOGS;
                continue;
            } 

            /* Just In Case */
            if ( !d->character->in_room )
                d->character->in_room = get_room_index ( ROOM_VNUM_TEMPLE );
            /* Insert in the char_list */
            d->character->next = char_list;
            char_list = d->character;
            char_to_room ( d->character, d->character->in_room );
            do_look ( d->character, "auto" );
            act ( "$n materializes!", d->character, NULL, NULL, TO_ROOM );
            d->connected = CON_PLAYING;
            d->character->hit = d->character->max_hit;
            d->character->mana = d->character->max_mana;
            d->character->move = d->character->max_move;
            write_to_descriptor ( desc, "You have been restored for the inconvenience.\n\r", 0 );
/*            if ( d->character->pet )
            {
                char_to_room ( d->character->pet, d->character->in_room );
                act ( "$n materializes!.", d->character->pet, NULL, NULL,
                      TO_ROOM );
            } */
        }

    }
    file_close ( fp );
}

#define	CORE_EXAMINE_SCRIPT	"../area/gdbscript"

void halt_mud ( int sig )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    struct sigaction default_action;
    int i;
    pid_t forkpid;

    wait ( NULL );
    if ( !crashed )
    {
        crashed++;
        fprintf ( stderr, "GAME CRASHED (SIGNAL %d).\rLast command: %s\r", sig,
                  last_command );
        // Inform last command typer that he caused the crash
/*        if ( strlen ( last_command2 ) )
        {
            write_to_descriptor ( last_descriptor,
                                  "\n\rThe last command you typed, '", 0 );
            write_to_descriptor ( last_descriptor, last_command2, 0 );
            write_to_descriptor ( last_descriptor,
                                  "', might have caused this crash.\n\r"
                                  "Please note any unusual circumstances to IMP and avoid using that command.\n\r",
                                  0 );
        } */
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            ch = CH ( d );
            if ( !ch )
            {
                close_socket ( d );
                continue;
            }
            if ( IS_NPC ( ch ) )
                continue;
            write_to_descriptor ( d->descriptor,
                                  "\n\rThe mud has CRASHED.\007\n\r", 0 );
            write_to_descriptor ( d->descriptor,
                                  "\n\rThe mud has CRASHED.\n\r", 0 );
            write_to_descriptor ( d->descriptor,
                                  "\n\rThe mud has CRASHED.\n\r", 0 );
        }

        // try to save all characters - save_char_obj has sanity checking
        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            ch = CH ( d );
            if ( !ch )
            {
                close_socket ( d );
                continue;
            }
//            save_char_obj ( ch );
        }

        // success - proceed with fork/copyover plan.  Otherwise will go to
        // next section and crash with a full reboot to recover
        if ( ( forkpid = fork (  ) ) > 0 )
        {
            // Parent process copyover and exit 
            waitpid ( forkpid, NULL, WNOHANG | WUNTRACED );
            // this requires you to add an "if (ch)" before the send_to_char
            // statements in do_copyover.
            do_copyover ( NULL, "" );
            exit ( 0 );
        }
        else if ( forkpid < 0 )
        {
            exit ( 1 );
        }
        // Child process proceed to dump
        // Close all files!
        for ( i = 255; i >= 0; i-- )
            close ( i );

        // Dup /dev/null to STD{IN,OUT,ERR}
        open ( "/dev/null", O_RDWR );
        dup ( 0 );
        dup ( 0 );

        default_action.sa_handler = SIG_DFL;
        sigaction ( sig, &default_action, NULL );

        // I run different scripts depending on my port
        if ( !fork (  ) )
        {
//            execl ( CORE_EXAMINE_SCRIPT, CORE_EXAMINE_SCRIPT, ( char * ) NULL );
            exit ( 0 );
        }
        else
            return;
        raise ( sig );
    }

    if ( crashed == 1 )
    {
        crashed++;

        for ( d = descriptor_list; d != NULL; d = d_next )
        {
            d_next = d->next;
            ch = d->original ? d->original : d->character;
            if ( ch == NULL )
            {
                close_socket ( d );
                continue;
            }
            if ( IS_NPC ( ch ) )
                continue;
            write_to_descriptor ( d->descriptor,
                                  "** Error saving character files; conducting full reboot. **\007\n\r",
                                  0 );
            close_socket ( d );
            continue;
        }
        fprintf ( stderr, "CHARACTERS NOT SAVED.\r" );
        default_action.sa_handler = SIG_DFL;
        sigaction ( sig, &default_action, NULL );

        if ( !fork (  ) )
        {
            kill ( getppid (  ), sig );
            exit ( 1 );
        }
        else
            return;
        raise ( sig );
    }

    if ( crashed == 2 )
    {
        crashed++;
        fprintf ( stderr, "TOTAL GAME CRASH." );
        default_action.sa_handler = SIG_DFL;
        sigaction ( sig, &default_action, NULL );

        if ( !fork (  ) )
        {
            kill ( getppid (  ), sig );
            exit ( 1 );
        }
        else
            return;
        raise ( sig );
    }

    if ( crashed == 3 )
    {
        default_action.sa_handler = SIG_DFL;
        sigaction ( sig, &default_action, NULL );

        if ( !fork (  ) )
        {
            kill ( getppid (  ), sig );
            exit ( 1 );
        }
        else
            return;
        raise ( sig );
    }
}

