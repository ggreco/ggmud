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

#include "config.h"

#include <gtk/gtk.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ggmud.h"

/* Global variables */
SYSTEM_DATA font;
GtkWidget   *font_window;
GtkWidget   *font_button_save;
GtkWidget   *entry_fontname;
GtkWidget *menu_Option_font;
PangoFontDescription *font_normal = NULL;
GtkStyle *style;

void set_style() 
{
    if (mud && mud->text && font_normal) {
        gtk_widget_modify_font(GTK_WIDGET(mud->text), font_normal);
    }
}

#define DEFAULT_FONT "Monospace 12"

void load_font () {
    FILE *fp;
    gchar line[255], pref[25], value[250];

    *value = 0;
    
    if (fp = fileopen ("font", "r")) {
        while (fgets (line, 80, fp)) {
            sscanf (line, "%s %[^\n]", pref, value);
            if (!strcmp (pref, "FontName")) {
                break;
            }
            else
                *value = 0;
        }

        fclose (fp);
    }

    if ( !*value ||
         !( font_normal = pango_font_description_from_string (value) )  ) {
        if (!(font_normal = pango_font_description_from_string (DEFAULT_FONT) )) {
            g_error ("Can't load font... Using default.\n");
            return;
        }
        else
            font.FontName = strdup(DEFAULT_FONT);
    }
    else
        font.FontName = strdup (value);
    
    set_style();
}

void save_font () {
    FILE *fp;
    gchar buf[256];

    if(fp = fileopen("font", "w")) {
    	if (strlen(font.FontName)) fprintf (fp, "FontName %s\n", font.FontName);
    	fclose (fp);
    }	
}


void font_font_selected (GtkWidget *button, GtkFontSelectionDialog *fs) {
    gchar *temp, buf[256];

    temp = gtk_font_selection_get_font_name (GTK_FONT_SELECTION (fs->fontsel));

    if (temp) {
        if ((font_normal = pango_font_description_from_string (temp))) {
            if (font.FontName)
                free (font.FontName);
            
            font.FontName = strdup (temp);
            set_style();
            save_font();
            gtk_widget_destroy (GTK_WIDGET (fs));
            return;
        }
    }

    popup_window ( "The selected Font isn't valid, be sure to select a font that does exist.");
}

void window_font (GtkWidget *button, gpointer data) {
    GtkWidget *fontw;

    fontw = gtk_font_selection_dialog_new ("Font Selection...");
    gtk_font_selection_dialog_set_preview_text (GTK_FONT_SELECTION_DIALOG (fontw),
                                                "This is a sample text?");

    gtk_font_selection_dialog_set_font_name (GTK_FONT_SELECTION_DIALOG (fontw), font.FontName);

    gtk_signal_connect (GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (fontw)->ok_button), "clicked",
                        GTK_SIGNAL_FUNC (font_font_selected), fontw);

    gtk_signal_connect (GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (fontw)->cancel_button), "clicked",
                        GTK_SIGNAL_FUNC (close_window), fontw);

    gtk_widget_show (fontw);
}


