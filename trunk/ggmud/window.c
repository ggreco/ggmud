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

GList *windows_list = NULL;

typedef struct {
    char name[32];
    GtkWidget *listptr;    
} window_entry;

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

GtkText *new_view(char *name, GtkWidget *parent);

GtkWidget *create_new_window(char *title)
{
    GtkWidget *win, *vbox;
    GtkWidget *list;

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize(win, 400, 300);
    gtk_window_set_title(GTK_WINDOW(win), title);
    gtk_container_border_width(GTK_CONTAINER(win), 4);

    gtk_widget_realize(win);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show(vbox);

    gtk_container_add(GTK_CONTAINER(win), vbox);
    
    list = (GtkWidget *) new_view(NULL, vbox);
    gtk_signal_connect (GTK_OBJECT (list), "destroy", GTK_SIGNAL_FUNC (destroy_a_window), NULL);

    gtk_widget_show(win);

    return list;
}

window_entry *create_new_entry(char *title)
{
    window_entry *entry;
    
    if(!(entry = malloc(sizeof(window_entry))))
        return NULL;

    strncpy(entry->name, title, 31);

    if((entry->listptr = create_new_window(title)))
        windows_list = g_list_append(windows_list, entry);
    else {
        free(entry);
        return NULL;
    }

    return entry;
}

void clear_text_widget(GtkText *w)
{
    int l = gtk_text_get_length(w);

    gtk_editable_delete_text(GTK_EDITABLE(w), 0, l);
}

void clr_command(char *arg, struct session *s)
{
    extern char *get_arg_in_braces(char *s, char *arg, int flag);
    char left[BUFFER_SIZE];
    window_entry *entry = NULL;
    
    arg = get_arg_in_braces(arg, left, 0);

    if(!*left) {
        clear_text_widget(mud->text);
    }
    else {
        if(!(entry = in_window_list(left))) {
            create_new_entry(left); // creo la nuova finestra nel caso non ci sia
        }
        else
            clear_text_widget(GTK_TEXT(entry->listptr));
    }
}

void mess_command(char *arg, struct session *s)
{
    extern char *get_arg_in_braces(char *s, char *arg, int flag);
    char left[BUFFER_SIZE], right[BUFFER_SIZE];

    arg = get_arg_in_braces(arg, left, 0);
    arg = get_arg_in_braces(arg, right, 1);

    if(!*left || !*right)
        return;
   
    substitute_myvars(right, left, s);
    substitute_vars(left, right, s);

    popup_window(right);
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
        entry = create_new_entry(left);
    }

    if(right && *right && entry) {
        char *result;
        substitute_myvars(right, left, s);
        substitute_vars(left, right, s);
        result = ParseAnsiColors(right);
        strcat(result, "\n");
        textfield_add((GtkText *)entry->listptr, result , MESSAGE_ANSI);
    }
}

GtkWidget *
MakeButton(char *name, char **image, GtkSignalFunc func, gpointer data, GtkAccelGroup *accel_group)
{
    GdkPixmap *icon;
    GdkBitmap *mask;
    GtkWidget *hbox, *label, *button;
    guint key;
 
    button = gtk_button_new();
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox);
    gtk_container_add(GTK_CONTAINER(button), hbox);

    if (icon = gdk_pixmap_colormap_create_from_xpm_d ( NULL, cmap, 
            &mask, NULL, image )) {
        GtkWidget *pixmap = gtk_pixmap_new ( icon, mask ); 					/* icon widget */

        gdk_pixmap_unref(icon);
        gdk_bitmap_unref(mask);
        
        gtk_widget_show(pixmap);
        gtk_box_pack_start (GTK_BOX (hbox), pixmap, FALSE, FALSE, 0);
    }    

    label = gtk_label_new("");
    key = gtk_label_parse_uline(GTK_LABEL(label), name);
    gtk_widget_show(label);
    gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, FALSE, 0);
    
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                               func,
                               data );
    
    gtk_widget_add_accelerator (button, "clicked", accel_group,
                              key, GDK_MOD1_MASK,
                              0);
    

    gtk_widget_show(button);

    return button;
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

    
    button_add    = MakeButton("_Add", new_xpm, add_func, data, accel_group);    
    button_delete = MakeButton("_Delete", remove_xpm, del_func, data, accel_group);
    button_save   = MakeButton("_Save", save_xpm, save_func, data, accel_group);
    button_quit   = MakeButton("_Close", cross_xpm,
            GTK_SIGNAL_FUNC(close_window), 
            gtk_widget_get_toplevel(vbox),
            accel_group);

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
    GtkWidget *iconw;

    if(gtk_toggle_button_get_active(togglebutton)) {
        textfield_add(mud->text, "# PARSING ENABLED\n", MESSAGE_SENT);
        verbatim = 0;
    }
    else {
        textfield_add(mud->text, "# PARSING DISABLED\n", MESSAGE_SENT);
        verbatim = 1;
        image = SC_parsingoff;
    }

    if (icon = gdk_pixmap_create_from_xpm_d ( GTK_WIDGET(togglebutton)->window, 
            &mask, NULL, image )) {
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
    GtkWidget *iconw;

    if(gtk_toggle_button_get_active(togglebutton)) {
        textfield_add(mud->text, "# TRIGGERS ENABLED\n", MESSAGE_SENT);
        use_triggers = 1;
    }
    else {
        textfield_add(mud->text, "# TRIGGERS DISABLED\n", MESSAGE_SENT);
        use_triggers = 0;
        image = SC_triggersoff;
    }

    if (icon = gdk_pixmap_create_from_xpm_d ( GTK_WIDGET(togglebutton)->window, 
            &mask, NULL, image )) {
        gtk_pixmap_set (GTK_PIXMAP(GTK_BIN(togglebutton)->child), 
                icon, mask ); 					/* icon widget */

        gdk_pixmap_unref(icon);
        gdk_bitmap_unref(mask);
    }    
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
    char *port;
    char *host;
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

    button_connect = gtk_button_new_with_label ("Connect");
    gtk_signal_connect (GTK_OBJECT (button_connect), "clicked",
                        GTK_SIGNAL_FUNC (do_con), mud);
    gtk_signal_connect (GTK_OBJECT (button_connect), "clicked",
                        GTK_SIGNAL_FUNC (close_window), con_window);
    gtk_widget_show (button_connect);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button_connect);
    gtk_container_border_width (GTK_CONTAINER (button_connect), 3);

    button_cancel = gtk_button_new_with_label ("Cancel");
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
void
spawn_gui()
{
  gint i = 0;
  gint key = 0xFFBE; /* F1 key */
  GtkWidget *vbox1;
  GtkWidget *menubar;
  GtkWidget *menu_File;
  GtkWidget *menu_File_menu;
  GtkWidget *menu_File_Connection_Wizard;
  GtkWidget *separator1;
  GtkWidget *separator2;
  GtkWidget *separator3;
  GtkWidget *menu_File_Quit;
  GtkWidget *menu_Options;
  GtkWidget *menu_Options_menu;
  GtkWidget *menu_Options_Fonts;
  GtkWidget *menu_Options_Colors;
  GtkWidget *menu_Options_Preference;
  GtkWidget *menu_Tools;
  GtkWidget *menu_Tools_menu;
  GtkWidget *menu_Tools_Macro;
  GtkWidget *menu_Tools_Alias;
  GtkWidget *menu_Tools_Trigger;
  GtkWidget *menu_Tools_LogViewer;
  GtkWidget *menu_Help;
  GtkWidget *menu_Help_menu;
  GtkWidget *menu_Help_About;
  GtkWidget *menu_Help_Manual;
  GtkWidget *vbox2;
  GtkWidget *hbox1;
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *macro_label;
  GtkAccelGroup *accel_group;
  GtkWidget *macro_button;
  GtkWidget *fill_block2;
  GtkWidget *vscrollbar;
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
  gtk_widget_set_usize (mud->window, 695, 480);
  gtk_container_border_width (GTK_CONTAINER (mud->window), 3);
  gtk_window_set_title (GTK_WINDOW (mud->window), "GGMud "VERSION"");
  gtk_window_set_policy (GTK_WINDOW (mud->window), FALSE, TRUE, FALSE);

  /* handlers so we can quit the close the app */
  gtk_signal_connect (GTK_OBJECT (mud->window), "delete_event", GTK_SIGNAL_FUNC (quit), NULL);
  gtk_signal_connect (GTK_OBJECT (mud->window), "destroy", GTK_SIGNAL_FUNC (quit), NULL);

  /* create the accel_group for the keyboard bindings */
  accel_group = gtk_accel_group_new ();
  gtk_window_add_accel_group (GTK_WINDOW (mud->window), accel_group);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (mud->window), vbox1);

  /* menu bar */
  menubar = gtk_menu_bar_new ();
  gtk_widget_show (menubar);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar, FALSE, TRUE, 3);

  /* file menu */
  menu_File = gtk_menu_item_new_with_label ("File");
  gtk_widget_show (menu_File);
  gtk_container_add (GTK_CONTAINER (menubar), menu_File);

  menu_File_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_File), menu_File_menu);

  menu_File_Connection_Wizard = gtk_menu_item_new_with_label ("Connection Wizard");
  gtk_widget_show (menu_File_Connection_Wizard);
  gtk_container_add (GTK_CONTAINER (menu_File_menu), menu_File_Connection_Wizard);
  gtk_signal_connect (GTK_OBJECT (menu_File_Connection_Wizard), "activate",
                      GTK_SIGNAL_FUNC (do_wiz),
                      NULL);
  gtk_widget_add_accelerator (menu_File_Connection_Wizard, "activate", accel_group,
                              GDK_W, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  separator1 = gtk_menu_item_new ();
  gtk_widget_show (separator1);
  gtk_container_add (GTK_CONTAINER (menu_File_menu), separator1);

  menu_File_Connect = gtk_menu_item_new_with_label ("Connect");
  gtk_widget_show (menu_File_Connect);
  gtk_container_add (GTK_CONTAINER (menu_File_menu), menu_File_Connect);
  gtk_signal_connect (GTK_OBJECT (menu_File_Connect), "activate",
                      GTK_SIGNAL_FUNC (cbox),
                      NULL);
  gtk_widget_add_accelerator (menu_File_Connect, "activate", accel_group,
                              GDK_C, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  menu_File_DisConnect = gtk_menu_item_new_with_label ("DisConnect");
  gtk_widget_show (menu_File_DisConnect);
  gtk_container_add (GTK_CONTAINER (menu_File_menu), menu_File_DisConnect);
  gtk_signal_connect (GTK_OBJECT (menu_File_DisConnect), "activate",
                      GTK_SIGNAL_FUNC (disconnect),
                      NULL);
  gtk_widget_add_accelerator (menu_File_DisConnect, "activate", accel_group,
                              GDK_D, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  separator2 = gtk_menu_item_new ();
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (menu_File_menu), separator2);

  menu_File_Quit = gtk_menu_item_new_with_label ("Quit");
  gtk_widget_show (menu_File_Quit);
  gtk_container_add (GTK_CONTAINER (menu_File_menu), menu_File_Quit);
  gtk_signal_connect (GTK_OBJECT (menu_File_Quit), "activate",
                      GTK_SIGNAL_FUNC (quit),
                      NULL);
  gtk_widget_add_accelerator (menu_File_Quit, "activate", accel_group,
                              GDK_Q, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  gtk_widget_set_sensitive (menu_File_DisConnect, FALSE);

  /* options menu */
  menu_Options = gtk_menu_item_new_with_label ("Options");
  gtk_widget_show (menu_Options);
  gtk_container_add (GTK_CONTAINER (menubar), menu_Options);

  menu_Options_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_Options), menu_Options_menu);

  menu_Options_Fonts = gtk_menu_item_new_with_label ("Fonts");
  gtk_widget_show (menu_Options_Fonts);
  gtk_container_add (GTK_CONTAINER (menu_Options_menu), menu_Options_Fonts);
  gtk_signal_connect (GTK_OBJECT (menu_Options_Fonts), "activate",
                      GTK_SIGNAL_FUNC (window_font),
                      NULL);
  gtk_widget_add_accelerator (menu_Options_Fonts, "activate", accel_group,
                              GDK_F, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  menu_Options_Colors = gtk_menu_item_new_with_label ("Colors");
  gtk_widget_show (menu_Options_Colors);
  gtk_container_add (GTK_CONTAINER (menu_Options_menu), menu_Options_Colors);
  gtk_signal_connect (GTK_OBJECT (menu_Options_Colors), "activate",
                      GTK_SIGNAL_FUNC (color_prefs),
                      NULL);
  gtk_widget_add_accelerator (menu_Options_Colors, "activate", accel_group,
                              GDK_O, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

    menu_Options_Preference = gtk_menu_item_new_with_label ("Preferences");
  gtk_widget_show (menu_Options_Preference);
  gtk_container_add (GTK_CONTAINER (menu_Options_menu), menu_Options_Preference);
  gtk_signal_connect (GTK_OBJECT (menu_Options_Preference), "activate",
                      GTK_SIGNAL_FUNC (window_prefs),
                      NULL);
  gtk_widget_add_accelerator (menu_Options_Preference, "activate", accel_group,
                              GDK_P, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  separator2 = gtk_menu_item_new ();
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (menu_Options_menu), separator2);

  separator2 = gtk_menu_item_new_with_label ("Load TT++ command file...");
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (menu_Options_menu), separator2);
  gtk_signal_connect (GTK_OBJECT (separator2), "activate",
                      GTK_SIGNAL_FUNC (load_tt_prefs),
                      NULL);
  separator2 = gtk_menu_item_new_with_label ("Import ZMud configuration...");
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (menu_Options_menu), separator2);
  gtk_signal_connect (GTK_OBJECT (separator2), "activate",
                      GTK_SIGNAL_FUNC (load_tt_prefs),
                      NULL);
  separator2 = gtk_menu_item_new_with_label ("Save settings");
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (menu_Options_menu), separator2);
  gtk_signal_connect (GTK_OBJECT (separator2), "activate",
                      GTK_SIGNAL_FUNC (save_all_prefs),
                      NULL);

  

  /* tools menu */
  menu_Tools = gtk_menu_item_new_with_label ("Tools");
  gtk_widget_show (menu_Tools);
  gtk_container_add (GTK_CONTAINER (menubar), menu_Tools);

  menu_Tools_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_Tools), menu_Tools_menu);

  menu_Tools_Macro = gtk_menu_item_new_with_label ("Macros");
  gtk_widget_show (menu_Tools_Macro);
  gtk_container_add (GTK_CONTAINER (menu_Tools_menu), menu_Tools_Macro);
  gtk_signal_connect (GTK_OBJECT (menu_Tools_Macro), "activate",
                      GTK_SIGNAL_FUNC (window_macro),
                      NULL);
  gtk_widget_add_accelerator (menu_Tools_Macro, "activate", accel_group,
                              GDK_M, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);


  menu_Tools_Alias = gtk_menu_item_new_with_label ("Aliases");
  gtk_widget_show (menu_Tools_Alias);
  gtk_container_add (GTK_CONTAINER (menu_Tools_menu), menu_Tools_Alias);
  gtk_signal_connect (GTK_OBJECT (menu_Tools_Alias), "activate",
                      GTK_SIGNAL_FUNC (window_alias),
                      NULL);
  gtk_widget_add_accelerator (menu_Tools_Alias, "activate", accel_group,
                              GDK_A, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  menu_Tools_Trigger = gtk_menu_item_new_with_label ("Triggers");
  gtk_widget_show (menu_Tools_Trigger);
  gtk_container_add (GTK_CONTAINER (menu_Tools_menu), menu_Tools_Trigger);
  gtk_signal_connect (GTK_OBJECT (menu_Tools_Trigger), "activate",
                      GTK_SIGNAL_FUNC (triggers_window),
		      NULL);
  gtk_widget_add_accelerator (menu_Tools_Trigger, "activate", accel_group,
                              GDK_T, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  
  menu_Tools_Highlight = gtk_menu_item_new_with_label ("Highlights");
  gtk_widget_show (menu_Tools_Highlight);
  gtk_container_add (GTK_CONTAINER (menu_Tools_menu), menu_Tools_Highlight);
  gtk_signal_connect (GTK_OBJECT (menu_Tools_Highlight), "activate",
                      GTK_SIGNAL_FUNC (highlights_window),
		      NULL);
  gtk_widget_add_accelerator (menu_Tools_Highlight, "activate", accel_group,
                              GDK_H, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
  
  menu_Tools_Gag = gtk_menu_item_new_with_label ("Gags");
  gtk_widget_show (menu_Tools_Gag);
  gtk_container_add (GTK_CONTAINER (menu_Tools_menu), menu_Tools_Gag);
  gtk_signal_connect (GTK_OBJECT (menu_Tools_Gag), "activate",
                      GTK_SIGNAL_FUNC (gags_window),
		      NULL);
  gtk_widget_add_accelerator (menu_Tools_Gag, "activate", accel_group,
                              GDK_G, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
  
  menu_Tools_Variable = gtk_menu_item_new_with_label ("Variables");
  gtk_widget_show (menu_Tools_Variable);
  gtk_container_add (GTK_CONTAINER (menu_Tools_menu), menu_Tools_Variable);
  gtk_signal_connect (GTK_OBJECT (menu_Tools_Variable), "activate",
                      GTK_SIGNAL_FUNC (variables_window),
		      NULL);
  gtk_widget_add_accelerator (menu_Tools_Variable, "activate", accel_group,
                              GDK_V, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  separator2 = gtk_menu_item_new ();
  gtk_widget_show (separator2);
  gtk_container_add (GTK_CONTAINER (menu_Tools_menu), separator2);

  menu_Tools_Logger = gtk_check_menu_item_new_with_label ("Logger");
  gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM(menu_Tools_Logger), TRUE);
  gtk_widget_show (menu_Tools_Logger);
  gtk_container_add (GTK_CONTAINER (menu_Tools_menu), menu_Tools_Logger);
  gtk_signal_connect (GTK_OBJECT (menu_Tools_Logger), "activate",
                      GTK_SIGNAL_FUNC (toggle_logger),
                      NULL);
  gtk_widget_add_accelerator (menu_Tools_Logger, "activate", accel_group,
                              GDK_L, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  menu_Tools_LogViewer = gtk_menu_item_new_with_label ("Log Viewer");
  gtk_widget_show (menu_Tools_LogViewer);
  gtk_container_add (GTK_CONTAINER (menu_Tools_menu), menu_Tools_LogViewer);
  gtk_signal_connect (GTK_OBJECT (menu_Tools_LogViewer), "activate",
                      GTK_SIGNAL_FUNC (log_viewer),
                      NULL);
  gtk_widget_add_accelerator (menu_Tools_LogViewer, "activate", accel_group,
                              GDK_V, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  /* help menu */
  menu_Help = gtk_menu_item_new_with_label ("Help");
  gtk_widget_show (menu_Help);
  gtk_container_add (GTK_CONTAINER (menubar), menu_Help);
  gtk_menu_item_right_justify (GTK_MENU_ITEM (menu_Help));

  menu_Help_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_Help), menu_Help_menu);

  menu_Help_Manual = gtk_menu_item_new_with_label ("Manual");
  gtk_widget_show (menu_Help_Manual);
  gtk_container_add (GTK_CONTAINER (menu_Help_menu), menu_Help_Manual);
  gtk_signal_connect (GTK_OBJECT (menu_Help_Manual), "activate",
                      GTK_SIGNAL_FUNC (do_manual),
                      NULL);
  gtk_widget_add_accelerator (menu_Help_Manual, "activate", accel_group,
                              GDK_H, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

  separator3 = gtk_menu_item_new ();
  gtk_widget_show (separator3);
  gtk_container_add (GTK_CONTAINER (menu_Help_menu), separator3);

  menu_Help_About = gtk_menu_item_new_with_label ("About");
  gtk_widget_show (menu_Help_About);
  gtk_container_add (GTK_CONTAINER (menu_Help_menu), menu_Help_About);
  gtk_signal_connect (GTK_OBJECT (menu_Help_About), "activate",
                      GTK_SIGNAL_FUNC (do_about),
                      NULL);
  gtk_widget_add_accelerator (menu_Help_About, "activate", accel_group,
                              GDK_B, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);

/*** ToolBar ***/
  /* we need to realize the window because we use pixmaps for 
   * items on the toolbar in the context of it */
  gtk_widget_realize ( mud->window );

  /* to make it nice we'll put the toolbar into the handle box, 
   * so that it can be detached from the main window */
  handlebox = gtk_handle_box_new ();
  gtk_box_pack_start ( GTK_BOX ( vbox1 ), handlebox, FALSE, FALSE, 2 );

  /* toolbar will be horizontal, with both icons and text, and
   * with 5pxl spaces between items and finally, 
   * we'll also put it into our handlebox */
  toolbar = gtk_toolbar_new ( GTK_ORIENTATION_HORIZONTAL,
                              GTK_TOOLBAR_ICONS );
  gtk_container_set_border_width ( GTK_CONTAINER ( toolbar ) , 5 );
  gtk_toolbar_set_space_size ( GTK_TOOLBAR ( toolbar ), 5 );
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

  /* our next item is <Font> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_font );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);
  
  btn_toolbar_font = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar), 		/* our toolbar */
                                           NULL,               			/* button label */
                                           "Font (Alt+F)",     			/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                 		/* icon widget */
                                           GTK_SIGNAL_FUNC (window_font), 	/* a signal */
                                           NULL );

  /* our next item is <Colors> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_colors );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);

  btn_toolbar_colors = gtk_toolbar_append_item ( GTK_TOOLBAR (toolbar),		/* our toolbar */
                                           NULL,               			/* button label */
                                           "Colors (Alt+O)",   			/* this button's tooltip */
                                           "Private",             		/* tooltip private info */
                                           iconw,                 		/* icon widget */
                                           GTK_SIGNAL_FUNC (color_prefs), 	/* a signal */
                                           NULL );

  /* our next item is <Preferences> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_prefs );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);

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

  /* our next item is <help> button */
  icon = gdk_pixmap_create_from_xpm_d ( mud->window->window, &mask, &mud->window->style->white, SC_help );
  iconw = gtk_pixmap_new ( icon, mask ); 					/* icon widget */
  gdk_pixmap_unref(icon); gdk_bitmap_unref(mask);

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
      gtk_signal_connect (GTK_OBJECT (macro_button), "clicked",
                          GTK_SIGNAL_FUNC (macro_send), (gpointer) i);
      gtk_widget_show (macro_button);
      gtk_table_attach (GTK_TABLE (table), macro_button, 1, 2, i, i+1,
                       (GtkAttachOptions) GTK_EXPAND | GTK_FILL,
		       (GtkAttachOptions) GTK_EXPAND, 0, 0);
      gtk_widget_set_usize (macro_button, 65, 26);
      gtk_container_border_width (GTK_CONTAINER (macro_button), 3);
      gtk_widget_add_accelerator (macro_button, "clicked", accel_group,
                                  key++, 0, GTK_ACCEL_VISIBLE);
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
  mud->text = new_view("not connected", mud->notebook);        
#else
  mud->text = new_view("not connected", hbox1);
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

  statusbar_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "Statusbar");
  gtk_statusbar_push (GTK_STATUSBAR(statusbar), statusbar_id, "Ready");
  macro_btnLabel_change();

}

static int scrolled_up;

/* FOR THE WINDOW BUFFER FUNCTION */
void clear_backbuffer()
{
    
    if (mud->maxlines > 0) {
        int n;

        n= gtk_text_get_length (mud->text);
        
        if( n < mud->maxlines)
            return;

        gtk_text_set_point (mud->text, n - mud->maxlines);
        gtk_text_backward_delete (mud->text, n - mud->maxlines);
        gtk_text_set_point (mud->text, mud->maxlines);
    }
}

void textfield_freeze()
{
  GtkAdjustment *adj = mud->text->vadj;

  if (adj->value < (adj->upper - adj->page_size)) {
     scrolled_up = TRUE;
  }
  
  gtk_text_freeze(mud->text);
}

void textfield_unfreeze()
{
    GtkAdjustment *adj = mud->text->vadj;

    clear_backbuffer();
  
    gtk_text_thaw (mud->text);  
  
    if (scrolled_up) {
        scrolled_up = FALSE;
    }
    else if(adj->value < (adj->upper - adj->page_size))
        gtk_adjustment_set_value(adj, (adj->upper - adj->page_size));

    gtk_widget_grab_focus ((GtkWidget *)mud->ent);
}

void textfield_add(GtkText *txt, const char *message, int colortype)
{
    int numbytes;

    if (!*message)
        return;

    switch (colortype) {
        case MESSAGE_ANSI:
            numbytes = strlen(message);
            
            disp_ansi(numbytes, message, txt);
            break;
        case MESSAGE_SENT:
            if (mud->LOGGING) /* Loging */
                fprintf(mud->LOG_FILE, message);
            gtk_text_insert(txt, font_normal,
                    &color_lightyellow, NULL, message, -1);
            break;
        case MESSAGE_ERR:
            if (mud->LOGGING) /* Loging */
                fprintf(mud->LOG_FILE, message);
            gtk_text_insert(txt, font_normal, &color_red, NULL, message, -1);
            break;
        case MESSAGE_TICK:
            gtk_text_insert(txt, font_normal,
                    &color_lightcyan, NULL, message, -1);
            break;
        default:
            if (mud->LOGGING) /* Loging */
                fprintf(mud->LOG_FILE, message);
            gtk_text_insert(txt, font_normal,
                    &prefs.DefaultColor, NULL, message, -1);
    }
}


	/* n is the number of lines to NOT delete, if it's 0, delete 'em all */
void clear(int n, GtkText *target)
{
    gtk_text_freeze (target);
    gtk_text_backward_delete ((target), gtk_text_get_length (target));
    gtk_text_thaw (target);
}	


void popup_window (const gchar *message)
{
    GtkWidget *window;
    GtkWidget *label;
    GtkWidget *button;
    GtkWidget *box;
    GtkWidget *separator;

    gchar       buf[3072];

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "Popup Message");

    box = gtk_vbox_new (FALSE, 5);
    gtk_container_set_border_width (GTK_CONTAINER (box), 5);
    gtk_container_add (GTK_CONTAINER (window), box);

    g_snprintf ( buf, 3072, " %s ", message);
    label = gtk_label_new (buf);
    gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 5);
    gtk_widget_show (label);

    separator = gtk_hseparator_new ();
    gtk_box_pack_start (GTK_BOX (box), separator, TRUE, TRUE, 0);
    gtk_widget_show (separator);
    
    button = gtk_button_new_with_label (" OK ");
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (close_window),
                        window);
    gtk_box_pack_start (GTK_BOX (box), button, TRUE, TRUE, 5);
    gtk_widget_show (button);
    
    gtk_widget_show (box);
    gtk_widget_show (window);
}


GtkText *new_view(char *name, GtkWidget *parent)
{
  GtkWidget *hbox4, *templabel, *vscrollbar;
  GtkText *text;
  
  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox4);

#ifdef USE_NOTEBOOK
      gtk_container_add(GTK_CONTAINER(parent), hbox4);

      templabel = gtk_label_new(name);
      gtk_widget_show(templabel);
      
      gtk_notebook_set_tab_label (mud->notebook, 
              gtk_notebook_get_nth_page(mud->notebook, 0), templabel);
#else
      gtk_box_pack_start(GTK_BOX(parent), GTK_WIDGET(hbox4), TRUE, TRUE, 0);
#endif
      
  /* main text window */
  text = (GtkText *)gtk_text_new (NULL, NULL);
  gtk_signal_connect(GTK_OBJECT(text),"key_press_event",GTK_SIGNAL_FUNC(change_focus), mud);
  gtk_widget_show (GTK_WIDGET(text));
  gtk_box_pack_start (GTK_BOX (hbox4), GTK_WIDGET(text), TRUE, TRUE, 0);

  gtk_widget_realize (GTK_WIDGET(text));

  /* the scrollbar attached to the main text window */
  vscrollbar = gtk_vscrollbar_new (text->vadj);
  gtk_widget_show (vscrollbar);
  gtk_box_pack_start (GTK_BOX (hbox4), vscrollbar, FALSE, TRUE, 0);
  GTK_WIDGET_UNSET_FLAGS (vscrollbar, GTK_CAN_FOCUS);

  gdk_window_set_background(text->text_area,
			      &(prefs.BackgroundColor));

  return text;
}

