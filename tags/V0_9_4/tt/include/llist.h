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
/* ../llist.c */
#ifndef __LLIST_H__
#define __LLIST_H__
extern int timeofday;
extern struct listnode *init_list(void);
extern void kill_list(struct listnode *nptr);
extern void kill_all(struct session *ses, int mode);
extern struct listnode *copy_list(struct listnode *sourcelist, int mode);
extern struct listnode *insertnode_list(struct listnode *listhead, const char *ltext, const char *rtext, const char *prtext, int mode);
extern void deletenode_list(struct listnode *listhead, struct listnode *nptr);
extern struct listnode *searchnode_list(struct listnode *listhead, const char *cptr);
extern struct listnode *searchnode_list_begin(struct listnode *listhead, const char *cptr, int mode);
extern void shownode_list(struct listnode *nptr);
extern void shownode_list_action(struct listnode *nptr);
extern void show_list(struct listnode *listhead);
extern void show_list_action(struct listnode *listhead);
extern struct listnode *search_node_with_wild(struct listnode *listhead, const char *cptr);
extern int check_one_node(const char *text, const char *action);
extern void addnode_list(struct listnode *listhead, const char *ltext, const char *rtext, const char *prtext);
extern int count_list(struct listnode *listhead);
#endif

