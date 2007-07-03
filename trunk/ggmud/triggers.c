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

/* Most of the functions in this file is almost identical to the
 * functions in wizard.c and alias.c as they all use the CList
 * widgets.
 */

#include "ggmud.h"
#include <string.h>
#include "interface.h"
#include "support.h"

#define TRIGGER_FILE "triggers"

static GtkWidget *trig_window = NULL;
static GtkWidget *class_window = NULL;
static gint      trigger_selected_row    = -1;

#define ALIAS_LEN 128
#define REPL_LEN 512
#define CLASS_LEN 16

/*
 * Save triggers to file
 */
void save_triggers (GtkWidget *w, void *userdata) 
{
    FILE *fp;
    GtkCList *data = GTK_CLIST(lookup_widget(w, "clist_trig"));

    if ((fp = fileopen (TRIGGER_FILE, "w"))) {
        int done = FALSE;
        gint  row = 0;
        gchar *alias, *replace, *class;
        
        while ( !done && data) {
            if ( !gtk_clist_get_text (data, row, 0, &alias)
                    || !gtk_clist_get_text (data, row, 1, &replace) 
                    || !gtk_clist_get_text (data, row, 2, &class))
                break;

            if (!alias[0]) {
                done = TRUE;
                break;
            }

            // don't save scripting triggers
            if (strcmp(class, "scripting"))
                fprintf (fp, "#action {%s} {%s} {%s}\n", alias, replace, class);

            row++;      
        }
        fclose (fp);
    }	
}

/*
 * Load triggers from file at startup
 */
void load_triggers() 
{
    FILE *fp;

    if ((fp = fileopen(TRIGGER_FILE, "r"))) {
        parse_config(fp, NULL);   
        fclose(fp);
    }
}

GdkPixmap *enabled_pixmap = NULL, *disabled_pixmap = NULL;
GdkBitmap *enabled_mask = NULL, *disabled_mask = NULL;


#include "include/llist.h"

static void  insert_triggers  (GtkCList *clist)
{
    extern struct listnode *common_actions;
    gchar *text[4];
    struct listnode *list = mud->activesession ? mud->activesession->actions : common_actions;
    int row;
    
    gtk_clist_clear (clist);

    gtk_clist_freeze(clist);
    
    while ( (list = list->next) ) {
        // doesn't show scripting triggers
        if (!strcmp(list->pr, "scripting"))
            continue;
        
        text[0] = list->left;
        text[1] = list->right;
        text[2] = list->pr;
        text[3] = "";
        row = gtk_clist_append (clist, text);

        if (list->enabled)
            gtk_clist_set_pixmap(clist, row, 3, enabled_pixmap, enabled_mask);
        else
            gtk_clist_set_pixmap(clist, row, 3, disabled_pixmap, disabled_mask);

        gtk_clist_set_row_data(clist, row, list);
    }

    gtk_clist_thaw(clist);
}

void save_triggerclass_state(void)
{
}

extern trigger_class *trigger_classes;

static void insert_trigger_classes(GtkCList *clist)
{
    const char *text[2];
    trigger_class *cl = trigger_classes;
    int row;
   
    gtk_clist_clear (clist);

    gtk_clist_freeze(clist);

    while (cl) {
        text[0] = cl->name;
        text[1] = "";
        row = gtk_clist_append(clist, (gchar **)text);

        if (cl->enabled)
            gtk_clist_set_pixmap(clist, row, 1, enabled_pixmap, enabled_mask);
        else
            gtk_clist_set_pixmap(clist, row, 1, disabled_pixmap, disabled_mask);
            
        cl = cl->next;
    }

    gtk_clist_thaw(clist);
}

void trigger_class_toggle (GtkCList *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    GdkPixmap *pixmap;
    GdkBitmap *mask;
    gchar *text;
    char buffer[1024];
    
    gtk_clist_get_text (clist, row, 0, &text);
    gtk_clist_get_pixmap(clist, row, 1, &pixmap, &mask);
            
    if (pixmap == disabled_pixmap)
        sprintf(buffer, "#action +%s", text);
    else
        sprintf(buffer, "#action -%s", text);

    parse_input(buffer, NULL);

    insert_trigger_classes(clist);

    if (trig_window)
        insert_triggers(GTK_CLIST(
                    lookup_widget(trig_window, "clist_trig")));
}

void trigger_selection_made (GtkCList *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    static guint32 old_time = 0L;
    gchar *text;

    if (old_time != 0L && (event->time - old_time) < 1000 &&
            trigger_selected_row == row) {
        struct listnode *entry = (struct listnode *)
            gtk_clist_get_row_data(clist, row);

        entry->enabled ^= 1;

        gtk_clist_set_pixmap(clist, row, 3, 
                entry->enabled ? enabled_pixmap : disabled_pixmap, 
                entry->enabled ? enabled_mask   : disabled_mask);

        old_time = 0L; // to avoid triple click;
    }
    else
        old_time = event->time;
    trigger_selected_row    = row;

    gtk_clist_get_text (clist, row, 0, &text);
    gtk_entry_set_text (GTK_ENTRY (
                lookup_widget((GtkWidget *)clist, "entry_trig")), text);
    gtk_clist_get_text (clist, row, 1, &text);
    gtk_entry_set_text (GTK_ENTRY (
                lookup_widget((GtkWidget *)clist, "entry_cmd")), text);
    gtk_clist_get_text (clist, row, 2, &text);
    gtk_entry_set_text (GTK_ENTRY (GTK_BIN (lookup_widget((GtkWidget *)clist,
                        "comboboxentry_class"))->child), text);
}

void triggerclass_window(GtkWidget *widget, gpointer data )
{
    GtkWidget *clist;

    if (class_window) {
        gtk_window_present(GTK_WINDOW(class_window));
        return;
    }
    
    class_window = create_window_classes ();
    gtk_signal_connect (GTK_OBJECT (class_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &class_window );
    

    clist = lookup_widget(class_window, "clist_class");
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 250);
    gtk_clist_set_column_width (GTK_CLIST (clist), 1, 50);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 0, GTK_JUSTIFY_LEFT);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 1, GTK_JUSTIFY_LEFT);
    gtk_clist_set_row_height (GTK_CLIST (clist), 20);

    insert_trigger_classes  (GTK_CLIST(clist)  );
    gtk_widget_show (class_window );
}

void
update_classes(GtkWidget *cb)
{
    trigger_class *cl = trigger_classes;
    GtkTreeModel *m = gtk_combo_box_get_model(GTK_COMBO_BOX(cb));
    GtkTreeIter it;

    if (!m)
        m = (GtkTreeModel *) gtk_list_store_new(1, G_TYPE_STRING);

    if (class_window)
        insert_trigger_classes(GTK_CLIST(
                    lookup_widget(class_window, "clist_class")));

    gtk_list_store_clear(GTK_LIST_STORE(m));

    while (cl) {
        if (strcmp(cl->name, "scripting")) {
            gtk_list_store_append(GTK_LIST_STORE(m), &it);
            gtk_list_store_set(GTK_LIST_STORE(m), &it, 0, (void *)cl->name, -1);
        }

        cl = cl->next;
    }
}

static void add_trigger(const char *a, const char *b, const char *class)
{
    char buffer[1024];

    sprintf(buffer, "#action {%s} {%s} {%s}", a, b, class);

    parse_input(buffer, NULL);

    if (trig_window)
        update_classes(lookup_widget(trig_window, "comboboxentry_class"));
}

void trigger_button_add (GtkWidget *button, void *data)
{
    const gchar *text[3];

    text[0]   = gtk_entry_get_text (GTK_ENTRY (lookup_widget(button,
                    "entry_trig")));
    text[1]   = gtk_entry_get_text (GTK_ENTRY (lookup_widget(button,
                    "entry_cmd")));
    text[2]   = gtk_entry_get_text (GTK_ENTRY (GTK_BIN (lookup_widget(button,
                    "comboboxentry_class"))->child));
    
    if ( text[0][0] == '\0' || text[1][0] == '\0' ) {
        popup_window (INFO, "Please complete the trigger first.");
        return;
    }

    if ( strlen (text[0]) > ALIAS_LEN)  {
        popup_window (ERR, "Trigger string too big.");
        return;
    }
    
    if ( strlen (text[1]) > REPL_LEN)  {
        popup_window (ERR, "Command string too big.");
        return;
    }

    if (!strcmp(text[2], "scripting")) {
        popup_window(ERR, "You cannot define SCRIPTING triggers at hand,\nyou should use LUA!");
        return;
    }
    
    if ( strlen (text[2]) > CLASS_LEN)  {
        popup_window (ERR, "Class string too big.");
        return;
    }

    add_trigger (text[0], text[1], text[2]);

    insert_triggers(GTK_CLIST(lookup_widget(button, "clist_trig")));
}

void trigger_button_delete (GtkWidget *button, gpointer data) {
    gchar *word;
    
    if ( trigger_selected_row == -1 ) {
        popup_window (WARN, "No selection made.");
    }
    else {
        GtkCList *l = GTK_CLIST(lookup_widget(button, "clist_trig"));
        char buffer[ALIAS_LEN + 20];
        
        gtk_clist_get_text (l, trigger_selected_row, 0, &word);
        sprintf(buffer, "#unaction {%s}", word);
        gtk_clist_remove (l, trigger_selected_row);
        trigger_selected_row = -1;

        parse_input(buffer, NULL);
    }

}

static int
do_find(GtkCList *clist, const char *match)
{
    char *text;
    int i, j;

    for (i = 0; i < clist->rows; i++) {
        for (j = 0; j < clist->columns; j++) {

            if(gtk_clist_get_text (clist, i, j, &text)) {
                if (strstr(text, match))
                    return i;
            }
        }
    }

    return -1;
}

static void 
emit_response(GtkEntry *entry, GtkDialog *d)
{
    const char *text = gtk_entry_get_text(entry);
    
    if (text && *text)
        gtk_dialog_response(d, GTK_RESPONSE_ACCEPT);
}

void 
find_in_list(GtkWidget *widget, GtkCList *clist)
{
     GtkWidget *dialog = gtk_dialog_new_with_buttons ("Find...",
                                                  GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                  GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                  GTK_STOCK_OK,
                                                  GTK_RESPONSE_ACCEPT,
                                                  GTK_STOCK_CANCEL,
                                                  GTK_RESPONSE_REJECT,
                                                  NULL);
    GtkWidget *label = gtk_label_new("Insert the string you want to search for:");
    GtkWidget *entry = gtk_entry_new();
    
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                      label);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                      entry);

    gtk_signal_connect (GTK_OBJECT (entry), "activate", 
            GTK_SIGNAL_FUNC (emit_response), dialog);
    
    gtk_widget_show_all (dialog);
    
    if (gtk_dialog_run((GtkDialog *)dialog) == GTK_RESPONSE_ACCEPT) {
        const char *finding =  gtk_entry_get_text(GTK_ENTRY(entry));

        if (finding && *finding) {
            int row = do_find(clist, finding);

            if (row < 0)
                popup_window(ERR, "Unable to find '%s' in this list.", finding);
            else {
                // I hope this will trigger the cbk.
                gtk_clist_select_row(clist, row, 0);
            }
        }
    }

    gtk_widget_destroy(dialog);
}

void triggers_window(GtkWidget *widget, gpointer data)
{
    GtkWidget *clist;

    if (trig_window) {
        gtk_window_present(GTK_WINDOW(trig_window));
        return;
    }

    trig_window = create_window_triggers();
    gtk_signal_connect (GTK_OBJECT (trig_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &trig_window );


    clist = lookup_widget(trig_window, "clist_trig");
    gtk_clist_column_titles_passive (GTK_CLIST (clist));
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 200);
    gtk_clist_set_column_width (GTK_CLIST (clist), 1, 360);
    gtk_clist_set_column_width (GTK_CLIST (clist), 2, 80);
    gtk_clist_set_column_width (GTK_CLIST (clist), 3, 40);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 0, GTK_JUSTIFY_LEFT);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 1, GTK_JUSTIFY_LEFT);

    update_classes(
            lookup_widget(trig_window, "comboboxentry_class"));
    
    insert_triggers  (GTK_CLIST(clist)  );
    gtk_widget_show(trig_window );

}
