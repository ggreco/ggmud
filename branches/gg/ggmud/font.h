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

#ifndef _FONT_H_
#define _FONT_H_
#include "ggmud.h"

/* Function prototypes */
void load_font      ( void );
void save_font ();
void font_font_selected (GtkWidget *button, GtkFontSelectionDialog *fs);
void window_font    ( GtkWidget *widget, gpointer data   );

/* Global variables */
SYSTEM_DATA font;
GtkWidget   *font_window;
GtkWidget   *font_button_save;
GtkWidget   *entry_fontname;
GtkWidget *menu_Option_font;
GdkFont  *font_normal;
GtkStyle *style;

#endif /* _FONT_H_ */
