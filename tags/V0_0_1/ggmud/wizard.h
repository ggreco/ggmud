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

#ifndef _WIZARD_H_
#define _WIZARD_H_
#include "ggmud.h"

/* Funktion prototypes */
void  wiz_destructify ();
void  free_wizard_data (WIZARD_DATA *w);
void  load_wizard        (void);
void  save_wizard ();
WIZARD_DATA *wizard_get_wizard_data (gchar *text);
void  wizard_clist_append (WIZARD_DATA *w, GtkCList *clist);
void  wizard_selection_made (GtkWidget *clist, gint row, gint column,
                             GdkEventButton *event, gpointer data);
void  wizard_unselection_made (GtkWidget *clist, gint row, gint column,
                               GdkEventButton *event, gpointer data);
void  wizard_button_connect (GtkWidget *button, gpointer data);
void  wizard_button_delete (GtkWidget *button, gpointer data);
void  wizard_button_modify (GtkWidget *button, gpointer data);
void  wizard_button_add (GtkWidget *button, gpointer data);
void  wizard_check_callback (GtkWidget *widget, GtkWidget *check_button);
void  do_wiz (GtkWidget *widget, gpointer data);

/* Global Variables */
gint         wizard_selected_row;
GtkWidget   *wizard_entry_name;
GtkWidget   *wizard_entry_host;
GtkWidget   *wizard_entry_port;
GtkWidget   *wizard_check_autologin;
GtkWidget   *wizard_entry_player;
GtkWidget   *wizard_entry_password;
GtkWidget   *wizard_window;
GtkWidget   *button_update;
GtkWidget   *button_delete;
GtkWidget   *button_connect;
static GList       *wizard_connection_list2;

#endif /* _WIZARD_H_ */
