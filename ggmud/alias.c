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


#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ggmud.h"
#include "interface.h"
#include "support.h"

#define ALIAS_LEN	20
#define REPL_LEN	1000
#define ALIAS_FILE "aliases"

/* Global variables for alias */
static GtkWidget *alias_window = NULL;

void save_aliases (GtkCList *data, GtkWidget *button)
{
    FILE *fp;
    gint done = FALSE;
    gchar *alias, *replace;
    gint  row = 0;

    if ((fp = fileopen (ALIAS_FILE, "w"))) {
    	while ( !done && data) {
            if ( !gtk_clist_get_text(data, row, 0, &alias)
                || !gtk_clist_get_text(data, row, 1, &replace) )
                break;
            
            if (!alias[0]) {
                done = TRUE;
                break;
            }
            fprintf (fp, "#alias {%s} {%s}\n", alias, replace);
            row++;
        }
        fclose (fp);
    }    
}

static void add_alias(const char *alias, const char *replacement)
{
    char buffer[1024];
    
    snprintf(buffer, sizeof(buffer), "#alias {%s} {%s}", alias, replacement);

    parse_input(buffer, mud->activesession);
}


void load_aliases () 
{
    FILE *fp;

    if((fp = fileopen(ALIAS_FILE, "r"))) {
        parse_config(fp, NULL);
        fclose(fp);
    }
}

#include "include/llist.h"

static void  insert_aliases  (GtkCList *clist)
{
    extern struct listnode *common_aliases;
    gchar *text[2];
    struct listnode *list = mud->activesession ? mud->activesession->aliases : common_aliases;

    gtk_clist_clear(clist);
    gtk_object_set_user_data(GTK_OBJECT(clist), (void *) -1);
    gtk_clist_freeze(clist);
            
    while ( (list = list->next) ) {
        text[0] = list->left;
        text[1] = list->right;
        gtk_clist_append (clist, text);
    }
    gtk_clist_thaw(clist);
}

void alias_selection_made (GtkWidget *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    gchar *text;

    gtk_object_set_user_data(GTK_OBJECT(clist), GINT_TO_POINTER(row));
    gtk_clist_get_text ((GtkCList*) clist, row, 0, &text);
    gtk_entry_set_text (GTK_ENTRY(
                lookup_widget(clist, "entry_alias")), text);
    gtk_clist_get_text ((GtkCList*) clist, row, 1, &text);
    gtk_entry_set_text (GTK_ENTRY(
                lookup_widget(clist, "entry_replace")), text);
}

void alias_button_add (GtkCList *list, GtkWidget *button)
{
    const gchar *text[2];
    gint   i;

    text[0]   = gtk_entry_get_text (GTK_ENTRY (lookup_widget(
                    button, "entry_alias")));
    text[1]   = gtk_entry_get_text (GTK_ENTRY (lookup_widget(
                    button, "entry_replace")));

    if ( text[0][0] == '\0' || text[1][0] == '\0' )
    {
        popup_window (INFO, "Please complete the alias first.");
        return;
    }

    for ( i = 0 ; i < strlen (text[0]) ; i++ )
    {
        if ( isspace (text[0][i]) )
        {
            popup_window (ERR, "I can't make an alias of that.");
            return;
        }
    }

    if ( strlen (text[0]) > ALIAS_LEN)
    {
        popup_window (ERR, "Alias to big.");
        return;
    }
    
    if ( strlen (text[1]) > REPL_LEN)
    {
        popup_window (ERR, "Replace to big.");
        return;
    }

    add_alias (text[0], text[1]);
    insert_aliases(list);
}

void alias_button_delete (GtkCList *data, GtkWidget *button) {
    gchar *word;
    int alias_selected_row = GPOINTER_TO_INT( gtk_object_get_user_data(GTK_OBJECT(data)));
 
    if ( alias_selected_row == -1 ) {
        popup_window (WARN, "No selection made.");
    }
    else {
        char buffer[ALIAS_LEN + 20];
        gtk_clist_get_text(data, alias_selected_row, 0, &word);
        sprintf(buffer, "#unalias %s", word);

        parse_input(buffer, mud->activesession);

        insert_aliases(data);
    }

}

void window_alias (GtkWidget *widget, gpointer data)
{
    if (alias_window) {
        gtk_window_present(GTK_WINDOW(alias_window));
        return;
    }

    alias_window = create_window_aliases();

    gtk_signal_connect (GTK_OBJECT (alias_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &alias_window );

    insert_aliases  (GTK_CLIST(lookup_widget(alias_window, "clist_alias")));
    gtk_widget_show(alias_window );
}
