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

#ifndef _TRIGGERS_H_
#define _TRIGGERS_H_

#include "ggmud.h"
#include <sys/types.h>
/* Define syntax for regex, see regex.h for explanation of all the bits */

typedef struct trigger_data TRIGGER_DATA;

struct trigger_data {
    TRIGGER_DATA *next;
    gchar *trigger;
};

struct trig_d {
    gchar *name;
    gchar *tooltip;
};

struct trig_d trigger_desc[] = {
    {"The Trigger", "This is the text that will cause the trigger to be activated.\n"
      "NOTE: The trigger string is CaSe SeNsItIvE!"}
};

/* Function Prototypes */
void  free_trigger_data ( TRIGGER_DATA *w );
TRIGGER_DATA *trigger_get_trigger_data ( gchar *text );
void  trigger_selection_made (GtkWidget *clist, gint row, gint column,
                             GdkEventButton *event, gpointer data);
void  trigger_unselection_made (GtkWidget *clist, gint row, gint column,
                               GdkEventButton *event, gpointer data);
void  trigger_button_add (GtkWidget *button, gpointer data);
void  trigger_button_delete (GtkWidget *button, gpointer data);
void  trigger_button_modify (GtkWidget *button, gpointer data);
void  save_triggers ();
void  load_triggers ();
void  trigger_clist_append (TRIGGER_DATA *w, GtkCList *clist);
void  triggers_window ();
void search_triggers (gchar *incomming);
extern void alt_send_to_connection (gchar *text);
extern gchar *strdelim (gchar *str, const gchar *delim, gchar new_delim);

/* Global Variables */
gint         trigger_selected_row;
static GSList *trigger_list;
GtkWidget    *entry_trigger;
GtkWidget    *entry_send[5];
GtkWidget    *window_triggers;
GtkWidget    *button_apply;
GtkWidget    *button_delete;

#endif /* _TRIGGERS_H_ */
