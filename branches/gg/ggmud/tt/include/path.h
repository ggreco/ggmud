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
/* ../path.c */
#ifndef __PATH_H__
#define __PATH_H__
extern int timeofday;
extern void mark_command(struct session *ses);
extern void map_command(const char *arg, struct session *ses);
extern void savepath_command(const char *arg, struct session *ses);
extern void path_command(struct session *ses);
extern void return_command(struct session *ses);
extern void unpath_command(struct session *ses);
extern void pathdir_command(const char *arg, struct session *ses);
extern void check_insert_path(const char *command, struct session *ses);
#endif

