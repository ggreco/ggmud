/*
TinTin++
Copyright (C) 2001 Davin Chan, Robert Ellsworth, etc. (See CREDITS file)

This program is protected under the GNU GPL (See COPYING)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
/*********************************************************************/
/* file: tintin.h - the include file for tintin++                    */
/*                             TINTIN ++                             */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                    modified by Bill Reiss 1993                    */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

#include <stdio.h>
#ifndef __TINTIN_H__
#define __TINTIN_H__
/************************/
/* The meaning of life: */
/************************/
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* DSC : according to the man page for bcopy on my linux machine,
   bcopy has been deprecated and to use memcpy instead.
*/
/*
#ifdef HAVE_BCOPY
#define memcpy(s1, s2, n) bcopy(s2, s1, n)
#endif
*/
/***********************************************/
/* Some default values you might wanna change: */
/***********************************************/
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24
#define MAX_RETRY 10                 /* Max number of retry connect attempts */
#define TIME_BETWEEN_RETRIES 10      /* Time between retry attempts */
#define ALPHA 1
#define PRIORITY 0
#define CLEAN 0
#define END 1
#define OLD_LOG 0 /* set to one to use old-style logging */
#define DEFAULT_PURIST_MODE 0 /* purity and chastity are overrated */
#define DEFAULT_OPEN '{' /*character that starts an argument */
#define DEFAULT_CLOSE '}' /*character that ends an argument */
#define SYSTEM_COMMAND_DEFAULT "system"   /* name of the system command */
#define HISTORY_SIZE 30                   /* history size */
#define MAX_PATH_LENGTH 200               /* max path length */
#define DEFAULT_TINTIN_CHAR '#'           /* tintin char */
#define DEFAULT_VERBATIM_CHAR '\\'        /* if an input starts with this
                                             char, it will be sent 'as is'
                                             to the MUD */
#define DEFAULT_LUA_CHAR '&'
#ifndef DEFAULT_FILE_DIR
#define DEFAULT_FILE_DIR "/usr/local/lib/tintin" /* Path to Tintin files, or HOME */
#endif

#ifndef DEFAULT_RESOURCE_FILE
#define DEFAULT_RESOURCE_FILE ".tintinrc" /* Default name for the resource file */
#endif

#define CONFIG_FILE "tt.conf" /* tt's configuration file DSC */

#ifndef DEFAULT_HELP_FILE
#define DEFAULT_HELP_FILE "gg_help.txt" /* Default name for the help file */
#endif
#define COMPLETE_FILE "complete"
typedef struct list_s {
  char *word;
  struct list_s *next;
} list_t;

#define HISTORY_FILE ".tt_history" /* The name of the tintin history file */
#define DEFAULT_CAT_STR "cat "                 /* for unix:     cat */
#define DEFAULT_COMPRESS_EXT ".Z"              /* for compress: .Z */
#define DEFAULT_UNCOMPRESS_STR "uncompress -c "/* for compress: uncompress -c */
#define DEFAULT_GZIP_EXT ".gz"                 /* for gzip:     .gz */
#define DEFAULT_GUNZIP_STR "gunzip -c "        /* for gzip:     gunzip -c */

#define DEFAULT_BZIP_EXT ".bz2"                /* for bzip2: .bz2 */
#define DEFAULT_BUNZIP_STR "bunzip2 -c "       /* for bzip2: bunzip -c */

#define DEFAULT_ECHO FALSE                /* echo */         
#define DEFAULT_IGNORE FALSE              /* ignore */
#define DEFAULT_SPEEDWALK TRUE            /* speedwalk */
#define DEFAULT_PRESUB TRUE               /* presub before actions */
#define DEFAULT_TOGGLESUBS FALSE          /* turn subs on and off FALSE=ON*/
#define DEFAULT_USE_FIXED_MATH TRUE       /* dsc use broken or fixed math */
#define DEFAULT_USE_BROKEN_TELNET FALSE   /* dsc use broken telnet protocol */

#define DEFAULT_ALIAS_MESS TRUE           /* messages for responses */
#define DEFAULT_ACTION_MESS TRUE          /* when setting/deleting aliases, */
#define DEFAULT_SUB_MESS TRUE             /* actions, etc. may be set to */
#define DEFAULT_ANTISUB_MESS TRUE         /* default either on or off */
#define DEFAULT_HIGHLIGHT_MESS TRUE       /* TRUE=ON FALSE=OFF */
#define DEFAULT_VARIABLE_MESS TRUE        /* might want to turn off these */
#define DEFAULT_PATHDIR_MESS TRUE
#define DEFAULT_REDRAW FALSE
#define DEFAULT_SAVE_HISTORY TRUE         /* Write/Read history file? */
#define DEFAULT_PRETICK TRUE              /* print out the 10 seconds to tick*/
#define DEFAULT_APPEND_LOG FALSE          /* append or overwrite log file */

/* slow-walk mode. (see walkset_command():walk.c) */
#define WALKMODE_LOOP   0
#define WALKMODE_NOLOOP 1

/* zombi mode status */
#define ZOMBI_NOT          0 // normal session
#define ZOMBI_CONNECTED    1 // connected zombi session
#define ZOMBI_CONNECTING   2 // connecting zombi session
#define ZOMBI_DISCONNECTED 3 // disconnected zombi session

/* test a session x whether it is in zombi mode or not. */
#define ZOMBI_IS_ALIVE(x)  (x->zombistat == ZOMBI_CONNECTED || x->zombistat == ZOMBI_NOT)

/**************************************************************************/
/* Whenever TINTIN has written something to the screen, the program sends */
/* a CR/LF to the diku to force a new prompt to appear. You can have      */
/* TINTIN print it's own pseudo prompt instead.                           */
/**************************************************************************/
/*
 * new for readline support -- this #define is ignored.
 * there's no more pseudo-prompting, it drives me haywire
 * (trying to code it).  -- daw
 */
#define PSEUDO_PROMPT TRUE

/**************************************************************************/
/* the codes below are used for highlighting text, and is set for the     */
/* codes for VT-100 terminal emulation. If you are using a different      */
/* teminal type, replace the codes below with the correct codes and       */
/* change the codes set up in highlight.c                                 */
/**************************************************************************/
#define DEFAULT_BEGIN_COLOR "["
#define DEFAULT_END_COLOR "[m"

/*************************************************************************/
/* The text below is checked for. If it trickers then echo is turned off */
/* echo is turned back on the next time the user types a return          */
/*************************************************************************/
#define PROMPT_FOR_PW_TEXT "assword:"

/**************************************************************************/ 
/* The stuff below here shouldn't be modified unless you know what you're */
/* doing........                                                          */
/**************************************************************************/ 

#define BUFFER_SIZE 2048
#define NORMAL_STRLEN 256 

/* NOTE: get rid of the DEVELOPMENT warning in main() when you update this! */
#define VERSION_NUM "v1.86 (Beta)"

/***************
* Command Tags *
***************/
#define CHAT_NAME_CHANGE			1
#define CHAT_REQUEST_CONNECTIONS		2
#define CHAT_CONNECTION_LIST			3
#define CHAT_TEXT_EVERYBODY			4
#define CHAT_TEXT_PERSONAL			5
#define CHAT_TEXT_GROUP				6
#define CHAT_MESSAGE				7
#define CHAT_DO_NOT_DISTURB			8
#define CHAT_SEND_ACTION			9
#define CHAT_SEND_ALIAS				10
#define CHAT_SEND_MACRO				11
#define CHAT_SEND_VARIABLE			12
#define CHAT_SEND_EVENT				13
#define CHAT_SEND_GAG				14
#define CHAT_SEND_HIGHLIGHT			15
#define CHAT_SEND_LIST				16
#define CHAT_SEND_ARRAY				17
#define CHAT_SEND_BARITEM			18
#define CHAT_VERSION				19
#define CHAT_FILE_START				20
#define CHAT_FILE_DENY				21
#define CHAT_FILE_BLOCK_REQUEST			22
#define CHAT_FILE_BLOCK				23
#define CHAT_FILE_END				24
#define CHAT_FILE_CANCEL			25
#define CHAT_PING_REQUEST			26
#define CHAT_PING_RESPONSE			27
#define CHAT_END_OF_COMMAND			255

/***************
* Chat flags   *
***************/
#define CHAT_XFER_IALLOW (1 << 0)
#define CHAT_PRIVATE     (1 << 1)

/*****************************************
* Size of a file transfer block in bytes *
*****************************************/
#define BLOCK_SIZE 500

extern void load_tabs(void);

int timeofday;
/************************ structures *********************/

struct listnode {
  struct listnode *next;
  char *left, *right, *pr;
  int enabled;
};

typedef struct _trigclass
{
    const char *name;
    int how_many;
    struct _trigclass *next;
    char enabled;    
} trigger_class;

struct completenode {
  struct completenode *next;
  char *strng;
};

struct session {
  struct session *next;
  char *name;
  char *address;
  int tickstatus;
  int snoopstatus;
  FILE *logfile;
  int ignore;
  struct listnode *aliases, *actions, *subs, *myvars, *highs, *antisubs, *myfuncs;  
  char *history[HISTORY_SIZE];
  struct listnode *path, *pathdirs;
  int path_length, path_list_size;
  int socket, socketbit;
  int old_more_coming,more_coming;
  char last_line[BUFFER_SIZE];

  /* support for zombi-mode sessions */
  char zombistat ;            /* current status */
  char host[NORMAL_STRLEN] ;  /* host string */
  char port[NORMAL_STRLEN] ;  /* port string */

  /* support for slow-walk feature */
  char walk_path[BUFFER_SIZE] ; /* the path to be walked through */

  /* walk_index :
       The index of each pathdir within the walk-path.
       The maximum number of pathdirs can be contained in the walk-path
       is half the BUFFER_SIZE(the size of walk_path).
       But reducing this structure's size is not so much important,
       I think.
  */
  int  walk_index[BUFFER_SIZE] ;

  int  walk_now ;               /* current position in the walk_path */
  int  walk_standstill ; /* standstill flag. */
  /* advance and send move command if TRUE,
     advance but don't send move command if FALSE
  */
  int  walk_mode ;  /* set slow-walk properties here. */

  FILE *lowlogfile;   /* added for low-level logging - ycjhi */
#ifdef TELNET_SUPPORT
  char *telnet_buf;
  int telnet_buflen;
#endif
};

#ifdef TELNET_SUPPORT
#define INPUT_CHUNK 512
#endif

/* DSC: for the chat module */
#define SHORT_STRING 128
#define MAX_STRING_LENGTH 8192
#define COLOR_NORMAL "\x1B[0m"

struct chat_data {
  char chatname[SHORT_STRING];
  char ip[SHORT_STRING];
  unsigned int port;
  int flags;
  int fd;
  char version[SHORT_STRING];
  unsigned char writeq[MAX_STRING_LENGTH];
  unsigned char readq[MAX_STRING_LENGTH];
  FILE *xfer_fd;
  char filename[MAX_STRING_LENGTH];
  int filesize;
  int blocks_recvd;
  int last_block;
  int transfer_start_time;
  struct chat_data *next;
  
};

#endif
