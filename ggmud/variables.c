#include "ggmud.h"

GtkWidget *menu_Tools_Variable;
static GtkWidget *variable_window;
static GtkWidget *textvariable;
static GtkWidget *textvariablevalue;
static int variable_selected_row = -1;

#define VARIABLE_FILE "variable"

#define VAR_LEN 56
#define VALUE_LEN 200

static void
save_variables (GtkWidget *button, gpointer data) {
    FILE *fp;
    gint done = FALSE;
    gchar *alias, *replace;
    gint  row = 0;

    if (fp = fileopen (VARIABLE_FILE, "w")) {
    	while ( !done && (GtkCList*) data) {
            if ( !gtk_clist_get_text ((GtkCList*) data, row, 0, &alias)
                || !gtk_clist_get_text ((GtkCList*) data, row, 1, &replace) )
                break;
            
            if (!alias[0]) {
                done = TRUE;
                break;
            }
            fprintf (fp, "#var {%s} {%s}\n", alias, replace);
            row++;
        }
        fclose (fp);
    }    
}

static void
insert_variables  (GtkWidget *clist)
{
    extern struct listnode *common_myvars;
    gchar *text[2];
    struct listnode *list = mud->activesession ? mud->activesession->myvars : common_myvars;

    while ( list = list->next ) {
        text[0] = list->left;
        text[1] = list->right;
        gtk_clist_prepend (GTK_CLIST (clist), text);
    }
}

static void variable_selection_made (GtkWidget *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    gchar *text;
    
    variable_selected_row    = row;

    if ( (GtkCList*) data )
    {
        gtk_clist_get_text ((GtkCList*) data, row, 0, &text);
        gtk_entry_set_text (GTK_ENTRY (textvariable), text);
        gtk_clist_get_text ((GtkCList*) data, row, 1, &text);
        gtk_entry_set_text (GTK_ENTRY (textvariablevalue), text);
    }
}

static void  add_variable (char *alias, char *replacement)
{
    char buffer[1024];
    
    sprintf(buffer, "#var {%s} {%s}", alias, replacement);

    parse_input(buffer, mud->activesession);
}

void load_variables ()
{
    FILE *fp;

    if((fp = fileopen(VARIABLE_FILE, "r"))) {
        parse_config(fp, NULL);
        fclose(fp);
    }
}


static void variable_button_add (GtkWidget *button, gpointer data)
{
    gchar *text[2];
    gint   i;

    text[0]   = gtk_entry_get_text (GTK_ENTRY (textvariable  ));
    text[1]   = gtk_entry_get_text (GTK_ENTRY (textvariablevalue));

    if ( text[0][0] == '\0' || text[1][0] == '\0' )    {
        popup_window ("Please complete the variable first.");
        return;
    }

    for ( i = 0 ; i < strlen (text[0]) ; i++ )    {
        if ( isspace (text[0][i]) )        {
            popup_window ("I can't make a variable of that.");
            return;
        }
    }

    if ( strlen (text[0]) > VAR_LEN)  {
        popup_window ("Variable name too big.");
        return;
    }
    
    if ( strlen (text[1]) > VALUE_LEN)    {
        popup_window ("Variable value too big.");
        return;
    }

    gtk_clist_append ((GtkCList *) data, text);
    add_variable (text[0], text[1]);
}

static void variable_button_delete (GtkWidget *button, gpointer data) {
    gchar *word;
    
    if ( variable_selected_row == -1 ) {
        popup_window ("No selection made.");
    }
    else {
        char buffer[VAR_LEN + 20];
        
        gtk_clist_get_text ((GtkCList*) data, variable_selected_row, 0, &word);
        gtk_clist_remove ((GtkCList*) data, variable_selected_row);
        variable_selected_row = -1;

        sprintf(buffer, "#unvar %s", word);

        parse_input(buffer, mud->activesession);
    }

}

void
variables_window(GtkWidget *w, gpointer data)
{
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *hbox2;
    GtkWidget *hbox3;
    GtkWidget *clist;
    GtkWidget *button_add;
    GtkWidget *button_quit;
    GtkWidget *button_delete;
    GtkWidget *button_save;
    GtkWidget *label;
    GtkWidget *separator;
    GtkTooltips *tooltip;
    GtkWidget *scrolled_window;

    gchar     *titles[2] = { "Variable", "Initial value" };

    tooltip = gtk_tooltips_new ();
    gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);


    variable_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (variable_window), "Variables");
    gtk_signal_connect (GTK_OBJECT (variable_window), "destroy",
                               GTK_SIGNAL_FUNC(close_window), variable_window );
    gtk_widget_set_usize (variable_window, 450, 320);			       
    vbox = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
    gtk_container_add (GTK_CONTAINER (variable_window), vbox);
    gtk_widget_show (vbox);

    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show (scrolled_window);

    clist = gtk_clist_new_with_titles (2, titles);
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
                               GTK_SIGNAL_FUNC (variable_selection_made),
                               (gpointer) clist);
    gtk_clist_column_titles_passive (GTK_CLIST (clist));
    gtk_clist_set_shadow_type (GTK_CLIST (clist), GTK_SHADOW_IN);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 100);
    gtk_clist_set_column_width (GTK_CLIST (clist), 1, 250);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 0, GTK_JUSTIFY_LEFT);
    gtk_clist_set_column_justification (GTK_CLIST (clist), 1, GTK_JUSTIFY_LEFT);

    gtk_clist_column_titles_show (GTK_CLIST (clist));

    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), clist);

    gtk_widget_show (clist);

    hbox3 = gtk_table_new(2, 2, FALSE);

    gtk_box_pack_start (GTK_BOX (vbox), hbox3, FALSE, FALSE, 0);
    gtk_widget_show (hbox3);

    
    label = gtk_label_new ("Variable");
    gtk_table_attach(GTK_TABLE(hbox3), label, 0, 1, 0, 1,
                        /*GTK_FILL*/ 0L, /*GTK_FILL*/ 0L, 2, 2);
    gtk_widget_show (label);
    
    label = gtk_label_new ("value");
    gtk_table_attach(GTK_TABLE(hbox3), label, 1, 2, 0, 1,
                        GTK_FILL | GTK_EXPAND, /*GTK_FILL*/ 0L, 2, 2);
    gtk_widget_show (label);

    textvariable   = gtk_entry_new ();
    gtk_table_attach(GTK_TABLE(hbox3), textvariable, 0, 1, 1, 2,
                         0L, /*GTK_FILL*/ 0L, 2, 2);
    gtk_widget_show (textvariable  );


    textvariablevalue = gtk_entry_new ();
    gtk_table_attach(GTK_TABLE(hbox3), textvariablevalue, 1, 2, 1, 2,
                        GTK_FILL | GTK_EXPAND, /*GTK_FILL*/ 0L, 2, 2);
    gtk_widget_show (textvariablevalue);
    
    AddButtonBar(vbox, (gpointer)clist,
            GTK_SIGNAL_FUNC(variable_button_add),
            GTK_SIGNAL_FUNC(variable_button_delete),
            GTK_SIGNAL_FUNC(save_variables));

    insert_variables  (clist        );
    gtk_widget_show (variable_window );
}

