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
/* file: action.c - funtions related to the action command           */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

#include "config.h"
#include "tintin.h"

#include <string.h>
#include <stdlib.h>

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#include "include/action.h"
#include "include/ansi.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/parse.h"
#include "include/rl.h"
#include "include/variables.h"

int use_triggers = 1;

static int var_len[10];
static const char *var_ptr[10];

trigger_class *trigger_classes = NULL;
extern char lua_char;

/***********************/
/* the #action command */
/***********************/

/*  Priority code added by Robert Ellsworth 2/2/94 */
/* Priority code removed by Gabriele Greco and substituted
   with more useful trigger class code.
 */


static trigger_class * add_class(const char *name)
{
    trigger_class *class = malloc(sizeof(trigger_class));

    if (class) {
        class->name = strdup(name);
        class->how_many = 0;
        class->next = trigger_classes;
        class->enabled = 1;
    
        trigger_classes = class;
    }

    return class;
}

static trigger_class * get_class(const char *name)
{
    trigger_class *t = trigger_classes;
    
    while (t) {
        if (!strcmp(t->name, name))
            return t;

        t = t->next;
    }
    
    return NULL;
}

static int enable_class(trigger_class *cl, int enable, struct session *ses)
{
     struct listnode *myactions = (ses ? ses->actions : common_actions);

     cl->enabled = enable;

     while ((myactions = myactions->next)) {
         if (!myactions->pr)
             continue;

         if (!strcmp(myactions -> pr, cl->name))
             myactions-> enabled = enable;
     }
}


void action_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], result[BUFFER_SIZE];
  char pr[BUFFER_SIZE];
  struct listnode *myactions, *ln;
  trigger_class *cl;
 
  while (*arg == ' ')
      arg++;
  
  if (*arg == '+' || *arg == '-') {
      trigger_class *t;
      char *c = left, d = *arg++;
      
      while (*arg > ' ')
        *c++ = *arg++;    
      
      *c = 0;

      if (!(t = get_class(left))) {
          sprintf(result, "#Undefined trigger class [%s]\n", left);
          tintin_puts2(result, ses);
      }
      else {
          enable_class(t, (d == '+'), ses);
          sprintf(result, "#%s trigger class [%s]\n", 
                  d == '+' ? "Enabled" : "Disabled", left);
      }
      return;
  }
  
  myactions = (ses ? ses->actions : common_actions);
  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);
  arg = get_arg_in_braces(arg, pr, 1); // pr now is used as trigger class

  if(!*pr)
    strcpy(pr, "undefined"); 

// if a class is not available I add it and enable it.
  if (!(cl = get_class(pr)))
      cl = add_class(pr);

  cl->how_many++;
  
  if(!*left) {
    tintin_puts2("#Defined actions:", ses);
    show_list_action(myactions);
    prompt(ses);
  }
  else if(*left && !*right) {
    if((ln = search_node_with_wild(myactions,left))) {
      while((myactions = search_node_with_wild(myactions, left))) {
        shownode_list_action(myactions);
      }
      prompt(ses);
    }
    else
      if(mesvar[1])
        tintin_puts("#That action is not defined.", ses);
  }
  else {
    if((ln = searchnode_list(myactions, left)))
      deletenode_list(myactions, ln);
    insertnode_list(myactions, left, right, pr, PRIORITY);
    if(mesvar[1]) {
      sprintf(result, "#Ok. {%s} now triggers {%s} @ {%s}", left, right, pr);
      tintin_puts2(result, ses);
    }
    acnum++;
  }
}

/*************************/
/* the #unaction command */
/*************************/

void unaction_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], result[BUFFER_SIZE];
  struct listnode *myactions, *ln;
  int flag = FALSE;

  myactions = (ses ? ses->actions : common_actions);
  arg = get_arg_in_braces(arg, left, 1);
  while((ln = search_node_with_wild(myactions, left))) {
    if(mesvar[1]) {
      sprintf(result, "#Ok. {%s} is no longer a trigger.", ln->left);
      tintin_puts2(result, ses);
    }
    deletenode_list(myactions, ln);
    flag = TRUE;
  }

  if(!flag && mesvar[1]) {
    sprintf(result, "#No match(es) found for {%s}", left);
    tintin_puts2(result, ses);
  }
}

/**************************************************************************/
/* run throught each of the commands on the right side of an alias/action */
/* expression, call substitute_text() for all commands but #alias/#action */
/**************************************************************************/

void prepare_actionalias(const char *string, char *result, struct session *ses)
{
  char arg[BUFFER_SIZE];

  *result = '\0';
  substitute_vars(string, arg);
  substitute_myvars(arg, result, ses);
}

/*************************************************************************/
/* copy the arg text into the result-space, but substitute the variables */
/* %0..%9 with the real variables                                        */
/*************************************************************************/

void substitute_vars(const char *arg, char *result)
{
  int nest = 0, numands, n;
  char *ptr;

  while(*arg) {
    if(*arg == '%') { /* substitute variable */
      numands = 0;
      while(arg[numands] == '%')
        numands++;
      if(isdigit(arg[numands]) && numands == nest+1) {
         n = arg[numands]-'0';
         sprintf(result, "%s", vars[n]);
         arg += numands+1;
         result += strlen(vars[n]);
      }
      else {
        memcpy(result, arg, numands+1);
        arg += numands+1;
        result += numands+1;
      }
    } else
    if(*arg == '$') { /* substitute variable */
      numands = 0;
      while(arg[numands] == '$') 
        numands++;
      if(isdigit(arg[numands]) && numands == nest+1) {
         n = arg[numands]-'0';
         ptr = vars[n];
         while(*ptr)
           if(*ptr == ';')
             ptr++;
           else
             *result++ = *ptr++;
         arg += numands+1;
      }
      else {
        memcpy(result, arg, numands);
        arg += numands;
        result += numands;
      }
    }
#ifdef BIG5
    /* for Big5 encoding */
    else if(*arg & 0x80) {
      *result++ = *arg++;
      if(*arg)
        *result++ = *arg++;
    }
#endif
    else if(*arg == DEFAULT_OPEN) {
      nest++;
      *result++ = *arg++;
    }
    else if(*arg == DEFAULT_CLOSE) {
      nest--;
      *result++ = *arg++;
    }
    else if(*arg == '\\' && !nest) {
      while(*arg == '\\')
        *result++ = *arg++;
      if(*arg == '%') {
        result--;
        *result++ = *arg++;
	*result++ = *arg++;
      }
    }
    else if(*arg)
      *result++ = *arg++;
  }
  *result = '\0';
}

/**********************************************/
/* check actions from a sessions against line */
/**********************************************/

void check_all_actions(const char *line, struct session *ses)
{
  struct listnode *ln;
  static char temp[BUFFER_SIZE] = PROMPT_FOR_PW_TEXT;
  char linebuf[BUFFER_SIZE];

  if (!use_triggers)
      return;

  strip_ansi(line, linebuf);
 
  
  if(check_one_action(linebuf, temp, ses) && ses == activesession) {
    term_echoing = FALSE;
  }

  ln = (ses ? ses->actions : common_actions);
  while((ln = ln->next)) {
    if (!ln->enabled)
        continue;

    if(check_one_action(linebuf, ln->left, ses)) {
      char buffer[BUFFER_SIZE], strng[BUFFER_SIZE];

#ifdef WITH_LUA
      if (*ln->right == lua_char) {
          execute_luatrigger(ln->right + 1, var_ptr, var_len);
      }
      else
#endif
      {
          prepare_actionalias(ln->right, buffer, ses);
          if(echo && activesession == ses) { 
              sprintf(strng, "[ACTION: %s]", buffer);
              tintin_puts2(strng, ses);
          }
          parse_input(buffer, ses);
      }
//      return; this prevent the use of multiple actions on a single line
    }
  }
}

int match_a_string(const char *line, const char *mask)
{
  const char *lptr = line;

  while(*lptr && *mask && (*mask != '%' || !isdigit(mask[1])))
    if(*lptr++ != *mask++)
      return(-1);

  if(!*mask || (*mask == '%' && isdigit(mask[1])))
    return((int)(lptr-line));

  return(-1);
}

int check_one_action(const char *line, const char *action, struct session *ses)
{
  int i; 

  if(check_a_action(line, action, ses)) {
    for(i = 0; i < 10; i++)
      if(var_len[i] != -1) {
        memcpy(vars[i], var_ptr[i], var_len[i]);
        vars[i][var_len[i]] = '\0'; 
      }
    return(TRUE);
  }
  else
    return(FALSE);
}

/******************************************************************/
/* check if a text triggers an action and fill into the variables */
/* return TRUE if triggered                                       */
/******************************************************************/

int check_a_action(const char *line, const char *action, struct session *ses)
{   
  char result[BUFFER_SIZE];
  char *temp2, *tptr = result;
  const char *line2;
  int  i, len, flag;

  for(i = 0; i < 10; i++)
    var_len[i] = -1;
  substitute_myvars(action, result, ses);

  if(*tptr == '^') {
    if((len = match_a_string(line, ++tptr)) == -1)
      return(FALSE);
    line += len;
    tptr += len;
  }
  else {
    flag = TRUE;
    len = -1;
    while(*line && flag)
      if((len = match_a_string(line, tptr)) != -1)
        flag = FALSE;
      else
        line++;

    if(len != -1) {
      line += len;
      tptr += len;
    }
    else
      return(FALSE);
  }

  while(*line && *tptr) {
    temp2 = tptr+2;
    if(!*temp2) {
      var_len[tptr[1]-'0'] = strlen(line);
      var_ptr[tptr[1]-'0'] = line;
      return(TRUE);
    }

    line2 = line;
    flag = TRUE;
    len = -1;

    while(*line2 && flag)
      if((len = match_a_string(line2, temp2)) != -1)
        flag = FALSE;
      else 
        line2++;

    if(len != -1) {
      var_len[tptr[1]-'0'] = line2-line;
      var_ptr[tptr[1]-'0'] = line;
      line = line2+len;
      tptr = temp2+len;
    }
    else
      return(FALSE);
  }

  return(*tptr ? FALSE : TRUE);
}       
