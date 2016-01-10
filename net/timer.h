/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdio.h>
#include <sys/timerfd.h>
#include "event.h"
#include "conn.h"


int timerCallback(connTree *tree, struct eventLoop *eLoop, int fd,int mask);
int createTimerfd();
int addTimer(eventLoop *eLoop, int timerfd, int mask);
int deleteTimer(eventLoop *eLoop, int timerfd, int mask);
int runEvery(int timerfd, int interval);   //interval counts by milliseconds

#endif
