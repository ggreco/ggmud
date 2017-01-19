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
/* file: ticks.c - functions for the ticker stuff                    */
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
#include <signal.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#ifndef BADSIG
#define BADSIG (RETSIGTYPE (*)(int))-1
#endif

#include "include/ticks.h"

/* local globals */
int time0 = -1, tick_size = 75;

/*********************/
/* the #tick command */
/*********************/

void tick_command(struct session *ses)
{
  if(ses) {
    if(ses->tickstatus) {
      char buf[100];

      sprintf(buf, "#THERE'S ABOUT %d SECONDS TO THE TICK.", timetilltick());
      tintin_puts(buf, ses);
    }
    else
      tintin_puts("#MY TICKER IS OFF! DUNNO SECONDS TO TICK", ses);
  } else
    tintin_puts("#NO SESSION ACTIVE => NO TICKER!", ses);
}

/************************/
/* the #tickoff command */
/************************/

void tickoff_command(struct session *ses)
{
  if(ses) {
    ses->tickstatus = FALSE;
    tintin_puts("#TICKER IS NOW OFF.", ses);
  }
  else
   tintin_puts("#NO SESSION ACTIVE => NO TICKER!", ses);
}

/***********************/
/* the #tickon command */
/***********************/

void tickon_command(struct session *ses)
{
  if(ses) {
    ses->tickstatus = TRUE;
    if(time0 < 0)
      time0 = time(NULL);
    tintin_puts("#TICKER IS NOW ON.", ses);
  }
  else
    tintin_puts("#NO SESSION ACTIVE => NO TICKER!", ses);
}

/************************/
/* the #tickset command */
/************************/

void tickset_command(struct session *ses)
{
  if(ses) {
    ses->tickstatus = TRUE;
    time0 = time(NULL); /* we don't prompt! too many ticksets... */
  }
  else
    tintin_puts("#NO SESSION ACTIVE => NO TICKER!", ses);
}

/*************************/
/* the #ticksize command */
/*************************/

void ticksize_command(const char *arg, struct session *ses)
{
  if(*arg) {
    if(isdigit(*arg)) {
      /* DSC : make sure the tick is between 0 and 1000.  The is no reason
	 for 1000, other than it's a nice round number.  Muds usually are
	 around 75 seconds per tick.
      */
      if ((atoi(arg) <= 0) || (atoi(arg) > 1000))
	{
	  tintin_puts("#SPECIFY A number between 1 and 1000", ses);
	  return;
	}
      tick_size = atoi(arg);
      time0 = time(NULL);
      tintin_puts("#OK NEW TICKSIZE SET", ses);        
    }
    else
      tintin_puts("#SPECIFY A NUMBER!!!! TRYING TO CRASH ME, EH?", ses);
  } else
    tintin_puts("#SET THE TICK-SIZE TO WHAT?", ses);
}     
