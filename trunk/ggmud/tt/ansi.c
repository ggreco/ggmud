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

#include "config.h"
#include "tintin.h"

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#include "include/ansi.h"

/* Warning! This function is more of a hack than anything else.         */
/* It tries to calculate the length of an ansi code sequence.           */
/* The calculations were obtained by trial and error by matching to the */
/* output echo gives in a normal unix xterm (and some knowledge of what */
/* the code sequences look like).                                       */

int skip_ansi(const char *str)
{
  int skip = 0;

  if(!str || !*str)
    return(skip);

  switch(*str++) { // cr and lf was commented, why?
  case '\a': /* BEL */
  case '\b': /* BS */
  case '\t': /* HT */
  case '\n': /*   NL */
  case '\v': /* VT */
  case '\f': /* NP */
  case '\r': /*   CR */
  case 14:   /* SO */
  case 15:   /* SI */
    return(++skip);

  case 27:   /* ESC */
    skip++;
    break;

  default:
    return(skip);
  }

  if(*str == '[') {
    str++;
    skip++;
    if(*str == '@' || *str == '[' || *str == '\\' || *str == ']' ||
       *str == '^' || *str == '_' || *str == '`' || *str == '{' ||
       *str == '|' || *str == '}' || *str == '~')
      return(skip+1);
    
    if(ispunct(*str) &&
       *str != '!' && *str != '"' && *str != '#' && *str != '$' &&
       *str != '%' && *str != '&' && *str != '\'' && *str != '(' &&
       *str != ')' && *str != '*' && *str != '+' && *str != ',' &&
       *str != '-' && *str != '.' && *str != '/' && *str != ' ') {
      str++;
      skip++;
      if(ispunct(*str) &&
	 *str != '!' && *str != '"' && *str != '#' && *str != '$' &&
	 *str != '%' && *str != '&' && *str != '\'' && *str != '(' &&
	 *str != ')' && *str != '*' && *str != '+' && *str != ',' &&
	 *str != '-' && *str != '.' && *str != '/' && *str != ' ') {
	str++;
	skip++;
      }
    }
    
    while(*str &&
	  (*str == '!' || *str == '"' || *str == '#' || *str == '$' ||
	   *str == '%' || *str == '&' || *str == '\'' || *str == '(' ||
	   *str == ')' || *str == '*' || *str == '+' || *str == ',' ||
	   *str == '-' || *str == '.' || *str == '/' || *str == ' ' ||
	   *str == ';' || isdigit(*str) || iscntrl(*str))) {
      str++;
      skip++;
    }
  }
  else if(*str == ']') {
    do {
      str++;
      skip++;
    } while(isdigit(*str));

    if(!*str)
      return(skip);

    do {
      str++;
      skip++;
    } while(isgraph(*str) || *str == ' ');
  }
  else
    while(*str == '!' || *str == '"' || *str == '#' || *str == '$' ||
	  *str == '%' || *str == '&' || *str == '\'' || *str == '(' ||
	  *str == ')' || *str == '*' || *str == '+' || *str == ',' ||
	  *str == '-' || *str == '.' || *str == '/' || *str == ' ') {
      str++;
      skip++;
    }

  return(!*str ? skip : skip+1);
}

/******************************************************************/
/* returns the actual amount line must be advanced to skip len    */
/* non-ansi characters                                            */
/******************************************************************/

int skip_non_ansi(const char *line, int len)
{
  int ansi = 0, nonansi = 0;
  int n;

  while(line && *line && nonansi < len) {
    if (n = skip_ansi(line)) {
      ansi += n;
      line += n;
    } else {
      ++nonansi;
      ++line;
    }
  }

  return ansi + nonansi;
}

/* Strip the ansi code sequences from a string. */

char *strip_ansi(const char *str, char *buffer)
{
  int i = 0, j = 0, k;

  while(str[i])
    if(!(k = skip_ansi(&str[i])))
      buffer[j++] = str[i++];
    else
      i += k;
  buffer[j] = '\0';

  return(buffer);
}
