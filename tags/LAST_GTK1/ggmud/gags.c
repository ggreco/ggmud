#include "ggmud.h"

#define GAG_FILE "gag"
#define GAG_LEN 128

static void
save_gags (GtkWidget *button, gpointer data) {
    FILE *fp;
    gint done = FALSE;
    gchar *gag;
    gint  row = 0;

    if ((fp = fileopen (GAG_FILE, "w"))) {
    	while ( !done && (GtkCList*) data) {
            if ( !gtk_clist_get_text ((GtkCList*) data, row, 0, &gag))
                break;
            
            if (!gag[0]) {
                done = TRUE;
                break;
            }
            fprintf (fp, "#gag {%s}\n", gag);
            row++;
        }
        fclose (fp);
    }    
}

static void 
add_gag(char *line)
{
    char buffer[GAG_LEN + 20];

    sprintf(buffer, "#gag {%s}", line);
    
    parse_input(buffer, mud->activesession);
}

void
load_gags()
{
    FILE *fp;

    if((fp = fileopen(GAG_FILE, "r"))) {
        parse_config(fp, NULL);
        fclose(fp);
    }
}


static void
insert_gags  (GtkCList *clist)
{
    extern struct listnode *common_subs;
    gchar *text[1];
    struct listnode *list = mud->activesession ? mud->activesession->subs : common_subs;

    gtk_clist_clear(clist);
    gtk_clist_freeze(clist);
 
    while ( (list = list->next) ) {
        text[0] = list->left;
        gtk_clist_prepend (GTK_CLIST (clist), text);
    }
    gtk_clist_thaw(clist);
}

static void
gag_button_add (GtkWidget *button, GtkCList * data)
{
    gchar *text;
    GtkWidget *textgag = (GtkWidget *)
                gtk_object_get_data(GTK_OBJECT(data), "entry");

    text   = gtk_entry_get_text (GTK_ENTRY (textgag  ));

    if ( text[0] == '\0' )    {
        popup_window ("Please insert some text first.");
        return;
    }

    if ( strlen (text) < 4)  {
        popup_window ("It's unsafe to gag a such short text.");
        return;
    }
    
    if ( strlen (text) > GAG_LEN)    {
        popup_window ("Gag value too big.");
        return;
    }

    add_gag (text);
    insert_gags(data);
}

static void gag_button_delete (GtkWidget *button, GtkCList * data) {
    gchar *word;
    int selected_row = (int) gtk_object_get_user_data(GTK_OBJECT(data));
    
    if (selected_row == -1 ) {
        popup_window ("No selection made.");
    }
    else {
        char buffer[GAG_LEN + 20];
        
        gtk_clist_get_text (data, selected_row, 0, &word);
        gtk_object_set_user_data(GTK_OBJECT(data), (void *) -1);

        sprintf(buffer, "#ungag {%s}", word);

        parse_input(buffer, mud->activesession);

        insert_gags(data);
    }

}

static void gag_selection_made (GtkWidget *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    gchar *text;
    
    if ( GTK_CLIST(data) ) {
        GtkWidget *textgag = (GtkWidget *)
                gtk_object_get_data(GTK_OBJECT(data), "entry");
                
        gtk_object_set_user_data(GTK_OBJECT(clist), (void *) row);

        gtk_clist_get_text (GTK_CLIST(data), row, 0, &text);
        gtk_entry_set_text (GTK_ENTRY (textgag), text);
    }
}

void
gags_window(GtkWidget *w, gpointer data)
{
    GtkWidget *gag_window, *textgag;
    GtkWidget *vbox;
    GtkWidget *hbox3;
    GtkWidget *clist;
    GtkTooltips *tooltip;
    GtkWidget *scrolled_window;

    gchar     *titles[1] = { "Text to gag" };

    tooltip = gtk_tooltips_new ();
//    gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);


    gag_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (gag_window), "Gags");
    gtk_signal_connect (GTK_OBJECT (gag_window), "destroy",
                               GTK_SIGNAL_FUNC(close_window), gag_window );
    gtk_widget_set_usize (gag_window, 450, 320);			       
    vbox = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
    gtk_container_add (GTK_CONTAINER (gag_window), vbox);
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
                               GTK_SIGNAL_FUNC (gag_selection_made),
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

    hbox3 = gtk_frame_new("Insert text to gag");
    gtk_widget_show(hbox3);
    
    gtk_box_pack_start (GTK_BOX (vbox), hbox3, FALSE, FALSE, 0);

    textgag   = gtk_entry_new ();
    gtk_widget_show (textgag  );
    gtk_container_add(GTK_CONTAINER(hbox3), textgag);

    gtk_object_set_data(GTK_OBJECT(clist), "entry", textgag);
    gtk_object_set_user_data(GTK_OBJECT(clist), (void *) -1);

    AddButtonBar(vbox, (gpointer)clist,
            GTK_SIGNAL_FUNC(gag_button_add),
            GTK_SIGNAL_FUNC(gag_button_delete),
            GTK_SIGNAL_FUNC(save_gags));
    
    insert_gags  (GTK_CLIST(clist)        );
    gtk_widget_show (gag_window );
}

