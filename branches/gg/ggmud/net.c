/*  Sclient
 *  Copyright (C) 1999 Heathen (the.heathen@swipnet.se)
 *		  1999 Drizzt  (doc.day@swipnet.se)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* ripped straight from amcl and altered a bit ;) */


#include "config.h"

#include <sys/types.h>
#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "tintin.h"
/*
 * Added by Michael Stevens
 */
#ifndef INHIBIT_STRING_HEADER
# if defined (HAVE_STRING_H) || defined (STDC_HEADERS) || defined (_LIBC)
#  include <string.h>
#  ifndef bcmp
#   define bcmp(s1, s2, n) memcmp ((s1), (s2), (n))
#  endif
#  ifndef bcopy
#   define bcopy(s, d, n)  memcpy ((d), (s), (n))
#  endif
#  ifndef bzero
#   define bzero(s, n)     memset ((s), 0, (n))
#  endif
# else
#  include <strings.h>
# endif
#endif

#ifndef WIN32
#define sockclose(x) close(x)
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>

#ifdef HAVE_TELNET_H
#include <telnet.h>
#endif
#ifdef HAVE_ARPA_TELNET_H
#include <arpa/telnet.h>
#endif
#else
#include <winsock2.h>

#define sockclose(x) closesocket(x)
#endif

#include "net.h"
extern struct session *activesession;

struct session *new_session(char *, char *, struct session *);
struct session *newactive_session();
struct session *parse_input(char *, struct session *);

extern int prompt_on;

gchar* strdelim(gchar *str, const gchar *delim, gchar new_delim)

/* The same as g_strdelimit(), but doesn't modify the original string */

{
    register gchar	*c;
    gchar		*newstr;

    g_return_val_if_fail (str != NULL, NULL);
    if (!delim) delim = G_STR_DELIMITERS;
    newstr = g_new(char, strlen(str) + 1);
    strcpy(newstr, str);
    for (c = newstr; *c; c++) {
    	if (strchr (delim, *c)) *c = new_delim;
    }
    return newstr;
}

static void printline(const char *str, int isaprompt)
{
    textfield_add(str, MESSAGE_ANSI);
    
    if (!isaprompt) 
        textfield_add("\n", MESSAGE_NORMAL);
    
}

void tintin_puts2(const char *cptr, struct session *ses)
{
  extern int puts_echoing;

  if((ses != activesession && ses) || !puts_echoing)
    return;

  textfield_freeze();
  textfield_add(cptr, MESSAGE_NORMAL);
  textfield_add("\n", MESSAGE_NORMAL);
  textfield_unfreeze();
}

void tintin_puts(const char *cptr, struct session *ses)
{
 /* bug! doesn't do_one_line() sometimes send output to stdout? */
  if(ses) {
    char buf[BUFFER_SIZE];

    sprintf(buf, "%s", cptr);
    do_one_line(buf, ses);
    if(strcmp(buf, "."))
      tintin_puts2(buf, ses);
  }
  else
    tintin_puts2(cptr, ses);
}

void make_connection (char *name, char *host, char *port)
{
    char buf[2048];

    if ( !(strcmp (host, "\0")) )
    {
        sprintf (buf, "\n*** Can't connect - you didn't specify a host\n");
        textfield_add ( buf, MESSAGE_ERR);
        return;
    }

    if ( !(strcmp(port, "\0")) )
    {
        sprintf (buf, "\n*** No port specified - assuming port 23\n");
        textfield_add ( buf, MESSAGE_NORMAL);
        port = "23\0";
    }

    sprintf (buf, "\n*** Connecting to %s, port %s\n", host, port);
    textfield_add ( buf, MESSAGE_NORMAL);

    open_connection (name, host, port);
}

void disconnect ( void )
{
    cleanup_session(activesession);
    gdk_input_remove (input_monitor);
    textfield_add ( "\n*** Connection closed.\n", MESSAGE_NORMAL);
    connected = FALSE;
    gtk_widget_set_sensitive (menu_File_Connect, TRUE);
    gtk_widget_set_sensitive (btn_toolbar_connect, TRUE);
    gtk_widget_set_sensitive (menu_File_DisConnect, FALSE);
    gtk_widget_set_sensitive (btn_toolbar_disconnect, FALSE);
    gtk_window_set_title (GTK_WINDOW (mud->window), "GGMud "VERSION"");
}

void open_connection (const char *name, const char *host, const char *port)
{
    struct hostent *he;
    struct sockaddr_in their_addr;
    int sockfd;
#ifdef WIN32
    static int winsock_initted = 0;

    if(!winsock_initted) {
        WSADATA datas;

		if(WSAStartup(2,&datas)){
            fprintf(stderr, "Non riesco a inizializzare Winsock 2+\n");
			exit(0);
        }
    }
    
#endif

    /* strerror(3) */
    if ( ( he = gethostbyname (host) ) == NULL )
    {
        return;
    }

    if ( ( sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        textfield_add (strerror(errno), MESSAGE_ERR);
        return;
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port   = htons( (short)atoi (port));
    their_addr.sin_addr   = *((struct in_addr *)he->h_addr);
    bzero (&(their_addr.sin_zero), 8);

    if (connect (sockfd, (struct sockaddr *)&their_addr,
                 sizeof (struct sockaddr)) == -1 )
    {
        textfield_add (strerror(errno), MESSAGE_ERR);
        return;
    }

    textfield_add ("\n*** Connection established.\n", MESSAGE_NORMAL);

    {
        char buffer[200];

        sprintf(buffer, "%s %s", host, port);
        
        activesession = new_session(name, buffer, activesession);

        activesession->socket = sockfd;
    }
    
    input_monitor = gdk_input_add (sockfd, GDK_INPUT_READ,
    				   read_from_connection,
    				   activesession );
    connected = TRUE;
    gtk_widget_set_sensitive (menu_File_Connect, FALSE);
    gtk_widget_set_sensitive (btn_toolbar_connect, FALSE);
    gtk_widget_set_sensitive (menu_File_DisConnect, TRUE);
    gtk_widget_set_sensitive (btn_toolbar_disconnect, TRUE);
}

int check_status(const char *buf, struct session *ses)
{
  extern char *prompt_line;

  if (check_one_action(buf, prompt_line, ses)) { 
//     status_in_splitline(buf, ses);
     return(1);
  }
  return(0);
}
/* data waiting on this mud session; read & display it; do the dirty work */
/* seriuos bug was found by DasI:                                         */
/* no reaction to actions in incative sessions                            */
/* fixed by DasI                                                          */
/* btw: i don't tested my fix very much, so it's up to you.               */
static void readmud(struct session *s)
{
    char thebuffer[2*BUFFER_SIZE+1], *buf, *line, *next_line;
    /* char mybuf[512]; */
    char linebuf[BUFFER_SIZE], header[BUFFER_SIZE];
    int rv, headerlen;

    /* If not connected, return.  - ycjhi */
    if(!ZOMBI_IS_ALIVE(s))
        return ;

    buf = thebuffer + BUFFER_SIZE;
    rv = read_buffer_mud(buf, s);
    /* sprintf(mybuf, "rv: %d", rv);
       tintin_puts(mybuf, NULL); */ 
    if(!rv) {
//           cleanup_session(s); da riconsiderare quando avremo piu' sessioni

        disconnect();
        
        newactive_session();
        return;
    }
    else if(rv < 0)
        syserr("readmud: read");

    buf[++rv] = '\0';

    /* changed by DasI */
    if( s->snoopstatus && (s != activesession))
        sprintf(header, "%s%% ", s->name);
    else
        header[0] = '\0';

    headerlen = strlen(header);

    if(s->old_more_coming) {
        line = s->last_line;
        buf -= strlen(line);
        while(*line)
            *buf++ = *line++;
        buf -= strlen(s->last_line);
        s->last_line[0] = '\0';
    }

    if(strlen(buf)>BUFFER_SIZE)
        syserr("readmud: read one line longer than BUFFERSIZE");

    logit(s, buf);

    /* separate into lines and print away */

    textfield_freeze();
    
    for(line = buf; line && *line; line = next_line) 
    {
        if(!(next_line = strchr(line, '\n')) && s->more_coming)
            break;
        if(next_line) {
            *next_line++ = '\0';
            if(*next_line == '\r')			/* ignore \r's */
                next_line++;
        }
        sprintf(linebuf, "%s", line);
        do_one_line(linebuf, s);		/* changes linebuf */

        /* added by DasI */
        if( (s == activesession) || s->snoopstatus )
        {
            if(strcmp(linebuf, ".")) {
                strcat(header, linebuf);
                if (prompt_on == 0) {
                    printline(header, !next_line );
                } else if (check_status(linebuf, s) == 0)
                    printline(header, !next_line );

                header[headerlen] = '\0';
            } 
        }
    }

    textfield_unfreeze();
    
    if(line && *line)
        sprintf(s->last_line, "%s", line);
}

void read_from_connection (gpointer data, gint source, GdkInputCondition condition)
{
    readmud((struct session *)data);
}

void send_to_connection (GtkWidget *widget, gpointer data)
{
    char buffer[2048];
    gchar *entry_text;

    entry_text = gtk_entry_get_text (GTK_ENTRY (mud->ent));
//    gtk_entry_select_region (GTK_ENTRY (mud->ent), 0,  GTK_ENTRY (mud->ent)->text_length); ???


    strcpy(buffer, entry_text);
    
    activesession = parse_input(buffer, activesession); // can change active session
    hist_add(entry_text);

    //textfield_add ( "\n", MESSAGE_NONE);
    if ( prefs.KeepText )
        gtk_entry_select_region (GTK_ENTRY (mud->ent), 0,
                GTK_ENTRY (mud->ent)->text_length);
    else
        gtk_entry_set_text (GTK_ENTRY (mud->ent), "");

}

/* used by the auto login function in the connection wizard!
*/
void connection_send (gchar *message)
{
    if(activesession)
        send (activesession->socket, message, strlen (message), 0);
}

/************************************************************/
/* write line to the mud ses is connected to - add \n first */
/************************************************************/

void write_line_mud(const char *line, struct session *ses)
{
  char outtext[BUFFER_SIZE+2];
  extern int broken_telnet;

  if (!ZOMBI_IS_ALIVE(ses))  /* Fixed. Connection test -- ycjhi */
    return ;

  sprintf(outtext, "%s", line);
  /* lost this fix somehow.  It appears that I lost it during
     1.81 Might have lost it when I intergrated the zombie
     code -- DSC */
  if (broken_telnet)
    strcat(outtext, "\n");
  else
    strcat(outtext, "\r\n");
    
  if(send(ses->socket, outtext, strlen(outtext), 0) == -1)
    syserr("write in write_to_mud");

  if(prefs.EchoText) {
      if(!broken_telnet) // remove the "\r"
          strcpy(outtext + strlen(outtext) - 2, "\n");

      textfield_add(outtext, MESSAGE_SENT);
  }
}

/* send the macro and triggered ext to mud! */
void alt_send_to_connection (gchar *text)
{
    gchar *temp;
    if (strlen(text)) {
        temp = g_malloc0(strlen(text) + 2);
        strcpy(temp, strdelim(text,";",'\n'));
        strcat(temp,"\n");
        if (connected && activesession) 
            send (activesession->socket, temp, strlen(temp), 0);
        if (prefs.EchoText) textfield_add (temp, MESSAGE_SENT);
    }
}

