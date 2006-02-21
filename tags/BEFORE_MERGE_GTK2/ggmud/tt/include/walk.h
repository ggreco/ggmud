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
/* ../walk.c */
#ifndef __WALK_H__
#define __WALK_H__
extern void walkset_command(const char *arg, struct session * ses) ;
extern void walk_command(struct session * ses) ;
extern void walkreset_command(const char *arg, struct session * ses) ;
extern void cleanup_walk(struct session * ses) ;
extern void walkback_command(struct session * ses) ;
extern void walkinfo_command(struct session * ses) ;
extern void walkon_command(struct session * ses) ;
extern void walkoff_command(struct session * ses) ;
extern void getwalkposition_command(const char *arg, struct session * ses) ;
extern void getwalkdirection_command(const char *arg, struct session * ses) ;
extern int get_back_dir(struct session * ses, char *dst, char *src) ;
#endif

