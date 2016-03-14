/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __SERVER_H__
#define __SERVER_H__

#include <pthread.h>
#include "event.h"
#include "buffer.h"
#include "conn.h"
#include "dict.h"
#include "object.h"

/* refers to your CPU count */
#define THREADCNT 2
#define PORT      33060
#define READABLE 1

pthread_mutex_t connLock[THREADCNT];

typedef struct Server {
    int listenfd;
    int port;
    char *bindaddr;
    char *neterr;
    int tcp_backlog;
    connTree *tree;
    eventLoop *eLoop;
} Server;

int initServer(Server *server);

#endif
