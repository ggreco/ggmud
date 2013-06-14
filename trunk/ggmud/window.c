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

#include <glib.h>
#include <string.h>
#include "ggmud.h"
#include "config.h"
#include "ansi.h"
#include "interface.h"
#include "support.h"
#include "include/action.h"
#include "include/variables.h"

/* External variables used */
extern int hide_input;

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

GList *windows_list = NULL;

/******************************/
/* the #showme / #say command */
/******************************/

extern char *get_arg_in_braces(char *s, char *arg, int flag);

static void
send_refresh_signal(void)
{
	GdkEventClient event;
    	event.type = GDK_CLIENT_EVENT;
	event.send_event = TRUE;
	event.window = NULL;
	event.message_type = gdk_atom_intern("_GTK_READ_RCFILES", FALSE);
	event.data_format = 8;
	gdk_event_send_clientmessage_toall((GdkEvent *)&event);
}

void set_style() 
{
    static char *previous_font = NULL;
    GList *l = windows_list;

    if (fonts[INTERFACE_FONT].name) {
        char buff[256];

        sprintf(buff,  "style \"user-font\"\n{\n  font_name=\"%s\"\n}\nwidget_class \"*\" style \"user-font\"\n\n", fonts[INTERFACE_FONT].name);
        gtk_rc_parse_string(buff);
        sprintf(buff, "gtk-font-name=\"%s\"\n", fonts[INTERFACE_FONT].name);
        gtk_rc_parse_string(buff);

        if (previous_font) {
            if (strcmp(previous_font, fonts[INTERFACE_FONT].name))
                popup_window(INFO, "You'll need to save and restart the client to correctly\n"
                             "update the <b>interface</b> font to \"%s\" in every window.",
                             fonts[INTERFACE_FONT].name);

            free(previous_font);
        }
        previous_font = strdup(fonts[INTERFACE_FONT].name);

    }

    if (fonts[INPUT_FONT].desc && mud && mud->ent) {
        gtk_widget_modify_font(GTK_WIDGET(mud->ent), fonts[INPUT_FONT].desc);
    }
    
    // set the output font
    if (!fonts[OUTPUT_FONT].desc)
        return;
    
    if (mud && mud->text)
        gtk_widget_modify_font(GTK_WIDGET(mud->text), fonts[OUTPUT_FONT].desc);

    while (l) {
        gtk_widget_modify_font(
                GTK_WIDGET(((window_entry *)l->data)->listptr), fonts[OUTPUT_FONT].desc);
                
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

GtkWidget *create_new_window(const char *title, int width, int height)
{
    GtkWidget *win, *vbox;
    GtkWidget *list;

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_skip_taskbar_hint (GTK_WINDOW(win), prefs.SkipTaskbar);
    gtk_widget_set_usize(win, width, height);
    gtk_window_set_title(GTK_WINDOW(win), title);
    gtk_container_border_width(GTK_CONTAINER(win), 4);

    gtk_widget_realize(win);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show(vbox);

    gtk_container_add(GTK_CONTAINER(win), vbox);
    
    list = (GtkWidget *) new_view(NULL, vbox, FALSE);
    g_signal_connect (G_OBJECT (list), "destroy", (GCallback)destroy_a_window, NULL);

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

window_entry *create_new_entry(const char *title, int width, int height)
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

extern GdkPixmap *enabled_pixmap, *disabled_pixmap;
extern GdkBitmap *enabled_mask, *disabled_mask;

void setup_pixmaps(GtkWidget *w)
{
    enabled_pixmap = gdk_pixmap_create_from_xpm_d ( w->window,
            &enabled_mask, &mud->window->style->white, get_image_pointer("green.xpm") );
    disabled_pixmap = gdk_pixmap_create_from_xpm_d ( w->window,
            &disabled_mask, &mud->window->style->white, get_image_pointer("red.xpm") );
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

window_entry *in_window_list(const char *tag)
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
on_clear_review_buffer_activate(GtkMenuItem *u1, gpointer u2)
{
    GtkTextBuffer *b;

    if (!mud || !mud->text ||
        !(b = gtk_text_view_get_buffer(mud->text)))
        return;

    gtk_text_buffer_set_text(b, "<clear>\n", -1);
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
    char left[BUFFER_SIZE], right[BUFFER_SIZE],
         temp[BUFFER_SIZE];

    arg = get_arg_in_braces(arg, left, 0);
    arg = get_arg_in_braces(arg, right, 1);

    if(!*left || !*right)
        return;
   
    substitute_myvars(right, temp, s);
    substitute_vars(temp, right);

    if (!strcasecmp(left, "error"))
        popup_window(ERR, right);
    else if (!strcasecmp(left, "warning"))
        popup_window(WARN, right);
    else
        popup_window(INFO, right);
}

void do_grep(char *arg, struct session *s)
{
    GtkTextIter start, end;
    GtkTextBuffer *b = gtk_text_view_get_buffer(mud->text);
    int i, lines = gtk_text_buffer_get_line_count(b), allocation = 1024;
    gchar *text;
    char *result = malloc(allocation);

    snprintf(result, allocation, "\nSearching for <%s>...\n", arg);
    textfield_add(mud->text, result, MESSAGE_SENT);
    *result = 0;

    for (i = 0; i < lines; ++i) {
        gtk_text_buffer_get_iter_at_line(b, &start, i);
        gtk_text_buffer_get_iter_at_line(b, &end, i + 1);
        if ((text = gtk_text_buffer_get_text(b, &start, &end, FALSE))) {
            if (strlen(text) > 1 && 
                strncasecmp(text + 1, "grep", 4) &&  // remove previous grep calls
                strncmp(text, "Searching for", 13) && // remove previous grep headers
                strncmp(text, "Line ", 5) && // remove previous grep results
                strstr(text, arg)) {
                int len = strlen(result);
                snprintf(result + len, allocation - len, "Line %d: %s", i + 1, text);

                if (strlen(result) > (allocation * 2 / 3)) {
                    allocation *= 2;
                    result = realloc(result, allocation);
                }
            }
            g_free(text);
        }
    }
    strcat(result, "\n");
    textfield_add(mud->text, result , MESSAGE_SENT);
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

    
    if(*right && entry) {
        char *result;
        substitute_myvars(right, left, s);
        substitute_vars(left, right);
        result = ParseAnsiColors(right);
        strcat(result, "\n");
        textfield_add((GtkTextView *)entry->listptr, result , MESSAGE_LOCAL);
    }
}

GtkWidget *
MakeButton(const char *image, GtkSignalFunc func, gpointer data)
{
    GtkWidget *button = gtk_button_new_from_stock(image);
    g_signal_connect (G_OBJECT (button), "clicked",
                               (GCallback)func,
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
    GdkPixbuf *icon;

    if(!gtk_toggle_button_get_active(togglebutton)) {
        textfield_add(mud->text, "# PARSING ENABLED\n", MESSAGE_SENT);
        verbatim = 0;
    }
    else {
        textfield_add(mud->text, "# PARSING DISABLED\n", MESSAGE_SENT);
        verbatim = 1;
    }

    icon = create_pixbuf(verbatim ? "parsing-off.xpm" : "parsing.xpm");

    gtk_image_set_from_pixbuf(GTK_IMAGE(GTK_BIN(togglebutton)->child), icon);

    g_object_unref(icon);
}

void toggle_triggers(GtkToggleButton *togglebutton,
                                            gpointer user_data)
{
    extern int use_triggers;
    GdkPixbuf *icon;

    if(!gtk_toggle_button_get_active(togglebutton)) {
        textfield_add(mud->text, "# TRIGGERS ENABLED\n", MESSAGE_SENT);
        use_triggers = 1;
    }
    else {
        textfield_add(mud->text, "# TRIGGERS DISABLED\n", MESSAGE_SENT);
        use_triggers = 0;
    }

    icon = create_pixbuf(use_triggers ? "trig.xpm" : "trig-off.xpm");

    gtk_image_set_from_pixbuf(GTK_IMAGE(GTK_BIN(togglebutton)->child), icon);

    g_object_unref(icon);
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

void cbox()
{
    GtkWidget *w = create_window_connect();
    if (*prefs.default_host) {
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(w, "entry_host")), prefs.default_host);
        if (prefs.default_port > 0 && prefs.default_port < 65536) {
            char buffer[32];
            sprintf(buffer, "%d", prefs.default_port);
            gtk_entry_set_text(GTK_ENTRY(lookup_widget(w, "entry_port")), buffer);
        }
    }
    gtk_window_set_skip_taskbar_hint (GTK_WINDOW(w), prefs.SkipTaskbar);
    gtk_widget_show(w);
    gtk_window_set_transient_for(GTK_WINDOW(w), GTK_WINDOW(mud->window));
}

void do_con(GtkWidget *base, void *userdata)
{
    const char *port;
    const char *host;
    GtkWidget *root = gtk_widget_get_toplevel(base);

    gchar buf[256];

    host = gtk_entry_get_text(GTK_ENTRY(lookup_widget(base, "entry_host")));
    port = gtk_entry_get_text(GTK_ENTRY(lookup_widget(base, "entry_port")));
	
    make_connection("unknown", host,port);
    sprintf (buf, "Connected to %s - GGMud %s", host, VERSION);
    gtk_window_set_title (GTK_WINDOW (mud->window), buf);

    gtk_widget_destroy(root);
}

/*
 * This Function creates the main window
 */

void toggle_review(void)
{
    GtkWidget *review_toggle = lookup_widget(mud->window, "togglebutton_review");

    if (GTK_WIDGET_VISIBLE(mud->review)) {
        gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (review_toggle), FALSE);
        gtk_widget_hide(mud->review);
    }
    else {
        gtk_toggle_button_set_state (GTK_TOGGLE_BUTTON (review_toggle), TRUE);
        gtk_widget_show(mud->review);
    }
}

#ifdef ENABLE_MCCP
void mccp_status()
{
    char buffer[256];
    unsigned long comp, uncomp;

    if (!connected) {
        popup_window(INFO, "MCCP/MSP status can be determined only if connected.");
        return;
    }

    GtkWidget *w = create_window_mccp_status();
    gtk_window_set_skip_taskbar_hint (GTK_WINDOW(w), prefs.SkipTaskbar);
 
    if (mudcompress_compressing(mud->activesession->mccp)) {
        gtk_image_set_from_pixmap(GTK_IMAGE(
                    lookup_widget(w, "image_mccp_status")), enabled_pixmap, enabled_mask);

        gtk_label_set_markup(GTK_LABEL(
                    lookup_widget(w, "label_mccp_status")), "<b>MCCP is active</b>");
    }
    else {
        gtk_image_set_from_pixmap(GTK_IMAGE(
                    lookup_widget(w, "image_mccp_status")), disabled_pixmap, disabled_mask);

        gtk_label_set_markup(GTK_LABEL(
                    lookup_widget(w, "label_mccp_status")), "<b>MCCP is inactive</b>");
    }

    if (mud->msp) {
        gtk_image_set_from_pixmap(GTK_IMAGE(
                    lookup_widget(w, "image_msp_status")), enabled_pixmap, enabled_mask);

        gtk_label_set_markup(GTK_LABEL(
                    lookup_widget(w, "label_msp_status")), "<b>MSP is active</b>");

        sprintf(buffer, "<big>%ld</big>", mud->msp->sounds_played);

        gtk_label_set_markup(GTK_LABEL(
                    lookup_widget(w, "label_sounds")), buffer);

        sprintf(buffer, "<big>%ld</big>", mud->msp->musics_played);

        gtk_label_set_markup(GTK_LABEL(
                    lookup_widget(w, "label_musics")), buffer);
    }
    else {
        gtk_image_set_from_pixmap(GTK_IMAGE(
                    lookup_widget(w, "image_msp_status")), disabled_pixmap, disabled_mask);

        gtk_label_set_markup(GTK_LABEL(
                    lookup_widget(w, "label_msp_status")), "<b>MSP is inactive</b>");
    }

    mudcompress_stats(mud->activesession->mccp, &comp, &uncomp);

    sprintf(buffer, "<big>%ld</big>", comp);

    gtk_label_set_markup(GTK_LABEL(
                lookup_widget(w, "label_comp")), buffer);

    sprintf(buffer, "<big>%ld</big>", uncomp);

    gtk_label_set_markup(GTK_LABEL(
                lookup_widget(w, "label_uncomp")), buffer);


    gtk_widget_show(w);
}
#endif

void
spawn_gui()
{
  // experimental code for GTK 2.4/2.6 to avoid "gtk-connect"
  GtkStockItem item;

  if (!gtk_stock_lookup("gtk-connect", &item)) {
      item.stock_id = "gtk-connect";
      item.label = "Connect";
      item.modifier = GDK_MOD1_MASK;
      item.keyval = GDK_c;
      item.translation_domain = NULL;
      gtk_stock_add(&item, 1);
  }

  if (!gtk_stock_lookup("gtk-disconnect", &item)) {
      item.stock_id = "gtk-disconnect";
      item.label = "Disconnect";
      item.modifier = GDK_MOD1_MASK;
      item.keyval = GDK_d;
      item.translation_domain = NULL;
      gtk_stock_add(&item, 1);
  }

  /* create the main window */
  mud->window = create_window_main();
  gtk_window_set_skip_taskbar_hint (GTK_WINDOW(mud->window), prefs.SkipTaskbar);

#if defined(__APPLE__) && defined(GTKQUARTZ)
  {
      extern void sync_menu_takeover_menu(GtkMenuShell *);

      GtkWidget *menu = lookup_widget(mud->window, "menubar_main");
      sync_menu_takeover_menu (GTK_MENU_SHELL(menu));
  }
#endif

  gtk_window_set_title (GTK_WINDOW (mud->window), "GGMud "VERSION"");
  gtk_widget_realize ( mud->window );

  gtk_widget_set_sensitive(lookup_widget(mud->window, "menuitem_disconnect"), FALSE); 

#ifndef ENABLE_MCCP
  // disable MCCP menu
  gtk_widget_set_sensitive(lookup_widget(mud->window, "menuitem_mccp_status"), FALSE);
#endif
  
  gtk_menu_item_right_justify (GTK_MENU_ITEM(lookup_widget(mud->window, "menu_help")));


  
  gtk_widget_set_sensitive(lookup_widget(mud->window, "toolbutton_disconnect"), FALSE);

  if (!prefs.Toolbar)
      gtk_widget_hide (lookup_widget(mud->window, "handlebox_toolbar"));

/*** ToolBar End ***/

  mud->text = new_view("not connected", lookup_widget(mud->window, "hbox_output"),  TRUE);
  
/* the entry box, that we use to type the commands in */
  mud->ent = GTK_ENTRY(lookup_widget(mud->window, "entry_input"));

  if (fonts[INPUT_FONT].desc)
      gtk_widget_modify_font(GTK_WIDGET(mud->ent), fonts[INPUT_FONT].desc);

  // tick counter
  mud->tick_counter = GTK_LABEL(lookup_widget(mud->window, "label_tickcounter"));
 
  if (!prefs.Statusbar)
      gtk_widget_hide (lookup_widget(mud->window, "statusbar_main"));

  setup_pixmaps(mud->window);

  gtk_widget_show(mud->window);
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
        case MESSAGE_SCROLLING_ANSI:
            numbytes = strlen(message);
            
            local_disp_ansi(numbytes, message, txt);

            if ((mark = gtk_object_get_user_data(GTK_OBJECT(txt))))
                   gtk_text_view_scroll_mark_onscreen(txt, mark);
            return;
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
                fprintf(mud->LOG_FILE, "%s", message);
            tag = fg_colors[1][3];// light yellow 
            break;
        case MESSAGE_ERR:
            if (mud->LOGGING) /* Loging */
                fprintf(mud->LOG_FILE, "%s", message);
            tag = fg_colors[0][1]; // red
            break;
        case MESSAGE_TICK:
            tag = blink_colors[1][6]; // cyan
            break;
        default:
            if (mud->LOGGING) /* Loging */
                fprintf(mud->LOG_FILE, "%s", message);
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

void popup_window (int type, const gchar *message, ...)
{
    va_list va;
    gchar       buf[3072];
    GtkWidget *d;

    va_start(va, message);
    vsnprintf(buf, sizeof(buf), message, va);
    va_end(va);

    d = gtk_message_dialog_new_with_markup(NULL, GTK_DIALOG_MODAL,
                           type, GTK_BUTTONS_OK,
                           "%s", buf);

    gtk_dialog_run(GTK_DIALOG(d));
   
    gtk_widget_destroy(d);
}

extern gboolean textview_motion_notify_cb(GtkWidget *textview, GdkEventMotion *event, __attribute__((unused))gpointer d);

static 
GtkWidget *create_tv(GtkTextBuffer *buffer, GtkTextView **view)
{
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
   
    if (fonts[OUTPUT_FONT].desc)
        gtk_widget_modify_font((GtkWidget *)text, fonts[OUTPUT_FONT].desc);

    g_signal_connect(G_OBJECT(text),"key_press_event",(GCallback)change_focus, mud);
    g_signal_connect(G_OBJECT(text), "motion-notify-event", (GCallback)textview_motion_notify_cb, NULL);

    return sw;
}

GtkTextView *new_view(char *name, GtkWidget *parent, int ismain)
{
    GtkWidget *paned, *sw, *sw2;
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

    sw2 = create_tv(buf, &t2);
    gtk_paned_add2(GTK_PANED(paned), sw2);

    gtk_window_get_size(GTK_WINDOW(gtk_widget_get_toplevel(parent)), &w, &h);
    gtk_paned_set_position(GTK_PANED(paned), h * 2 / 3);

    if (ismain) 
        mud->review = sw;

    return (GtkTextView *)t2;
}

