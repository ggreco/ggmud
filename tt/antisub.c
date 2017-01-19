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
/* file: antisub.c - functions related to the substitute command     */
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

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "include/action.h"
#include "include/antisub.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/parse.h"
#include "include/rl.h"

/*******************************/
/* the #antisubstitute command */
/*******************************/

void parse_antisub(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], result[BUFFER_SIZE];
  struct listnode *myantisubs, *ln;

  myantisubs = (ses ? ses->antisubs : common_antisubs);
  arg = get_arg_in_braces(arg, left, 1);

  if(!*left) {
    tintin_puts("#THESE ANTISUBSTITUTES HAS BEEN DEFINED:", ses);
    show_list(myantisubs);
    prompt(ses);
  }
  else {
    if((ln = searchnode_list(myantisubs, left)))
      deletenode_list(myantisubs, ln);
    insertnode_list(myantisubs, left, left, "0", ALPHA);
    antisubnum++;
    if(mesvar[3]) {
      sprintf(result, "#Ok. Any line with {%s} will not be subbed.", left);
      tintin_puts2(result, ses);
    }
  }
}

/*********************************/
/* the #unantisubstitute command */
/*********************************/

void unantisubstitute_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], result[BUFFER_SIZE];
  struct listnode *myantisubs, *ln;
  int flag = FALSE;

  myantisubs = (ses ? ses->antisubs : common_antisubs);
  arg = get_arg_in_braces(arg, left, 1);
  while((ln = search_node_with_wild(myantisubs, left))) {
    if(mesvar[3]) {
      sprintf(result, "#Ok. Lines with {%s} will now be subbed.", ln->left);
      tintin_puts2(result, ses);
    }
    deletenode_list(myantisubs, ln);
    flag = TRUE;
  }

  if(!flag && mesvar[3])
    tintin_puts2("#THAT ANTISUBSTITUTE IS NOT DEFINED.", ses);
}


int do_one_antisub(const char *line, struct session *ses)
{
  struct listnode *ln = ses->antisubs;

  while((ln = ln->next)) 
    if(check_one_action(line, ln->left, ses))
      return(TRUE);

  return(FALSE);
}
