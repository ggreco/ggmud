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
/* file: utils.c - some utility-functions                            */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

/* note: changed a little bit for readline support -- daw */

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
// #include <sys/param.h>
#include <errno.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "include/rl.h"
#include "include/utils.h"

/**********************************************/
/* return: TRUE if s1 is an abbrevation of s2 */
/**********************************************/

int is_abbrev(const char *s1, const char *s2)
{
  if(!*s1 || !*s2)
    return(FALSE);

  if(strlen(s2) < strlen(s1))
    return(FALSE); /* not really abbrev if s2 < s1 */

  return(!strncasecmp(s2, s1, strlen(s1)));
}

/********************************/
/* strdup - duplicates a string */
/* return: address of duplicate */
/********************************/

char *mystrdup(const char *str)
{
#if 0
    char *buf;

  if(!(buf = (char *)malloc(strlen(str)+1)))
    syserr("Not enought memory for mystrdup().");
  sprintf(buf, "%s", str);
  return(buf);
#else
  return strdup(str);
#endif
}

/*************************************************/
/* print system call error message and terminate */
/*************************************************/

void syserr(const char *msg)
{
  extern int errno;

  char s[128], *syserrmsg;

  syserrmsg = strerror(errno);

  if(syserrmsg)
    sprintf(s, "ERROR: %s (%d: %s)", msg, errno, syserrmsg);
  else
    sprintf(s, "ERROR: %s (%d)", msg, errno);

  quitmsg(s);
}

 /* Whoops, strcasecmp wasn't found. */
#if !defined(HAVE_STRCASECMP)
#define UPPER(c) (islower(c) ? toupper(c) : c)

int strcasecmp(const char *string1, const char *string2)
{
  for(; UPPER(*string1) == UPPER(*string2); string1++, string2++)
    if(!*string1)
      return(0);
  return(UPPER(*string1) - UPPER(*string2));
}

int strncasecmp(const char *string1, const char *string2, size_t count)
{
  if(count)
    do {
      if(UPPER(*string1) != UPPER(*string2))
	return(UPPER(*string1) - UPPER(*string2));
      if(!*string1++)
	break;
      string2++;
    } while(--count);
  return(0);
}

#endif
