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

/* Sat Jul 3 1999 
 * Color editor code done by Patrik Nordebo, 
 *  changes made by The SClient Team.
 */

#include "config.h"


# include <gtk/gtk.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ggmud.h"
#include "ansi.h"
#include "interface.h"
#include "support.h"

#ifdef WIN32
#define PREFS_FILE "ggmud.prf"
#else
#define PREFS_FILE "Preference"
#endif

/* Used for the color selection UI
 */
typedef struct {
    GdkColor *color;
    gchar *name;
    GtkWidget *widget;
} color_struct;

/* Global ToolBar stuff */
extern int use_tickcounter;

/* Global variables */
PREFS_DATA prefs = {0};

#ifndef min
    #define min(x,y) ((x) > (y) ? (y) : (x))
#endif

color_struct color_arr[] = {
      {&color_white, "white", NULL},
      {&color_lightwhite, "light white", NULL},
      {&color_blue, "blue", NULL},
      {&color_lightblue, "light blue", NULL},
      {&color_red, "red", NULL},
      {&color_lightred, "light red", NULL},
      {&color_green, "green", NULL},
      {&color_lightgreen, "light green", NULL},
      {&color_yellow, "yellow", NULL},
      {&color_lightyellow, "light yellow", NULL},
      {&color_cyan, "cyan", NULL},
      {&color_lightcyan, "light cyan", NULL},
      {&color_magenta, "magenta", NULL},
      {&color_lightmagenta, "light magenta", NULL},
      {&color_black, "black", NULL},
      {&color_lightblack, "light black", NULL},
      {&prefs.BackgroundGdkColor, "background color", NULL},
      {&prefs.DefaultGdkColor, "default color", NULL},
      {&prefs.BrightGdkColor, "bright default color", NULL},
      {NULL,NULL}
};

void update_widget_color(color_struct *col)
{
    if(col->widget) {
        gtk_color_button_set_color(GTK_COLOR_BUTTON(col->widget), col->color);
        update_color_tags(col->color);
    }
}

void load_zmud_prefs(void)
{
    popup_window(WARN, "At the moment trigger params (%%%%d), (%%%%w)...\nand other custom zmud settings are not imported!");

    load_tt_prefs();
}

void load_tt_prefs(void)
{
    /* Create a new file selection widget */
    GtkWidget *filew = gtk_file_chooser_dialog_new ( 
             "Load TT++/GGMUD command file..", GTK_WINDOW(mud->window),
             GTK_FILE_CHOOSER_ACTION_OPEN,
             GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
             GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
             NULL);

    gtk_widget_show(filew);

    if (gtk_dialog_run(GTK_DIALOG(filew)) == GTK_RESPONSE_ACCEPT) {
        char buffer[512];
        gchar *file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filew));
        sprintf(buffer, "{%s}", file);
        g_free(file);

        read_command(buffer, NULL);
    }

    gtk_widget_destroy(filew);
    
}

#ifdef WIN32
#define CONFIG_NAME "ggmud.cfg"
#else
#define CONFIG_NAME ".ggmudrc"
#endif

void save_all_prefs(void)
{
#ifdef WIN32
    write_command(CONFIG_NAME, NULL);

    popup_window(INFO, "Configuration exported to file " CONFIG_NAME);
#else
    char buffer[256], *c;

    if (!(c = getenv("HOME")))
        c = "./";
    
    strcpy(buffer, c);
    strcat(buffer, "/" CONFIG_NAME);
    write_command(buffer, NULL);
    
    popup_window(INFO, "Configuration exported to: %s" CONFIG_NAME , c);
#endif
}

gdouble *gdk_color_to_gdouble (GdkColor *gdkcolor)
{
    gdouble *color = malloc(sizeof(gdouble)*3);
    
    color[0] = (double)gdkcolor->red / 65535;
    color[1] = (double)gdkcolor->green / 65535;
    color[2] = (double)gdkcolor->blue / 65535;

    return color;
}

extern int tick_size;

typedef struct {
    char *label;
    char *config;
    int *value;
} checkbutton;

checkbutton prefs_buttons[] = {
    {"checkbutton_keep", "KeepText", &prefs.KeepText},
    {"checkbutton_echo", "EchoText",  &prefs.EchoText},
    {"checkbutton_ww", "WordWrap", &prefs.WordWrap},
    {"checkbutton_blink", "Blinking", &prefs.Blinking},
    {"checkbutton_toolbar", "Toolbar", &prefs.Toolbar},
    {"checkbutton_macro", "Macrobuttons", &prefs.Macrobuttons},
    {"checkbutton_statusbar", "Statusbar", &prefs.Statusbar},
    {"checkbutton_tickcounter", "TickCounter", &use_tickcounter},
    {"checkbutton_beep", "DoBeep", &prefs.DoBeep},
    {"checkbutton_save_vars", "SaveVars", &prefs.SaveVars},
    {"checkbutton_use_proxy", "UseSocks", &prefs.UseSocks},
    {"checkbutton_wizard", "WizAtStartup", &prefs.WizAtStartup},
    {"checkbutton_taskbar", "SkipTaskbar", &prefs.SkipTaskbar},
    {"checkbutton_autoupdate", "AutoUpdate", &prefs.AutoUpdate},
    {"checkbutton_msp", "UseMSP", &prefs.UseMSP},
    {NULL, NULL, NULL}
};

int check_boolean_keys(const char *pref, const char *value)
{
    int i = 0;

    while (prefs_buttons[i].config) {
        if (!strcmp(pref, prefs_buttons[i].config)) {
            if (!strcmp(value, "Off"))
                *(prefs_buttons[i].value) = FALSE;
            else
                *(prefs_buttons[i].value) = TRUE;

            return 1;
        }

        i++;
    }

    return 0;
}

void load_prefs () 
{
    FILE *fp;
    gchar line[255], pref[100], value[230];

    //    prefs.BackgroundColor = color_black;
    //    prefs.BackgroundColor.pixel = 0;
    //    prefs.DefaultColor = color_white;


    if ((fp = fileopen(PREFS_FILE, "r"))) {
        prefs.SaveVars = prefs.Blinking = prefs.KeepText = prefs.EchoText  = prefs.WordWrap = prefs.DoBeep = TRUE;
        prefs.UseSocks = prefs.WizAtStartup = prefs.SkipTaskbar = FALSE;
        prefs.AutoUpdate = TRUE; prefs.UseMSP = TRUE;
        prefs.SoundPath[0] = 0;
        prefs.LuaConfig = NULL;

        while (fgets (line, sizeof(line) - 1, fp)) {
            sscanf (line, "%[^=]=%[^\n]", pref, value);

            // if we find a key we skip the remaining part of the line
            if (check_boolean_keys(pref, value)) 
                continue;

            if (!strcmp(pref, "SocksUser")) {
                strncpy(prefs.socks_user, value, sizeof(prefs.socks_user));
            }
            else if (!strcmp(pref, "SocksPwd")) {
                strncpy(prefs.socks_password, value, sizeof(prefs.socks_password));
            }
            else if (!strcmp(pref, "SocksAddress")) {
                strncpy(prefs.socks_addr, value, sizeof(prefs.socks_addr));
            }
            else if (!strcmp(pref, "SocksProtocol")) {
                prefs.socks_protocol = atoi(value);
            } else if (!strcmp(pref, "SocksPort")) {
                prefs.socks_port = atoi(value);
            } else if (!strcmp(pref, "ReviewSize")) {
                int temp = atoi(value);

                if(temp > 100 && temp < 100000)
                    mud->maxlines = temp;
            } else if (!strcmp(pref, "TickSize")) {
                int temp = atoi(value);

                if(temp > 10 && temp < 1000)
                    tick_size = temp;
            } else if (!strcmp(pref, "LuaConfig")) {
                FILE *f = fopen(value, "r");

                if (f) {
                    fclose(f);
                    prefs.LuaConfig = strdup(value);
                }
            } else { // import colors
                int i = 0;

                while(color_arr[i].name) {	    
                    if(!strcmp (color_arr[i].name, pref)) {
                        int red, green, blue;

                        if (sscanf(value, "(%u,%u,%u)",
                                    &red,
                                    &green,
                                    &blue) == 3) {

                            color_arr[i].color->red = red;
                            color_arr[i].color->green = green;
                            color_arr[i].color->blue = blue;

                            if(!gdk_color_alloc(gdk_colormap_get_system(), 
                                        color_arr[i].color)) {

                                g_error("Couldn't allocate background color - reverting to black\n");
                                color_arr[i].color->pixel = color_black.pixel;
                                color_arr[i].color->red = color_black.red;
                                color_arr[i].color->green = color_black.green;
                                color_arr[i].color->blue = color_black.blue;
                            }

                            update_color_tags(color_arr[i].color);
                        }
                        break;
                    }
                    i++;
                }
            }
        }
        fclose (fp);
    }

}

void change_file(GtkWidget *from, GtkEntry *dest)
{
        GtkWidget *filew = gtk_file_chooser_dialog_new ( 
                        "Select a startup LUA script..", GTK_WINDOW(gtk_widget_get_toplevel(from)),
                        GTK_FILE_CHOOSER_ACTION_OPEN,
                        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                        GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                        NULL);

        gtk_widget_show(filew);

        if (gtk_dialog_run(GTK_DIALOG(filew)) == GTK_RESPONSE_ACCEPT) {
                gchar *file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filew));
                gtk_entry_set_text(dest, file);
                g_free(file);
        }

        gtk_widget_destroy(filew);
}

static void save_prefs (GtkWidget *button, gpointer data) 
{
#define CFGW(x,y) fprintf(fp, "%s=%s\n", x, y ? "On" : "Off")
#define CFGI(x,y) fprintf(fp, "%s=%d\n", x, y)
#define CFGS(x,y) fprintf(fp, "%s=%s\n", x, y)
    FILE *fp;
    
    if ((fp = fileopen (PREFS_FILE, "w"))) {
        int i=0;
        extern int tick_size;
       

        while(prefs_buttons[i].config) {
            CFGW(prefs_buttons[i].config, *prefs_buttons[i].value);
            i++;
        }

        CFGI("TickSize", tick_size);
        CFGI("ReviewSize", mud->maxlines);
        CFGS("LuaConfig", prefs.LuaConfig);
        CFGS("SocksAddress", prefs.socks_addr);
        CFGI("SocksProtocol", prefs.socks_protocol);
        CFGI("SocksPort", prefs.socks_port);
        CFGS("SocksUser", prefs.socks_user);
        CFGS("SocksPwd", prefs.socks_password);

        i = 0;

        while(color_arr[i].name) {
    	    fprintf(fp, "%s=(%u,%u,%u)\n",color_arr[i].name,
	    	color_arr[i].color->red, color_arr[i].color->green,
		    color_arr[i].color->blue);
    	    i++;
    	}
    	fclose (fp);
    }

    gtk_widget_destroy(gtk_widget_get_toplevel(button));
}


void on_socks_ok_clicked(GtkWidget *button)
{
    int port;

    port = atoi(gtk_entry_get_text(
                GTK_ENTRY(lookup_widget(button, "entry_socks_port"))));

    if (port < 1 || port > 65535) {
        popup_window(ERR, "Invalid Port number!");
        return;
    }

    if (gtk_toggle_button_get_active(
                GTK_TOGGLE_BUTTON(lookup_widget(button, "checkbutton_pwd_auth")))) {
        const char *user, *pwd;

        user = gtk_entry_get_text(
                GTK_ENTRY(lookup_widget(button, "entry_socks_user"))); 
        pwd = gtk_entry_get_text(
                GTK_ENTRY(lookup_widget(button, "entry_socks_password"))); 

        if (!*user || !*pwd) {
            popup_window(ERR, "Invalid username or password!");
            return;
        }

        strcpy(prefs.socks_user, user);
        strcpy(prefs.socks_password, pwd);
    }

    strncpy(prefs.socks_addr, gtk_entry_get_text(
                GTK_ENTRY(lookup_widget(button, "entry_socks_addr"))), 
                sizeof(prefs.socks_addr));

    prefs.socks_port = port;

    if (gtk_toggle_button_get_active(
                GTK_TOGGLE_BUTTON(lookup_widget(button, "radiobutton_v4"))))
        prefs.socks_protocol = 4;
    else
        prefs.socks_protocol = 5;

    gtk_widget_destroy(gtk_widget_get_toplevel(button));
}

void on_socks_ko_clicked(GtkWidget *button)
{
    gtk_widget_destroy(gtk_widget_get_toplevel(button));
}

void change_socks_settings()
{
    GtkWidget *w = create_window_socks_settings();
    char buffer[32];

    if (prefs.socks_protocol != 5) {
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(lookup_widget(w, "radiobutton_v4")), TRUE);
        gtk_widget_hide(lookup_widget(w, "frame_pwd_auth"));
    }
    else {
        gtk_widget_show(lookup_widget(w, "frame_pwd_auth"));
        gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(lookup_widget(w, "radiobutton_v5")), TRUE);
    }

    gtk_entry_set_text(GTK_ENTRY(lookup_widget(w, "entry_socks_addr")),
                prefs.socks_addr);

    sprintf(buffer, "%d", prefs.socks_port);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(w, "entry_socks_port")),
                buffer);

    gtk_widget_set_sensitive(
            lookup_widget(w, "table_pwd_auth"),
            *prefs.socks_user && *prefs.socks_password);
    gtk_toggle_button_set_active(
                GTK_TOGGLE_BUTTON(lookup_widget(w, "checkbutton_pwd_auth")),
                *prefs.socks_user && *prefs.socks_password);
    

    gtk_entry_set_text(GTK_ENTRY(lookup_widget(w, "entry_socks_user")),
                prefs.socks_user);
    gtk_entry_set_text(GTK_ENTRY(lookup_widget(w, "entry_socks_password")),
                prefs.socks_password);

    gtk_widget_show(w);
}

void color_ok (GtkColorButton *widget, color_struct *col)
{
    gtk_color_button_get_color(widget, col->color);

    if(!strcmp("background color",col->name))
        text_bg(mud->text, prefs.BackgroundGdkColor);

    update_widget_color(col);
}
    
void color_reset_to_default (GtkWidget *button, gpointer data)
{
    int i = 0;

    color_white		= default_color_white;
    color_lightwhite	= default_color_lightwhite;
    color_blue		= default_color_blue;
    color_lightblue	= default_color_lightblue;
    color_green		= default_color_green;
    color_lightgreen	= default_color_lightgreen;
    color_red		= default_color_red;
    color_lightred		= default_color_lightred;
    color_yellow		= default_color_yellow;
    color_lightyellow	= default_color_lightyellow;
    color_magenta	= default_color_magenta;
    color_lightmagenta	= default_color_lightmagenta;
    color_cyan		= default_color_cyan;
    color_lightcyan	= default_color_lightcyan;
    color_black		= default_color_black;
    color_lightblack	= default_color_lightblack;
    prefs.BackgroundGdkColor	= color_black;
    prefs.DefaultGdkColor	= color_white;
    prefs.BrightGdkColor        = color_lightwhite;

    text_bg(mud->text, prefs.BackgroundGdkColor);

    while (color_arr[i].color) {
        update_widget_color(&color_arr[i]);

        i++;
    }

}

void color_prefs_done (GtkWidget *widget, GtkWidget *dialog)
{
    int i = 0;
    
    while(color_arr[i].color) {
        color_arr[i].widget = NULL;
        i++;
    }

    gtk_widget_destroy(dialog);
}    

void color_prefs (GtkWidget *widget, GtkWidget *dummy)
{
  GtkWidget *color_box;
  GtkWidget *color_row;
  GtkWidget *color_label;
  GtkWidget *dialog;
  GtkWidget *separator;
  GtkWidget *color_hbuttonbox;
  GtkWidget *save_button;
  GtkWidget *close_button;
  GtkWidget *color_reset_button;
  GtkTooltips *tooltip;
  char  *tmp = malloc(255);
  int i = 0;

  dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(&GTK_DIALOG(dialog)->window), "Colors");
  color_box = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG (dialog)->action_area), color_box, TRUE, TRUE, 0);

  /* Make buttons for all the colors */
  while(color_arr[i].color) {     
      color_row = gtk_hbox_new(TRUE, 0);
      sprintf(tmp, "Color for %s", color_arr[i].name);
      
      color_arr[i].widget =  gtk_color_button_new_with_color(color_arr[i].color);
      gtk_color_button_set_title(GTK_COLOR_BUTTON(color_arr[i].widget), tmp);
      gtk_signal_connect (GTK_OBJECT (color_arr[i].widget), "color-set",
			  GTK_SIGNAL_FUNC (color_ok), (gpointer) &color_arr[i]);

      color_label = gtk_label_new(tmp);
      tooltip = gtk_tooltips_new ();
//      gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);
      sprintf(tmp, "You can use this button to change the %s color", color_arr[i].name);
      gtk_tooltips_set_tip (tooltip, color_arr[i].widget, tmp, NULL);
      gtk_widget_show (color_arr[i].widget);
      gtk_widget_show (color_label);
      gtk_widget_show (color_row);
      gtk_box_pack_start (GTK_BOX (color_row), color_arr[i].widget, TRUE, TRUE, 3);
      gtk_box_pack_start (GTK_BOX (color_row), color_label, TRUE, TRUE, 1);
      gtk_box_pack_start (GTK_BOX(color_box), color_row, TRUE, TRUE, 1);

      i++;
  }

  //gtk_widget_show (color_row);
  gtk_widget_show (color_box);  

  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (color_box), separator, FALSE, TRUE, 5);
  gtk_widget_show (separator);

  color_hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (color_hbuttonbox);
  gtk_box_pack_start (GTK_BOX (color_box), color_hbuttonbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (color_hbuttonbox), 3);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (color_hbuttonbox), 5);
  gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (color_hbuttonbox), 5, 0);

  save_button = gtk_button_new_from_stock (GTK_STOCK_SAVE);
  gtk_signal_connect_object (GTK_OBJECT (save_button), "clicked",
                             GTK_SIGNAL_FUNC (save_prefs),
                             NULL);
  gtk_widget_show (save_button);
  gtk_container_add (GTK_CONTAINER (color_hbuttonbox), save_button);

  color_reset_button = gtk_button_new_with_label ("Reset colors to default");
  gtk_signal_connect_object (GTK_OBJECT (color_reset_button), "clicked",
                             GTK_SIGNAL_FUNC (color_reset_to_default),
                             NULL);
  gtk_widget_show (color_reset_button);
  gtk_container_add (GTK_CONTAINER (color_hbuttonbox), color_reset_button);

  close_button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
  gtk_signal_connect (GTK_OBJECT (close_button), "clicked",
                             GTK_SIGNAL_FUNC (color_prefs_done),
			     dialog);
  gtk_widget_show (close_button);
  gtk_container_add (GTK_CONTAINER (color_hbuttonbox), close_button);

  gtk_widget_show(dialog);
}


void
on_button_browse_lua_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *w = lookup_widget((GtkWidget *)button,
            "entry_lua_script");

    change_file((GtkWidget *)button, GTK_ENTRY(w));
}


void
on_button_proxy_settings_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    change_socks_settings();
}


void
set_checkbutton(GtkWidget *w, const char *name, int var)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                lookup_widget(w, name)), var);
}

void
get_checkbutton(GtkWidget *w, const char *name, int *var)
{
    *var = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
                lookup_widget(w, name)));
}

void prefs_apply_settings(GtkWidget *prefs_window)
{
    const char *t;
    int i = 0;

    while (prefs_buttons[i].label) {
        get_checkbutton(prefs_window, 
                prefs_buttons[i].label, 
                prefs_buttons[i].value);

        i++;
    }

    gtk_text_view_set_wrap_mode(mud->text, prefs.WordWrap ? 
            GTK_WRAP_CHAR : GTK_WRAP_NONE);

    if (prefs.Toolbar)
        gtk_widget_show(lookup_widget(mud->window, "handlebox_toolbar"));
    else
        gtk_widget_hide(lookup_widget(mud->window, "handlebox_toolbar"));

    if (mud->macrobuttons) {
        if (prefs.Macrobuttons)
            gtk_widget_show(mud->macrobuttons);
        else
            gtk_widget_hide(mud->macrobuttons);
    }

    if (prefs.Statusbar )
        gtk_widget_show(lookup_widget(mud->window, "statusbar_main"));
    else
        gtk_widget_hide(lookup_widget(mud->window, "statusbar_main"));

    tick_size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                lookup_widget(prefs_window, "spinbutton_ticklength")));

    gtk_window_set_skip_taskbar_hint (GTK_WINDOW(mud->window), prefs.SkipTaskbar);

    if (use_tickcounter  )
        tickon_command( mud->activesession);
    else 
        tickoff_command( mud->activesession);

    mud->maxlines = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(
                lookup_widget(prefs_window, "spinbutton_review")));

    if ((t = gtk_entry_get_text(GTK_ENTRY(
                        lookup_widget(prefs_window, "entry_lua_script"))))) {
        if (prefs.LuaConfig) {
            free(prefs.LuaConfig);
            prefs.LuaConfig = NULL;
        }

        if (*t) {
            prefs.LuaConfig = strdup(t);
        }
    }
}

void window_prefs (GtkWidget *widget, gpointer data)
{
  GtkWidget *prefs_window = create_window_preferences();
  int i = 0;

  while (prefs_buttons[i].label) {
      set_checkbutton(prefs_window, 
                      prefs_buttons[i].label, 
                      *prefs_buttons[i].value);

      i++;
  }

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(
          lookup_widget(prefs_window, "spinbutton_ticklength")), tick_size);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(
          lookup_widget(prefs_window, "spinbutton_review")), mud->maxlines);

  gtk_entry_set_text(GTK_ENTRY(
          lookup_widget(prefs_window, "entry_lua_script")), prefs.LuaConfig ? prefs.LuaConfig : "");

  gtk_widget_show (prefs_window);
}

void
on_button_preferences_ok_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *w = gtk_widget_get_toplevel((GtkWidget *)button);

    prefs_apply_settings(w);
    gtk_widget_destroy(w);
}

void
on_button_preferences_save_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
    prefs_apply_settings(gtk_widget_get_toplevel((GtkWidget *)button));
    save_prefs((GtkWidget *)button, user_data);
}


