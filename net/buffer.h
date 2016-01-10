/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define READ_GRANULARITY 100
#define INIT_BUFFER_SIZE 100
#define INC_BUFFER_GRANULARITY 100
#define ARGUMENTCNT 10

typedef struct buffer {
    int rBuflen;
    int rBuflenRemain;
    int wBuflen;
    int wBuflenRemain;
    int readDatalen;
    int parsedLen;
    int parseFlag;
    int parseBufsize;
    int beginrBufpoint;
    int copyMessageSeq;
    int parseMessageSeq;
    int corruptMessagelen;
    char *rpos;
    char *wpos;
    char rBuf[INIT_BUFFER_SIZE];
    char *wBuf;
    char *messageBeginpos;
    char *parseBuf;
    char *parseBeginpos;
    char *argBeginpos;
    char *cursor;
    int argCnt;
    char argv[ARGUMENTCNT/2][ARGUMENTCNT];
    int (*expandBuf)(struct buffer *buf, int newSize, int rwFlag);

    int (*fetchBufdec)();
    int (*putBufenc)();
} buffer;


#endif
