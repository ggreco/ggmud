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

#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_
#include "ggmud.h"

/* Used for the color selection UI
 */
typedef struct {
    GdkColor *color;
    gchar *name;
} color_struct;

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

/* Funktion prototypes */
void load_misc_prefs ();
void load_prefs ();
void save_prefs (GtkWidget *button, gpointer data);
void check_text_toggle (GtkWidget *widget, GtkWidget *button);
void check_callback (GtkWidget *widget, GtkWidget *check_button);
void check_wrap (GtkWidget *widget, GtkWidget *wrap_button);
void check_beep (GtkWidget *widget, GtkWidget *check_button);
void check_Toolbar (GtkWidget *widget, GtkWidget *check_button_toolbar);
void check_Macrobuttons (GtkWidget *widget, GtkWidget *check_button_macros);
void check_Statusbar (GtkWidget *widget, GtkWidget *check_button_statusbar);
void window_prefs (GtkWidget *widget, gpointer data);
gdouble *gdk_color_to_gdouble (GdkColor *gdkcolor);
void text_toggle_word_wrap (GtkWidget *checkbutton_wrap, GtkWidget *text);
void color_ok (GtkWidget *widget, GtkWidget *color_sel);
void color_cancel (GtkWidget *widget, GtkWidget *color_sel);
void color_callback (GtkWidget *widget, color_struct *color);
void color_reset_to_default (GtkWidget *button, gpointer data);
void color_prefs_done (GtkWidget *widget, GtkWidget *dialog);
void color_prefs (GtkWidget *widget, GtkWidget *dummy);

/* Global variables */
PREFS_DATA prefs;
GtkWidget   *prefs_window;
GtkWidget   *prefs_button_save;
GtkWidget *checkbutton_Toolbar;
GtkWidget *checkbutton_Macrobuttons;
GtkWidget *checkbutton_Statusbar;
extern   GtkWidget *handlebox;
extern  GtkWidget *vbox3;
extern  GtkWidget *statusbar;


#endif /* _PREFERENCES_H_ */
