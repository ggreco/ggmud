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
/*********************************************************************/
/* file: walk.c - stuff for slow-walk features                       */
/*                                                                   */
/*                  TINTIN++ v1.80 developing version                */
/*        original tintin++ v1.80 was coded by R. Ellsworth  1999    */
/*         some enhanced features are added by Yoon-Chan Jhi 2000    */
/*********************************************************************/
/* Slow-walk feature was tested for odd-path-dirs.                   */
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

#include "include/llist.h"
#include "include/main.h"
#include "include/net.h"
#include "include/parse.h"
#include "include/rl.h"
#include "include/path.h"
#include "include/utils.h"
#include "include/walk.h"
#include "include/action.h"
#include "include/variables.h"

/* search and retreive an alias */
static int get_alias(char *body, char *alias_name, struct session * ses)
{
    struct listnode * list;
    struct listnode * ln;
    char buf[128];
    list = ses->aliases ;
    
    if((ln = (struct listnode *)searchnode_list(list, alias_name)) == NULL) {
      if(mesvar[5]) {
            sprintf(buf, "#ALIAS %s IS NOT FOUND.", alias_name);
	    tintin_puts2(buf, ses);
      }

	return 0 ;  /* NOT MATCHED */
    }

    /* MATCHED */

    if(!body)     /* check existence only */
	return 1 ;

    /* copy */
    strcpy(body, ln->right) ;

    return 2 ;
}

/* find pathdirs and mark the begining of each.
   it returns the number of pathdirs found.
   return value :
         0 : failure
       > 0 : success
   #al {testp} { e ; e ; e ; e ; e ;w;w;s;n}

   algorithm :
     while not EOS
     begin
       skip preceeding SPACEs
       if EOS or ';' then break ---> error
       mark the begining of pathdir
       skip rest of pathdir
       skip SPACEs
       if EOS then break ---> success
       if not ';' then break ---> error
       skip ';'
     end
*/
static int index_pathdir(int *index, char *path)
{
  int src = 0, dst = 0 ;
  char buf[128];

  while(path[src])  /* loop until the end-of-string */
    {
      /* skip preceeding SPACEs */
      while(isspace(path[src])) src++ ;

      /* if EOS or ';' then break ---> error */
      if(!path[src])  {
	tintin_puts2("#walk:unexpected end-of-string", NULL) ;
	dst = 0 ;
	break ;
      }
      
      if(path[src] == ';')  {
	sprintf(buf, "#walk:unexpected \';\' : \'%s\'", &path[src]) ;
	tintin_puts2(buf, NULL);
	dst = 0 ;
	break ;
      }

      /* mark the begining of pathdir
         TO DEBUG, UNCOMMENT FOLLOWING LINES...
         tintin_printf("pathdir[%d] starts at walkpath[%d]",
	 NULL, dst, src); */
      index[dst++] = src++ ;
      
      /* skip rest of pathdir */
      while(!isspace(path[src]) && path[src] != ';' && path[src])
	src ++ ;

      /* skip SPACEs */
      while(isspace(path[src])) src ++ ;

      /* if EOS then break ---> success */
      if(!path[src])
	break ;

      /* if not ';' then break ---> error */
      if(path[src] != ';')  {
	sprintf(buf, "#walk:invalid pathdir : \'%s\'", &path[src]);
	tintin_puts2(buf, NULL);
	dst = 0 ;
	break ;
      }

      /* skip ';' (path[src] must be ';' at this point) */
      src ++ ;
    }

  index[dst] = -1 ; /* mark end of pathdirs */

  return dst ;
}

static int sizeof_index(int *index)
{
  int *ptr ;
  ptr = index ;
  while(*ptr != -1) ptr++ ;

  return ptr - index ;
}

/* get_back_dir_savedpath retrieves one pathdir from the position
   one step previous to the specified position within a path alias.
   it is strongly recommended that the path alias should be produced by
   #savepath command.
  
   return value:
      on error   : < 0
      on success : the pointer to the next move.
*/
static int get_back_dir_savedpath(char *buf, char *savedpath, int *index, int ptr)
{
  char *dst, *src ;

  /* set ptr to the previous move. */

  if(ptr == 0) /* set ptr to the end of the path */
      while(index[ptr] != -1) ptr ++ ;

  ptr -- ;

  /* copy a pathdir */

  dst = buf ;
  src = &(savedpath[index[ptr]]) ;

  /* there's no doubt that all the tintin buffers are terminated
     with '\0'. */
  while(!(!*src || *src == ';' || isspace(*src)))
      *dst++ = *src ++ ;

  *dst = '\0' ;

  return ptr ;  /* return ptr for the next move. */
}

/* get_dir_savedpath retrieves one pathdir from the specified position
   within the path alias.
   it is strongly recommended that the path alias should be produced by
   #savepath command.
   
   return value:
      on error   : < 0
      on success : the pointer to the next move.
*/
static int get_dir_savedpath(char *buf, char *savedpath, int *index, int ptr)
{
  char *dst, *src ;

  /* copy a pathdir */

  dst = buf ;
  src = &(savedpath[index[ptr]]) ;

  /* there's no doubt that all the tintin buffers are terminated
     with '\0'. */
  while(!(!*src || *src == ';' || isspace(*src)))
      *dst++ = *src ++ ;

  *dst = '\0' ;

  /* set next read position */

  ptr ++ ;

  if(index[ptr] == -1)  {
    ptr = 0 ;
  }

  return ptr ;  /* return the ptr for the next move. */
}

/* walkset_command initializes slow-walk feature.
   Syntax : #walkset {<alias-name>}[ {<mode-string>}]
            <alias-name> : The name of alias that contains a series
                           of pathdirs separated by a semicolon.
                           It can be easily created by #savepath
                           command.
           <mode-string> : User can specify behavior of walk
                           feature through optional argument
                           <mode-string>.
                           Currently available mode-strings are
                           listed below :
                           - loop   : do looping with walk alias.
                                      (default)
                           - noloop : do not loop with walk alias.
*/ 
void walkset_command(const char *arg, struct session * ses)
{
    char al_name[BUFFER_SIZE], mode_str[BUFFER_SIZE], buf[128];
    
    if(!ses)  {
	tintin_puts2("#NO SESSION ACTIVE!!! ==> NO WALK PATH", NULL) ;
	return;
    }

    arg = get_arg_in_braces(arg, al_name, 0) ;
    arg = get_arg_in_braces(arg, mode_str, 0) ;

    if(!get_alias(ses->walk_path, al_name, ses))  {
      /* no such an alias. initialize and return. */
	ses->walk_now = 0 ;
	ses->walk_path[0] = '\0' ;
	return ;
    }

    sprintf(buf, "#Ok. Alias[%s] is copied into the walkpath.", al_name);
    tintin_puts2(buf, ses);

    if(!index_pathdir(ses->walk_index, ses->walk_path)) {
      /* indexing error */
	ses->walk_now = 0 ;
	/* DSC : i'm assuming you want to reset the array 
	   adding the = '\0' */
	ses->walk_path[0] = '\0';
	return ;
    }

    /* set all the things to default */
    ses->walk_now = 0 ;
    ses->walk_standstill = FALSE ;
    ses->walk_mode = WALKMODE_LOOP ;

    /* process the 2nd argument */
    if(mode_str[0])  {
        if(strcmp(mode_str, "loop") == 0)
            ses->walk_mode = WALKMODE_LOOP ;
        else if(strcmp(mode_str, "noloop") == 0)
            ses->walk_mode = WALKMODE_NOLOOP ;
        else
	  ses->walk_mode = WALKMODE_LOOP ; /* default if error */
    }
}

/******************************************************************************
   The alias that #savepath command of tintin++ 1.80 generates
   is like below.
  
       {testpath}={e;s;n;w;u;d;e;e;e}

   The alias that #savepath command of tintin98 1.2.2 generates
   is like below.
  
       {testpath}={esnwudeee}

   I think that tintin++ produces the better alias to support odd
   pathdirs.

   walk_command takes one pathdir at the current position in the
   current walk-path(see walkset_command). It sends the pathdir to mud
   and advances the current position in the walk-path by one step.
   If it reached the end of the walk-path, it jumps to the beginning
   of the walk-path and continues.
*/
void walk_command(struct session * ses)
{
    static char onedir[NORMAL_STRLEN] ;
    int  walk_next ;

    if(!ses)  {
	tintin_puts2("#NO SESSION ACTIVE!!! ==> NO WALK PATH", NULL) ;
	return;
    }

    if(!ses->walk_path[0] || ses->walk_index[0] == -1)  {
	tintin_puts2("#WALK PATH WASN'T SET UP CORRECTLY", NULL) ;
	return;
    }

    /** make direction command string for sending to mud **/
    walk_next = get_dir_savedpath(onedir,          // return value
				  ses->walk_path,  // walk-path
				  ses->walk_index, // walk-index
				  ses->walk_now    // start position
				  ) ;

    if(walk_next < 0)
        return ;

    /* loop mode check */
    if(walk_next < ses->walk_now && ses->walk_mode == WALKMODE_NOLOOP)
	return ;

    ses->walk_now = walk_next ;

    /* checks for standstill flag. if ON, don't move. */
    if(ses->walk_standstill)   return ;

    /** step forward through the path **/
    write_line_mud(onedir, ses) ;
}

/* walkreset_command resets the current position in the walk alias
   to the beginning. -- ycjhi

       Syntax : #walkreset[ {<num-skip>}]
       An optional argument <num-skip> is the number of pathdirs
       to be skiped from the beginning of the path alias. A negative
       number will be ignored.
  
       eg.
       #walkreset 10
       ---> 1. current position is reset to 0.
            2. skip 10 pathdirs.
            3. the pathdir to be sent by the next #walk command
               is the 11th pathdir.
*/

void walkreset_command(const char *arg, struct session * ses)
{
    char left[BUFFER_SIZE], temp[BUFFER_SIZE] ;
    int  old_standstill, new_pos ;

    if(!ses)  {
	tintin_puts2("#NO SESSION ACTIVE!!! ==> NO WALK PATH", NULL) ;
	return;
    }

    if(!ses->walk_path[0] || ses->walk_index[0] == -1)  {
	tintin_puts2("#WALK PATH WASN'T SET UP CORRECTLY", NULL) ;
	return;
    }

    ses->walk_now = 0 ;

    get_arg_in_braces(arg, left, 0) ;

    if(!*left)     /* stop going ahead if no optional argument. */
	return ;

    /* move to new position without sending move commands. */

    substitute_vars(left, temp) ;
    substitute_myvars(temp, left, ses) ;

    new_pos        = atoi(left) ;
    old_standstill = ses->walk_standstill ;

    walkoff_command(ses) ;

    while(new_pos > 0)  {
	walk_command(ses) ;
	new_pos -- ;
    }

    if(!old_standstill)
	walkon_command(ses) ;
}

/* cleanup_walk cleans a session's walk informations.
   It is needed to prevent crash when changed aliases can cause
   harm to the walk informations.
   eg. savepath, alias, killall, etc.
*/
void cleanup_walk(struct session * ses)
{
    if(!ses)  {
	tintin_puts2("#NO SESSION ACTIVE!!! ==> NO WALK PATH", NULL) ;
	return;
    }

    if(!ses->walk_path[0] || ses->walk_index[0] == -1)  {
	tintin_puts2("#WALK PATH WASN'T SET UP CORRECTLY", NULL) ;
	return;
    }

    ses->walk_now = 0 ;
    ses->walk_path[0] = '\0' ;
}

/* walkback_command takes one pathdir at the previous position in the
   current walk-path(see walkset_command). It sends the reversed pathdir
   to mud and advances the current position in the walk-path by one step
   to the backward.
   If it reached the beginning of the walk-path, it jumps to the end
   of the walk-path and continues.
*/
void walkback_command(struct session * ses)
{
    static char onedir[NORMAL_STRLEN] ;
    int         walk_next ;

    if(!ses)  {
	tintin_puts2("#NO SESSION ACTIVE!!! ==> NO WALK PATH", NULL) ;
	return;
    }

    if(!ses->walk_path[0] || ses->walk_index[0] == -1)  {         
        tintin_puts2("#WALK PATH WASN'T SET UP CORRECTLY", NULL) ;
        return;                                                   
    }                                                             


    /** make direction command string for sending to mud **/
    walk_next = get_back_dir_savedpath(onedir,          /* return value */
		                       ses->walk_path,  /* walk-path */
		                       ses->walk_index, /* walk-index */
		                       ses->walk_now    /* start position */
		                      ) ;

    if(walk_next < 0)
        return ;

    /** reverse **/
    if(!get_back_dir(ses, onedir, onedir))
      return ; /* invalid pathdir */

    /* loop mode check */
    if(walk_next > ses->walk_now && ses->walk_mode == WALKMODE_NOLOOP)
	return ;

    ses->walk_now = walk_next ;

    /* checks for standstill flag. if ON, don't move. */
    if(ses->walk_standstill)   return ;

    /** step backward through the path **/
    write_line_mud(onedir, ses) ;
}

void walkinfo_command(struct session * ses)
{
    char     linebuf[BUFFER_SIZE], buf[128] ;
    char     modestr[NORMAL_STRLEN] ;
    
    if(!ses)  {
	tintin_puts2("#NO SESSION ACTIVE!!! ==> NO WALK PATH", NULL) ;
	return;
    }

    if(!ses->walk_path[0] || ses->walk_index[0] == -1)  {
	tintin_puts2("#WALK PATH WASN'T SET UP CORRECTLY", NULL) ;
	return;
    }

    /* visualize the walk-path and the current position. */
    memset(linebuf, ' ', BUFFER_SIZE) ;
    linebuf[ses->walk_index[ses->walk_now]    ] = '^';
    linebuf[ses->walk_index[ses->walk_now] + 1] = '\0' ;

    /* walk mode */
    if(ses->walk_mode == WALKMODE_LOOP)
	 strcpy(modestr, "LOOP") ;
    else if(ses->walk_mode == WALKMODE_NOLOOP)
	 strcpy(modestr, "NO LOOP") ;
    else
	 strcpy(modestr, "UNKNOWN") ;

    /* display */
    tintin_puts2( "#Walk Information#", ses) ;
    tintin_puts2( "-----------------", ses) ;
    sprintf(buf, " Standstill:%s", ses->walk_standstill ? "TRUE" : "FALSE") ;
    tintin_puts2(buf, ses);
    sprintf(buf, "  Walk Mode:%s", modestr) ;
    tintin_puts2(buf, ses);
    sprintf(buf, "  Walk Path:%s", ses->walk_path) ;
    tintin_puts2(buf, ses);
    sprintf(buf, "   Position:%s", linebuf) ;
    tintin_puts2(buf, ses);
}

/* walkon_command function turns off a session's standstill flag.
   The default status of standstill flag is OFF.
*/
void walkon_command(struct session * ses)
{
    if(ses) ses->walk_standstill = FALSE ;
    else    tintin_puts2("#NO SESSION ACTIVE....", ses) ;
}

void stop_command(const char *arg, struct session *ses)
{
	if (!ses) {
		tintin_puts2("#NO SESSION ACTIVE....", ses);
		return;
	}

	ses->walk_standstill = FALSE;
	ses->walk_path[0] = 0;
	ses->walk_now = 0;
}

void step_command(const char *arg, struct session *ses);
void slow_command(const char *arg, struct session *ses);

void ok_command(const char *arg, struct session *ses)
{
    char left[BUFFER_SIZE];

    if (!ses) {
		tintin_puts2("#NO SESSION ACTIVE....", ses);
		return;
	}

    if (!ses->walk_standstill) {
		tintin_puts2("#NO SLOW WALK ACTIVE....", ses);
		return;
    }

    ses->walk_now++;

    get_arg_in_braces(arg, left, 1);

    if (*left == '1') 
        step_command("", ses);
    
}

void step_command(const char *arg, struct session *ses)
{
    char left[BUFFER_SIZE];

	if (!ses) {
		tintin_puts2("#NO SESSION ACTIVE....", ses);
		return;
	}

    get_arg_in_braces(arg, left, 1);

	if (*left) {
		ses->walk_standstill = FALSE;
		slow_command(left, ses);
	}
    else if (!ses->walk_standstill) {
		tintin_puts2("#NO SLOW WALK ACTIVE....", ses);
		return;
    }
	else if (ses->walk_path[ses->walk_now]) {
        char sc[2];
        sc[1] = 0;
        sc[0] = ses->walk_path[ses->walk_now];
        write_com_arg_mud(sc, "", ses);
	}
    else {
        // path completed
        ses->walk_standstill = FALSE;
    }
}

void expand_path(const char *cp, char *dest, int maxlen)
{
  const char *loc; 
  int multflag, loopcnt, i, pos = 0;

  while(*cp && pos < maxlen ) {
    loc = cp;
    multflag = FALSE;
    while(isdigit(*cp)) {
      cp++;
      multflag = TRUE;
    }
    if(multflag && *cp) {
      char sc;
      sscanf(loc, "%d%c", &loopcnt, &sc);
      for(i = 0; i++ < loopcnt; )
	    dest[pos++] = sc;
    }
    else if(*cp) {
      dest[pos++] = *cp;
    }

    if(*cp)
      cp++;
  }
  dest[pos] = 0;
}

void slow_command(const char *arg, struct session *ses)
{
    char left[BUFFER_SIZE], *l, dest[BUFFER_SIZE];

	if (!ses) {
		tintin_puts2("#NO SESSION ACTIVE....", ses);
		return;
	}

    get_arg_in_braces(arg, left, 1);

	if (!*left)
        tintin_puts2("#NO SLOW PATH GIVEN!", ses);

	l = left;
    if (ses->walk_standstill) 
	    tintin_puts2("#OVERWRITTEN PREVIOUS SLOW WALK SEQUENCE", ses);

	if (*l == '.')
		++l;

    expand_path(l, dest, BUFFER_SIZE);
	ses->walk_standstill = TRUE;
	strcpy(ses->walk_path, dest); 
	ses->walk_now = 0;

	step_command("", ses);
}


/* walkoff_command function turns on a session's standstill flag.
   When a session's standstill flag is on, tintin will advance
   current position in a path alias only without sending an actual
   movement command to the mud for #walk, #walkback command.
   It is useful when the user wants to adjust current position in
   a path alias.
*/
void walkoff_command(struct session * ses)
{
    if(ses) ses->walk_standstill = TRUE ;
    else    tintin_puts2("#NO SESSION ACTIVE....", ses) ;
}


/* getwalkposition_command function stores current walk position
   in a given variable.
   Syntax: #getwalkpos {<var-name>}
   eg. #getwalkpos {whereami}
       #showme {$whereami}
       10 --> 11th pathdir will be sent by the next #walk command.
              note) position starts from zero.
*/
void getwalkposition_command(const char *arg, struct session * ses)
{
    char left[BUFFER_SIZE], temp[BUFFER_SIZE];
    struct listnode * my_vars;

    /* exception check */

    if(!ses)  {
	tintin_puts2("#NO SESSION ACTIVE!!! ==> NO WALK PATH", NULL) ;
	return;
    }

    if(!ses->walk_path[0] || ses->walk_index[0] == -1)  {
	tintin_puts2("#WALK PATH WASN'T SET UP CORRECTLY", NULL) ;
	return;
    }

    my_vars = (ses ? ses->myvars : common_myvars);

    /* fetch variable name */
    get_arg_in_braces(arg, left, 0);

    sprintf(temp, "%cvariable {%s} {%d}", tintin_char, left, ses->walk_now) ;

    parse_input(temp, ses) ;
}

/* getwalkdirection_command function returns a pathdir from given
   position within walk path. 
   Syntax: #getwalkdir {<var-name>}[ {<position>}]
             <var-name> is the name of a variable into which the
           fetched pathdir is stored.
             Optional argument <position> indicates the position
           to fetch a pathdir. If it is omitted, pathdir will be
           fetched from current position in a walk path.
   eg.
       #getwalkdir mydir 10
       --> it will store the 11th pathdir into mydir from the
           current session's walk path.
           note) position starts from zero.
*/
void getwalkdirection_command(const char *arg, struct session * ses)
{
    char left[BUFFER_SIZE], right[BUFFER_SIZE], temp[BUFFER_SIZE] ;
    char onedir[NORMAL_STRLEN], buf[128] ;
    struct listnode * my_vars;
    int pos, index_size ;

    /* exception check */

    if(!ses)  {
	tintin_puts2("#NO SESSION ACTIVE!!! ==> NO WALK PATH", NULL) ;
	return;
    }

    if(!ses->walk_path[0] || ses->walk_index[0] == -1)  {
	tintin_puts2("#WALK PATH WASN'T SET UP CORRECTLY", NULL) ;
	return;
    }

    my_vars = (ses ? ses->myvars : common_myvars);

    /* fetch variable name */
    arg = get_arg_in_braces(arg, left, 0);

    /* fetch position */
    get_arg_in_braces(arg, right, 0);

    if(!*right)  { /* if no second argument */
	pos = ses->walk_now ;
    } else { /* if user specified position where tintin read pathdir */
	substitute_vars(right, temp) ;
	substitute_myvars(temp, right, ses) ;

	/* convert to number */
	pos = atoi(right) ;

	index_size = sizeof_index(ses->walk_index) - 1 ;
	if(pos > index_size)  {
	    sprintf(buf, "#Wrong position error. Max is %d", index_size) ;
	    tintin_puts2(buf, ses);
	    return ;
	}
    }

    /* fetch a pathdir from pos. */
    get_dir_savedpath(onedir, ses->walk_path, ses->walk_index, pos) ;
    sprintf(temp, "%cvariable {%s} {%s}", tintin_char, left, onedir) ;

    /* DEBUG
     tintin_puts2(temp, ses) ;
    */
    parse_input(temp, ses) ;
}

/* get_back_dir returns the reversed pathdir. */
/* DSC : this was from the path.c in the version that ycjhi
   sent me.  I want to keep all the walk related things together
   to make it easiler to track
*/
int get_back_dir(struct session * ses, char *dst, char *src)
{
  char buf[128];
  struct listnode *mypathdirs, *ln;

  mypathdirs = (ses ? ses->pathdirs : common_pathdirs);

  if(!(ln = searchnode_list(mypathdirs, src))) {
    sprintf(buf, "#No such a direction in current pathdirs[%s]", src);
    tintin_puts2(buf, ses);
    return 0 ;  /* invalid pathdir */
  }

  /* ln->left : normal
     ln->right: reverse
  */
  strcpy(dst, ln->right) ;

  return strlen(dst) ; /* returns the length of the reversed pathdir */
}
