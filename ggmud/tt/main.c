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
/* Autoconf patching by David Hedbor, neotron@lysator.liu.se */
/*********************************************************************/
/* file: main.c - main module - signal setup/shutdown etc            */
/*                             TINTIN++                              */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

/* note: a bunch of changes were made here to add readline support -- daw */

#include "config.h"
#include "../config.h"
#include "tintin.h"

#if defined(HAVE_STRING_H)
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#include <signal.h>
#include <fcntl.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef SOCKS
#include <socks.h>
#endif


#include "include/files.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/rl.h"
#include "include/rltab.h"
#include "include/utils.h"

#ifndef BADSIG
#define BADSIG (RETSIGTYPE (*)(int))-1
#endif

/*************** globals ******************/
int term_echoing = TRUE;
int echo = DEFAULT_ECHO;
int fixed_math = DEFAULT_USE_FIXED_MATH;
int show_pretick = DEFAULT_PRETICK;
int broken_telnet = DEFAULT_USE_BROKEN_TELNET;
int speedwalk = DEFAULT_SPEEDWALK;
int togglesubs = DEFAULT_TOGGLESUBS;
int presub = DEFAULT_PRESUB;
int redraw = DEFAULT_REDRAW;
int save_history = DEFAULT_SAVE_HISTORY;
int append_log = DEFAULT_APPEND_LOG;
int sessionsstarted;
int puts_echoing = TRUE;
int verbose = FALSE;
int alnum = 0;
int acnum = 0;
int subnum = 0;
int varnum = 0;
int hinum = 0;
int pdnum = 0;
int antisubnum = 0;
int verbatim = 0;
int prompt_on = 0;
int funcnum = 0;
char homepath[1025];
char prompt_line[BUFFER_SIZE];

struct session *sessionlist, *activesession;
struct listnode *common_aliases, *common_actions, *common_subs, *common_myvars;
struct listnode *common_highs, *common_antisubs, *common_pathdirs, *common_functions;
char vars[10][BUFFER_SIZE]; /* the %0, %1, %2,....%9 variables */
char tintin_char = DEFAULT_TINTIN_CHAR;
char verbatim_char = DEFAULT_VERBATIM_CHAR;    
char system_com[80] = SYSTEM_COMMAND_DEFAULT;
int retries = MAX_RETRY;
int time_before_try = TIME_BETWEEN_RETRIES;
int mesvar[7];
int display_row, display_col, input_row, input_col;
int split_line, term_columns;
char k_input[BUFFER_SIZE];
char done_input[BUFFER_SIZE], prev_command[BUFFER_SIZE];
int hist_num;
int is_split;
int text_came;


/****************************************************************************/
/* main() - show title - setup signals - init lists - readcoms - mainloop() */
/****************************************************************************/

int ttmain(int argc, char **argv)
{
  struct session *ses;
  char *strptr, temp[BUFFER_SIZE];
  int arg_num;
  FILE * fd;

  is_split = FALSE;
  ses = NULL;

  common_aliases = init_list();
  common_actions = init_list();
  common_subs = init_list();
  common_myvars = init_list();
  common_highs = init_list();
  common_antisubs = init_list();
  common_pathdirs = init_list();
  common_functions = init_list();
  mesvar[0] = DEFAULT_ALIAS_MESS;
  mesvar[1] = DEFAULT_ACTION_MESS;
  mesvar[2] = DEFAULT_SUB_MESS;
  mesvar[3] = DEFAULT_ANTISUB_MESS;
  mesvar[4] = DEFAULT_HIGHLIGHT_MESS;
  mesvar[5] = DEFAULT_VARIABLE_MESS;
  mesvar[6] = DEFAULT_PATHDIR_MESS;

  *homepath = '\0';
  if(!strcmp(DEFAULT_FILE_DIR, "HOME")) {
    if((strptr = (char *)getenv("HOME")))
      sprintf(homepath, "%s", strptr);
    else
      *homepath = '\0';
  } else
    sprintf(homepath, "%s", DEFAULT_FILE_DIR);
  arg_num = 1;
  if(argc > 1 && argv[1]) {
    if(*argv[1] == '-' && argv[1][1] == 'v') {
      arg_num = 2;
      verbose = TRUE;
    }
  }

  if(argc > arg_num && argv[arg_num])
    activesession = read_command(argv[arg_num], NULL);
  else {
    sprintf(temp, "%s", homepath);
    strcat(temp, "/.tintinrc");
    if((fd = fopen(temp, "r")) > 0) { /* Check if it exists */
      fclose(fd);
      activesession = read_command(temp, NULL);
    }
    else {
      if((strptr = (char *)getenv("HOME"))) {
        sprintf(homepath, "%s", strptr);
        sprintf(temp, "%s", homepath);
        strcat(temp, "/.tintinrc");
        if((fd = fopen(temp, "r")) > 0) { /* Check if it exists */
          fclose(fd);
          activesession = read_command(temp, NULL);
        }
      }
    }
  }
  tintin_puts2("##################################################", ses);
  sprintf(temp,"#                    G G M U D %12s      #", VERSION);
  tintin_puts2(temp, ses);
  tintin_puts2("#        THIS IS A DEVELOPMENT VERSION!!!        #", ses);
  tintin_puts2("#  Based on a merge between TinTin++ and SClient #", ses);
  tintin_puts2("#                     written by                 #", ses);
  tintin_puts2("#                   Gabriele Greco               #", ses);
  tintin_puts2("##################################################", ses);

  return 0;
}
