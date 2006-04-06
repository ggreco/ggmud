/* GGMud 
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

#include <glib.h>
#include <string.h>
#include "ggmud.h"
#include "config.h"
#include "pixmaps.h"	// ToolBar Icons
#include "ansi.h"

extern void log_viewer();
extern void alt_send_to_connection  (gchar *text);

typedef enum { F1, F2, F3, F4, F5, F6, F7, F8, F9, F10 } BUTTON;
#define LorC(L)		(strlen(L) < 10 ? 0.5 : 0)

/* External variables used */
extern gchar *keys[];
extern int hide_input;

/* Global variables */
GtkWidget *btnLabel[12];
GtkWidget *menu_Tools_Logger;
GtkWidget *handlebox;

/* Global ToolBar stuff */
GtkWidget *btn_toolbar_logger;
GtkWidget *btn_toolbar_disconnect;
GtkWidget *btn_toolbar_connect;
GtkWidget *menu_File_Connect;
GtkWidget *menu_File_DisConnect;
GtkWidget *menu_File_Reconnect;

void reconnect(void)
{
    if(!connected || !mud->activesession)
        popup_window(INFO, "Must be connected first!");
    else {
        char *name, *host, *port;
        
        name = strdup(mud->activesession->name);
        host = strdup(mud->activesession->host);
        port = strdup(mud->activesession->port);
        disconnect();

        make_connection(name, host, port);

        free(name); free(host); free(port);
    }
}


typedef struct {
    char name[32];
    GtkWidget *listptr;    
} window_entry;

GList *windows_list = NULL;

/******************************/
/* the #showme / #say command */
/******************************/

extern char *get_arg_in_braces(char *s, char *arg, int flag);


void set_style() 
{
    extern PangoFontDescription *font_normal;
    
    if (!font_normal)
        return;
    
    if (mud && mud->text) {
        gtk_widget_modify_font(GTK_WIDGET(mud->text), font_normal);
    }

    GList *l = windows_list;

    while (l) {
        gtk_widget_modify_font(
                GTK_WIDGET(((window_entry *)l->data)->listptr), font_normal);
                
        l = l->next;
    }
}

GtkTextView *new_view(char *name, GtkWidget *parent, int ismain);

void destroy_a_window(GtkWidget *w)
{
    GList *l = windows_list;
    
    while(l) {
        if(((window_entry *)l->data)->listptr == w) {
            free(l->data);
            windows_list = g_list_remove(windows_list, l->data);
            
            return;
        }
        
        l = l->next;
    }
}

GtkWidget *create_new_window(char *title, int width, int height)
{
    GtkWidget *win, *vbox;
    GtkWidget *list;

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(win, width, height);
    gtk_window_set_title(GTK_WINDOW(win), title);
    gtk_container_border_width(GTK_CONTAINER(win), 4);

    gtk_widget_realize(win);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show(vbox);

    gtk_container_add(GTK_CONTAINER(win), vbox);
    
    list = (GtkWidget *) new_view(NULL, vbox, FALSE);
    gtk_signal_connect (GTK_OBJECT (list), "destroy", GTK_SIGNAL_FUNC (destroy_a_window), NULL);

    gtk_widget_show(win);


    return list;
}

void
input_line_visible(int state)
{
    if (state == hide_input) {
       const char *temp = gtk_entry_get_text(mud->ent);
       int pos = mud->hist->pos;
       
       pos--;

       if(pos < 0)
           pos = mud->hist->size - 1;

       if(!strcmp(temp, mud->hist->list[pos])) 
           gtk_entry_set_text(mud->ent, "");
    }
    
    if(state == FALSE) {
        hide_input = TRUE;
    }
    else
        hide_input = FALSE;
            
    gtk_entry_set_visibility(mud->ent, state);
}

window_entry *create_new_entry(char *title, int width, int height)
{
    window_entry *entry;
    
    if(!(entry = malloc(sizeof(window_entry))))
        return NULL;

    strncpy(entry->name, title, 31);

    if((entry->listptr = create_new_window(title, width, height)))
        windows_list = g_list_append(windows_list, entry);
    else {
        free(entry);
        return NULL;
    }

    return entry;
}

void load_win_pos()
{
    FILE *f;

    if((f = fileopen("winpositions", "r"))) {
        char name[100];
        int x,y,w,h;

        while(!feof(f)) {
            if (fscanf(f, "%s %d,%d %dx%d\n", name, &x, &y, &w, &h) == 5) {

//                printf("%s %d %d -> %d %d\n", name, x, y, w, h);

                if(!strcmp(name, "main")) {
                    gdk_window_move_resize(gtk_widget_get_toplevel(mud->window)->window, x, y, w, h);
                }
                else {
                    window_entry * e = create_new_entry(name, w, h); // creo la nuova finestra nel caso non ci sia
                    gdk_window_move_resize(gtk_widget_get_toplevel(e->listptr)->window, x, y, w, h);
                }
            }
        }
        fclose(f);
    }
    gdk_window_raise(gtk_widget_get_toplevel(mud->window)->window);
}

void setup_pixmaps()
{
    extern GdkPixmap *enabled_pixmap, *disabled_pixmap;
    extern GdkBitmap *enabled_mask, *disabled_mask;
   
    enabled_pixmap = gdk_pixmap_create_from_xpm_d ( mud->window->window, &enabled_mask, &mud->window->style->white, yes_xpm );
    disabled_pixmap = gdk_pixmap_create_from_xpm_d ( mud->window->window, &disabled_mask, &mud->window->style->white, no_xpm );

}

void write_win_pos(char *name, FILE *dest, GtkWidget *widget)
{
    int x,y, w, h;

    widget = gtk_widget_get_toplevel(widget);

//    gdk_window_get_position(widget->window, &x, &y);
    gdk_window_get_root_origin(widget->window, &x, &y);
    gdk_window_get_size(widget->window, &w, &h);
    fprintf(dest, "%s %d,%d %dx%d\n", name, x, y, w, h);    
}

void save_win_pos()
{
    FILE *f;

    if(!mud->window)
        return;

    if((f = fileopen("winpositions", "w"))) {
        GList *l = windows_list;

        write_win_pos("main", f, mud->window);

        while(l) {
            window_entry *e = (window_entry *)l->data;

            write_win_pos(e->name, f, e->listptr);

            l = l->next;
        }

        fclose(f);
    }
}

void showme_command(char *arg, struct session *ses)
{
  char left[BUFFER_SIZE], result[BUFFER_SIZE], *d;

  get_arg_in_braces(arg, left, 1);
  prepare_actionalias(left, result, ses);
  d = ParseAnsiColors(result);

  strcat(d, "\n");
  
  textfield_add(mud->text, d, MESSAGE_LOCAL);
}

window_entry *in_window_list(char *tag)
{
    GList *l = windows_list;

    while(l) {
        if(!strcmp(((window_entry *)l->data)->name, tag))
            return (window_entry *)l->data;
        
        l = l->next;
    }

    return NULL;
}

void clear_text_widget(GtkTextView *w)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(w), "", -1); 
}

void clr_command(char *arg, struct session *s)
{
    char left[BUFFER_SIZE], right[BUFFER_SIZE];
    window_entry *entry = NULL;
    
    arg = get_arg_in_braces(arg, left, 0);

    if(!*left) {
        clear_text_widget(mud->text);
    }
    else {
        int width = 400, height = 300;
        arg = get_arg_in_braces(arg, right, 0);
        
        if(*right) {
            int t1, t2;
            
            if(sscanf(right, "%dx%d", &t1, &t2) == 2) {
                width = t1;
                height = t2;
            }
        }
        
        if(!(entry = in_window_list(left))) {
            create_new_entry(left, width, height); // creo la nuova finestra nel caso non ci sia
            gtk_window_activate_focus(GTK_WINDOW(mud->window));
        }
        else
            clear_text_widget(GTK_TEXT_VIEW(entry->listptr));
    }
}

void
save_review(void)
{
    GtkWidget *filew = gtk_file_chooser_dialog_new ("Save Review buffer as...",
                                             GTK_WINDOW(mud->window), 
                                             GTK_FILE_CHOOSER_ACTION_SAVE,
                                             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                             GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                             NULL);
    
    gtk_widget_show(filew);

    if (gtk_dialog_run(GTK_DIALOG(filew)) == GTK_RESPONSE_ACCEPT) {
        GtkTextIter start, end;
        GtkTextBuffer *b = gtk_text_view_get_buffer(mud->text);
        FILE *f;    
        char *buffer, *name;

        gtk_text_buffer_get_bounds(b, &start, &end);

        buffer = gtk_text_buffer_get_text(b, &start, &end, FALSE);

        name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filew));
        
        if (( f = fopen(name, "w") )) {
            if (buffer) {
                fputs(buffer, f);
            }
            else
                popup_window(ERR, "Unable to lock review buffer!");

            fclose(f);
        }
        else
            popup_window(ERR, "Unable to open %s for reading.", name);

        g_free(name);
        
        if (buffer)
            g_free(buffer);
    }

    gtk_widget_destroy(filew);
}

void mess_command(char *arg, struct session *s)
{
    char left[BUFFER_SIZE], right[BUFFER_SIZE];

    arg = get_arg_in_braces(arg, left, 0);
    arg = get_arg_in_braces(arg, right, 1);

    if(!*left || !*right)
        return;
   
    substitute_myvars(right, left, s);
    substitute_vars(left, right, s);

    popup_window(INFO, right);
}

void window_command(char *arg, struct session *s)
{
    extern char *get_arg_in_braces(char *s, char *arg, int flag);
    char left[BUFFER_SIZE], right[BUFFER_SIZE];
    window_entry *entry;

    arg = get_arg_in_braces(arg, left, 0);
    arg = get_arg_in_braces(arg, right, 1);

    if(!*left || strlen(left) > 31)
        return;

    if(!(entry = in_window_list(left))) {
        int width = 400, height = 300;
// anche qui metto la possibilita' di settare le dimensioni?        
        entry = create_new_entry(left, width, height);

        gtk_window_activate_focus(GTK_WINDOW(mud->window));
    }

    
    if(right && *right && entry) {
        char *result;
        substitute_myvars(right, left, s);
        substitute_vars(left, right, s);
        result = ParseAnsiColors(right);
        strcat(result, "\n");
        textfield_add((GtkTextView *)entry->listptr, result , MESSAGE_LOCAL);
    }
}

GtkWidget *
MakeButton(const char *image, GtkSignalFunc func, gpointer data)
{
    GtkWidget *button = gtk_button_new_from_stock(image);
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                               func,
                               data );

    gtk_widget_show(button);

    return button;
}

void AddSimpleBar(GtkWidget *vbox, gpointer *data,
        GtkSignalFunc save_func,
        GtkSignalFunc close_func
        ) {
  GtkAccelGroup *accel_group = gtk_accel_group_new ();
  GtkWidget *hbuttonbox = gtk_hbutton_box_new (), *button;
  gtk_widget_show (hbuttonbox);
  
  gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, TRUE, 5);
  button = MakeButton(GTK_STOCK_SAVE, save_func, data);
  gtk_box_pack_start (GTK_BOX (hbuttonbox), button,    TRUE, TRUE, 15);

  button = MakeButton(GTK_STOCK_CANCEL, close_func, gtk_widget_get_toplevel(vbox));
  gtk_box_pack_start (GTK_BOX (hbuttonbox), button,    TRUE, TRUE, 15);

  gtk_window_add_accel_group(
            GTK_WINDOW(gtk_widget_get_toplevel(vbox)), 
            accel_group);
}

void AddButtonBar(GtkWidget *vbox, gpointer *data,
        GtkSignalFunc add_func,
        GtkSignalFunc del_func,
        GtkSignalFunc save_func
        )
{
    GtkWidget *hbox;
    GtkWidget * separator;
    GtkWidget *button_add;
    GtkWidget *button_quit;
    GtkWidget *button_delete;
    GtkWidget *button_save;
    GtkAccelGroup *accel_group = gtk_accel_group_new ();
    
    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, TRUE, 5);
    gtk_widget_show (separator);

    hbox = gtk_hbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 10);
    gtk_widget_show (hbox);

    
    button_add    = MakeButton(GTK_STOCK_ADD  , add_func, data);    
    button_delete = MakeButton(GTK_STOCK_DELETE, del_func, data);
    button_save   = MakeButton(GTK_STOCK_SAVE, save_func, data);
    button_quit   = MakeButton(GTK_STOCK_CANCEL, GTK_SIGNAL_FUNC(close_window), 
            gtk_widget_get_toplevel(vbox));

    gtk_box_pack_start (GTK_BOX (hbox), button_add,    TRUE, TRUE, 15);
    gtk_box_pack_start (GTK_BOX (hbox), button_delete, TRUE, TRUE, 15);
    gtk_box_pack_start (GTK_BOX (hbox), button_save,   TRUE, TRUE, 15);
    gtk_box_pack_start (GTK_BOX (hbox), button_quit,   TRUE, TRUE, 15);

    gtk_window_add_accel_group(
            GTK_WINDOW(gtk_widget_get_toplevel(vbox)), 
            accel_group);
}

void toggle_parsing(GtkToggleButton *togglebutton,
                                            gpointer user_data)
{
    extern int verbatim;
    GdkPixmap *icon;
    GdkBitmap *mask;
    char **image = SC_parsing;
    //GtkWidget *iconw;

    if(gtk_toggle_button_get_active(togglebutton)) {
        textfield_add(mud->text, "# PARSING ENABLED\n", MESSAGE_SENT);
        verbatim = 0;
    }
    else {
        textfield_add(mud->text, "# PARSING DISABLED\n", MESSAGE_SENT);
        verbatim = 1;
        image = SC_parsingoff;
    }

    if ((icon = gdk_pixmap_create_from_xpm_d ( GTK_WIDGET(togglebutton)->window, 
            &mask, NULL, image ))) {
        gtk_pixmap_set (GTK_PIXMAP(GTK_BIN(togglebutton)->child), 
                icon, mask ); 					/* icon widget */

        gdk_pixmap_unref(icon);
        gdk_bitmap_unref(mask);
    }    
}

void toggle_triggers(GtkToggleButton *togglebutton,
                                            gpointer user_data)
{
    extern int use_triggers;
    GdkPixmap *icon;
    GdkBitmap *mask;
    char **image = SC_triggers;
    //GtkWidget *iconw;

    if(gtk_toggle_button_get_active(togglebutton)) {
        textfield_add(mud->text, "# TRIGGERS ENABLED\n", MESSAGE_SENT);
        use_triggers = 1;
    }
    else {
        textfield_add(mud->text, "# TRIGGERS DISABLED\n", MESSAGE_SENT);
        use_triggers = 0;
        image = SC_triggersoff;
    }

    if ( (icon = gdk_pixmap_create_from_xpm_d ( GTK_WIDGET(togglebutton)->window, 
            &mask, NULL, image ))) {
        gtk_pixmap_set (GTK_PIXMAP(GTK_BIN(togglebutton)->child), 
                icon, mask ); 					/* icon widget */

        gdk_pixmap_unref(icon);
        gdk_bitmap_unref(mask);
    }    
}

void kill_window (GtkWidget *widget, gpointer data)
{
    GtkWidget **w = (GtkWidget **)data;

    *w = NULL;
}

void close_window (GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy (GTK_WIDGET (data));
}

void quit (GtkWidget *widget, gpointer data)
{
    if (mud->LOGGING)
        fclose (mud->LOG_FILE);
    gtk_main_quit();
}

void do_con()
{
    const char *port;
    const char *host;
    gchar buf[256];

    host = gtk_entry_get_text(GTK_ENTRY(mud->hostentry));
    port = gtk_entry_get_text(GTK_ENTRY(mud->portentry));
	
    make_connection("unknown", host,port);
    sprintf (buf, "Connected to %s - GGMud %s", host, VERSION);
    gtk_window_set_title (GTK_WINDOW (mud->window), buf);
}

void cbox()
{
    GtkWidget *con_window;
    GtkWidget *table_main;
    GtkWidget *frame;
    GtkWidget *table_frame;
    GtkWidget *label_host;
    GtkWidget *label_port;
    GtkWidget *table_button;
    GtkWidget *hbuttonbox;
    GtkWidget *button_connect;
    GtkWidget *button_cancel;
    
    con_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (con_window), "GGMud Connection");
    gtk_window_set_policy (GTK_WINDOW (con_window), TRUE, TRUE, FALSE);

    table_main = gtk_table_new (2, 1, FALSE);
    gtk_widget_show (table_main);
    gtk_container_add (GTK_CONTAINER (con_window), table_main);
    gtk_container_set_border_width (GTK_CONTAINER (table_main), 3);

    frame = gtk_frame_new ("Connect To");
    gtk_widget_show (frame);
    gtk_table_attach (GTK_TABLE (table_main), frame, 0, 1, 0, 1,
                     (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 3);
    gtk_frame_set_label_align (GTK_FRAME (frame), 0.03, 0.5);

    table_frame = gtk_table_new (2, 2, FALSE);
    gtk_widget_show (table_frame);
    gtk_container_add (GTK_CONTAINER (frame), table_frame);
    gtk_container_set_border_width (GTK_CONTAINER (table_frame), 5);
    gtk_table_set_row_spacings (GTK_TABLE (table_frame), 5);
    gtk_table_set_col_spacings (GTK_TABLE (table_frame), 4);

    label_host = gtk_label_new ("Host");
    gtk_widget_show (label_host);
    gtk_table_attach (GTK_TABLE (table_frame), label_host, 0, 1, 0, 1,
                     (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

    label_port = gtk_label_new ("Port");
    gtk_widget_show (label_port);
    gtk_table_attach (GTK_TABLE (table_frame), label_port, 0, 1, 1, 2,
                     (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

    mud->portentry = gtk_entry_new ();
    gtk_widget_show (mud->portentry);
    gtk_table_attach (GTK_TABLE (table_frame), mud->portentry, 1, 2, 1, 2,
                    (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

    mud->hostentry = gtk_entry_new ();
    gtk_widget_show (mud->hostentry);
    gtk_table_attach (GTK_TABLE (table_frame), mud->hostentry, 1, 2, 0, 1,
                     (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);

    table_button = gtk_table_new (1, 1, FALSE);
    gtk_widget_show (table_button);
    gtk_table_attach (GTK_TABLE (table_main), table_button, 0, 1, 1, 2,
                     (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 2, 0);
    gtk_container_set_border_width (GTK_CONTAINER (table_button), 4);

    hbuttonbox = gtk_hbutton_box_new ();
    gtk_widget_show (hbuttonbox);
    gtk_table_attach (GTK_TABLE (table_button), hbuttonbox, 0, 1, 0, 1,
                     (GtkAttachOptions) GTK_EXPAND | GTK_FILL, (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);
    gtk_widget_set_usize (hbuttonbox, 219, -1);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_SPREAD);
    gtk_button_box_set_child_size (GTK_BUTTON_BOX (hbuttonbox), 72, 25);

    if (gtk_minor_version > 7)
        button_connect = gtk_button_new_from_stock ("gtk-connect");
    else
        button_connect = gtk_button_new_with_label ("Connect");
    
    gtk_signal_connect (GTK_OBJECT (button_connect), "clicked",
                        GTK_SIGNAL_FUNC (do_con), mud);
    gtk_signal_connect (GTK_OBJECT (button_connect), "clicked",
                        GTK_SIGNAL_FUNC (close_window), con_window);
    gtk_widget_show (button_connect);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button_connect);
    gtk_container_border_width (GTK_CONTAINER (button_connect), 3);

    button_cancel = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
    gtk_signal_connect (GTK_OBJECT(button_cancel), "clicked", 
                        GTK_SIGNAL_FUNC (close_window), con_window);
    gtk_widget_show (button_cancel);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button_cancel);
    gtk_container_border_width (GTK_CONTAINER (button_cancel), 3);

    gtk_widget_show (con_window);

}


void macro_send (GtkWidget *widget, gint button) {
    parse_input(macro_list[button], mud->activesession);
}

/*
 * This Function vil toggle the logger
 */
void toggle_logger (GtkWidget *menuitem, gpointer data)
{
    /* Toggle the toolbar logger button and do the stuff */
    gtk_toggle_button_set_state(GTK_TOGGLE_BUTTON(btn_toolbar_logger), GTK_CHECK_MENU_ITEM(menuitem)->active);
}

/* Change Macro buttons labels
 */
void macro_btnLabel_change() {
    gint i;
    
    for (i = 0; i < 12; i++) {
       gtk_label_set_text(GTK_LABEL(btnLabel[i]), macro_list[i]);
       gtk_misc_set_alignment (GTK_MISC(btnLabel[i]), LorC(macro_list[i]), 0.5);
    }
}

/*
 * This Function creates the main window
 */

static GtkWidget * add_menu(GtkWidget *father, const char *name)
{
  GtkWidget *menuitem = gtk_menu_item_new_with_label (name), *menu = gtk_menu_new();
  gtk_widget_show (menuitem);
  gtk_container_add (GTK_CONTAINER (father), menuitem);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), menu);

  return menu;
}

static GtkWidget * add_menu_item(GtkWidget *father, GtkAccelGroup *group, 
        const char *name, GtkSignalFunc cbk, gint acc_key, gint acc_mod)
{
    GtkWidget *item;
    
    if (name) {
        if (*name != '*')
            item = gtk_menu_item_new_with_label (name);
        else {
            name++;
            item = gtk_check_menu_item_new_with_label (name);            
        }
            
        gtk_signal_connect (GTK_OBJECT (item), "activate", cbk, NULL);

        if (group)
            gtk_widget_add_accelerator (item, "activate", group,
                    acc_key, acc_mod, GTK_ACCEL_VISIBLE);
    }
    else
        item = gtk_menu_item_new();

    gtk_widget_show (item);
    gtk_container_add (GTK_CONTAINER (father), item);

    return item;
}

static GtkWidget * add_stock_item(GtkWidget *father, GtkAccelGroup *group, 
        const char *icon, GtkSignalFunc cbk)
{
    GtkWidget *item;
    
    item = gtk_image_menu_item_new_from_stock (icon, group);
            
    gtk_signal_connect (GTK_OBJECT (item), "activate", cbk, NULL);

    gtk_widget_show (item);
    gtk_container_add (GTK_CONTAINER (father), item);

    return item;
}

#define add_separator(x) add_menu_item(x, NULL, NULL, NULL, 0, 0)

static GtkWidget * review_toggle;

void toggle_review(void)
{
    if (GTK_WIDGET_VISIBLE(mud->review)) {
        gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (review_toggle), FALSE);
        gtk_widget_hide(mud->review);
    }
    else {
        gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (review_toggle), TRUE);
        gtk_widget_show(mud->review);
    }
}

void
spawn_gui()
{
  gint i = 0;
  //gint key = GDK_F1; /* F1 key */
  GtkWidget *vbox1;
  GtkWidget *menubar;
  GtkWidget *menu;
  GtkWidget *vbox2;
  GtkWidget *hbox1;
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *macro_label;
  GtkAccelGroup *accel_group;
  GtkWidget *macro_button;
  GtkWidget *fill_block2;
  GtkWidget *hbox2;
  GtkWidget *fill_block;
  /*** ToolBar ***/
  GtkWidget *toolbar;
  GdkPixmap *icon;
  GdkBitmap *mask;
  GtkWidget *iconw;
  GtkWidget *btn_toolbar_connection_wizard,
            *btn_toolbar_font,
            *btn_toolbar_prefs,
            *btn_toolbar_colors,
            *btn_toolbar_alias,
            *btn_toolbar_macro,
            *btn_toolbar_trigger,
            *btn_toolbar_logview,
            *btn_toolbar_help;
  GtkWidget *toggle1;

  /*** ToolBar End ***/

  /* create the main window */
  mud->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_usize (mud->window, 640, 480);
  gtk_container_border_width (GTK_CONTAINER (mud->window), 3);
  gtk_window_set_title (GTK_WINDOW (mud->window), "GGMud "VERSION"");
//  gtk_window_set_policy (GTK_WINDOW (mud->window), FALSE, TRUE, FALSE);
  gtk_widget_realize ( mud->window );

  /* handlers so we can quit the close the app */
  gtk_signal_connect (GTK_OBJECT (mud->window), "delete_event", GTK_SIGNAL_FUNC (quit), NULL);
  gtk_signal_connect (GTK_OBJECT (mud->window), "destroy", GTK_SIGNAL_FUNC (quit), NULL);

  /* create the accel_group for the keyboard bindings */
  accel_group = gtk_accel_group_new ();
 
  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (mud->window), vbox1);

  /* menu bar */
  menubar = gtk_menu_bar_new ();
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar, FALSE, TRUE, 3);

  /* file menu */
  menu = add_menu(menubar, "File");
  
  add_menu_item(menu, accel_group, "Connection Wizard", GTK_SIGNAL_FUNC(do_wiz),  GDK_W, GDK_MOD1_MASK);
  add_separator(menu);


  if (gtk_minor_version > 7)
      menu_File_Connect = add_stock_item(menu, accel_group, "gtk-connect", 
              GTK_SIGNAL_FUNC(cbox));
  else
      menu_File_Connect = add_menu_item(menu, accel_group, "Connect", 
              GTK_SIGNAL_FUNC(cbox),  GDK_C, GDK_MOD1_MASK);
  
  if (gtk_minor_version > 7)
      menu_File_Connect = add_stock_item(menu, accel_group, "gtk-disconnect", 
              GTK_SIGNAL_FUNC(disconnect));
  else
      menu_File_DisConnect = add_menu_item(menu, accel_group, "Disconnect", 
              GTK_SIGNAL_FUNC(disconnect),  GDK_D, GDK_MOD1_MASK);

  gtk_widget_set_sensitive (menu_File_DisConnect, FALSE);
  menu_File_Reconnect = add_menu_item(menu, accel_group, "Reconnect", GTK_SIGNAL_FUNC(reconnect),  GDK_R, GDK_MOD1_MASK);
  add_separator(menu);
//  add_menu_item(menu, accel_group, "Quit", GTK_SIGNAL_FUNC(quit),  GDK_Q, GDK_MOD1_MASK);
  add_stock_item(menu, accel_group, GTK_STOCK_QUIT, GTK_SIGNAL_FUNC(quit));

  /* options menu */
  menu = add_menu (menubar, "Options");
  
//  add_menu_item(menu, accel_group, "Fonts", GTK_SIGNAL_FUNC(window_font),  GDK_F, GDK_MOD1_MASK);
//  add_menu_item(menu, accel_group, "Colors", GTK_SIGNAL_FUNC(color_prefs),  GDK_O, GDK_MOD1_MASK);
//  add_menu_item(menu, accel_group, "Preferences", GTK_SIGNAL_FUNC(window_prefs),  GDK_P, GDK_MOD1_MASK);
  add_stock_item(menu, accel_group, GTK_STOCK_SELECT_FONT, GTK_SIGNAL_FUNC(window_font));
  add_stock_item(menu, accel_group, GTK_STOCK_SELECT_COLOR, GTK_SIGNAL_FUNC(color_prefs));
  add_stock_item(menu, accel_group, GTK_STOCK_PREFERENCES, GTK_SIGNAL_FUNC(window_prefs));

  add_separator(menu);
  add_menu_item(menu, NULL, "Load TT++/GGMUD file...", GTK_SIGNAL_FUNC(load_tt_prefs), 0, 0);
  add_menu_item(menu, NULL, "Import ZMud configuration...", GTK_SIGNAL_FUNC(load_zmud_prefs), 0, 0);
  add_separator(menu);
  add_menu_item(menu, NULL, "Save window positions", GTK_SIGNAL_FUNC(save_win_pos), 0, 0);
  add_menu_item(menu, NULL, "Export settings", GTK_SIGNAL_FUNC(save_all_prefs), 0, 0);
  add_separator(menu);
  add_menu_item(menu, NULL, "Save review buffer...", GTK_SIGNAL_FUNC(save_review), 0, 0);

  /* tools menu */
  menu = add_menu (menubar, "Tools");
  
  add_menu_item(menu, accel_group, "Macros", GTK_SIGNAL_FUNC(window_macro),  GDK_M, GDK_MOD1_MASK);
  add_menu_item(menu, accel_group, "Aliases", GTK_SIGNAL_FUNC(window_alias),  GDK_A, GDK_MOD1_MASK);
  add_menu_item(menu, accel_group, "Triggers", GTK_SIGNAL_FUNC(triggers_window),  GDK_T, GDK_MOD1_MASK);
  add_menu_item(menu, NULL, "Highlights", GTK_SIGNAL_FUNC(highlights_window),  0, 0);
  add_menu_item(menu, accel_group, "Gags", GTK_SIGNAL_FUNC(gags_window),  GDK_G, GDK_MOD1_MASK);  
  add_menu_item(menu, NULL, "Tab complete", GTK_SIGNAL_FUNC(create_complete_window), 0, 0);
  add_menu_item(menu, accel_group, "Variables", GTK_SIGNAL_FUNC(variables_window),  GDK_V, GDK_MOD1_MASK);  
  add_separator(menu);
  menu_Tools_Logger = add_menu_item(menu, accel_group, "*Logger", GTK_SIGNAL_FUNC(toggle_logger),  GDK_L, GDK_MOD1_MASK);  
  gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM(menu_Tools_Logger), TRUE);
  add_menu_item(menu, NULL, "Log Viewer", GTK_SIGNAL_FUNC(log_viewer), 0, 0);
  add_separator(menu);
  add_menu_item(menu, NULL, "Enable/disable trigger classes", GTK_SIGNAL_FUNC(triggerclass_window), 0, 0);

  
  /* help menu */
  menu = add_menu (menubar, "Help");
  gtk_menu_item_right_justify (GTK_MENU_ITEM (gtk_menu_get_attach_widget(GTK_MENU(menu))));
  add_stock_item(menu, accel_group, GTK_STOCK_HELP, GTK_SIGNAL_FUNC(do_manual));
  add_separator(menu);
  add_menu_item(menu, accel_group, "About", GTK_SIGNAL_FUNC(do_about),  GDK_B, GDK_MOD1_MASK);

/*** ToolBar ***/
  /* we need to realize the window because we use pixmaps for 
   * items on the toolbar in the context of it */
 
  /* to make it nice we'll put the toolbar into the handle box, 
   * so that it can be detached from the main window */
  handlebox = gtk_handle_box_new ();
  gtk_box_pack_start ( GTK_BOX ( vbox1 ), handlebox, FALSE, FALSE, 2 );

  /* toolbar will be horizontal, with both icons and text, and
   * with 5pxl spaces between items and finally, 
   * we'll also put it into our handlebox */
  toolbar = gtk_toolbar_new ( );
  gtk_container_set_border_width ( GTK_CONTAINER ( toolbar ) , 5 );
  //gtk_toolbar_set_space_size ( GTK_TOOLBAR ( toolbar ), 5 );
  gtk_container_add ( GTK_CONTAINER ( handlebox ) , toolbar );

  /* our first item is <Connection Wizard> button */
  /* now we create icon with mask: we'll reuse it to create
   * icon widgets for toolbar items */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_wizard );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);

  btn_toolbar_connection_wizard = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), /* our toolbar */
                                           NULL,               			/* button label */
                                           "Connection Wizard (Alt+W)",     	/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                 		/* icon widget */
                                           GTK_SIGNAL_FUNC (do_wiz), 		/* a signal */
                                           NULL );
  gtk_toolbar_append_space ( GTK_TOOLBAR ( toolbar ) ); 			/* space after item */

  /* our next item is <Connect> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_connect );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);
  
  btn_toolbar_connect = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), 	/* our toolbar */
                                           NULL,               			/* button label */
                                           "Connect (Alt+C)",     		/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                 		/* icon widget */
                                           GTK_SIGNAL_FUNC (cbox), 		/* a signal */
                                           NULL );

  /* our next item is <DisConnect> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_disconnect );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  btn_toolbar_disconnect = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), 	/* our toolbar */
                                           NULL,               			/* button label */
                                           "Disconnect (Alt+D)",     		/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                		 /* icon widget */
                                           GTK_SIGNAL_FUNC (disconnect), 	/* a signal */
                                           NULL );
  gtk_toolbar_append_space ( GTK_TOOLBAR ( toolbar ) ); 			/* space after item */

  iconw = gtk_image_new_from_stock(GTK_STOCK_SELECT_FONT, GTK_ICON_SIZE_SMALL_TOOLBAR);
  
  btn_toolbar_font = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), 		/* our toolbar */
                                           NULL,               			/* button label */
                                           "Font (Alt+F)",     			/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                 		/* icon widget */
                                           GTK_SIGNAL_FUNC (window_font), 	/* a signal */
                                           NULL );

  iconw = gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_SMALL_TOOLBAR);

  btn_toolbar_colors = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar),		/* our toolbar */
                                           NULL,               			/* button label */
                                           "Colors (Alt+O)",   			/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                 		/* icon widget */
                                           GTK_SIGNAL_FUNC (color_prefs), 	/* a signal */
                                           NULL );

  iconw = gtk_image_new_from_stock(GTK_STOCK_PREFERENCES, GTK_ICON_SIZE_SMALL_TOOLBAR);

  btn_toolbar_prefs = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), 		/* our toolbar */
                                           NULL,               			/* button label */
                                           "Preferences (Alt+P)",     		/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                 		/* icon widget */
                                           GTK_SIGNAL_FUNC (window_prefs), 	/* a signal */
                                           NULL );
  gtk_toolbar_append_space ( GTK_TOOLBAR ( toolbar ) ); 			/* space after item */

  /* our next item is <Alias> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_alias );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);

  btn_toolbar_alias = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), 		/* our toolbar */
                                           NULL,               			/* button label */
                                           "Alias (Alt+A)",     		/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                 		/* icon widget */
                                           GTK_SIGNAL_FUNC (window_alias), 	/* a signal */
                                           NULL );

  /* our next item is <macros> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_macros );
  iconw = gtk_pixmap_new ( icon, mask );				 	/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);

  btn_toolbar_macro = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), 		/* our toolbar */
                                           NULL,               			/* button label */
                                           "Macros (Alt+M)",    		/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                 		/* icon widget */
                                           GTK_SIGNAL_FUNC (window_macro), 	/* a signal */
                                           NULL );

  /* our next item is <trigger> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_triggers );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);

  btn_toolbar_trigger = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar),  	/* our toolbar */
                                           NULL,           		    	/* button label */
                                           "Triggers (Alt+T)",		   	/* this button's tooltip */
                                           "Private",             	    	/* tooltip private info */
                                           iconw,                 	    	/* icon widget */
                                           GTK_SIGNAL_FUNC (triggers_window),  	/* a signal */
                                           NULL );

  gtk_toolbar_append_space ( GTK_TOOLBAR ( toolbar ) ); 		    	/* space after item */

  /* our next item is <Logger> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_logger );
  iconw = gtk_pixmap_new ( icon, mask ); /* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);
  
  btn_toolbar_logger = gtk_toolbar_append_element(GTK_TOOLBAR(toolbar),
                                               GTK_TOOLBAR_CHILD_TOGGLEBUTTON,	/* a type of element */
                                               NULL,				/* pointer to widget */
                                               NULL,				/* label */
                                               "Logger (Alt+L)",		/* tooltip */
                                               "Private",			/* tooltip private string */
                                               iconw,				/* icon */
                                               GTK_SIGNAL_FUNC (do_log),	/* signal */
                                               toolbar);
  //gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tooltips_button), FALSE);

  /* our next item is <LogViewer> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_logviewer );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);

  btn_toolbar_logview = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), 	/* our toolbar */
                                           NULL,              		 	/* button label */
                                           "Log Viewer (Alt+V)",     	 	/* this button's tooltip */
                                           "Private",             	 	/* tooltip private info */
                                           iconw,                 	 	/* icon widget */
                                           GTK_SIGNAL_FUNC (log_viewer), 	/* a signal */
                                           NULL );
  gtk_toolbar_append_space ( GTK_TOOLBAR ( toolbar ) );			 	/* space after item */

  iconw = gtk_image_new_from_stock(GTK_STOCK_HELP, GTK_ICON_SIZE_SMALL_TOOLBAR);

  btn_toolbar_help = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), 		/* our toolbar */
                                           NULL,               			/* button label */
                                           "Help (Alt+H)",     			/* this button's tooltip */
                                           "Private",          			/* tooltip private info */
                                           iconw,      		        	/* icon widget */
                                           GTK_SIGNAL_FUNC (do_manual),		/* a signal */
                                           NULL );

  /* sets the disconnect button unsensitive on startup */
  gtk_widget_set_sensitive (btn_toolbar_disconnect, FALSE);

  /* that's it ! let's show everything. */
  gtk_widget_show ( toolbar );

  if (prefs.Toolbar)
      gtk_widget_show (handlebox);
  else 
      gtk_widget_hide (handlebox);

/*** ToolBar End ***/

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox2, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox1, TRUE, TRUE, 0);

  mud->macrobuttons = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox1), mud->macrobuttons, FALSE, TRUE, 0);

  if (prefs.Macrobuttons)
      gtk_widget_show (mud->macrobuttons);
  else 
      gtk_widget_hide (mud->macrobuttons);

  /* Macro frame */
  frame = gtk_frame_new ("Macros");
  gtk_widget_show (frame);
  gtk_box_pack_start (GTK_BOX (mud->macrobuttons), frame, FALSE, FALSE, 0);
  gtk_container_border_width (GTK_CONTAINER (frame), 3);
  gtk_frame_set_label_align (GTK_FRAME (frame), 0.03, 0.5);

  table = gtk_table_new (12, 2, FALSE);
  gtk_widget_show (table);
  gtk_container_add (GTK_CONTAINER (frame), table);

  /* Macro buttons */
  while (keys[i]) {
      /* the labels that are beside the macro buttons */
      macro_label = gtk_label_new (keys[i]);
      gtk_widget_show (macro_label);
      gtk_table_attach (GTK_TABLE (table), macro_label, 0, 1, i, i+1,
                       (GtkAttachOptions) GTK_EXPAND | GTK_FILL,
		       (GtkAttachOptions) GTK_EXPAND | GTK_FILL, 0, 0);
      /* Macro buttons with accel. keys F1-F12 */
      macro_button = gtk_button_new();
      gtk_widget_show (macro_button);
      gtk_table_attach (GTK_TABLE (table), macro_button, 1, 2, i, i+1,
                       (GtkAttachOptions) GTK_EXPAND | GTK_FILL,
		       (GtkAttachOptions) GTK_EXPAND, 0, 0);
      gtk_widget_set_usize (macro_button, 65, 26);
      gtk_container_border_width (GTK_CONTAINER (macro_button), 3);
      gtk_signal_connect (GTK_OBJECT (macro_button), "clicked",
                          GTK_SIGNAL_FUNC (macro_send), (gpointer) i);
#if 0      
      gtk_widget_add_accelerator (macro_button, "clicked", accel_group,
                                  key++, 0, 0 /*GTK_ACCEL_VISIBLE*/);
#endif
      btnLabel[i] = gtk_label_new(macro_list[i]);
      gtk_container_add (GTK_CONTAINER(macro_button), btnLabel[i]);
      gtk_widget_show(btnLabel[i]);
      i++;
  }
  /* just a empty label we use as a fillblock so it should look nice */
  fill_block2 = gtk_label_new ("");
  gtk_widget_show (fill_block2);
  gtk_box_pack_start (GTK_BOX (mud->macrobuttons), fill_block2, FALSE, TRUE, 0);

#ifdef USE_NOTEBOOK
  mud->notebook = (GtkNotebook *)gtk_notebook_new();
  gtk_widget_show(GTK_WIDGET(mud->notebook));
  gtk_box_pack_start(GTK_BOX(hbox1), GTK_WIDGET(mud->notebook), TRUE, TRUE, 0);
  mud->text = new_view("not connected", mud->notebook, TRUE);        
#else
  mud->text = new_view("not connected", hbox1,  TRUE);
#endif
  
  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox2), hbox2, FALSE, TRUE, 3);


  /* the entry box, that we use to type the commands in */
  mud->ent = (GtkEntry *)gtk_entry_new ();
  gtk_signal_connect_object (GTK_OBJECT (mud->ent), "activate", GTK_SIGNAL_FUNC (send_to_connection), NULL);
  gtk_signal_connect(GTK_OBJECT(mud->ent),"key_press_event",GTK_SIGNAL_FUNC(hist_evt), mud);
  gtk_widget_show (GTK_WIDGET(mud->ent));
  gtk_box_pack_start (GTK_BOX (hbox2), GTK_WIDGET(mud->ent), TRUE, TRUE, 8);
  GTK_WIDGET_SET_FLAGS (GTK_WIDGET(mud->ent), GTK_CAN_DEFAULT);
  gtk_widget_grab_focus (GTK_WIDGET(mud->ent));
  gtk_widget_grab_default (GTK_WIDGET(mud->ent));

  
  // toggle triggers & parsing buttons
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_triggers );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);
  gtk_widget_show(iconw);
  
  toggle1 = gtk_toggle_button_new();
  gtk_container_add(GTK_CONTAINER(toggle1), iconw);
  gtk_widget_show(toggle1);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (toggle1), TRUE);
  GTK_WIDGET_UNSET_FLAGS (toggle1, GTK_CAN_FOCUS|GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (toggle1), "toggled",
                       GTK_SIGNAL_FUNC (toggle_triggers), NULL);
  gtk_box_pack_start (GTK_BOX (hbox2), toggle1, FALSE, FALSE, 4);

  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_parsing );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);
  gtk_widget_show(iconw);

  toggle1 = gtk_toggle_button_new();
  gtk_container_add(GTK_CONTAINER(toggle1), iconw);
  gtk_widget_show(toggle1);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (toggle1), TRUE);
  GTK_WIDGET_UNSET_FLAGS (toggle1, GTK_CAN_FOCUS|GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (toggle1), "toggled",
                       GTK_SIGNAL_FUNC (toggle_parsing), NULL);
  gtk_box_pack_start (GTK_BOX (hbox2), toggle1, FALSE, FALSE, 4);

  iconw = gtk_image_new_from_stock(GTK_STOCK_JUSTIFY_LEFT, GTK_ICON_SIZE_BUTTON);
  review_toggle = toggle1 = gtk_toggle_button_new();
  gtk_container_add(GTK_CONTAINER(toggle1), iconw);
  gtk_widget_show_all(toggle1);
  gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (toggle1), FALSE);
  GTK_WIDGET_UNSET_FLAGS (toggle1, GTK_CAN_FOCUS|GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (toggle1), "toggled",
                       GTK_SIGNAL_FUNC (toggle_review), NULL);
  gtk_box_pack_start (GTK_BOX (hbox2), toggle1, FALSE, FALSE, 4);

  // tick counter
  fill_block = gtk_frame_new ("Tick");
  mud->tick_counter = (GtkLabel *)gtk_label_new("OFF");
  gtk_widget_show(GTK_WIDGET(mud->tick_counter));
  gtk_container_add(GTK_CONTAINER(fill_block), GTK_WIDGET(mud->tick_counter));
  gtk_widget_show (fill_block);
  gtk_box_pack_start (GTK_BOX (hbox2), fill_block, FALSE, TRUE, 4);

  
  /* status bar wich can be used for displaying information */
  statusbar = gtk_statusbar_new ();
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar, FALSE, TRUE, 0);
  gtk_container_border_width (GTK_CONTAINER (statusbar), 3);

  if (prefs.Statusbar)
      gtk_widget_show (statusbar);
  else 
      gtk_widget_hide (statusbar);

  gtk_window_add_accel_group (GTK_WINDOW (mud->window), accel_group);
  
  statusbar_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "Statusbar");
  gtk_statusbar_push (GTK_STATUSBAR(statusbar), statusbar_id, "Ready");
  macro_btnLabel_change();

}
/* FOR THE WINDOW BUFFER FUNCTION */
void clear_backbuffer()
{
    if (mud->maxlines > 0) {
        GtkTextIter start, end;
        GtkTextBuffer *b = gtk_text_view_get_buffer(mud->text);
        int n = gtk_text_buffer_get_line_count(b);
        
        if( n < mud->maxlines)
            return;

        gtk_text_buffer_get_start_iter(b, &start);
        gtk_text_buffer_get_iter_at_line_offset(b, &end, mud->maxlines / 20, 0);
        gtk_text_buffer_delete(b, &start, &end); 
    }
}

void textfield_freeze()
{
}

void textfield_unfreeze()
{
    GtkTextMark *mark = gtk_object_get_user_data(GTK_OBJECT(mud->text));
    
    if (!mark)
        return;

    clear_backbuffer();
  
    gtk_text_view_scroll_mark_onscreen(mud->text, mark);
}

void textfield_add(GtkTextView *txt, const char *message, int colortype)
{
    int numbytes;
    GtkTextIter iter;
    GtkTextBuffer *tbuf;
    GtkTextMark *mark;
    GtkTextTag *tag;
    extern GtkTextTag *fg_colors[2][8];
    extern GtkTextTag *blink_colors[2][8];

    
    if (!*message)
        return;
    
    switch (colortype) {
        case MESSAGE_LOCAL:
            numbytes = strlen(message);
            
            local_disp_ansi(numbytes, message, txt);
            
            if (txt != mud->text) {
                if ((mark = gtk_object_get_user_data(GTK_OBJECT(txt))))
                   gtk_text_view_scroll_mark_onscreen(txt, mark);
            }
            return;
        case MESSAGE_ANSI:
            numbytes = strlen(message);
            
            disp_ansi(numbytes, message, txt);

            if (txt != mud->text) {
                if ((mark = gtk_object_get_user_data(GTK_OBJECT(txt))))
                    gtk_text_view_scroll_mark_onscreen(txt, mark);
            }
            return;
        case MESSAGE_SENT:
            if (mud->LOGGING) /* Loging */
                fprintf(mud->LOG_FILE, message);
            tag = fg_colors[1][3];// light yellow 
            break;
        case MESSAGE_ERR:
            if (mud->LOGGING) /* Loging */
                fprintf(mud->LOG_FILE, message);
            tag = fg_colors[0][1]; // red
            break;
        case MESSAGE_TICK:
            tag = blink_colors[1][6]; // cyan
            break;
        default:
            if (mud->LOGGING) /* Loging */
                fprintf(mud->LOG_FILE, message);
            tag = prefs.DefaultColor;
    }
    
    tbuf = gtk_text_view_get_buffer(txt);
    gtk_text_buffer_get_end_iter(tbuf, &iter);
    gtk_text_buffer_insert_with_tags(tbuf, &iter, message, -1, tag, NULL);

    if ((mark = gtk_object_get_user_data(GTK_OBJECT(txt))))
        gtk_text_view_scroll_mark_onscreen(txt, mark);
}


	/* n is the number of lines to NOT delete, if it's 0, delete 'em all */
void clear(int n, GtkTextView *target)
{
    GtkTextBuffer *b = (GtkTextBuffer *)gtk_text_view_get_buffer(target);

    gtk_text_buffer_set_text(b, "", -1);
}	

#if 0
void popup_window (const gchar *message, ...)
{
    va_list va;
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *button;
    GtkWidget *box;
    GtkWidget *separator, *image, *hbox;

    gchar       buf[3072];

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Popup Message");

    box = gtk_vbox_new (FALSE, 3);
    hbox = gtk_hbox_new (FALSE, 2);
    gtk_container_set_border_width (GTK_CONTAINER (box), 5);
    gtk_container_add (GTK_CONTAINER (window), box);

    va_start(va, message);
    *buf = ' ';
    vsprintf(buf + 1, message, va);
    strcat(buf, " ");
    va_end(va);

    label = gtk_label_new (buf);
    image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_WARNING, 
            GTK_ICON_SIZE_DIALOG);
    
    gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 5);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 5);
    gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 5);
 
    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (box), separator, TRUE, TRUE, 0);
 
    button = gtk_button_new_from_stock (GTK_STOCK_OK);
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
            GTK_SIGNAL_FUNC (close_window),
            window);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, TRUE, 5);
    gtk_widget_show_all (window);
}
#else
void popup_window (int type, const gchar *message, ...)
{
    va_list va;
    gchar       buf[3072];
    GtkWidget *d;

    va_start(va, message);
    vsprintf(buf, message, va);
    va_end(va);

    d = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                           type, GTK_BUTTONS_OK,
                           message);

    gtk_dialog_run(GTK_DIALOG(d));
   
    gtk_widget_destroy(d);
}
#endif

static 
GtkWidget *create_tv(GtkTextBuffer *buffer, GtkTextView **view)
{
    extern PangoFontDescription *font_normal; 
    GtkTextView *text;
    GtkTextIter it;
    GtkTextMark *mark;
    
    GtkWidget *sw = gtk_scrolled_window_new (NULL, NULL);

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
            GTK_POLICY_AUTOMATIC,
            GTK_POLICY_ALWAYS);
    gtk_widget_show(sw);
    text = (GtkTextView *)gtk_text_view_new_with_buffer (buffer);
    gtk_container_add(GTK_CONTAINER(sw), (GtkWidget *)text);
    gtk_widget_show((GtkWidget *)text);

    gtk_widget_modify_base((GtkWidget *)text, GTK_STATE_NORMAL, &prefs.BackgroundGdkColor);
    gtk_widget_modify_text ((GtkWidget *)text, GTK_STATE_NORMAL, &prefs.DefaultGdkColor); 

    gtk_text_view_set_wrap_mode(text, 
            prefs.WordWrap ? GTK_WRAP_CHAR : GTK_WRAP_NONE);

    gtk_text_view_set_cursor_visible(text, FALSE);
    gtk_text_view_set_left_margin(text, 3);

    *view = text;

    gtk_text_buffer_get_end_iter(buffer, &it);
    mark = gtk_text_buffer_create_mark(buffer, NULL, &it, FALSE);
    gtk_object_set_user_data(GTK_OBJECT(text), mark);
   
    if (font_normal)
        gtk_widget_modify_font((GtkWidget *)text, font_normal);

    gtk_signal_connect(GTK_OBJECT(text),"key_press_event",GTK_SIGNAL_FUNC(change_focus), mud);

    return sw;
}

GtkTextView *new_view(char *name, GtkWidget *parent, int ismain)
{
  GtkWidget *paned, *sw;
  GtkTextView *t1, *t2;
  GtkTextBuffer *buf;
  int w, h;
  extern GtkTextTagTable *tag_table;

  paned = gtk_vpaned_new();
  gtk_widget_show (paned);
  
#ifdef USE_NOTEBOOK
      gtk_container_add(GTK_CONTAINER(parent), paned);

      templabel = gtk_label_new(name);
      gtk_widget_show(templabel);
      
      gtk_notebook_set_tab_label (mud->notebook, 
              gtk_notebook_get_nth_page(mud->notebook, 0), templabel);
#else
      gtk_box_pack_start(GTK_BOX(parent), paned, TRUE, TRUE, 0);
#endif
     
  buf = gtk_text_buffer_new(tag_table);

  sw = create_tv(buf, &t1);
  gtk_widget_hide(sw);
  gtk_paned_add1(GTK_PANED(paned), sw);
  
  gtk_paned_add2(GTK_PANED(paned), create_tv(buf, &t2));

  gtk_window_get_size(GTK_WINDOW(gtk_widget_get_toplevel(parent)), &w, &h);
  gtk_paned_set_position(GTK_PANED(paned), h * 2 / 3);
      
  if (ismain) 
    mud->review = sw;
  
  return (GtkTextView *)t2;
}

