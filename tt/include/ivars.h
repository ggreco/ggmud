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
/* ../ivars.c */
#ifndef __IVARS_H__
#define __IVARS_H__
extern int timeofday;
extern int stacks[100][4];
extern void math_command(const char *line, struct session *ses);
extern void if_command(const char *line, struct session *ses);
extern void ifstrequal_command(const char *line, struct session *ses);
extern void ifexist_command(const char *line, struct session *ses);
extern void revstring_command(const char *line, struct session *ses);
extern int eval_expression(const char *arg, int *exitstatus);
extern int conv_to_ints(const char *arg);
extern int do_one_inside(int begin, int end);
extern void ifmatch_command(const char *arg, struct session *ses);
#endif

