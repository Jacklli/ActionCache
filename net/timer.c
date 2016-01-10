/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache 
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#include "timer.h"
#include "log.h"
#include "server.h"
#include "dict.h"

extern dict *db[THREADCNT];

int timerCallback(connTree *tree, struct eventLoop *eLoop, int fd,int mask) {
    int i = 0, keys = 0;
    long long tmp;
    read(fd, tmp, sizeof(tmp));
    writeLog(1, "timerCallback!");
    return 1;
}
int createTimerfd() {
    int timerfd = -1;
    if ((timerfd = timerfd_create(CLOCK_MONOTONIC, 0)) == -1) {
        printf("create timerfd error!\n");
        return -1;
    }
    return timerfd;
}
int addTimer(eventLoop *eLoop, int timerfd, int mask) {
    int ret = -1;
    if ((ret = aePollAddEvent(eLoop, timerfd, mask)) == -1) {
        printf("add timer failed!\n");
        return -1;
    }
    return 1;
}
int deleteTimer(eventLoop *eLoop, int timerfd, int mask) {
    return deleteFileEvent(eLoop, timerfd, mask);
}
int runEvery(int timerfd, int interval) {
    struct itimerspec interv;
    interv.it_value.tv_sec = 0;
    interv.it_value.tv_nsec = 1;
    interv.it_interval.tv_sec = 0;
    interv.it_interval.tv_nsec = interval*1000000;
    if (timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &interv, NULL) == -1) {
        printf("timerfd_settime failed!\n");
        return -1;
    }
    return 1;
}
