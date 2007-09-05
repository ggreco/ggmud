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

#include "ggmud.h"
#include "interface.h"
#include "support.h"

#define GAG_FILE "gag"
#define GAG_LEN 128

static GtkWidget *gag_window = NULL;

void
save_gags (GtkCList *data, GtkWidget *button) {
    FILE *fp;
    gint done = FALSE;
    gchar *gag;
    gint  row = 0;

    if ((fp = fileopen (GAG_FILE, "w"))) {
    	while ( !done && data) {
            if ( !gtk_clist_get_text ( data, row, 0, &gag))
                break;
            
            if (!gag[0]) {
                done = TRUE;
                break;
            }
            fprintf (fp, "#gag {%s}\n", gag);
            row++;
        }
        fclose (fp);
    }    
}

static void 
add_gag(const char *line)
{
    char buffer[GAG_LEN + 20];

    sprintf(buffer, "#gag {%s}", line);
    
    parse_input(buffer, mud->activesession);
}

void
load_gags()
{
    FILE *fp;

    if((fp = fileopen(GAG_FILE, "r"))) {
        parse_config(fp, NULL);
        fclose(fp);
    }
}


static void
insert_gags  (GtkCList *clist)
{
    extern struct listnode *common_subs;
    gchar *text[1];
    struct listnode *list = mud->activesession ? mud->activesession->subs : common_subs;

    gtk_clist_clear(clist);
    gtk_object_set_user_data(GTK_OBJECT(clist), (void *) -1);
    gtk_clist_freeze(clist);
 
    while ( (list = list->next) ) {
        text[0] = list->left;
        gtk_clist_prepend (GTK_CLIST (clist), text);
    }
    gtk_clist_thaw(clist);
}

void
gag_button_add (GtkCList * data, GtkWidget *button)
{
    const gchar *text;

    text   = gtk_entry_get_text (GTK_ENTRY (lookup_widget(button, "entry_gag")  ));

    if ( text[0] == '\0' )    {
        popup_window (INFO, "Please insert some text first.");
        return;
    }

    if ( strlen (text) < 4)  {
        popup_window (WARN, "It's unsafe to gag a such short text.");
        return;
    }
    
    if ( strlen (text) > GAG_LEN)    {
        popup_window (ERR, "Gag value too big.");
        return;
    }

    add_gag (text);
    insert_gags(data);
}

void gag_button_delete ( GtkCList * data, GtkWidget *button) {
    gchar *word;
    int selected_row = GPOINTER_TO_INT(
            gtk_object_get_user_data(GTK_OBJECT(data)));
    
    if (selected_row == -1 ) {
        popup_window (WARN, "No selection made.");
    }
    else {
        char buffer[GAG_LEN + 20];
        
        gtk_clist_get_text (data, selected_row, 0, &word);

        sprintf(buffer, "#ungag {%s}", word);

        parse_input(buffer, mud->activesession);

        insert_gags(data);
    }

}

void gag_selection_made (GtkWidget *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    gchar *text;

    gtk_clist_get_text (GTK_CLIST(clist), row, 0, &text); 
    gtk_object_set_user_data(GTK_OBJECT(clist), GINT_TO_POINTER(row));
    gtk_entry_set_text (GTK_ENTRY (lookup_widget(clist, "entry_gag")), text);
}

void
gags_window(GtkWidget *w, gpointer data)
{
    if (gag_window) {
        gtk_window_present(GTK_WINDOW(gag_window));
        return;
    }

    gag_window = create_window_gags();

    gtk_signal_connect (GTK_OBJECT (gag_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &gag_window );

    insert_gags  (GTK_CLIST(lookup_widget(gag_window, "clist_gag")));
    gtk_widget_show (gag_window );
}

