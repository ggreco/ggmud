#include "ggmud.h"
#include "support.h"
#include "include/action.h"
#include "interface.h"

#define BUTTONS_FILE "buttons"

extern char *get_arg_in_braces(char *s, char *arg, int flag);

static GList *hbutton_list = NULL;
static GList *vbutton_list = NULL;
static GtkWidget *buttons_window = NULL;

typedef struct button_def_
{
    char label[20];
    char command[512];
    char shortcut[40];
    GtkWidget *widget;
} button_def;

void on_button_clicked(GtkWidget *button, button_def *def)
{
    parse_input(def->command, mud->activesession);
}

GList *new_button(GList *list, GtkWidget *dest, char *label, char *command)
{
    button_def *button;
    GList *l = list;

    if (!*label || strlen(label) >= sizeof(button->label)) {
        tintin_puts("#ERROR IN BUTTON LABEL FORMAT (MAX 19 CHRS)", mud->activesession);
        return list;
    }

    if (!*command || strlen(command) >= sizeof(button->command)) {
        tintin_puts("#ERROR IN BUTTON COMMAND FORMAT (MAX 512 CHRS)", mud->activesession);
        return list;
    }
    
    while(l) {
        button_def *b = (button_def *)l->data;

        if ( !strcasecmp(b->label, label)) {
            // updating existing button
            strcpy(b->command, command);
            return list;
        }
        l = l->next;
    }

    // new button creation
    button = malloc(sizeof(button_def));
    strcpy(button->label, label);
    strcpy(button->command, command);
    button->widget = gtk_button_new_with_label(label);
    gtk_signal_connect(GTK_OBJECT(button->widget), "clicked", 
            GTK_SIGNAL_FUNC(on_button_clicked), button);
    list = g_list_append(list, button);

    gtk_box_pack_end(GTK_BOX(dest), button->widget, FALSE, FALSE, 2);

    return list;
}

GList * del_button(GList *list, GtkWidget *cont, char *label)
{
    GList *l = list;

    while(l) {
        button_def *b = (button_def *)l->data;

        if ( !strcasecmp(b->label, label)) {
            gtk_container_remove(GTK_CONTAINER(cont), b->widget);

            list = g_list_remove(list, l->data);

            free(b);
            return list;
        }

        l = l->next;
    }

    return list;
}

void button_command(char *arg, struct session *s, GList **list, GtkWidget *base)
{
    char left[BUFFER_SIZE], right[BUFFER_SIZE];
    
    arg = get_arg_in_braces(arg, left, 0);

    if(!*left) { // show button list
        GList *l = *list;

       tintin_puts(" *** defined button list ***", s);
        while(l) {
            button_def *b = (button_def *)l->data;

            sprintf(right, " Label [%19.19s] Command(s) [%s]", b->label, b->command);
            tintin_puts(right, s);
            l = l->next;
        }
        return;
    }
    else {
        arg = get_arg_in_braces(arg, right, 0);
        
        if(!*right) {
           *list = del_button(*list, base, left);
           tintin_puts("#DELETED SELECTED BUTTON", s);
        }
        else {
            *list = new_button(*list, base, left, right);
            tintin_puts("#BUTTON CREATED/UPDATED", s);
        }
    }

    if (!list)
        gtk_widget_hide(base->parent);
    else
        gtk_widget_show_all(base->parent);
}

void vbutton_command(char *arg, struct session *s)
{
    button_command(arg, s, &vbutton_list, lookup_widget(mud->window, "vbox_button"));

    if (buttons_window) {
        GtkWidget *w = lookup_widget(buttons_window, "clist_vert");

        if (w)
            insert_buttons(GTK_CLIST(w), vbutton_list);
    }
}
void hbutton_command(char *arg, struct session *s)
{
    button_command(arg, s, &hbutton_list, lookup_widget(mud->window, "hbox_button"));

    if (buttons_window) {
        GtkWidget *w = lookup_widget(buttons_window, "clist_horiz");

        if (w)
            insert_buttons(GTK_CLIST(w), hbutton_list);
    }
}

void
save_buttons (GtkWidget *button, gpointer *user_data) {
    FILE *fp;

    if ((fp = fileopen (BUTTONS_FILE, "w"))) {
        GList *l = hbutton_list;

        while(l) {
            button_def *b = (button_def *)l->data;
            fprintf(fp, "#hbutton {%s} {%s}\n", b->label, b->command);
            l = l->next;
        }

        l = vbutton_list;
        while(l) {
            button_def *b = (button_def *)l->data;
            fprintf(fp, "#vbutton {%s} {%s}\n", b->label, b->command);
            l = l->next;
        }
        fclose (fp);
    }    
}

void
load_buttons()
{
    FILE *fp;

    if((fp = fileopen(BUTTONS_FILE, "r"))) {
        parse_config(fp, NULL);
        fclose(fp);
    }
}

void insert_buttons(GtkCList *clist, GList *source)
{
    GList *l = source;
    gchar *text[2];

    gtk_clist_clear(clist);
    gtk_object_set_user_data(GTK_OBJECT(clist), GINT_TO_POINTER(-1));
    gtk_clist_freeze(clist);   

    while(l) {
        button_def *b = (button_def *)l->data;
        text[0] = b->label;
        text[1] = b->command;
        gtk_clist_append (GTK_CLIST (clist), text);
        l = l -> next;
    }
    gtk_clist_thaw(clist);
}

void
on_clist_button_select_row             (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    gchar *text;
    GtkEntry *l = GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(clist), "blabel")), 
             *c = GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(clist), "bcmd"));

    gtk_object_set_user_data(GTK_OBJECT(clist), GINT_TO_POINTER(row) );
    gtk_clist_get_text (GTK_CLIST(clist), row, 0, &text); 
    gtk_entry_set_text (l, text);
    gtk_clist_get_text (GTK_CLIST(clist), row, 1, &text); 
    gtk_entry_set_text (c, text);
}


void
on_button_buttadd_clicked(GtkCList *list, GtkButton *button)
{
    GtkEntry *l = GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(list), "blabel")), 
             *c = GTK_ENTRY(gtk_object_get_data(GTK_OBJECT(list), "bcmd"));
    const gchar *t1, *t2,
          *cmd = (const char *) gtk_object_get_data(GTK_OBJECT(list), "cmd");
    char buffer[BUFFER_SIZE];

    t1 = gtk_entry_get_text (l);
    t2 = gtk_entry_get_text (c);
    
    if ( t1[0] == '\0' || t2[0] == '\0')    {
        popup_window (INFO, "Please insert some text first.");
        return;
    }

    sprintf(buffer, "#%s {%s} {%s}", cmd, t1, t2);
    parse_input(buffer, mud->activesession);
}


void
on_button_buttdel_clicked(GtkCList *list, GtkButton *button)
{
    int selected_row = GPOINTER_TO_INT(
            gtk_object_get_user_data(GTK_OBJECT(list)));
    
    if (selected_row == -1 ) {
        popup_window (WARN, "No selection made.");
    }
    else {
        gchar *word;
        char buffer[128];
        char *cmd = (char *)gtk_object_get_data(GTK_OBJECT(list), "cmd");

        gtk_clist_get_text (list, selected_row, 0, &word);

        sprintf(buffer, "#%s {%s}", cmd, word);

        parse_input(buffer, mud->activesession);
    }
}

void
on_buttons_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    if (buttons_window) {
        gtk_window_present(GTK_WINDOW(buttons_window));
        return;
    }
    GtkWidget *h, *v; 
    buttons_window = create_window_buttons();

    gtk_signal_connect (GTK_OBJECT (buttons_window), "destroy",
                               GTK_SIGNAL_FUNC(kill_window), &buttons_window );

    h = lookup_widget(buttons_window, "clist_horiz");
    v = lookup_widget(buttons_window, "clist_vert");

    // set the reference object for the list, this is useful for future expansion to the gui,
    // we'll have only to change this function
    gtk_object_set_data(GTK_OBJECT(h), "blabel", lookup_widget(buttons_window, "entry_hlabel"));
    gtk_object_set_data(GTK_OBJECT(h), "bcmd", lookup_widget(buttons_window, "entry_hcmd"));
    gtk_object_set_data(GTK_OBJECT(h), "cmd", "hbutton");

    gtk_object_set_data(GTK_OBJECT(v), "blabel", lookup_widget(buttons_window, "entry_vlabel"));
    gtk_object_set_data(GTK_OBJECT(v), "bcmd", lookup_widget(buttons_window, "entry_vcmd"));
    gtk_object_set_data(GTK_OBJECT(v), "cmd", "vbutton");

    insert_buttons  (GTK_CLIST(h), hbutton_list);
    insert_buttons  (GTK_CLIST(v), vbutton_list);

    gtk_widget_show (buttons_window );
}

