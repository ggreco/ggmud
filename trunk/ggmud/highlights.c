#include "ggmud.h"
#include "ansi.h"

extern struct listnode *common_highs;

#define HIGH_FILE "highlight"
#define ALIAS_LEN 128

static GtkWidget *high_window = NULL;

static void
save_highlights (GtkWidget *button, gpointer data) {
    FILE *fp;

    if ((fp = fileopen (HIGH_FILE, "w"))) {
        struct listnode *list = mud->activesession ? mud->activesession->highs : common_highs;
        
        while ( (list = list->next) ) {
            fprintf(fp, "#high {%s} {%s}\n", list->right, list->left);
        }
        fclose (fp);
    }    
}

void
load_highlights() 
{
    FILE *fp;

    if ((fp = fileopen(HIGH_FILE, "r"))) {
        parse_config(fp, NULL);   
        fclose(fp);
    }
}

static GtkWidget *bgwidget, *fgwidget, *textalias;


typedef struct {
    char *name;
    char *tt_name;
    GdkColor *color;
}
color_options;

static color_options bg_options[] =
{
    {"Black", "b black", &color_black},
    {"Red", "b red", &color_red},
    {"Green", "b green", &color_green},
    {"Brown", "b brown", &color_yellow},
    {"Blue", "b blue", &color_blue},
    {"Magenta", "b magenta", &color_magenta},
    {"Cyan", "b cyan", &color_cyan},
    {"Grey", "b grey", &color_white},
    {"Charcoal", "b charcoal", &color_lightblack},
    {"Light red", "b light red", &color_lightred},
    {"Light green", "b light green", &color_lightgreen},
    {"Yellow", "b yellow", &color_lightyellow},
    {"Light blue", "b light blue", &color_lightblue},
    {"Light magenta", "b light magenta", &color_lightmagenta},
    {"Light cyan", "b light cyan", &color_lightcyan},
    {"White", "b white", &color_lightwhite},
    {"Bold", "bold", NULL},
    {"Italic", "italic", NULL},
    {"Reverse", "reverse", NULL},
    {NULL, NULL, NULL}
};

static color_options fg_options[] =
{
    {"Black", "black", &color_black},
    {"Red", "red", &color_red},
    {"Green", "green", &color_green},
    {"Brown", "brown", &color_yellow},
    {"Blue", "blue", &color_blue},
    {"Magenta", "magenta", &color_magenta},
    {"Cyan", "cyan", &color_cyan},
    {"Grey", "grey", &color_white},
    {"Charcoal", "charcoal", &color_lightblack},
    {"Light red", "light red", &color_lightred},
    {"Light green", "light green", &color_lightgreen},
    {"Yellow", "yellow", &color_lightyellow},
    {"Light blue", "light blue", &color_lightblue},
    {"Light magenta", "light magenta", &color_lightmagenta},
    {"Light cyan", "light cyan", &color_lightcyan},
    {"White", "white", &color_lightwhite},
    {NULL, NULL, NULL}
};

int get_optionmenu_active(GtkWidget *t)
{
    GtkWidget *k,*active_item;
    
    if (!(k = gtk_option_menu_get_menu(GTK_OPTION_MENU(t)))) {
        return -1;
    }
        
    if (!(active_item = gtk_menu_get_active (GTK_MENU (k)))) {
        return -1;
    }

    return g_list_index (
                GTK_MENU_SHELL(k)->children,
                active_item);
}

static int get_options(char *text, color_options *opts)
{
    int i = 0;
    
    while(opts[i].name) {
        if(!strcmp(text, opts[i].name))
            return i;
        
        i++;
    }
    return -1;
}

static int get_options_tt(char *text, color_options *opts)
{
    int i = 0;
    
    while(opts[i].tt_name) {
        if(!strcmp(text, opts[i].tt_name))
            return i;
        
        i++;
    }
    return -1;
}

static void high_selection_made (GtkWidget *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    gchar *text;
    int i;
    
    if ( (GtkCList*) data )
    {
        gtk_object_set_user_data(GTK_OBJECT(data), GINT_TO_POINTER( row) );
         
        gtk_clist_get_text ((GtkCList*) data, row, 0, &text);
        gtk_entry_set_text (GTK_ENTRY (textalias), text);
        
        gtk_clist_get_text ((GtkCList*) data, row, 1, &text);
        i = get_options(text, bg_options);
        if(i >= 0)
            gtk_option_menu_set_history(GTK_OPTION_MENU(bgwidget), i);
        
        gtk_clist_get_text ((GtkCList*) data, row, 2, &text);
        i = get_options(text, fg_options);
        if(i >= 0)
            gtk_option_menu_set_history(GTK_OPTION_MENU(fgwidget), i);
    }
    
    return;
}


static void
add_highlight(const char *string, color_options *bg, color_options *fg)
{
    char buffer[200];

    sprintf(buffer, "#high {%s, %s} {%s}",
            bg->tt_name, fg->tt_name, string);
            
    parse_input(buffer, NULL);
}

static void  insert_highlights  (GtkCList *clist)
{
    gchar *text[3];
    struct listnode *list = mud->activesession ? mud->activesession->highs : common_highs;
    int i, j;
    char buffer[200], *c;

    gtk_clist_clear (clist);

    gtk_clist_freeze(clist);

    while ( (list = list->next) ) {
        text[0] = list->left;
        
        strcpy(buffer, list->right);

        c = buffer;

        i = j = - 1;
        
        while (*c != ',' && *c > 0)
            c++;

        if( *c == ',') {
            char *e;

            *c = 0;
            c++;

            while( *c == ' ' || *c == '\t')
                c++;

            e = c;
            
            while(*c != ',' && *c != 0)
                c++;

            *c = 0;
            
            if( (i = get_options_tt(e, bg_options)) < 0)
                j = get_options_tt(e, fg_options);
        }

        if( (i = get_options_tt(buffer, bg_options)) < 0 && j < 0)
            j = get_options_tt(buffer, fg_options);

        if(i >= 0)
            text[1] = bg_options[i].name;
        else
            text[1] = "None";
        
        if(j >= 0)
            text[2] = fg_options[j].name;
        else
            text[2] = "None";
        
        gtk_clist_append (clist, text);
    }
    gtk_clist_thaw(clist);

}

static void high_button_add (GtkWidget *button, GtkCList * data)
{
    const gchar *text;
    gint   i, j;

    text   = gtk_entry_get_text (GTK_ENTRY (textalias  ));

    i = get_optionmenu_active(bgwidget);
    j = get_optionmenu_active(fgwidget);
    
    if ( text[0] == '\0'  ) {
        popup_window (INFO, "Please insert some text first.");
        return;
    }

    if ( strlen (text) > ALIAS_LEN) {
        popup_window (ERR, "String too big.");
        return;
    }
    
    add_highlight (text, &bg_options[i], &fg_options[j]);

    insert_highlights(data);
}



static void high_button_delete (GtkWidget *button, GtkCList * data) {
    gchar *word;
    int selected_row = GPOINTER_TO_INT(
            gtk_object_get_user_data(GTK_OBJECT(data)));
 
    if ( selected_row == -1 ) {
        popup_window (WARN, "No selection made.");
    }
    else {
        char buffer[ALIAS_LEN + 20];
        
        gtk_clist_get_text (data, selected_row, 0, &word);
        sprintf(buffer, "#unhigh {%s}", word);
        parse_input(buffer, mud->activesession);
        gtk_object_set_user_data(GTK_OBJECT(data), (void *) -1);

        insert_highlights(data);
    }

}

static void append_options(GtkWidget *w, color_options *options)
{
    int i = 0;
    GtkWidget *t;
    GtkWidget *m = gtk_menu_new();
    gtk_widget_show(m);
    
    while(options[i].name) {
        t = gtk_menu_item_new_with_label (options[i].name);
        gtk_widget_show(t);
        gtk_menu_append(GTK_MENU(m), t);
        i++;
    }

    gtk_option_menu_set_menu(GTK_OPTION_MENU(w), m);
}

void highlights_window (GtkWidget *widget, gpointer data)
{
    GtkWidget *vbox;
    GtkWidget *hbox3;
    GtkWidget *clist;
    GtkWidget *label;
//    GtkTooltips *tooltip;
    GtkWidget *scrolled_window;

    gchar     *titles[3] = { "Highlight", "Background/Effect", "Foreground" };

//    tooltip = gtk_tooltips_new ();
//    gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);
    if (high_window) {
        gtk_window_present(GTK_WINDOW(high_window));
        return;
    }


    high_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (high_window), "Highlights");
    gtk_signal_connect (GTK_OBJECT (high_window), "destroy",
                               GTK_SIGNAL_FUNC(close_window), high_window );
    gtk_signal_connect (GTK_OBJECT (high_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &high_window );
    gtk_widget_set_usize (high_window, 550, 320);			       
    vbox = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 0);
    gtk_container_add (GTK_CONTAINER (high_window), vbox);
    gtk_widget_show (vbox);

    /* create a new scrolled window. */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show (scrolled_window);

    clist = gtk_clist_new_with_titles (3, titles);
    gtk_signal_connect_object (GTK_OBJECT (clist), "select_row",
                               GTK_SIGNAL_FUNC (high_selection_made),
                               (gpointer) clist);
    gtk_clist_column_titles_passive (GTK_CLIST (clist));
    gtk_clist_set_shadow_type (GTK_CLIST (clist), GTK_SHADOW_IN);
    gtk_clist_set_column_width (GTK_CLIST (clist), 0, 250);
    gtk_clist_set_column_width (GTK_CLIST (clist), 1, 100);
    gtk_clist_set_column_width (GTK_CLIST (clist), 2, 100);
//    gtk_clist_set_column_justification (GTK_CLIST (clist), 0, GTK_JUSTIFY_LEFT);
//    gtk_clist_set_column_justification (GTK_CLIST (clist), 1, GTK_JUSTIFY_LEFT);

    gtk_clist_column_titles_show (GTK_CLIST (clist));

    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window), clist);

    gtk_widget_show (clist);

    gtk_object_set_user_data(GTK_OBJECT(clist), (void *) -1);
    
    hbox3 = gtk_table_new(3, 2, FALSE);

    gtk_box_pack_start (GTK_BOX (vbox), hbox3, FALSE, FALSE, 0);
    gtk_widget_show (hbox3);

    
    label = gtk_label_new ("Text");
    gtk_table_attach(GTK_TABLE(hbox3), label, 0, 1, 0, 1,
                        GTK_FILL|GTK_EXPAND, /*GTK_FILL*/ 0L, 2, 2);
    gtk_widget_show (label);
    
    label = gtk_label_new ("Back/Effect");
    gtk_table_attach(GTK_TABLE(hbox3), label, 1, 2, 0, 1,
                        0L, /*GTK_FILL*/ 0L, 2, 2);
    gtk_widget_show (label);

    label = gtk_label_new ("Foreground");
    gtk_table_attach(GTK_TABLE(hbox3), label, 2, 3, 0, 1,
                        0L, /*GTK_FILL*/ 0L, 2, 2);
    gtk_widget_show (label);

    textalias   = gtk_entry_new ();
    gtk_table_attach(GTK_TABLE(hbox3), textalias, 0, 1, 1, 2,
                         GTK_FILL|GTK_EXPAND,  0L, 2, 2);
    gtk_widget_show (textalias  );


    bgwidget = gtk_option_menu_new();
    append_options(bgwidget, bg_options);
    gtk_table_attach(GTK_TABLE(hbox3), bgwidget, 1, 2, 1, 2,
                            0L, /*GTK_FILL*/ 0L, 2, 2);
    gtk_widget_show (bgwidget);

    fgwidget = gtk_option_menu_new();
    append_options(fgwidget, fg_options);
    gtk_table_attach(GTK_TABLE(hbox3), fgwidget, 2, 3, 1, 2,
                            0L, /*GTK_FILL*/ 0L, 2, 2);
    gtk_widget_show (fgwidget);
    
    AddButtonBar(vbox, (gpointer)clist,
            GTK_SIGNAL_FUNC(high_button_add),
            GTK_SIGNAL_FUNC(high_button_delete),
            GTK_SIGNAL_FUNC(save_highlights));

    insert_highlights  (GTK_CLIST(clist)        );
    gtk_widget_show (high_window );

}
