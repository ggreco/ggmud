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
/* file: path.c - stuff for the path feature                         */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                    coded by peter unold 1992                      */
/*                  recoded by Jeremy C. Jack 1994                   */
/*********************************************************************/
/* the path is implemented as a fix-sized queue. It gets a bit messy */
/* here and there, but it should work....                            */
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

#include "include/llist.h"
#include "include/main.h"
#include "include/parse.h"
#include "include/path.h"
#include "include/rl.h"

static int return_flag = TRUE;

/*********************/
/* the #mark command */
/*********************/

void mark_command(struct session *ses)
{
  if(ses) {
    kill_list(ses->path);
    ses->path = init_list();
    ses->path_length = 0;
    tintin_puts("#Beginning of path marked.", ses);
  }
  else
    tintin_puts("#No session active => NO PATH!", ses);
}

/********************/
/* the #map command */
/********************/

void map_command(const char *arg, struct session *ses)
{
  if(ses) {
    char left[BUFFER_SIZE];

    get_arg_in_braces(arg, left, 1);
    check_insert_path(left, ses);
  }
  else
    tintin_puts2("#No session active => NO PATH!", ses);
}

/*************************/
/* the #savepath command */
/*************************/

void savepath_command(const char *arg, struct session *ses)
{
  if(ses) {
    char left[BUFFER_SIZE];

    get_arg_in_braces(arg, left, 1);
    if(*left && ses->path_length) {
      char result[BUFFER_SIZE];
      struct listnode *ln = ses->path;
      int dirlen, len = 0;

      sprintf(result, "%calias {%s} {", tintin_char, left);
      len = strlen(result);
      while((ln = ln->next)) {
        dirlen = strlen(ln->left);
	if(dirlen+len < BUFFER_SIZE) {
          strcat(result, ln->left);
	  len += dirlen+1;
          if(ln->next)
	    strcat(result, ";");
        }
	else {
	  tintin_puts("#Error - buffer  too small to contain alias", ses);
	  break;
        }
      }
      strcat(result, "}");
      parse_input(result, ses);
    }
    else
      tintin_puts("#Error: no alias for savepath or no path", ses);
  }
  else
    tintin_puts2("#No session active => NO PATH TO SAVE!", ses);
}

/*********************/
/* the #path command */
/*********************/

void path_command(struct session *ses)
{
  if(ses) {
    int len = 0, dirlen;
    struct listnode *ln = ses->path;
    char mypath[81];

    sprintf(mypath, "%s", "#Path:  ");
    while((ln = ln->next)) {
      dirlen = strlen(ln->left);
      if(dirlen+len > 70) {
	tintin_puts(mypath, ses);
        sprintf(mypath, "%s", "#Path:  ");
        len = 0;
      }
      strcat(mypath, ln->left);
      strcat(mypath, " ");
      len += dirlen+1;
    }
    tintin_puts(mypath, ses);
  }
  else
    tintin_puts("#No session active => NO PATH!", ses);
}

/***********************/
/* the #return command */
/***********************/

void return_command(struct session *ses)
{
  if(ses) {
    if(ses->path_length) {
      struct listnode *ln = ses->path;
      char command[BUFFER_SIZE];

      ses->path_length--;
      while(ln->next)
	ln = ln->next;
      sprintf(command, "%s", ln->right);
      return_flag = FALSE;	/* temporarily turn off path tracking */
      parse_input(command, ses);
      return_flag = TRUE;	/* restore path tracking */
      deletenode_list(ses->path, ln);
    }
    else
      tintin_puts("#No place to return from!", ses);
  } else
    tintin_puts("#No session active => NO PATH!", ses);
}

/***********************/
/* the #unpath command */
/***********************/

void unpath_command(struct session *ses)
{
  if(ses) {
    if(ses->path_length) {
      struct listnode *ln = ses->path;

      ses->path_length--;
      while(ln->next)
	ln = ln->next;
      deletenode_list(ses->path, ln);
      tintin_puts("#Ok.  Forgot that move.", ses);
    }
    else
      tintin_puts("#No move to forget!", ses);
  } else
    tintin_puts("#No session active => NO PATH!", ses);
}

/************************/
/* the #pathdir command */
/************************/

void pathdir_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], arg2[BUFFER_SIZE];
  struct listnode *mypathdirs, *ln;

  mypathdirs = (ses ? ses->pathdirs : common_pathdirs);
  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);

  if(!*left) {
    tintin_puts2("#These PATHDIRS have been defined:", ses);
    show_list(mypathdirs);
    prompt(ses);
  }
  else if(*left && !*right) {
    if((ln = search_node_with_wild(mypathdirs, left))) {
      while((mypathdirs = search_node_with_wild(mypathdirs, left)))
        shownode_list(mypathdirs);
      prompt(ses);
    }
    else if(mesvar[6])
      tintin_puts2("#That PATHDIR is not defined.", ses);
  }
  else {
    if((ln = searchnode_list(mypathdirs, left)))
      deletenode_list(mypathdirs, ln);
    insertnode_list(mypathdirs, left, right, "0", ALPHA);
    if(mesvar[6]) {
      sprintf(arg2, "#Ok.  {%s} is marked in #path. {%s} is it's #return.",
	      left, right);
      tintin_puts2(arg2, ses);
    }
    pdnum++;
  }
}

void check_insert_path(const char *command, struct session *ses)
{
  struct listnode *ln;

  if(!return_flag)
    return;

  if((ln = searchnode_list(ses->pathdirs, command))) {
    if(ses->path_length != MAX_PATH_LENGTH)
      ses->path_length++;
    else if(ses->path_length)
      deletenode_list(ses->path, ses->path->next);
    addnode_list(ses->path, ln->left, ln->right, "0");
  }
}
