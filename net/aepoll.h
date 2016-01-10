/* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __EPOLL_H__
#define __EPOLL_H__

#include <sys/time.h>
#include "event.h"

typedef struct aePoll {
    int epfd;
    struct epoll_event *events;
} aePoll;

int aePollCreate(eventLoop *eLoop);
int aePollFree(eventLoop *eLoop);
int aePollResize(eventLoop *eLoop, int setsize);
int aePollAddEvent(eventLoop *eLoop, int fd, int mask);
int aePollDeleteEvent(eventLoop *eLoop, int fd, int delmask);
int aePollPoll(eventLoop *eLoop, struct timeval *tm);


#endif
