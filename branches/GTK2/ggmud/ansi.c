/*  Sclient
 *  Copyright (C) 1999 Heathen (the.heathen@swipnet.se)
 *		  1999 Drizzt  (doc.day@swipnet.se)
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
GtkTextTagTable *tag_table = NULL;

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
                bg_colors[j][i] = gtk_text_tag_new(NULL);            
                g_object_set(bg_colors[j][i], "background-gdk", orig_colors[j][i], NULL);

                gtk_text_tag_table_add(tag_table, fg_colors[j][i]);
                gtk_text_tag_table_add(tag_table, bg_colors[j][i]);
            }
        }
        fg_col = prefs.DefaultColor = gtk_text_tag_new(NULL);
        g_object_set(prefs.DefaultColor, "foreground-gdk", &color_white, NULL);
        bg_col = prefs.BackgroundColor = gtk_text_tag_new(NULL);
        g_object_set(prefs.BackgroundColor, "background-gdk", &color_black, NULL);

        gtk_text_tag_table_add(tag_table, bg_col);
        gtk_text_tag_table_add(tag_table, fg_col);
    }
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
    else for (i = 0; i < 8; i++) {
        for (j = 0; j < 2; j++) {
            if (orig_colors[j][i] == color) {
                g_object_set(fg_colors[j][i], "foreground-gdk", color, NULL);
                g_object_set(bg_colors[j][i], "background-gdk", color, NULL);
            }
        }
    }
}

static int
get_first(int in)
{
    in -= '0';

    if(in < 0 || in > 1)
        return 1; // return bright color for default

    return in;
}

static int
get_second(int in)
{
    in -= '0';

    if(in < 0 || in > 7)
        return 7; // return white for default

    return in;
}

void test_getcol(const char *tmp, int bleh)
{
#ifdef debug
    printf("code(%d): %.9s\n", bleh, tmp);
#endif
    
    switch(bleh){

        case 2: if( tmp[0]== '[' && tmp[1] == 'm') {
                    fg_col = prefs.DefaultColor; 
                    bg_col = prefs.BackgroundColor;
                    return;
                } 
                break;
        case 3: if(!strncmp(tmp,"[0m",3)) {
                    fg_col = prefs.DefaultColor;
                    return;
                }
                if(!strncmp(tmp,"[1m",3)) {
                    fg_col = fg_colors[1][7]; // light white
                    return;
                }
                break;
        case 4:
                if(tmp[1] == '3') {
                    fg_col = fg_colors[0][get_second(tmp[2])];
                    return;
                }
                else if(tmp[1] == '4') {
                    bg_col = bg_colors[0][get_second(tmp[2])];
                    return;
                }
                
                break;
        case 6:
                // code of type [X;[3|4]Ym
parse6chars:        
                if(tmp[1] == '0' || tmp[1] == '1') {
                    if(tmp[3] == '3') {
                        fg_col = fg_colors[get_first(tmp[1])][get_second(tmp[4])];
                    }
                    else if(tmp[3] == '4')
                        bg_col = bg_colors[get_first(tmp[1])][get_second(tmp[4])];
                    return;
                }
                else if(tmp[1] == '7') {
                    fg_col = fg_colors[0][0]; // black
                    bg_col = bg_colors[1][get_second(tmp[4])];
                    return;
                }
                
                break;
        case 7:
                if(tmp[1] == ';') {
//                    printf("7 chars code!\n");
                    tmp++;
                    goto parse6chars;
                }
                break;
        case 8:
                if(tmp[5] == '3') {
                    fg_col = fg_colors[get_first(tmp[3])][get_second(tmp[6])];

                    return;
                }
                break;
// added by GG, doesn't handle correctly blinking codes but works
// with 11 chars ansi codes
        case 11:
                if (tmp[3] == '4') {
                    if(tmp[1] == '1' && tmp[4] == '0') // lasciamo lo sfondo nero
                        bg_col = bg_colors[0][0]; // black;
                    else {
                        bg_col = bg_colors[get_first(tmp[1])][get_second(tmp[4])];
                    }
                }
                if (tmp[8] == '3') {
                    fg_col = fg_colors[get_first(tmp[6])][get_second(tmp[9])];
                }
                return;
        case 9:
                if (tmp[3] == '4') {
                    if(tmp[4] != '0') {
                        bg_col = bg_colors[1][get_second(tmp[4])];
                    }
                    else
                        bg_col = bg_colors[0][0]; // lasciamo lo sfondo nero
                }
                if (tmp[6] == '3') {
                    int a = get_first(tmp[1]);
                    int b = get_second(tmp[7]);
                    
                    fg_col = fg_colors[a][b];
                }
                return;

        default:
                break;
    }

    fg_col = prefs.DefaultColor;
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

void disp_ansi(int size, const char *in, GtkTextView *target)
{
    char buffer[256];
    int bufferpos = 0;
    static int started_code = 0;
    static char ansibuffer[12];
    GtkTextBuffer *tbuff = gtk_text_view_get_buffer(target);
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(tbuff, &iter);
 
    int n=0,x=0;
    
    if(!started_code) {
        fg_col=prefs.DefaultColor;
        bg_col=prefs.BackgroundColor;
    }
    else {
        int i = 0;
        
        while(in[i] != 'm' && i < size) {
            i++;
        }

        if( (i + started_code) < 12) {
            memcpy(ansibuffer + started_code, in, i);
            
            test_getcol(ansibuffer, i + started_code);

            in += i;
            size -= i;
        }
        started_code = 0;
    }
    
    while(n < size) {

        /* stripp out goofy signs like linefeeds.... */
        if(in[n] == '\r') {
            n++;

            continue;
        }
        
        /* mask the password at login */
        if(in[n] == -1 && in[n+2] == 1) {
            if(in[n+1] == -5)
                mud->ent->visible = 0;
            if(in[n+1] == -4)
                mud->ent->visible = 1;
            n+=3;
        }

        /* we have intercepted the beep character */
        if (in[n] =='\a') {
            /* are we going to send out the beep? */
            if (prefs.DoBeep)
                gdk_beep();
            n++;
        }

        /* plain text no color nothing */
        if(in[n] != 27) {
            if(bufferpos < sizeof(buffer)) {
                buffer[bufferpos++] = in[n];
            }
            else 
                flush_text_buffer(buffer, &bufferpos, &iter, tbuff);
        }
        /* color and special signs -> stripp it! */
        else {
            flush_text_buffer(buffer, &bufferpos, &iter, tbuff);
            
            while(in[x+n] != 'm') {
                x++;

                if((x + n) >= size) {
                    if(x <= 11) {
                        started_code = x - 1;
                        memcpy(ansibuffer, &in[n + 1], x - 1);
                    }
                    else
                        started_code = 0;
                    return;
                }
            }
            if(x <= 11)
                test_getcol(&in[n+1], x);
            n += x;

            x = 0;
        }
        n++;
    }	
    started_code = 0;
    
    flush_text_buffer(buffer, &bufferpos, &iter, tbuff);
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

char *ParseAnsiColors( char *txt)
{
  static char buf[BUFFER_SIZE*2];
  char tmp[256];
  register int i,l,f=0;

  *buf = 0;

  for(i=0,l=0;*txt;) {
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

