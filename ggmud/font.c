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

#include "config.h"

#include <gtk/gtk.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ggmud.h"
#include "interface.h"

/* Global variables */
SYSTEM_DATA font;
PangoFontDescription *font_normal = NULL;
PangoFontDescription *font_interface = NULL;
PangoFontDescription *font_input = NULL;

extern void set_style();

#define DEFAULT_FONT "Monospace 12"

void load_font () {
    FILE *fp;
    gchar line[255], pref[25], value[250];

    *value = 0;
    
    if ((fp = fileopen ("font", "r"))) {
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

    if ((fp = fileopen("font", "w"))) {
    	if (strlen(font.FontName)) fprintf (fp, "FontName %s\n", font.FontName);
    	fclose (fp);
    }	
}


void font_font_selected (GtkWidget *button, GtkFontSelectionDialog **fs) {
    gchar *temp;

    if (!*fs)
        return;

    temp = gtk_font_selection_get_font_name (GTK_FONT_SELECTION ((*fs)->fontsel));

    gtk_entry_set_text(GTK_ENTRY(gtk_object_get_user_data(GTK_OBJECT(*fs))), temp);

#if 0
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

    popup_window (ERR, "The selected Font isn't valid, be sure to select a font that does exist.");
#endif
    gtk_widget_destroy(GTK_WIDGET(*fs));
}

void close_font_window(GtkWidget *button, GtkFontSelectionDialog **fs)
{
    if (!*fs)
        return;

    gtk_widget_destroy(GTK_WIDGET(*fs));
}

void  on_font_selection_clicked(GtkWidget *entry, GtkWidget *button)
{
    static GtkWidget *font_selection = NULL;

    if (font_selection)
        return;

    font_selection = gtk_font_selection_dialog_new ("Font Selection...");
    gtk_object_set_user_data(GTK_OBJECT(font_selection), entry);
    gtk_font_selection_dialog_set_preview_text (GTK_FONT_SELECTION_DIALOG (font_selection),
            "This is a sample text?");

    gtk_font_selection_dialog_set_font_name (GTK_FONT_SELECTION_DIALOG (font_selection), font.FontName);

    gtk_signal_connect (GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (font_selection)->ok_button), "clicked",
            GTK_SIGNAL_FUNC (font_font_selected), &font_selection);

    gtk_signal_connect (GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (font_selection)->cancel_button), "clicked",
            GTK_SIGNAL_FUNC (close_font_window), &font_selection);

    gtk_signal_connect (GTK_OBJECT(font_selection), "destroy",
            GTK_SIGNAL_FUNC (kill_window), &font_selection);

    gtk_widget_show (font_selection);
}


void
on_button_fonts_ok_clicked             (GtkButton       *button,
                                        gpointer         user_data)
{

}


void
on_button_fonts_save_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{

}

void window_font (GtkWidget *button, gpointer data) {
    static GtkWidget *fontw = NULL;
    
    if (fontw) {
        gtk_window_present(fontw);
        return;
    }

    fontw = create_window_fonts();

    gtk_widget_show(fontw);

    gtk_signal_connect (GTK_OBJECT(fontw), "destroy",
            GTK_SIGNAL_FUNC (kill_window), &fontw);
}


