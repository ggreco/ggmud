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

#include "font.h"

void set_style() {
    style = gtk_widget_get_default_style();
    if (style) {
	style->font = font_normal;
	gtk_widget_set_style(mud->window, style);
	gtk_widget_queue_draw(mud->window);
    }	
}

void load_font () {
    FILE *fp;
    gchar line[255], pref[25], value[250];

#ifndef WIN32
    font.FontName = strdup ("-adobe-courier-medium-r-normal-*-*-120-*-*-m-*-iso8859-1"); //("fixed");
#else
	font.FontName = strdup ("fixed");
#endif
    if (fp = fileopen ("font", "r")) {
    	while (fgets (line, 80, fp)) {
            sscanf (line, "%s %[^\n]", pref, value);
            if (!strcmp (pref, "FontName")) {
				free(font.FontName);
				font.FontName = strdup (value);
			}
    	}
    	font_normal = gdk_font_load (font.FontName);
		set_style();
    	fclose (fp);
    }	
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
        free (font.FontName);
        font.FontName = strdup (temp);
        font_normal = gdk_font_load (font.FontName);
        set_style();
        free (temp);
        save_font();
        gtk_widget_destroy (GTK_WIDGET (fs));
    } else {
        sprintf (buf, "The selected Font isn't valid, be sure to select a font that does exist.");
        popup_window (buf);
    }	    
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


