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
/* ../action.c */
#ifndef __ACTION_H__
#define __ACTION_H__
extern int timeofday;
extern void action_command(const char *arg, struct session *ses);
extern void unaction_command(const char *arg, struct session *ses);
extern void prepare_actionalias(const char *string, char *result, struct session *ses);
extern void substitute_vars(const char *arg, char *result);
extern void check_all_actions(const char *line, struct session *ses);
extern int match_a_string(const char *line, const char *mask);
extern int check_one_action(const char *line, const char *action, struct session *ses);
extern int check_a_action(const char *line, const char *action, struct session *ses);
#endif

