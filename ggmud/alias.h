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

#ifndef _ALIAS_H_
#define _ALIAS_H_
#include "ggmud.h"

/* Funktion prototypes */
void alias_button_add     (GtkWidget *button, gpointer data);
void alias_button_delete  (GtkWidget *button, gpointer data);
void alias_selection_made (GtkWidget *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data);
void load_aliases         (void );
void save_aliases         (GtkWidget *button, gpointer data );
void add_alias            (gchar *alias, gchar *replacement );
void insert_aliases       (GtkWidget *clist );
void window_alias         (GtkWidget *widget, gpointer data);

/* Global variables for alias */
GtkWidget *textalias;
GtkWidget *textreplace;
GtkWidget *alias_window;
gint      alias_selected_row    = -1;
gint      alias_selected_column = -1;

ALIAS_DATA *alias_list;

#define ALIAS_LEN	20
#define REPL_LEN	1000

#endif /* _ALIAS_H_ */
