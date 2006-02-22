#include "ggmud.h"

#define COMPLETE_LEN 40
static GtkWidget *complete_window = NULL;

static void
save_complete (GtkWidget *button, gpointer data) {
    FILE *fp;
    gint done = FALSE;
    gchar *gag;
    gint  row = 0;

    if ((fp = fileopen (COMPLETE_FILE, "w"))) {
    	while ( !done && (GtkCList*) data) {
            if ( !gtk_clist_get_text ((GtkCList*) data, row, 0, &gag))
                break;
            
            if (!gag[0]) {
                done = TRUE;
                break;
            }
            fprintf (fp, "%s\n", gag);
            row++;
        }
        fclose (fp);
    }    
}

static void 
add_complete(char *line)
{
    char buffer[COMPLETE_LEN + 20];

    sprintf(buffer, "#tabadd {%s}", line);
    
    parse_input(buffer, mud->activesession);
}


static void
insert_words  (GtkCList *clist)
{
    extern list_t *complist;
    gchar *text[1];
    list_t *list = complist;

    gtk_clist_clear(clist);
    gtk_clist_freeze(clist);

    if (!list)
        return;

    do {
        text[0] = list->word;
        gtk_clist_prepend (GTK_CLIST (clist), text);
    }
    while ( (list = list->next) );
    
    gtk_clist_thaw(clist);
}

static void
complete_button_add (GtkWidget *button, GtkCList * data)
{
    gchar *text;
    GtkEntry *textcomplete = GTK_ENTRY(
            gtk_object_get_data(GTK_OBJECT(data), "entry"));

    text   = gtk_entry_get_text (textcomplete);

    if ( text[0] == '\0' )    {
        popup_window ("Please insert some text first.");
        return;
    }

    if ( strlen (text) < 4)  {
        popup_window ("It's unsafe to TAB complete such a short text.");
        return;
    }
    
    if ( strlen (text) > COMPLETE_LEN)    {
        popup_window ("Word length too big.");
        return;
    }

    add_complete (text);
    insert_words(data);
}

static void complete_button_delete (GtkWidget *button, GtkCList * data) {
    gchar *word;
    int selected_row = (int) gtk_object_get_user_data(GTK_OBJECT(data));

    if (selected_row == -1 ) {
        popup_window ("No selection made.");
    }
    else {
        char buffer[COMPLETE_LEN + 20];
        
        gtk_clist_get_text (data, selected_row, 0, &word);
        gtk_object_set_user_data(GTK_OBJECT(data), (void *)-1);

        rltab_delete(word);

        insert_words(data);
    }
}

static void complete_selection_made (GtkWidget *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    

    if ( (GtkCList*) data ) {
        gchar *text;
        GtkWidget *entry = (GtkWidget *)
            gtk_object_get_data(GTK_OBJECT(clist), "entry");
        
        gtk_object_set_user_data(GTK_OBJECT(data), (void *)row);
        gtk_clist_get_text ((GtkCList*) data, row, 0, &text);

        if (text)
            gtk_entry_set_text (GTK_ENTRY (entry), text);
    }
}

void
create_complete_window(GtkWidget *w, gpointer data)
{
    GtkWidget *vbox;
    GtkWidget *hbox3;
    GtkWidget *clist, *text;
//    GtkTooltips *tooltip;
    GtkWidget *scrolled_window;

    gchar     *titles[1] = { "Word list" };

//    tooltip = gtk_tooltips_new ();
//    gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);

    if (complete_window) {
        gtk_window_present(GTK_WINDOW(complete_window));
        return;
    }

    complete_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (complete_window), "TAB complete settings");
    gtk_signal_connect (GTK_OBJECT (complete_window), "destroy",
                               GTK_SIGNAL_FUNC(close_window), complete_window );
    gtk_signal_connect (GTK_OBJECT (complete_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &complete_window );
    gtk_widget_set_usize (complete_window, 450, 320);			       
    vbox = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
    gtk_container_add (GTK_CONTAINER (complete_window), vbox);
    gtk_widget_show (vbox);

    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show (scrolled_window);

    clist = gtk_clist_new_with_titles (1, titles);
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
                               GTK_SIGNAL_FUNC (complete_selection_made),
                               (gpointer) clist);
    gtk_clist_column_titles_passive (GTK_CLIST (clist));
    gtk_clist_set_shadow_type (GTK_CLIST (clist), GTK_SHADOW_IN);
//    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 100);
//    gtk_clist_set_column_width (GTK_CLIST (clist), 1, 250);
//    gtk_clist_set_column_justification (GTK_CLIST (clist), 0, GTK_JUSTIFY_LEFT);
//    gtk_clist_set_column_justification (GTK_CLIST (clist), 1, GTK_JUSTIFY_LEFT);

    gtk_clist_column_titles_show (GTK_CLIST (clist));

    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), clist);

    gtk_widget_show (clist);
    gtk_object_set_user_data(GTK_OBJECT(clist), (void *)-1);
    
    hbox3 = gtk_frame_new("Insert a word to complete with TAB");
    gtk_widget_show(hbox3);
    
    gtk_box_pack_start (GTK_BOX (vbox), hbox3, FALSE, FALSE, 0);

    text   = gtk_entry_new ();
    gtk_widget_show (text  );
    gtk_container_add(GTK_CONTAINER(hbox3), text);

    gtk_object_set_data(GTK_OBJECT(clist), "entry", text);

    AddButtonBar(vbox, (gpointer)clist,
            GTK_SIGNAL_FUNC(complete_button_add),
            GTK_SIGNAL_FUNC(complete_button_delete),
            GTK_SIGNAL_FUNC(save_complete));
    
    insert_words  (GTK_CLIST(clist) );
    gtk_widget_show (complete_window );
}

