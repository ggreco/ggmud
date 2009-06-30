#include <sys/types.h>
#ifndef WIN32
#define sockclose(x) close(x)
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#else
#define sockclose(x) closesocket(x)
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif
#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "ggmud.h"

#define LINE_MARKER "*-*"

void chdisconnect_command(struct session *ses)
{
    if (mud->chat_input) {
        gdk_input_remove (mud->chat_input);
        sockclose(mud->chat_socket);
        mud->chat_input = 0;
        free(mud->chat_buffer);
        textfield_add(mud->text, "Disconnected from chat server!\n", MESSAGE_SENT);
    }
    else 
        textfield_add(mud->text, "You are not connected to a chat server!\n", MESSAGE_SENT);
}

static void
read_from_chat(gpointer data, gint source, GdkInputCondition condition)
{
    int size = recv(mud->chat_socket, mud->chat_buffer + mud->chat_offset, mud->chat_size - mud->chat_offset - 1, 0L);
    char *line;

    if ((size < 0 && errno != EWOULDBLOCK) ||
         size == 0) {
        textfield_add(mud->text, "\n*** chat server closes connection...\n", MESSAGE_SENT);
        chdisconnect_command(NULL);
        return;
    }

    mud->chat_offset += size;

    mud->chat_buffer[mud->chat_offset] = 0;

    if ((line = strstr(mud->chat_buffer, LINE_MARKER))) {
        char buffer[BUFFER_SIZE], *result;
        *line = 0;
        line += strlen(LINE_MARKER);
        strcpy(buffer, mud->chat_buffer);
        if (mud->activesession)
            do_one_line(buffer, mud->activesession);

        result = ParseAnsiColors(buffer);
        strcat(result, "\n");
        textfield_add(mud->text, result , MESSAGE_SCROLLING_ANSI);
        mud->chat_offset = strlen(line);
        memmove(mud->chat_buffer, line, mud->chat_offset);
    }
}

void chat_command(const char *arg, struct session *ses)
{
    char buffer[BUFFER_SIZE];
    if (mud->chat_input == 0) {
        textfield_add(mud->text, "You are not connected to a chat server!\n", MESSAGE_SENT);
        return;
    }
    sprintf(buffer, "[chat] '%s'\n", arg);
    textfield_add(mud->text, buffer, MESSAGE_SENT);
    sprintf(buffer, "W%s" LINE_MARKER, arg);
    send(mud->chat_socket, buffer, strlen(buffer), 0L);
}

extern char *get_arg_stop_spaces(char *, char *);

void chatwho_command(char *arg, struct session *ses)
{
    char buffer[BUFFER_SIZE];

    if (!mud->chat_input) {
        textfield_add(mud->text, "You are not connected to a chat server!\n", MESSAGE_SENT);
        return;
    }
    textfield_add(mud->text, "Asking WHO command to chat server...\n", MESSAGE_SENT);
    sprintf(buffer, "Z" LINE_MARKER); 
    send(mud->chat_socket, buffer, strlen(buffer), 0L);
}

void chconnect_command(char *arg, struct session *ses)
{
    struct hostent *he;
    struct sockaddr_in their_addr;
    int sockfd;
    char buffer[BUFFER_SIZE];
    char host[128];
    unsigned short port;
    char user[20];
    char pwd[20];

    // get host
    arg = get_arg_stop_spaces(arg, buffer);
    if (strlen(buffer) < 3) {
        textfield_add(mud->text, "Usage: #chconnect hostname port username password\n\n", MESSAGE_SENT);
        return;
    }

    strncpy(host, buffer, sizeof(host) - 1);

    // get port
    arg = get_arg_stop_spaces(arg, buffer);
    port = (unsigned short)atoi(buffer);
    if (port == 0) {
        textfield_add(mud->text, "Wrong port number!\n", MESSAGE_ERR);
        return;
    }
    // get user
    arg = get_arg_stop_spaces(arg, buffer);
    if (strlen(buffer) < 2 || strlen(buffer) > 19) {
        textfield_add(mud->text, "Invalid username (must be between 2 and 19 characters)!\n", MESSAGE_ERR);
        return;
    }
    strcpy(user, buffer);
    // get pwd
    arg = get_arg_stop_spaces(arg, buffer);
    if (strlen(buffer) < 2 || strlen(buffer) > 19) {
        textfield_add(mud->text, "Invalid password %s (must be between 2 and 19 characters)!\n", MESSAGE_ERR);
        return;
    }
    strcpy(pwd, buffer);

    if (mud->chat_input)
        chdisconnect_command(NULL);

    sprintf(buffer, "Connecting to %s/%d as %s...\n",
                    host, port, user); 
    textfield_add(mud->text, buffer, MESSAGE_SENT);

    while(gtk_events_pending())
        gtk_main_iteration();

    if ( ( he = gethostbyname (host) ) == NULL ) {
        popup_window(ERR, "Host not found or host name not valid (%s)", host);
        return;
    }

    if ( ( sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1 ) {
        textfield_add (mud->text, strerror(errno), MESSAGE_ERR);
        return;
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port   = htons( port );
    their_addr.sin_addr   = *((struct in_addr *)he->h_addr);
    memset(&(their_addr.sin_zero), 0, 8);

    if (connect (sockfd, (struct sockaddr *)&their_addr,
                 sizeof (struct sockaddr)) == -1 ) {
            textfield_add (mud->text, strerror(errno), MESSAGE_ERR);
            sockclose(sockfd);
            return;
    }

    sprintf(buffer, "U%s" LINE_MARKER "P%s" LINE_MARKER, user, pwd);
    send(sockfd, buffer, strlen(buffer), 0L);

    textfield_add(mud->text, "** Connected to chat server **\n\n", MESSAGE_SENT);

    while(gtk_events_pending())
        gtk_main_iteration();

    mud->chat_socket = sockfd;
    mud->chat_buffer = malloc(BUFFER_SIZE);
    mud->chat_offset = 0;
    mud->chat_size = BUFFER_SIZE;
    mud->chat_input = gdk_input_add(sockfd, GDK_INPUT_READ,
                                    read_from_chat,
                                    ses );
    // sending initialization...
}


