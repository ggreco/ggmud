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

/* Funktion prototypes */
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

extern GtkWidget *handlebox;
extern GtkWidget *statusbar;

/* Global variables */
extern GtkWidget *win;
extern GtkWidget *btnLabel[12];
extern GtkWidget *menu_Tools_Logger;
extern GtkWidget *handlebox;


/* Global ToolBar stuff */
extern GtkWidget *btn_toolbar_logger;
extern GtkWidget *btn_toolbar_disconnect;
extern GtkWidget *btn_toolbar_connect;

#endif /* _PREFERENCES_H_ */
