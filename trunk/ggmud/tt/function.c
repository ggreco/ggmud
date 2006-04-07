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
/* file: function.c - functions related to the functions             */
/* Modified the substitute.c file to create this,   //SN             */
/*********************************************************************/
#include "config.h"
#include "tintin.h"

#if defined(HAVE_STRING_H)
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif
#include <stdlib.h>

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

char last_result[BUFFER_SIZE];

#include "include/action.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/parse.h"
#include "include/help.h"
#include "include/ivars.h"
#include "include/misc.h"
#include "include/net.h"
#include "include/path.h"
#include "include/rlhist.h"
#include "include/rltab.h"
#include "include/session.h"
#include "include/substitute.h"
#include "include/text.h"
#include "include/ticks.h"
#include "include/utils.h"
#include "include/variables.h"
#include "include/function.h"


/************************************************************************/
/* Usage of functions:                                                  */
/************************************************************************/
/* ex1:                                                                 */
/* #function {rnd} {#random {temp} {%1,%2};#result {$temp};#unvar temp} */
/* #showme A random number between 0 and 100: @rnd{0 100}               */
/************************************************************************/
/* ex2:                                                                 */
/* #function {ifs} {#ifstringequal {%1} {%2} {#result 1}                */
/*                  else {#result 0}}                                   */
/* #if {!@ifs{{$name} {$myname}} && $spam} {say Hi $name!}              */
/************************************************************************/
/* In short: functions are treated much like variables. Their value is  */
/* a command-line which is executed, and the functions are substituted  */
/* by the parameter last sent to the #result command                    */
/************************************************************************/
/* last ex:                                                             */
/* #function {lastfuncresult} {#nop}                                    */
/* #showme Last use of a function gave @lastfuncresult as result.       */
/************************************************************************/

void do_trim(char *result, char * string)
{
//    printf("in do_trim( %s )\n", string);

    while (*string) {
        if (*string != ' ')
            *result++= *string;

        string++;
    }
    
    *result = 0;
}

void do_oneword(char *result, char *string, char *word_num)
{
    int w = atoi(word_num);
    char buffer[BUFFER_SIZE];
    int i = 0;
    
//    printf("in do_oneword( %s, %d )\n", string, w);
    
    buffer[0] = 0;
    
    while (*string) {
        char *b = buffer;

        while(*string != ' ' && *string != '\t' && *string != 0)
            *b++=*string++;

        *b = 0;
        
        if (w == i) {
            strcpy(result, buffer);
            return;
        }
        else if (*string)
            string++;

        i++;
    }

    strcpy(result, buffer);
}

struct predefined_function
{
    char *name;
    void (*func)(char *result, char * arg1, char * arg2, char * arg3, char * arg4);
};

struct predefined_function predefineds[] = 
{
    {"trim", do_trim},
    {"oneword", do_oneword},
    {NULL, NULL}
};

struct listnode *init_functions()
{
    struct listnode *l = init_list();
    int i = 0;

    while (predefineds[i].name) {
        char buffer[20];

        sprintf(buffer, "%ld", (long)predefineds[i].func);
        
        insertnode_list(l, predefineds[i].name, 
                           buffer, "0", ALPHA);
        i++;
    }

    return l;
}

/*************************/
/* the #function command */
/*************************/
void function_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], right[BUFFER_SIZE], result[BUFFER_SIZE];
  struct listnode *myfunctions, *ln;
  myfunctions=(ses) ? ses->myfuncs : common_functions;
  arg=get_arg_in_braces(arg, left,0);
  arg=get_arg_in_braces(arg, right,1);

  if(!*left) {
    tintin_puts2("#THESE FUNCTIONS HAVE BEEN DEFINED:", ses);
    show_list(myfunctions);
    prompt(ses);
  }

  else if(*left && !*right) {
    if ((ln=search_node_with_wild(myfunctions,left))!=NULL) {
      while((myfunctions=search_node_with_wild(myfunctions, left))!=NULL) {
        shownode_list(myfunctions);
      }
      prompt(ses);
    }
    else
      /* same mesvar toggle as aliases */
      if (mesvar[0])
        tintin_puts2("#THAT FUNCTION IS NOT DEFINED.", ses);
  }

  else {
    if((ln=searchnode_list(myfunctions, left))!=NULL)
      deletenode_list(myfunctions, ln);
    insertnode_list(myfunctions, left, right, "0", ALPHA);
    funcnum++;
    if (mesvar[0]) {
      sprintf(result, "#Ok. {%s} now gives {%s}.",left,right);
      tintin_puts2(result, ses);
    }
  }
}


/*****************************/
/* the #unfunction command */
/*****************************/
void unfunction_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE] ,result[BUFFER_SIZE];
  struct listnode *myfunctions, *ln, *temp;
  int flag;
  flag=FALSE;
  myfunctions=(ses) ? ses->myfuncs : common_functions;
  temp=myfunctions;
  arg=get_arg_in_braces(arg,left,1);
  while ((ln=search_node_with_wild(temp, left))!=NULL) {
    if (mesvar[0]) {
      sprintf(result, "#Ok. {%s} is no longer a function.", ln->left);
      tintin_puts2(result, ses);
    }
    deletenode_list(myfunctions, ln);
    flag=TRUE;
    temp=ln;
  }
  if (!flag && mesvar[2])
    tintin_puts2("#THAT FUNCTION IS NOT DEFINED.", ses);
}


/***********************/
/* the #result command */
/***********************/
void result_command(const char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], temp[BUFFER_SIZE];
  arg=get_arg_in_braces(arg, left,1);
  substitute_vars(left,temp);   
  substitute_myvars(temp,left,ses);
  strcpy(last_result,left);
  *(last_result+strlen(left))='\0';
}

/*************************************************************************/
/* copy the arg text into the result-space, but substitute the functions */
/* @<function>{params} with the values they stand for                    */
/*************************************************************************/
/* this is just a modification of substitute_myvars in variables.c       */
/*************************************************************************/
/* functions also allow the 'new' format used for variables. that is     */
/* @{name}{params} to allow complex names containing non-alpha chars.    */
/*************************************************************************/
void substitute_functions(const char *arg, char *result, struct session *ses)
{

  char funcname[BUFFER_SIZE], temp[BUFFER_SIZE];
  int nest=0,counter,funclen;
  int specfunc;
  char params[BUFFER_SIZE];
  int i;
  char *cpsource, *cpsource2, end;
  struct listnode *ln, *tempfuncs;
  extern struct listnode *predefined_functions;

 
  tempfuncs=(ses) ? ses->myfuncs : common_functions;
  fflush(stdout);

  while(*arg) {
    if(*arg=='@') {        /* substitute function */
      counter=0;
      while (*(arg+counter)=='@')
        counter++;
      funclen=0;
 
      if(*(arg+counter)!=DEFAULT_OPEN) {      /* using the @{name} or @name format? */
        specfunc=FALSE;
        while(isalpha(*(arg+funclen+counter)))
          funclen++;
        if (funclen>0)
          strncpy(funcname,arg+counter,funclen);
        *(funcname+funclen)='\0';
      }
      else {
        specfunc=TRUE;
        get_arg_in_braces(arg+counter, temp, 0);
	substitute_myvars(temp, funcname, ses); /* dsc*/
        /*substitute_functions(temp, funcname, ses);*/  /* RECURSIVE CALL */
        funclen=strlen(temp);
      }
 
      if(specfunc) funclen+=2;
 
      if (counter==nest+1 && !isdigit(*(arg+counter+1))) {
        if((ln = searchnode_list(predefined_functions, funcname))!=NULL) {
            void (*ptr)(char *r, char *a1, char *a2, char *a3, char *a4);
            char arg1[BUFFER_SIZE/4], arg2[BUFFER_SIZE/4],
                 arg3[BUFFER_SIZE/4], arg4[BUFFER_SIZE/4],
                 myres[BUFFER_SIZE];
            char *args[4] = {arg1, arg2, arg3, arg4};
            int i = 0;

            *arg1 = *arg2 = *arg3 = *arg4 = 0;

            *myres = 0;
            arg+=counter+funclen;
//            printf("Found predefined for: %s arg: %s\n", funcname, arg);
                       
            ptr = atol(ln->right);

            if (*arg=='(') {
               char bb[BUFFER_SIZE];
               arg++;

               while (*arg != ')' && *arg != 0) {
                   int k = 0;
                   
                   while(*arg == ' ')
                       arg++;
        
                   while(*arg != ' ' && *arg != ',' && *arg != 0 && *arg != ')')
                       bb[k++] = *arg++;

                   bb[k] = 0;

                   substitute_myvars(bb, args[i], ses);

                   if (*arg == ',')
                       arg++;
                   
                   i++;
               }


               if (*arg || i > 0) {
                  if (*arg == ')')
                       arg++;
/*
                  printf("Processo: %s (%s,%s,%s,%s)\n",
                          funcname, args[0], args[1], args[2], args[3]);
 */
                   ptr(myres, args[0], args[1], args[2], args[3]);

                   
 /*                  printf("Result: %s\n", myres); */
                   strcpy(result, myres);
                   result+= strlen(myres);
               }
            }

        }
        else if((ln=searchnode_list(tempfuncs, funcname))!= NULL) {
          char newcommand[BUFFER_SIZE];
	  arg+=counter+funclen;
           
          if (*arg==DEFAULT_OPEN) {
            /* get parameters and change the %0-%9 vars */
            arg=get_arg_in_braces(arg, params, 0);              /* write a new function for this? */
            
        /* The following chunk of code is copied from parse_input in parse.c */
            strcpy(vars[0], params);
            for(i=1, cpsource=params; i<10; i++) {


              /* Next lines CHANGED to allow argument grouping with aliases */
              while (*cpsource == ' ')
                cpsource++;
              end = (*cpsource == '{') ? '}' : ' ';
              cpsource = (*cpsource == '{') ? cpsource+1 : cpsource;
              for(cpsource2=cpsource; *cpsource2 && *cpsource2!=end; cpsource2++);
              strncpy(vars[i], cpsource, cpsource2-cpsource);
              *(vars[i]+(cpsource2-cpsource))='\0'; 
              cpsource=(*cpsource2) ? cpsource2+1 : cpsource2;
            }
            /* There, now we've set the %0-%9 variables too :) */
          }
	  prepare_actionalias(ln->right, newcommand, ses);
          parse_input(newcommand, ses);
          strcpy(result,last_result);
          result+=strlen(last_result);

        }
        else { 
          strncpy(result,arg,counter+funclen);
          result+=funclen+counter;
          arg+=counter+funclen;
        }
      }
      else  {
          strncpy(result,arg,counter+funclen);
          result+=funclen+counter;
          arg+=funclen+counter;
      }
    }
    else if (*arg==DEFAULT_OPEN) {
      nest++;
      *result++= *arg++;
    }
#ifdef BIG5
    /* for Big5 encoding */
    else if(*arg & 0x80) {
      *result++ = *arg++;
      if(*arg)
	*result++ = *arg++;
    }
#endif
    else if (*arg==DEFAULT_CLOSE) {
      nest--;
      *result++= *arg++;
    }
    else if (*arg=='\\' && *(arg+1)=='@' && nest==0) {
     arg++;
      *result++= *arg++;
    }
        
    else
      *result++= *arg++;
  }
  *result='\0';

}

