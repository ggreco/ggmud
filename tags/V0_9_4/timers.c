/*  GGMud
 *  Copyright (C) 1999 Heathen (the.heathen@swipnet.se)
 *                1999 Drizzt  (doc.day@swipnet.se)
 *                2003 Gabry (gabrielegreco@gmail.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <gtk/gtk.h>
#include "ggmud.h"
#include "include/ticks.h"
#include <stdlib.h>
#include <time.h>
#include "include/llist.h"

// #alias {celepath} {e;#wait 500;e;#wait 500;s;#wait 500;s;#wait 500;e;#wait 500;n;#wait 500;e;#wait 500;e;#wait 500;e;#wait 500;w;#wait 500;w;#wait 500;s;#wait 500;e;#wait 500}

extern char *get_arg_in_braces(char *s, char *arg, int flag);

int use_tickcounter = 0;

int timetilltick(void)
{
  int ttt;

  ttt = (time(0) - time0) % tick_size;
  ttt = (tick_size - ttt) % tick_size;
  return(ttt);
}

static timerdata *get_timer_by_name(char *left)
{
    GList *l = mud->timers;

    while (l) {
        char *name = ((timerdata *)l->data)->string;

        if (!strcmp(left, name))
            return (timerdata *) l->data;
        
        l = l -> next;
    }

    return NULL;
}


void do_gettimer(char *arg, struct session *ses)
{
  extern struct listnode *common_myvars;
  char left[BUFFER_SIZE], right[BUFFER_SIZE], arg2[BUFFER_SIZE];
  struct listnode *tempvars, *ln;
  timerdata *d;

  tempvars = (ses ? ses->myvars : common_myvars);
  arg = get_arg_in_braces(arg, left, 0);
  arg = get_arg_in_braces(arg, right, 0);

  if (!*left) {
      tintin_puts("#NO VARIABLE PARAMETER INCLUDED.  TRY AGAIN.\n", ses);
  }
  else if(!*right) {
      tintin_puts("#NO TIMER NAME INCLUDED. TRY AGAIN.\n", ses);
  }
  else if ((d = get_timer_by_name(right))) {
    time_t finish;
    
    if ((ln = searchnode_list(tempvars, left)))
      deletenode_list(tempvars, ln);

    finish = d->finish - time(NULL);
    
    sprintf(right, "%ld", finish);
    insertnode_list(tempvars, left, right, "0", ALPHA);
    
//    varnum++;
    if(0) {
      sprintf(arg2, "#Ok. $%s is now set to {%s}.", left, right);
      tintin_puts(arg2, ses);
    }
  }
  else 
      tintin_puts("#INVALID TIMER NAME.\n", ses);
}

void do_stoptimer(char *arg, struct session *ses)
{
    char left[BUFFER_SIZE];
    timerdata *d;
    char buffer[256];
    
    arg = get_arg_in_braces(arg, left, 0);

    if(!*left) {
        tintin_puts("#YOU SHOULD SPECIFY A TIMER NAME\n", ses);
        return;
    }
        
    if ((d = get_timer_by_name(left))) {
        sprintf(buffer, "#TIMER %s REMOVED.\n", left);

        mud->timers = g_list_remove(mud->timers, d);
        free(d->string);
        free(d);
    }
    else 
        sprintf(buffer, "#TIMER %s NOT FOUND.\n", left);

    tintin_puts(buffer, ses);        
}

int
put_command(char *line)
{
    mud->activesession = parse_input(line, mud->activesession);
    free(line);

    return FALSE;
}

void 
wait_command(struct session *ses, const char *arg, const char *line)
{
    char temp[BUFFER_SIZE];
    int msec;
    
    arg = get_arg_in_braces((char *)arg, temp, 0);

    if (!*temp) {
        tintin_puts("#WAIT NEEDS AT LEAST A PARAMETER, LINE SKIPPED\n", ses);
        return;
    }

    if (*arg) {
        tintin_puts("#WAIT NEEDS ONLY ONE ARGUMENT, EXTRA ARGS SKIPPED\n", ses); 
    }
    
    msec = atoi(temp);

    if (msec < 100 || msec > 1000000) {
        tintin_puts("#WAIT MUST BE BETWEEN 100 AND 1000000 MSECS\n", ses);
        return;
    }
        
    g_timeout_add(msec, (GSourceFunc)put_command, strdup(line));
}

void do_timer(char *arg, struct session *ses)
{
    timerdata *d;
    int length;
    char left[BUFFER_SIZE], right[BUFFER_SIZE];
    
    arg = get_arg_in_braces(arg, left, 0);

    if(!*left) {
        tintin_puts("#YOU SHOULD SPECIFY A TIMER PERIOD\n", ses);
        return;
    }
    
    arg = get_arg_in_braces(arg, right, 0);
    
    length = atoi(left);

    if (length < 1 || length > 7200) {
        tintin_puts("#TIMER PERIOD TOO BIG OR TOO SMALL\n", ses);
        return;
    }
    
    if ((d = malloc(sizeof(timerdata)))) {
        char buffer[200];
        
        d->last = 0;
        d->string = strdup(right);
        d->finish = time(NULL) + length;

        mud->timers = g_list_append(mud->timers, d);

        sprintf(buffer, "#NEW TIMER %s (%d seconds)\n", d->string, length);
        tintin_puts(buffer, ses);        
    }

}

static int already_asked = FALSE;
extern int need_update;

int checktick(void)
{
    extern int use_tickcounter;
    extern int show_pretick;
    static int last = -1, ttt = -1, using_tickcounter = 0; /* ttt = time to tick */
    time_t now;
    static time_t last_invisible = 0;
    GList *l = mud->timers;

    now = time(0);

    if (need_update && !already_asked) {
        already_asked = TRUE; // do this before otherwise we'll do the popup every second!
        ask_for_update();
    }

#ifdef WITH_LUA
    if (mud->lua_idle_function) {
        lua_getglobal(mud->lua, mud->lua_idle_function);

        if (lua_pcall(mud->lua, 0, 0, 0) != 0) {
            char buffer[1024];
            sprintf(buffer, "Error running idle function %s: %s\n", 
                    mud->lua_idle_function, lua_tostring(mud->lua, -1));
            textfield_add(mud->text, buffer, MESSAGE_ERR);
            lua_pop(mud->lua, 1);
        }
    }
#endif
    
// this is a workaround for a cursor invisibility problem with telnet sessions
    if (mud && mud->activesession) {
        if (last_invisible) {
            if (mud->ent->visible) {
                last_invisible = 0;
            }
            else if ((time(NULL) - last_invisible) > 30) {
                mud->ent->visible = 1;
                input_line_visible(TRUE);
            }
        }
        else if (!mud->ent->visible) {
            last_invisible = time(NULL);
        }
    }
    
    swap_blinks();
    
    while (l) {
        timerdata *d = (timerdata *) l->data;

        l = l->next;

        if (d->last > 0 && d->last < now) {
            time_t l = d->finish;

            l -= now;

            if (l == 0) {
                char buffer[BUFFER_SIZE];
                
                sprintf(buffer, "#TIMER %s EXPIRED\n", d->string);
                do_one_line(buffer, mud->activesession);

                textfield_add(mud->text, buffer, MESSAGE_TICK);

                free(d->string); free(d);
                
                mud->timers = g_list_remove(mud->timers, d);
            }
            else if (l == 10) {
                char buffer[BUFFER_SIZE];
                
                sprintf(buffer, "#TIMER %s WILL EXPIRE IN 10 SECONDS\n", d->string);
                do_one_line(buffer, mud->activesession);

                textfield_add(mud->text, buffer, MESSAGE_TICK);
                d->last = now;
            }
        }
        else
         d->last = now;
    }
    
    if(!use_tickcounter) {
        if(using_tickcounter) {
            gtk_label_set_text(mud->tick_counter, "OFF");
            using_tickcounter = 0;
        }
        return TRUE;
    }

    if(time0 <= 0)
        return TRUE;	/* big number */
 
    if(last > 0) {
        while(last <= now) {
            ttt = (++last - time0) % tick_size;
            ttt = (tick_size - ttt) % tick_size;
            if(!ttt || (ttt == 10 && show_pretick))
                if(mud->activesession && mud->activesession->tickstatus)
                {
                   char buffer[BUFFER_SIZE];
                   
                   strcpy(buffer, !ttt ? "#TICK!!!\n" :
                           "#10 SECONDS TO TICK!!!\n");
                   
                   do_one_line(buffer, mud->activesession);

                   textfield_add(mud->text, buffer, MESSAGE_TICK);
                }
        }
    } else {
        last = now + 1;
        ttt = (now - time0) % tick_size;
        ttt = (tick_size - ttt) % tick_size;
    }


    // aggiornamento del campo coi secondi con ttt
    if(mud->activesession && use_tickcounter) {
        char buffer[20];
        
        sprintf(buffer, "%d",ttt);
        
        gtk_label_set_text(mud->tick_counter, buffer);

        using_tickcounter = 1;
    }
    
    return TRUE;
}

