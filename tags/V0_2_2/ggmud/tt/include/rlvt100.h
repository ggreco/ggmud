/*
TinTin++
Copyright (C) 2001 Davin Chan, Robert Ellsworth, etc. (See CREDITS file)

This program is protected under the GNU GPL (See COPYING)

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
/* ../rlvt100.c */
#ifndef __RLVT100_H__
#define __RLVT100_H__
extern void save_pos(void);
extern void restore_pos(void);
extern void goto_rowcol(int row, int col);
extern void erase_screen(void);
extern void erase_toeol(void);
extern void reset(void);
extern void scroll_region(int top, int bottom);
#endif

