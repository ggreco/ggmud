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
/* file: substitute.c - functions related to the substitute command  */
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

#include "include/action.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/parse.h"
#include "include/rl.h"

/***************************/
/* the #substitute command */
/***************************/

void parse_sub(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], result[BUFFER_SIZE];
  struct listnode *mysubs, *ln;

  mysubs = (ses ? ses->subs : common_subs);
  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);

  if(!*left) {
    tintin_puts2("#THESE SUBSTITUTES HAVE BEEN DEFINED:", ses);
    show_list(mysubs);
    prompt(ses);
  }
  else if(*left && !*right) {
    if((ln = search_node_with_wild(mysubs, left))) {
      while((mysubs = search_node_with_wild(mysubs, left)))
        shownode_list(mysubs);
      prompt(ses);
    }
    else if(mesvar[2])
      tintin_puts2("#THAT SUBSTITUTE IS NOT DEFINED.", ses);
  }
  else {
    if((ln = searchnode_list(mysubs, left)))
      deletenode_list(mysubs, ln);
    insertnode_list(mysubs, left, right, "0", ALPHA);
    subnum++;
    if(strcmp(right, "."))
      sprintf(result, "#Ok. {%s} now replaces {%s}.", right, left);
    else
      sprintf(result, "#Ok. {%s} is now gagged.", left);
    if(mesvar[2])
      tintin_puts2(result, ses);
  }
}

/*****************************/
/* the #unsubstitute command */
/*****************************/

void unsubstitute_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], result[BUFFER_SIZE];
  struct listnode *mysubs, *ln;
  int flag = FALSE;

  mysubs = (ses ? ses->subs : common_subs);
  arg = get_arg_in_braces(arg, left, 1);
  while((ln = search_node_with_wild(mysubs, left))) {
    if(mesvar[2]) {
      if(*ln->right == '.' && !ln->right[1])
        sprintf(result, "#Ok. {%s} is no longer gagged.", ln->left);
      else
        sprintf(result, "#Ok. {%s} is no longer substituted.", ln->left);
      tintin_puts2(result, ses);
    }
    deletenode_list(mysubs, ln);
    flag = TRUE;
  }

  if(!flag && mesvar[2])
    tintin_puts2("#THAT SUBSTITUTE IS NOT DEFINED.", ses);
}


void do_one_sub(char *line, struct session *ses)
{
  struct listnode *ln = ses->subs;

  while((ln = ln->next)) 
    if(check_one_action(line, ln->left, ses))
      prepare_actionalias(ln->right, line, ses);
}
