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
/* New for v2.0: readline support -- daw */

/* all the tab completion support is here in this one file */


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

#include "include/parse.h"
#include "include/rl.h"
#include "include/rltab.h"
#include "include/utils.h"

extern FILE *fileopen (char *filename, char *mode);

list_t *complist = 0;	/* tab completion list */

/*
 * the completion generation function for readline.
 * this function will be called repeatedly with the
 * same s -- state=0 the first time, and state!=0 the
 * rest of the time.
 * 
 * it returns a pointer to a match each time, and a
 * (char *) 0 pointer when it's done.
 */
char *tab_complete(const char *s, int state)
{
  static list_t *p;
  int len = strlen(s);
  
  if(!state) {
    if(!complist)
      return(0);
    p = complist->next;
  }
  else p = p -> next;

  for(; p; p = p->next)
    if(!strncasecmp(s, p->word, len)) {
      return(p->word);
    }
  return(0);
}

static void rltab_add(const char *word)
{
  list_t *p, *last;

  if(!complist) {
    /* add dummy header node */
    if(!(complist = (list_t *)malloc(sizeof(list_t))))
      syserr("rltab_add: malloc");
    complist->word = 0;
    if(!(p = complist->next = (list_t *)malloc(sizeof(list_t))))
      syserr("rltab_add: malloc");
    p->word = mystrdup(word);
    p->next = 0;
    return;
  }

  /* avoid duplicates */
  for(last = complist, p = complist->next; p; last = p, p = p->next)
    if(!strcasecmp(p->word, word))
      return;

  /* add to end of list */
  if(!(p = (list_t *)malloc(sizeof(list_t))))
    syserr("rltab_add: malloc");
  p->word = mystrdup(word);
  p->next = 0;
  last->next = p;
}

void rltab_delete(const char *word)
{
  list_t *p, *q;

  if(!complist) {
    tintin_puts("#Delete failed: empty completion list.", 0);
    return;
  }

  for(p = complist; p->next; p = p->next)
    if(!strcasecmp(p->next->word, word)) {
      q = p->next;
      p->next = p->next->next;
      free(q->word);
      free(q);
      tintin_puts("#Ok, deleted.", 0);
      return;
    }
  tintin_puts("#Delete failed: word not in completion list.", 0);
}

/************************/
/* the #tablist command */
/************************/

void rltab_list(void)
{
  list_t *p;
  int col = 0, ncols = 5;
  char line[128];

  if(!complist || !complist->next) {
    tintin_puts("#Empty completion list.", 0);
    return;
  }

  *line = '\0';
  for(p = complist->next; p; p = p->next) {
    sprintf(line+strlen(line), "%-15.15s ", p->word);
    if(++col == ncols) {
      tintin_puts(line, 0);
      col = 0;
      *line = '\0';
    }
  }
  if(*line)
    tintin_puts(line, 0);
}

static void rltab_purge(void)
{
  list_t *p, *q;

  if(!complist) {
    if(!(complist = (list_t *)malloc(sizeof(list_t))))
      syserr("rltab_purge: malloc");
    complist->word = 0;
    complist->next = 0;
    return;
  }

  for(p = complist->next; p; p = q) {
    q = p->next;
    free(p->word);
    free(p);
  }
  complist->next = 0;
}

/**********************/
/* the #retab command */
/**********************/

void load_tabs(void)
{
  FILE *f;
  char *s, *t, line[128];

  if(!(f = fileopen(COMPLETE_FILE, "r"))) {
    tintin_puts("#Couldn't open " COMPLETE_FILE, 0);
    return;
  }

  rltab_purge();

  while(fgets(line, sizeof(line), f)) {
    /* delete leading and trailing whitespace */
    for(s = line; isspace(*s); s++)
      ;
    for(t = s; *t && !isspace(*t); t++)
      ;
    *t = '\0';
    rltab_add(s);
  }
  fclose(f);
//  tintin_puts("#Read , completion list initialized.", 0);
}

/***********************/
/* the #tabadd command */
/***********************/

void do_tabadd(const char *arg)
{
  char buf[BUFFER_SIZE];

  if(!arg || !*arg) {
    tintin_puts("#Add failed: no word specified.", 0);
    return;
  }

  get_arg_in_braces(arg, buf, 1);
  rltab_add(buf);
  tintin_puts("#Added.", 0);
}

/***********************/
/* the #tabdel command */
/***********************/

void do_tabdel(const char *arg)
{
  char buf[BUFFER_SIZE];

  if(!arg || !*arg) {
    tintin_puts("#Delete failed: no word specified.", 0);
    return;
  }

  get_arg_in_braces(arg, buf, 1);
  rltab_delete(buf);
}

/************************/
/* the #tabsave command */
/************************/

void do_tabsave(void)
{
  FILE *f;
  list_t *p;

  if(!complist || !complist->next) {
    tintin_puts("#Empty completion list, nothing to save.", 0);
    return;
  }

  if(!(f = fileopen(COMPLETE_FILE, "w"))) {
    tintin_puts("#Couldn't open " COMPLETE_FILE, 0);
    return;
  }

  for(p = complist->next; p; p = p->next)
    fprintf(f, "%s\n", p->word);
  fclose(f);
//  tintin_puts("#Saved to ", 0);
}
