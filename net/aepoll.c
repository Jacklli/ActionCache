/* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#include <stdio.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "aepoll.h"

int aePollCreate(eventLoop *eLoop) {
    aePoll *state = (aePoll *)malloc(sizeof(*state));
    if (!state) return -1;
    memset(state, '\0', sizeof(*state));
    state->events = (struct epoll_event*)malloc(sizeof(struct epoll_event)*(eLoop->setsize));
    if (!state->events) {
        free(state);
        return -1;
    }
    memset(state->events, '\0', sizeof(struct epoll_event)*(eLoop->setsize));
    state->epfd = epoll_create(1024); /* 1024 is just a hint for the kernel */
    if (state->epfd == -1) {
        free(state->events);
        free(state);
        return -1;
    }
    eLoop->apidata = state;
    return 1;
}
int aePollFree(eventLoop *eLoop) {
    aePoll *state = eLoop->apidata;
    close(state->epfd);
    free(state->events);
    free(state);
}
int aePollResize(eventLoop *eLoop, int setsize) {
    aePoll *state = eLoop->apidata;
    state->events = realloc(state->events, sizeof(struct epoll_event)*setsize);
    if(state->events)
        return -1;
    return 1;
}
int aePollAddEvent(eventLoop *eLoop, int fd, int mask) {
    aePoll *state = eLoop->apidata;
    struct epoll_event ee;
    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    int op = eLoop->fEvents[fd].mask == 1 ?
            EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    ee.events = 0;
    mask |= eLoop->fEvents[fd].mask; /* Merge old events */
    if (mask & READABLE) ee.events |= EPOLLIN;
    if (mask & WRITEABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0; /* avoid valgrind warning */
    ee.data.fd = fd;
    if (epoll_ctl(state->epfd, op, fd, &ee) == -1)
        return -1;
    return 1;
}
int aePollDeleteEvent(eventLoop *eLoop, int fd, int delmask) {
    aePoll *state = eLoop->apidata;
    struct epoll_event ee;
    int mask = eLoop->fEvents[fd].mask & (~delmask);

    ee.events = 0;
    if (mask & READABLE) ee.events |= EPOLLIN;
    if (mask & WRITEABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0; /* avoid valgrind warning */
    ee.data.fd = fd;
    if (mask != NONE) {
        epoll_ctl(state->epfd,EPOLL_CTL_MOD,fd,&ee);
    } else {
        /* Note, Kernel < 2.6.9 requires a non null event pointer even for
         * EPOLL_CTL_DEL. */
        epoll_ctl(state->epfd,EPOLL_CTL_DEL,fd,&ee);
    }
}
int aePollPoll(eventLoop *eLoop, struct timeval *tm) {
    aePoll *state = eLoop->apidata;
    int retval = 0, numevents = 0;

    retval = epoll_wait(state->epfd,state->events,eLoop->setsize,tm ? (tm->tv_sec*1000 + tm->tv_usec/1000) : -1);
//    retval = epoll_wait(state->epfd,state->events,eLoop->setsize,5000);
    if (retval > 0) {
        int j;

        numevents = retval;
        for (j = 0; j < numevents; j++) {
            int mask = 0;
            struct epoll_event *e = state->events+j;

            if (e->events & EPOLLIN) mask |= READABLE;
            if (e->events & EPOLLOUT) mask |= WRITEABLE;
            if (e->events & EPOLLERR) mask |= WRITEABLE;
            if (e->events & EPOLLHUP) mask |= WRITEABLE;
            eLoop->fired[j].fd = e->data.fd;
            eLoop->fired[j].mask = mask;
        }
    }
    return numevents;
}
