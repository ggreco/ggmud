/*  GGMud
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
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macro.h"

#define MACRO_FILE "macro"

/* The callback function for the ok button */
void button_ok_callback(GtkWidget *button, GtkWidget *entry[]) {
    gint i;
    
    for (i = 0; i <12; i++) {
        if(macro_list[i])
            free(macro_list[i]);
        
    	macro_list[i] = strdup (gtk_entry_get_text(GTK_ENTRY(entry[i])));
    }	
    macro_btnLabel_change();
    save_macro(NULL);
}

/* saves the macro's to file */
void save_macro (gpointer data) {
    FILE *fp;
    gint i = 0;
    
    if (fp = fileopen (MACRO_FILE, "w")) {
	while (keys[i]) {
	    fprintf (fp, "%s %s\n", keys[i], macro_list[i]);
	    i++;
	}    
        fclose (fp);
    }
}

/* load's the macro's from file into the entry boxes */
void load_macro () {
    FILE *fp;
    gchar line[255], macro[5], value[250];
    gint i;
    
    if (!(macro_list = calloc(12, sizeof (gchar *)))) return;
    for (i = 0; i < 12; i++) if (!(macro_list[i] = calloc(1, sizeof(char)))) return;
    
    if (fp = fileopen (MACRO_FILE, "r")) {
        /* load in the macro's into the entry[] boxes */
        while (fgets (line, sizeof(line) - 1, fp)) {
            value[0] = 0;
            sscanf (line, "%s %[^\n]", macro, value);
            i = 0;
            while (keys[i]) {
                if (!strcmp (macro, keys[i])) {
                    if (!(macro_list[i] = realloc(macro_list[i],strlen(value) + 1))) return;
                    strcpy(macro_list[i], value);
                    break;
                }
                i++;
            }
        }    
        fclose (fp);
    }	
}

void window_macro (GtkWidget *widget, gpointer data)
{
  gint i;
  GtkWidget *macro_window;
  GtkWidget *vbox1;
  GtkWidget *frame1;
  GtkWidget *hbox1;
  GtkWidget *vbox2;
  GtkWidget *label;
  GtkWidget *vbox3;
  GtkWidget *vbox4;
  GtkWidget *vbox5;
  GtkWidget *hbuttonbox;
  GtkWidget *button;
  static GtkWidget *entry[12];

  /* macro window */
  macro_window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_widget_set_usize (macro_window, 400, 200);
  gtk_container_border_width (GTK_CONTAINER (macro_window), 3);
  gtk_window_set_title (GTK_WINDOW (macro_window), "Macros");
  gtk_window_set_policy (GTK_WINDOW (macro_window), FALSE, FALSE, FALSE);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (macro_window), vbox1);

  frame1 = gtk_frame_new ("Macros");
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox1), frame1, TRUE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (frame1), 3);
  gtk_frame_set_label_align (GTK_FRAME (frame1), 0.03, 0.5);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (frame1), hbox1);
  /* create vbox's for the left half of the window */
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox2, FALSE, TRUE, 5);
  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox3, TRUE, TRUE, 5);
  /* create labels end entry points for the left half of the window */
  for (i = 0; i < 6; i++) {
    label = gtk_label_new (keys[i]);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (vbox2), label, TRUE, TRUE, 0);
    entry[i] = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY(entry[i]), macro_list[i]);
    gtk_widget_show (entry[i]);
    gtk_box_pack_start (GTK_BOX (vbox3), entry[i], TRUE, TRUE, 0);
    gtk_widget_set_usize (entry[i], 140, -1);
  }
  /* create vbox's for the right half of the window */
  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox4, FALSE, TRUE, 0);
  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox5);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox5, TRUE, TRUE, 5);
  /* create labels end entry points for the right half of the window */
  for (; i < 12; i++) {
    label = gtk_label_new (keys[i]);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (vbox4), label, TRUE, TRUE, 0);
    entry[i] = gtk_entry_new ();
    gtk_entry_set_text (GTK_ENTRY(entry[i]), macro_list[i]);
    gtk_widget_show (entry[i]);
    gtk_box_pack_start (GTK_BOX (vbox5), entry[i], TRUE, TRUE, 0);
    gtk_widget_set_usize (entry[i], 140, -1);
  }
  
  /* create hbox for ok/cancel buttons */
  hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox);
  gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox, FALSE, TRUE, 5);
  gtk_widget_set_usize (hbuttonbox, -1, 25);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);

  button = gtk_button_new_with_label ("Ok");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                     GTK_SIGNAL_FUNC (button_ok_callback),
                     entry);
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (close_window), macro_window);
  gtk_widget_show (button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
  gtk_widget_set_usize (button, -1, 20);
  gtk_container_border_width (GTK_CONTAINER (button), 3);

  button = gtk_button_new_with_label ("Cancel");
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (close_window), macro_window);
  gtk_widget_show (button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
  gtk_widget_set_usize (button, -1, 20);
  gtk_container_border_width (GTK_CONTAINER (button), 3);

  gtk_widget_show (macro_window);
}
