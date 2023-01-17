/* wolfssl_KEIL_ARM.h
 *
 * Copyright (C) 2006-2022 wolfSSL Inc.  All rights reserved.
 *
 * This file is part of wolfSSL.
 *
 * Contact licensing@wolfssl.com with any questions or comments.
 *
 * https://www.wolfssl.com
 */

/******************************************************************************/
/**   This file is for defining types, values for specific to KEIL-MDK-ARM.  **/
/******************************************************************************/
#ifndef WOLFSSL_KEIL_ARM_H
#define WOLFSSL_KEIL_ARM_H

#include <stdio.h>

#define SOCKET_T int

/*** #include <socket.h> ***/
#define  NUMBITSPERBYTE 8
#define FD_SETSIZE 10

typedef long fd_mask;
#define NFDBITS   (sizeof(fd_mask) * NUMBITSPERBYTE)  /* bits per mask */

typedef struct fd_set {
    fd_mask fds_bits[(FD_SETSIZE + NFDBITS - 1) / NFDBITS];
} fd_set;

/*** #include <sys/types.h> ***/
struct timeval {
   long tv_sec;     /* seconds      */
   long tv_usec;    /* microseconds */
};


#if defined(WOLFSSL_KEIL_TCP_NET)

#define SCK_EWOULDBLOCK     BSD_ERROR_WOULDBLOCK
#define SCK_ETIMEOUT        BSD_ERROR_TIMEOUT
#include "rl_net.h"

typedef int socklen_t ;

/* for avoiding conflict with KEIL-TCPnet BSD socket */
/* Bodies are in cyassl_KEIL_RL.c                    */
#if defined(HAVE_KEIL_RTX)
    #define sleep(t) os_dly_wait(t/1000+1) ;
#elif defined (WOLFSSL_CMSIS_RTOS)
    #define sleep(t)  osDelay(t/1000+1) ;
#endif

/* for avoiding conflicting with KEIL-TCPnet TCP socket */
/* Bodies are in test.h */
#define tcp_connect   wolfssl_tcp_connect
#define tcp_socket    wolfssl_tcp_soket
#define tcp_listen    wolfssl_tcp_listen
#define connect(a,b,c)    connect(a, (SOCKADDR *)(b), c)
#define SOCKADDR struct sockaddr
	
/** KEIL-RL TCPnet ****/
/* TCPnet BSD socket does not have following functions. */
extern char *inet_ntoa(struct in_addr in);
extern unsigned long inet_addr(const char *cp);
extern int select(int nfds, fd_set *readfds, fd_set *writefds,
                          fd_set *exceptfds, const struct timeval *timeout);

#endif /* WOLFSSL_KEIL_TCP_NET */

#endif /* WOLFSSL_KEIL_ARM_H */
