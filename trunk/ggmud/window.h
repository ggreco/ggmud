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

#ifndef _WINDOW_H_
#define _WINDOW_H_
#include "ggmud.h"

/* Function prototypes */
void destructify();
void close_window (GtkWidget *widget, gpointer data);
void quit (GtkWidget *widget, gpointer data);
void do_con();
void cbox (void);
GtkWidget *spawn_gui (void);
void macro_btnLabel_change ();
void clear (int,GtkText *);
void clear_backbuffer();
void popup_window (const gchar *message);

extern void log_viewer();
extern void alt_send_to_connection  (gchar *text);

typedef enum { F1, F2, F3, F4, F5, F6, F7, F8, F9, F10 } BUTTON;
#define LorC(L)		(strlen(L) < 10 ? 0.5 : 0)

/* External variables used */
extern gchar *keys[];

#endif /* _WINDOW_H_ */
