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
/* ../net.c */
#ifndef __NET_H__
#define __NET_H__
extern int timeofday;
extern int connect_mud(const char *host, const char *port, struct session *ses);
extern void write_line_mud(const char *line, struct session *ses);
extern int read_buffer_mud(char *buffer, struct session *ses);
void translate_telnet_protocol(unsigned char *dst, unsigned char *src, 
				size_t srclen);
#define TELNET_EOF   0xEC /* EOF */
#define TELNET_SUSP  0xED /* SUSP */
#define TELNET_ABORT 0xEE /* ABORT */
#define TELNET_EOR   0xEF /* EOR */
#define TELNET_SE    0xF0 /* SE */
#define TELNET_NOP   0xF1 /* NOP */
#define TELNET_DM    0xF2 /* Data Mark */
#define TELNET_BREAK 0xF3 /* Break */
#define TELNET_IP    0xF4 /* Interrupt Process */
#define TELNET_AO    0xF5 /* Abort Output */
#define TELNET_AYT   0xF6 /* Are You There */
#define TELNET_EC    0xF7 /* Erase Character */
#define TELNET_EL    0xF8 /* Erase Line */
#define TELNET_GA    0xF9 /* Go Ahead */
#define TELNET_SB    0xFA /* SB */
#define TELNET_WILL  0xFB /* WILL (option code) */
#define TELNET_WONT  0xFC /* WON'T (option code) */
#define TELNET_DO    0xFD /* DO (option code) */
#define TELNET_DONT  0xFE /* DON'T (option code) */
#define TELNET_IAC   0xFF /* Interprete As Command */
#endif

