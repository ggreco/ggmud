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

#include "ggmud.h"
#include "interface.h"
#include "support.h"

/* Wizard txt */
const char * WIZARD_MESSAGE = 
"The connection wizard let you save a list of the mud and the characters you have\n"
"and let you login automatically on a particular mud with a particular character.\n";

/* Macro txt */
const char * MACROS_MESSAGE = 
"Actually macros are shortcuts assigned to keys from F1 to F12, you can call them\n"
"with a single keypress and this will not affect the contents of your input line.\n";


/* Logger txt */
const char * LOGGER_MESSAGE = 
"You have three ways of logging text in GGMud:\n\n"
"- The input line command:\n"
"  #log <filename>\n"
"  ...see more info with #help log, it logs text with full ANSI codes.\n\n"
"- The menu option Tools -> Logger.\n"
"  This one strips ANSI codes from the text and let you choose\n"
"  your output file in a file browser.\n\n"
"- Configuring a HIGH number of lines in the review buffer and then saving\n"
"  them with the menu option Options -> Save review.\n"
"  This is the suggested method if you have a lot of memory since you don't\n"
"  have to specify a filename in advance.\n";

/* Logviewer txt */
const char * LOGVIEWER_MESSAGE =
"This tool is useful to examine log files without using an external editor.\n";

/* Font txt */
const char * FONT_MESSAGE = 
"You can select any font on your system, if you don't see the font you need in\n"
"the font selection widget, maybe you need to modify your /etc/fonts/fonts.conf\n"
"to be sure the path where the font is located it's listed.\n\n"
"Use monospace fonts for better results.\n\n"
"To change font use the Options -> Font menu (shortcut ALT+F).\n";

/* Color txt */
const char * COLOR_MESSAGE = 
"GGMud by default uses the 16 colors ansi mode, it supports blinking text too.\n\n"
"You can change any color with the Options -> Colors menu (shortcut ALT+C). \n"
"To disable blinking if you don't like it or if it slow down the output untick\n"
"Options -> Preferences -> Blinking.";

void do_about (GtkWidget *widget, gpointer data)
{
  GtkWidget *about_window = create_window_about();

  gtk_widget_set_usize (about_window, 400, 375);
  gtk_window_set_title (GTK_WINDOW (about_window), "About GGMud " VERSION " (" __DATE__ ")");

  gtk_widget_show(about_window);
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
  GtkWidget *hbuttonbox;
  GtkWidget *done_button;
  GtkTextView *oldtext = mud->text;
  GtkTextTag *oldcolor = prefs.DefaultColor;
  extern GtkTextTagTable *tag_table;
 
  prefs.DefaultColor = gtk_text_tag_new(NULL);
  g_object_set(prefs.DefaultColor, "foreground-gdk", &color_black, NULL);
  gtk_text_tag_table_add(tag_table, prefs.DefaultColor);
 
  man_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_usize (man_window, 550, 550);
  gtk_window_set_title (GTK_WINDOW (man_window), "GGMud "VERSION" Manual");
  gtk_window_set_policy (GTK_WINDOW (man_window), FALSE, FALSE, FALSE);

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (man_window), vbox);

  notebook = gtk_notebook_new ();
  gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);

  scrolledwindow_contents = gtk_scrolled_window_new (NULL, NULL);
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), scrolledwindow_contents, gtk_label_new ("Contents"));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_contents), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_contents = gtk_text_view_new_with_buffer (gtk_text_buffer_new(tag_table));
  gtk_object_set_user_data(GTK_OBJECT(text_contents), NULL), 
  gtk_widget_show (text_contents);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_contents), text_contents);
  gtk_widget_realize (text_contents);

  mud->text = GTK_TEXT_VIEW(text_contents);
  parse_input("#help", NULL);

  scrolledwindow_wizard = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_wizard);
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), scrolledwindow_wizard, gtk_label_new ("Connection Wizard"));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_wizard), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_wizard = gtk_text_view_new ();
  gtk_object_set_user_data(GTK_OBJECT(text_wizard), NULL), 
  gtk_widget_show (text_wizard);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_wizard), text_wizard);
  text_insert(text_wizard, WIZARD_MESSAGE);

  scrolledwindow_macros = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_macros);
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), scrolledwindow_macros, gtk_label_new ("Macros"));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_macros), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_macros = gtk_text_view_new ();
  gtk_object_set_user_data(GTK_OBJECT(text_macros), NULL), 
  gtk_widget_show (text_macros);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_macros), text_macros);
  text_insert(text_macros, MACROS_MESSAGE);

  scrolledwindow_alias = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_alias);
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), scrolledwindow_alias, gtk_label_new ("Aliases"));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_alias), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_alias = gtk_text_view_new_with_buffer (gtk_text_buffer_new(tag_table));
  gtk_object_set_user_data(GTK_OBJECT(text_alias), NULL), 

  gtk_widget_show (text_alias);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_alias), text_alias);

  mud->text = GTK_TEXT_VIEW(text_alias);
  parse_input("#help alias", NULL);

  scrolledwindow_triggers = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_triggers);
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), scrolledwindow_triggers, gtk_label_new ("Triggers"));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_triggers), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_triggers = gtk_text_view_new_with_buffer (gtk_text_buffer_new(tag_table));
  gtk_object_set_user_data(GTK_OBJECT(text_triggers), NULL), 
  gtk_widget_show (text_triggers);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_triggers), text_triggers);

  mud->text = GTK_TEXT_VIEW(text_triggers);
  parse_input("#help action", NULL);

  scrolledwindow_logger = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_logger);
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), scrolledwindow_logger, gtk_label_new ("Logger"));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_logger), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_logger = gtk_text_view_new ();
  gtk_object_set_user_data(GTK_OBJECT(text_logger), NULL), 
  gtk_widget_show (text_logger);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_logger), text_logger);
  text_insert(text_logger, LOGGER_MESSAGE);

  scrolledwindow_logviewer = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_logviewer);
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), scrolledwindow_logviewer, gtk_label_new ("Log Viewer"));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_logviewer), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_logviewer = gtk_text_view_new ();
  gtk_object_set_user_data(GTK_OBJECT(text_logviewer), NULL), 
  gtk_widget_show (text_logviewer);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_logviewer), text_logviewer);
  text_insert(text_logviewer, LOGVIEWER_MESSAGE);

  scrolledwindow_font = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_font);
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), scrolledwindow_font, gtk_label_new ("Fonts"));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_font), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_font = gtk_text_view_new ();
  gtk_object_set_user_data(GTK_OBJECT(text_font), NULL), 
  gtk_widget_show (text_font);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_font), text_font);
  text_insert(text_font, FONT_MESSAGE);

  scrolledwindow_color = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_color);
  gtk_notebook_append_page(GTK_NOTEBOOK (notebook), scrolledwindow_color, gtk_label_new ("Colors"));
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_color), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_color = gtk_text_view_new ();
  gtk_object_set_user_data(GTK_OBJECT(text_color), NULL), 
  gtk_widget_show (text_color);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_color), text_color);
  text_insert(text_color, COLOR_MESSAGE);

  gtk_widget_show_all (notebook);

  hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (hbuttonbox), 3);

  done_button = gtk_button_new_with_label ("Done");
  gtk_signal_connect (GTK_OBJECT (done_button), "clicked",
                      GTK_SIGNAL_FUNC (close_window), man_window);
  gtk_widget_show (done_button);
  gtk_container_add (GTK_CONTAINER (hbuttonbox), done_button);

  gtk_widget_show (man_window);
  gtk_text_tag_table_remove(tag_table, prefs.DefaultColor);
  g_object_unref(prefs.DefaultColor);
  mud->text = oldtext;
  prefs.DefaultColor = oldcolor;
}
