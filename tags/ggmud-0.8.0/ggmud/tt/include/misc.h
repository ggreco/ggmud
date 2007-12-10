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
/* ../misc.c */
#ifndef __MISC_H__
#define __MISC_H__
extern int timeofday;
extern void cr_command(struct session *ses);
extern void version_command(void);
extern void verbatim_command(void);
extern struct session *all_command(const char *arg, struct session *ses);
extern void redraw_command(void);
extern void bell_command(struct session *ses);
extern void boss_command(struct session *ses);
extern void char_command(const char *arg, struct session *ses);
extern void url_command(const char *arg, struct session *ses);
extern void echo_command(struct session *ses);
extern void end_command(const char *command, struct session *ses);
extern void ignore_command(struct session *ses);
extern void presub_command(struct session *ses);
extern void togglesubs_command(struct session *ses);
extern void showme_command(const char *arg, struct session *ses);
extern void loop_command(const char *arg, struct session *ses);
extern void forall_command(const char *arg, struct session *ses);
extern void message_command(const char *arg, struct session *ses);
extern void snoop_command(const char *arg, struct session *ses);
extern void speedwalk_command(struct session *ses);
extern void system_command(const char *arg, struct session *ses);
extern struct session *zap_command(struct session *ses);
extern void wizlist_command(struct session *ses);
extern void display_info(struct session *ses);
extern void random_command(const char *arg, struct session *ses);
extern void setprompt_command(const char *arg, struct session *ses);
extern void number_of_tries(const char *arg, struct session *ses);
extern void time_between_tries(const char *arg, struct session *ses);
extern void sleep_command(const char *arg, struct session *ses);
extern void fixed_math_command(struct session *ses);
extern void broken_telnet_command(struct session *ses);
extern void save_history_command(struct session *ses);
extern void show_pretick_command(struct session *ses);
extern void append_log_command(struct session *ses);

// window handling commands in window.c
extern void clr_command(char *, struct session *);
extern void mess_command(char *, struct session *);
extern void window_command(char *, struct session *);
extern void hbutton_command(char *, struct session *);
extern void vbutton_command(char *, struct session *);

#endif

