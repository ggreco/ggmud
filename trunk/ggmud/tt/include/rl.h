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
/* ../rl.c */
#ifndef __RL_H__
#define __RL_H__
extern int timeofday;
extern int ignore_interrupt;
extern int am_purist;
extern int requested_scrsize;
extern int save_history;
extern void initrl(void);
extern void initsplit(void);
extern void mainloop(void);
extern void tintin_puts2(const char *cptr, struct session *ses);
extern void tintin_puts(const char *cptr, struct session *ses);
extern void cleanscreen(void);
extern void dirtyscreen(void);
extern void quitmsg(const char *m);
extern void myquitsig(int no_care);
extern int tickcounter_in_splitline(int ttt);
extern int status_in_splitline(const char *arg, struct session *ses);
extern void split_command(const char *arg);
extern void unsplit_command(void);
extern void purist_command(void);
extern void unpurist_command(void);
extern int check_status(const char *buf, struct session *ses);
#endif

