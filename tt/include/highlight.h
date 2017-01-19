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
/* ../highlight.c */
#ifndef __HIGHLIGHT_H__
#define __HIGHLIGHT_H__
extern int timeofday;
extern void parse_high(const char *arg, struct session *ses);
extern int is_high_arg(const char *s);
extern void unhighlight_command(const char *arg, struct session *ses);
extern void do_one_high(char *line, struct session *ses);
extern void add_codes(const char *line, char *result, const char *htype, int flag);
#endif

