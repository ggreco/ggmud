/*  GGMud
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
#include <string.h>

ggmud *mud;

/* menu items */
GtkWidget *menu_File_Connect;
GtkWidget *menu_File_DisConnect;

/* statusbar */
GtkWidget *statusbar;
gint statusbar_id;

/* Global macro list */
gchar **macro_list;


/* Initialize the list of keys */
gchar *keys[] = {"F1", "F2", "F3", "F4", "F5",
	         "F6", "F7", "F8", "F9", "F10", "F11", "F12", NULL};

void
quitmsg(char *s)
{
    fputs(s,stderr);
    
    exit(0);
}

int use_tickcounter = 0;

int timetilltick(void)
{
  int ttt;

  ttt = (time(0) - time0) % tick_size;
  ttt = (tick_size - ttt) % tick_size;
  return(ttt);
}

void do_timer(char *arg, struct session *ses)
{
    extern char *get_arg_in_braces(char *s, char *arg, int flag);
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
    
    if (d = malloc(sizeof(timerdata))) {
        char buffer[200];
        
        d->last = 0;
        d->string = strdup(right);
        d->finish = time(NULL) + length;

        mud->timers = g_list_append(mud->timers, d);

        sprintf(buffer, "#NEW TIMER %s (%d seconds)\n", d->string, length);
        tintin_puts(buffer, ses);        
    }
}

int checktick(void)
{
    extern int use_tickcounter;
    extern int show_pretick;
    static int last = -1, ttt = -1, using_tickcounter = 0; /* ttt = time to tick */
    int now, found = 0;

    now = time(0);

    GList *l = mud->timers;
    
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
            if(!ttt || ttt == 10)
                if(mud->activesession && mud->activesession->tickstatus)
                {
                    if (!ttt)
                        textfield_add(mud->text, "#TICK!!!\n", MESSAGE_TICK);
                    else if (show_pretick)
                        textfield_add(mud->text, "#10 SECONDS TO TICK!!!\n", MESSAGE_TICK);
                    /*	    tintin_puts(!ttt ? "#TICK!!!" : "#10 SECONDS TO TICK!!!", s); */
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

int main(int argc, char **argv)
{
    char *display;
    extern int checktick(void);

#ifndef WIN32
    if(!(display = getenv("DISPLAY")) || !*display) {
        putenv("DISPLAY=:0.0");
    }
#endif
    
    gtk_set_locale ();
    gtk_init (&argc, &argv);
    gdk_init (&argc, &argv);
    
    mud = calloc(sizeof(ggmud), 1);

    mud->hist=calloc(sizeof(struct ggmud_history),1);
    mud->hist->size=0;
    mud->hist->max=20;
    mud->hist->cur=0;
    mud->hist->pos=0;
    mud->hist->cyclic=1;
    mud->hist->list=calloc(sizeof(gpointer), (mud->hist->max+1));
    mud->disp_font_name="fixed";
    mud->lines=0;
    mud->maxlines = 300 * 70;	// This will be an option


    /* load the stuff that needs to be loaded before the GUI comes up! */
    load_prefs();

    /* Spawn the Graphical User Interface */
    load_macro();
    spawn_gui();
    init_colors();
    load_font();
    load_wizard();

    hist_add(""); /* Needed to get rid of a blank line in history list */

    mud->curr_color=prefs.DefaultColor;


    gtk_widget_show(mud->window);

    load_win_pos();

    g_timeout_add(500, (GSourceFunc)checktick, NULL);

    ttmain(argc, argv);

// placed here since thos needs the initializations of tt++ underlying structures.
    load_aliases();
    load_variables();
    load_highlights();
    load_gags();
    load_triggers();

    gtk_main();
	
    return 0;
}			

#ifdef WIN32

int _stdcall
WinMain (int hInstance, int hPrevInstance, char *lpszCmdLine, int nCmdShow)
{
  return main (__argc, __argv);
}

#endif
