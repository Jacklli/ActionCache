/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache 
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"
#include "aepoll.h"

eventLoop *createEventLoop() {
    int setsize = 10000;
    eventLoop *eLoop = NULL;
    if(!(eLoop = (eventLoop *)malloc(sizeof(*eLoop)))) {
        printf("create eLoop failed due to not enough memory remained.\n");
        return NULL;
    }
    memset(eLoop, '\0', sizeof(*eLoop));
    eLoop->setsize = setsize;
    if(!(eLoop->fEvents = (fileEvent *)malloc(sizeof(struct fileEvent)*(eLoop->setsize)))) {
        printf("create fileEvent set failed due to not enough memory remained.\n");
        return NULL;
    }
    memset(eLoop->fEvents, '\0', sizeof(struct fileEvent)*(eLoop->setsize));
    if(!(eLoop->fired = (firedEvent *)malloc(sizeof(struct firedEvent)*(eLoop->setsize)))) {
        printf("create firedEvent set failed due to not enough memory remained.\n");
        return NULL;
    }
    memset(eLoop->fired, '\0', sizeof(struct firedEvent)*(eLoop->setsize));
    return eLoop;
}
int createFileEvent(eventLoop *eLoop, int fd, int mask,
       fileProc *proc) {
    fileEvent *fe = NULL;
    if(fd > eLoop->setsize) {
        return -1;
    }
    fe = &(eLoop->fEvents[fd]); 
    fe->mask |= mask;
    if(mask & READABLE)
        fe->rfileProc = proc;
    if(mask & WRITEABLE)
        fe->wfileProc = proc;
    if(fd > eLoop->maxfd)
        eLoop->maxfd = fd;
    return 1;
}
int deleteFileEvent(eventLoop *eLoop, int fd, int mask) {
    int j = 0;
    if (fd >= eLoop->setsize) {
        printf("fd error\n");
        return -1;
    }
    fileEvent *fe = &eLoop->fEvents[fd];
    if (fe->mask == NONE) return -1;

    if (aePollDeleteEvent(eLoop, fd, mask) == -1) return -1;
    fe->mask = fe->mask & (~mask);
    if (fd == eLoop->maxfd && fe->mask == NONE) {
        for (j = eLoop->maxfd-1; j >= 0; j--)
            if (eLoop->fEvents[j].mask != NONE) break;
        eLoop->maxfd = j;
    }
}
