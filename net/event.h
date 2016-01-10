/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __EVENT_H__
#define __EVENT_H__

#include "conn.h"

#define NONE         0
#define READABLE     1
#define WRITEABLE    2

struct eventLoop;
typedef int fileProc(connTree *tree, struct eventLoop *eLoop, int fd, int mask);
typedef struct firedEvent {
    int fd;
    int mask;
} firedEvent;
typedef struct fileEvent {
    int mask;
    fileProc *rfileProc;
    fileProc *wfileProc;
} fileEvent;
typedef struct eventLoop {
    int maxfd;
    int setsize;
    fileEvent *fEvents;
    firedEvent *fired;
    void *apidata;
} eventLoop;
eventLoop *createEventLoop();
int createFileEvent(eventLoop *eLoop, int fd, int mask,
       fileProc *proc); 
int deleteFileEvent(eventLoop *eLoop, int fd, int mask);


#endif
