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

#ifndef _MACRO_H_
#define _MACRO_H_
#include "ggmud.h"

/* Funktion prototypes */
void  button_ok_callback(GtkWidget *button, GtkWidget *entry[]);
void  load_macro    ( void );
void  save_macro    ( gpointer data );
void  window_macro    (GtkWidget *widget, gpointer data);

/* External functions used */
extern void macro_btnLabel_change ();

/* External variables used */
extern gchar *keys[];

#endif /* _MACRO_H_ */
