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
#include <gdk/gdkkeysyms.h>
#include "support.h"

ggmud *mud;
extern void load_win_pos();

int home_is_program_dir = 0;

gint snoop_keys (GtkWidget *grab_widget,
                 GdkEventKey *event,
                 ggmud *mud)
{
   extern gint capture_enabled;

   if (capture_enabled || !mud)
       return FALSE;

   if (event->type == GDK_KEY_PRESS) {
       // handle pageup/down to show/move review buffer, thanks to  Ingo Gambin for the idea & debug
       if (event->keyval == GDK_Page_Up || event->keyval == GDK_Page_Down) {
           if (mud->review && GTK_WIDGET_VISIBLE(mud->review)) {
               GdkRectangle rect;
               GtkTextIter iter;
               GtkTextView *tv = GTK_TEXT_VIEW(GTK_BIN(mud->review)->child);

               if (!tv)
                   return FALSE;

               gtk_text_view_get_visible_rect(tv, &rect);

               if (event->keyval == GDK_Page_Up) {
                   int pos = rect.y - rect.height;
                   if (pos < 0) pos = 0;

                   gtk_text_view_get_line_at_y(tv, &iter, pos, NULL);
               }
               else
                   gtk_text_view_get_line_at_y(tv, &iter, rect.y + rect.height, NULL);
               
               gtk_text_view_scroll_to_iter(tv, &iter, 0.0, TRUE, 0.0, 0.0);
           }
           else  {
               GtkWidget *review_toggle = lookup_widget(mud->window, "togglebutton_review");

               gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (review_toggle), TRUE);
               gtk_widget_show(mud->review);
           }

           return TRUE;
       } // handle ordinary macro keys
       else if (event->type == GDK_KEY_PRESS) 
           return check_macro(event->state , 
                   gdk_keyval_to_upper(event->keyval));
   }

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

#if defined(__APPLE__) && defined(GTKQUARTZ)
    extern void fix_bundle_environment();

    fix_bundle_environment();
#endif

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

    if (argc >= 2 && !strcmp(argv[1], "-nohome"))
        home_is_program_dir = 1;

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
    mud->maxlines = 300 * 70;

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

    atexit(save_vars);
    
    hist_add(""); /* Needed to get rid of a blank line in history list */

    mud->curr_color=prefs.DefaultColor;


    gtk_widget_show(mud->window);

    load_win_pos();


    g_timeout_add(500, (GSourceFunc)checktick, NULL);

    extern int ttmain(int argc, char* argv[]);

    ttmain(argc, argv);

// placed here since thos needs the initializations of tt++ underlying structures.
    load_aliases();
    load_variables();
    load_highlights();
    load_gags();
    load_triggers();
    load_tabs();

#ifdef WITH_LUA
    if (prefs.LuaConfig)
	    script_command(prefs.LuaConfig, NULL);
#endif

    if (prefs.WizAtStartup)
        do_wiz(NULL, NULL);

    load_buttons();

    if (prefs.AutoUpdate) {
        g_thread_init(NULL);

        g_thread_create((GThreadFunc)check_for_updates, NULL, FALSE, NULL);
    }

    gtk_window_present(GTK_WINDOW(mud->window));
    gtk_key_snooper_install((GtkKeySnoopFunc)snoop_keys, mud);
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

#ifdef GDK_WINDOWING_QUARTZ
#include <mach-o/dyld.h>
#include <sys/param.h>
void
fix_bundle_environment ()
{
	char execpath[MAXPATHLEN+1];
	char path[MAXPATHLEN * 4];
    FILE *f;

    uint32_t pathsz = sizeof (execpath);
	_NSGetExecutablePath (execpath, &pathsz);

// if we are not in a bundle let's use the system GTK enviroment...
    if (!strstr(execpath, ".app"))
        return;

	gchar * dir_path = g_path_get_dirname (execpath);
	strcpy(path, dir_path);
	strcat(path, "/../Frameworks/clearlooks");
	setenv ("GTK_PATH", path, 1);
	strcat(path + strlen(dir_path), "/../Resources/locale");
//	localedir = strdup (path);
	/* write a pango.rc file and tell pango to use it */
	strcpy(path + strlen(dir_path), "/../Resources/pango.rc");

	if ((f = fopen(path, "w"))) {
        fprintf(f, "[Pango]\nModuleFiles=%s/../Resources/pango.modules\n",
                dir_path);
        fclose(f);
		setenv ("PANGO_RC_FILE", path, 1);
    }
	// gettext charset aliases
	setenv ("CHARSETALIASDIR", path, 1);

	// data prefix
	strcpy(path + strlen(dir_path), "/../Resources/share/themes");
	setenv ("GTK_DATA_PREFIX", path, 1);
	// font config
	strcpy(path + strlen(dir_path), "/../Resources/fonts.conf");
	setenv ("FONTCONFIG_FILE", path, 1);
	// GDK Pixbuf loader module file
	strcpy(path + strlen(dir_path), "/../Resources/gdk-pixbuf.loaders");
	setenv ("GDK_PIXBUF_MODULE_FILE", path, 1);
    g_free(dir_path);
}
#endif
