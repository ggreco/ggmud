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

extern GdkColor color_lightwhite;	/* BOLD white  */
extern GdkColor color_white;		/* LOW white */
extern GdkColor color_lightblue;	/* BOLD blue */
extern GdkColor color_blue;		/* LOW blue */
extern GdkColor color_lightgreen;	/* BOLD green */
extern GdkColor color_green;		/* LOW green */
extern GdkColor color_lightred;	/* BOLD red */
extern GdkColor color_red;		/* LOW red */
extern GdkColor color_lightyellow;	/* BOLD yellow */
extern GdkColor color_yellow;		/* LOW yellow ( brown )*/
extern GdkColor color_lightmagenta;	/* BOLD magenta ( pink ) */
extern GdkColor color_magenta;		/* LOW magenta ( purple ) */
extern GdkColor color_lightcyan;	/* BOLD cyan */
extern GdkColor color_cyan;		/* LOW cyan */
extern GdkColor color_lightblack;	/* BOLD grey (highlighted black) */
extern GdkColor color_black;		/* LOW black */

extern GdkColor default_color_lightwhite;	/* BOLD white  */
extern GdkColor default_color_white;		/* LOW white */
extern GdkColor default_color_lightblue;	/* BOLD blue */
extern GdkColor default_color_blue;		/* LOW blue */
extern GdkColor default_color_lightgreen;	/* BOLD green */
extern GdkColor default_color_green;		/* LOW green */
extern GdkColor default_color_lightred;	/* BOLD red */
extern GdkColor default_color_red;		/* LOW red */
extern GdkColor default_color_lightyellow;	/* BOLD yellow */
extern GdkColor default_color_yellow;		/* LOW yellow ( brown )*/
extern GdkColor default_color_lightmagenta;	/* BOLD magenta ( pink ) */
extern GdkColor default_color_magenta;		/* LOW magenta ( purple ) */
extern GdkColor default_color_lightcyan;	/* BOLD cyan */
extern GdkColor default_color_cyan;		/* LOW cyan */
extern GdkColor default_color_lightblack;	/* BOLD grey (highlighted black) */
extern GdkColor default_color_black;		/* LOW black */
extern GdkColormap *cmap;

#endif /* __ANSI_H__ */
