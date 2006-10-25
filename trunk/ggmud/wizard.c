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
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include "interface.h"
#include "ggmud.h"

#define CONN_FILE "connections"

/* Global Variables */
static gint         wizard_selected_row;
static GList       *wizard_connection_list2;
WIZARD_DATA *wizard_autologin = NULL;

static void free_wizard_data ( WIZARD_DATA *w )
{
    free (w->name);
    free (w->hostname);
    free (w->port);
    if(w->playername)
        free (w->playername);
    if(w->password)
        free (w->password);
    
    free (w);
}

void load_wizard ()
{
    WIZARD_DATA *w = NULL;
    FILE *fp;
    gchar line[1024], value[1004], name[256];

    if ((fp = fileopen (CONN_FILE, "r"))) {
        while (fgets (line, sizeof(line) - 1, fp)) {
            sscanf (line, "%s %[^\n]", name, value);
            if (!strcmp (name, "Connection")) {
                if (w) {
                    if (!wizard_connection_list2) wizard_connection_list2 = g_list_alloc ();
                    wizard_connection_list2 = g_list_append (wizard_connection_list2, w);
                }
                w = (WIZARD_DATA *) calloc(sizeof (WIZARD_DATA), 1);
                w->name = strdup (value);
            }
            if(w) {
                if (!strcmp (name, "Hostname")) w->hostname = strdup (value);
                if (!strcmp (name, "Port")) w->port = strdup (value);
                if (!strcmp (name, "Player")) w->playername = strdup (value);
                if (!strcmp (name, "Password")) w->password = strdup (value);
                if (!strcmp (name, "AutoLogin")) w->autologin = TRUE;
            }
        }
        if (w) {
            if (w->name) {
                if (!wizard_connection_list2) wizard_connection_list2 = g_list_alloc ();
                wizard_connection_list2 = g_list_append (wizard_connection_list2, w);
            } 
            else if (w) free_wizard_data (w);
            wizard_connection_list2 = wizard_connection_list2->next;
            wizard_connection_list2->prev = NULL;
        }
        fclose (fp);
    }    
}

void on_wiz_save__clicked   (GtkButton       *button,
                             gpointer         user_data)
{
    GList       *tmp;
    WIZARD_DATA *w;
    FILE *fp;

    if ((fp = fileopen(CONN_FILE, "w"))) {
    	for (tmp = wizard_connection_list2; tmp; tmp = tmp->next) {
            if (tmp->data) {
                w = (WIZARD_DATA *) tmp->data;
            	fprintf (fp, "Connection %s\n", w->name);
            	if (w->hostname && strlen (w->hostname)) fprintf (fp, "Hostname %s\n", w->hostname);
            	if (w->port && strlen (w->port)) fprintf (fp, "Port %s\n", w->port);
            	if (w->playername && strlen (w->playername)) fprintf (fp, "Player %s\n", w->playername);
            	if (w->password && strlen (w->password)) fprintf (fp, "Password %s\n", w->password);
            	if (w->autologin) fprintf (fp, "AutoLogin YES\n");
            	fprintf (fp, "\n");
            }
        }
    	fclose (fp);
    }
}

static WIZARD_DATA *wizard_get_wizard_data (const gchar *text )
{
    GList       *tmp;
    WIZARD_DATA *w;

    for (tmp = g_list_first (wizard_connection_list2); tmp != NULL; tmp = tmp->next)
    {
        if ( tmp->data != NULL)
        {
            w = (WIZARD_DATA *) tmp->data;

            if ( !strcmp (w->name, text) )
                 return w;
        }
    }

    return NULL;
}

static void wizard_clist_append (WIZARD_DATA *w, GtkCList *clist)
{
    if ( w )
    {
        gchar *text[1];

        text[0] = w->name;

        gtk_clist_append (GTK_CLIST (clist), text);
    }
}

static void wizard_selection_made (GtkWidget *clist, gint row, gint column,
                            GdkEventButton *event, gpointer data)
{
    WIZARD_DATA *w;
    
    gchar *text;

    wizard_selected_row = row;

    gtk_clist_get_text (GTK_CLIST(clist), row, 0, &text);

    w = wizard_get_wizard_data ( text );

    if ( w != NULL)
    {
        if ( w->name)
            gtk_entry_set_text (GTK_ENTRY (lookup_widget(clist, "entry_name")), w->name);
        if ( w->hostname)
            gtk_entry_set_text (GTK_ENTRY (lookup_widget(clist, "entry_host")), w->hostname);
        if ( w->port)
            gtk_entry_set_text (GTK_ENTRY (lookup_widget(clist, "entry_port")), w->port);

        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(
                    lookup_widget(clist, "checkbutton_autologin")), w->autologin);

        if ( w->playername )
            gtk_entry_set_text (GTK_ENTRY (
                        lookup_widget(clist, "entry_char_name")), w->playername);
        if ( w->password )
            gtk_entry_set_text (GTK_ENTRY (
                        lookup_widget(clist, "entry_pwd")), w->password);
        gtk_widget_set_sensitive (lookup_widget(clist, "button_update"), TRUE);
        gtk_widget_set_sensitive (lookup_widget(clist, "button_ok"), TRUE);
        gtk_widget_set_sensitive (lookup_widget(clist, "button_delete"), TRUE);
    }
}

static void wizard_unselection_made (GtkWidget *clist, gint row, gint column,
                              GdkEventButton *event, GtkWidget * data)
{
    wizard_selected_row = -1;

    gtk_widget_set_sensitive (lookup_widget(data, "button_update"), FALSE);
    gtk_widget_set_sensitive (lookup_widget(data, "button_ok"), FALSE);
    gtk_widget_set_sensitive (lookup_widget(data, "button_delete"), FALSE);
}

void
on_wiz_ok_clicked    (GtkWidget *button, gpointer data)
{
    WIZARD_DATA *w;
    gchar *word;

    if ( wizard_selected_row < 0 )
    {
        popup_window (WARN, "No selection made");
        return;
    }
    
    /* just so we don't try to connect while connected, 
       wich would crash the client??? */
    if ( connected )
    {
        popup_window (INFO, "You are already connected.\n"
		              "Either disconnect or start\n"
		              "an new client.");   
        return;
    }

    gtk_clist_get_text (GTK_CLIST(lookup_widget(button, "clist_conns")), wizard_selected_row, 0, &word);

    w = wizard_get_wizard_data(word);

    make_connection (w->name, w->hostname, w->port);

    wizard_autologin = w;

    gtk_widget_destroy(gtk_widget_get_toplevel(button));
}

void
on_button_wiz_delete_clicked  (GtkWidget *button, gpointer data)
{
    WIZARD_DATA *w;
    gchar *word;
    
    if ( wizard_selected_row < 0 )
    {
        popup_window (WARN, "No selection made");
        return;
    }
    
    gtk_clist_get_text (GTK_CLIST(lookup_widget(button, "clist_conns")), 
            wizard_selected_row, 0, &word);

    w = wizard_get_wizard_data (word);
    
    wizard_connection_list2 = g_list_remove (wizard_connection_list2, w);
    
    gtk_clist_remove (GTK_CLIST(lookup_widget(button, "clist_conns")), 
            wizard_selected_row);
    wizard_selected_row = -1;

    if ( wizard_connection_list2 == NULL )
    {
        gtk_widget_set_sensitive (lookup_widget(button, "button_update"), FALSE);
        gtk_widget_set_sensitive (lookup_widget(button, "button_delete"), FALSE);
        gtk_widget_set_sensitive (lookup_widget(button, "button_ok"), FALSE);
    }
}

void on_button_wiz_apply_clicked (GtkWidget *button, gpointer data)
{
    WIZARD_DATA *w;
    const gchar *texta[1];

    texta[0] = gtk_entry_get_text (GTK_ENTRY (lookup_widget(button, "entry_name")));

    if ( texta[0] == NULL || texta[0][0] == '\0' )
    {
        popup_window (ERR, "Your connection doesn't have a name." );
        return;
    }

    if ( (  w = wizard_get_wizard_data (texta[0]) ) == NULL )
    {
        popup_window (INFO, "As for the moment, everything but the name can be "
                       "changed.\n\nIf you need to change the name of the "
                       "connection, you have to use delete.");
        return;
    }

    if(w->hostname)
        free (w->hostname);  
    w->hostname = strdup (gtk_entry_get_text (GTK_ENTRY (lookup_widget(button, "entry_host"))));
    
    if(w->port)
        free (w->port); 
    w->port = strdup (gtk_entry_get_text (GTK_ENTRY (lookup_widget(button, "entry_port"))));
    
    if(w->playername)
        free(w->playername); 
    w->playername = strdup (gtk_entry_get_text (GTK_ENTRY (lookup_widget(button, "entry_char_name"))));
    
    if(w->password)
        free (w->password);
    w->password = strdup (gtk_entry_get_text (GTK_ENTRY (lookup_widget(button, "entry_name"))));

    if ( GTK_TOGGLE_BUTTON (lookup_widget(button, "checkbutton_autologin"))->active )
        w->autologin = TRUE;
    else
        w->autologin = FALSE;
}

void on_button_wiz_add_clicked (GtkWidget *button, gpointer data)
{
    WIZARD_DATA *w;
    const gchar *texta[1];

    texta[0] = gtk_entry_get_text (GTK_ENTRY (
                lookup_widget(button, "entry_name")));

    if ( texta[0] == NULL || texta[0][0] == '\0' )
    {
        popup_window (ERR, "Your connection doesn't have a name." );
        return;
    }

    if ( wizard_get_wizard_data (texta[0]) )
    {
        popup_window (WARN, "Can't add an existing connection.");
        return;
    }

    gtk_clist_append ((GtkCList *) data, (void *)texta);

    if ( !wizard_connection_list2 || !wizard_connection_list2->data )
        gtk_clist_select_row ((GtkCList *) data, 0, 0);

    w = (WIZARD_DATA *) calloc( sizeof (WIZARD_DATA), 1);

    w->name       = strdup (gtk_entry_get_text (
                GTK_ENTRY (lookup_widget(button, "entry_name"))));
    w->hostname   = strdup (gtk_entry_get_text (
                GTK_ENTRY (lookup_widget(button, "entry_host"))));
    w->port       = strdup (gtk_entry_get_text (
                GTK_ENTRY (lookup_widget(button, "entry_port"))));
    w->playername = strdup (gtk_entry_get_text (
                GTK_ENTRY (lookup_widget(button, "entry_char_name"))));
    w->password   = strdup (gtk_entry_get_text (
                GTK_ENTRY (lookup_widget(button, "entry_pwd"))));
    
    if ( GTK_TOGGLE_BUTTON (
                lookup_widget(button, "checkbutton_autologin"))->active )
        w->autologin = TRUE;
    else
        w->autologin = FALSE;

    wizard_connection_list2 = g_list_append (wizard_connection_list2, w);

    gtk_widget_set_sensitive (
            lookup_widget(button, "button_update"), TRUE);
    gtk_widget_set_sensitive (
            lookup_widget(button, "button_delete"), TRUE);
    gtk_widget_set_sensitive (
            lookup_widget(button, "button_ok"), TRUE);
}

void do_wiz (GtkWidget *widget, gpointer data)
{
    GtkWidget *w,
              *wizard_window = create_window_wizard();

    w = lookup_widget(wizard_window, "clist_conns");

    gtk_signal_connect_object (GTK_OBJECT (w), "select_row",
                               GTK_SIGNAL_FUNC (wizard_selection_made),
                               (gpointer) w);
    gtk_signal_connect_object (GTK_OBJECT (w), "unselect_row",
                               GTK_SIGNAL_FUNC (wizard_unselection_made),
                               w);

    gtk_widget_set_sensitive (lookup_widget(wizard_window, "button_ok"), FALSE);
    gtk_widget_set_sensitive (lookup_widget(wizard_window, "button_delete"), FALSE);
    gtk_widget_set_sensitive (lookup_widget(wizard_window, "button_update"), FALSE);

    g_list_foreach (wizard_connection_list2, (GFunc) wizard_clist_append, w);
    gtk_clist_select_row (GTK_CLIST (w), 0, 0);
    
    gtk_widget_show (wizard_window);
}
