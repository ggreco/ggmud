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
/* file: files.c - funtions for logfile and reading/writing comfiles */
/*                             TINTIN + +                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*                    New code by Bill Reiss 1993                    */
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

#include "include/files.h"
#include "include/llist.h"
#include "include/main.h"
#include "include/misc.h"
#include "include/parse.h"
#include "include/rl.h"

struct completenode *complete_head;

/**********************************/
/* load a completion file         */
/**********************************/

void read_complete(void)
{
  FILE *myfile;
  char buffer[BUFFER_SIZE], *cptr;
  struct completenode *tcomplete, *tcomp2;

  if(!(complete_head = (struct completenode *)malloc(sizeof(struct completenode)))) {
    fprintf(stderr, "couldn't alloc completehead\n");
    exit(1);
  }
  tcomplete = complete_head;

  if(!(myfile = fopen("tab.txt", "r")))
    if((cptr = (char *)getenv("HOME"))) {
      sprintf(buffer, "%s" , cptr);
      strcat(buffer, "/.tab.txt");
      myfile = fopen(buffer, "r");
    }
  if(!myfile) {
    tintin_puts("#no tab.txt file, no completion list\n", NULL);
    return;
  }

  while(fgets(buffer, sizeof(buffer), myfile)) {
    for(cptr = buffer; *cptr && *cptr != '\n'; cptr++);
    *cptr = '\0';
    if(!(tcomp2 = (struct completenode *)malloc(sizeof(struct completenode)))) {
      fprintf(stderr, "couldn't alloc comletehead\n");
      exit(1);
    }

    if(!(cptr = (char *)malloc(strlen(buffer)+1))) {
      fprintf(stderr, "couldn't alloc memory for string in complete\n");
      exit(1);
    }
    sprintf(cptr, "%s", buffer);
    tcomp2->strng = cptr;
    tcomplete->next = tcomp2;      
    tcomplete = tcomp2;
  }
  tcomplete->next = NULL;
  fclose(myfile);
  tintin_puts("#tab.txt file loaded.\n", NULL);
  prompt(NULL);
  tintin_puts("\n", NULL);
}

/********************/
/* the #log command */
/********************/

void log_command(const char *arg, struct session *ses)
{
  if(ses) {
    if(!ses->logfile) {
      if(*arg) {

	if (append_log == 1) {
	  if((ses->logfile = fopen(arg, "a"))) {
	    tintin_puts("#OK. LOGGING (appending).....", ses);
	  } else {
	    tintin_puts("#COULDN'T OPEN FILE.", ses);
	  }
	}
	else {
	  if ((ses->logfile = fopen(arg, "w"))) {
	    tintin_puts("#OK. LOGGING (overwriting).....", ses);
	  } else {
	    tintin_puts("#COULDN'T OPEN FILE.", ses);
	  }
	}
	
      } else {
	tintin_puts("#SPECIFY A FILENAME.", ses);
      }
    } else {
      fclose(ses->logfile);
      ses->logfile = NULL;
      tintin_puts("#OK. LOGGING TURNED OFF.", ses);
    }
  } else {
    tintin_puts("#THERE'S NO SESSION TO LOG.", ses);
  }
  prompt(NULL);
}

/***********************************/
/* read and execute a command file */
/***********************************/

struct session *read_command(const char *filename, struct session *ses)
{
  FILE *myfile;
  char buffer[BUFFER_SIZE], *cptr;
  char message[80];
  int flag = TRUE;

  get_arg_in_braces(filename, buffer, 1);
  if(!(myfile = fopen(buffer, "r"))) {
    tintin_puts("#ERROR - COULDN'T OPEN THAT FILE.", ses);
    prompt(NULL);
    return(ses);
  }

  if(!verbose)
    puts_echoing = FALSE;
  alnum = 0;
  acnum = 0;
  subnum = 0;
  varnum = 0;
  hinum = 0;
  antisubnum = 0;
  funcnum = 0;

  while(fgets(buffer, sizeof(buffer), myfile)) {
    if(flag) {
      puts_echoing = TRUE;
      char_command(buffer, ses);
      if(!verbose)
	puts_echoing = FALSE;
      flag = FALSE;
    }

    for(cptr = buffer; *cptr && *cptr != '\n'; cptr++)
      ;
    *cptr = '\0';

    if (buffer[0] != '\0')
      ses = parse_input(buffer, ses); 
  }
  if(!verbose) {
    puts_echoing = TRUE;
    sprintf(message,"#OK. %d ALIASES LOADED.", alnum);
    tintin_puts2(message, ses);
    sprintf(message,"#OK. %d ACTIONS LOADED.", acnum);
    tintin_puts2(message, ses);
    sprintf(message,"#OK. %d ANTISUBS LOADED.", antisubnum);
    tintin_puts2(message, ses);
    sprintf(message,"#OK. %d SUBSTITUTES LOADED.", subnum);
    tintin_puts2(message, ses);
    sprintf(message,"#OK. %d VARIABLES LOADED.", varnum);
    tintin_puts2(message, ses);
    sprintf(message,"#OK. %d HIGHLIGHTS LOADED.", hinum);
    tintin_puts2(message, ses);
    sprintf(message,"#OK. %d FUNCTIONS LOADED.", funcnum);
    tintin_puts2(message, ses);
  }
  fclose(myfile);
  prompt(NULL);
  return(ses);
}

/************************/
/* write a command file */
/************************/

struct session *write_command(const char *filename, struct session *ses)
{
  FILE *myfile;
  char buffer[BUFFER_SIZE];
  struct listnode *nodeptr;

  get_arg_in_braces(filename, buffer, 1);
  if(!*buffer) {
    tintin_puts("#ERROR - COULDN'T OPEN THAT FILE.", ses);
    prompt(NULL);
    return(0); /* added zero return */
  }

  if(!(myfile = fopen(buffer, "w"))) {
    tintin_puts("#ERROR - COULDN'T OPEN THAT FILE.", ses);
    prompt(NULL);
    return(0); /* added zero return */
  }

  nodeptr = (ses ? ses->aliases : common_aliases);
  while((nodeptr = nodeptr->next)) {
    prepare_for_write("alias", nodeptr->left, nodeptr->right, "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->actions : common_actions);
  while((nodeptr = nodeptr->next)) {
    prepare_for_write("action", nodeptr->left, nodeptr->right, nodeptr->pr, buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->antisubs : common_antisubs);
  while((nodeptr = nodeptr->next)) {
    prepare_for_write("antisubstitute", nodeptr->left, "", "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->subs : common_subs);
  while((nodeptr = nodeptr->next)) {
    prepare_for_write("substitute", nodeptr->left, nodeptr->right, "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->myvars : common_myvars);
  while((nodeptr = nodeptr->next)) {
    prepare_for_write("variable", nodeptr->left, nodeptr->right, "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->highs : common_highs);
  while((nodeptr = nodeptr->next)) {
    prepare_for_write("highlight", nodeptr->right, nodeptr->left, "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->pathdirs : common_pathdirs);
  while((nodeptr = nodeptr->next)) {
    prepare_for_write("pathdir", nodeptr->right, nodeptr->left, "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->myfuncs : common_functions);
  while((nodeptr = nodeptr->next)) {
    prepare_for_write("function", nodeptr->left, nodeptr->right, "", buffer);
    fputs(buffer, myfile);
  }
  
  if (prompt_on == 1) {
    prepare_for_write("setprompt", prompt_line,"", "", buffer);
    fputs(buffer, myfile);
  }
  fclose(myfile);
  tintin_puts("#COMMANDO-FILE WRITTEN.", ses);
  return(ses);
}

/************************/
/* write a command file */
/************************/

struct session *writesession_command(const char *filename, struct session *ses)
{
  FILE *myfile;
  char buffer[BUFFER_SIZE];
  struct listnode *nodeptr;

  get_arg_in_braces(filename, buffer, 1);
  if(!*buffer) {
    tintin_puts("#ERROR - COULDN'T OPEN THAT FILE.", ses);
    prompt(NULL);
    return(0); /* added zero return */
  }

  if(!(myfile = fopen(buffer, "w"))) {
    tintin_puts("#ERROR - COULDN'T OPEN THAT FILE.", ses);
    prompt(NULL);
    return(0); /* added zero return */
  }

  nodeptr = (ses ? ses->aliases : common_aliases);
  while((nodeptr = nodeptr->next)) {
    if(ses && searchnode_list(common_aliases, nodeptr->left))
      continue;
    prepare_for_write("alias", nodeptr->left, nodeptr->right, "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->actions : common_actions);
  while((nodeptr = nodeptr->next)) {
    if(ses && searchnode_list(common_actions, nodeptr->left))
      continue;
    prepare_for_write("action", nodeptr->left, nodeptr->right, nodeptr->pr, buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->antisubs : common_antisubs);
  while((nodeptr = nodeptr->next)) {
    if(ses && searchnode_list(common_antisubs, nodeptr->left))
      continue;
    prepare_for_write("antisubstitute", nodeptr->left, "", "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->subs : common_subs);
  while((nodeptr = nodeptr->next)) {
    if(ses && searchnode_list(common_subs, nodeptr->left))
      continue;
    prepare_for_write("substitute", nodeptr->left, nodeptr->right, "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->myvars : common_myvars);
  while((nodeptr = nodeptr->next)) {
    if(ses && searchnode_list(common_myvars, nodeptr->left))
      continue;
    prepare_for_write("variable", nodeptr->left, nodeptr->right, "", buffer);
    fputs(buffer, myfile);
  }

  nodeptr = (ses ? ses->highs : common_highs);
  while((nodeptr = nodeptr->next)) {
    if(ses && searchnode_list(common_highs, nodeptr->left))
      continue;
    prepare_for_write("highlight", nodeptr->right, nodeptr->left, "", buffer);
    fputs(buffer, myfile);
  }

  fclose(myfile);
  tintin_puts("#COMMANDO-FILE WRITTEN.", ses);
  return(ses);
}


void prepare_for_write(const char *command,
		       const char *left, const char *right, const char *pr,
		       char *result)
{
  sprintf(result, "%c%s {%s}", tintin_char, command, left);
  if(*right)
    sprintf(result+strlen(result), " {%s}", right);
  if(*pr)
    sprintf(result+strlen(result), " {%s}", pr);
  strcat(result, "\n");
}
/**********************************************************/
/* the #lowlog command                                    */
/* log performed at the most low level of read_mud_buffer */
/* I do it for development purpose. - ycjhi               */
/**********************************************************/

void lowlog_command(const char *arg, struct session *ses)
{
  if(ses) {
    if(!ses->lowlogfile) {
      if(*arg) {
	if((ses->lowlogfile = fopen(arg, "w"))) {
	  tintin_puts("#OK. LOW-LEVEL LOGGING.....", ses);
	  fputs("REMARK: (old_more_coming,more_coming,didget)\n", 
		ses->lowlogfile) ;
	} else {
	  tintin_puts("#COULDN'T OPEN FILE.", ses); 
	}
      } else {
	tintin_puts("#SPECIFY A FILENAME.", ses);
      }
    } else {
      fclose(ses->lowlogfile);
      ses->lowlogfile = NULL;
      tintin_puts("#OK. LOW-LEVEL LOGGING TURNED OFF.", ses);
    }
  } else {
    tintin_puts("#THERE'S NO SESSION TO LOG.", ses);
  }

  prompt(NULL);
}

/**************************************************************************/
/* New and improved #read command can read nicely formatted command files */
/* Added by Greg Milford  4/2000                                          */
/* Modified by Davin Chan to fix some issues                              */
/**************************************************************************/
struct session *read_command_new(char *filename, struct session *ses)
{
  FILE *myfile;
  char buffer[BUFFER_SIZE], tempbuffer[BUFFER_SIZE], *cptr;
  char message[80];
  int flag;
  int num_opening = 0;
  int num_closing = 0;
  int line_number = 0;
  int bufferlength;
  int startlastcommand = 1;
  int i;
  tempbuffer[0] = '\0';


  flag = TRUE;
  get_arg_in_braces(filename, filename, 1);

  if ((myfile = fopen(filename, "r")) == NULL)
  {
    tintin_puts("#ERROR - COULDN'T OPEN THAT FILE.", ses);
    prompt(NULL);
    return ses;
  }

  if (!verbose)
    puts_echoing = FALSE;

  alnum = 0;
  acnum = 0;
  subnum = 0;
  varnum = 0;
  hinum = 0;
  antisubnum = 0;
  funcnum = 0;

  while (fgets(buffer, sizeof(buffer), myfile))
  {
	line_number++;

    if (flag)
    {
      puts_echoing = TRUE;
      char_command(buffer, ses);

      if (!verbose)
	puts_echoing = FALSE;

      flag = FALSE;
    }

    /*  This for loop traverses a single line of the command file. */
    for (cptr = buffer; *cptr && *cptr != '\n'; cptr++) {
      if (*cptr == '{')
        num_opening++;
	  if (*cptr == '}')
	    num_closing++;
	}

    /* trim off spaces from ends of strings */
	
	while (*(cptr - 1) == ' ') {
	  cptr--;
	}
	/* think this should be *cptr = '\0'; because you want to terminate
	   the string at the \n and not at the character before it
	*(cptr - 1) = '\0';
	*/
	*cptr = '\0';

	/* trim spaces of front of new string buffer */
	cptr = buffer;
	
	while (*cptr == ' ') {
	  cptr++;
	}

	/* the following lines talley up how long a command is getting
	   to catch a brackets error. */
	bufferlength = 0;
	while (tempbuffer[bufferlength] != '\0') {
	  bufferlength++;
	}
	
	for (i = 0; buffer[i] != '\0';i++) {
	  bufferlength++;
	}

	/*  This catches a buffer overflow before it happens and makes
	    a good guess as to where the problem started.
	    If it catches it, it will close the file and exit cleanly 
	    back to tintin.  All commands prior to the erroneous one
	    were successfully read into the interpreter. */
	if (bufferlength > 2040) {
	  puts_echoing = TRUE;
	  sprintf(message, "#OK. %d ALIASES LOADED.", alnum);
	  tintin_puts2(message, ses);
	  sprintf(message, "#OK. %d ACTIONS LOADED.", acnum);
	  tintin_puts2(message, ses);
	  sprintf(message, "#OK. %d ANTISUBS LOADED.", antisubnum);
	  tintin_puts2(message, ses);
	  sprintf(message, "#OK. %d SUBSTITUTES LOADED.", subnum);
	  tintin_puts2(message, ses);
	  sprintf(message, "#OK. %d VARIABLES LOADED.", varnum);
	  tintin_puts2(message, ses);
	  sprintf(message, "#OK. %d HIGHLIGHTS LOADED.", hinum);
	  tintin_puts2(message, ses);
	  sprintf(message, "#OK. %d FUNCTIONS LOADED.", funcnum);
	  tintin_puts2(message, ses);
	  sprintf(message, " ");
	  tintin_puts2(message, ses);
	  sprintf(message, "Buffer overflow on line %d of the command file.", 
		  line_number);
	  tintin_puts2(message, ses);
  	  sprintf(message, "Check for a brace error possibly in your tintin command starting at line %d of your command file.", startlastcommand);
	  tintin_puts2(message, ses);
	  fclose(myfile);
	  prompt(NULL);
	  return ses;
	}

	/*  Concatenate buffer to what previously existed */
	strcat(tempbuffer, cptr);
	/*  make sure cptr is pointing to end of full buffer when exiting. */
	
	/* dsc : yes I know, this is an ugly ugly ugly hack :P */
	if (!strncmp(tempbuffer, "#nop", 4)) {
	
	  num_closing = 0;
	  num_opening = 0;
	  tempbuffer[0] = '\0';
	  startlastcommand = line_number + 1;
	}

	else if (num_opening == num_closing) {
	  num_closing = 0;
	  num_opening = 0;
	  if (tempbuffer[0] != '\0')
	    ses = parse_input(tempbuffer, ses);
	  tempbuffer[0] = '\0';
	  startlastcommand = line_number + 1;
	}
  }

  if (!verbose)
  {
    puts_echoing = TRUE;
    sprintf(message, "#OK. %d ALIASES LOADED.", alnum);
    tintin_puts2(message, ses);
    sprintf(message, "#OK. %d ACTIONS LOADED.", acnum);
    tintin_puts2(message, ses);
    sprintf(message, "#OK. %d ANTISUBS LOADED.", antisubnum);
    tintin_puts2(message, ses);
    sprintf(message, "#OK. %d SUBSTITUTES LOADED.", subnum);
    tintin_puts2(message, ses);
    sprintf(message, "#OK. %d VARIABLES LOADED.", varnum);
    tintin_puts2(message, ses);
    sprintf(message, "#OK. %d HIGHLIGHTS LOADED.", hinum);
    tintin_puts2(message, ses);
    sprintf(message, "#OK. %d FUNCTIONS LOADED.", funcnum);
    tintin_puts2(message, ses);
  }

  fclose(myfile);
  prompt(NULL);

  return ses;
}
