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

#ifndef _NET_H_
#define _NET_H_
#include "ggmud.h"

/* Funktion prototypes */
void make_connection (char *name, char *host, char *port );
void disconnect ( void );
void open_connection (const char *name,  const char *host, const char *port );
void send_to_connection ( GtkWidget *, gpointer data );
void read_from_connection (gpointer, gint , GdkInputCondition );
void connection_send ( gchar *message );
/* void macro_send_to_connection  (gchar *text); */
void alt_send_to_connection (gchar *text);
extern void search_triggers (gchar *incomming);

/*
 * Global Variables
 */
int connected;

extern GtkWidget *btn_toolbar_disconnect;
extern GtkWidget *btn_toolbar_connect;

#endif /* _NET_H_ */
