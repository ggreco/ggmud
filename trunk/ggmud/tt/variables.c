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
/* file: variables.c - functions related the the variables           */
/*                             TINTIN ++                             */
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
#ifdef HAVE_CTYPE_H
#include <ctype.h>
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
#include "include/rltab.h"
#include "include/session.h"
#include "include/substitute.h"
#include "include/text.h"
#include "include/ticks.h"
#include "include/utils.h"
#include "include/variables.h"
#include "include/function.h"

/*************************/
/* the #variable command */
/*************************/
void var_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], arg2[BUFFER_SIZE];
  struct listnode *tempvars, *ln;
  /* char right2[BUFFER_SIZE]; */
  tempvars=(ses) ? ses->myvars : common_myvars;
  arg=get_arg_in_braces(arg, left,0);
  arg=get_arg_in_braces(arg, right,1);
  if(!*left) {
    tintin_puts2("#THESE VARIABLES HAVE BEEN SET:", ses);
    show_list(tempvars);
    prompt(ses);
  }

  else if(*left && !*right) {
    if ((ln=search_node_with_wild(tempvars,left))!=NULL) {
      while((tempvars=search_node_with_wild(tempvars, left))!=NULL) {
        shownode_list(tempvars);
      }
      prompt(ses);
    }
    else
      if (mesvar[5])
        tintin_puts2("#THAT VARIABLE IS NOT DEFINED.", ses);
  }

  else {
    if((ln=searchnode_list(tempvars, left))!=NULL) {
        if (ln->right)
            free(ln->right);
        
        ln->right = strdup(right);
    }
    else {
        ln = insertnode_list(tempvars, left, right, "0", ALPHA);
#ifdef WITH_LUA
        add_lua_global(ln->left, &(ln->right));
#endif
    }
    varnum++;
    if (mesvar[5] && verbose) {
      sprintf(arg2, "#Ok. $%s is now set to {%s}.",left, right);
      tintin_puts2(arg2, ses);
    }
  }
}
/************************/
/* the #unvar   command */
/************************/
void unvar_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE] ,result[BUFFER_SIZE];
  struct listnode *tempvars, *ln, *temp;
  int flag;
  flag=FALSE;
  tempvars=(ses) ? ses->myvars : common_myvars;
  temp=tempvars;
  arg=get_arg_in_braces(arg,left,1);
  while ((ln=search_node_with_wild(temp, left))!=NULL) {
    if (mesvar[5] && verbose) {
      sprintf(result, "#Ok. $%s is no longer a variable.", ln->left);
      tintin_puts2(result, ses);
    }
    deletenode_list(tempvars, ln);
    flag=TRUE;
    /* You don't deallocate  something and then you try to assign it! 
       and this is not even *USED* after assigning it either. sheesh - dsc
    temp=ln;
    */
  }
  if (!flag && mesvar[5])
    tintin_puts2("#THAT VARIABLE IS NOT DEFINED.", ses);
}


/*************************************************************************/
/* copy the arg text into the result-space, but substitute the variables */
/* $<string> with the values they stand for                              */
/*                                                                       */
/* Notes by Sverre Normann (sverreno@stud.ntnu.no):                      */
/* xxxxxxxxx 1996: added 'simulation' of secstotick variable             */
/* Tue Apr 8 1997: added ${<string>} format to allow use of non-alpha    */
/*                 characters in variable name                           */
/*************************************************************************/
/* Note: $secstotick will return number of seconds to tick (from tt's    */
/*       internal counter) provided no variable named 'secstotick'       */
/*       already exists                                                  */
/*************************************************************************/
 void substitute_myvars(const char *string, char *result, struct session *ses)
{

  char temp[BUFFER_SIZE];
  *result='\0';
  substitute_functions(string, temp, ses);
  substitute_myvariables(temp, result, ses);
}

void substitute_myvariables(const char *arg, char *result, struct session *ses)
{
    char varname[BUFFER_SIZE], temp[BUFFER_SIZE];
    int nest=0,counter,varlen;
    int specvar;
    struct listnode *ln, *tempvars;

    tempvars=(ses) ? ses->myvars : common_myvars;
    fflush(stdout);
    while(*arg) {
        if(*arg=='$') { /* substitute variable */
            counter=0;
            while (*(arg+counter)=='$')
                counter++;
            varlen=0;

            /* ${name} code added by Sverre Normann */
            if(*(arg+counter)!=DEFAULT_OPEN) {
                specvar=FALSE;
                while(isalpha(*(arg+varlen+counter)))
                    varlen++;  
                if (varlen>0)
                    strncpy(varname,arg+counter,varlen);
                *(varname+varlen)='\0';
            } else {
                specvar=TRUE;
                get_arg_in_braces(arg+counter, temp, 0);
                substitute_myvars(temp, varname, ses);      /* RECURSIVE CALL */
                varlen=strlen(temp);
            }

            if(specvar) varlen+=2;

            if (counter==nest+1 && !isdigit(*(arg+counter+1))) {
                if((ln=searchnode_list(tempvars, varname))!= NULL) {
                    
                    strcpy(result, ln->right);
                    result+=strlen(ln->right);
                    arg+=counter+varlen;
                } else {
                    /* secstotick code added by Sverre Normann */
                    if (strcmp(varname,"secstotick")) {
                        strncpy(result,arg,counter+varlen);
                        result+=varlen+counter;
                    }
                    else {
                        char buf[BUFFER_SIZE];
                        sprintf(buf,"%d",timetilltick());
                        strcpy(result, buf);
                        result+=strlen(buf);
                    }
                    arg+=counter+varlen;
                }
            } else  {  
                strncpy(result,arg,counter+varlen);
                result+=varlen+counter;
                arg+=varlen+counter;
            }
#ifdef BIG5
            // for Big5 encoding
        } else if(*arg & 0x80) {
            *result++ = *arg++;
            if(*arg)
                *result++ = *arg++;
#endif
        } else if (*arg==DEFAULT_OPEN) {
            nest++;
            *result++= *arg++;
        } else if (*arg==DEFAULT_CLOSE) {
            nest--;
            *result++= *arg++;
        } else if (*arg=='\\' && *(arg+1)=='$' && nest==0) {
            arg++;
            *result++= *arg++;
        } else
            *result++= *arg++;
    }
    *result='\0';
}


/*****************************************************/
/* the #getvarvalue command * By Sverre Normann      */
/*****************************************************/
/* Needed since tintin's var substitute can't handle */
/* variable names with non-alpha characters like     */
/* space, numbers and others.                        */
/*****************************************************/
/* NB! #getvarvalue is redundant after the ${<name>} */
/*     format was made. Kept only for to keep comp.  */
/*     with older comfiles...                        */
/*****************************************************/
void getvarvalue_command(const char *arg, struct session *ses)
{
  char   left[BUFFER_SIZE], right[BUFFER_SIZE],
         temp[BUFFER_SIZE];
  struct listnode *ln, *tempvars;
  
  tempvars = (ses) ?  ses->myvars : common_myvars;
  arg=get_arg_in_braces(arg, left, 0);
  arg=get_arg_in_braces(arg, right, 1);
  if (!*left || !*right) {
    tintin_puts2("#Syntax: #getvarvalue {dest var} {source var}", ses);
  } else {
    substitute_vars(left,temp);
    substitute_myvars(temp,left,ses);
    substitute_vars(right,temp);
    substitute_myvars(temp,right,ses);

    if((ln=searchnode_list(tempvars, right))!= NULL) {
      sprintf(temp,"{%s} {%s}",left,ln->right);
      var_command(temp, ses);
    }
    else {
      tintin_puts2("#Error in #getvarvalue: variable does not exists",ses);
    }
  }
}


/********************************************************/
/* the #getlistlength command * By Sverre Normann       */
/********************************************************/
/* Syntax: #getlistlength {destination variable} {list} */
/*****************************************************************/
/* Note: This will return the number of items in the list.       */
/*       An item is either a word, or grouped words in brackets. */
/*       Ex:  #getl {listlength} {smile {say Hi!} flip bounce}   */
/*            -> listlength = 4                                  */
/*****************************************************************/
void getlistlength_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], list[BUFFER_SIZE],
       temp[BUFFER_SIZE];
  int  i;

  arg = get_arg_in_braces(arg, left,0);
  if (!*left) {
    tintin_puts2("#Error - Syntax: #getlistlength {dest var} {list}", ses);
  }
  else {
    substitute_vars(left,temp);
    substitute_myvars(temp,left,ses);
    get_arg_in_braces(arg, list, 1);
    substitute_vars(list,temp);
    substitute_myvars(temp,list,ses);
    arg = list;
    i=0;
    do {
      if(*arg) i++;
      arg = get_arg_in_braces(arg, temp, 0);
    } while(*arg);
    sprintf(temp,"{%s} {%d}",left,i);
    var_command(temp,ses);
  }
}


/******************************************************************/
/* the #getitemnr command * By Sverre Normann                     */  
/******************************************************************/
/* Syntax: #getitemnr {destination variable} {item number} {list} */
/******************************************************************/
/* Note: This will return a specified item from a list.           */
/*       An item is either a word, or grouped words in brackets.  */
/*       Ex:  #geti {dothis} {2} {smile {say Hi!} flip bounce}    */
/*            -> dothis = say Hi!                                 */
/******************************************************************/
void getitemnr_command(const char *arg, struct session *ses)  
{
  char destvar[BUFFER_SIZE], itemnrtxt[BUFFER_SIZE],
       list[BUFFER_SIZE], temp1[BUFFER_SIZE], temp2[BUFFER_SIZE];
  int  i, itemnr;

  arg = get_arg_in_braces(arg, destvar, 0);
  arg = get_arg_in_braces(arg, itemnrtxt, 0);
  
  if (!*destvar || !*itemnrtxt) {
    tintin_puts2("#Error - Syntax: #getitemnr {destination variable} {item number} {list}", ses);
  }
  else {
    substitute_vars(destvar,temp1);
    substitute_myvars(temp1,destvar,ses);
    substitute_vars(itemnrtxt,temp1);
    substitute_myvars(temp1,itemnrtxt,ses);
    if (sscanf(itemnrtxt,"%d",&itemnr)!=1) {
      tintin_puts2("#Error in #getitemnr - item number has to be a digit!",ses);
    }
    else {
      get_arg_in_braces(arg, list, 1);
      substitute_vars(list,temp1);
      substitute_myvars(temp1,list,ses);
      arg = list;
      i=0;
      if (itemnr>0) {
        do {
          arg = get_arg_in_braces(arg, temp1, 0);
          i++;
        } while (i!=itemnr);

        if (*temp1) {
          sprintf(temp2,"{%s} {%s}",destvar,temp1);
          var_command(temp2,ses);
        }
        else {
          tintin_puts2("#Error in #getitemnr - item doesn't exist!",ses);
        }
      }
    }
  }
}


/*************************/
/* the #tolower command */
/*************************/
void tolower_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], arg2[BUFFER_SIZE], *p;
  struct listnode *tempvars, *ln;

  tempvars = (ses) ? ses->myvars : common_myvars;
  arg = get_arg_in_braces(arg, left,0);
  arg = get_arg_in_braces(arg, right,1);
  if (!*left || !*right) {
    tintin_puts2("#Syntax: #tolower <var> <text>", ses);
  } else {
    if ((ln=searchnode_list(tempvars, left)) != NULL)
      deletenode_list(tempvars, ln);
    for (p=right; *p; p++)
      *p = tolower(*p);
    insertnode_list(tempvars, left, right, "0", ALPHA);
    varnum++;
    if (mesvar[5]) {
      sprintf(arg2, "#Ok. $%s is now set to {%s}.",left, right);
      tintin_puts2(arg2, ses);
    }
  }
}

/*************************/
/* the #toupper command */
/*************************/
void toupper_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], arg2[BUFFER_SIZE], *p;
  struct listnode *tempvars, *ln;

  tempvars = (ses) ? ses->myvars : common_myvars;
  arg = get_arg_in_braces(arg, left,0);
  arg = get_arg_in_braces(arg, right,1);
  if (!*left || !*right) {
    tintin_puts2("#Syntax: #toupper <var> <text>", ses);
  } else {
    if ((ln=searchnode_list(tempvars, left)) != NULL)
      deletenode_list(tempvars, ln);
    for (p=right; *p; p++)
      *p = toupper(*p);
    insertnode_list(tempvars, left, right, "0", ALPHA);
    varnum++;
    if (mesvar[5]) {
      sprintf(arg2, "#Ok. $%s is now set to {%s}.",left, right);
      tintin_puts2(arg2, ses);
    }
  }
}

/************************************************/
/* the #postpad command * By Sverre Normann     */
/************************************************/
/* Syntax: #postpad {dest var} {length} {text}  */
/*****************************************************/
/* Truncates text if it's too long for the specified */
/* length. Pads with spaces at the end if the text   */
/* isn't long enough.                                */
/*****************************************************/
void postpad_command(const char *arg, struct session *ses)
{
  char destvar[BUFFER_SIZE], textstr[BUFFER_SIZE],
       newtextstr[BUFFER_SIZE],temp[BUFFER_SIZE], *p, 
       lengthstr[BUFFER_SIZE];
  int  length;
 
  arg = get_arg_in_braces(arg, destvar, 0);
  arg = get_arg_in_braces(arg, lengthstr, 0);
  arg = get_arg_in_braces(arg, textstr, 1);
 
  if (!*lengthstr) {
    tintin_puts2("#Error - Syntax: #postpad {dest var} {length} {text}", ses);
  }
  else {
    substitute_vars(destvar,temp);
    substitute_myvars(temp,destvar,ses);
    substitute_vars(lengthstr,temp);   
    substitute_myvars(temp,lengthstr,ses);
    substitute_vars(textstr,temp); 
    substitute_myvars(temp,textstr,ses);    

    if (!sscanf(lengthstr,"%d", &length) || (length <1)) {
      tintin_puts2("#Error in #postpad - length has to be a digit!",ses);
    }
    else {
      strncpy(newtextstr,textstr,length);
      if (strlen(textstr) < length) {
        p=newtextstr+strlen(newtextstr);
        for (length-=strlen(newtextstr);length>0;length--) {
          *p=' ';
          p++;
        }
        *p='\0';
      }
      sprintf(temp,"{%s} {%s}",destvar,newtextstr);
      var_command(temp,ses);
    }
  }
}
 

/***********************************************/
/* the #prepad command * By Sverre Normann     */
/***********************************************/
/* Syntax: #prepad {dest var} {length} {text}  */
/*****************************************************/
/* Truncates text if it's too long for the specified */
/* length. Pads with spaces at the start if the text */
/* isn't long enough.                                */
/*****************************************************/
void prepad_command(const char *arg, struct session *ses)
{
  char destvar[BUFFER_SIZE], textstr[BUFFER_SIZE], 
       newtextstr[BUFFER_SIZE],temp[BUFFER_SIZE], *p, 
       lengthstr[BUFFER_SIZE];
  int  length,i;
 
  arg = get_arg_in_braces(arg, destvar, 0);
  arg = get_arg_in_braces(arg, lengthstr, 0);
  arg = get_arg_in_braces(arg, textstr, 1);
  
  if (!*lengthstr) {
    tintin_puts2("#Error - Syntax: #prepad {dest var} {length} {text}", ses);
  }  
  else {
    substitute_vars(destvar,temp);
    substitute_myvars(temp,destvar,ses);
    substitute_vars(lengthstr,temp); 
    substitute_myvars(temp,lengthstr,ses); 
    substitute_vars(textstr,temp);  
    substitute_myvars(temp,textstr,ses);

    if (!sscanf(lengthstr,"%d",&length) || (length < 1)) {
      tintin_puts2("#Error in #prepad - length has to be a digit!",ses);
    }   
    else {
      p=newtextstr;
      if (strlen(textstr) < length) {
        for (i=length-strlen(textstr);i>0;i--) {
          *p=' ';
          p++;
        }
      }
      strncpy(p,textstr,length);
      sprintf(temp,"{%s} {%s}",destvar,newtextstr);
      var_command(temp,ses);
    }
  }
}

/*****************************/
/* the #removestring command */
/*****************************/

void removestring_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], arg2[BUFFER_SIZE];
  char buf[BUFFER_SIZE], *s;
  struct listnode *tempvars, *ln;
  int i = 0;

  tempvars = (ses ? ses->myvars : common_myvars);
  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 1);
  if(!*left || !*right)
    tintin_puts2("#Syntax: #removestring <var> <text>", ses);
  else {
    if(!(ln = searchnode_list(tempvars, left)))
      tintin_puts2("#THAT VARIABLE IS NOT DEFINED.", ses);

    for(s = ln->right; *s;)
      if(is_abbrev(right, s))
        s += strlen(right);
      else
        buf[i++] = *s++;
    buf[i] = '\0';

    if(!*buf)
      strcpy(buf, " ");

    if(strcmp(buf, ln->right)) {
      deletenode_list(tempvars, ln);
      insertnode_list(tempvars, left, buf, "0", ALPHA);
      varnum++;
      if(mesvar[5]) {
        sprintf(arg2, "#Ok. $%s is now set to {%s}.", left, buf);
        tintin_puts2(arg2, ses);
      }
    }
  }
}

