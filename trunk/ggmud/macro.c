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
#include "ggmud.h"
#include "support.h"
#include "interface.h"

#define MACRO_FILE "macro"
GList *macro_list = NULL;
gint capture_enabled = FALSE;

typedef struct
{
    char *key;
    char *value;
}
ggmud_macro;

// macro editor

gchar *internal_key_to_string(gint state, gint key)
{
  gchar *buff = g_malloc(125);
  *buff = 0;

  if (state & GDK_CONTROL_MASK)
    strcat(buff, "Ctrl+");
  if (state & GDK_MOD1_MASK)
    strcat(buff, "Alt+");
  strcat(buff, gdk_keyval_name(key));
  return buff;
}

gint check_macro(gint state, gint key)
{
    gchar *test = internal_key_to_string(state, key);
    GList *l = macro_list;
    gint rc = FALSE;

    while (l) {
        ggmud_macro *m = (ggmud_macro *)l->data;

        if (!strcmp(m->key, test)) {
            parse_input(m->value, mud->activesession);

            rc = TRUE;
        }

        l = l -> next;
    }

    g_free(test);

    return rc;
}

void
del_macro(ggmud_macro *m)
{
    if (g_list_find(macro_list, m)) {
        free(m->key);
        free(m->value);
        free(m);
        macro_list = g_list_remove(macro_list, m);
    }
}

void 
add_macro(const char *macro, const char *value)
{
    ggmud_macro *m = malloc(sizeof(ggmud_macro));

    m->key = strdup(macro); 
    m->value = strdup(value);

    macro_list = g_list_append(macro_list, m);
}

/* saves the macro's to file */
void save_macro () 
{
    FILE *fp;
    GList *l = macro_list;

    if ((fp = fileopen (MACRO_FILE, "w"))) {
        while (l) {
            fprintf (fp, "%s %s\n", 
                    ((ggmud_macro *)l->data)->key,
                    ((ggmud_macro *)l->data)->value);
            l = l -> next;
        }    
        fclose (fp);
    }
}

void
on_button_capture_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *entry = lookup_widget((GtkWidget *)button, "entry_shortcut");

  gtk_entry_set_text(GTK_ENTRY(entry), "");
  GTK_WIDGET_SET_FLAGS(entry, GTK_CAN_FOCUS);
  gtk_widget_grab_focus(GTK_WIDGET(entry));

  capture_enabled = TRUE;
}


void
on_macro_save_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
    save_macro();

    gtk_widget_destroy(gtk_widget_get_toplevel((GtkWidget *)button));
}


void
on_clist_macros_select_row             (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    ggmud_macro *m = 
        (ggmud_macro *)gtk_clist_get_row_data(clist, row);

    gtk_entry_set_text(GTK_ENTRY(
                lookup_widget((GtkWidget *)clist, "entry_shortcut")), m->key);

    gtk_entry_set_text(GTK_ENTRY(
                lookup_widget((GtkWidget *)clist, "entry_cmd")), m->value);

    gtk_widget_set_sensitive(
                lookup_widget((GtkWidget *)clist, "button_mod"), TRUE);

}

void
refresh_macro_list(GtkCList *clist)
{
    GList *l = macro_list;

    gtk_clist_freeze(clist);

    gtk_clist_clear(clist);

    while (l) {
        char *texts[2];

        texts[0] = ((ggmud_macro *)l->data)->key;
        texts[1] = ((ggmud_macro *)l->data)->value;

        gtk_clist_prepend(clist, texts);

        gtk_clist_set_row_data(clist, 0, l->data);

        l = l->next;
    }

    gtk_clist_thaw(clist);
}

void
on_clist_macros_unselect_row           (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    gtk_entry_set_text(GTK_ENTRY(
                lookup_widget((GtkWidget *)clist, "entry_shortcut")), "");

    gtk_entry_set_text(GTK_ENTRY(
                lookup_widget((GtkWidget *)clist, "entry_cmd")), "");
    
    gtk_widget_set_sensitive(
                lookup_widget((GtkWidget *)clist, "button_mod"), FALSE);
}



void
on_me_button_add_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    const char *key, *cmd;

    GtkCList *list = GTK_CLIST(lookup_widget(
                (GtkWidget *)button, "clist_macros"));

    key = gtk_entry_get_text(GTK_ENTRY(
                lookup_widget((GtkWidget *)button, "entry_shortcut")));

    cmd = gtk_entry_get_text(GTK_ENTRY(
                lookup_widget((GtkWidget *)button, "entry_cmd")));

    if (!*cmd || !*key) {
        popup_window(INFO, "Shortcut and Commands must not be empty!");
        return;
    }

    add_macro(key, cmd);

    refresh_macro_list(list);

    gtk_widget_set_sensitive(
                lookup_widget((GtkWidget *)list, "button_mod"), FALSE);
}


void
on_me_button_mod_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    const char *key, *cmd;
    GtkCList *list = GTK_CLIST(lookup_widget(
                (GtkWidget *)button, "clist_macros"));

    if (!list->selection) {
        popup_window(INFO, "You should select a macro to be modified first...");
        return;
    }

    key = gtk_entry_get_text(GTK_ENTRY(
                lookup_widget((GtkWidget *)button, "entry_shortcut")));

    cmd = gtk_entry_get_text(GTK_ENTRY(
                lookup_widget((GtkWidget *)button, "entry_cmd")));

    if (!*cmd || !*key) {
        popup_window(INFO, "Shortcut and Commands must not be empty!");
        return;
    }

    ggmud_macro *m = 
        (ggmud_macro *)gtk_clist_get_row_data(list, (gint)list->selection->data);

    free(m->key); free(m->value);

    m->key = strdup(key); m->value = strdup(cmd);

    refresh_macro_list(list);

    gtk_widget_set_sensitive(
                lookup_widget((GtkWidget *)list, "button_mod"), FALSE);
}


void
on_me_button_del_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkCList *list = GTK_CLIST(lookup_widget(
                (GtkWidget *)button, "clist_macros"));

    if (!list->selection) {
        popup_window(INFO, "You should select a macro to be deleted first...");
        return;
    }

    ggmud_macro *m = 
        (ggmud_macro *)gtk_clist_get_row_data(list, (gint)list->selection->data);

    del_macro(m);

    refresh_macro_list(list);

    gtk_widget_set_sensitive(
                lookup_widget((GtkWidget *)list, "button_mod"), FALSE);
}

gboolean
on_entry_shortcut_key_press_event      (GtkWidget       *widget,
                                        GdkEventKey     *event,
                                        gpointer         user_data)
{
  gboolean done = FALSE;

  gint state = event->state;
  gint key = gdk_keyval_to_upper(event->keyval);

  if ((state & (GDK_CONTROL_MASK | GDK_MOD1_MASK)) != 0) {
    if (key < 65500) {
      gchar *buff = internal_key_to_string(state, key);
      gtk_entry_set_text(GTK_ENTRY(widget), buff);
      g_free(buff);
      done = TRUE;
    }
  } else {
    if ((key > 255) && (key < 65500)) {
      gtk_entry_append_text(GTK_ENTRY(widget),
                gdk_keyval_name(key));
      done = TRUE;
    }
  }
  if (done) {
    GTK_WIDGET_UNSET_FLAGS(widget, GTK_CAN_FOCUS);
    gtk_widget_grab_focus(
            lookup_widget(widget, "button_capture"));

    capture_enabled = FALSE;
  }
  return FALSE;
}


/* load's the macro's from file into the entry boxes */
void load_macro () 
{
    FILE *fp;
    gchar line[512], macro[100], value[400];

    if ((fp = fileopen (MACRO_FILE, "r"))) {
        /* load in the macro's into the entry[] boxes */
        while (fgets (line, sizeof(line) - 1, fp)) {
            value[0] = 0;
            if (sscanf (line, "%s %[^\n]", macro, value) == 2) {
                if (*value)
                    add_macro(macro, value);
            }
        }    
        fclose (fp);
    }

}

void window_macro (GtkWidget *widget, gpointer data)
{
  GtkWidget *macro_window = create_window_macro_editor();

  refresh_macro_list(GTK_CLIST(lookup_widget(macro_window, "clist_macros")));

  gtk_widget_set_sensitive(
                lookup_widget(macro_window, "button_mod"), FALSE);

  gtk_widget_show(macro_window);
}
