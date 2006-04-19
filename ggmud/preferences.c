/*  GGMud
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

/* Global variables */
extern GtkWidget *btnLabel[12];
extern GtkWidget *handlebox;
extern GtkWidget *statusbar;

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
    popup_window(WARN, "At the moment trigger params (%%d), (%%w)...\nand other custom zmud settings are not imported!");

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
        gchar *file = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(filew));
        read_command(file, NULL);
        g_free(file);
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
 
void load_prefs () 
{
    FILE *fp;
    gchar line[255], pref[100], value[230];

//    prefs.BackgroundColor = color_black;
//    prefs.BackgroundColor.pixel = 0;
//    prefs.DefaultColor = color_white;

    if ((fp = fileopen(PREFS_FILE, "r"))) {
        prefs.SaveVars = prefs.Blinking = prefs.KeepText = prefs.EchoText  = prefs.WordWrap = prefs.DoBeep = TRUE;
        prefs.LuaConfig = NULL;

        while (fgets (line, sizeof(line) - 1, fp)) {
            sscanf (line, "%[^=]=%[^\n]", pref, value);
            if (!strcmp(value, "Off")) {
                if (!strcmp (pref, "KeepText")) {
                    prefs.KeepText = FALSE;
                } else if (!strcmp (pref, "SaveVars")) {
                    prefs.SaveVars = FALSE;
                } else if (!strcmp (pref, "Blinking")) {
                    prefs.Blinking = FALSE;
                } else if (!strcmp (pref, "EchoText")) {
                    prefs.EchoText = FALSE;
                } else if (!strcmp (pref, "Wordwrap")) {
                    prefs.WordWrap = FALSE;
                } else if (!strcmp (pref, "Beep")) {
                    prefs.DoBeep = FALSE;
                } else if (!strcmp(pref, "Toolbar")) {
                    prefs.Toolbar = FALSE;
                } else if (!strcmp (pref, "Macrobuttons")) {
                    prefs.Macrobuttons = FALSE;
                } else if (!strcmp (pref, "Statusbar")) {
                    prefs.Statusbar = FALSE;
                }
            } else if (!strcmp(pref, "ReviewSize")) {
                int temp = atoi(value);
                
                if(temp > 100 && temp < 100000)
                    mud->maxlines = temp;
            } else if (!strcmp(pref, "TickSize")) {
                int temp = atoi(value);
                
                if(temp > 10 && temp < 1000)
                    tick_size = temp;
            } else if (!strcmp(pref, "TickCounter")) {
                if(!strcmp(value, "On")) {
                    use_tickcounter = 1;
                } else {
                    use_tickcounter = 0;
                }
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

void change_script(GtkEntry *entry, PREFS_DATA *p)
{
        const char *text = gtk_entry_get_text(entry);

        if (!p->LuaConfig || strcmp(p->LuaConfig, text)) {
            if (p->LuaConfig)
                    free(p->LuaConfig);

            if (*text)
                p->LuaConfig = strdup(text);
            else
                p->LuaConfig = NULL;
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

void save_prefs (GtkWidget *button, gpointer data) 
{
#define CFGW(x,y) fprintf(fp, "%s=%s\n", x, y ? "On" : "Off")
#define CFGI(x,y) fprintf(fp, "%s=%d\n", x, y)
#define CFGS(x,y) fprintf(fp, "%s=%s\n", x, y)
    FILE *fp;
    
    if ((fp = fileopen (PREFS_FILE, "w"))) {
        int i=0;
        extern int tick_size;
        
        CFGW("KeepText", prefs.KeepText);
    	CFGW("EchoText", prefs.EchoText);
        CFGW("Wordwrap", prefs.WordWrap);
        CFGW("Blinking", prefs.Blinking);
        CFGW("SaveVars", prefs.SaveVars);
    	CFGW("Beep", prefs.DoBeep);
        CFGW("Toolbar", prefs.Toolbar);
        CFGW("Macrobuttons", prefs.Macrobuttons);
        CFGW("Statusbar", prefs.Statusbar);
        CFGI("TickSize", tick_size);
        CFGI("ReviewSize", mud->maxlines);
        CFGS("LuaConfig", prefs.LuaConfig);

        CFGW("TickCounter", use_tickcounter);

        while(color_arr[i].name) {
    	    fprintf(fp, "%s=(%u,%u,%u)\n",color_arr[i].name,
	    	color_arr[i].color->red, color_arr[i].color->green,
		    color_arr[i].color->blue);
    	    i++;
    	}
    	fclose (fp);
    }
}

void change_tick_size (GtkWidget *widget, ggmud *mud)
{
    tick_size = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
}

void change_review_size (GtkWidget *widget, ggmud *mud)
{
    mud->maxlines = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
}

void check_tickcounter (GtkWidget *widget, GtkWidget *entry_TickSize)
{
    if ( GTK_TOGGLE_BUTTON (widget)->active ) {
        tickon_command( mud->activesession);
        use_tickcounter = 1;
    }
    else {
        tickoff_command( mud->activesession);
        use_tickcounter = 0;
    }

    gtk_widget_set_sensitive(entry_TickSize, GTK_TOGGLE_BUTTON (widget)->active);
}

static void check_toggle (GtkWidget *widget, gint *var)
{
    if ( GTK_TOGGLE_BUTTON (widget)->active )
        *var = TRUE;
    else
        *var = FALSE;
}

/* wordwrapper for the main textwindow */
void text_toggle_word_wrap (GtkWidget *checkbutton_wrap, GtkWidget *text)
{
  gtk_text_view_set_wrap_mode(mud->text, GTK_TOGGLE_BUTTON(checkbutton_wrap)->active ? 
          GTK_WRAP_CHAR : GTK_WRAP_NONE);
}

void toggle_visibility(GtkWidget *widget, GtkWidget *dest)
{
  if ( GTK_TOGGLE_BUTTON (widget)->active )
      gtk_widget_show(dest);
  else
      gtk_widget_hide (dest);
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

  save_button = gtk_button_new_with_label ("Save");
  gtk_signal_connect_object (GTK_OBJECT (save_button), "clicked",
                             GTK_SIGNAL_FUNC (save_prefs),
                             NULL);
  gtk_widget_show (save_button);
  gtk_container_add (GTK_CONTAINER (color_hbuttonbox), save_button);
  gtk_container_border_width (GTK_CONTAINER (save_button), 3);

  color_reset_button = gtk_button_new_with_label ("Reset colors to default");
  gtk_signal_connect_object (GTK_OBJECT (color_reset_button), "clicked",
                             GTK_SIGNAL_FUNC (color_reset_to_default),
                             NULL);
  gtk_widget_show (color_reset_button);
  gtk_container_add (GTK_CONTAINER (color_hbuttonbox), color_reset_button);
  gtk_container_border_width (GTK_CONTAINER (color_reset_button), 3);

  close_button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (close_button), "clicked",
                             GTK_SIGNAL_FUNC (color_prefs_done),
			     dialog);
  gtk_widget_show (close_button);
  gtk_container_add (GTK_CONTAINER (color_hbuttonbox), close_button);
  gtk_container_border_width (GTK_CONTAINER (close_button), 3);

  gtk_widget_show(dialog);
}

void window_prefs (GtkWidget *widget, gpointer data)
{
  GtkWidget *prefs_window;
  GtkWidget *vbox, *hbox, *vbox2, *vbox3;
  GtkWidget *frame_entry;
  GtkWidget *frame_vbox;
  GtkWidget *checkbutton_keep;
  GtkWidget *checkbutton_echo;
  GtkWidget *frame_text;
  GtkWidget *frame_vbox_text;
  GtkWidget *checkbutton_wrap;
  GtkWidget *checkbutton_blinking;
  GtkWidget *checkbutton_beep;
  GtkWidget *frame_misc, *frame_new;
  GtkWidget *frame_vbox_misc;
  GtkWidget *temp;
  GtkTooltips *tooltip;
  GtkWidget *checkbutton_Toolbar;
  GtkWidget *checkbutton_Macrobuttons;
  GtkWidget *checkbutton_Statusbar;
  GtkWidget *checkbutton_savevars;
  GtkWidget *checkbutton_Tickcounter;
  GtkWidget *entry_TickSize;
  GtkWidget *entry_ReviewSize;
  GtkWidget *entry_LuaConfig;
  GtkWidget *im;
  
  prefs_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (prefs_window), "Preferences");
  gtk_window_set_policy (GTK_WINDOW (prefs_window), TRUE, TRUE, TRUE);
  gtk_signal_connect (GTK_OBJECT (prefs_window), "destroy",
                             GTK_SIGNAL_FUNC(close_window), prefs_window );

  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (prefs_window), vbox);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 0);
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);

  frame_entry = gtk_frame_new ("Entry box Options");
  gtk_widget_show (frame_entry);
  gtk_box_pack_start (GTK_BOX (vbox2), frame_entry, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame_entry), 7);
  gtk_frame_set_label_align (GTK_FRAME (frame_entry), 0.07, 0.5);

  frame_vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (frame_vbox);
  gtk_container_add (GTK_CONTAINER (frame_entry), frame_vbox);
  gtk_container_set_border_width (GTK_CONTAINER (frame_vbox), 6);

  checkbutton_keep = gtk_check_button_new_with_label ("Keep Text Entered");
  gtk_signal_connect (GTK_OBJECT (checkbutton_keep), "toggled",
                      GTK_SIGNAL_FUNC (check_toggle),
                      &prefs.KeepText);
  tooltip = gtk_tooltips_new ();
//  gtk_tooltips_set_colors (tooltip1, &color_lightyellow, &color_black);
  gtk_tooltips_set_tip (tooltip, checkbutton_keep,
                        "With this toggled on, the text you have entered and sent "
                        "to the connection, will be left in the entry box but "
                        "seleceted.", NULL);
  gtk_widget_show (checkbutton_keep);
  gtk_box_pack_start (GTK_BOX (frame_vbox), checkbutton_keep, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_keep), prefs.KeepText);

  checkbutton_echo = gtk_check_button_new_with_label ("Echo Text");
  gtk_signal_connect (GTK_OBJECT (checkbutton_echo), "toggled",
                      GTK_SIGNAL_FUNC (check_toggle), &prefs.EchoText);
  
  gtk_tooltips_set_tip (tooltip, checkbutton_echo,
                        "With this toggled on, all the text you type and "
                        "enter will be echoed on the connection so you can "
                        "control what you are sending."
                        "\nSome people think this is annoying, and therefor this "
                        "is an options.",
                        NULL);
  gtk_widget_show (checkbutton_echo);
  gtk_box_pack_start (GTK_BOX (frame_vbox), checkbutton_echo, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_echo), prefs.EchoText);

  frame_text = gtk_frame_new ("Main Window Options");
  gtk_widget_show (frame_text);
  gtk_box_pack_start (GTK_BOX (vbox2), frame_text, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame_text), 7);
  gtk_frame_set_label_align (GTK_FRAME (frame_text), 0.07, 0.5);

  frame_vbox_text = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (frame_vbox_text);
  gtk_container_add (GTK_CONTAINER (frame_text), frame_vbox_text);
  gtk_container_set_border_width (GTK_CONTAINER (frame_vbox_text), 6);

  checkbutton_wrap = gtk_check_button_new_with_label ("Word Wrap");
  gtk_signal_connect (GTK_OBJECT (checkbutton_wrap), "toggled",
                      GTK_SIGNAL_FUNC (check_toggle), &prefs.WordWrap);
  gtk_signal_connect (GTK_OBJECT(checkbutton_wrap), "toggled",
		      GTK_SIGNAL_FUNC(text_toggle_word_wrap), mud->text);

  gtk_tooltips_set_tip (tooltip, checkbutton_wrap,
                        "Wordwrap the lines in the main window!",
                        NULL);
  gtk_widget_show (checkbutton_wrap);
  gtk_box_pack_start (GTK_BOX (frame_vbox_text), checkbutton_wrap, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_wrap), prefs.WordWrap);

  checkbutton_blinking = gtk_check_button_new_with_label ("Text blinking");
  gtk_signal_connect (GTK_OBJECT (checkbutton_blinking), "toggled",
                      GTK_SIGNAL_FUNC (check_toggle), &prefs.Blinking);

  gtk_tooltips_set_tip (tooltip, checkbutton_blinking,
                        "Enable/disable text blinking.",
                        NULL);
  gtk_widget_show (checkbutton_blinking);
  gtk_box_pack_start (GTK_BOX (frame_vbox_text), checkbutton_blinking, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_blinking), prefs.Blinking);

  checkbutton_beep = gtk_check_button_new_with_label ("Emit Beeps");
  gtk_signal_connect (GTK_OBJECT (checkbutton_beep), "toggled",
                      GTK_SIGNAL_FUNC (check_toggle),
                      &prefs.DoBeep);

  gtk_tooltips_set_tip (tooltip, checkbutton_beep,
                        "If enabled SClient will emit the beep (system bell) sound.",
                        NULL);
  gtk_widget_show (checkbutton_beep);
  gtk_box_pack_start (GTK_BOX (frame_vbox_text), checkbutton_beep, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_beep), prefs.DoBeep);

  frame_misc = gtk_frame_new ("General Options");
  gtk_widget_show (frame_misc);
  gtk_box_pack_start (GTK_BOX (vbox2), frame_misc, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame_misc), 7);
  gtk_frame_set_label_align (GTK_FRAME (frame_misc), 0.07, 0.5);

  frame_vbox_misc = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (frame_vbox_misc);
  gtk_container_add (GTK_CONTAINER (frame_misc), frame_vbox_misc);
  gtk_container_set_border_width (GTK_CONTAINER (frame_vbox_misc), 6);

  checkbutton_Toolbar = gtk_check_button_new_with_label ("Show/Hide Toolbar");
  gtk_signal_connect (GTK_OBJECT (checkbutton_Toolbar),"toggled",
                       GTK_SIGNAL_FUNC (check_toggle), &prefs.Toolbar);
  gtk_signal_connect (GTK_OBJECT (checkbutton_Toolbar),"toggled",
                       GTK_SIGNAL_FUNC (toggle_visibility), handlebox);

  gtk_tooltips_set_tip (tooltip, checkbutton_Toolbar,
                        "Toggle this on to hide the Toolbar.",
                        NULL);
  gtk_widget_show (checkbutton_Toolbar);
  gtk_box_pack_start (GTK_BOX (frame_vbox_misc), checkbutton_Toolbar, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_Toolbar), prefs.Toolbar);


  checkbutton_Macrobuttons = gtk_check_button_new_with_label ("Show/Hide Macro buttons");
  gtk_signal_connect (GTK_OBJECT (checkbutton_Macrobuttons),"toggled",
                       GTK_SIGNAL_FUNC (check_toggle), &prefs.Macrobuttons);
  gtk_signal_connect (GTK_OBJECT (checkbutton_Macrobuttons),"toggled",
                       GTK_SIGNAL_FUNC (toggle_visibility), mud->macrobuttons);

  gtk_tooltips_set_tip (tooltip, checkbutton_Macrobuttons,
                        "Toggle this on to hide the Macro buttons.",
                        NULL);
  gtk_widget_show (checkbutton_Macrobuttons);
  gtk_box_pack_start (GTK_BOX (frame_vbox_misc), checkbutton_Macrobuttons, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_Macrobuttons), prefs.Macrobuttons);

  checkbutton_Statusbar = gtk_check_button_new_with_label ("Show/Hide Statusbar");
  
  gtk_signal_connect (GTK_OBJECT (checkbutton_Statusbar),"toggled",
                       GTK_SIGNAL_FUNC (check_toggle), &prefs.Statusbar);
  gtk_signal_connect (GTK_OBJECT (checkbutton_Statusbar),"toggled",
                       GTK_SIGNAL_FUNC (toggle_visibility), statusbar );

  gtk_tooltips_set_tip (tooltip, checkbutton_Statusbar,
                        "Toggle this on to hide the Statusbar.",
                        NULL);
  gtk_widget_show (checkbutton_Statusbar);
  gtk_box_pack_start (GTK_BOX (frame_vbox_misc), checkbutton_Statusbar, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_Statusbar), prefs.Statusbar);

  
// new options
  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 0);

// ticks
  frame_new = gtk_frame_new ("Tickcounter Options");
  gtk_widget_show (frame_new);
  gtk_box_pack_start (GTK_BOX (vbox2), frame_new, TRUE, TRUE, 0);
  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);

  
  gtk_container_add(GTK_CONTAINER(frame_new), vbox3);
  
// tickcounter
  checkbutton_Tickcounter = gtk_check_button_new_with_label ("Use TickCounter");

  gtk_tooltips_set_tip (tooltip, checkbutton_Tickcounter,
                        "Toggle this to enable/disable the builtin tickcounter.",
                        NULL);
  
  gtk_widget_show (checkbutton_Tickcounter);
  
  gtk_box_pack_start (GTK_BOX (vbox3), checkbutton_Tickcounter, TRUE, TRUE, 0);
// ticksize
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox, TRUE, TRUE, 0);
  temp = gtk_label_new("Tick length:");
  gtk_widget_show(temp);
  gtk_box_pack_start (GTK_BOX (hbox), temp, TRUE, TRUE, 0);
  temp = (GtkWidget *)gtk_adjustment_new (tick_size, 10, 1000, 1, 10, 10);
  entry_TickSize = gtk_spin_button_new (GTK_ADJUSTMENT (temp), 1, 0);

  gtk_signal_connect(GTK_OBJECT(entry_TickSize), "changed", 
                        GTK_SIGNAL_FUNC(change_tick_size), mud);
  
  gtk_signal_connect (GTK_OBJECT (checkbutton_Tickcounter),"toggled",
                       GTK_SIGNAL_FUNC (check_tickcounter), entry_TickSize);
  
  gtk_widget_show(entry_TickSize);

  if (use_tickcounter)
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbutton_Tickcounter), TRUE);
  else 
      gtk_widget_set_sensitive(entry_TickSize, FALSE);
  
  gtk_box_pack_start (GTK_BOX (hbox), entry_TickSize, TRUE, TRUE, 0);
  gtk_tooltips_set_tip(tooltip, entry_TickSize,
          "The length of the tick in seconds", NULL);
  
// other
  frame_new = gtk_frame_new ("Other Options");
  gtk_widget_show (frame_new);
  gtk_box_pack_start (GTK_BOX (vbox2), frame_new, TRUE, TRUE, 0);
  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add(GTK_CONTAINER(frame_new), vbox3);

// Lua configuration
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox, FALSE, FALSE, 0);
 
  temp = gtk_label_new("Lua startup script:");
  gtk_widget_show(temp);
  gtk_box_pack_start (GTK_BOX (hbox), temp, TRUE, TRUE, 0);
 
  entry_LuaConfig = gtk_entry_new();
  if (prefs.LuaConfig)
          gtk_entry_set_text(GTK_ENTRY(entry_LuaConfig), prefs.LuaConfig);

  gtk_widget_show(entry_LuaConfig);
  gtk_box_pack_start (GTK_BOX (hbox), entry_LuaConfig, TRUE, TRUE, 0);
  temp = gtk_button_new();
  im = gtk_image_new_from_stock(GTK_STOCK_OPEN, GTK_ICON_SIZE_BUTTON);
  gtk_container_add(GTK_CONTAINER(temp), im);
  gtk_widget_show(im);
  gtk_widget_show(temp);
  gtk_box_pack_start (GTK_BOX (hbox), temp, FALSE, FALSE, 0);
  gtk_signal_connect(GTK_OBJECT(temp), "clicked", GTK_SIGNAL_FUNC(change_file),
                  entry_LuaConfig);
  gtk_signal_connect(GTK_OBJECT(entry_LuaConfig), "changed", GTK_SIGNAL_FUNC(change_script),
                        &prefs);
// review size
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox, TRUE, TRUE, 0);
 
  temp = gtk_label_new("Review lines:");
  gtk_widget_show(temp);
  gtk_box_pack_start (GTK_BOX (hbox), temp, TRUE, TRUE, 0);
  checkbutton_savevars = gtk_check_button_new_with_label ("Save variables");
  gtk_signal_connect (GTK_OBJECT (checkbutton_savevars), "toggled",
                      GTK_SIGNAL_FUNC (check_toggle), &prefs.SaveVars);

  gtk_tooltips_set_tip (tooltip, checkbutton_savevars,
                        "Save variable values on program exit.",
                        NULL);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_savevars), prefs.SaveVars);
  gtk_widget_show (checkbutton_savevars);
  gtk_box_pack_start (GTK_BOX (vbox3), checkbutton_savevars, TRUE, TRUE, 0);
  
  temp = (GtkWidget *)gtk_adjustment_new (mud->maxlines, 1000, 1000000, 100, 10000, 10000);
  entry_ReviewSize = gtk_spin_button_new (GTK_ADJUSTMENT (temp), 1, 0);

  gtk_signal_connect(GTK_OBJECT(entry_ReviewSize), "changed", 
                        GTK_SIGNAL_FUNC(change_review_size), mud);
  gtk_widget_show(entry_ReviewSize);

  gtk_tooltips_set_tip(tooltip, entry_ReviewSize,
          "The size in lines of the review buffer.",
          NULL);
  gtk_box_pack_start (GTK_BOX (hbox), entry_ReviewSize, TRUE, TRUE, 0);
  
// button box
  AddSimpleBar(vbox, NULL, 
          GTK_SIGNAL_FUNC(save_prefs), GTK_SIGNAL_FUNC(close_window));
  
  gtk_widget_show (prefs_window);
}

