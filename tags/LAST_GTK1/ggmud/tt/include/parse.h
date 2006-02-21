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
/* ../parse.c */
#ifndef __PARSE_H__
#define __PARSE_H__
extern int timeofday;
extern struct session *parse_input(char *input, struct session *ses);
extern int is_speedwalk_dirs(const char *cp);
extern void do_speedwalk(const char *cp, struct session *ses);
extern struct session *parse_tintin_command(const char *command, char *arg, struct session *ses);
extern const char *get_arg_all(const char *s, char *arg);
extern const char *get_arg_with_spaces(const char *s, char *arg);
extern const char *get_arg_in_braces(const char *s, char *arg, int flag);
extern const char *get_arg_stop_spaces(const char *s, char *arg);
extern const char *space_out(const char *s);
extern void write_com_arg_mud(const char *command, const char *argument, struct session *ses);
extern void prompt(struct session *ses);
extern void do_one_line(char *line, struct session *ses);
#endif

