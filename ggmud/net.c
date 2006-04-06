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
#include <sys/time.h>
#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#include <sys/ioctl.h>
#endif

extern GtkWidget *btn_toolbar_disconnect;
extern GtkWidget *btn_toolbar_connect;

#include <stdlib.h>
#include <stdio.h>
#include "ggmud.h"
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
#define errno WSAGetLastError()
#define sockclose(x) closesocket(x)
#define ioctl ioctlsocket
#define EINPROGRESS WSAEWOULDBLOCK
#define EISCONN WSAEISCONN
#endif


struct session *new_session(char *, char *, struct session *);
struct session *newactive_session();
struct session *parse_input(char *, struct session *);

int hide_input = FALSE;

extern int prompt_on;

/*
 * Global Variables
 */
int connected;
static int connecting = 0;

static void printline(const char *str, int isaprompt)
{    
    if (!isaprompt) {
        char buffer[2048];
        strcpy(buffer, str);
        strcat(buffer, "\n");
        textfield_add(mud->text, buffer, MESSAGE_ANSI);
    }
    else
        textfield_add(mud->text, str, MESSAGE_ANSI);
}

void tintin_puts2(const char *cptr, struct session *ses)
{
  extern int puts_echoing;

  if((ses != mud->activesession && ses) || !puts_echoing)
    return;

  textfield_freeze();
  textfield_add(mud->text, cptr, MESSAGE_NORMAL);
  textfield_add(mud->text, "\n", MESSAGE_NORMAL);
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

void make_connection (const char *name, const char *host, const char *port)
{
    char buf[2048];

    if ( !(strcmp (host, "\0")) )
    {
        sprintf (buf, "\n*** Can't connect - you didn't specify a host\n");
        textfield_add (mud->text, buf, MESSAGE_ERR);
        return;
    }

    if ( !(strcmp(port, "\0")) )
    {
        sprintf (buf, "\n*** No port specified - assuming port 23\n");
        textfield_add (mud->text,  buf, MESSAGE_NORMAL);
        port = "23\0";
    }

    sprintf (buf, "\n*** Connecting to %s, port %s\n", host, port);
    textfield_add (mud->text,  buf, MESSAGE_NORMAL);

    open_connection (name, host, port);
}

void disconnect ( void )
{
    extern struct session *activesession;

    if (mud->input_monitor >= 0) {
        gdk_input_remove (mud->input_monitor);
        mud->input_monitor = -1;
    }
   
    while(gtk_events_pending())
        gtk_main_iteration(); // to ensure the input is removed

    textfield_add (mud->text,  "\n*** Connection closed.\n", MESSAGE_NORMAL);
    connected = FALSE;
    gtk_widget_set_sensitive (menu_File_Connect, TRUE);
    gtk_widget_set_sensitive (btn_toolbar_connect, TRUE);
    gtk_widget_set_sensitive (menu_File_DisConnect, FALSE);
    gtk_widget_set_sensitive (btn_toolbar_disconnect, FALSE);
    gtk_window_set_title (GTK_WINDOW (mud->window), "GGMud "VERSION"");
    cleanup_session(mud->activesession);
    
    newactive_session();
    mud->activesession = activesession;
}

struct tempdata
{
    char name[80];
    char hostport[200];
    int sock;
    GtkWidget *window;
};

#ifdef WIN32
void
winsock_init()
{
    WSADATA datas;

    if(WSAStartup(2,&datas)){
        MessageBox(NULL, "Unable to init Winsock 2+", NULL, MB_OK);
        exit(0);
    }
}
#endif

void
connection_part_two(int sockfd, struct tempdata *mystr)
{
    int onoff = 0;
    
    ioctl(sockfd, FIONBIO, (char *)&onoff);

    textfield_add (mud->text, "\n*** Connection established.\n", MESSAGE_NORMAL);

    {
        if ((mud->activesession = new_session(mystr->name, mystr->hostport, mud->activesession)))
            mud->activesession->socket = sockfd;
    }
    
    mud->input_monitor = gdk_input_add (sockfd, GDK_INPUT_READ,
    				   read_from_connection,
    				   mud->activesession );
    connected = TRUE;
    gtk_widget_set_sensitive (menu_File_Connect, FALSE);
    gtk_widget_set_sensitive (btn_toolbar_connect, FALSE);
    gtk_widget_set_sensitive (menu_File_DisConnect, TRUE);
    gtk_widget_set_sensitive (btn_toolbar_disconnect, TRUE);
}

void stop_connecting(GtkWidget *widget, struct tempdata *data)
{
    connecting = 0;
    gdk_input_remove(mud->input_monitor);
    sockclose(data->sock);
    textfield_add(mud->text, "\n*** connection ABORTED.\n",
            MESSAGE_NORMAL);
    
    gtk_widget_destroy(data->window);
    free(data);
}

void connection_cbk (gpointer data, gint source, GdkInputCondition condition)
{
    gdk_input_remove (mud->input_monitor);
    connecting = 0;
    gtk_widget_destroy(((struct tempdata *)data)->window);
    connection_part_two(source, (struct tempdata *)data);
    free(data);
}

void open_connection (const char *name, const char *host, const char *port)
{
    struct hostent *he;
    struct sockaddr_in their_addr;
    int sockfd, onoff = 1;

    if(connecting) {
        textfield_add(mud->text,
                "\n...Connection already in progress...\n",
                MESSAGE_NORMAL);
        return;
    }
    
    if(connected) {
#ifdef USE_NOTEBOOK
        new_view(name);
#else
        popup_window(INFO, "You have to close the previous connection!");
        return;
#endif
    }
    
    /* strerror(3) */
    if ( ( he = gethostbyname (host) ) == NULL )
    {
        popup_window(ERR, "Host not found or host name not valid");
        return;
    }

    if ( ( sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1 ) {
        textfield_add (mud->text, strerror(errno), MESSAGE_ERR);
        return;
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port   = htons( (short)atoi (port));
    their_addr.sin_addr   = *((struct in_addr *)he->h_addr);
    bzero (&(their_addr.sin_zero), 8);

    ioctl(sockfd, FIONBIO, (char *)&onoff);
    
    if (connect (sockfd, (struct sockaddr *)&their_addr,
                 sizeof (struct sockaddr)) == -1 ) {

        if (errno == EINPROGRESS) {
            GtkWidget *label, *box, *hbox, *separator, *button, *image;
            char buffer[200];
            struct tempdata *datas = malloc(sizeof(struct tempdata));

            strcpy(datas->name, name);
            sprintf(datas->hostport, "%s %s", host, port);
            datas->sock = sockfd;

            connecting = 1;

            mud->input_monitor = gdk_input_add (sockfd, GDK_INPUT_WRITE,
                    connection_cbk,
                    datas );

            datas->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
            gtk_window_set_title (GTK_WINDOW (datas->window), "GGMud message");

            box = gtk_vbox_new (FALSE, 3);
            hbox = gtk_hbox_new (FALSE, 2);
            gtk_container_set_border_width (GTK_CONTAINER (box), 5);
            gtk_container_add (GTK_CONTAINER (datas->window), box);

            sprintf(buffer, " Connection to %s:%s in progress... ", host, port);
            label = gtk_label_new (buffer);
            image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_INFO, 
                                             GTK_ICON_SIZE_DIALOG);
            gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 5);
            gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 5);
            gtk_box_pack_start (GTK_BOX (box), hbox, FALSE, FALSE, 5);

            separator = gtk_hseparator_new ();
            gtk_box_pack_start (GTK_BOX (box), separator, TRUE, TRUE, 0);

            button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
            gtk_signal_connect (GTK_OBJECT (button), "clicked",
                    GTK_SIGNAL_FUNC (stop_connecting),
                    datas);
            gtk_box_pack_start (GTK_BOX (box), button, TRUE, TRUE, 5);

            gtk_widget_show_all (datas->window);

            return;
        }
        else if(errno != EISCONN) {
            textfield_add (mud->text, strerror(errno), MESSAGE_ERR);
            sockclose(sockfd);
            return;
        }
    }
    else {
        struct tempdata datas;
        
        strcpy(datas.name, name);
        sprintf(datas.hostport, "%s %s", host, port);
        connection_part_two(sockfd, &datas);
    }
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

    if (!rv)
        return;

    /* sprintf(mybuf, "rv: %d", rv);
       tintin_puts(mybuf, NULL); */ 
#ifdef TELNET_SUPPORT
    if(rv <0) 
#else
    if (!rv)
#endif
    {
#ifdef TELNET_SUPPORT
        if (rv != -666) {
            char *e = strerror(errno);

            if (!e) 
                e = "<UNKNOWN>";

            popup_window(INFO, "Connection aborted\nError: %s (%d)", e, errno);
        }
#endif
        disconnect();
        
        return;
    }
#ifndef TELNET_SUPPORT
    else if(rv < 0) {
        syserr("readmud: read"); // this call ends the program
    }
#endif
    buf[++rv] = '\0';

    /* changed by DasI */
    if( s->snoopstatus && (s != mud->activesession))
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

    if(strlen(buf)>BUFFER_SIZE) {
        popup_window(ERR, "readmud: read one line longer than BUFFERSIZE");
        return;
    }
    
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
        strcpy(linebuf, line);
        do_one_line(linebuf, s);		/* changes linebuf */

        /* added by DasI */
        if( (s == mud->activesession) || s->snoopstatus )
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
    const gchar *entry_text;
    // GtkAdjustment *adj = mud->text->vadjustment;

    entry_text = gtk_entry_get_text (mud->ent);


    strcpy(buffer, entry_text);
    
    mud->activesession = parse_input(buffer, mud->activesession); // can change active session
    hist_add(entry_text);

#if 0 
    if(adj->value < (adj->upper - adj->page_size))
        gtk_adjustment_set_value(adj, (adj->upper - adj->page_size));
#endif

    //textfield_add ( "\n", MESSAGE_NONE);
    if ( prefs.KeepText && !hide_input)
        gtk_entry_select_region (mud->ent, 0,
                mud->ent->text_length);
    else
        gtk_entry_set_text (mud->ent, "");

}

/* used by the auto login function in the connection wizard!
*/
void connection_send (gchar *message)
{
    if(mud->activesession)
        send (mud->activesession->socket, message, strlen (message), 0);
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
    
  if(send(ses->socket, outtext, strlen(outtext), 0) == -1) {
      char *e = strerror(errno);

      if (!e)
          e = "<UNKNOWN>";

      popup_window(INFO, "Connection aborted\nError: %s (%d)", e, errno);
      disconnect();

      return;
  }

  if(prefs.EchoText && !hide_input) {
      if(!broken_telnet) // remove the "\r"
          strcpy(outtext + strlen(outtext) - 2, "\n");

      textfield_add(mud->text, outtext, MESSAGE_SENT);
  }
}

/* send the macro and triggered ext to mud! */
void alt_send_to_connection (gchar *text)
{
    if (strlen(text) && mud->activesession) 
        mud->activesession = parse_input(text, mud->activesession);
}


