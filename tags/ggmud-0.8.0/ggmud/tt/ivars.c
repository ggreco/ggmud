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

#include "config.h"
#include "tintin.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "include/action.h"
#include "include/ivars.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/parse.h"
#include "include/rl.h"
#include "include/variables.h"
#include "include/glob.h"
#include "include/lua.h"

int stacks[100][4];

/*********************/
/* the #math command */
/*********************/

void math_command(const char *line, struct session *ses)
{
    char left[BUFFER_SIZE], right[BUFFER_SIZE];
    char temp[BUFFER_SIZE], result[BUFFER_SIZE];
    struct listnode *my_vars, *ln;
    int i;
    int status = 1;
    my_vars = (ses ? ses->myvars : common_myvars);
    line = get_arg_in_braces(line, left, 0);
    line = get_arg_in_braces(line, right, 1);
    substitute_vars(right, result);
    substitute_myvars(result, right, ses);
    i = eval_expression(right, &status);
    if (fixed_math == 1) {
        if (status == 1) {
            sprintf(temp, "%d", i);
            if((ln = searchnode_list(my_vars,left)))
                deletenode_list(my_vars, ln);
            ln = insertnode_list(my_vars, left, temp, "0", ALPHA); 
#ifdef WITH_LUA
            add_lua_global(ln->left, &(ln->right));
#endif
            varnum++;
            if(mesvar[5]) {
                sprintf(result, "#Ok. $%s is now set to {%s}.", left, temp);
                tintin_puts2(result, ses);
            }
        }
    } else {
        sprintf(temp, "%d", i);
        if((ln = searchnode_list(my_vars,left)))
            deletenode_list(my_vars, ln);
        ln = insertnode_list(my_vars, left, temp, "0", ALPHA); 
#ifdef WITH_LUA
        add_lua_global(ln->left, &(ln->right));
#endif
        varnum++;
        if(mesvar[5]) {
            sprintf(result, "#Ok. $%s is now set to {%s}.", left, temp);
            tintin_puts2(result, ses);
        }
    }
}

/*******************/
/* the #if command */
/*******************/

void if_command(const char *line, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE];
  char temp[BUFFER_SIZE], elsebuf[BUFFER_SIZE];
  int garbage;

  line = get_arg_in_braces(line, left, 0);
  line = get_arg_in_braces(line, right, 1);
  line = get_arg_in_braces(line, elsebuf, 1);
  substitute_vars(left, temp);
  substitute_myvars(temp, left, ses);  
  if(eval_expression(left, &garbage)) {
    substitute_vars(right, temp);
    substitute_myvars(temp, right, ses);
    parse_input(right, ses); 
  }
  else if(*elsebuf) {
    substitute_vars(elsebuf, temp);
    substitute_myvars(temp, elsebuf, ses);
    parse_input(elsebuf, ses);
  }
}

/***************************/
/* the #ifstrequal command */
/***************************/

void ifstrequal_command(const char *line, struct session *ses)
{
  char str1[BUFFER_SIZE], dothis[BUFFER_SIZE];
  char str2[BUFFER_SIZE], temp[BUFFER_SIZE], elsebuf[BUFFER_SIZE];

  line = get_arg_in_braces(line, str1, 0);
  line = get_arg_in_braces(line, str2, 1);
  line = get_arg_in_braces(line, dothis, 1);
  line = get_arg_in_braces(line, elsebuf, 1);

if ((*str1) && (*str2) && (*dothis)) {
  substitute_vars(str1, temp);
  substitute_myvars(temp, str1, ses);  
  substitute_vars(str2, temp);
  substitute_myvars(temp, str2, ses);  
  /* sprintf(temp,"Str1: %s  Str2: %s",str1, str2);
     tintin_puts2(temp, ses); */
  if (strcmp(str1,str2)==0) {
    substitute_vars(dothis, temp);
    substitute_myvars(temp, dothis, ses);
    parse_input(dothis, ses); 
    } else if(*elsebuf) {
    substitute_vars(elsebuf, temp);
    substitute_myvars(temp, elsebuf, ses);
    parse_input(elsebuf, ses); 
    }
  } else
    tintin_puts2("#NOT ENOUGH ARGUMENTS!!!", ses);
} 


void ifexist_command(const char *line, struct session *ses)
{
  char varname[BUFFER_SIZE], dothis[BUFFER_SIZE];
  char temp[BUFFER_SIZE], elsebuf[BUFFER_SIZE];
  struct listnode *my_vars, *ln;

  line = get_arg_in_braces(line,  varname, 0);
  line = get_arg_in_braces(line, dothis, 1);
  line = get_arg_in_braces(line, elsebuf, 1);

  my_vars = (ses ? ses->myvars : common_myvars);
 
  if ((*varname) && (*dothis)) {
    if((ln = searchnode_list(my_vars,varname))) {
      substitute_vars(dothis, temp);
      substitute_myvars(temp, dothis, ses);
      parse_input(dothis, ses); 
      } else if(*elsebuf) {
      substitute_vars(elsebuf, temp);
      substitute_myvars(temp, elsebuf, ses);
      parse_input(elsebuf, ses); 
      }
  } else
    tintin_puts2("#NOT ENOUGH ARGUMENTS!!!", ses);
} 

void revstring_command(const char *line, struct session *ses)
{
    char left[BUFFER_SIZE], result[BUFFER_SIZE];
    char string[BUFFER_SIZE], temp[BUFFER_SIZE];
    struct listnode *my_vars, *ln;
    int i;

    my_vars = (ses ? ses->myvars : common_myvars);
    line = get_arg_in_braces(line, left, 0);
    line = get_arg_in_braces(line, string, 1);

    if ((*left) && (*string)) {
        for(i=(int)strlen(string)-1; i>=0; i--)
            temp[strlen(string)-i-1] = string[i];
        temp[strlen(string)] = '\0';
        if((ln = searchnode_list(my_vars,left)))
            deletenode_list(my_vars, ln);
        ln = insertnode_list(my_vars, left, temp, "0", ALPHA); 
#ifdef WITH_LUA
        add_lua_global(ln->left, &(ln->right));
#endif
        varnum++;
        if(mesvar[5]) {
            sprintf(result, "#Ok. $%s is now set to {%s}.", left, temp);
            tintin_puts2(result, ses);
        } else
            tintin_puts2("#NOT ENOUGH ARGUMENTS!!!", ses);
    }
} 

int eval_expression(const char *arg, int *exitstatus)
{
  int i, begin, end, flag, prev;
  char temp[BUFFER_SIZE];

  i = conv_to_ints(arg);
  if(i == 1) { /* changed for if strings */
    while(1) {
      i = 0;
      flag = 1;
      begin = -1;
      end = -1;
      prev = -1;
      while(stacks[i][0] && flag) {
        if(!stacks[i][1])
          begin = i;
        else if(stacks[i][1]==1) {
          end = i;
          flag = 0;
        }
        prev = i;
        i = stacks[i][0];
      }
      if((flag && begin != -1) || (!flag && begin == -1)) {
        tintin_puts2("#Unmatched parentheses error.", NULL);
        return(0);
      }
      if(flag) {
        if(prev == -1)
          return(stacks[0][2]);
        begin = -1;
        end = i;
      }
      i = do_one_inside(begin, end);
      if(!i) {
        sprintf(temp, "#Invalid expression to evaluate in {%s}", arg);
        tintin_puts2(temp, NULL);
        return(0);
      }
    }
  }
  else if(i==2)
    return(1); /* changed for #if strings */
  else 
    {
      *(int *) exitstatus = 0;
      return(0);
    }
}

int conv_to_ints(const char *arg)
{
  int i = 0, flag;
  const char *ptr = arg, *tptr2;
  char *tptr, left[256], right[256];

  while(*ptr) {
    if(*ptr == ' ')
      ;

    /* START ADDITION for #if-strings */
    else if(*ptr == '[') {
      ptr++;
      tptr = left;
      while(*ptr && *ptr != ']' && *ptr != '=') {
	*tptr = *ptr;
	ptr++;
	tptr++;
      }
      *tptr = '\0';
      if(!*ptr)
	return(-1);
      if(*ptr == ']')
	return(-2);
      tptr = right;
      ptr++;
      while(*ptr && *ptr != ']') {
	*tptr = *ptr;
	ptr++;
	tptr++;
      }
      *tptr = '\0';
      if(!*ptr)
	return(-1);
      if(!strcmp(right,left))
	return(2);
      return(3);
    }
    /* END ADDITION for #if-strings */

    else if(*ptr == '(')
      stacks[i][1] = 0;
    else if(*ptr == ')')
      stacks[i][1] = 1;
    else if(*ptr == '!') {
      if(ptr[1] == '=') { 
        stacks[i][1] = 12;
        ptr++;
      }
      else
        stacks[i][1] = 2;
    } else if(*ptr == '*') {
      stacks[i][1] = 3;
      stacks[i][3] = 1;
    }
    else if(*ptr == '/') {
      if (fixed_math == 1)
	stacks[i][1] = 3;
      else
	stacks[i][1] = 4;
      stacks[i][3] = 0;
    }
    else if(*ptr == '+') {
      stacks[i][1] = 5;
      stacks[i][3] = 1; /* 1 is addition */
    }
    else if(*ptr == '-') {
      flag = -1;
      if(i > 0)
        flag = stacks[i-1][1];
      if(flag == 15) {
	if (fixed_math == 1) {
	    stacks[i][1] = 5;
	  } else {
	  stacks[i][1] = 6;
	  }
	stacks[i][3] = 0; /* 0 is substraction */
      }
      else {
        tptr2 = ptr;
        ptr++;
        while(isdigit(*ptr))
          ptr++;
        sscanf(tptr2, "%d", &stacks[i][2]);
        stacks[i][1] = 15;
        ptr--;
      }
    }
    else if(*ptr == '>') {
      if(ptr[1] == '=') {
        stacks[i][1] = 8;
        ptr++;
      }
      else
        stacks[i][1] = 7;
    } else if(*ptr == '<') {
      if(ptr[1] == '=') {
        ptr++;
        stacks[i][1] = 10;
      }
      else
        stacks[i][1]=9;
    } else if(*ptr == '=') {
      stacks[i][1] = 11;
      if(ptr[1] == '=')
        ptr++;
    } else if(*ptr == '&') {
      stacks[i][1] = 13;
      if(ptr[1] == '&')
        ptr++;
    } else if(*ptr == '|') {
      stacks[i][1] = 14;
      if(ptr[1] == '|')
        ptr++;
    }
    else if(isdigit(*ptr)) {
      stacks[i][1] = 15;
      tptr2 = ptr;
      while(isdigit(*ptr))
        ptr++;
      sscanf(tptr2, "%d", &stacks[i][2]);
      ptr--;
    }
    else if(*ptr == 'T') {
      stacks[i][1] = 15;
      stacks[i][2] = 1;
    }
    else if(*ptr == 'F') {
      stacks[i][1] = 15;
      stacks[i][2] = 0;
    }
    else {
      tintin_puts2("#Error. Invalid expression in #if or #math", NULL);
      return(0);
    }

    if(*ptr != ' ') {
      stacks[i][0] = i+1;
      i++;
    }
    ptr++;
  }

  if(i > 0)
    stacks[i][0] = 0;
  return(1);
}

int do_one_inside(int begin, int end)
{
  int prev, ptr, highest, loc, ploc, next;
  /*char buf[100];*/
  while(1) {
    ptr = 0;
    if(begin > -1)
      ptr = stacks[begin][0];
    highest = 16;
    loc = -1;
    ploc = -1;
    prev = -1;
    while(ptr < end) {
      if(stacks[ptr][1] < highest) {
        highest = stacks[ptr][1];
        loc = ptr;
        ploc = prev;
      }
      prev = ptr;
      ptr = stacks[ptr][0];
    }
    if(highest == 15) {
      if(begin > -1) {
        stacks[begin][1] = 15;
        stacks[begin][2] = stacks[loc][2];
        stacks[begin][0] = stacks[end][0];
        return(1);
      }
      else {
        stacks[0][0] = stacks[end][0];
        stacks[0][1] = 15;
        stacks[0][2] = stacks[loc][2];
        return(1);
      }
    }
    else if(highest == 2) {
      next = stacks[loc][0];
      if(stacks[next][1] != 15 || !stacks[next][0])
        return(0);
      stacks[loc][0] = stacks[next][0];
      stacks[loc][1] = 15;
      stacks[loc][2] = !stacks[next][2];
    }  
    else {
      next = stacks[loc][0];
      if(ploc == -1 || !stacks[next][0] || stacks[next][1] != 15) 
        return(0);
      if(stacks[ploc][1] != 15)
        return(0);
      switch(highest) {
      case 3: /* highest priority is * */
      case 4:
	if (fixed_math == 1) {
	  if (stacks[loc][3] == 1) {
	    stacks[ploc][0] = stacks[next][0];
	    stacks[ploc][2] *= stacks[next][2];
	    /*tintin_puts2("multiple", (struct session *)NULL);
	    sprintf(buf, "%d", stacks[ploc][2]);
	    tintin_puts2(buf, (struct session *) NULL);*/
	  } else {
	    stacks[ploc][0] = stacks[next][0];
	    if (stacks[next][2]) {
	      stacks[ploc][2] /= stacks[next][2];
	      /*tintin_puts2("divide", (struct session *) NULL);
	      sprintf(buf, "%d", stacks[ploc][2]);
	      tintin_puts2(buf, (struct session *) NULL);*/
	    } else
	      tintin_puts2("#ERROR - dividing by zero.. You know something I don't know yet? :-)", (struct session *)NULL);
	  }
	} else {
	  if (highest == 3) {
	    stacks[ploc][0] = stacks[next][0];
	    stacks[ploc][2] *= stacks[next][2];
	  } else if (highest == 4) {
	    stacks[ploc][0] = stacks[next][0];
	    if (stacks[next][2]) {
	      stacks[ploc][2] /= stacks[next][2];
	    }
	    else
	      tintin_puts2("#ERROR - dividing by zero.. You know something I don't know yet? :-)", (struct session *)NULL);
	  } else
	    tintin_puts2("Rohan messed up", (struct session *) NULL);
	}
	break;
      case 5: /* highest priority is + */
      case 6:
	if (fixed_math == 1) {
	  if (stacks[loc][3] == 1) {
	    stacks[ploc][0] = stacks[next][0];
	    stacks[ploc][2] += stacks[next][2];
	    break;
	  }
	  if (stacks[loc][3] == 0) {
	    stacks[ploc][0] = stacks[next][0];
	    stacks[ploc][2] -= stacks[next][2];
	    break;
	  }
	} else {
	  if (highest == 5) {
	    stacks[ploc][0] = stacks[next][0];
	    stacks[ploc][2] += stacks[next][2];
	    break;
	  }

	  if (highest == 6) {
	    stacks[ploc][0] = stacks[next][0];
	    stacks[ploc][2] -= stacks[next][2];
	    break;
	  }
	}
      case 7: /* highest priority is > */
	stacks[ploc][0] = stacks[next][0];
	stacks[ploc][2] = (stacks[ploc][2] > stacks[next][2]);
	break;
      case 8: /* highest priority is >= */
	stacks[ploc][0] = stacks[next][0];
	stacks[ploc][2] = (stacks[ploc][2] >= stacks[next][2]);
	break;
      case 9: /* highest priority is < */
	stacks[ploc][0] = stacks[next][0];
	stacks[ploc][2] = (stacks[ploc][2] < stacks[next][2]);
	break;
      case 10: /* highest priority is <= */
	stacks[ploc][0] = stacks[next][0];
	stacks[ploc][2] = (stacks[ploc][2] <= stacks[next][2]);
	break;
      case 11: /* highest priority is == */
	stacks[ploc][0] = stacks[next][0];
	stacks[ploc][2] = (stacks[ploc][2] == stacks[next][2]);
	break;
      case 12: /* highest priority is != */
	stacks[ploc][0] = stacks[next][0];
	stacks[ploc][2] = (stacks[ploc][2] != stacks[next][2]);
	break;
      case 13: /* highest priority is && */
	stacks[ploc][0] = stacks[next][0];
	stacks[ploc][2] = (stacks[ploc][2] && stacks[next][2]);
	break;
      case 14: /* highest priority is || */
	stacks[ploc][0] = stacks[next][0];
	stacks[ploc][2] = (stacks[ploc][2] || stacks[next][2]);
	break;
      default:
	tintin_puts2("#Programming error *slap Bill*", NULL);
	return(0);
      }
    }
  }
}

/*********************************************************************/
/* the #ifmatch command * By Sverre Normann                          */
/*********************************************************************/
/* Syntax: #ifmatch {mask} {string} {do this} else {do this instead} */
/*********************************************************************/
/* Uses "int match" from glob.c */
/********************************/
/*void ifmatch_command(const char *arg, struct session *ses)
{
  char mask[BUFFER_SIZE], string[BUFFER_SIZE],
       right[BUFFER_SIZE], elsebuf[BUFFER_SIZE],
       temp[BUFFER_SIZE];
  arg=get_arg_in_braces(arg, mask, 0);
  arg=get_arg_in_braces(arg, string, 0);
  arg=get_arg_in_braces(arg, right, 1);
  if (!*mask || !*string || !*right) {
    tintin_puts2("#Syntax: #ifmatch {mask} {string} {do this} else {do this instead}", ses);
  }
  else {
    substitute_vars(mask,temp);
    substitute_myvars(temp,mask,ses);
    substitute_vars(string,temp);
    substitute_myvars(temp,string,ses);
    if (match(mask,string)) {

      substitute_vars(right,temp);
      substitute_myvars(temp,right,ses);
      parse_input(right, ses);
    }
    else {

      arg = (char *) get_arg_stop_spaces(arg, elsebuf);
      if (*elsebuf && strcmp(elsebuf, "else") == 0) {
        arg = get_arg_in_braces(arg, elsebuf, 1);
        substitute_vars(elsebuf, temp);
        substitute_myvars(temp, elsebuf, ses);
        parse_input(elsebuf, ses);
      }
    }
  }
}*/

/*
   ifmatch_command is rearranged by ycjhi.
   I recoded if_command function to make it's coding style complient
   with thoes of other if*_command functions.
   And also changed the syntax slightly so that we can use it more
   common way. -- I removed *else* keyword from the syntax.
   The portion enclosed by brackets is optional.
   New Syntax :
       #ifmatch {str1} {str2} {do-if-match}[ {do-if-mismatch}]
*/

void ifmatch_command(const char *line, struct session *ses)
{
  char left[BUFFER_SIZE], pattern[BUFFER_SIZE];
  char temp[BUFFER_SIZE], thenbuf[BUFFER_SIZE], elsebuf[BUFFER_SIZE];

  line = get_arg_in_braces(line, left, 0);
  line = get_arg_in_braces(line, pattern, 0);
  line = get_arg_in_braces(line, thenbuf, 1);
  line = get_arg_in_braces(line, elsebuf, 1);

  if(!*left || !*pattern || !*thenbuf) {
     tintin_puts2("#Syntax: #ifmatch {mask} {string} {do this}[ {do this instead}]", ses);
     return ;
  }

  substitute_vars(left, temp);         /* match argument */
  substitute_myvars(temp, left, ses);
  substitute_vars(pattern, temp);      /* target pattern */
  substitute_myvars(temp, pattern, ses);

  if(match(left, pattern)) { /* match */
    substitute_vars(thenbuf, temp);
    substitute_myvars(temp, thenbuf, ses);
    parse_input(thenbuf, ses);
  } else if(*elsebuf) { /* else clouse is optional */
    substitute_vars(elsebuf, temp);
    substitute_myvars(temp, elsebuf, ses);
    parse_input(elsebuf, ses);
  }
}
