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

#define ORIG_LEN	100
#define REPL_LEN	1000
#define SUB_FILE "subs"

/* Global variables for substitutions */
static GtkWidget *sub_window = NULL;

void save_subs (GtkCList *data, GtkWidget *button)
{
    FILE *fp;
    gint done = FALSE;
    gchar *orig, *replace;
    gint  row = 0;

    if ((fp = fileopen (SUB_FILE, "w"))) {
    	while ( !done && data) {
            if ( !gtk_clist_get_text(data, row, 0, &orig)
                || !gtk_clist_get_text(data, row, 1, &replace) )
                break;
            
            if (!orig[0]) {
                done = TRUE;
                break;
            }
            fprintf (fp, "#subst {%s} {%s}\n", orig, replace);
            row++;
        }
        fclose (fp);
    }    
}

static void add_sub(const char *orig, const char *replacement)
{
    char buffer[1024];
    
    snprintf(buffer, sizeof(buffer), "#subst {%s} {%s}", orig, replacement);

    parse_input(buffer, mud->activesession);
}


void load_subs () 
{
    FILE *fp;

    if((fp = fileopen(SUB_FILE, "r"))) {
        parse_config(fp, NULL);
        fclose(fp);
    }
}

#include "include/llist.h"

static void  insert_subs  (GtkCList *clist)
{
    extern struct listnode *common_subs;
    gchar *text[2];
    struct listnode *list = mud->activesession ? mud->activesession->subs : common_subs;

    gtk_clist_clear(clist);
    gtk_object_set_user_data(GTK_OBJECT(clist), GINT_TO_POINTER(-1));
    gtk_clist_freeze(clist);
            
    while ( (list = list->next) ) {
        text[0] = list->left;
        text[1] = list->right;
        // e' un gag, lo skippo
        if (list->right[0] == '.' && list->right[1] == 0)
            continue;

        gtk_clist_append (clist, text);
    }
    gtk_clist_thaw(clist);
}

void sub_selection_made (GtkWidget *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    gchar *text;

    gtk_object_set_user_data(GTK_OBJECT(clist), GINT_TO_POINTER(row));
    gtk_clist_get_text ((GtkCList*) clist, row, 0, &text);
    gtk_entry_set_text (GTK_ENTRY(
                lookup_widget(clist, "entry_orig")), text);
    gtk_clist_get_text ((GtkCList*) clist, row, 1, &text);
    gtk_entry_set_text (GTK_ENTRY(
                lookup_widget(clist, "entry_replacement")), text);
}

void sub_button_add (GtkCList *list, GtkWidget *button)
{
    const gchar *text[2];

    text[0]   = gtk_entry_get_text (GTK_ENTRY (lookup_widget(
                    button, "entry_orig")));
    text[1]   = gtk_entry_get_text (GTK_ENTRY (lookup_widget(
                    button, "entry_replacement")));

    if ( text[0][0] == '\0' || text[1][0] == '\0' ) {
        popup_window (INFO, "Please fill both fields first.");
        return;
    }

    if ( strlen (text[0]) > ORIG_LEN) {
        popup_window (ERR, "Original string too big.");
        return;
    }
    
    if ( strlen (text[1]) > REPL_LEN) {
        popup_window (ERR, "Replacement too big.");
        return;
    }

    add_sub (text[0], text[1]);
    insert_subs(list);
}

void sub_button_delete (GtkCList *data, GtkWidget *button) {
    gchar *word;
    int sub_selected_row = GPOINTER_TO_INT( gtk_object_get_user_data(GTK_OBJECT(data)));
 
    if ( sub_selected_row == -1 ) {
        popup_window (WARN, "No selection made.");
    }
    else {
        char buffer[ORIG_LEN + 20];
        gtk_clist_get_text(data, sub_selected_row, 0, &word);
        sprintf(buffer, "#unsub {%s}", word);

        parse_input(buffer, mud->activesession);

        insert_subs(data);
    }

}

void window_subs (GtkWidget *widget, gpointer data)
{
    if (sub_window) {
        gtk_window_present(GTK_WINDOW(sub_window));
        return;
    }

    sub_window = create_window_sub();

    gtk_signal_connect (GTK_OBJECT (sub_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &sub_window );

    insert_subs  (GTK_CLIST(lookup_widget(sub_window, "clist_sub")));
    gtk_widget_show(sub_window );
}
