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

#include "ggmud.h"
#include <string.h>

#define TRIGGER_FILE "triggers"

static GtkWidget *trig_window = NULL;
static GtkWidget *class_window = NULL;
static GtkWidget *textalias;
static GtkWidget *textreplace;
static GtkWidget *textpri;
static gint      trigger_selected_row    = -1;
static GList *classes_store = NULL;

#define ALIAS_LEN 128
#define REPL_LEN 512
#define CLASS_LEN 16

/*
 * Save triggers to file
 */
void save_triggers (GtkWidget *w, GtkCList *data) 
{
    FILE *fp;
    
    if (fp = fileopen (TRIGGER_FILE, "w")) {
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

extern void setup_pixmaps();


#include "include/llist.h"

static void  insert_triggers  (GtkCList *clist)
{
    extern struct listnode *common_actions;
    gchar *text[4];
    struct listnode *list = mud->activesession ? mud->activesession->actions : common_actions;
    int row;
    
    if (!enabled_pixmap)
        setup_pixmaps();

    gtk_clist_clear (clist);

    gtk_clist_freeze(clist);
    
    while ( list = list->next ) {
        text[0] = list->left;
        text[1] = list->right;
        text[2] = list->pr;
        text[3] = "";
        row = gtk_clist_prepend (clist, text);

        if (list->enabled)
            gtk_clist_set_pixmap(clist, row, 3, enabled_pixmap, enabled_mask);
        else
            gtk_clist_set_pixmap(clist, row, 3, disabled_pixmap, disabled_mask);

        gtk_clist_set_row_data(clist, row, list);
    }

    gtk_clist_thaw(clist);
}

static void save_triggerclass_state(void)
{
}

extern trigger_class *trigger_classes;

static void insert_trigger_classes(GtkCList *clist)
{
    const char *text[2];
    trigger_class *cl = trigger_classes;
    int row;
   
    if (!enabled_pixmap)
        setup_pixmaps();

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
static void trigger_class_toggle (GtkCList *clist, gint row, gint column,
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
                    gtk_object_get_user_data(GTK_OBJECT(trig_window))));
}

static void trigger_selection_made (GtkCList *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    static guint32 old_time = 0L;
    gchar *text;
   
    if (old_time != 0L && (event->time - old_time) < 1000 &&
            trigger_selected_row == row) {
        struct listnode *entry = (struct listnode *)
                        gtk_clist_get_row_data(clist, row);

        entry->enabled ^= 1;
        
/*        printf("%s row %d, %s\n", entry->enabled ? "Enabling" : "Disabling",
                row, entry->left);
 */      
        gtk_clist_set_pixmap(clist, row, 3, 
                entry->enabled ? enabled_pixmap : disabled_pixmap, 
                entry->enabled ? enabled_mask   : disabled_mask);
        
        old_time = 0L; // to avoid triple click;
    }
    else
        old_time = event->time;
    trigger_selected_row    = row;

    if ( (GtkCList*) data )
    {
        gtk_clist_get_text (clist, row, 0, &text);
        gtk_entry_set_text (GTK_ENTRY (textalias), text);
        gtk_clist_get_text (clist, row, 1, &text);
        gtk_entry_set_text (GTK_ENTRY (textreplace), text);
        gtk_clist_get_text (clist, row, 2, &text);
        gtk_entry_set_text (GTK_ENTRY (GTK_COMBO(textpri)->entry), text);
    }
    
    return;
}

void triggerclass_window(GtkWidget *widget, gpointer data )
{
    GtkWidget *vbox;
    GtkWidget *clist;
    GtkWidget *label;
//    GtkTooltips *tooltip;
    GtkWidget *scrolled_window;

    gchar     *titles[2] = { "Trigger class", "State"};

//    tooltip = gtk_tooltips_new ();
//    gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);

    if (class_window) {
        gtk_window_present(GTK_WINDOW(class_window));
        return;
    }
    
    class_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (class_window), "Trigger Classes");
    gtk_signal_connect (GTK_OBJECT (class_window), "destroy",
                               GTK_SIGNAL_FUNC(close_window), class_window );
    gtk_signal_connect (GTK_OBJECT (class_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &class_window );
    
    gtk_widget_set_usize (class_window, 400, 350);			       
    vbox = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
    gtk_container_add (GTK_CONTAINER (class_window), vbox);
    gtk_widget_show (vbox);

    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show (scrolled_window);

    clist = gtk_clist_new_with_titles (2, titles);
    gtk_object_set_user_data(GTK_OBJECT(class_window), clist);
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
                               GTK_SIGNAL_FUNC (trigger_class_toggle),
                               (gpointer) clist);
    gtk_clist_column_titles_passive (GTK_CLIST (clist));
    gtk_clist_set_shadow_type (GTK_CLIST (clist), GTK_SHADOW_IN);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 250);
    gtk_clist_set_column_width (GTK_CLIST (clist), 1, 50);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 0, GTK_JUSTIFY_LEFT);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 1, GTK_JUSTIFY_LEFT);

    gtk_clist_column_titles_show (GTK_CLIST (clist));

    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), clist);

    gtk_widget_show (clist);

    label = gtk_label_new(" Click on a row to enable/disable a trigger class it lists. ");
    gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);
    gtk_widget_show (label);


    AddSimpleBar(vbox, (gpointer)clist,
            GTK_SIGNAL_FUNC(save_triggerclass_state),
            GTK_SIGNAL_FUNC(close_window));

    insert_trigger_classes  (GTK_CLIST(clist)  );
    gtk_widget_show (class_window );
}

void
update_classes()
{
    trigger_class *cl = trigger_classes;
    GtkTreeIter it;

    if (class_window)
        insert_trigger_classes(GTK_CLIST(
                    gtk_object_get_user_data(GTK_OBJECT(class_window))));

    if (classes_store) {
        g_list_free(classes_store);
        classes_store = NULL;
    }
    
    while (cl) {
        classes_store = g_list_append(classes_store, (void *)cl->name);
        
        cl = cl->next;
    }

    gtk_combo_set_popdown_strings (GTK_COMBO (textpri), classes_store);
}

static void add_trigger(const char *a, const char *b, const char *class)
{
    char buffer[1024];

    sprintf(buffer, "#action {%s} {%s} {%s}", a, b, class);

    parse_input(buffer, NULL);

    update_classes();
}

static void trigger_button_add (GtkWidget *button, GtkCList *data)
{
    const gchar *text[3];

    text[0]   = gtk_entry_get_text (GTK_ENTRY (textalias  ));
    text[1]   = gtk_entry_get_text (GTK_ENTRY (textreplace));
    text[2]   = gtk_entry_get_text (GTK_ENTRY (GTK_COMBO(textpri)->entry));
    
    if ( text[0][0] == '\0' || text[1][0] == '\0' ) {
        popup_window ("Please complete the trigger first.");
        return;
    }

    if ( strlen (text[0]) > ALIAS_LEN)  {
        popup_window ("Trigger string too big.");
        return;
    }
    
    if ( strlen (text[1]) > REPL_LEN)  {
        popup_window ("Command string too big.");
        return;
    }

    if ( strlen (text[2]) > CLASS_LEN)  {
        popup_window ("Class string too big.");
        return;
    }

    add_trigger (text[0], text[1], text[2]);

    insert_triggers(data);
}

static void trigger_button_delete (GtkWidget *button, gpointer data) {
    gchar *word;
    
    if ( trigger_selected_row == -1 ) {
        popup_window ("No selection made.");
    }
    else {
        char buffer[ALIAS_LEN + 20];
        
        gtk_clist_get_text ((GtkCList*) data, trigger_selected_row, 0, &word);
        sprintf(buffer, "#unaction {%s}", word);
        gtk_clist_remove ((GtkCList*) data, trigger_selected_row);
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
                popup_window("Unable to find '%s' in this list.", finding);
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
    GtkWidget *vbox; 
    GtkWidget *hbox;
    GtkWidget *hbox2;
    GtkWidget *hbox3;
    GtkWidget *clist;
    GtkWidget *label, *but;
//    GtkTooltips *tooltip;
    GtkWidget *scrolled_window;
    
    gchar     *titles[4] = { "Trigger string", "Commands", "Class", "State" };

//    tooltip = gtk_tooltips_new ();
//    gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);

    if (trig_window) {
        gtk_window_present(GTK_WINDOW(trig_window));
        return;
    }

    trig_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (trig_window), "Triggers (actions)");
    gtk_signal_connect (GTK_OBJECT (trig_window), "destroy",
                               GTK_SIGNAL_FUNC(close_window), trig_window );
    gtk_signal_connect (GTK_OBJECT (trig_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &trig_window );
    gtk_widget_set_usize (trig_window, 720, 450);			       
    vbox = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
    gtk_container_add (GTK_CONTAINER (trig_window), vbox);

    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);

    clist = gtk_clist_new_with_titles (4, titles);
    gtk_object_set_user_data(GTK_OBJECT(trig_window), clist); // needed to call insert_triggers from outside
    
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
                               GTK_SIGNAL_FUNC (trigger_selection_made),
                               (gpointer) clist);
    gtk_clist_column_titles_passive (GTK_CLIST (clist));
    gtk_clist_set_shadow_type (GTK_CLIST (clist), GTK_SHADOW_IN);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 200);
    gtk_clist_set_column_width (GTK_CLIST (clist), 1, 360);
    gtk_clist_set_column_width (GTK_CLIST (clist), 2, 80);
    gtk_clist_set_column_width (GTK_CLIST (clist), 3, 40);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 0, GTK_JUSTIFY_LEFT);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 1, GTK_JUSTIFY_LEFT);

    gtk_clist_column_titles_show (GTK_CLIST (clist));

    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), clist);

    hbox3 = gtk_table_new(4, 2, FALSE);

    gtk_box_pack_start (GTK_BOX (vbox), hbox3, FALSE, FALSE, 0);

    
    label = gtk_label_new ("Trigger");
    gtk_table_attach(GTK_TABLE(hbox3), label, 1, 2, 0, 1,
                        GTK_EXPAND|GTK_FILL, /*GTK_FILL*/ 0L, 2, 2);
    
    label = gtk_label_new ("Commands");
    gtk_table_attach(GTK_TABLE(hbox3), label, 2, 3, 0, 1,
                        GTK_FILL | GTK_EXPAND, /*GTK_FILL*/ 0L, 2, 2);

    label = gtk_label_new ("Class");
    gtk_table_attach(GTK_TABLE(hbox3), label, 3, 4, 0, 1,
                        0L, /*GTK_FILL*/ 0L, 2, 2);

    but = gtk_button_new();
    label = gtk_image_new_from_stock(GTK_STOCK_FIND, GTK_ICON_SIZE_SMALL_TOOLBAR);
    gtk_container_add(GTK_CONTAINER(but), label);
    gtk_table_attach(GTK_TABLE(hbox3), but, 0, 1, 1, 2,
                         0L, /*GTK_FILL*/ 0L, 2, 2);
    gtk_signal_connect (GTK_OBJECT (but), "clicked",
                       GTK_SIGNAL_FUNC (find_in_list), clist);

    textalias   = gtk_entry_new ();
    gtk_table_attach(GTK_TABLE(hbox3), textalias, 1, 2, 1, 2,
                         GTK_EXPAND|GTK_FILL, /*GTK_FILL*/ 0L, 2, 2);


    
    textreplace = gtk_entry_new ();
    gtk_table_attach(GTK_TABLE(hbox3), textreplace, 2, 3, 1, 2,
                        GTK_FILL | GTK_EXPAND, /*GTK_FILL*/ 0L, 2, 2);
   

    textpri = gtk_combo_new();

    update_classes();
    
    gtk_table_attach(GTK_TABLE(hbox3), textpri, 3, 4, 1, 2,
                        0L, /*GTK_FILL*/ 0L, 2, 2);

    AddButtonBar(vbox, (gpointer)clist,
            GTK_SIGNAL_FUNC(trigger_button_add),
            GTK_SIGNAL_FUNC(trigger_button_delete),
            GTK_SIGNAL_FUNC(save_triggers));

    insert_triggers  (GTK_CLIST(clist)  );
    gtk_widget_show_all (trig_window );

}
