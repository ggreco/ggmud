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
#include "tintin.h"
#include "ggmud.h"
#include "ansi.h"
#include "preferences.h"	/* Needs to be after sclient.h */

#ifdef WIN32
#define PREFS_FILE "ggmud.prf"
#else
#define PREFS_FILE "Preference"
#endif

/* Global variables */
PREFS_DATA prefs;
GtkWidget   *prefs_window;
GtkWidget   *prefs_button_save;
GtkWidget *checkbutton_Toolbar;
GtkWidget *checkbutton_Macrobuttons;
GtkWidget *checkbutton_Statusbar;
GtkWidget *checkbutton_Tickcounter;
GtkWidget *entry_TickSize;
GtkWidget *entry_ReviewSize;
int use_tickcounter = 0;


color_struct color_arr[] = {
      {&color_white, "white"},
      {&color_lightwhite, "light white"},
      {&color_blue, "blue"},
      {&color_lightblue, "light blue"},
      {&color_red, "red"},
      {&color_lightred, "light red"},
      {&color_green, "green"},
      {&color_lightgreen, "light green"},
      {&color_yellow, "yellow"},
      {&color_lightyellow, "light yellow"},
      {&color_cyan, "cyan"},
      {&color_lightcyan, "light cyan"},
      {&color_magenta, "magenta"},
      {&color_lightmagenta, "light magenta"},
      {&color_black, "black"},
      {&color_lightblack, "light black"},
      {&prefs.BackgroundColor, "background color"},
      {&prefs.DefaultColor, "default color"},      
      {NULL,NULL}
};

gdouble *gdk_color_to_gdouble (GdkColor *gdkcolor)
{
    gdouble *color = malloc(sizeof(gdouble)*3);
    
    color[0] = (double)gdkcolor->red / 65535;
    color[1] = (double)gdkcolor->green / 65535;
    color[2] = (double)gdkcolor->blue / 65535;

    return color;
}
 
void load_prefs () 
{
    FILE *fp;
    gchar line[255], pref[100], value[230];

    prefs.BackgroundColor = color_black;
    prefs.BackgroundColor.pixel = 0;
    prefs.DefaultColor = color_white;

    if (fp = fileopen(PREFS_FILE, "r")) {
        prefs.KeepText = prefs.EchoText  = prefs.WordWrap = prefs.DoBeep = TRUE;

        while (fgets (line, sizeof(line) - 1, fp)) {
            sscanf (line, "%[^=]=%[^\n]", pref, value);
            if (!strcmp(value, "Off")) {
                if (!strcmp (pref, "KeepText")) {
                    prefs.KeepText = FALSE;
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
                extern int tick_size;
                int temp = atoi(value);
                
                if(temp > 10 && temp < 1000)
                    tick_size = temp;
            } else if (!strcmp(pref, "TickCounter")) {
                if(!strcmp(value, "On")) {
                    use_tickcounter = 1;
                } else {
                    use_tickcounter = 0;
                }
            } else { // import colors
                int i = 0;
                
                while(color_arr[i].name) {	    
                    if(!strcmp (color_arr[i].name, pref)) {
                        sscanf(value, "(%u,%u,%u)",
                                &(color_arr[i].color->red),
                                &(color_arr[i].color->green),
                                &(color_arr[i].color->blue));
                        if(!gdk_color_alloc(gdk_colormap_get_system(), color_arr[i].color)) {
                            g_error("Couldn't allocate background color - reverting to black\n");
                            color_arr[i].color->pixel = color_black.pixel;
                            color_arr[i].color->red = color_black.red;
                            color_arr[i].color->green = color_black.green;
                            color_arr[i].color->blue = color_black.blue;
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

void save_prefs (GtkWidget *button, gpointer data) 
{
#define CFGW(x,y) fprintf(fp, "%s=%s\n", x, y ? "On" : "Off")
#define CFGI(x,y) fprintf(fp, "%s=%d\n", x, y)
    FILE *fp;
    
    if (fp = fileopen (PREFS_FILE, "w")) {
        int i=0;
        extern int tick_size;
        
        CFGW("KeepText", prefs.KeepText);
    	CFGW("EchoText", prefs.EchoText);
        CFGW("Wordwrap", prefs.WordWrap);
    	CFGW("Beep", prefs.DoBeep);
        CFGW("Toolbar", prefs.Toolbar);
        CFGW("Macrobuttons", prefs.Macrobuttons);
        CFGW("Statusbar", prefs.Statusbar);
        CFGI("TickSize", tick_size);
        CFGI("ReviewSize", mud->maxlines);

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

void check_text_toggle (GtkWidget *widget, GtkWidget *button)
{
    if ( GTK_TOGGLE_BUTTON (button)->active )
        prefs.KeepText = TRUE;
    else
        prefs.KeepText = FALSE;
}

void check_tickcounter (GtkWidget *widget, GtkWidget *button)
{
    if ( GTK_TOGGLE_BUTTON (button)->active ) {
        parse_input("#tickon", mud->activesession);
        use_tickcounter = 1;
    }
    else {
        parse_input("#tickoff", mud->activesession);
        use_tickcounter = 0;
    }

    gtk_widget_set_sensitive(entry_TickSize, GTK_TOGGLE_BUTTON (button)->active);
}

void check_callback (GtkWidget *widget, GtkWidget *check_button)
{
    if ( GTK_TOGGLE_BUTTON (check_button)->active )
        prefs.EchoText = TRUE;
    else
        prefs.EchoText = FALSE;
}

void check_wrap (GtkWidget *widget, GtkWidget *wrap_button)
{
    if ( GTK_TOGGLE_BUTTON (wrap_button)->active )
        prefs.WordWrap = TRUE;
    else
        prefs.WordWrap = FALSE;
}

/* wordwrapper for the main textwindow */
void text_toggle_word_wrap (GtkWidget *checkbutton_wrap, GtkWidget *text)
{
  gtk_text_set_word_wrap(GTK_TEXT(mud->text), GTK_TOGGLE_BUTTON(checkbutton_wrap)->active);
}

void check_beep (GtkWidget *widget, GtkWidget *check_button)
{
    if ( GTK_TOGGLE_BUTTON (check_button)->active )
        prefs.DoBeep = TRUE;
    else
        prefs.DoBeep = FALSE;
}

/*
 * check to see if we should show or hide the toolbar
 */
void check_Toolbar (GtkWidget *widget, GtkWidget *check_button_toolbar)
{
    if ( GTK_TOGGLE_BUTTON (check_button_toolbar)->active )
        prefs.Toolbar = TRUE;
    else
        prefs.Toolbar = FALSE;

  if ( GTK_TOGGLE_BUTTON (check_button_toolbar)->active )
      gtk_widget_show(handlebox);
  else
      gtk_widget_hide (handlebox);

}

/* 
 * check to see if we should show or hide the Macro buttons
 */
void check_Macrobuttons (GtkWidget *widget, GtkWidget *check_button_macro)
{
    if ( GTK_TOGGLE_BUTTON (check_button_macro)->active )
        prefs.Macrobuttons = TRUE;
    else
        prefs.Macrobuttons = FALSE;

  if ( GTK_TOGGLE_BUTTON (check_button_macro)->active )
      gtk_widget_show(mud->macrobuttons);
  else
      gtk_widget_hide (mud->macrobuttons);
}

/* 
 * check to see if we should show or hide the Statusbar
 */
void check_Statusbar (GtkWidget *widget, GtkWidget *check_button_statusbar)
{
    if ( GTK_TOGGLE_BUTTON (check_button_statusbar)->active )
        prefs.Statusbar = TRUE;
    else
        prefs.Statusbar = FALSE;

  if ( GTK_TOGGLE_BUTTON (check_button_statusbar)->active )
      gtk_widget_show(statusbar);
  else
      gtk_widget_hide (statusbar);
}

void color_ok (GtkWidget *widget, GtkWidget *color_sel)
{
    gdouble fcolor[3];
    color_struct *color;
    GdkColor new_color;
    
    gtk_color_selection_get_color(GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(color_sel)->colorsel), fcolor);
    /* FIXME: Need to make the change take effect at once */
    color = (color_struct *)gtk_object_get_data (GTK_OBJECT(color_sel), "color");
    new_color.red = 65535 * fcolor[0];
    new_color.green = 65535 * fcolor[1];
    new_color.blue = 65535 * fcolor[2];
    if(!gdk_color_alloc(cmap,&new_color)) {
	g_error("Couldn't allocate color - keeping old color\n");
    } else {
	color->color->red = new_color.red;
	color->color->blue = new_color.blue;
	color->color->green = new_color.green;
	color->color->pixel = new_color.pixel;
    }
    if(!strcmp("background color",color->name))
	gdk_window_set_background(GTK_TEXT(mud->text)->text_area, &prefs.BackgroundColor);

    gtk_widget_destroy(color_sel);
}

void color_cancel (GtkWidget *widget, GtkWidget *color_sel)
{
    char *str;
    
    str = (char *) gtk_object_get_data(GTK_OBJECT(color_sel), "name");
    free(str);
    gtk_widget_destroy(color_sel);
}

void color_callback (GtkWidget *widget, color_struct *color)
{
    GtkWidget *color_sel;
    gdouble *fcolor;
    char *str = malloc(255);

    sprintf(str, "Set color for %s", color->name);
    color_sel = gtk_color_selection_dialog_new (str);
    fcolor = gdk_color_to_gdouble(color->color);
    gtk_color_selection_set_color (GTK_COLOR_SELECTION(GTK_COLOR_SELECTION_DIALOG(color_sel)->colorsel),
				   fcolor);
    gtk_object_set_data (GTK_OBJECT (color_sel), "color", color);
    gtk_object_set_data (GTK_OBJECT (color_sel), "name", str);
    gtk_signal_connect( GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(color_sel)->ok_button),
			"clicked", GTK_SIGNAL_FUNC(color_ok),
			color_sel);
    gtk_signal_connect( GTK_OBJECT(GTK_COLOR_SELECTION_DIALOG(color_sel)->cancel_button),
			"clicked", GTK_SIGNAL_FUNC(color_cancel),
			color_sel);
    gtk_widget_show(color_sel);
    free(fcolor);
}

void color_reset_to_default (GtkWidget *button, gpointer data)
{

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
    prefs.BackgroundColor	= color_black;
    prefs.DefaultColor	= color_white;

    gdk_window_set_background(GTK_TEXT(mud->text)->text_area, &prefs.BackgroundColor);

}

void color_prefs_done (GtkWidget *widget, GtkWidget *dialog)
{
    gtk_widget_destroy(dialog);
}    

void color_prefs (GtkWidget *widget, GtkWidget *dummy)
{
  GtkWidget *color_box;
  GtkWidget *color_row;
  GtkWidget *color_button;
  GtkWidget *color_label;
  GtkWidget *dialog;
  GtkWidget *separator;
  GtkWidget *color_hbuttonbox;
  GtkWidget *save_button;
  GtkWidget *close_button;
  GtkWidget *color_reset_button;
  GtkTooltips *tooltip;
  char i = 0, *tmp = malloc(255);

  dialog = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(&GTK_DIALOG(dialog)->window), "Colors");
  color_box = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG (dialog)->action_area), color_box, TRUE, TRUE, 0);

  /* Make buttons for all the colors */
  while(color_arr[i].color) {     
      GtkStyle *style;
      
      color_row = gtk_hbox_new(TRUE, 0);
      color_button = gtk_button_new();

      sprintf(tmp, "Color for %s", color_arr[i].name);
      gtk_object_set_data (GTK_OBJECT (color_button), "color", &color_arr[i]);
      color_label = gtk_label_new(tmp);
      gtk_signal_connect (GTK_OBJECT (color_button), "clicked",
			  GTK_SIGNAL_FUNC (color_callback), (gpointer) &color_arr[i]);
      tooltip = gtk_tooltips_new ();
//      gtk_tooltips_set_colors (tooltip, &color_lightyellow, &color_black);
      sprintf(tmp, "You can use this button to change the %s color", color_arr[i].name);
      gtk_tooltips_set_tip (tooltip, color_button, tmp, NULL);
      gtk_widget_show (color_button);
      gtk_widget_show (color_label);
      gtk_widget_show (color_row);
      gtk_box_pack_start (GTK_BOX (color_row), color_button, TRUE, TRUE, 3);
      gtk_box_pack_start (GTK_BOX (color_row), color_label, TRUE, TRUE, 1);
      gtk_box_pack_start (GTK_BOX(color_box), color_row, TRUE, TRUE, 1);
      
      style = gtk_style_copy(gtk_widget_get_style(color_button));
      
      style->bg[0] = *color_arr[i].color;
      
      gtk_widget_set_style(color_button, style);

      gtk_style_unref(style);
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
  GtkWidget *checkbutton_beep;
  GtkWidget *frame_misc, *frame_new;
  GtkWidget *frame_vbox_misc;
  GtkWidget *prefs_hbuttonbox;
  GtkWidget *save_button;
  GtkWidget *close_button;
  GtkWidget *temp;
  GtkTooltips *tooltip;

  prefs_window = gtk_window_new (GTK_WINDOW_DIALOG);
  gtk_window_set_title (GTK_WINDOW (prefs_window), "Preferences");
  gtk_window_set_policy (GTK_WINDOW (prefs_window), FALSE, FALSE, FALSE);
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
                      GTK_SIGNAL_FUNC (check_text_toggle),
                      checkbutton_keep);
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
                      GTK_SIGNAL_FUNC (check_callback), checkbutton_echo);
  
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
                      GTK_SIGNAL_FUNC (check_wrap), checkbutton_wrap);
  gtk_signal_connect (GTK_OBJECT(checkbutton_wrap), "toggled",
		      GTK_SIGNAL_FUNC(text_toggle_word_wrap), mud->text);

  gtk_tooltips_set_tip (tooltip, checkbutton_wrap,
                        "Wordwrap the lines in the main window!",
                        NULL);
  gtk_widget_show (checkbutton_wrap);
  gtk_box_pack_start (GTK_BOX (frame_vbox_text), checkbutton_wrap, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_wrap), prefs.WordWrap);

  checkbutton_beep = gtk_check_button_new_with_label ("Emit Beeps");
  gtk_signal_connect (GTK_OBJECT (checkbutton_beep), "toggled",
                      GTK_SIGNAL_FUNC (check_beep),
                      checkbutton_beep);

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
                       GTK_SIGNAL_FUNC (check_Toolbar),checkbutton_Toolbar);
  gtk_tooltips_set_tip (tooltip, checkbutton_Toolbar,
                        "Toggle this on to hide the Toolbar.",
                        NULL);
  gtk_widget_show (checkbutton_Toolbar);
  gtk_box_pack_start (GTK_BOX (frame_vbox_misc), checkbutton_Toolbar, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_Toolbar), prefs.Toolbar);


  checkbutton_Macrobuttons = gtk_check_button_new_with_label ("Show/Hide Macro buttons");
  gtk_signal_connect (GTK_OBJECT (checkbutton_Macrobuttons),"toggled",
                       GTK_SIGNAL_FUNC (check_Macrobuttons),checkbutton_Macrobuttons);

  gtk_tooltips_set_tip (tooltip, checkbutton_Macrobuttons,
                        "Toggle this on to hide the Macro buttons.",
                        NULL);
  gtk_widget_show (checkbutton_Macrobuttons);
  gtk_box_pack_start (GTK_BOX (frame_vbox_misc), checkbutton_Macrobuttons, TRUE, TRUE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton_Macrobuttons), prefs.Macrobuttons);

  checkbutton_Statusbar = gtk_check_button_new_with_label ("Show/Hide Statusbar");
  gtk_signal_connect (GTK_OBJECT (checkbutton_Statusbar),"toggled",
                       GTK_SIGNAL_FUNC (check_Statusbar),checkbutton_Statusbar);

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
  gtk_signal_connect (GTK_OBJECT (checkbutton_Tickcounter),"toggled",
                       GTK_SIGNAL_FUNC (check_tickcounter),checkbutton_Tickcounter);

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
  entry_TickSize = gtk_entry_new();
  gtk_widget_show(entry_TickSize);

  if (mud->activesession && mud->activesession->tickstatus)
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

// review size
  hbox = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox);
  gtk_box_pack_start (GTK_BOX (vbox3), hbox, TRUE, TRUE, 0);
 
  temp = gtk_label_new("Review size:");
  gtk_widget_show(temp);
  gtk_box_pack_start (GTK_BOX (hbox), temp, TRUE, TRUE, 0);
  entry_ReviewSize = gtk_entry_new();
  gtk_widget_show(entry_ReviewSize);

  gtk_tooltips_set_tip(tooltip, entry_ReviewSize,
          "The size in bytes of the review buffer, use about 50 x number of lines you need.",
          NULL);
  gtk_box_pack_start (GTK_BOX (hbox), entry_ReviewSize, TRUE, TRUE, 0);
  
// button box
  prefs_hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (prefs_hbuttonbox);
  gtk_box_pack_start (GTK_BOX (vbox), prefs_hbuttonbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (prefs_hbuttonbox), 7);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (prefs_hbuttonbox), 10);
  gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (prefs_hbuttonbox), 5, 0);

  save_button = gtk_button_new_with_label ("Save");
  gtk_signal_connect_object (GTK_OBJECT (save_button), "clicked",
                             GTK_SIGNAL_FUNC (save_prefs),
                             NULL);
  gtk_widget_show (save_button);
  gtk_container_add (GTK_CONTAINER (prefs_hbuttonbox), save_button);
  gtk_container_border_width (GTK_CONTAINER (save_button), 3);

  close_button = gtk_button_new_with_label ("Close");
  gtk_signal_connect (GTK_OBJECT (close_button), "clicked",
                             GTK_SIGNAL_FUNC (close_window), prefs_window);
  gtk_widget_show (close_button);
  gtk_container_add (GTK_CONTAINER (prefs_hbuttonbox), close_button);
  gtk_container_border_width (GTK_CONTAINER (close_button), 3);

  gtk_widget_show (prefs_window);
}

