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

#ifndef _HELP_H_
#define _HELP_H_
#include "ggmud.h"

/* Funktion prototypes */
void do_about (GtkWidget *widget, gpointer data);
void do_manual (GtkWidget *widget, gpointer data);


/* About txt */
const char * ABOUT_MESSAGE = "\n\n Author:\n"
"    Gabriele Greco (gabriele.greco@aruba.it)\n"
"\n"
" SClient authors:\n"
"    P.E. Segolsson, Fredrik Andersson\n"
"\n"
" TinTin++ authors:\n"
"    Bill Reiss, David A. Wagner, Rob Ellsworth,\n"
"    Jeremy C. Jack, Davin Chan\n"
"\n"  
" WWW:\n" 
"    http://ggmud.sourceforge.net\n" 
"\n";

#endif /* _HELP_H_ */
