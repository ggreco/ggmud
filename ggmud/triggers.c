/*  Sclient
 *  Copyright (C) 1999 Heathen (the.heathen@swipnet.se)
 *                1999 Drizzt  (doc.day@swipnet.se)
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

/* Most of the functions in this file is almost identical to the
 * functions in wizard.c and alias.c as they all use the CList
 * widgets.
 */

#include "triggers.h"
#include <string.h>

TRIGGER_DATA *trigger_get_trigger_data (gchar *text) {
    GSList       *tmp;
    TRIGGER_DATA *w;

    for (tmp = trigger_list; tmp; tmp = tmp->next) {
        if (tmp->data) {
            w = (TRIGGER_DATA *) tmp->data;
            if (!strcmp (w->trigger, text)) return w;
        }
    }
    return NULL;
}

void clean_string(gchar *ptr, gchar *result) {
    /* strips off some of non-printable chars from the string */
    while (*ptr) {
	/* Strip out goofy signs like linefeeds.... */
        if (*ptr == '\x0d') {
    	    ptr++;
        } else {	
    	    /* Color and special signs -> strip it! */
            if (*ptr == 27) {
        	/* We want to skip ANSI, but not overrun buffer. */ 
              	while (*ptr && *ptr != 'm') ptr++; 
              	/* If on last character of ANSI code, skip past it. */ 
                if (*ptr == 'm') ptr++; 
            } else {
               /* And here we are with usable characters. */
               *result++ = *ptr++;
            } /* end if */
        } /* end if */
    } /* end while */
    *result = 0;
}

/*
 * Save triggers to file
 */
void save_triggers () {
    FILE *fp;

    if (fp = fileopen ("triggers", "w")) {
        // TODO
    	fclose (fp);
    }	
}

/*
 * Load triggers from file at startup
 */
void load_triggers () {
    FILE *fp;
    
    if (fp = fileopen ("triggers", "r")) {
        // TODO
    	fclose (fp);
    }    
}


void trigger_selection_made (GtkWidget *clist, gint row, gint column,
                                 GdkEventButton *event, gpointer data) {
    TRIGGER_DATA *w;
    gchar *text;
    gint i;
    
    trigger_selected_row = row;
    gtk_clist_get_text (GTK_CLIST(clist), row, 0, &text);
    w = trigger_get_trigger_data (text);
    if (w) {
        if (w->trigger)
            gtk_entry_set_text (GTK_ENTRY (entry_trigger), w->trigger);
	for (i = 0; i < 5; i++) {    
//    	    gtk_entry_set_text (GTK_ENTRY (entry_send[i]), w->send[i]);
	}	
    }
    gtk_widget_set_sensitive (button_apply, TRUE);
    gtk_widget_set_sensitive (button_delete, TRUE);
}

void trigger_unselection_made (GtkWidget *clist, gint row, gint column,
                                   GdkEventButton *event, gpointer data) {

    trigger_selected_row = -1;
    gtk_widget_set_sensitive (button_apply, FALSE);
    gtk_widget_set_sensitive (button_delete, FALSE);
}

void trigger_button_add (GtkWidget *button, gpointer data) {
    TRIGGER_DATA *w;
    gchar *texta[1];
    gchar error[512];
    gint i;

    texta[0] = gtk_entry_get_text (GTK_ENTRY (entry_trigger));

    if (texta[0] && texta[0][0]) {
		if (!trigger_get_trigger_data (texta[0])) {
	    	gtk_clist_append ((GtkCList *) data, texta);
	    	if (!trigger_list || !trigger_list->data) {
    			gtk_clist_select_row ((GtkCList *) data, 0, 0);
	    	}
	    	w = (TRIGGER_DATA *) g_malloc0(sizeof(TRIGGER_DATA));
	    	w->trigger = g_strdup (gtk_entry_get_text (GTK_ENTRY (entry_trigger)));
		} else {
	    	popup_window("Can't add an existing trigger.");
		}
    } else {
		popup_window("You haven't set a trigger.");
    }	 
}

void trigger_button_delete (GtkWidget *button, gpointer data) {
    TRIGGER_DATA *w;
    gchar *word;
    gint i;
    
    if (trigger_selected_row >= 0) {
	gtk_clist_get_text ((GtkCList *) data, trigger_selected_row, 0, &word);
	w = trigger_get_trigger_data(word);
	trigger_list = g_slist_remove (trigger_list, w);
	gtk_clist_remove ((GtkCList *) data, trigger_selected_row);
	trigger_selected_row = -1;
	/* Clear out the entry boxes */
	gtk_entry_set_text (GTK_ENTRY (entry_trigger), "");
	for (i = 0; i < 5; i++) {
	    gtk_entry_set_text (GTK_ENTRY (entry_send[i]), "");
	}
	if (!trigger_list) {
    	    gtk_widget_set_sensitive (button_apply, FALSE);
    	    gtk_widget_set_sensitive (button_delete, FALSE);
	}
    } else {
	popup_window("No selection made");
    }	
}

void trigger_button_modify (GtkWidget *button, gpointer data) {
    TRIGGER_DATA *w;
    gchar *texta[1];
    gint i;
    
    texta[0] = gtk_entry_get_text (GTK_ENTRY (entry_trigger));
    if (texta[0] && texta[0][0]) {
	if (w = trigger_get_trigger_data (texta[0])) {
	    for (i = 0; i < 5; i++) {
//	         strcpy(w->send[i], gtk_entry_get_text(GTK_ENTRY(entry_send[i])));
	    }
	} else {
	    popup_window( "As for the moment, everything but the trigger can be "
                          "changed.\n\nIf you need to change the trigger, you "
		          "have to use delete.");
	}
    } else {
	popup_window("You haven't set a trigger.");
    }	    
}
void trigger_clist_append (TRIGGER_DATA *w, GtkCList *clist) {
    gchar *text[1];
    
    if (w) {
	text[0] = w->trigger;
	gtk_clist_append (GTK_CLIST (clist), text);
    }	
}

void triggers_window () {
    gint i;
    gchar *text;
    GtkWidget *vbox1;
    GtkWidget *hbox1;
    GtkWidget *scrolledwindow_triggerlist;
    GtkWidget *clist;
    GtkWidget *label_clist_trigger;
    GtkWidget *vbox2;
    GtkWidget *label_trigger;
    GtkWidget *hseparator1;
    GtkWidget *label_send;
    GtkWidget *hbuttonbox1;
    GtkWidget *button_add;
    GtkWidget *button_done;
    GtkWidget *button_cancel;
    GtkTooltips *tooltip;

    /* create tooltips */
    tooltip = gtk_tooltips_new ();
    gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);
    /* create window */
    window_triggers = gtk_window_new (GTK_WINDOW_DIALOG);
    gtk_container_border_width (GTK_CONTAINER (window_triggers), 3);
    gtk_window_set_title (GTK_WINDOW (window_triggers), "Triggers");
    gtk_window_set_policy (GTK_WINDOW (window_triggers), FALSE, FALSE, FALSE);

    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (window_triggers), vbox1);
    gtk_container_border_width (GTK_CONTAINER (vbox1), 3);

    hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, TRUE, 0);

    scrolledwindow_triggerlist = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (scrolledwindow_triggerlist);
    gtk_box_pack_start (GTK_BOX (hbox1), scrolledwindow_triggerlist, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_triggerlist), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    clist = gtk_clist_new (1);
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
                               GTK_SIGNAL_FUNC (trigger_selection_made),
                               (gpointer) clist);
    gtk_signal_connect_object (GTK_OBJECT (clist), "unselect_row",
                               GTK_SIGNAL_FUNC (trigger_unselection_made),
                               NULL);
    gtk_widget_show (clist);
    gtk_container_add (GTK_CONTAINER (scrolledwindow_triggerlist), clist);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 80);
    gtk_clist_column_titles_show (GTK_CLIST (clist));

    label_clist_trigger = gtk_label_new ("Trigger");
    gtk_widget_show (label_clist_trigger);
    gtk_clist_set_column_widget (GTK_CLIST (clist), 0, label_clist_trigger);
    gtk_label_set_justify (GTK_LABEL (label_clist_trigger), GTK_JUSTIFY_LEFT);
    gtk_misc_set_alignment (GTK_MISC (label_clist_trigger), 0.0500002, 0.5);

    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (vbox2);
    gtk_box_pack_start (GTK_BOX (hbox1), vbox2, TRUE, TRUE, 0);
    gtk_container_border_width (GTK_CONTAINER (vbox2), 9);

    label_trigger = gtk_label_new ("The Trigger");
    gtk_widget_show (label_trigger);
    gtk_box_pack_start (GTK_BOX (vbox2), label_trigger, TRUE, TRUE, 0);

    entry_trigger = gtk_entry_new ();
    gtk_widget_show (entry_trigger);
    gtk_box_pack_start (GTK_BOX (vbox2), entry_trigger, TRUE, TRUE, 3);
    gtk_tooltips_set_tip (tooltip, entry_trigger,
  			  "This is the text that will cause the trigger to be activated."
			  " NOTE, the trigger string is cAsE sEnSiTiVe!",
                          NULL);

    hseparator1 = gtk_hseparator_new ();
    gtk_widget_show (hseparator1);
    gtk_box_pack_start (GTK_BOX (vbox2), hseparator1, TRUE, TRUE, 5);
    
    for (i = 0; i < 5; i++) {
    	text = g_malloc(18);
    	strcpy(text, " st line to send:");
    	text[0] = i + 49;
  	label_send = gtk_label_new (text);
  	gtk_widget_show (label_send);
  	gtk_box_pack_start (GTK_BOX (vbox2), label_send, TRUE, TRUE, 3);
  	entry_send[i] = gtk_entry_new ();
  	gtk_widget_show (entry_send[i]);
  	gtk_box_pack_start (GTK_BOX (vbox2), entry_send[i], TRUE, TRUE, 0);
  	gtk_tooltips_set_tip (tooltip, entry_send[i],
	                      "This is the string of text that will be sent to the mud,"
			      " when the trigger is activated.", NULL);
	g_free(text);		      
    }
    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 15);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_SPREAD);

    button_add = gtk_button_new_with_label ("Add");
    gtk_signal_connect (GTK_OBJECT (button_add), "clicked",
                             GTK_SIGNAL_FUNC (trigger_button_add),
                             (gpointer) clist);
    gtk_widget_show (button_add);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button_add);
    gtk_container_border_width (GTK_CONTAINER (button_add), 3);

    button_delete = gtk_button_new_with_label ("Delete");
    gtk_signal_connect (GTK_OBJECT (button_delete), "clicked",
                             GTK_SIGNAL_FUNC (trigger_button_delete),
                             (gpointer) clist);
    gtk_widget_show (button_delete);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button_delete);
    gtk_container_border_width (GTK_CONTAINER (button_delete), 3);

    button_apply = gtk_button_new_with_label ("Apply");
    gtk_signal_connect (GTK_OBJECT (button_apply), "clicked",
                             GTK_SIGNAL_FUNC (trigger_button_modify),
                             (gpointer) clist);
    gtk_widget_show (button_apply);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button_apply);
    gtk_container_border_width (GTK_CONTAINER (button_apply), 3);

    button_done = gtk_button_new_with_label ("Done");
    gtk_signal_connect (GTK_OBJECT (button_done), "clicked",
                             GTK_SIGNAL_FUNC (save_triggers), NULL);
    gtk_signal_connect (GTK_OBJECT (button_done), "clicked",
                             GTK_SIGNAL_FUNC (close_window), window_triggers);
    gtk_widget_show (button_done);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button_done);
    gtk_container_border_width (GTK_CONTAINER (button_done), 3);

    button_cancel = gtk_button_new_with_label ("Cancel");
    gtk_signal_connect (GTK_OBJECT (button_cancel), "clicked",
                             GTK_SIGNAL_FUNC (close_window), window_triggers);
    gtk_widget_show (button_cancel);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), button_cancel);
    gtk_container_border_width (GTK_CONTAINER (button_cancel), 3);

    gtk_widget_set_sensitive (button_apply, FALSE);
    gtk_widget_set_sensitive (button_delete, FALSE);

//    g_slist_foreach (trigger_list, (GFunc) trigger_clist_append, clist);
    gtk_clist_select_row (GTK_CLIST (clist), 0, 0);

    gtk_widget_show (window_triggers);
}
