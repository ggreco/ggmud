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
/* file: session.c.c - funtions related to sessions                  */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

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
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef WIN32
#include <winsock2.h>
#define sockclose(x) closesocket(x)
#else
#define sockclose(x) close(x)
#endif

#include "include/llist.h"
#include "include/main.h"
#include "include/net.h"
#include "include/parse.h"
#include "include/session.h"
#include "include/utils.h"

/************************/
/* the #session command */
/************************/

struct session *session_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE];
  struct session *sesptr;

  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);

  if(!*left) {
    tintin_puts("#THESE SESSIONS HAS BEEN DEFINED:", ses);
    for(sesptr = sessionlist; sesptr; sesptr = sesptr->next)
      show_session(sesptr);
    prompt(ses);
  }
  else if(*left && !*right) {
    for(sesptr = sessionlist; sesptr; sesptr = sesptr->next)
      if(!strcmp(sesptr->name, left)) {
        show_session(sesptr);
        break;
      }
    if(!sesptr) {
      tintin_puts("#THAT SESSION IS NOT DEFINED.", ses);
      prompt(NULL);
    }
  }
  else {
    for(sesptr = sessionlist; sesptr; sesptr = sesptr->next)
      if(!strcmp(sesptr->name, left)) {
        tintin_puts("#THERE'S A SESSION WITH THAT NAME ALREADY.", ses);
        prompt(NULL);
        return(ses);
      }

    {
        char *port;
        char host[256];

        strcpy(host, right);

        if(!*host) {
            tintin_puts("#HEY! SPECIFY AN ADDRESS WILL YOU?", ses);
            return(ses);
        }

        port = host;

        while(*port && !isspace(*port))
            port++;
        *port++ = '\0';
        port = (char *)space_out(port);

        if(!*port) {
            tintin_puts("#HEY! SPECIFY A PORT NUMBER WILL YOU?", ses);
            return(ses);
        }
        make_connection(left, host, port);
    }
    ses = activesession;
  }

  return(ses);
}


/******************/
/* show a session */
/******************/

void show_session(struct session *ses)
{
  char temp[BUFFER_SIZE];

  sprintf(temp, "%-10s%s", ses->name, ses->address);

  if(ses == activesession) 
    strcat(temp, " (active)");
  if(ses->snoopstatus)
    strcat(temp, " (snooped)");
  if(ses->logfile)
    strcat(temp, " (logging)");
  if(ses->lowlogfile)  /* low-level logging - ycjhi */
    strcat(temp, " (low-logging)");
 tintin_puts2(temp, NULL); 
  prompt(NULL);
}

/**********************************/
/* find a new session to activate */
/**********************************/

struct session *newactive_session(void)
{
  if(sessionlist) {
    char buf[BUFFER_SIZE];

    activesession = sessionlist;
    sprintf(buf, "#SESSION '%s' ACTIVATED.", sessionlist->name);    
    tintin_puts(buf, NULL);
  }
  else {
    activesession = NULL;
    tintin_puts("#THERE'S NO ACTIVE SESSION NOW.", NULL);
  }
  prompt(NULL);
  return(sessionlist);
}

/**********************/
/* open a new session */
/**********************/

struct session *new_session(const char *name, const char *address,
			    struct session *ses)
{
  int i, sock, counter;
  char *host, *port;
  struct session *newsession;
  char buf[100];
  sock = 0;
  counter = 0;
  port = host = mystrdup(space_out(address));


  if(!*host) {
    tintin_puts("#HEY! SPECIFY AN ADDRESS WILL YOU?", ses);
    return(ses);
  }

  while(*port && !isspace(*port))
    port++;
  *port++ = '\0';
  port = (char *)space_out(port);

  if(!*port) {
    tintin_puts("#HEY! SPECIFY A PORT NUMBER WILL YOU?", ses);
    return(ses);
  }
  
  /* dsc : have tintin try to keep reconnecting for a set number of tries
     and a set number of time between retries. values are adjustable in
     tintin.h */
#if 0
  while (!sock)
    {
      if (counter == retries)
	break;

      counter++;

      sprintf(buf, "\n#Trying attempt %d out of %d attempts to connect.", 
	      counter, retries);
      tintin_puts(buf, ses);
      sock = connect_mud(host, port, ses);

      if (sock)
	break;

      if (counter != retries) {
	sprintf(buf, "\n#Will try to reconnect in %d seconds.", 
		time_before_try);
	tintin_puts(buf, ses);
	sleep(time_before_try);
      }
      else
	tintin_puts("#Could not connect.  Giving up.", ses);
    }

  if (!sock) {
    free(host);
    return(ses);
  }
  /*if(!(sock = connect_mud(host, port, ses))) {
    free(host);
    return(ses);
    }*/
  /*  free(host); */
#endif
  newsession = (struct session *)malloc(sizeof(struct session));
  newsession->name = mystrdup(name);
  newsession->address = mystrdup(address);
  newsession->tickstatus = FALSE;
  newsession->snoopstatus = FALSE;
  newsession->logfile = NULL;
  newsession->ignore = DEFAULT_IGNORE;
#if 0
  newsession->aliases = copy_list(common_aliases, ALPHA);
  newsession->actions = copy_list(common_actions, PRIORITY);
  newsession->subs = copy_list(common_subs, ALPHA);
  newsession->myvars = copy_list(common_myvars, ALPHA);
  newsession->highs = copy_list(common_highs, ALPHA);
  newsession->myfuncs = copy_list(common_functions, ALPHA);
  newsession->pathdirs = copy_list(common_pathdirs, ALPHA);
  newsession->antisubs = copy_list(common_antisubs, ALPHA);
#else
  newsession->aliases = common_aliases;
  newsession->actions = common_actions;
  newsession->subs = common_subs, ALPHA;
  newsession->myvars = common_myvars;
  newsession->highs = common_highs;
  newsession->myfuncs = common_functions;
  newsession->pathdirs = common_pathdirs;
  newsession->antisubs = common_antisubs;
#endif
  newsession->socket = sock;
  newsession->socketbit = 1<<sock;
  newsession->next = sessionlist;
  for(i = 0; i < HISTORY_SIZE; i++)
    newsession->history[i] = NULL;
  newsession->path = init_list();
  newsession->path_list_size = 0;
  newsession->path_length = 0;
  newsession->more_coming = 0;
  newsession->old_more_coming = 0;
  sessionlist = newsession;
  activesession = newsession;  
  sessionsstarted++;

  /* Added by ycjhi -------------- begin
     zombi mode support :
     save host address and port
  */
  strcpy(newsession->host, host);
  strcpy(newsession->port, port);
  /* initialize status */
  newsession->zombistat = ZOMBI_NOT; /* default status is normal session */

  /* slow-walk support : */
  memset(newsession->walk_path, 0, BUFFER_SIZE) ;
  memset(newsession->walk_index, 0, BUFFER_SIZE) ;
  newsession->walk_index[0] = -1 ;
  newsession->walk_now = 0 ;
  newsession->walk_standstill = FALSE ;
  newsession->walk_mode = WALKMODE_LOOP ;
  /* Added by ycjhi -------------- end */

  free(host) ; /* moved from above - ycjhi */

  newsession->lowlogfile = NULL;  /* low-level logging - ycjhi */

  return(newsession);
}

/*****************************************************************************/
/* cleanup after session died. if session=activesession, try find new active */
/*****************************************************************************/

void cleanup_session(struct session *ses)
{
  if(ses->zombistat == ZOMBI_NOT)
    cleanup_nonzombi_session(ses) ;
  else
    cleanup_zombi_session(ses) ;
}

void cleanup_nonzombi_session(struct session *ses)
{
  int i;
  char buf[BUFFER_SIZE];
  struct session *sesptr;

  sessionsstarted--;
  kill_all(ses, END);
  /* printf("DEBUG: Hist: %d \n\r",HISTORY_SIZE); */
  /* CHANGED to fix a possible memory leak
  for(i = 0; i < HISTORY_SIZE; i++)
    ses->history[i] = NULL;
  */
  for(i = 0; i < HISTORY_SIZE; i++)
    if(ses->history[i])
      free(ses->history[i]); 
  if(ses == sessionlist)
    sessionlist=ses->next;
  else {
    for(sesptr = sessionlist; sesptr->next != ses; sesptr = sesptr->next);
    sesptr->next = ses->next;
  }
  sprintf(buf, "#SESSION '%s' DIED.", ses->name);
  tintin_puts(buf, NULL);
/*  if(write(ses->socket, "ctld\n", 5) < 5)
    syserr("write in cleanup"); */    /* can't do this, cozof the peer stuff in net.c */
  if (ses->socket)
    if(sockclose(ses->socket) == -1)
      syserr("close in cleanup");

  if(ses->logfile)
    fclose(ses->logfile);

  if(ses->lowlogfile) /* lowlevel logging - ycjhi */
    fclose(ses->lowlogfile);

  free(ses->address);
  free(ses->name);
  free(ses);
}

/* changed cleanup_nonzombi_session to process a session in zombi mode */
void cleanup_zombi_session(struct session *ses)
{
  char buf[128];

  sprintf(buf, "#SESSION '%s' DIED IN ZOMBIMODE.", ses->name);
  tintin_puts2(buf, ses);

  ses->zombistat = ZOMBI_DISCONNECTED ;

  /* close socket */
  if(ses->socket)
  if(sockclose(ses->socket) == -1)
    syserr("close in cleanup");

  /* initialize socket descriptor for the next use. */
  ses->socket = 0 ;

  /* clean last_line buffer.
     I'm not sure but I think I should do it. */
  ses->last_line[0] = '\0' ;

  /* ticker seems to be taken care of but I didn't. */
  /* keep logging
  if (ses->logfile) {
    fclose(ses->logfile);
    ses->logfile = NULL ;
  }
  if (ses->lowlogfile) {
    fclose(ses->lowlogfile);
    ses->lowlogfile = NULL ;
  }
  */

  /* there's no need to free session because the session is not dead.
  free(ses);
  */
  term_echoing=1; /* restore term_echoing,
		     in case session closed while typing password */

  /* initialize some members of the session. */
  ses->tickstatus = FALSE;
  ses->snoopstatus = FALSE;
  ses->ignore = DEFAULT_IGNORE;
  ses->more_coming = 0;
  ses->old_more_coming = 0;

  /* I think that there could be still remaining problems but it works :) */
}

/* try to reconnect only one time.
   If session fails to reconnect, tintin will try to reconnect
   after processing other opened sockets' I/O requests.
   return value:
     on success - 0
     on failure - > 0
*/
int revive_zombi(struct session * ses)
{
  int sock ;

  ses->zombistat = ZOMBI_CONNECTING;
  if(!(sock=connect_mud(ses->host, ses->port, ses))) {
    ses->zombistat = ZOMBI_DISCONNECTED;
    return 1 ;
  }

  /* initialize members of the session if needed. */
  ses->socket = sock;

  /* set current status of the session as
     "connected zombi mode session." */
  ses->zombistat = ZOMBI_CONNECTED;
  return 0 ;
}


/* #zombion command turns a normal session into a zombi session. */
void zombion_command(struct session * ses)
{
  char buf[128];

  if(!ses) {
    tintin_puts2("#NO SESSION ACTIVE.", ses);
    return;
  }

  if(ses->zombistat == ZOMBI_NOT)
    ses->zombistat = ZOMBI_CONNECTED;

  sprintf(buf, "#ZOMBI MODE ON:[SESSION '%s']", ses->name);
  tintin_puts2(buf, NULL);
}

/* #zombioff command turns a zombi session into a normal session. */
void zombioff_command(struct session * ses)
{
  char buf[128];

  if(!ses) {
    tintin_puts2("#NO SESSION ACTIVE.", ses);
    return;
  }

  if(ses->zombistat != ZOMBI_NOT)
    ses->zombistat = ZOMBI_NOT;

  sprintf(buf, "#ZOMBI MODE OFF:[SESSION '%s']", ses->name);
  tintin_puts2(buf, NULL);
}
