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

#ifndef __ANSI_H__
#define __ANSI_H__
#include "ggmud.h"

/* Funktion prototypes */
gushort  convert_color (unsigned color);
void     extract_color (GdkColor *color, unsigned red, unsigned green, unsigned blue);
void     init_colors ();

/* Glbal Variables */
GtkWidget *text_field;
GtkWidget *text_entry;
GtkWidget *entry_host;
GtkWidget *entry_port;

/* Colors */
GdkColormap *cmap;		/* BOLD = bright color. LOW = darkish color */
GdkColor color_lightwhite;	/* BOLD white  */
GdkColor color_white;		/* LOW white */
GdkColor color_lightblue;	/* BOLD blue */
GdkColor color_blue;		/* LOW blue */
GdkColor color_lightgreen;	/* BOLD green */
GdkColor color_green;		/* LOW green */
GdkColor color_lightred;	/* BOLD red */
GdkColor color_red;		/* LOW red */
GdkColor color_lightyellow;	/* BOLD yellow */
GdkColor color_yellow;		/* LOW yellow ( brown )*/
GdkColor color_lightmagenta;	/* BOLD magenta ( pink ) */
GdkColor color_magenta;		/* LOW magenta ( purple ) */
GdkColor color_lightcyan;	/* BOLD cyan */
GdkColor color_cyan;		/* LOW cyan */
GdkColor color_lightblack;	/* BOLD grey (highlighted black) */
GdkColor color_black;		/* LOW black */

GdkColor default_color_lightwhite;	/* BOLD white  */
GdkColor default_color_white;		/* LOW white */
GdkColor default_color_lightblue;	/* BOLD blue */
GdkColor default_color_blue;		/* LOW blue */
GdkColor default_color_lightgreen;	/* BOLD green */
GdkColor default_color_green;		/* LOW green */
GdkColor default_color_lightred;	/* BOLD red */
GdkColor default_color_red;		/* LOW red */
GdkColor default_color_lightyellow;	/* BOLD yellow */
GdkColor default_color_yellow;		/* LOW yellow ( brown )*/
GdkColor default_color_lightmagenta;	/* BOLD magenta ( pink ) */
GdkColor default_color_magenta;		/* LOW magenta ( purple ) */
GdkColor default_color_lightcyan;	/* BOLD cyan */
GdkColor default_color_cyan;		/* LOW cyan */
GdkColor default_color_lightblack;	/* BOLD grey (highlighted black) */
GdkColor default_color_black;		/* LOW black */

GdkColor fg_col;		/* Foreground color */
GdkColor bg_col;		/* background color */

#endif /* __ANSI_H__ */
