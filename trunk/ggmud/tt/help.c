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
/*****************************************************************/
/* functions for the #help command                               */
/* Some small patches by David Hedbor (neotron@lysator.liu.se)   */
/* to make it work better.                                       */
/*****************************************************************/

#include "config.h"
#include "tintin.h"

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>

#include "include/help.h"
#include "include/parse.h"
#include "include/rl.h"
#include "include/utils.h"

FILE *check_file(const char *filestring)
{
  return fopen(filestring, "r");
}

/*********************/
/* the #help command */
/*********************/

void help_command(const char *arg)
{
    FILE *myfile = NULL;
    char *cptr, text[80], line[80], filestring[500];
    int flag = TRUE, counter = 0;

    if (getenv("TINTIN_HELP") == NULL)
        sprintf(filestring, "%s/%s", getenv("HOME"), DEFAULT_HELP_FILE);
    else
        sprintf(filestring, "%s/%s", getenv("TINTIN_HELP"), DEFAULT_HELP_FILE);
    myfile = check_file(filestring);

    if(!myfile && strcmp(DEFAULT_FILE_DIR, "HOME")) {
        sprintf(filestring, "%s/%s", DEFAULT_FILE_DIR, DEFAULT_HELP_FILE);
        myfile = check_file(filestring);
    }

    if(!myfile ) {
        sprintf(filestring, "./%s", DEFAULT_HELP_FILE);
        myfile = check_file(filestring);
    }

    if(!myfile) {
        tintin_puts2("#Help file '" DEFAULT_HELP_FILE "' not found - no help available.", NULL);
        prompt(NULL);
        return;
    }

    if(*arg) {
        sprintf(text, "~%s", arg);
        cptr = text;

        while(*++cptr)
            *cptr = toupper(*cptr);

        while(flag && fgets(line, sizeof(line), myfile))
            if(*line == '~') {
                if(line[1] == '*') {
                    tintin_puts2("#Sorry, no help on that word.", NULL);
                    flag = FALSE;
                }
                else if(is_abbrev(text, line)) {
                    while(flag && fgets(line, sizeof(line), myfile)) {
                        if(*line == '~') {
                            flag = FALSE;
                        } else {
                            line[strlen(line)-1] = '\0';
                            tintin_puts2(line, NULL);
                        }
                    }
                }
            }
    }
    else
        while(fgets(line, sizeof(line), myfile))
            if(*line == '~') 
                break;
            else {
                line[strlen(line)-1] = '\0';
                tintin_puts2(line, NULL);
            }

    prompt(NULL);
    fclose(myfile);
}
