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
/* Autoconf patching by David Hedbor, neotron@lysator.liu.se */
/*********************************************************************/
/* file: net.c - do all the net stuff                                */
/*                             TINTIN III                            */
/*          (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t             */
/*                     coded by peter unold 1992                     */
/*********************************************************************/

#include "config.h"
#include "tintin.h"

#if defined(HAVE_STRING_H)
#include <string.h>
#elif defined(HAVE_STRINGS_H)
#include <strings.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <sys/types.h>

#ifdef WIN32

#define alarm(x) 

#include <winsock2.h>
#define sockclose(x) closesocket(x)
#define ECONNREFUSED WSAECONNREFUSED
#define ENETUNREACH WSAENETUNREACH
#define EWOULDBLOCK WSAEWOULDBLOCK
#else
#define sockclose(x) close(x)
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <arpa/inet.h>
#ifdef HAVE_NET_ERRNO_H
#include <net/errno.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef SOCKS
#include <socks.h>
#endif

#endif

#include "include/main.h"
#include "include/net.h"
#include "include/parse.h"
#include "include/rl.h"
#include "include/utils.h"
#include "include/log.h"

#ifndef BADSIG
#define BADSIG (RETSIGTYPE (*)(int))-1
#endif

static RETSIGTYPE alarm_handler(int no_care)
{
  /* do nothing; the connect will fail, returning -1, with errno = EINTR */
}

/**************************************************/
/* try connect to the mud specified by the args   */
/* return fd on success / 0 on failure            */
/**************************************************/

int connect_mud(const char *host, const char *port, struct session *ses)
{
  int sock, connectresult;
  struct sockaddr_in sockaddr;
  char buf[1024];

  if(isdigit(*host))                            /* interprete host part */
    sockaddr.sin_addr.s_addr = inet_addr(host);
  else {
    struct hostent *hp;

    if(!(hp = gethostbyname(host))) {
      tintin_puts("#ERROR - UNKNOWN HOST.", ses);
      prompt(NULL); 
      return(0);
    }
    memcpy((char *)&sockaddr.sin_addr, hp->h_addr, sizeof(sockaddr.sin_addr));
  }

  if(isdigit(*port))  
    sockaddr.sin_port = htons((short)atoi(port));      /* inteprete port part */
  else {
    tintin_puts("#THE PORT SHOULD BE A NUMBER.", ses);
    prompt(NULL);
    return(0);
  }

  if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    syserr("socket");

  sockaddr.sin_family = AF_INET;


  tintin_puts("#Trying to connect..", ses);

#ifndef WIN32
  /* ignore the alarm: it'll cause the connect to return -1 with errno=EINTR */
  if(signal(SIGALRM, alarm_handler) == BADSIG)
    syserr("signal SIGALRM");
#endif
  alarm(30);         /* We'll allow connect to hang in 15seconds! NO MORE! */

  connectresult = connect(sock, (struct sockaddr *)&sockaddr, sizeof(sockaddr));

  alarm(0);

  if(connectresult) {
    sockclose(sock);
    switch(errno) {
    case EINTR:
      tintin_puts("#CONNECTION TIMED OUT.", ses);
      break;
    case ECONNREFUSED:
      tintin_puts("#ERROR - CONNECTION REFUSED.", ses);
      break;
    case ENETUNREACH:
      tintin_puts("#ERROR - THE NETWORK IS NOT REACHABLE FROM THIS HOST.", ses);
      break;
    default:
      if (strerror(errno)) {
        sprintf(buf, "#ERROR - %s.", strerror(errno));
        tintin_puts(buf, ses);
      }

    } 
    prompt(NULL);
    return(0);
  }
  return(sock);
}



/*******************************************************************/
/* read at most BUFFER_SIZE chars from mud - parse protocol stuff  */
/*******************************************************************/

#ifndef TELNET_SUPPORT 
int read_buffer_mud(char *buffer, struct session *ses)
{
  int i, didget;
  char tmpbuf[BUFFER_SIZE], *cpsource, *cpdest;
  tmpbuf[0] = '\0';
  didget = recv(ses->socket, tmpbuf, 512, 0);
  ses->old_more_coming = ses->more_coming;
  ses->more_coming = (didget == 512 ? 1 : 0);

  if(didget < 0) 
    return(-1); /*syserr("read from socket");  we do this here instead - dunno quite
                why, but i got some mysterious connection read by peer on some hps */
  else if (didget == 0 && errno == EWOULDBLOCK)
    return 0;
  else if(!didget)
    return(-666);
  else {
    cpsource = tmpbuf;
    cpdest = buffer;
    i = didget;

    translate_telnet_protocol(cpdest, cpsource, i);

  }
  return(didget);
}

extern void input_line_visible(int );


void translate_telnet_protocol(unsigned char *dst, unsigned char *src, 
				 size_t srclen)
{
    static int input_hidden = 0;
    char buf[100];
    int skip_telnet = 0;
    unsigned char *cpdst;
    unsigned char *cpsrc;
    
    cpdst = dst;
    cpsrc = src ;

    while(srclen) {
        if(*cpsrc == TELNET_IAC) {  /* if telnet command */
//            sprintf(buf, "%X, %X, %X", cpsrc[1], cpsrc[2], cpsrc[3]);
//            tintin_puts2(buf, NULL);
            
            if(cpsrc[1] == 0xfb &&
               cpsrc[2] == 1 &&
               cpsrc[3] == 0 ) {
            
                input_hidden = 1;
                
                input_line_visible(FALSE);
            }
            else if(input_hidden) {
                input_hidden = 0;
                input_line_visible(TRUE);
            }
                
            switch(cpsrc[1]) {
                /* 2-byte commands ---- begin */
                case TELNET_SE:    skip_telnet = 2 ; break ;
                case TELNET_NOP:   skip_telnet = 2 ; break ;
                case TELNET_DM:    skip_telnet = 2 ; break ;
                case TELNET_BREAK: skip_telnet = 2 ; break ;
                case TELNET_IP:    skip_telnet = 2 ; break ;
                case TELNET_AO:    skip_telnet = 2 ; break ;
                case TELNET_AYT:   skip_telnet = 2 ; break ;
                case TELNET_EC:    skip_telnet = 2 ; break ;
                case TELNET_EL:    skip_telnet = 2 ; break ;
                case TELNET_GA:    *cpdst++ = '\n' ;
                                   skip_telnet = 2 ;
                                   break ;
                case TELNET_SB:    skip_telnet = 2 ; break ;
                                   /* 2-byte commands ---- end */
                                   /* 3-byte commands ---- begin */
                case TELNET_WILL:  skip_telnet = 3 ; 
                                   break ;
                case TELNET_WONT:  skip_telnet = 3 ; 
                                   break ;
                case TELNET_DO:    skip_telnet = 3 ; break ;
                case TELNET_DONT:  skip_telnet = 3 ; break ;
                                   /* 3-byte commands ---- end */

                                   /* data commands   ---- begin */
                case TELNET_IAC:   *cpdst++ = 0xFF ;
                                   skip_telnet = 2 ;
                                   break ;
                                   /* data commands   ---- end */
                                   /* unknown telnet commands gonna be 
                                      just skipped. */

                default:           skip_telnet = 1 ; break ;
            } /* switch */
            srclen -= skip_telnet ;
            cpsrc  += skip_telnet ;
        } else {                   /* if not telnet command */
            /* skip (char)0 in socket input */
            if(*cpsrc==0)
                cpsrc++;
            else
                *cpdst++ = *cpsrc++ ;
            srclen -- ;
        } /* if */
    } /* while */
    *cpdst = '\0' ;

}

#else

int read_buffer_mud(char *buffer, struct session *ses)
{
    extern int do_telnet_protocol(unsigned char *data,int nb,struct session *ses);
    int i, didget, b;
    char *cpsource, *cpdest;
    char tmpbuf[INPUT_CHUNK + 1];
#ifdef ENABLE_MCCP
	const char  *string;
	int   mccp_i;
    char *mccp_buffer;
#endif

    didget = recv(ses->socket, tmpbuf, INPUT_CHUNK, 0);

    if (didget < 0)
        return -1;
    else if (didget == 0 && errno == EWOULDBLOCK)
        return 0;
    else if (didget == 0)
        return -666;

    ses->old_more_coming = ses->more_coming;
 
    if (didget == INPUT_CHUNK)
        ses->more_coming = 1;
    else
        ses->more_coming = 0;

    tmpbuf[didget]=0;

    cpdest = buffer;
#ifdef ENABLE_MCCP
    mudcompress_receive(ses->mccp, tmpbuf, didget);

	while((mccp_i = mudcompress_pending(ses->mccp)) > 0)
	{
		mccp_buffer = calloc(1, mccp_i + 1 + ses->telnet_buflen);
		mudcompress_get(ses->mccp, mccp_buffer + ses->telnet_buflen, mccp_i);

        if (ses->telnet_buflen) {
            memcpy(mccp_buffer, ses->telnet_buf, ses->telnet_buflen);
            ses->telnet_buflen = 0;
        }

        i = mccp_i;
        cpsource = mccp_buffer;
#else
    cpsource = tmpbuf;
    i = didget;
#endif
   
    while (i > 0) {
        switch(*(unsigned char *)cpsource)
        {
        case 0:
            i--;
            didget--;
            cpsource++;
            break;
        case 255:
            b=do_telnet_protocol((unsigned char *)cpsource, i, ses);
            switch(b)
            {
            case -1:
                memmove(ses->telnet_buf + ses->telnet_buflen, cpsource, i);
                ses->telnet_buflen+=i;
                i = 0;
                break;
            case -2:
            	i-=2;
            	didget-=2;
            	cpsource+=2;
//            	if (!i)
//            		ses->ga=1;
            	break;
            case -3:
                i -= 2;
                didget-=1;
                *cpdest++=255;
                cpsource+=2;
                break;
            default:
                i -= b;
                didget-=b;
                cpsource += b;
            }
            break;
        default:
            *cpdest++ = *cpsource++;
            i--;
        }
    }

#ifdef ENABLE_MCCP
        free(mccp_buffer);
	}
   
	if ((string = mudcompress_response(ses->mccp)) 
            != NULL) {
		send (ses->socket, string, strlen(string), 0);
 	}
#endif  
    *cpdest = '\0';

    return strlen(buffer);
}
#endif


