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
/* file: text.c  - funtions for logfile and reading/writing comfiles */
/*                             TINTIN + +                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*                    New code by Joann Ellsworth                    */
/*********************************************************************/

#include "config.h"
#include "tintin.h"

#if defined(HAVE_STRING_H)
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#include "include/net.h"
#include "include/parse.h"
#include "include/rl.h"
#include "include/text.h"

/**********************************/
/* load a file for input to mud.  */
/**********************************/

void read_file(const char *arg, struct session *ses)
{
  FILE *myfile;
  char buffer[4*BUFFER_SIZE], *cptr;

  get_arg_in_braces(arg, buffer, 1);
  if(!ses) {
    tintin_puts("You can't read any text in without an active session.", NULL);
    prompt(NULL);
    return;
  }
     
  if(!(myfile = fopen(buffer, "r"))) {
    tintin_puts("ERROR: No file exists under that name.\n", NULL);
    prompt(NULL);
    return;
  }

  while(fgets(buffer, sizeof(buffer), myfile)) {
    for(cptr = buffer; *cptr && *cptr != '\n'; cptr++)
      ;
    *cptr = '\0';
    if(*buffer)
      write_line_mud(buffer, ses);
    else
      write_line_mud(" ", ses);
  }

  fclose(myfile);
  tintin_puts("File read - Success.\n", NULL);
  prompt(NULL);
  tintin_puts("\n", NULL);
}
