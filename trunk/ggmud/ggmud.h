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

#ifndef _GGMUD_H_
#define _GGMUD_H_

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "tintin.h"

#define MESSAGE_ERR     0
#define MESSAGE_NORMAL  1
#define MESSAGE_ANSI    2
#define MESSAGE_NONE    3
#define MESSAGE_SENT	4
#define MESSAGE_TICK    5

typedef struct {
    GtkWidget *menu, *hostentry, *portentry, *vbox, *macrobuttons;
    GtkWidget *window;
    GtkText *text;
    GtkEntry *ent;
    GtkLabel *tick_counter;
    GtkNotebook *notebook;
    GdkColor curr_color;
    GdkFont *disp_font;
    gchar *disp_font_name;
    struct ggmud_history *hist;
    gint lines;
    gint maxlines;
    gint totcharline;
    gboolean LOGGING;		/* Are we logging? */
    FILE *LOG_FILE;		/* File ptr to write to. */
    gchar *log_filename;	/* The name of the Log File. */
    struct session *activesession;
    int input_monitor;
} ggmud;

typedef struct alias_data  ALIAS_DATA;
typedef struct system_data SYSTEM_DATA;
typedef struct prefs_data PREFS_DATA;
typedef struct wizard_data WIZARD_DATA;

struct system_data {
        gchar     *FontName;
};

struct prefs_data {
    gint       KeepText;
    gint       EchoText;
    gint       WordWrap;
    gint       DoBeep;
    gint       Toolbar;
    gint       Macrobuttons;
    gint       Statusbar;
    GdkColor   BackgroundColor; /* Red, Green, Blue */
    GdkColor   DefaultColor; /* Red, Green, Blue */
};

struct alias_data {
    ALIAS_DATA *next;
    gchar      *alias;
    gchar      *replace;
};

struct wizard_data {
    WIZARD_DATA *next;
    gchar      *name;
    gchar      *hostname;
    gchar      *port;
    gchar      *cstring;
    gchar      *playername;
    gchar      *password;
    gint       autologin;
};

/* alias.c */
extern void  load_aliases    ( void );
extern void  window_alias    (GtkWidget *widget, gpointer data);

/* trigger.c */
extern void  load_triggers    ( void );
extern void triggers_window ();

extern ggmud *mud;
extern gchar **macro_list;
extern GtkWidget *menu_File_Connect;
extern GtkWidget *menu_File_DisConnect;
extern GtkWidget *statusbar;
extern gint statusbar_id;

/* extprefs.c */
extern void  gags_window    (GtkWidget *widget, gpointer data);
extern void  highlights_window    (GtkWidget *widget, gpointer data);
extern void  variables_window    (GtkWidget *widget, gpointer data);
extern void  load_aliases    ( void );
extern void  load_variables    ( void );
extern void  load_highlights    ( void );

/* wiz.c */
extern void  load_wizard        ( void );

/* hist.c */
struct ggmud_history{
	gchar **list;	/* the actual history */
	gchar *tmp;	/* in case something was typed before traverssing hist */
	int size;	/* current number of elements */
	int pos;	/* where to add */
	int cur;	/* which is displayed */
	int max;	/* total number of elements :) */
	int cyclic;	/* controls cyclic vs linear history */
};

extern void hist_add(char *);
extern gint hist_evt(GtkWidget*,GdkEventKey*,gpointer);
gint change_focus (GtkWidget* w, GdkEventKey* event, gpointer data);
extern void hist_clear();

/* macro.c */
extern void  load_macro    ( void );
extern void  window_macro    (GtkWidget *widget, gpointer data);


/* win.c */
extern void spawn_gui( void );
extern void textfield_add(const char *, int);
extern void textfield_freeze();
extern void textfield_unfreeze();
extern void clear(int,GtkText *);
extern void cbox(void);

/* font.c */
extern void window_font ( GtkWidget *widget, gpointer data   );
extern void load_font ();
extern void save_font ();
extern FILE *fileopen (gchar *filename, gchar *mode);

//extern GtkWidget *window;

/* net.c */
extern int connected;
extern void disconnect ( void );
extern void make_connection ( char *name, char *host, char *port );
extern void open_connection (  const char *name, const char *host, const char *port );
extern void send_to_connection ( GtkWidget *, gpointer data );
extern void read_from_connection (gpointer, gint , GdkInputCondition );

/* window.c */
extern GtkWidget *menu_File_wiz;	
extern GtkWidget *menu_Option_Font;	
extern GtkWidget *menu_Option_Preference;
GtkWidget *menu_Option_Colors;
extern GtkWidget *menu_Tools_Alias;	
extern GtkWidget *menu_Tools_Macro;
extern GtkWidget *menu_Tools_Trigger;
extern GtkWidget *menu_Tools_Variable;	
extern GtkWidget *menu_Tools_Highlight;
extern GtkWidget *menu_Tools_Gag;
extern GtkWidget *menu_Tools_Logger;
extern GtkWidget *menu_Tools_Logview;
extern void do_menu(GtkWidget *);

extern void popup_window ( const gchar *message );

extern GdkFont  *font_bold;
extern GdkFont  *font_normal;

/* prefs.c */
void  load_prefs ( void );
void  window_prefs ( GtkWidget *widget, gpointer data );
void color_prefs (GtkWidget *widget, GtkWidget *dummy);
void load_tt_prefs(void);
void save_all_prefs(void);

/* help.c */
extern void do_about();
extern void do_manual();

/* wiz.c */
extern void do_wiz( GtkWidget *widget, gpointer data );
extern void connection_send ( gchar *message );


/* log.c */
extern void do_log ();

/* logviewer.c */
extern void log_viewer ();
extern void TextPaste (GtkWidget *widget, gpointer data);
extern void TextCopy (GtkWidget *widget, gpointer data);


/* win.c */
extern void  close_window ( GtkWidget *widget, gpointer data );
extern void init_colors();

/* ansi.c */
extern void disp_ansi(int, const char *, GtkText *);

/* BOLD = bright color. LOW = darkish color */
extern GdkColor color_lightwhite;	/* BOLD white  */
extern GdkColor color_white;		/* LOW white */
extern GdkColor color_lightblue;	/* BOLD blue */
extern GdkColor color_blue;		/* LOW blue */
extern GdkColor color_lightgreen;	/* BOLD green */
extern GdkColor color_green;		/* LOW green */
extern GdkColor color_lightred;		/* BOLD red */
extern GdkColor color_red;		/* LOW red */
extern GdkColor color_lightyellow;	/* BOLD yellow */
extern GdkColor color_yellow;		/* LOW yellow ( brown )*/
extern GdkColor color_lightmagneta;	/* BOLD magneta ( pink ) */
extern GdkColor color_magneta;		/* LOW magneta ( purple ) */
extern GdkColor color_lightcyan;	/* BOLD cyan */
extern GdkColor color_cyan;		/* LOW cyan */
extern GdkColor color_lightblack;	/* BOLD grey (highlighted black) */
extern GdkColor color_black;		/* LOW black (same as background, hmm gotta fix it!) */
extern GdkColor *foreground;		/* foreground color */


extern SYSTEM_DATA font;

extern PREFS_DATA prefs;

extern ALIAS_DATA *alias_list;

extern WIZARD_DATA *wizard_connection_list;



#endif /* _GGMUD_H_ */
