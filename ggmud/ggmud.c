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
#include <string.h>
#include <signal.h>

ggmud *mud;
extern void load_win_pos();

/* menu items */
GtkWidget *menu_File_Connect;
GtkWidget *menu_File_DisConnect;

/* statusbar */
GtkWidget *statusbar;
gint statusbar_id;

/* Global macro list */

/* Initialize the list of keys */
gchar *keys[] = {"F1", "F2", "F3", "F4", "F5",
	         "F6", "F7", "F8", "F9", "F10", "F11", "F12", NULL};


gint snoop_keys (GtkWidget *grab_widget,
                                             GdkEventKey *event,
                                             ggmud *mud)
{
   extern gint capture_enabled;

   if (event->type == GDK_KEY_PRESS && !capture_enabled) {
       return check_macro(event->state , 
               gdk_keyval_to_upper(event->keyval));
   }
   else
       return FALSE;
}

#ifdef linux
#include "prefix.h"
#include <unistd.h>
#endif

void
set_default_prefs(void)
{
    prefs.DefaultGdkColor.red = 50000;
    prefs.DefaultGdkColor.green = 50000;
    prefs.DefaultGdkColor.blue = 50000;
    prefs.KeepText = 1;
    prefs.EchoText = 1;
    prefs.WordWrap = 1;
    prefs.DoBeep = 1;
}

int main(int argc, char **argv)
{
    extern int checktick(void);
    extern void save_vars(void);

#ifndef WIN32
    char *display;

    signal(SIGPIPE, SIG_IGN);
        
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

#elif defined(WIN32) 
	{
		extern void winsock_init();

		winsock_init();
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

#ifdef WITH_LUA
    if ((mud->lua = lua_open()))
        init_lua();
#endif
    /* load the stuff that needs to be loaded before the GUI comes up! */
    init_colors();

    set_default_prefs();

    load_prefs();
    load_font();

    /* Spawn the Graphical User Interface */
    load_macro();
    spawn_gui();
    load_wizard();

    init_lua_gtk2();

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

//    chdir("GGMud.app/Contents/Resources/bin"); it was needed to find the help file

#ifdef WITH_LUA
    if (prefs.LuaConfig)
	    script_command(prefs.LuaConfig);
#endif

    if (prefs.WizAtStartup)
        do_wiz(NULL, NULL);

    gtk_window_present(GTK_WINDOW(mud->window));
    gtk_key_snooper_install(snoop_keys, mud);
    gtk_main();
	
    return 0;
}			

#if defined(WIN32) && !defined(__GNUC__)

int _stdcall
WinMain (int hInstance, int hPrevInstance, char *lpszCmdLine, int nCmdShow)
{
  return main (__argc, __argv);
}

#endif
