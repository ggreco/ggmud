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
/* file: misc.c - misc commands                                      */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

/* note: a bunch of changes were made for readline support -- daw */

#include "config.h"
#include "tintin.h"

#ifdef WIN32
#include <winsock2.h>
#define sleep(x) Sleep(x*1000)
#endif

#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#if defined(HAVE_STRING_H)
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "include/action.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/misc.h"
#include "include/net.h"
#include "include/parse.h"
#include "include/rl.h"
#include "include/session.h"
#include "include/utils.h"
#include "include/variables.h"

#include <time.h>

/****************************/
/* the cr command           */
/****************************/

void cr_command(struct session *ses)
{
  if(ses)
    write_line_mud("\n", ses);
}

/****************************/
/* the version command      */
/****************************/

void version_command(void)
{ 
  char temp[80];

  sprintf(temp, "#You are using TINTIN++ %s\n\r", VERSION_NUM);
  tintin_puts2(temp, NULL);
  prompt(NULL);
} 

/****************************/
/* the verbatim command,    */
/* used as a toggle         */
/****************************/

void verbatim_command(void)
{
  if((verbatim = !verbatim)) 
    tintin_puts2("#All text is now sent 'as is'.", NULL);
  else 
    tintin_puts2("#Text is no longer sent 'as is'.", NULL); 
  prompt(NULL);
}

/********************/
/* the #all command */
/********************/

struct session *all_command(const char *arg, struct session *ses)
{
  struct session *sesptr, *next_ses;

  if(sessionlist) {
    char left[BUFFER_SIZE];

    get_arg_in_braces(arg, left, 1);
    for(sesptr = sessionlist; sesptr; sesptr = next_ses) {
      next_ses = sesptr->next;
      parse_input(left, sesptr);
    }
  }
  else
    tintin_puts("#BUT THERE ISN'T ANY SESSION AT ALL!", ses);

  return(sessionlist ? ses : NULL);
}

/***********************/
/* the #redraw command */
/***********************/

void redraw_command(void)
{
  if((redraw = !redraw)) 
    tintin_puts2("#Ok. I now redraw input line when text arrives.", NULL);
  else
    tintin_puts2("#Ok. I no longer redraw the input line.", NULL);
  prompt(NULL);
}

/*********************/
/* the #bell command */
/*********************/

void bell_command(struct session *ses)
{
  char temp[2];

  temp[0] = 7;
  temp[1] = 0;
//  write(1, temp, 1);
  gdk_beep();
}

/*********************/
/* the #boss command */
/*********************/

void boss_command(struct session *ses)
{
  char temp[80];
  int i;

  for(i = 0; i < 50; i++) {
    sprintf(temp, "in-order traverse of tree starting from node %d"
	    "resulted in %d red nodes\n", i, 50-i);
    tintin_puts2(temp, NULL);
  }
  getchar(); /* stop screen from scrolling stuff */
}

/*********************/
/* the #char command */
/*********************/

void char_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], strng[80];

  get_arg_in_braces(arg, left, 1);
  if(ispunct(*left)) {
    tintin_char = *left;
    sprintf(strng, "#OK. TINTIN-CHAR is now {%c}\n", tintin_char);
    tintin_puts2(strng, NULL);
  }
  else
    tintin_puts2("#SPECIFY A PROPER TINTIN-CHAR! SOMETHING LIKE # OR /!", NULL);
}

/**********************/
/* the #tries command */
/**********************/
void number_of_tries(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], buf[100];
  get_arg_in_braces(arg, left, 1);
  if (*left) {
    if (atoi(left) > 0)
      {
	retries = atoi(left);
	sprintf(buf, "#OK. The number of retries is now %d\n", retries);
	tintin_puts2(buf, NULL);
      }
    else tintin_puts2("#SPECIFY A VALID NUMBER FOR RETRIES", NULL);
  }
  else
    tintin_puts2("#SPECIFY A NUMBER FOR RETRIES", NULL);
}
/************************/
/* the #timetry command */
/************************/
void time_between_tries(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], buf[100];
  get_arg_in_braces(arg, left, 1);
  if (*left) {
    if (atoi(left) > 0)
      {
	time_before_try = atoi(left);
	sprintf(buf, "#OK. The time between tries is now %d\n", 
		time_before_try);
	tintin_puts2(buf, NULL);
      }
    else tintin_puts2("#SPECIFY A VALID TIME BETWEEN TRIES", NULL);
  }
  else
    tintin_puts2("#SPECIFY A TIME (IN SECONDS) BETWEEN TRIES", NULL);
}

/*****************************/
/* the #brokentelnet command */
/*****************************/
void broken_telnet_command(struct session *ses)
{
  if (broken_telnet == 0) {
    broken_telnet = 1;
    tintin_puts("#BROKEN TELNET IS NOW ON.", ses);
  } else {
    broken_telnet = 0;
    tintin_puts("#BROKEN TELNET IS NOW OFF.", ses);
  }    
  
}

/**************************/
/* the #fixedmath command */
/**************************/
void fixed_math_command(struct session *ses)
{
  if(fixed_math) {
    fixed_math = 0;
    tintin_puts("#FIXED MATH IS NOW OFF.", ses);
  } else {
    fixed_math = 1;
    tintin_puts("#FIXED MATH IS NOW ON.", ses);
  }
}

/**************************/
/* the #appendlog command */
/**************************/
void append_log_command(struct session *ses)
{
  if(append_log) {
    append_log = 0;
    tintin_puts("#APPEND LOG IS NOW OFF.", ses);
  } else {
    append_log = 1;
    tintin_puts("#APPEND LOG IS NOW ON.", ses);
  }
}

/**************************/
/* the #show_pretick command */
/**************************/
void show_pretick_command(struct session *ses)
{
  if(show_pretick) {
    show_pretick = 0;
    tintin_puts("#PRETICK IS NOW OFF.", ses);
  } else {
    show_pretick = 1;
    tintin_puts("#PRETICK IS NOW ON.", ses);
  }
}

/*********************/
/* the #echo command */
/*********************/

void echo_command(struct session *ses)
{
  if((echo = !echo))
    tintin_puts("#ECHO IS NOW ON.", ses);
  else
    tintin_puts("#ECHO IS NOW OFF.", ses);
}

/****************************/
/* the #savehistory command */
/****************************/

void save_history_command(struct session *ses)
{
  if (save_history == 1) {
    save_history = 0;
    tintin_puts("#SAVE HISTORY IS NOW OFF.", ses);
  } else {
    save_history = 1;
    tintin_puts("#SAVE HISTORY IS NOW ON.", ses);
  }

}

/*********************/
/* the #end command */
/*********************/

void end_command(const char *command, struct session *ses)
{
  if(strcmp(command, "end"))
    tintin_puts("#YOU HAVE TO WRITE #end - NO LESS, TO END!", ses);
  else
    exit(0);
}

/***********************/
/* the #ignore command */
/***********************/

void ignore_command(struct session *ses)
{
  if(ses) {
    if((ses->ignore = !ses->ignore))
      tintin_puts("#ACTIONS ARE IGNORED FROM NOW ON.", ses);
    else
      tintin_puts("#ACTIONS ARE NO LONGER IGNORED.", ses);
  } else
    tintin_puts("#No session active => Nothing to ignore!", ses);     
}

/***********************/
/* the #presub command */
/***********************/

void presub_command(struct session *ses)
{
  if((presub = !presub))
    tintin_puts("#ACTIONS ARE NOW PROCESSED ON SUBSTITUTED BUFFER.", ses);
  else
    tintin_puts("#ACTIONS ARE NO LONGER DONE ON SUBSTITUTED BUFFER.", ses);
}

/***************************/
/* the #togglesubs command */
/***************************/

void togglesubs_command(struct session *ses)
{
  if((togglesubs = !togglesubs))
    tintin_puts("#SUBSTITUTES ARE NOW IGNORED.", ses);
  else 
    tintin_puts("#SUBSTITUTES ARE NO LONGER IGNORED.", ses);
}

/*********************/
/* the #loop command */
/*********************/

void loop_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE];
  char result[BUFFER_SIZE], temp[BUFFER_SIZE];

  int bound1, bound2, counter, step;

  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);
  substitute_vars(left, temp);
  substitute_myvars(temp, left, ses);  
  if(sscanf(left, "%d,%d", &bound1, &bound2) != 2) 
    tintin_puts2("#Wrong number of arguments in #loop", ses);
  else {
    if(bound1 < bound2) {
      step = 1;
      bound2++;
    }
    else {
      step = -1;
      bound2--;
    }

    for(counter = bound1; counter != bound2; counter += step) {
      sprintf(vars[0], "%d", counter);
      substitute_vars(right, result);
      parse_input(result, ses);
    }
  }  
}

/***********************/
/* the #forall command */
/***********************/

void forall_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE];
  char result[BUFFER_SIZE], *buf;

  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);
  buf = strtok(left, ",");
  if(!buf)
    tintin_puts2("#Empty argument in #forall", ses);
  else
    for(; buf; buf = strtok(0, ",")) {
      sprintf(vars[0], "%s", buf);
      substitute_vars(right, result);
      parse_input(result, ses);
    }
}

/************************/
/* the #message command */
/************************/

void message_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], tpstr[80], temp[100];
  int mestype = 0;
  static const char * const ms[6] =
  {
    "aliases", "actions", "substitutes",
    "antisubstitutes", "highlights", "variables"
  };

  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 0);

  while((mestype < 6) && !is_abbrev(left, ms[mestype])) {
    sprintf(temp, "%s %s", ms[mestype],(mesvar[mestype] ? "ON" : "OFF"));
    tintin_puts2(temp, ses);
    mestype++;
  }
  if(mestype == 6)
  {
    tintin_puts2("#Invalid message type to toggle.", ses);
    tintin_puts2("#Syntax: #message <type> [<on/off>].", ses);
  }
  else {
    if(!*right)
      mesvar[mestype] = !mesvar[mestype];
    else if(is_abbrev(right, "on"))
      mesvar[mestype] = TRUE;
    else if(is_abbrev(right, "off"))
      mesvar[mestype] = FALSE;
    else
      mesvar[mestype] = !mesvar[mestype];

    sprintf(tpstr, "#Ok. messages concerning %s are now %s.",
      ms[mestype], (mesvar[mestype] ? "ON" : "OFF"));
    tintin_puts2(tpstr, ses);
  }
}

/**********************/
/* the #snoop command */
/**********************/

void snoop_command(const char *arg, struct session *ses)
{
  struct session *sesptr = ses;
  char left[BUFFER_SIZE];

  if(ses) {
    get_arg_in_braces(arg, left, 1);
    if(*left) {
      for(sesptr = sessionlist;
	  sesptr && strcmp(sesptr->name, left);
	  sesptr = sesptr->next)
	;
      if(!sesptr) {
        tintin_puts("#NO SESSION WITH THAT NAME!", ses);
        return;
      }
    }

    if(sesptr->snoopstatus) {
      sesptr->snoopstatus = FALSE;
      sprintf(left, "#UNSNOOPING SESSION '%s'", sesptr->name);
      tintin_puts(left, ses);
    }
    else {
      sesptr->snoopstatus = TRUE;
      sprintf(left, "#SNOOPING SESSION '%s'", sesptr->name);
      tintin_puts(left, ses);
    }
  }
  else
    tintin_puts("#NO SESSION ACTIVE => NO SNOOPING", ses);
}

/**************************/
/* the #speedwalk command */
/**************************/

void speedwalk_command(struct session *ses)
{
  if((speedwalk = !speedwalk))
    tintin_puts("#SPEEDWALK IS NOW ON.", ses);
  else
    tintin_puts("#SPEEDWALK IS NOW OFF.", ses);
}

/***********************/
/* the #sleep command  */
/***********************/
/* DSC - I could have sworn that there was a command like this already
   in tintin by the name of delay, but I guess I was wrong about that.
   Idea came from chitchat's version but implementation is different
   (simplier I believe)
*/
void sleep_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE];
  int sleep_time = 0;
  get_arg_in_braces(arg, left, 1);
  if (*left) {
    if (atoi(left) > 0)
      {
	sleep_time = atoi(left);
	sleep(sleep_time);
      }
    else tintin_puts2("#SPECIFY A VALID NUMBER TO SLEEP", NULL);
  }
  else
    tintin_puts2("#SLEEP FOR HOW LONG?", NULL);
}

/***********************/
/* the #system command */
/***********************/

void system_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE];

  get_arg_in_braces(arg, left, 1);

  if(!*left) {
    tintin_puts2("#EXECUTE WHAT COMMAND?", ses);
    return;
  }

  tintin_puts2("#OK EXECUTING SHELL COMMAND.", ses);
  system(left);
  tintin_puts2("#OK COMMAND EXECUTED.", ses);
  return;
}

/********************/
/* the #zap command */
/********************/

struct session *zap_command(struct session *ses)
{
  tintin_puts("#ZZZZZZZAAAAAAAAPPPP!!!!!!!!! LET'S GET OUTTA HERE!!!!!!!!", ses);
  if(!ses)
    return NULL;

  disconnect();

  return(newactive_session());
}

/************************/
/* the #wizlist command */
/************************/
/*
void wizlist_command(struct session *ses)
{
  tintin_puts2("==========================================================================", ses);
  tintin_puts2("                           Implementor:", ses);
  tintin_puts2("                              Valgar ", ses);
  tintin_puts2("", ses);
  tintin_puts2("          Special thanks to Grimmy for all her help :)", ses);
  tintin_puts2("\n\r                         TINTIN++ testers:", ses);
  tintin_puts2(" Nemesis, Urquan, Elvworn, Kare, Merjon, Grumm, Tolebas, Winterblade ", ses); 
  tintin_puts2("\n\r A very special hello to Zoe, Oura, GODDESS, Reyna, Randela, Kell, ", ses);
  tintin_puts2("                  and everyone else at GrimneDIKU\n\r", ses);
  tintin_puts2("==========================================================================", ses);
  prompt(ses);
} */

void wizlist_command(struct session *ses)
{

  tintin_puts2("==========================================================================", ses);
  tintin_puts2("  There are too many people to thank for making tintin++ into one of the", ses);
  tintin_puts2("finest clients available.  Those deserving mention though would be ", ses);
  tintin_puts2("Peter Unold, Bill Reiss, Robert Ellsworth, Jeremy Jack, and the many people", ses);
  tintin_puts2("who send us bug reports and suggestions.", ses);
  tintin_puts2("            Enjoy!!!  And keep those suggestions coming in!!\n\r", ses);
  tintin_puts2("                       The Management...", ses);
  tintin_puts2("==========================================================================", ses);
}

/*********************/
/* the #info command */
/*********************/

void display_info(struct session *ses)
{
  char buf[BUFFER_SIZE];
  int actions = 0, aliases = 0, variables = 0, 
      subs = 0, antisubs = 0, highs = 0, funcs = 0;
  int ignore;

  actions = count_list(ses ? ses->actions : common_actions);
  aliases = count_list(ses ? ses->aliases : common_aliases);
  subs = count_list(ses ? ses->subs : common_subs);
  antisubs = count_list(ses ? ses->antisubs : common_antisubs);
  variables = count_list(ses ? ses->myvars : common_myvars);
  highs = count_list(ses ? ses->highs : common_highs);
  funcs = count_list(ses ? ses->myfuncs  : common_functions);

  ignore = (ses ? ses->ignore : 0);

  tintin_puts2("You have defined the following:",ses);
  sprintf(buf, "Actions: %d", actions);
  tintin_puts2(buf, ses);
  sprintf(buf, "Aliases: %d", aliases);
  tintin_puts2(buf, ses);
  sprintf(buf, "Substitutes: %d", subs);
  tintin_puts2(buf, ses);
  sprintf(buf, "Antisubstitutes: %d",antisubs);
  tintin_puts2(buf, ses);
  sprintf(buf, "Variables: %d", variables);
  tintin_puts2(buf, ses);
  sprintf(buf, "Highlights: %d", highs);
  tintin_puts2(buf, ses);
  sprintf(buf, "Functions: %d", funcs);
  tintin_puts2(buf, ses);
  tintin_puts2("(1 - on, 0 - off)", ses);
  sprintf(buf, "Echo: %d   Speedwalking: %d   Redraw: %d",
	  echo, speedwalk, redraw);
  tintin_puts2(buf, ses);
  sprintf(buf, "Toggle Subs: %d   Ignore Actions: %d   PreSub-ing: %d",
	  togglesubs, ignore, presub);
  tintin_puts2(buf, ses);
  sprintf(buf, "Fixed Math:  %d   Broken Telnet:  %d",
	  fixed_math, broken_telnet);
  tintin_puts2(buf, ses);
  prompt(ses);
}

/*************************/
/* the #random command   */
/*************************/

void random_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], arg2[BUFFER_SIZE];
  char temp[BUFFER_SIZE];
  struct listnode *tempvars, *ln;
  int i=0;
  int j=0;

  i=time(NULL);
  if ((i % 3) == 0) {
     if (timeofday != i)  
        srand(i);
  }
  timeofday=i;
  tempvars = (ses ? ses->myvars : common_myvars);
  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);
  substitute_vars(right, temp);
  substitute_myvars(temp, right, ses);

  if(!*left) {
    tintin_puts2("#NO VARIABLE PARAMETER INCLUDED.  TRY AGAIN.", ses);
    prompt(ses);
  }
  else if(*left && !*right) {
    tintin_puts2("#NO RANGE INCLUDED.  TRY AGAIN.", ses);
    prompt(ses);
    }
  else {
    if((ln = searchnode_list(tempvars, left)))
      deletenode_list(tempvars, ln);
    j=1+(int)((strtod(right,NULL))*rand()/(RAND_MAX+1.0));
    sprintf(right,"%i",j);
    insertnode_list(tempvars, left, right, "0", ALPHA);
    varnum++;
    if(mesvar[5]) {
      sprintf(arg2, "#Ok. $%s is now set to {%s}.", left, right);
      tintin_puts2(arg2, ses);
    }
  }
}

/****************************/
/* the #setprompt command   */
/****************************/

void setprompt_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], result[BUFFER_SIZE];

  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);
  if(!*left) {
   if (prompt_on == 1) {
    tintin_puts2("#Defined prompt:", ses);
    tintin_puts2(prompt_line, ses);
    prompt(ses);
   }
  }
  else {
    sprintf(prompt_line, "%s", left);
    prompt_on = 1;
    if(mesvar[1]) {
      sprintf(result, "#Ok. {%s} will now be displayed on the status line ", left);
      tintin_puts2(result, ses);
    }
  }
}

