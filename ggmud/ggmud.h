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
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "tintin.h"
#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif
#include "socks.h"
#include "msp.h"

#define MESSAGE_ERR     0
#define MESSAGE_NORMAL  1
#define MESSAGE_ANSI    2
#define MESSAGE_NONE    3
#define MESSAGE_SENT	4
#define MESSAGE_TICK    5
#define MESSAGE_LOCAL   6
#define MESSAGE_SCROLLING_ANSI  7

#ifdef WITH_LUA
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

extern void init_lua();
#endif

typedef struct {
    time_t finish;
    time_t last;
    char *string;
} timerdata;

typedef struct {
    GtkWidget *menu, *vbox, *macrobuttons;
    GtkWidget *window;
    GtkTextView *text;
    GtkWidget *review;
    GtkEntry *ent;
    GtkLabel *tick_counter;
    GtkNotebook *notebook;
    GtkTextTag *curr_color;
    struct ggmud_history *hist;
    gint lines;
    gint maxlines;
    gint totcharline;
    gboolean LOGGING;		/* Are we logging? */
    FILE *LOG_FILE;		/* File ptr to write to. */
    gchar *log_filename;	/* The name of the Log File. */
    struct session *activesession;
    int input_monitor;
    GList *timers;
#ifdef WITH_LUA
    lua_State *lua;
    char *lua_idle_function;
    char *lua_filter_function;
#endif
    struct connreq *conn;
    msp_control *msp;
    // chat support
    int chat_input;
    int chat_socket;
    int chat_size;
    int chat_offset;
    char *chat_buffer;
} ggmud;

typedef struct alias_data  ALIAS_DATA;
typedef struct prefs_data PREFS_DATA;
typedef struct wizard_data WIZARD_DATA;

struct prefs_data {
    gint       KeepText;
    gint       EchoText;
    gint       WordWrap;
    gint       Blinking;
    gint       DoBeep;
    gint       Toolbar;
    gint       Macrobuttons;
    gint       Statusbar;
    GtkTextTag *BackgroundColor; /* Red, Green, Blue */
    GtkTextTag *DefaultColor; /* Red, Green, Blue */
    GtkTextTag *BrightColor;
    GdkColor BackgroundGdkColor; /* Red, Green, Blue */
    GdkColor DefaultGdkColor; /* Red, Green, Blue */
    GdkColor BrightGdkColor;
    gint       SaveVars;
    char *LuaConfig;
    gint    UseSocks;
    int socks_protocol;
    char socks_user[32];
    char socks_password[32];
    char socks_addr[64];
    unsigned short socks_port;
    gint WizAtStartup;
    gint SkipTaskbar;
    gint AutoUpdate;
    gint UseMSP;
    char SoundPath[256];
    char MusicPath[256];
    char SoundPlayer[128];
    char MusicPlayer[128];
    gint MSPDownload;
    gint MSPInline;
    gint ShowURL;
    char default_host[256];
    gint default_port;
};

#define WARN GTK_MESSAGE_WARNING
#define ERR GTK_MESSAGE_ERROR
#define INFO GTK_MESSAGE_INFO
#define QUEST GTK_MESSAGE_QUESTION


#define text_insert(w, text) gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(w)), text, -1)
#define text_bg(w, color) gtk_widget_modify_base((GtkWidget *)w, GTK_STATE_NORMAL, &color)

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

/* sub.c */
extern void  load_subs    ( void );
extern void  window_subs    (GtkWidget *widget, gpointer data);

/* alias.c */
extern void  load_aliases    ( void );
extern void  window_alias    (GtkWidget *widget, gpointer data);

/* trigger.c */
extern void  load_triggers    ( void );
extern void triggers_window ();

extern ggmud *mud;

/* from tintin */
extern void parse_config(FILE *, struct session *);
extern struct session *parse_input(const char *, struct session *);
extern void do_one_line(char *, struct session *);
extern void cleanup_session(struct session *);
extern int check_one_action(const char *, const char *, struct session *);
extern int read_buffer_mud(char *, struct session *);
extern void logit(struct session *, const char *);
extern struct session *read_command(const char *, struct session *);
extern struct session *write_command(const char *, struct session *);
extern void tickon_command(struct session *);
extern void tickoff_command(struct session *);

/* extprefs.c */
extern void  create_complete_window    (GtkWidget *widget, gpointer data);
extern void  gags_window    (GtkWidget *widget, gpointer data);
extern void  highlights_window    (GtkWidget *widget, gpointer data);
extern void  variables_window    (GtkWidget *widget, gpointer data);
extern void  load_gags    ( void );
extern void  load_variables    ( void );
extern void  load_highlights    ( void );
extern void  load_buttons (void);
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

extern void hist_add(const char *);
extern gint hist_evt(GtkWidget*,GdkEventKey*,gpointer);
gint change_focus (GtkWidget* w, GdkEventKey* event, gpointer data);
extern void hist_clear();

/* macro.c */
extern void  load_macro    ( void );
extern void  window_macro    (GtkWidget *widget, gpointer data);
extern gint check_macro(gint, gint);

/* win.c */
typedef struct {
    char name[32];
    GtkWidget *listptr;    
} window_entry;

extern void spawn_gui( void );
extern void textfield_add(GtkTextView *, const char *, int);
extern void textfield_freeze();
extern void textfield_unfreeze();
extern void clear(int,GtkTextView *);
extern void cbox(void);
extern window_entry *create_new_entry(const char *title, int width, int height);
extern window_entry *in_window_list(const char *tag);


/* font.c */
extern void window_font ( GtkWidget *widget, gpointer data   );
extern void load_font ();
extern void save_font ();
extern FILE *fileopen (gchar *filename, gchar *mode);
extern void openurl(const char *);
//extern GtkWidget *window;

/* net.c */
extern int connected;
extern void disconnect ( void );
extern void make_connection ( const char *name, const char *host, const char *port );
extern void open_connection (  const char *name, const char *host, const char *port );
extern void send_to_connection ( GtkWidget *, gpointer data );
extern void read_from_connection (gpointer, gint , GdkInputCondition );
extern void tintin_puts(const char *, struct session *);

/* window.c */
extern void do_menu(GtkWidget *);
extern void input_line_visible(int);
extern void popup_window (int type, const gchar *message, ... );
extern void clear_text_widget(GtkTextView *);

/* prefs.c */
void  load_prefs ( void );
void  window_prefs ( GtkWidget *widget, gpointer data );
void color_prefs (GtkWidget *widget, GtkWidget *dummy);
void load_tt_prefs(void);
void load_zmud_prefs(void);
void save_all_prefs(void);
void triggerclass_window(GtkWidget *widget, gpointer data );
/* help.c */
extern void do_about();
extern void do_manual();

/* wiz.c */
extern void do_wiz( GtkWidget *widget, gpointer data );
extern void connection_send ( gchar *message );

/* lua.c */
#include "include/lua.h"
extern void gmcp(char *value);

/* log.c */
extern void do_log ();

/* logviewer.c */
extern void log_viewer ();
extern void TextPaste (GtkWidget *widget, gpointer data);
extern void TextCopy (GtkWidget *widget, gpointer data);

/* callbacks.c */
extern void openurl(const char *);

/* autoupdate.c */
void check_for_updates();
void ask_for_update();

/* win.c */
extern void find_in_list(GtkCList *clist, GtkWidget *widget);
extern void  close_window ( GtkWidget *widget, gpointer data );
extern void  kill_window ( GtkWidget *widget, gpointer data );
extern void init_colors();

/* ansi.c */
extern void disp_ansi(int, const char *, GtkTextView *);
extern void local_disp_ansi(int, const char *, GtkTextView *);
extern void update_color_tags(GdkColor *);
extern void swap_blinks(void);

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


extern PREFS_DATA prefs;

extern ALIAS_DATA *alias_list;

extern WIZARD_DATA *wizard_connection_list;

struct GGFont
{
    char *config;
    char *widget;
    char *label;
    char *name;
    PangoFontDescription *desc;
};

typedef struct GGFont GGFont;
enum {OUTPUT_FONT, INPUT_FONT, INTERFACE_FONT, FONTS_NUM};

extern GGFont fonts[FONTS_NUM];


extern void set_style();

extern void AddButtonBar(GtkWidget *vbox, gpointer *data,
        GtkSignalFunc add_func,
        GtkSignalFunc del_func,
        GtkSignalFunc save_func
        );

extern void AddSimpleBar(GtkWidget *vbox, gpointer *data,
        GtkSignalFunc save_func, GtkSignalFunc close_func
        );

extern char *ParseAnsiColors(const char *);
#endif /* _GGMUD_H_ */
