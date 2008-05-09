#if defined(WIN32)

#include <winsock2.h>
#define EWOULDBLOCK WSAEWOULDBLOCK
#define SockClose(x) closesocket(x)
#else

#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define SockClose(x) close(x)
#endif /* WIN32 */
#define SockWrite(x, y, z) send(x,y,z,0)
#define SockRead(x, y, z) recv(x,y,z,0)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VERSION "V0.1"

// twelve hours of inactivity timeout
#define INACTIVITY_TIMEOUT (3600 * 12)
#define SOCKETBUFFERSIZE 8192

char server_password[32] = "";
enum { PLR_LOGGING, PLR_LOGGED };

typedef struct _player {
	struct _player *next;
	int socket;
	int status;
	long pingtime;
	time_t lastinput;
	char playername[40];
	char hostname[50];
	char packetbuffer[SOCKETBUFFERSIZE];
    int packetoffset;
    int kill_connection;
} player;

player *playerlist=NULL;

#define MSG_USERNAME 'U'
#define MSG_PASSWORD 'P'
#define MSG_CHAT 'W'
#define LINE_MARKER "*-*"

int serversocket;
int total_players=0,skip_dns=0;

char *getmsg(player *p)
{
	char *end;

    p->packetbuffer[p->packetoffset] = 0;

    if (end = strstr(p->packetbuffer, LINE_MARKER)) {
        static char linebuffer[1024];
        *end = 0;
        strncpy(linebuffer, p->packetbuffer, sizeof(linebuffer)-1);

        end += strlen(LINE_MARKER);
        p->packetoffset -= (end - p->packetbuffer);

        if (p->packetoffset > 0) 
            memmove(p->packetbuffer, end, p->packetoffset);
        else
            p->packetoffset = 0;

        return linebuffer;
    }
	
	return NULL;
}

#include <stdarg.h>

void broadcast(player *p, const char *fmt, ...)
{
    player *point;
    char buffer[2048];
    int bufferlen;
    va_list va;

    va_start(va, fmt);
    vsprintf(buffer, fmt, va);
    va_end(va);
    strcat(buffer, LINE_MARKER);
    bufferlen = strlen(buffer);

    for (point = playerlist; point; point = point->next) {
        if (point == p)
            continue;
        SockWrite(point->socket, buffer, bufferlen);
    }

}
void process_player(player *p)
{
	char *msg = NULL;

	do {
		switch(p->status) {
		case PLR_LOGGING:
			if( (msg = getmsg(p)) ) {
                if (*msg == MSG_USERNAME) {
                    if (strlen(msg + 1)<sizeof(p->playername) ) {
                        strcpy(p->playername, msg + 1);
	    				printf("changing player name for %s to %s\n",p->hostname,p->playername);
                    }
                    
                }
                else if (*msg == MSG_PASSWORD) {
                    if (!*server_password)
                        continue;

                    if (strcmp(msg + 1, server_password)) {
	    				printf("Player %s (%s) used wrong password\n",p->playername, p->hostname);
                        p->kill_connection = 1;
                    }
                    else {
	    				printf("Player %s (%s) correctly logged\n",p->playername, p->hostname);
                        p->status = PLR_LOGGED;
                        broadcast(p, "$c0015Player $c0014%s$c0015 joined the chat.\n", p->playername);
                    }
                }
            }
			break;
		case PLR_LOGGED:
			if( (msg=getmsg(p))) {
                if (*msg == MSG_CHAT)
                    broadcast(p, "$c0007--$c0014%s$c0007--$c0015%s" , p->playername,
                              msg + 1);
            }
			break;
        default:
            printf("Unknown state for player %s, kill him!\n", p->playername);
            p->kill_connection = 1;
        }
	}
	while(msg != NULL);
}

int process_input(player *p,time_t now)
{
    int l;

    l=SockRead(p->socket,p->packetbuffer+p->packetoffset,sizeof(p->packetbuffer)-p->packetoffset);

    if (l<=0) {
        if(errno!=EWOULDBLOCK) {
            printf("Error in socket read!\n");
            return -1;
        }
        else
            l=0;
    }

    p->packetoffset +=l;

    if( p->packetoffset == SOCKETBUFFERSIZE ) {
        printf("Heavy error, closing connection to %s!\n",p->playername);
        return -1;
    }

    if(p->packetoffset) {
        process_player(p);
        p->lastinput=now;
    }

    return l;
}


void close_socket(player *p)
{
	player *t;

	printf("Losing player %s...\n",p->playername);

	SockClose(p->socket);
	total_players--;

	if(playerlist==p)
		playerlist=p->next;
	else {
		for(t=playerlist; t->next!=p; t=t->next);

		t->next=p->next;
	}

    if (p->status == PLR_LOGGED)
        broadcast(NULL, "$C0015Player $c0014%s$c0015 left the chat.\n", p->playername);

	free(p);
}

void new_descriptor(int s)
{
	struct sockaddr_in isa;
	struct hostent *from;
	int i, t;
	unsigned long l;
	player *p,*p2;
	
	i = sizeof(isa);
	
	if ((t = accept(s, (struct sockaddr *)&isa, &i)) < 0){
		return;
	}
	
	if(!(p=calloc(sizeof(player),1))) {
		SockClose(t);
		return;
	}

	if (getpeername(t, (struct sockaddr *) &isa, &i) < 0) {
		*p->hostname = '\0';
	} else if ( skip_dns || !(from = gethostbyaddr((char *)&isa.sin_addr,
									sizeof(isa.sin_addr), AF_INET))) {
		l = isa.sin_addr.s_addr;
		sprintf(p->hostname, "%ld.%ld.%ld.%ld",  (l & 0x000000FF),
			(l & 0x0000FF00) >> 8, (l & 0x00FF0000) >> 16,(l & 0xFF000000) >> 24);
	} else {
		strncpy(p->hostname, from->h_name, sizeof(p->hostname)-1);
		p->hostname[sizeof(p->hostname)-1] = '\0';
	}
	
	total_players++;

	printf("New connection from %s...(players %d)\n",p->hostname,total_players);
	sprintf(p->playername,"player %d",total_players);
	p->socket=t;
	p->lastinput=time(NULL);
	p->status=PLR_LOGGING;
    p->next = playerlist;
    playerlist = p;
}

void my_close_all(void)
{
	player *next;

	while(playerlist) {
		next=playerlist->next;

		close_socket(playerlist);

		playerlist=next;
	}
	SockClose(serversocket);
}

void server_loop(void)
{
	fd_set input_set, output_set, exc_set;
	struct timeval timeout ={5,0};
	player *point,*next_point;
	int max_socket;
	long lastdelay=0;
	time_t nowtime;

	printf("Entering main loop\n");

	for(;;) {
        // kill marked descriptors
        for (point=playerlist; point; point=next_point) {
            next_point = point->next;

            if (point->kill_connection)
                close_socket(point);
        }

// Reset dei descrittori
		FD_ZERO(&input_set);
		FD_ZERO(&output_set);
		FD_ZERO(&exc_set);
		nowtime=time(NULL);

		FD_SET(serversocket, &input_set);
		max_socket=serversocket;

// Configurazione degli input set
		for(point=playerlist; point; point=point->next) {
			FD_SET(point->socket,&input_set);

			if(point->socket>max_socket)
				max_socket=point->socket;
		}
			
		timeout.tv_sec=2;
	    timeout.tv_usec=0;

		select(max_socket+1,&input_set,&output_set,&exc_set,&timeout); 
        
// check player inputs

		for (point = playerlist; point; point = next_point)
		{
			next_point = point->next;
	
// check for errors

			if (FD_ISSET(point->socket, &exc_set))
			{
				FD_CLR((unsigned)point->socket, &input_set);
				FD_CLR((unsigned)point->socket, &output_set);
				close_socket(point);
                continue;
			}

/*
 * check for player input
 */
			if (FD_ISSET(point->socket, &input_set)) {
				if (process_input(point,nowtime) < 0)
					close_socket(point);
			} else if ( (nowtime-point->lastinput)>
				INACTIVITY_TIMEOUT) {
// check for inactivity timeout
				close_socket(point);
			}
		}
	
// check for new descriptors
        if (FD_ISSET(serversocket, &input_set))
			new_descriptor(serversocket);

	}
}

void start_server(int port)
{
    static struct sockaddr_in sa_zero;
    struct sockaddr_in sa;
    int x = 1;

    printf("Creating server on port %d\n",port);

    if ( ( serversocket = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
		printf("Error opening socket\n");
        exit( 1 );
    }
	
    if ( setsockopt( serversocket, SOL_SOCKET, SO_REUSEADDR,
		(char *) &x, sizeof(x) ) < 0 )
    {
		printf("Error with SO_REUSEADDR\n");
		SockClose(serversocket);
        exit( 1 );
    }
	
    sa              = sa_zero;
    sa.sin_family   = AF_INET;
    sa.sin_port     = htons((unsigned short)port);
	
    if ( bind( serversocket, (struct sockaddr *) &sa, sizeof(sa) ) < 0 )
    {
		printf("Error in bind()\n");
		SockClose(serversocket);
        exit( 1 );
    }
	
    if ( listen( serversocket, 3 ) < 0 )
    {
		printf("Error in listen()\n");
		SockClose(serversocket);
        exit( 1 );
    }

	server_loop();
}

int main(int argc, char *argv[])
{
    printf("GGMud chat server " VERSION "\n\n");
#ifdef WIN32
    WSADATA wsaData; 
    WSAStartup(MAKEWORD(2, 0), &wsaData); 
#endif
    if (argc != 3 && argc != 2) {
        printf("Usage: %s port [server password]\n", argv[0]);
        exit(0);
    }
    if (argc == 3)
        sprintf(server_password, argv[2]);

    start_server(atoi(argv[1]));
}
