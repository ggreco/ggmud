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
#include <gdk/gdkkeysyms.h>

#include <gtk/gtk.h>
#include <glib.h>

#include "help.h"


/* About txt */
const char * ABOUT_MESSAGE = "\n\n Author:\n"
"    Gabriele Greco (gabriele.greco@aruba.it)\n"
"\n"
" SClient authors:\n"
"    P.E. Segolsson, Fredrik Andersson\n"
"\n"
" TinTin++ authors:\n"
"    Bill Reiss, David A. Wagner, Rob Ellsworth,\n"
"    Jeremy C. Jack, Davin Chan\n"
"\n"  
" WWW:\n" 
"    http://ggmud.sourceforge.net\n" 
"\n";


/* Contents txt */
const char * CONTENTS_MESSAGE = "TODO";

/* Wizard txt */
const char * WIZARD_MESSAGE = "TODO";

/* Macro txt */
const char * MACROS_MESSAGE = "TODO";

/* Alias txt */
const char * ALIAS_MESSAGE = "TODO";

/* Triggers txt */
const char * TRIGGERS_MESSAGE = "TODO";

/* Logger txt */
const char * LOGGER_MESSAGE = "TODO";

/* Logviewer txt */
const char * LOGVIEWER_MESSAGE ="TODO";

/* Font txt */
const char * FONT_MESSAGE = "TODO";

/* Color txt */
const char * COLOR_MESSAGE = "TODO";

void do_about (GtkWidget *widget, gpointer data)
{
  GtkWidget *about_window;
  GtkWidget *vbox;
  GtkWidget *frame;
  GtkWidget *text_about;
  GtkWidget *hbuttonbox;
  GtkWidget *ok_button;

  about_window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_widget_set_usize (about_window, 400, 375);
  gtk_window_set_title (GTK_WINDOW (about_window), "About GGMud");
  gtk_window_set_policy (GTK_WINDOW (about_window), FALSE, FALSE, FALSE);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (about_window), vbox);

  frame = gtk_frame_new ("The GGMud Team");
  gtk_object_set_data (GTK_OBJECT (about_window), "frame", frame);
  gtk_widget_show (frame);
  gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 5);
  gtk_widget_set_usize (frame, -1, 200);
  gtk_container_set_border_width (GTK_CONTAINER (frame), 5);
  gtk_frame_set_label_align (GTK_FRAME (frame), 0.03, 0.5);

  text_about = gtk_text_new (NULL, NULL);
  gtk_widget_show (text_about);
  gtk_container_add (GTK_CONTAINER (frame), text_about);

  hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 5);
  gtk_container_set_border_width (GTK_CONTAINER (hbuttonbox), 3);

  ok_button = gtk_button_new_with_label ("Ok");
  gtk_signal_connect (GTK_OBJECT (ok_button), "clicked",
                      GTK_SIGNAL_FUNC (close_window), about_window);
  gtk_widget_show (ok_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), ok_button);
  gtk_container_border_width (GTK_CONTAINER (ok_button), 3);

  gtk_widget_show (about_window);
  gdk_window_set_background(GTK_TEXT(text_about)->text_area, &color_black);

  gtk_text_insert(GTK_TEXT(text_about), NULL, &color_lightgreen, &color_black, ABOUT_MESSAGE,-1); // in help.h

}

void set_notebook_tab (GtkWidget *notebook, gint page_num, GtkWidget *widget)
{
  GtkNotebookPage *page;
  GtkWidget *notebook_page;

  page = (GtkNotebookPage*) g_list_nth (GTK_NOTEBOOK (notebook)->children, page_num)->data;
  notebook_page = page->child;
  gtk_widget_ref (notebook_page);
  gtk_notebook_remove_page (GTK_NOTEBOOK (notebook), page_num);
  gtk_notebook_insert_page (GTK_NOTEBOOK (notebook), notebook_page,
                            widget, page_num);
  gtk_widget_unref (notebook_page);
}


void do_manual(GtkWidget *widget, gpointer data)
{
  GtkWidget *man_window;
  GtkWidget *vbox;
  GtkWidget *notebook;
  GtkWidget *scrolledwindow_contents;
  GtkWidget *text_contents;
  GtkWidget *scrolledwindow_wizard;
  GtkWidget *text_wizard;
  GtkWidget *scrolledwindow_macros;
  GtkWidget *text_macros;
  GtkWidget *scrolledwindow_alias;
  GtkWidget *text_alias;
  GtkWidget *scrolledwindow_triggers;
  GtkWidget *text_triggers;
  GtkWidget *scrolledwindow_logger;
  GtkWidget *text_logger;
  GtkWidget *scrolledwindow_logviewer;
  GtkWidget *text_logviewer;
  GtkWidget *scrolledwindow_font;
  GtkWidget *text_font;
  GtkWidget *scrolledwindow_color;
  GtkWidget *text_color;
  GtkWidget *label_contents;
  GtkWidget *label_wizard;
  GtkWidget *label_Macros;
  GtkWidget *label_alias;
  GtkWidget *label_triggers;
  GtkWidget *label_logger;
  GtkWidget *label_logviewer;
  GtkWidget *label_font;
  GtkWidget *label_color;
  GtkWidget *hbuttonbox;
  GtkWidget *done_button;

  man_window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_object_set_data (GTK_OBJECT (man_window), "man_window", man_window);
  gtk_widget_set_usize (man_window, 550, 550);
  gtk_window_set_title (GTK_WINDOW (man_window), "GGMud "VERSION" Manual");
  gtk_window_set_policy (GTK_WINDOW (man_window), FALSE, FALSE, FALSE);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_object_set_data (GTK_OBJECT (man_window), "vbox", vbox);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (man_window), vbox);

  notebook = gtk_notebook_new ();
  gtk_object_set_data (GTK_OBJECT (man_window), "notebook", notebook);
  gtk_widget_show (notebook);
  gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);

  scrolledwindow_contents = gtk_scrolled_window_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "scrolledwindow_contents", scrolledwindow_contents);
  gtk_widget_show (scrolledwindow_contents);
  gtk_container_add (GTK_CONTAINER (notebook), scrolledwindow_contents);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_contents), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_contents = gtk_text_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "text_contents", text_contents);
  gtk_widget_show (text_contents);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_contents), text_contents);
  gtk_widget_realize (text_contents);
  gtk_text_insert(GTK_TEXT(text_contents), NULL, NULL, NULL, CONTENTS_MESSAGE,-1);

  scrolledwindow_wizard = gtk_scrolled_window_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "scrolledwindow_wizard", scrolledwindow_wizard);
  gtk_widget_show (scrolledwindow_wizard);
  gtk_container_add (GTK_CONTAINER (notebook), scrolledwindow_wizard);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_wizard), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_wizard = gtk_text_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "text_wizard", text_wizard);
  gtk_widget_show (text_wizard);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_wizard), text_wizard);
  gtk_text_insert(GTK_TEXT(text_wizard), NULL, NULL, NULL, WIZARD_MESSAGE,-1);

  scrolledwindow_macros = gtk_scrolled_window_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "scrolledwindow_macros", scrolledwindow_macros);
  gtk_widget_show (scrolledwindow_macros);
  gtk_container_add (GTK_CONTAINER (notebook), scrolledwindow_macros);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_macros), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_macros = gtk_text_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "text_macros", text_macros);
  gtk_widget_show (text_macros);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_macros), text_macros);
  gtk_text_insert(GTK_TEXT(text_macros), NULL, NULL, NULL, MACROS_MESSAGE,-1);

  scrolledwindow_alias = gtk_scrolled_window_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "scrolledwindow_alias", scrolledwindow_alias);
  gtk_widget_show (scrolledwindow_alias);
  gtk_container_add (GTK_CONTAINER (notebook), scrolledwindow_alias);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_alias), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_alias = gtk_text_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "text_alias", text_alias);
  gtk_widget_show (text_alias);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_alias), text_alias);
  gtk_text_insert(GTK_TEXT(text_alias), NULL, NULL, NULL, ALIAS_MESSAGE,-1);

  scrolledwindow_triggers = gtk_scrolled_window_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "scrolledwindow_triggers", scrolledwindow_triggers);
  gtk_widget_show (scrolledwindow_triggers);
  gtk_container_add (GTK_CONTAINER (notebook), scrolledwindow_triggers);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_triggers), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_triggers = gtk_text_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "text_triggers", text_triggers);
  gtk_widget_show (text_triggers);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_triggers), text_triggers);
  gtk_text_insert(GTK_TEXT(text_triggers), NULL, NULL, NULL, TRIGGERS_MESSAGE,-1);


  scrolledwindow_logger = gtk_scrolled_window_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "scrolledwindow_logger", scrolledwindow_logger);
  gtk_widget_show (scrolledwindow_logger);
  gtk_container_add (GTK_CONTAINER (notebook), scrolledwindow_logger);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_logger), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_logger = gtk_text_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "text_logger", text_logger);
  gtk_widget_show (text_logger);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_logger), text_logger);
  gtk_text_insert(GTK_TEXT(text_logger), NULL, NULL, NULL, LOGGER_MESSAGE,-1);

  scrolledwindow_logviewer = gtk_scrolled_window_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "scrolledwindow_logviewer", scrolledwindow_logviewer);
  gtk_widget_show (scrolledwindow_logviewer);
  gtk_container_add (GTK_CONTAINER (notebook), scrolledwindow_logviewer);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_logviewer), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_logviewer = gtk_text_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "text_logviewer", text_logviewer);
  gtk_widget_show (text_logviewer);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_logviewer), text_logviewer);
  gtk_text_insert(GTK_TEXT(text_logviewer), NULL, NULL, NULL, LOGVIEWER_MESSAGE,-1);

  scrolledwindow_font = gtk_scrolled_window_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "scrolledwindow_font", scrolledwindow_font);
  gtk_widget_show (scrolledwindow_font);
  gtk_container_add (GTK_CONTAINER (notebook), scrolledwindow_font);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_font), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_font = gtk_text_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "text_font", text_font);
  gtk_widget_show (text_font);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_font), text_font);
  gtk_text_insert(GTK_TEXT(text_font), NULL, NULL, NULL, FONT_MESSAGE,-1);

  scrolledwindow_color = gtk_scrolled_window_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "scrolledwindow_color", scrolledwindow_color);
  gtk_widget_show (scrolledwindow_color);
  gtk_container_add (GTK_CONTAINER (notebook), scrolledwindow_color);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_color), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_color = gtk_text_new (NULL, NULL);
  gtk_object_set_data (GTK_OBJECT (man_window), "text_color", text_color);
  gtk_widget_show (text_color);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_color), text_color);
  gtk_text_insert(GTK_TEXT(text_color), NULL, NULL, NULL, COLOR_MESSAGE,-1);

  label_contents = gtk_label_new ("Contents");
  gtk_object_set_data (GTK_OBJECT (man_window), "label_contents", label_contents);
  gtk_widget_show (label_contents);
  set_notebook_tab (notebook, 0, label_contents);

  label_wizard = gtk_label_new ("Connection Wizard");
  gtk_object_set_data (GTK_OBJECT (man_window), "label_wizard", label_wizard);
  gtk_widget_show (label_wizard);
  set_notebook_tab (notebook, 1, label_wizard);

  label_Macros = gtk_label_new ("Macros");
  gtk_object_set_data (GTK_OBJECT (man_window), "label_Macros", label_Macros);
  gtk_widget_show (label_Macros);
  set_notebook_tab (notebook, 2, label_Macros);

  label_alias = gtk_label_new ("Aliases");
  gtk_object_set_data (GTK_OBJECT (man_window), "label_alias", label_alias);
  gtk_widget_show (label_alias);
  set_notebook_tab (notebook, 3, label_alias);

  label_triggers = gtk_label_new ("Triggers");
  gtk_object_set_data (GTK_OBJECT (man_window), "label_triggers", label_triggers);
  gtk_widget_show (label_triggers);
  set_notebook_tab (notebook, 4, label_triggers);

  label_logger = gtk_label_new ("Logger");
  gtk_object_set_data (GTK_OBJECT (man_window), "label_logger", label_logger);
  gtk_widget_show (label_logger);
  set_notebook_tab (notebook, 5, label_logger);

  label_logviewer = gtk_label_new ("LogViewer");
  gtk_object_set_data (GTK_OBJECT (man_window), "label_logviewer", label_logviewer);
  gtk_widget_show (label_logviewer);
  set_notebook_tab (notebook, 6, label_logviewer);

  label_font = gtk_label_new ("Fonts");
  gtk_object_set_data (GTK_OBJECT (man_window), "label_font", label_font);
  gtk_widget_show (label_font);
  set_notebook_tab (notebook, 7, label_font);

  label_color = gtk_label_new ("Colors");
  gtk_object_set_data (GTK_OBJECT (man_window), "label_color", label_color);
  gtk_widget_show (label_color);
  set_notebook_tab (notebook, 8, label_color);

  hbuttonbox = gtk_hbutton_box_new ();
  gtk_object_set_data (GTK_OBJECT (man_window), "hbuttonbox", hbuttonbox);
  gtk_widget_show (hbuttonbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (hbuttonbox), 3);

  done_button = gtk_button_new_with_label ("Done");
  gtk_object_set_data (GTK_OBJECT (man_window), "done_button", done_button);
  gtk_signal_connect (GTK_OBJECT (done_button), "clicked",
                      GTK_SIGNAL_FUNC (close_window), man_window);
  gtk_widget_show (done_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), done_button);

  gtk_widget_show (man_window);
}
