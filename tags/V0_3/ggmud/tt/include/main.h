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
/* ../main.c */

#ifndef __MAIN_H__
#define __MAIN_H__
#include "../tintin.h"
extern int timeofday;
extern int term_echoing;
extern int echo;
extern int fixed_math;
extern int broken_telnet;
extern int append_log;
extern int show_pretick;
extern int speedwalk;
extern int togglesubs;
extern int presub;
extern int redraw;
extern int sessionsstarted;
extern int puts_echoing;
extern int verbose;
extern int alnum;
extern int acnum;
extern int subnum;
extern int varnum;
extern int hinum;
extern int pdnum;
extern int antisubnum;
extern int verbatim;
extern int prompt_on;
extern int funcnum;
extern int retries;
extern int time_before_try;
extern char homepath[1025];
extern char prompt_line[BUFFER_SIZE ];
extern struct session *sessionlist;
extern struct session *activesession;
extern struct listnode *common_aliases;
extern struct listnode *common_actions;
extern struct listnode *common_subs;
extern struct listnode *common_myvars;
extern struct listnode *common_highs;
extern struct listnode *common_antisubs;
extern struct listnode *common_pathdirs;
extern struct listnode *common_functions;
extern char vars[10][BUFFER_SIZE ];
extern char tintin_char;
extern char verbatim_char;
extern char system_com[80];
extern int mesvar[7];
extern int display_row;
extern int display_col;
extern int input_row;
extern int input_col;
extern int split_line;
extern int term_columns;
extern char k_input[BUFFER_SIZE ];
extern char done_input[BUFFER_SIZE ];
extern char prev_command[BUFFER_SIZE ];
extern int hist_num;
extern int is_split;
extern int text_came;
extern void winchhandler(int no_care);
extern void tstphandler(int no_care);
extern int main(int argc, char **argv);
#endif

