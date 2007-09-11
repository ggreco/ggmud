/* tsocks.h - Structures used by tsocks to form SOCKS requests */

#ifndef _SOCKS_H

#define _SOCKS_H	1

#ifdef WIN32
#define ETIMEDOUT WSAETIMEDOUT
#define ECONNREFUSED WSAECONNREFUSED
#define EHOSTUNREACH WSAEHOSTUNREACH
#define ECONNABORTED WSAECONNABORTED
#define ENETUNREACH WSAENETUNREACH
#define EWOULDBLOCK WSAEWOULDBLOCK
#include <inttypes.h>
#endif

/* Structure representing a socks connection request */
struct sockreq {
   int8_t version;
   int8_t command;
   int16_t dstport;
   int32_t dstip;
   /* A null terminated username goes here */
};

/* Structure representing a socks connection request response */
struct sockrep {
   int8_t version;
   int8_t result;
   int16_t ignore1;
   int32_t ignore2;
};

/* Structure representing a socket which we are currently proxying */
struct connreq {
   /* Information about the socket and target */
   int sockid;
   struct sockaddr_in connaddr;
   struct sockaddr_in serveraddr;

   /* Current state of this proxied socket */
   int state;

   /* Next state to go to when the send or receive is finished */
   int nextstate;

   /* When connections fail but an error number cannot be reported 
    * because the socket is non blocking we keep the connreq struct until
    * the status is queried with connect() again, we then return
    * this value */
   int err;

   /* Events that were set for this socket upon call to select() or
    * poll() */
   int selectevents;

   /* Buffer for sending and receiving on the socket */
   int datalen;
   int datadone;
   char buffer[1024];

};

/* Connection statuses */
#define UNSTARTED 0
#define CONNECTING 1
#define CONNECTED 2
#define SENDING 3
#define RECEIVING 4
#define SENTV4REQ 5
#define GOTV4REQ 6 
#define SENTV5METHOD 7 
#define GOTV5METHOD 8
#define SENTV5AUTH 9
#define GOTV5AUTH 10
#define SENTV5CONNECT 11
#define GOTV5CONNECT 12
#define DONE 13 
#define CONNFAILED 14 
   
/* Flags to indicate what events a socket was select()ed for */
#define READ (1<<0)
#define WRITE (1<<1)
#define EXCEPT (1<<2)
#define READWRITE (READ|WRITE)
#define READWRITEEXCEPT (READ|WRITE|EXCEPT)

int handle_request(struct connreq *conn);
struct connreq *new_socks_request(int sockid, struct sockaddr_in *connaddr, 
                                         struct sockaddr_in *serveraddr);
void kill_socks_request(struct connreq *conn);

#endif
