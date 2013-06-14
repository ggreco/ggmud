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
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ansi.h"

/* Glbal Variables */
GtkWidget *text_field;
GtkWidget *text_entry;
GtkWidget *entry_host;
GtkWidget *entry_port;

/* Colors */
GdkColormap *cmap;		/* BOLD = bright color. LOW = darkish color */
GdkColor color_lightwhite;	/* BOLD white  */
GdkColor color_white;		/* LOW white */
GdkColor color_lightblue;	/* BOLD blue */
GdkColor color_blue;		/* LOW blue */
GdkColor color_lightgreen;	/* BOLD green */
GdkColor color_green;		/* LOW green */
GdkColor color_lightred;	/* BOLD red */
GdkColor color_red;		/* LOW red */
GdkColor color_lightyellow;	/* BOLD yellow */
GdkColor color_yellow;		/* LOW yellow ( brown )*/
GdkColor color_lightmagenta;	/* BOLD magenta ( pink ) */
GdkColor color_magenta;		/* LOW magenta ( purple ) */
GdkColor color_lightcyan;	/* BOLD cyan */
GdkColor color_cyan;		/* LOW cyan */
GdkColor color_lightblack;	/* BOLD grey (highlighted black) */
GdkColor color_black;		/* LOW black */

GdkColor default_color_lightwhite;	/* BOLD white  */
GdkColor default_color_white;		/* LOW white */
GdkColor default_color_lightblue;	/* BOLD blue */
GdkColor default_color_blue;		/* LOW blue */
GdkColor default_color_lightgreen;	/* BOLD green */
GdkColor default_color_green;		/* LOW green */
GdkColor default_color_lightred;	/* BOLD red */
GdkColor default_color_red;		/* LOW red */
GdkColor default_color_lightyellow;	/* BOLD yellow */
GdkColor default_color_yellow;		/* LOW yellow ( brown )*/
GdkColor default_color_lightmagenta;	/* BOLD magenta ( pink ) */
GdkColor default_color_magenta;		/* LOW magenta ( purple ) */
GdkColor default_color_lightcyan;	/* BOLD cyan */
GdkColor default_color_cyan;		/* LOW cyan */
GdkColor default_color_lightblack;	/* BOLD grey (highlighted black) */
GdkColor default_color_black;		/* LOW black */

GtkTextTag *fg_col;		/* Foreground color */
GtkTextTag *bg_col;		/* background color */

int fg_col_i, bg_col_i;         /* Indices of fg_col and bg_col */
int fg_bright, bg_bright;       /* Use bright versions of fg_col and bg_col */
int fg_blink;                   /* Use blinking version of fg_col */

/* from bezerk */
gushort convert_color (unsigned c)
{
    if ( c == 0 )
        return 0;
    c *= 257;

    return (c > 0xffff) ? 0xffff : c;
}

/* from bezerk */
void extract_color (GdkColor *color, unsigned red, unsigned green, unsigned blue)
{
    color->red   = convert_color (red);
    color->green = convert_color (green);
    color->blue  = convert_color (blue);
}

GdkColor *orig_colors[2][8] =
{
    {
        &color_black,
        &color_red,
        &color_green,
        &color_yellow,
        &color_blue,
        &color_magenta,
        &color_cyan,
        &color_white
    },
    {
        &color_lightblack,
        &color_lightred,
        &color_lightgreen,
        &color_lightyellow,
        &color_lightblue,
        &color_lightmagenta,
        &color_lightcyan,
        &color_lightwhite
    }
};

GtkTextTag *fg_colors[2][8];
GtkTextTag *bg_colors[2][8];
GtkTextTag *blink_colors[2][8];
GtkTextTagTable *tag_table = NULL;
GtkTextTag *url_tag = NULL;

gboolean textview_motion_notify_cb(GtkWidget *textview, GdkEventMotion *event, __attribute__((unused))gpointer d){
	gint x=0, y=0;
	GtkTextIter iter;
	GtkTextBuffer *buff;

    if (!prefs.ShowURL)
        return FALSE;

	if (!(buff=gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview))))
		return FALSE;

	gtk_text_view_window_to_buffer_coords(GTK_TEXT_VIEW(textview), GTK_TEXT_WINDOW_WIDGET,
						event->x, event->y, &x, &y);
	
	gtk_text_view_get_iter_at_location(GTK_TEXT_VIEW(textview), &iter, x, y);

	gdk_window_get_pointer(event->window, 0, 0, 0);
	if (gtk_text_iter_has_tag(&iter, url_tag)){
		GdkCursor *cur=gdk_cursor_new(GDK_HAND2);
		gdk_window_set_cursor(event->window, cur);
	}
	else {
		GdkCursor *cur=gdk_cursor_new(GDK_XTERM);
		gdk_window_set_cursor(event->window, cur);
	}
	return FALSE;
}

gboolean textview_url_activate(GtkTextTag *tag,
                            __attribute__((unused))GObject *arg1, GdkEvent *event, 
                            GtkTextIter *arg2, __attribute__((unused))gpointer d){
       GdkEventButton *event_btn=(GdkEventButton*)event;
       if (event->type == GDK_BUTTON_RELEASE && event_btn->button == 1){
           GtkTextIter start, end;
           gchar *link;

           if (gtk_text_iter_toggles_tag(arg2, tag)) return FALSE;

           /* Go backward until this tag begins */
           start=*arg2;
           while (gtk_text_iter_begins_tag(&start, tag) == FALSE) gtk_text_iter_backward_char(&start);

           /* Go backward until this tag ends */
           end=*arg2;
           while (gtk_text_iter_ends_tag(&end, tag) == FALSE) gtk_text_iter_forward_char(&end);
           link=gtk_text_iter_get_text(&start, &end);

           if (link)
               openurl(link);
       }
       return FALSE;
}

void check_for_url(GtkTextBuffer *b, int offset)
{
    GtkTextIter it, start, end;

    gtk_text_buffer_get_iter_at_offset(b, &it, offset);

    if (gtk_text_iter_is_end(&it))
        return;

    while (gtk_text_iter_forward_search(&it, "http:/", 0, &start, &end, NULL)) {
        it = end;
        do 
        {
            int ch = gtk_text_iter_get_char(&it);

            if (ch == ' '  || ch == 0 || 
                ch == '\'' || ch < 32)
                break;
        }
        while (gtk_text_iter_forward_char(&it));

        gtk_text_buffer_apply_tag(b,
                url_tag, &start, &it);
    } 
}

/* from bezerk */
void init_colors ()
{
    int i,j;

    cmap = gdk_colormap_get_system ();
    
    extract_color(&color_lightwhite, 254, 254, 254);
    if (!gdk_color_alloc(cmap, &color_lightwhite)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_white, 191, 191, 191);
    if (!gdk_color_alloc(cmap, &color_white)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_lightblue, 0, 0, 255);
    if (!gdk_color_alloc(cmap, &color_lightblue)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_blue, 0, 0, 127);
    if (!gdk_color_alloc(cmap, &color_blue)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_lightgreen, 0, 255, 0);
    if (!gdk_color_alloc(cmap, &color_lightgreen)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_green, 0, 127, 0);
    if (!gdk_color_alloc(cmap, &color_green)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_lightred, 255, 0, 0);
    if (!gdk_color_alloc(cmap, &color_lightred)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_red, 127, 0, 0);
    if (!gdk_color_alloc(cmap, &color_red)) {
        g_error("couldn't allocate color");
    }
   extract_color(&color_lightyellow, 255, 255, 0);
    if (!gdk_color_alloc(cmap, &color_lightyellow)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_yellow, 127, 127, 0);
    if (!gdk_color_alloc(cmap, &color_yellow)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_lightmagenta, 255, 0, 255);
    if (!gdk_color_alloc(cmap, &color_lightmagenta)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_magenta, 127, 0, 127);
    if (!gdk_color_alloc(cmap, &color_magenta)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_lightcyan, 0, 255, 255);
    if (!gdk_color_alloc(cmap, &color_lightcyan)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_cyan, 0, 127, 127);
    if (!gdk_color_alloc(cmap, &color_cyan)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_lightblack, 127, 127, 127);
    if (!gdk_color_alloc(cmap, &color_lightblack)) {
        g_error("couldn't allocate color");
    }
    extract_color(&color_black, 0, 0, 0);
    if (!gdk_color_alloc(cmap, &color_black)) {
        g_error("couldn't allocate color");
    }

    default_color_white 		= color_white;
    default_color_lightwhite 	= color_lightwhite;
    default_color_blue 		= color_blue;		//default_color_white 	= color_white;
    default_color_lightblue 	= color_lightblue;	//default_color_white 	= color_white;
    default_color_green 		= color_green;		//default_color_white 	= color_white;
    default_color_lightgreen 	= color_lightgreen;	//default_color_white 	= color_white;
    default_color_red	 	= color_red;		//default_color_white 	= color_white;
    default_color_lightred 		= color_lightred;		//default_color_white 	= color_white;
    default_color_yellow 		= color_yellow;		//default_color_white 	= color_white;
    default_color_lightyellow 	= color_lightyellow;	//default_color_white 	= color_white;
    default_color_magenta 	= color_magenta;	//default_color_white 	= color_white;
    default_color_lightmagenta 	= color_lightmagenta;	//default_color_white 	= color_white;
    default_color_cyan	 	= color_cyan;		//default_color_white 	= color_white;
    default_color_lightcyan 	= color_lightcyan;	//default_color_white 	= color_white;
    default_color_black 		= color_black;		//default_color_white 	= color_white;
    default_color_lightblack 	= color_lightblack;	//default_color_white 	= color_white;


    if (!tag_table) {
        tag_table = (GtkTextTagTable *)gtk_text_tag_table_new();
        
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 2; j++) {
                fg_colors[j][i] = gtk_text_tag_new(NULL);
                g_object_set(fg_colors[j][i], "foreground-gdk", orig_colors[j][i], NULL);
                blink_colors[j][i] = gtk_text_tag_new(NULL);
                g_object_set(blink_colors[j][i], "foreground-gdk", orig_colors[j][i], NULL);
                bg_colors[j][i] = gtk_text_tag_new(NULL);            
                g_object_set(bg_colors[j][i], "background-gdk", orig_colors[j][i], NULL);

                gtk_text_tag_table_add(tag_table, fg_colors[j][i]);
                gtk_text_tag_table_add(tag_table, bg_colors[j][i]);
                gtk_text_tag_table_add(tag_table, blink_colors[j][i]);
            }
        }
        fg_col = prefs.DefaultColor = gtk_text_tag_new(NULL);
        g_object_set(prefs.DefaultColor, "foreground-gdk", &color_white, NULL);
        bg_col = prefs.BackgroundColor = gtk_text_tag_new(NULL);
        g_object_set(prefs.BackgroundColor, "background-gdk", &color_black, NULL);
        fg_col_i = -1;
        fg_bright = 0;
        bg_col_i = -1;
        bg_bright = 0;

        prefs.BrightColor = gtk_text_tag_new(NULL);
        g_object_set(prefs.BrightColor, "foreground-gdk", &color_lightwhite, NULL);

        gtk_text_tag_table_add(tag_table, prefs.BackgroundColor);
        gtk_text_tag_table_add(tag_table, prefs.DefaultColor);
        gtk_text_tag_table_add(tag_table, prefs.BrightColor);

        url_tag = gtk_text_tag_new(NULL);
        g_object_set(url_tag, "foreground-gdk", &color_lightblue,
                              "underline", PANGO_UNDERLINE_LOW, // was UNDERLINE_SINGLE, but low is better for an URL.
                              "underline-set", TRUE, NULL);
        g_signal_connect(G_OBJECT(url_tag), "event",
                                G_CALLBACK(textview_url_activate), NULL);
        gtk_text_tag_table_add(tag_table, url_tag);
    }
}

void swap_blinks(void)
{
    static int blink = 0;
    int i,j;
   
    if (!tag_table)
        return;
    
    if (blink == 0) {
        if (!prefs.Blinking)
            return;

        for (i = 0; i < 8; i++) {
            for (j = 0; j < 2; j++) {
                g_object_set(blink_colors[j][i], "foreground-gdk", &prefs.BackgroundGdkColor, NULL);
            }
        }
    }
    else {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 2; j++) {
                g_object_set(blink_colors[j][i], "foreground-gdk", orig_colors[j][i], NULL);
            }
        }        
    }
    
    blink = !blink;
}

void update_color_tags(GdkColor *color)
{
    int i, j;
    
    if (color == &prefs.BackgroundGdkColor) {
        g_object_set(prefs.BackgroundColor, "background-gdk", color, NULL);        
    }
    else if (color == &prefs.DefaultGdkColor) {
        g_object_set(prefs.DefaultColor, "foreground-gdk", color, NULL);
    }
    else if (color == &prefs.BrightGdkColor) {
        g_object_set(prefs.BrightColor, "foreground-gdk", color, NULL);
    }
    else for (i = 0; i < 8; i++) {
        for (j = 0; j < 2; j++) {
            if (orig_colors[j][i] == color) {
                g_object_set(fg_colors[j][i], "foreground-gdk", color, NULL);
                g_object_set(bg_colors[j][i], "background-gdk", color, NULL);
            }
        }
    }
}

/* fgb:    new fg brightness value  (-1 == no change)
 * fgi:    new fg index value       (-1 == no change; -2 == default)
 * bgb:    new bg brightness value  (-1 == no change)
 * bgi:    new bg index value       (-1 == no change; -2 == default)
 * blink:  new fg blink value       (-1 == no change) */
void set_colors(int fgb, int fgi, int bgb, int bgi, int blink)
{
    if (fgb >= 0)
      fg_bright = fgb;
    if (fgi >= 0 || fgi == -2)
      fg_col_i = fgi;
    if (bgb >= 0)
      bg_bright = bgb;
    if (bgi >= 0 || bgi == -2)
      bg_col_i = bgi;
    if (blink >= 0)
      fg_blink = blink;

    /* currently, default color can't blink */
    if (fg_bright && fg_col_i < 0) /* bright default */
      fg_col = prefs.BrightColor;
    else if (fg_col_i < 0)         /* dim default */
      fg_col = prefs.DefaultColor;
    else if (fg_blink)             /* blinking color */
      fg_col = blink_colors[fg_bright][fg_col_i];
    else                           /* normal color */
      fg_col = fg_colors[fg_bright][fg_col_i];

    if (bg_col_i < 0)              /* default background */
      bg_col = prefs.BackgroundColor;
    else                           /* other background */
      bg_col = bg_colors[bg_bright][bg_col_i];
}

#define reset_colors()   (set_colors(0,-2,0,-2,0))
#define set_fg_bright(b) (set_colors((b),-1,-1,-1,-1))
#define set_bg_bright(b) (set_colors(-1,-1,(b),-1,-1))
#define set_fg_color(c)  (set_colors(-1,(c),-1,-1,-1))
#define set_bg_color(c)  (set_colors(-1,-1,-1,(c),-1))
#define set_blink(b)     (set_colors(-1,-1,-1,-1,(b)))

void test_getcol(const char *code, int n)
{
    static int grcm = 1;
    const char *final = code + n-1;
    char *next;

#ifdef debug
    printf("code(%d): %s\n", n, code);
#endif
    
    /* if the code is an SGR, and the current GRCM is replace,
       then do a reset */
    if (!grcm && *final == 'm')
      reset_colors();

    while (code != NULL && code < final) {
      long int attr;
      
      /* advance pointer to next decimal number */
      while (*code == ';') {
        if (++code == final)
          return;
      }

      /* read the number */
      attr = strtol(code, &next, 10);
      if (next == code) {
        /* strtol failed */
        code++;
        continue;
      }

      switch (*final) {
      case 'h':  /* Set Mode (SM) */
        if (attr == 21) /* GRCM */
          grcm = 1;
        break;

      case 'l':  /* Reset Mode (RM) */
        if (attr == 21) /* GRCM */
          grcm = 0;
        break;

      case 'm':  /* Select Graphic Rendition (SGR) */
        if (attr == 0) /* total reset */
          reset_colors();
        else if (attr == 1) /* change to bright */
          set_fg_bright(1);
        else if (attr == 2 ||  /* change to dim -- implemented as normal */
                 attr == 22)   /* change to normal intensity */
          set_fg_bright(0);
        else if (attr == 5 || attr == 6) /* change to blinking */
          set_blink(1);
        else if (attr == 25) /* stop blinking */
          set_blink(0);
        else if (attr >= 30 && attr <= 37) /* change foreground hue */
          set_fg_color(attr - 30);
        else if (attr == 39) /* default foreground color */
          set_fg_color(-2);
        else if (attr >= 40 && attr <= 47) /* change background hue */
          set_bg_color(attr - 40);
        else if (attr == 49) /* default background color */
          set_bg_color(-2);
        else if (attr == 66) /* change bg bright GGMUD EXTENSION */
          set_bg_bright(1);
        break;
      }

      code = next;
    }
}

static void
flush_text_buffer(char *buf, int *pos, GtkTextIter *iter, GtkTextBuffer *tbuff)
{
    if(*pos) {
        if (mud->LOGGING) /* Logging */
            fwrite(buf, *pos, 1, mud->LOG_FILE);

        gtk_text_buffer_insert_with_tags(tbuff, iter, buf, *pos,  fg_col, bg_col, NULL);
        *pos = 0;
    }
}

void dump_buffer(const char *b, int len)
{
    while (len--) {
        fprintf(stderr, "%d (%c) ", *b, *b);
        b++;
    }
    fputc('\n', stderr);
}


void local_disp_ansi(int size, const char *in, GtkTextView *target)
{
    char buffer[256];
    int bufferpos = 0;
    char ansibuffer[32];
    GtkTextBuffer *tbuff = gtk_text_view_get_buffer(target);
    GtkTextIter iter;
    int n = 0, start = 0;

    gtk_text_buffer_get_end_iter(tbuff, &iter);

    if (prefs.ShowURL)
        start = gtk_text_iter_get_offset(&iter);
    /*
    fg_col=prefs.DefaultColor;
    bg_col=prefs.BackgroundColor;
    */

    while(n < size) {
        /* plain text no color nothing */
        if(in[n] != 27) {
            buffer[bufferpos++] = in[n];

            if(bufferpos == sizeof(buffer)) // flush_text_buffer resets internally bufferpos
                flush_text_buffer(buffer, &bufferpos, &iter, tbuff);    
        }
        /* color and special signs -> stripp it! */
        else {
            int started_code = 0;

            if (in[++n] != '[')
              continue;
            n++;

            flush_text_buffer(buffer, &bufferpos, &iter, tbuff);

            while(in[n] < 0x40 && n < size) {
                if (in[n] != 0)
                    ansibuffer[started_code++] = in[n];
                n++;
            }

            if (n < size)
              ansibuffer[started_code++] = in[n];
            ansibuffer[started_code] = '\0';
            test_getcol(ansibuffer, started_code);
        }
        n++;
    }	
    
    flush_text_buffer(buffer, &bufferpos, &iter, tbuff);

    if (prefs.ShowURL)
        check_for_url(tbuff, start);
}

void disp_ansi(int size, const char *in, GtkTextView *target)
{
    char buffer[256];
    int bufferpos = 0;
    static int started_code = -2;
    static char ansibuffer[32];
    GtkTextBuffer *tbuff = gtk_text_view_get_buffer(target);
    GtkTextIter iter;
    int start = 0;
    int n = 0;


    gtk_text_buffer_get_end_iter(tbuff, &iter);

    if (prefs.ShowURL)
        start = gtk_text_iter_get_offset(&iter);

    while(n < size) {

        /* saw ESC, expect next character to be '[' */
        if (started_code == -1) {
            if (in[n] == '[') {
                started_code++;
                n++;
                continue;
            } else {
                /* invalid ansi code */
                started_code = -2;
            }
        }

        /* continuation of ansi code */
        if (started_code >= 0) {
  
            /* end of ansi code */
            if (in[n] >= 0x40 && in[n] < 0x7f) {
                ansibuffer[started_code++] = in[n];
                ansibuffer[started_code] = '\0';
                test_getcol(ansibuffer, started_code);
                started_code = -2;
            }
            else
                ansibuffer[started_code++] = in[n];

            n++;
            continue;
        }

        /* stripp out goofy signs like linefeeds.... */
        if (in[n] == '\r' || in[n] == 0) {
            n++;
            continue;
        }

        if (in[n] < 0) {
            n++;
            continue;
        }

        /* we have intercepted the beep character */
        if (in[n] =='\a') {
            /* are we going to send out the beep? */
            if (prefs.DoBeep)
                gdk_beep();
            n++;
            continue;
        }

        /* beginning of ansi code */
        if(in[n] == 27) {
            flush_text_buffer(buffer, &bufferpos, &iter, tbuff);
            started_code++;
            n++;
            continue;
        }

        /* plain text no color nothing */
        buffer[bufferpos++] = in[n];

        if(bufferpos == sizeof(buffer)) // flush_text_buffer resets internally bufferpos
            flush_text_buffer(buffer, &bufferpos, &iter, tbuff);    
        n++;
    }	

    flush_text_buffer(buffer, &bufferpos, &iter, tbuff);

    if (prefs.ShowURL)
        check_for_url(tbuff, start);
}

#define MOD_NORMAL	"0"
#define MOD_BOLD	"1"
#define MOD_FAINT	"2"
#define MOD_UNDERLINE	"4"
#define MOD_BLINK	"5"
#define MOD_REVERSE	"7"

#define FG_BLACK	"30"
#define FG_RED		"31"
#define FG_GREEN	"32"
#define FG_BROWN	"33"
#define FG_BLUE		"34"
#define FG_MAGENTA	"35"
#define FG_CYAN		"36"
#define FG_LT_GRAY	"37"
#define FG_DK_GRAY	"1;30"
#define FG_LT_RED	"1;31"
#define FG_LT_GREEN	"1;32"
#define FG_YELLOW	"1;33"
#define FG_LT_BLUE	"1;34"
#define FG_LT_MAGENTA	"1;35"
#define FG_LT_CYAN	"1;36"
#define FG_WHITE	"1;37"

#define BK_BLACK	"40"
#define BK_RED		"41"
#define BK_GREEN	"42"
#define BK_BROWN	"43"
#define BK_BLUE		"44"
#define BK_MAGENTA	"45"
#define BK_CYAN		"46"
#define BK_LT_GRAY	"47"

char *ansi_parse(char *code )
{

    /* FF: Cambiamo il sistema di costruzione delle stringhe ansi, in modo
       da far funzionare anche i modi */

    static char s[MAX_STRING_LENGTH]; /* increased from 255 to MAX 2-18 msw */
    char m[128], b[128], f[128];

    if (!code)
        return(""); /* changed this from NULL to "" 2-18 msw */

    /* do modifier */
    switch(code[0]) {
        case '0':sprintf(m,"%s",MOD_NORMAL);
                 break;
        case '1':sprintf(m,"%s",MOD_BOLD);
                 break;
        case '2':sprintf(m,"%s",MOD_FAINT);
                 break;
                 /* not used in ansi that I know of */
        case '3':sprintf(m,"%s",MOD_NORMAL);
                 break;
        case '4':sprintf(m,"%s",MOD_UNDERLINE);
                 break;
        case '5': sprintf(m,"%s",MOD_BLINK);
                  break;

        case '6': sprintf(m,"%s",MOD_REVERSE);
                  break;

        default: sprintf(m,"%s",MOD_NORMAL);
                 break;
    }

    /* do back ground color */
    switch(code[1]) {
        case '0': sprintf(b,"%s",BK_BLACK);
                  break;
        case '1': sprintf(b,"%s",BK_RED);
                  break;
        case '2': sprintf(b,"%s",BK_GREEN);
                  break;
        case '3': sprintf(b,"%s",BK_BROWN);
                  break;
        case '4': sprintf(b,"%s",BK_BLUE);
                  break;
        case '5': sprintf(b,"%s",BK_MAGENTA);
                  break;
        case '6': sprintf(b,"%s",BK_CYAN);
                  break;
        case '7': sprintf(b,"%s",BK_LT_GRAY);
                  break;
        default:sprintf(b,"%s",BK_BLACK);
                break;
    }

    /* do foreground color */
    switch(code[2]) {     
        case '0':  	
            switch(code[3]) {  		/* 00-09 */
                case '0': sprintf(f,"%s",FG_BLACK);
                          break;
                case '1': sprintf(f,"%s",FG_RED);
                          break;
                case '2': sprintf(f,"%s",FG_GREEN);
                          break;
                case '3': sprintf(f,"%s",FG_BROWN);
                          break;
                case '4': sprintf(f,"%s",FG_BLUE);
                          break;
                case '5': sprintf(f,"%s",FG_MAGENTA);
                          break;
                case '6': sprintf(f,"%s",FG_CYAN);
                          break;
                case '7': sprintf(f,"%s",FG_LT_GRAY);
                          break;
                case '8': sprintf(f,"%s",FG_DK_GRAY);
                          break;
                case '9': sprintf(f,"%s",FG_LT_RED);
                          break;
                default: sprintf(f,"%s",FG_DK_GRAY);
                         break;
            } break;

        case '1':  	
            switch(code[3]) {  		/* 10-15 */
                case '0': sprintf(f,"%s",FG_LT_GREEN);
                          break;
                case '1': sprintf(f,"%s",FG_YELLOW);
                          break;
                case '2': sprintf(f,"%s",FG_LT_BLUE);
                          break;
                case '3': sprintf(f,"%s",FG_LT_MAGENTA);
                          break;
                case '4': sprintf(f,"%s",FG_LT_CYAN);
                          break;
                case '5': sprintf(f,"%s",FG_WHITE);
                          break;
                default: sprintf(f,"%s",FG_LT_GREEN);
                         break;
            } break;

        default : sprintf(f,"%s",FG_LT_RED);
                  break;  				
    }

    sprintf( s, "\033[%s;%s;%sm", m,b,f );
    return(s);
}

#include <ctype.h>

char *ParseAnsiColors(const char *txt)
{
  static char buf[BUFFER_SIZE*2];
  char tmp[256];
  register int l,f=0;

  *buf = 0;

  for(l=0;*txt;) {
    if(*txt=='$' && (toupper(*(txt+1)) == 'C' || 
                    (*(txt+1)=='$' && toupper(*(txt+2)) == 'C'))) {
      if(*(txt+1)=='$')
        txt+=3;
      else
        txt+=2;
      
      strncpy(tmp,txt,4);
      
      tmp[4] = 0;
      
      strcat(buf,ansi_parse(tmp));
      
      txt+=4;
      l=strlen(buf);
      f++;
    } /* LC: Usiamo il backslash come escape */
	else if (*txt=='\\' && txt[1]) {
		buf[l++]=*++txt;
		++txt;
	} else {
      buf[l++]=*txt++;
    }
    buf[l]=0;
  }
  if(f)
    strcat(buf,ansi_parse("0007"));
 
  return buf;
}

