/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/


#include <sys/timerfd.h>  
#include <sys/time.h>  
#include "timer.h"
#include "aepoll.h"

#define READABLE 1

int main() {
    int timerfd;
    int ret = 0;
    int numevents = 0;
    int j = 0;
    int processed = 0;
    struct timeval tvp;
    tvp.tv_sec = 0;
    tvp.tv_usec = 1;
    eventLoop *eLoop = createEventLoop();
    timerfd = createTimerfd();
    if ((ret = createFileEvent(eLoop, timerfd, 1, timerCallback)) == -1) {
        printf("timerfd createFileEvent failed!\n");
    } else {
        printf("timerfd createFileEvent ok!\n");
    }
    if ((ret = aePollCreate(eLoop)) == -1) {
        printf("aePollCreate failed!\n");
    } else {
        printf("aePollCreate ok!\n");
    }
    addTimer(eLoop, timerfd, READABLE);
    runEvery(timerfd, 800);

    int count = 0;
    while(1) {
        numevents = 0;
        numevents = aePollPoll(eLoop, &tvp);
        for (j = 0; j < numevents; j++) {
            fileEvent *fe = &eLoop->fEvents[eLoop->fired[j].fd];
            int mask = eLoop->fired[j].mask;
            int fd = eLoop->fired[j].fd;

	    /* note the fe->mask & mask & ... code: maybe an already processed
             * event removed an element that fired and we still didn't
             * processed, so we check if the event is still valid. */
            if (fe->mask & mask & READABLE)
                fe->rfileProc(NULL, eLoop,fd,mask);
            if (fe->mask & mask & WRITEABLE && fe->wfileProc != fe->rfileProc)
                fe->wfileProc(NULL, eLoop,fd,mask);
            processed++;
        }
        count++;
//        if(count==10000000) deleteTimer(eLoop, timerfd, 1);
    } 
}
