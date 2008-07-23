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
/* file: parse.c - some utility-functions                            */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

/* note: a bunch of changes were made here for readline support -- daw */

#include "config.h"
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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "include/action.h"
#include "include/alias.h"
#include "include/antisub.h"
#include "include/files.h"
#include "include/highlight.h"
#include "include/help.h"
#include "include/ivars.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/misc.h"
#include "include/net.h"
#include "include/parse.h"
#include "include/path.h"
#include "include/rl.h"
#include "include/rlhist.h"
#include "include/rltab.h"
#include "include/session.h"
#include "include/substitute.h"
#include "include/text.h"
#include "include/ticks.h"
#include "include/utils.h"
#include "include/variables.h"
#include "include/function.h"
#include "include/bindkey.h"
#include "include/walk.h"
#include "include/lua.h"

/**************************************************************************/
/* parse input, check for TINTIN commands and aliases and send to session */
/**************************************************************************/

extern void wait_command(struct session *ses, const char *arg, const char *line);
extern void chat_command(const char *arg, struct session *ses);
extern void chatwho_command(const char *arg, struct session *ses);
extern void chconnect_command(const char *arg,struct session *ses);
extern void chdisconnect_command(struct session *ses);

extern char lua_char;

struct session *parse_input(const char *input, struct session *ses)
{
  char command[BUFFER_SIZE], arg[BUFFER_SIZE], result[BUFFER_SIZE];
  const char *input2;
  struct listnode *ln;

  if(!term_echoing && activesession == ses) {
    term_echoing = TRUE;
  }

  if(!*input) {
    if(ses)
      write_line_mud("", ses);
    else
      write_com_arg_mud("", "", ses);
    return(ses);
  }

  if(*input == tintin_char && is_abbrev(input+1, "verbatim")) {
    verbatim_command();
    return(ses);
  }

  if(verbatim && ses) {
    write_line_mud(input, ses);
    return(ses);
  }
  
  if((*input == verbatim_char) && ses) {
    input++;
    write_line_mud(input, ses);
    return(ses);
  }

  /*  substitute_myvars(input, result, ses);
      input2 = result;*/

  input2 = input;

  while(*input2) {
    if(*input2 == ';')
      input2++;
   
#ifdef WITH_LUA
    if (*input2 == lua_char) {
        input2 = call_luafunction(input2 + 1);
        continue;
    }
#endif

    input2 = (char *)get_arg_stop_spaces(input2, command);
    input2 = (char *)get_arg_all(input2, arg);

    substitute_myvars(arg, result, ses);
    strcpy(arg, result);
    
    if(*command == tintin_char) {
      // we need an exception to handle the wait command...

      if (is_abbrev(command + 1, "wait")) {
          wait_command(ses, arg, input2);
          break;
      }
      ses = parse_tintin_command(command+1, arg, ses);
    }
    else if((ln = searchnode_list_begin((ses ? ses->aliases : common_aliases),
					command, ALPHA))) {
      int i;
      char *cpsource, *cpsource2, newcommand[BUFFER_SIZE], end;

      sprintf(vars[0],"%s", arg);

      for(i = 1, cpsource = arg; i < 10; i++) {

        /* Next lines CHANGED to allow argument grouping with aliases */
        while(*cpsource == ' ')
          cpsource++;
        end = (*cpsource == '{' ? '}' : ' ');
        cpsource = (*cpsource == '{' ? cpsource+1 : cpsource);
        for(cpsource2 = cpsource; *cpsource2 && *cpsource2 != end; cpsource2++)
	  ;
        memcpy(vars[i], cpsource, cpsource2-cpsource);
        vars[i][cpsource2-cpsource] = '\0';
        cpsource = (*cpsource2 ? cpsource2+1 : cpsource2);
      }
      prepare_actionalias(ln->right, newcommand, ses); 
      if(!strcmp(ln->right, newcommand) && *arg) {
        strcat(newcommand, " "); 
        strcat(newcommand, arg);
      }

      ses = parse_input(newcommand, ses);
    }
    else if(speedwalk && !*arg && is_speedwalk_dirs(command))
      do_speedwalk(command, ses);
    else if(*command == '.'&& is_speedwalk_dirs(command + 1)) 
      do_speedwalk(command+1, ses);
    else
    {
      get_arg_with_spaces(arg, arg);
      write_com_arg_mud(command, arg, ses);
    }
  }

  return(ses);
}

/**********************************************************************/
/* return TRUE if commands only consists of capital letters N,S,E ... */
/**********************************************************************/

int is_speedwalk_dirs(const char *cp)
{
  int flag = FALSE;

  while(*cp) {
    if(*cp != 'n' && *cp != 'e' && *cp != 's' && *cp != 'w' &&
       *cp != 'u' && *cp != 'd' && !isdigit(*cp))
      return(FALSE);
    if(!isdigit(*cp))
      flag = TRUE;
    cp++;
  }

  return(flag);
}

/**************************/
/* do the speedwalk thing */
/**************************/

void do_speedwalk(const char *cp, struct session *ses)
{
  char sc[2];
  const char *loc; 
  int multflag, loopcnt, i;

  sprintf(sc,"%s", "x");
  while(*cp) {
    loc = cp;
    multflag = FALSE;
    while(isdigit(*cp)) {
      cp++;
      multflag = TRUE;
    }
    if(multflag && *cp) {
      sscanf(loc, "%d%c", &loopcnt, sc);
      for(i = 0; i++ < loopcnt;)
	    write_com_arg_mud(sc, "", ses);
    }
    else if(*cp) {
      sc[0] = *cp;
      write_com_arg_mud(sc, "", ses);
    }
    /* Added the if to make sure we didn't move the pointer outside the 
       bounds of the origional pointer.  Corrects the bug with speedwalking
       where if you typed "u7" tintin would go apeshit. (JE)
    */
    if(*cp)
      cp++;
  }
}

/*************************************/
/* parse most of the tintin-commands */
/*************************************/

struct session *parse_tintin_command(const char *command, char *arg,
				     struct session *ses)
{
  struct session *sesptr;
  char buf[BUFFER_SIZE];

  for(sesptr = sessionlist; sesptr; sesptr = sesptr->next)
    if(!strcmp(sesptr->name, command)) {
      if(*arg) {
        get_arg_with_spaces(arg, arg);
        parse_input(arg, sesptr);  /* was: #sessioname commands */
        return(activesession);
      }
      else {
        activesession = sesptr;
        sprintf(buf, "#SESSION '%s' ACTIVATED.", sesptr->name);
        tintin_puts(buf, sesptr);
        prompt(NULL);
        return(sesptr);
      }
    }

  if(isdigit(*command)) {
    int i = atoi(command);

    if(i > 0) {
     get_arg_in_braces(arg, arg, 1);
     while(i-- > 0)
       ses = parse_input(arg, ses);
    }
    else {
      tintin_puts("#YEAH RIGHT! GO REPEAT THAT YOURSELF DUDE.", ses);
      prompt(NULL);
    }
    return(ses);
  }

  else if(is_abbrev(command, "action") || is_abbrev(command, "trigger"))
    action_command(arg, ses);
   
  else if(is_abbrev(command, "alias"))
    alias_command(arg, ses);
  else if(is_abbrev(command, "script"))
    script_command(arg, ses);
  else if(is_abbrev(command, "lua"))
    lua_command(arg, ses);
   
  else if(is_abbrev(command, "all"))
    ses = all_command(arg, ses);

  else if(is_abbrev(command, "antisubstitute"))
    parse_antisub(arg,ses);

  else if (is_abbrev(command, "appendlog"))
    append_log_command(ses);

  else if(is_abbrev(command, "bell") || is_abbrev(command, "beep"))
    bell_command(ses);

  else if(is_abbrev(command, "boss")) 
    boss_command(ses);

  else if(is_abbrev(command, "brokentelnet"))
    broken_telnet_command(ses);
  

  else if(is_abbrev(command, "cr"))
    cr_command(ses); 

  else if (is_abbrev(command, "clr")) 
    clr_command(arg, ses);
  
  else if(is_abbrev(command, "echo"))
    echo_command(ses);

  else if(is_abbrev(command, "end")) 
    end_command(command, ses);

  else if(is_abbrev(command, "fixedmath"))
    fixed_math_command(ses);

  else if(is_abbrev(command, "help")) 
    help_command(arg);

  else if(is_abbrev(command, "highlight"))
    parse_high(arg, ses);

#ifdef TODO
  else if(is_abbrev(command, "clearhistory"))
    clear_history();
  else if(is_abbrev(command, "history")) 
    rlhist_show();
  /* CHANGED to allow suspending from within tintin. */
  /* I know, I know, this is a hack *yawn* */
  else if(is_abbrev(command, "suspend"))
    tstphandler(0);
#endif

  else if(is_abbrev(command, "if")) 
    if_command(arg, ses);

  else if(is_abbrev(command, "ifstrequal")) 
    ifstrequal_command(arg, ses);
  
  else if(is_abbrev(command, "revstring")) 
    revstring_command(arg, ses);
  
  else if(is_abbrev(command, "ifexist")) 
    ifexist_command(arg, ses);
  
  else if(is_abbrev(command, "ifmatch")) 
    ifmatch_command(arg, ses);
  
  else if(is_abbrev(command, "ignore"))
    ignore_command(ses);
  
  else if(is_abbrev(command, "info"))
    display_info(ses);
  
  else if(is_abbrev(command, "killall"))
    kill_all(ses, CLEAN);

  else if(is_abbrev(command, "log")) 
    log_command(arg, ses);

  else if(is_abbrev(command, "loop"))
    loop_command(arg, ses);
  
  else if(is_abbrev(command, "forall"))
    forall_command(arg, ses);

  else if(is_abbrev(command, "nop"))
    ;  

  else if(is_abbrev(command, "map"))
    map_command(arg,ses);
  
  else if(is_abbrev(command, "math")) 
    math_command(arg, ses);

  else if(is_abbrev(command, "mark")) 
    mark_command(ses);
  else if(is_abbrev(command, "message"))
    mess_command(arg, ses);
  else if(is_abbrev(command, "path")) 
    path_command(ses);
  
  else if(is_abbrev(command, "pathdir")) 
    pathdir_command(arg, ses); 

  else if(is_abbrev(command, "presub"))
    presub_command(ses);

  else if(is_abbrev(command, "redraw"))
    redraw_command();

  else if(is_abbrev(command, "removestring"))
    removestring_command(arg, ses);

  else if(is_abbrev(command, "retab"))
    load_tabs();

  else if(is_abbrev(command, "return"))
    return_command(ses);

  else if(is_abbrev(command, "read"))
    ses = read_command(arg, ses);

  else if(is_abbrev(command, "newread"))
    ses = read_command_new(arg, ses);

  else if(is_abbrev(command, "savepath"))
    savepath_command(arg, ses);

  else if(is_abbrev(command, "savehistory"))
    save_history_command(ses);

  else if(is_abbrev(command, "session"))
    ses = session_command(arg, ses);

  else if(is_abbrev(command, "showme") || is_abbrev(command, "say"))
    showme_command(arg,ses);

  else if (is_abbrev(command, "show_pretick"))
    show_pretick_command(ses);

  else if(is_abbrev(command, "sleep"))
    sleep_command(arg, ses);

  else if(is_abbrev(command, "snoop"))
    snoop_command(arg, ses);

  else if(is_abbrev(command, "speedwalk"))
    speedwalk_command(ses);

  else if(is_abbrev(command, "substitute"))
    parse_sub(arg, ses);
 
  else if(is_abbrev(command, "tablist"))
    rltab_list();

  else if(is_abbrev(command, "tabadd"))
    do_tabadd(arg);

  else if(is_abbrev(command, "tabdelete"))
    do_tabdel(arg);

  else if(is_abbrev(command, "tabsave"))
    do_tabsave();

  else if(is_abbrev(command, "textin"))
    read_file(arg, ses);  
  else if(is_abbrev(command, "timer"))
    do_timer(arg, ses);
  else if(is_abbrev(command, "gettimer"))
    do_gettimer(arg, ses);
  else if(is_abbrev(command, "stoptimer"))
    do_stoptimer(arg, ses);
  else if(is_abbrev(command, "timetry"))
    time_between_tries(arg, ses);
  else if(is_abbrev(command, "vbutton"))
    vbutton_command(arg, ses);
  else if(is_abbrev(command, "hbutton"))
    hbutton_command(arg, ses);

  else if(is_abbrev(command, "tries"))
    number_of_tries(arg, ses);

  else if(is_abbrev(command, "gag")) {
    if(*arg != '{') {
      sprintf(buf, "%s", arg);
      sprintf(arg, "%s", "{");
      strcat(arg, buf);
      strcat(arg, "} ");
    }
    strcat(arg, " .");
    parse_sub(arg, ses);
  }
  
  else if(system_com && is_abbrev(command, system_com))
    system_command(arg, ses);

  else if(is_abbrev(command, "tick")) 
    tick_command(ses);

  else if(is_abbrev(command, "tickoff")) 
    tickoff_command(ses);

  else if(is_abbrev(command, "tickon")) 
    tickon_command(ses);

  else if(is_abbrev(command, "tickset") || is_abbrev(command, "tz") )
    tickset_command(ses);

  else if(is_abbrev(command, "ticksize") || is_abbrev(command, "ts") ) 
    ticksize_command(arg, ses);

  else if(is_abbrev(command, "tolower"))
    tolower_command(arg, ses);

  else if(is_abbrev(command, "togglesubs"))
    togglesubs_command(ses);

  else if(is_abbrev(command, "toupper"))
    toupper_command(arg, ses);

  else if(is_abbrev(command, "unaction"))
    unaction_command(arg, ses);

  else if(is_abbrev(command, "unalias"))
    unalias_command(arg, ses);

  else if(is_abbrev(command, "unantisubstitute"))
    unantisubstitute_command(arg, ses);
    
  else if(is_abbrev(command, "unhighlight"))
    unhighlight_command(arg, ses);
  
  else if(is_abbrev(command, "unsubstitute"))
    unsubstitute_command(arg, ses);

  else if(is_abbrev(command, "ungag"))
    unsubstitute_command(arg, ses);

  else if(is_abbrev(command, "unpath")) 
    unpath_command(ses);

  else if(is_abbrev(command, "variable"))
    var_command(arg, ses);
  else if(is_abbrev(command, "verbosity"))
    message_command(arg, ses);
    
  else if(is_abbrev(command, "version"))
    version_command();

  else if(is_abbrev(command, "unvariable"))
    unvar_command(arg, ses);
 
  else if(is_abbrev(command, "window"))
    window_command(arg, ses);
  else if(is_abbrev(command, "wizlist")) 
    wizlist_command(ses);

  else if(is_abbrev(command, "write"))
    ses = write_command(arg, ses);

  else if(is_abbrev(command, "writesession"))
    ses = writesession_command(arg, ses);

  else if(is_abbrev(command, "zap"))
    ses = zap_command(ses);

  else if(is_abbrev(command, "random"))
    random_command(arg, ses);
  
  else if(is_abbrev(command, "setprompt"))
   setprompt_command(arg, ses);

  else if(is_abbrev(command, "postpad"))
    postpad_command(arg, ses);

  else if(is_abbrev(command, "prepad"))
    prepad_command(arg, ses);

  else if(is_abbrev(command, "function"))
    function_command(arg, ses);

  else if(is_abbrev(command, "unfunction"))
    unfunction_command(arg, ses);

  else if(is_abbrev(command, "result"))
    result_command(arg, ses);

  else if(is_abbrev(command, "getvarvalue"))
    getvarvalue_command(arg, ses);

  else if(is_abbrev(command, "getitemnr"))
    getitemnr_command(arg, ses);

  else if(is_abbrev(command, "getlistlength"))
    getlistlength_command(arg, ses);

  else if(is_abbrev(command, "clearprompt")) {
   tintin_puts("#PROMPT LINE CLEARED.", ses);
   prompt_line[0] = '\0';
   sprintf(arg,"%s","-------------------------------------------------------------------------");
//   status_in_splitline(arg, ses);
   prompt_on = 0;
   }

  /* added by ycjhi ------------- begin */
  else if(is_abbrev(command, "lowlog"))
    lowlog_command(arg, ses);

  else if(is_abbrev(command, "walk"))
    walk_command(ses);
  else if(is_abbrev(command, "chat"))
    chat_command(arg, ses);
  else if(is_abbrev(command, "chatwho"))
    chatwho_command(arg, ses);
  else if(is_abbrev(command, "chconnect"))
    chconnect_command(arg, ses);
  else if(is_abbrev(command, "chdisconnect"))
    chdisconnect_command(ses);
  else if(is_abbrev(command, "char"))
    char_command(arg, ses);
  else if(is_abbrev(command, "walkback"))
    walkback_command(ses);

  else if(is_abbrev(command, "walkinfo"))
    walkinfo_command(ses);

  else if(is_abbrev(command, "walkoff"))
    walkoff_command(ses);

  else if(is_abbrev(command, "walkon"))
    walkon_command(ses);

  else if(is_abbrev(command, "walkreset"))
    walkreset_command(arg, ses);

  else if(is_abbrev(command, "walkset"))
    walkset_command(arg, ses);

  else if(is_abbrev(command, "getwalkposition"))
    getwalkposition_command(arg, ses) ;

  else if(is_abbrev(command, "getwalkdirection"))
    getwalkdirection_command(arg, ses) ;

  else if(is_abbrev(command, "zombioff"))
    zombioff_command(ses);

  else if(is_abbrev(command, "zombion"))
    zombion_command(ses);
  else if(is_abbrev(command, "url"))
    url_command(arg, ses);
  /* added by ycjhi ------------- end   */


  else {
    tintin_puts("#UNKNOWN TINTIN-COMMAND.", ses);
    prompt(NULL);
  }

  return(ses);
}

/**********************************************/
/* get all arguments - don't remove "s and \s */
/**********************************************/

const char *get_arg_all(const char *s, char *arg)
{
  int nest = 0;

  s = space_out(s);
  while(*s)
    if(*s == '\\') {
      *arg++ = *s++;
      if(*s)
	*arg++ = *s++;
    }
#ifdef BIG5
  /* for Big5 encoding */
    else if(*s & 0x80) {
      *arg++ = *s++;
      if(*s)
	*arg++ = *s++;
    }
#endif
    else if(*s == ';' && nest < 1)
      break;
    else if(*s == DEFAULT_OPEN) {
      nest++;
      *arg++ = *s++;
    }
    else if(*s == DEFAULT_CLOSE) {
      nest--;
      *arg++ = *s++;
    }
    else
      *arg++ = *s++;

  *arg = '\0';
  return(s);
}

/**************************************/
/* get all arguments - remove "s etc. */
/* Example:                           */
/* In: "this is it" way way hmmm;     */
/* Out: this is it way way hmmm       */ 
/**************************************/

const char *get_arg_with_spaces(const char *s, char *arg)
{
  int nest = 0;
  /* int inside = FALSE; */

  s = space_out(s);
  while(*s) {
    if(*s == '\\') {
      if(*++s)
	*arg++ = *s++;
    }
#ifdef BIG5
    /* for Big5 encoding */
    else if(*s & 0x80) {
      *arg++ = *s++;
      if(*s)
	*arg++ = *s++;
    }
#endif
    else if(*s == ';' && !nest)
      break;
    else if(*s == DEFAULT_OPEN) {
      nest++;
      *arg++ = *s++;
    }
    else if(*s == DEFAULT_CLOSE) {
      *arg++ = *s++;
      nest--;
    }
    else
      *arg++ = *s++;
  }

  *arg = '\0'; 
  return(s);
}

/********************/
/* my own routine   */
/********************/

const char *get_arg_in_braces(const char *s, char *arg, int flag)
{
  int nest = 0;
  const char *ptr;

  s = space_out(s);
  ptr = s;
  if(*s != DEFAULT_OPEN) {
    if(!flag)
      s = get_arg_stop_spaces(ptr, arg);
    else
      s = get_arg_with_spaces(ptr, arg);
    return(s);
  }

  s++;
  while(*s && !(*s == DEFAULT_CLOSE && !nest)) {
    if(*s == DEFAULT_OPEN)
      nest++;
    else if(*s == DEFAULT_CLOSE)
      nest--;
#ifdef BIG5
    /* for Big5 encoding */
    else if(*s & 0x80) {
      *arg++ = *s++;
      if(!*s) break;
    }
#endif
    *arg++ = *s++;
  }

  if(!*s)
    tintin_puts2("#Unmatched braces error!", (struct session *)NULL);
  else
    s++;

  *arg = '\0';
  return(s);
}

/**********************************************/
/* get one arg, stop at spaces                */
/* remove quotes                              */
/**********************************************/

const char *get_arg_stop_spaces(const char *s, char *arg)
{
  int inside = FALSE;

  s = space_out(s);
  
  while(*s) {
    if(*s == '\\') {
      if(*++s)
	*arg++ = *s++;
    }
#ifdef BIG5
    /* for Big5 encoding */
    else if(*s & 0x80) {
      *arg++ = *s++;
      if(*s)
	*arg++ = *s++;
    }
#endif
    else if(*s == '"') {
      s++;
      inside = !inside;
    }
    else if(*s == ';') {
      if(inside)
	*arg++ = *s++;
      else
	break;
    }
    else if(!inside && *s == ' ')
      break;
    else
      *arg++ = *s++;
  }

  *arg = '\0';
  return(s);
}

/*********************************************/
/* spaceout - advance ptr to next none-space */
/* return: ptr to the first none-space       */
/*********************************************/ 

const char *space_out(const char *s)
{
  while(isspace(*s))
    s++;
  return(s);
}

/************************************/
/* send command+argument to the mud */
/************************************/

void write_com_arg_mud(const char *command, const char *argument,
		       struct session *ses)
{
  char outtext[BUFFER_SIZE];
  int i;

  if(!ses) {
    char buf[100];

    sprintf(buf, "#NO SESSION ACTIVE. USE THE %cSESSION-COMMAND TO START ONE.", tintin_char);
    tintin_puts(buf, ses);
    prompt(NULL);
  }
  else {
    check_insert_path(command, ses);
    strncpy(outtext, command, BUFFER_SIZE);
    if(*argument) {
      strncat(outtext, " ", BUFFER_SIZE-strlen(command)-1);
      strncat(outtext, argument, BUFFER_SIZE-strlen(command)-2);
    }
    write_line_mud(outtext, ses);
    if(ses->logfile) {
      outtext[i = strlen(outtext)] = '\n';
      fwrite(outtext, i+1, 1, ses->logfile);
    }
  }
}


/***************************************************************/
/* show a prompt - mud prompt if we're connected/else just a > */
/***************************************************************/

void prompt(struct session *ses)
{
  return; /* for readline -- please god, no pseudo prompting... <beg> */

/*
  char strng[80];

  if(ses && !PSEUDO_PROMPT)
    write_line_mud("", ses);
  else if(!is_split)
    write(1, "> ", 2);
  else {
    sprintf(strng, "8> 7[%d;%df", input_row, input_col);
    write(1, strng, strlen(strng));
    display_col += 2;
  } 
*/
}

/**********************************************************/
/* do all of the functions to one line of buffer          */
/**********************************************************/

void do_one_line(char *line, struct session *ses)
{
  if (check_msp(line))
      return;

  if(!presub && !ses->ignore)
    check_all_actions(line, ses);
  if(!togglesubs && !do_one_antisub(line, ses))
    do_one_sub(line, ses);
  if(presub && !ses->ignore)
    check_all_actions(line, ses);
  do_one_high(line, ses);
}
