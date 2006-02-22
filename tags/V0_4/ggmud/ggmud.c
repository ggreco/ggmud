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

gint        snoop_keys              (GtkWidget *grab_widget,
                                             GdkEventKey *event,
                                             ggmud *mud)
{
   if (event->type == GDK_KEY_PRESS &&
       event->keyval >= GDK_F1 && event->keyval <= GDK_F12) {
       macro_send(NULL , event->keyval - GDK_F1);
       return TRUE;
   }
   else
       return FALSE;
}

#ifdef linux
#include "prefix.h"
#endif

int main(int argc, char **argv)
{
    char *display;
    extern int checktick(void);
    extern void save_vars(void);

#ifndef WIN32
    if(!(display = getenv("DISPLAY")) || !*display) {
        putenv("DISPLAY=:0.0");
    }
#endif
    
    gtk_set_locale ();
    gtk_init (&argc, &argv);
    gdk_init (&argc, &argv);
  
// this code set the ggmud path in a way you can find the help file
#ifdef linux
    {
        FILE *f;
        
        if ((f = fopen( DEFAULT_HELP_FILE, "r"))) {
            fclose(f);
        }
        else {
            char *path = strdup(SELFPATH), *c;

            if ((c = strrchr(path, '/'))) {
                *c = 0;

                chdir(path);

            }
            free(path);
        }
    }
#endif
    
    mud = calloc(sizeof(ggmud), 1);

    mud->hist=calloc(sizeof(struct ggmud_history),1);
    mud->hist->size=0;
    mud->hist->max=20;
    mud->hist->cur=0;
    mud->hist->pos=0;
    mud->hist->cyclic=1;
    mud->hist->list=calloc(sizeof(gpointer), (mud->hist->max+1));
    mud->lines=0;
    mud->maxlines = 300 * 70;	// This will be an option


    /* load the stuff that needs to be loaded before the GUI comes up! */
    init_colors();
    load_prefs();
    load_font();

    /* Spawn the Graphical User Interface */
    load_macro();
    spawn_gui();
    load_wizard();

    atexit(save_vars);
    
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
    load_tabs();

    gtk_window_present(GTK_WINDOW(mud->window));
    gtk_key_snooper_install(snoop_keys, mud);
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
