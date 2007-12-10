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
#include "support.h"

struct GGFont fonts[FONTS_NUM] =
{
    {"FontName", "entry_output", "Output", NULL, NULL},
    {"InputFontName", "entry_input", "Input", NULL, NULL},
    {"GuiFontName", "entry_interface", "Interface", NULL, NULL}
};


#define DEFAULT_FONT "Monospace 12"

static void setfonts()
{
    int i;

    for (i = 0; i < FONTS_NUM; i++) {   

        if (i == 0 && !fonts[0].name)
            fonts[i].name = strdup(DEFAULT_FONT);

        if (fonts[i].name) {
            if (!( fonts[i].desc = pango_font_description_from_string (fonts[i].name) )  ) {
                free(fonts[i].name);

                if (!(fonts[i].desc = pango_font_description_from_string (DEFAULT_FONT) )) {
                    fonts[i].name = NULL;
                    fonts[i].desc = NULL;
                    g_error ("Can't load font... Using default.\n");
                    continue;
                }
                else {
                    fonts[i].name = strdup(DEFAULT_FONT);
                }
            }
        }
    }
    set_style();
}

void load_font () {
    FILE *fp;
    gchar line[255], pref[25], value[250];
    int i;

    if ((fp = fileopen ("font", "r"))) {
        while (fgets (line, sizeof(line) - 1, fp)) {
            sscanf (line, "%s %[^\n]", pref, value);

            for (i = 0; i < FONTS_NUM; i++) {
                if (!strcmp (pref, fonts[i].config)) {
                    if (fonts[i].name)
                        free(fonts[i].name);

                    fonts[i].name = strdup(value);
                }
            }
        }

        fclose (fp);
    }

    setfonts();
}

void save_font () {
    FILE *fp;

    if ((fp = fileopen("font", "w"))) {
        int i;

        for (i = 0; i < FONTS_NUM; i++) {
        	if (fonts[i].name) 
                fprintf (fp, "%s %s\n", fonts[i].config, fonts[i].name);
        }
    	fclose (fp);
    }	
}


void font_font_selected (GtkWidget *button, GtkFontSelectionDialog **fs) {
    gchar *temp;

    if (!*fs)
        return;

    temp = gtk_font_selection_get_font_name (GTK_FONT_SELECTION ((*fs)->fontsel));

    gtk_entry_set_text(GTK_ENTRY(gtk_object_get_user_data(GTK_OBJECT(*fs))), temp);

    gtk_widget_destroy(GTK_WIDGET(*fs));
}

void close_font_window(GtkWidget *button, GtkFontSelectionDialog **fs)
{
    if (!*fs)
        return;
    gtk_entry_set_text(GTK_ENTRY(gtk_object_get_user_data(GTK_OBJECT(*fs))), "");

    gtk_widget_destroy(GTK_WIDGET(*fs));
}

void  on_font_selection_clicked(GtkWidget *entry, GtkWidget *button)
{
    static GtkWidget *font_selection = NULL;
    char buf[200];
    int i;

    if (font_selection)
        return;

    // find the font we are changing
    for (i = 0; i < FONTS_NUM; i++)
        if (lookup_widget(button, fonts[i].widget) == entry)
            break;

    if (i == FONTS_NUM)
        return;

    sprintf(buf, "%s Font Selection...", fonts[i].label);

    font_selection = gtk_font_selection_dialog_new (buf);
    gtk_object_set_user_data(GTK_OBJECT(font_selection), entry);
    gtk_font_selection_dialog_set_preview_text (GTK_FONT_SELECTION_DIALOG (font_selection),
            "This is a sample text?");


    gtk_font_selection_dialog_set_font_name (GTK_FONT_SELECTION_DIALOG (font_selection), fonts[i].name);

    gtk_signal_connect (GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (font_selection)->ok_button), "clicked",
            GTK_SIGNAL_FUNC (font_font_selected), &font_selection);

    gtk_signal_connect (GTK_OBJECT (GTK_FONT_SELECTION_DIALOG (font_selection)->cancel_button), "clicked",
            GTK_SIGNAL_FUNC (close_font_window), &font_selection);

    gtk_signal_connect (GTK_OBJECT(font_selection), "destroy",
            GTK_SIGNAL_FUNC (kill_window), &font_selection);

    gtk_widget_show (font_selection);
}

static void
getfonts(GtkWidget *r)
{
    int i;

    for (i = 0; i < FONTS_NUM; ++i) {
        GtkWidget *w;

        if ((w = lookup_widget(r, fonts[i].widget))) {
            const char *text = gtk_entry_get_text(GTK_ENTRY(w));

            if (fonts[i].name)
                free(fonts[i].name);

            if (text && *text) 
                fonts[i].name = strdup(text);
            else 
                fonts[i].name = NULL;
        }
    }
}

void
on_button_fonts_ok_clicked             (GtkWidget       *button,
                                        gpointer         user_data)
{
    getfonts(button);
    setfonts();

    gtk_widget_destroy(gtk_widget_get_toplevel(button));
}


void
on_button_fonts_save_clicked           (GtkWidget      *button,
                                        gpointer         user_data)
{
    getfonts(button);
    setfonts();
    save_font();
}

void window_font (GtkWidget *button, gpointer data) {
    static GtkWidget *fontw = NULL;
    int i;

    if (fontw) {
        gtk_window_present(GTK_WINDOW(fontw));
        return;
    }

    fontw = create_window_fonts();

    for (i = 0; i < FONTS_NUM; i++) {
        GtkWidget *e = lookup_widget(fontw, fonts[i].widget);

        if (e && fonts[i].name)
            gtk_entry_set_text(GTK_ENTRY(e), fonts[i].name);

    }

    gtk_widget_show(fontw);


    gtk_signal_connect (GTK_OBJECT(fontw), "destroy",
            GTK_SIGNAL_FUNC (kill_window), &fontw);
}


