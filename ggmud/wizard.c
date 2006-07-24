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

#include "ggmud.h"

#define CONN_FILE "connections"

/* Global Variables */
static gint         wizard_selected_row;
static GtkWidget   *wizard_entry_name;
static GtkWidget   *wizard_entry_host;
static GtkWidget   *wizard_entry_port;
static GtkWidget   *wizard_check_autologin;
static GtkWidget   *wizard_entry_player;
static GtkWidget   *wizard_entry_password;
static GtkWidget   *wizard_window;
static GtkWidget   *button_update;
static GtkWidget   *button_delete;
static GtkWidget   *button_connect;
static GList       *wizard_connection_list2;
WIZARD_DATA *wizard_autologin = NULL;

void
wiz_destructify()
{
    gtk_widget_hide(wizard_window);
    return;
}

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

static void save_wizard ()
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

    gtk_clist_get_text (GTK_CLIST(clist)/*(GtkCList*) data*/, row, 0, &text);

    w = wizard_get_wizard_data ( text );

    if ( w != NULL)
    {
        if ( w->name)
            gtk_entry_set_text (GTK_ENTRY (wizard_entry_name), w->name);
        if ( w->hostname)
            gtk_entry_set_text (GTK_ENTRY (wizard_entry_host), w->hostname);
        if ( w->port)
            gtk_entry_set_text (GTK_ENTRY (wizard_entry_port), w->port);
        
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (wizard_check_autologin), w->autologin);

        if ( w->autologin )
        {
            if ( w->playername )
                gtk_entry_set_text (GTK_ENTRY (wizard_entry_player), w->playername);
            if ( w->password )
                gtk_entry_set_text (GTK_ENTRY (wizard_entry_password), w->password);
        }
        else
        {
            gtk_entry_set_text (GTK_ENTRY (wizard_entry_player), "");
            gtk_entry_set_text (GTK_ENTRY (wizard_entry_password), "");
            gtk_widget_set_sensitive (wizard_entry_player, FALSE);
            gtk_widget_set_sensitive (wizard_entry_password, FALSE);
        }
    }

    gtk_widget_set_sensitive (button_update, TRUE);
    gtk_widget_set_sensitive (button_delete, TRUE);
    gtk_widget_set_sensitive (button_connect, TRUE);
}

static void wizard_unselection_made (GtkWidget *clist, gint row, gint column,
                              GdkEventButton *event, gpointer data)
{
    wizard_selected_row = -1;

    gtk_widget_set_sensitive (button_update, FALSE);
    gtk_widget_set_sensitive (button_delete, FALSE);
    gtk_widget_set_sensitive (button_connect, FALSE);
}

static void wizard_button_connect (GtkWidget *button, gpointer data)
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
		              "Either disconnect or start \n"
		              "       an new client.        ");   
        return;
    }

    gtk_clist_get_text ((GtkCList *) data, wizard_selected_row, 0, &word);

    w = wizard_get_wizard_data(word);

    make_connection (w->name, w->hostname, w->port);

    wizard_autologin = w;

}

static void wizard_button_delete (GtkWidget *button, gpointer data)
{
    WIZARD_DATA *w;
    gchar *word;
    
    if ( wizard_selected_row < 0 )
    {
        popup_window (WARN, "No selection made");
        return;
    }
    
    gtk_clist_get_text ((GtkCList *) data, wizard_selected_row, 0, &word);

    w = wizard_get_wizard_data (word);
    
    wizard_connection_list2 = g_list_remove (wizard_connection_list2, w);
    
    gtk_clist_remove ((GtkCList *) data, wizard_selected_row);
    wizard_selected_row = -1;

    if ( wizard_connection_list2 == NULL )
    {
        gtk_widget_set_sensitive (button_update, FALSE);
        gtk_widget_set_sensitive (button_delete, FALSE);
        gtk_widget_set_sensitive (button_connect, FALSE);
    }
}

static void wizard_button_modify (GtkWidget *button, gpointer data)
{
    WIZARD_DATA *w;
    const gchar *texta[1];

    texta[0] = gtk_entry_get_text (GTK_ENTRY (wizard_entry_name));

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
    w->hostname = strdup (gtk_entry_get_text (GTK_ENTRY (wizard_entry_host)));
    
    if(w->port)
        free (w->port); 
    w->port = strdup (gtk_entry_get_text (GTK_ENTRY (wizard_entry_port)));
    
    if(w->playername)
        free(w->playername); 
    w->playername = strdup (gtk_entry_get_text (GTK_ENTRY (wizard_entry_player)));
    
    if(w->password)
        free (w->password);
    w->password = strdup (gtk_entry_get_text (GTK_ENTRY (wizard_entry_password)));

    if ( GTK_TOGGLE_BUTTON (wizard_check_autologin)->active )
        w->autologin = TRUE;
    else
        w->autologin = FALSE;
}

static void wizard_button_add (GtkWidget *button, gpointer data)
{
    WIZARD_DATA *w;
    const gchar *texta[1];

    texta[0] = gtk_entry_get_text (GTK_ENTRY (wizard_entry_name));

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

    w->name       = strdup (gtk_entry_get_text (GTK_ENTRY (wizard_entry_name)));
    w->hostname   = strdup (gtk_entry_get_text (GTK_ENTRY (wizard_entry_host)));
    w->port       = strdup (gtk_entry_get_text (GTK_ENTRY (wizard_entry_port)));
    w->playername = strdup (gtk_entry_get_text (GTK_ENTRY (wizard_entry_player)));
    w->password   = strdup (gtk_entry_get_text (GTK_ENTRY (wizard_entry_password)));
    
    if ( GTK_TOGGLE_BUTTON (wizard_check_autologin)->active )
        w->autologin = TRUE;
    else
        w->autologin = FALSE;

    wizard_connection_list2 = g_list_append (wizard_connection_list2, w);

    gtk_widget_set_sensitive (button_update, TRUE);
    gtk_widget_set_sensitive (button_delete, TRUE);
    gtk_widget_set_sensitive (button_connect, TRUE);
}

static void wizard_check_callback (GtkWidget *widget, GtkWidget *check_button)
{
    if ( GTK_TOGGLE_BUTTON (check_button)->active )
    {
        gtk_widget_set_sensitive (wizard_entry_player, TRUE);
        gtk_widget_set_sensitive (wizard_entry_password, TRUE);
    }
    else
    {
        gtk_widget_set_sensitive (wizard_entry_player, FALSE);
        gtk_widget_set_sensitive (wizard_entry_password, FALSE);
    }
}


void do_wiz (GtkWidget *widget, gpointer data)
{

    GtkWidget *hbox;
    GtkWidget *hbox2;
    GtkWidget *hbox3;
    GtkWidget *vbox_base;
    GtkWidget *vbox;
    GtkWidget *clist;
    GtkWidget *label;
    GtkWidget *button_add;
    GtkWidget *button_save;
    GtkWidget *button_close;
    GtkWidget *separator;
    GtkTooltips *tooltip;

    gchar *titles[1] = { "Connections" };
    
    tooltip = gtk_tooltips_new ();
//    gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);


    wizard_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (wizard_window), "Connection Wizard");
    gtk_signal_connect (GTK_OBJECT (wizard_window), "destroy",
                               GTK_SIGNAL_FUNC(close_window), wizard_window );
    gtk_widget_set_usize (wizard_window,450, -1);

    vbox_base = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox_base), 5);
    gtk_container_add (GTK_CONTAINER (wizard_window), vbox_base);
    gtk_widget_show (vbox_base);

    hbox = gtk_hbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);
    gtk_container_add (GTK_CONTAINER (vbox_base), hbox);
    gtk_widget_show (hbox);

    clist = gtk_clist_new_with_titles ( 1, titles);
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
                               GTK_SIGNAL_FUNC (wizard_selection_made),
                               (gpointer) clist);
    gtk_signal_connect_object (GTK_OBJECT (clist), "unselect_row",
                               GTK_SIGNAL_FUNC (wizard_unselection_made),
                               NULL);
    gtk_clist_set_shadow_type (GTK_CLIST (clist), GTK_SHADOW_IN);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 50);
    gtk_clist_column_titles_passive (GTK_CLIST (clist));
    gtk_box_pack_start (GTK_BOX (hbox), clist, TRUE, TRUE, 0);
    gtk_widget_show (clist);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
    gtk_container_add (GTK_CONTAINER (hbox), vbox);
    gtk_widget_show (vbox);

    label = gtk_label_new ("Connection Name:");
    gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    wizard_entry_name = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (vbox), wizard_entry_name, FALSE, FALSE, 0);
    gtk_tooltips_set_tip (tooltip, wizard_entry_name, "This is what you will "
                          "call the connection, and will also be used in when "
                          "you chose a connection in the list to the left",
                          NULL);
    gtk_widget_show (wizard_entry_name);

    label = gtk_label_new ("\nHost:");
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    wizard_entry_host = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (vbox), wizard_entry_host, FALSE, FALSE, 0);
    gtk_tooltips_set_tip (tooltip, wizard_entry_host, "This is the host of "
                          "where the mud you will connect to is located.",
                          NULL);
    gtk_widget_show (wizard_entry_host);

    label = gtk_label_new ("\nPort:");
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    wizard_entry_port = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (vbox), wizard_entry_port, FALSE, FALSE, 0);
    gtk_tooltips_set_tip (tooltip, wizard_entry_port, "This is the port of "
                          "the host that the mud is located on.\n"
                          "Default set to: 23",
                          NULL);
    gtk_widget_show (wizard_entry_port);

    wizard_check_autologin = gtk_check_button_new_with_label ("Auto Login?");
    gtk_signal_connect (GTK_OBJECT (wizard_check_autologin), "toggled",
                        GTK_SIGNAL_FUNC (wizard_check_callback),
                        wizard_check_autologin);
    gtk_box_pack_start (GTK_BOX (vbox), wizard_check_autologin, FALSE, FALSE, 0);
    gtk_tooltips_set_tip (tooltip, wizard_check_autologin,
                          "Should GGMud login to this mud automatically?\n"
                          "For this to work, Player Name and Password must "
                          "be set.",
                          NULL);
    GTK_WIDGET_UNSET_FLAGS (wizard_check_autologin, GTK_CAN_FOCUS);
    gtk_widget_show (wizard_check_autologin);
    
    label = gtk_label_new ("\nPlayer Name:");
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    wizard_entry_player = gtk_entry_new ();
    gtk_box_pack_start (GTK_BOX (vbox), wizard_entry_player, FALSE, FALSE, 0);
    gtk_tooltips_set_tip (tooltip, wizard_entry_player,
                          "This is the player you login to the mud with, this "
                          "only works if AutoLogin is set.",
                          NULL);
    gtk_widget_show (wizard_entry_player);

    label = gtk_label_new ("\nPassword:");
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);

    wizard_entry_password = gtk_entry_new ();
    gtk_entry_set_visibility (GTK_ENTRY (wizard_entry_password), FALSE);
    gtk_box_pack_start (GTK_BOX (vbox), wizard_entry_password, FALSE, FALSE, 0);
    gtk_tooltips_set_tip (tooltip, wizard_entry_password,
                          "Use this together with PlayerName and AutoLogin.",
                          NULL);
    gtk_widget_show (wizard_entry_password);

    hbox2 = gtk_hbox_new (FALSE, 5);
    gtk_container_add (GTK_CONTAINER (vbox_base), hbox2);
    gtk_widget_show (hbox2);

    button_add     = gtk_button_new_from_stock (GTK_STOCK_ADD);
    gtk_signal_connect (GTK_OBJECT (button_add), "clicked",
                               GTK_SIGNAL_FUNC (wizard_button_add),
                               (gpointer) clist);
    gtk_box_pack_start (GTK_BOX (hbox2), button_add,    TRUE, TRUE, 15);
    gtk_widget_show (button_add);

    button_update  = gtk_button_new_from_stock (GTK_STOCK_APPLY);
    gtk_signal_connect (GTK_OBJECT (button_update), "clicked",
                               GTK_SIGNAL_FUNC (wizard_button_modify),
                               (gpointer) clist);
    gtk_box_pack_start (GTK_BOX (hbox2), button_update, TRUE, TRUE, 15);
    gtk_widget_show (button_update);

    button_delete  = gtk_button_new_from_stock (GTK_STOCK_DELETE);
    gtk_signal_connect (GTK_OBJECT (button_delete), "clicked",
                               GTK_SIGNAL_FUNC (wizard_button_delete),
                               (gpointer) clist);
    gtk_box_pack_start (GTK_BOX (hbox2), button_delete, TRUE, TRUE, 15);
    gtk_widget_show (button_delete);

    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox_base), separator, FALSE, TRUE, 5);
    gtk_widget_show (separator);

    hbox3 = gtk_hbox_new (FALSE, 5);
    gtk_container_add (GTK_CONTAINER (vbox_base), hbox3);
    gtk_widget_show (hbox3);

    if (gtk_minor_version > 7)
        button_connect = gtk_button_new_from_stock ("gtk-connect");
    else
        button_connect = gtk_button_new_with_label ("Connect");
        
    gtk_signal_connect (GTK_OBJECT (button_connect), "clicked",
                               GTK_SIGNAL_FUNC (wizard_button_connect),(gpointer) clist);
    gtk_box_pack_start (GTK_BOX (hbox3), button_connect, TRUE, TRUE, 15);
    gtk_widget_show (button_connect);

    button_save    = gtk_button_new_from_stock (GTK_STOCK_SAVE);
    gtk_signal_connect (GTK_OBJECT (button_save), "clicked",
                               GTK_SIGNAL_FUNC (save_wizard), NULL);
    gtk_box_pack_start (GTK_BOX (hbox3), button_save,    TRUE, TRUE, 15);
    gtk_widget_show (button_save);

    button_close   = gtk_button_new_from_stock (GTK_STOCK_CLOSE);
    gtk_signal_connect (GTK_OBJECT (button_close), "clicked",
                               GTK_SIGNAL_FUNC (close_window),wizard_window);
    gtk_box_pack_start (GTK_BOX (hbox3), button_close,   TRUE, TRUE, 15);
    gtk_widget_show (button_close);

    gtk_widget_set_sensitive (button_update, FALSE);
    gtk_widget_set_sensitive (button_delete, FALSE);
    gtk_widget_set_sensitive (button_connect, FALSE);

    g_list_foreach (wizard_connection_list2, (GFunc) wizard_clist_append, clist);
    gtk_clist_select_row (GTK_CLIST (clist), 0, 0);
    
    gtk_widget_show (wizard_window);
}
