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
/* ../session.c */
#ifndef __SESSION_H__
#define __SESSION_H__
extern int timeofday;
extern struct session *session_command(const char *arg, struct session *ses);
extern void show_session(struct session *ses);
extern struct session *newactive_session(void);
extern struct session *new_session(const char *name, const char *address, struct session *ses);
extern void cleanup_session(struct session *ses);
extern void cleanup_nonzombi_session(struct session * ses) ;
extern void cleanup_zombi_session(struct session * ses) ;
extern int revive_zombi(struct session * ses) ;
extern void zombion_command(struct session * ses) ;
extern void zombioff_command(struct session * ses) ;
#endif

