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

GdkColor *colors[2][8] =
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

/* from bezerk */
void init_colors ()
{
    
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

    /* Hmm, why is this here??? CHECK IT OUT */
    if ( !font.FontName ) {
#ifdef WIN32
		font.FontName = g_strdup("-*-*-normal-r-normal--*-100-*-*-m-*-iso8859-1");
#else
        font.FontName = g_strdup ("fixed");
#endif
	}
    if ( ( font_normal = gdk_font_load (font.FontName) ) == NULL )
    {
        g_error ("Can't load font... Using default.\n");
        g_free ( font.FontName );
        font.FontName = g_strdup ("fixed");
        save_font ();
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
                    fg_col = color_lightwhite;
                    return;
                }
                break;
        case 4:
                if(tmp[1] == '3') {
                    fg_col = *colors[0][get_second(tmp[2])];
                    return;
                }
                else if(tmp[1] == '4') {
                    bg_col = *colors[0][get_second(tmp[2])];
                    return;
                }
                
                break;
        case 6:
                // code of type [X;[3|4]Ym
parse6chars:        
                if(tmp[1] == '0' || tmp[1] == '1') {
                    if(tmp[3] == '3') {
                        fg_col = *colors[get_first(tmp[1])][get_second(tmp[4])];
                    }
                    else if(tmp[3] == '4')
                        bg_col = *colors[get_first(tmp[1])][get_second(tmp[4])];
                    return;
                }
                else if(tmp[1] == '7') {
                    fg_col = color_black;
                    bg_col = *colors[1][get_second(tmp[4])];
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
                    fg_col = *colors[get_first(tmp[3])][get_second(tmp[6])];

                    return;
                }
                break;
// added by GG, doesn't handle correctly blinking codes but works
// with 11 chars ansi codes
        case 11:
                if (tmp[3] == '4') {
                    if(tmp[1] == '1' && tmp[4] == '0') // lasciamo lo sfondo nero
                        bg_col = color_black;
                    else {
                        bg_col = *colors[get_first(tmp[1])][get_second(tmp[4])];
                    }
                }
                if (tmp[8] == '3') {
                    fg_col = *colors[get_first(tmp[6])][get_second(tmp[9])];
                }
                return;
        case 9:
                if (tmp[3] == '4') {
                    if(tmp[4] != '0') {
                        bg_col = *colors[1][get_second(tmp[4])];
                    }
                    else
                        bg_col = color_black; // lasciamo lo sfondo nero
                }
                if (tmp[6] == '3') {
                    int a = get_first(tmp[1]);
                    int b = get_second(tmp[7]);
                    
                    fg_col = *colors[a][b];
                }
                return;

        default:
                fg_col = prefs.DefaultColor;
                return;
    }
    fg_col = prefs.DefaultColor;
    return;
}

#ifdef WIN32
#define OLDSTYLE_ANSI
#endif

void disp_ansi(int size, const char *in, GtkText *target)
{
#ifndef OLDSTYLE_ANSI
    char buffer[256];
    int bufferpos = 0;
#endif
    static int started_code = 0;
    static char ansibuffer[12];
    
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

            if (n == size)
                break;
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
#ifndef OLDSTYLE_ANSI
            if(bufferpos < sizeof(buffer)) {
                buffer[bufferpos++] = in[n];
            }
            else {
                if (mud->LOGGING) /* Loging */
                    fwrite(buffer, bufferpos, 1, mud->LOG_FILE);
                
                gtk_text_insert(target, font_normal, &fg_col, &bg_col, buffer, bufferpos);
                bufferpos = 0;
            }
#else
            if (mud->LOGGING) /* Loging */
                fputc(in[n], mud->LOG_FILE);

            gtk_text_insert(target, font_normal, &fg_col, &bg_col, &in[n], 1);
            gtk_text_insert (target, NULL, NULL, NULL," ", 1 );
            gtk_text_backward_delete (target, 1);
#endif
        }
        /* color and special signs -> stripp it! */
        else {
#ifndef OLDSTYLE_ANSI
            if(bufferpos) {
                if (mud->LOGGING) /* Loging */
                    fwrite(buffer, bufferpos, 1, mud->LOG_FILE);

                gtk_text_insert(target, font_normal, &fg_col, &bg_col, buffer, bufferpos);
                bufferpos = 0;
            }
#endif
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
    
#ifndef OLDSTYLE_ANSI
    if(bufferpos) {
        if (mud->LOGGING) /* Loging */
           fwrite(buffer, bufferpos, 1, mud->LOG_FILE);
                
        gtk_text_insert(target, font_normal, &fg_col, &bg_col, buffer, bufferpos);
        bufferpos = 0;
    }    
#endif
}

