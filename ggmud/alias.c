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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ggmud.h"

#define ALIAS_LEN	20
#define REPL_LEN	1000
#define ALIAS_FILE "aliases"

/* Global variables for alias */
static GtkWidget *textalias;
static GtkWidget *textreplace;
static GtkWidget *alias_window = NULL;
static gint      alias_selected_row    = -1;

static void save_aliases (GtkWidget *button, gpointer data)
{
    FILE *fp;
    gint done = FALSE;
    gchar *alias, *replace;
    gint  row = 0;

    if ((fp = fileopen (ALIAS_FILE, "w"))) {
    	while ( !done && (GtkCList*) data) {
            if ( !gtk_clist_get_text ((GtkCList*) data, row, 0, &alias)
                || !gtk_clist_get_text ((GtkCList*) data, row, 1, &replace) )
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
    
    sprintf(buffer, "#alias {%s} {%s}", alias, replacement);

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
    gtk_clist_freeze(clist);
            
    while ( (list = list->next) ) {
        text[0] = list->left;
        text[1] = list->right;
        gtk_clist_prepend (clist, text);
    }
    gtk_clist_thaw(clist);
}

static void alias_selection_made (GtkWidget *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    gchar *text;
    
    alias_selected_row    = row;

    if ( (GtkCList*) data )
    {
        gtk_clist_get_text ((GtkCList*) data, row, 0, &text);
        gtk_entry_set_text (GTK_ENTRY (textalias), text);
        gtk_clist_get_text ((GtkCList*) data, row, 1, &text);
        gtk_entry_set_text (GTK_ENTRY (textreplace), text);
    }
    
    return;
}

static void alias_button_add (GtkWidget *button, gpointer data)
{
    const gchar *text[2];
    gint   i;

    text[0]   = gtk_entry_get_text (GTK_ENTRY (textalias  ));
    text[1]   = gtk_entry_get_text (GTK_ENTRY (textreplace));

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
    insert_aliases(GTK_CLIST(data));
}

static void alias_button_delete (GtkWidget *button, gpointer data) {
    gchar *word;
    
    if ( alias_selected_row == -1 ) {
        popup_window (WARN, "No selection made.");
    }
    else {
        char buffer[ALIAS_LEN + 20];
        
        gtk_clist_get_text ((GtkCList*) data, alias_selected_row, 0, &word);
        sprintf(buffer, "#unalias %s", word);
        alias_selected_row = -1;

        parse_input(buffer, mud->activesession);

        insert_aliases(GTK_CLIST(data));
    }

}

void window_alias (GtkWidget *widget, gpointer data)
{
    GtkWidget *vbox, *but;
    GtkWidget *hbox3;
    GtkWidget *clist;
    GtkWidget *label;
    GtkWidget *scrolled_window;

    gchar     *titles[2] = { "Alias", "Replacement" };

    if (alias_window) {
        gtk_window_present(GTK_WINDOW(alias_window));
        return;
    }

    alias_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (alias_window), "Aliases");
    gtk_signal_connect (GTK_OBJECT (alias_window), "destroy",
                               GTK_SIGNAL_FUNC(close_window), alias_window );
    gtk_signal_connect (GTK_OBJECT (alias_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &alias_window );
    gtk_widget_set_usize (alias_window, 450, 320);			       
    vbox = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
    gtk_container_add (GTK_CONTAINER (alias_window), vbox);

    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);

    clist = gtk_clist_new_with_titles (2, titles);
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
                               GTK_SIGNAL_FUNC (alias_selection_made),
                               (gpointer) clist);
    gtk_clist_column_titles_passive (GTK_CLIST (clist));
    gtk_clist_set_shadow_type (GTK_CLIST (clist), GTK_SHADOW_IN);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 100);
    gtk_clist_set_column_width (GTK_CLIST (clist), 1, 250);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 0, GTK_JUSTIFY_LEFT);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 1, GTK_JUSTIFY_LEFT);

    gtk_clist_column_titles_show (GTK_CLIST (clist));

    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), clist);

    hbox3 = gtk_table_new(3, 2, FALSE);

    gtk_box_pack_start (GTK_BOX (vbox), hbox3, FALSE, FALSE, 0);

    
    label = gtk_label_new ("Alias");
    gtk_table_attach(GTK_TABLE(hbox3), label, 1, 2, 0, 1,
                        /*GTK_FILL*/ 0L, /*GTK_FILL*/ 0L, 2, 2);
    
    label = gtk_label_new ("Replacement");
    gtk_table_attach(GTK_TABLE(hbox3), label, 2, 3, 0, 1,
                        GTK_FILL | GTK_EXPAND, /*GTK_FILL*/ 0L, 2, 2);

    but = gtk_button_new();
    label = gtk_image_new_from_stock(GTK_STOCK_FIND, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(but), label);
    gtk_table_attach(GTK_TABLE(hbox3), but, 0, 1, 1, 2,
                         0L, /*GTK_FILL*/ 0L, 2, 2);
    gtk_signal_connect (GTK_OBJECT (but), "clicked",
                       GTK_SIGNAL_FUNC (find_in_list), clist);
    
    textalias   = gtk_entry_new ();
    gtk_table_attach(GTK_TABLE(hbox3), textalias, 1, 2, 1, 2,
                         0L, /*GTK_FILL*/ 0L, 2, 2);

    textreplace = gtk_entry_new ();
    gtk_table_attach(GTK_TABLE(hbox3), textreplace, 2, 3, 1, 2,
                        GTK_FILL | GTK_EXPAND, /*GTK_FILL*/ 0L, 2, 2);
    
    AddButtonBar(vbox, (gpointer)clist,
            GTK_SIGNAL_FUNC(alias_button_add),
            GTK_SIGNAL_FUNC(alias_button_delete),
            GTK_SIGNAL_FUNC(save_aliases));

    insert_aliases  (GTK_CLIST(clist)        );
    gtk_widget_show_all (alias_window );
}
