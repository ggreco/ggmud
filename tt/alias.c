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
/* file: alias.c - funtions related the the alias command            */
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

#include "include/alias.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/parse.h"
#include "include/rl.h"
#include <ctype.h>

/**********************/
/* the #alias command */
/**********************/

void alias_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], arg2[BUFFER_SIZE];
  struct listnode *myaliases, *ln;
   
  myaliases = (ses ? ses->aliases : common_aliases);
  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);

  if(!*left) {
    tintin_puts2("#Defined aliases:", ses);
    show_list(myaliases);
    prompt(ses);
  }
  else if(*left && !*right) {
    if((ln = search_node_with_wild(myaliases, left))) {
      while((myaliases = search_node_with_wild(myaliases, left)))
        shownode_list(myaliases);
      prompt(ses);
    }
    else if(mesvar[0]) {
      sprintf(right, "#No match(es) found for {%s}", left);
      tintin_puts2(right, ses);
    }
  }
  else {
    if((ln = searchnode_list(myaliases, left)))
      deletenode_list(myaliases, ln);
    insertnode_list(myaliases, left, right, "0", ALPHA);
    if(mesvar[0]) {
      sprintf(arg2, "#Ok. {%s} aliases {%s}.", left, right);
      tintin_puts2(arg2, ses);
    }
    alnum++;
  }
}

/************************/
/* the #unalias command */
/************************/

void unalias_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], result[BUFFER_SIZE];
  struct listnode *myaliases, *ln;
  int flag = FALSE;

  myaliases = (ses ? ses->aliases : common_aliases);
  arg = get_arg_in_braces(arg, left, 1);
  while((ln = search_node_with_wild(myaliases, left))) {
    if(mesvar[0]) {
      sprintf(result, "#Ok. {%s} is no longer an alias.", ln->left);
      tintin_puts2(result, ses);
    }
    deletenode_list(myaliases, ln);
    flag = TRUE;
  }

  if(!flag && mesvar[0]) {
    sprintf(result, "#No match(es) found for {%s}", left);
    tintin_puts2(result, ses);
  }    
}
