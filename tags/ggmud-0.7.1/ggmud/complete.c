#include "ggmud.h"
#include "support.h"
#include "interface.h"

extern void rltab_delete(const char *word);

#define COMPLETE_LEN 40
static GtkWidget *complete_window = NULL;

void
save_complete (GtkCList *data, GtkWidget *button) {
    FILE *fp;
    gint done = FALSE;
    gchar *gag;
    gint  row = 0;

    if ((fp = fileopen (COMPLETE_FILE, "w"))) {
    	while ( !done && data) {
            if ( !gtk_clist_get_text (data, row, 0, &gag))
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
add_complete(const char *line)
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
    gtk_object_set_user_data(GTK_OBJECT(clist), (void *)-1);
    gtk_clist_freeze(clist);

    if (!list)
        return;

    while ( (list = list->next) ) {
        text[0] = list->word;
        gtk_clist_append (GTK_CLIST (clist), text);
    }
    
    gtk_clist_thaw(clist);
}

void
complete_button_add (GtkCList * data, GtkWidget *button)
{
    const gchar *text  = gtk_entry_get_text (GTK_ENTRY(
            lookup_widget(button, "entry_text")));

    if ( text[0] == '\0' )    {
        popup_window (INFO, "Please insert some text first.");
        return;
    }

    if ( strlen (text) < 4)  {
        popup_window (WARN, "It's unsafe to TAB complete such a short text.");
        return;
    }
    
    if ( strlen (text) > COMPLETE_LEN)    {
        popup_window (ERR, "Word length too big.");
        return;
    }

    add_complete (text);
    insert_words(data);
}

void complete_button_delete (GtkCList * data, GtkWidget *button) {
    gchar *word;
    int selected_row = GPOINTER_TO_INT(
            gtk_object_get_user_data(GTK_OBJECT(data)));

    if (selected_row == -1 ) {
        popup_window (WARN, "No selection made.");
    }
    else {
        gtk_clist_get_text (data, selected_row, 0, &word);

        rltab_delete(word);

        insert_words(data);
    }
}

void complete_selection_made (GtkCList *clist, gint row, gint column,
                           GdkEventButton *event, gpointer data)
{
    gchar *text;

    gtk_object_set_user_data(GTK_OBJECT(clist), GINT_TO_POINTER(row));
    gtk_clist_get_text (clist, row, 0, &text);

    gtk_entry_set_text (GTK_ENTRY (lookup_widget(GTK_WIDGET(clist), "entry_text")), 
                text);
}

void
create_complete_window(GtkWidget *w, gpointer data)
{
    if (complete_window) {
        gtk_window_present(GTK_WINDOW(complete_window));
        return;
    }

    complete_window = create_window_complete();
    gtk_signal_connect (GTK_OBJECT (complete_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &complete_window );
    
    insert_words  (GTK_CLIST(lookup_widget(complete_window, "clist_complete")) );
    gtk_widget_show (complete_window );
}

