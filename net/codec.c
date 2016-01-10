/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)

*protocol is illustrated here
*    buf starts with '*'
*    the second part of message is the length of current message, which has 4 bytes length.
*    message is seperated with "\t"
*    message is ended with "!"
*
*message example:
*    message "set a10 121" translated into
*    "*\t0020\tset\ta10\t121\t!"
*
* warning!!!
* if the 4 byte for message length are not accurate,parse error may will cause coredump.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codec.h"
#include "object.h"
#include "log.h"

int repairLastCorruptMessage(buffer *buf, int corruptMessagelen) {
    char *ptr = NULL;
    buf->corruptMessagelen = corruptMessagelen;

    if(!(ptr = memset(buf->rBuf, '\0', buf->rBuflen-corruptMessagelen))) {
        writeLog(1, "%s", "memset error!\n");
        return -1;
    }
    ptr = NULL;
    if(!(ptr = memcpy(buf->rBuf, buf->rBuf + buf->rBuflen - corruptMessagelen, corruptMessagelen))) {
        printf("memcpy error!\n");
        return -1;
    }
/* important */
    buf->parsedLen = 0; 
    return 1;
}

static int catchOneMessagefromBuffer(buffer *buf) {
    int messageLen = 0, parseRemainlen = 0;
    buf->messageBeginpos = buf->rBuf + buf->parsedLen;
    buf->cursor = buf->rBuf + buf->parsedLen;
    if(buf->readDatalen <= buf->parsedLen || *(buf->rBuf + buf->parsedLen) != '*') {
        buf->parseFlag = -1;
        buf->parsedLen = 0;
        buf->corruptMessagelen = 0;
        return -1;
    }
    buf->cursor++;
    buf->cursor++;
    buf->parsedLen++;
    buf->parsedLen++;
    
    parseRemainlen = buf->rBuflen - buf->parsedLen;
    if(parseRemainlen <= 6 - 2) {
        buf->parseFlag = 1;
        buf->parsedLen -= 2;
        if(repairLastCorruptMessage(buf, parseRemainlen + 2) == -1)
            return -1;
        return -1;
    }
    if(*buf->cursor < '0' || *buf->cursor > '9') {
        buf->parseFlag = -1;
        buf->parsedLen = 0;
        buf->corruptMessagelen = 0;
        logFile(buf->messageBeginpos,INIT_BUFFER_SIZE - buf->parsedLen);
        logFile(buf->messageBeginpos, messageLen);
        return -1;
    }
    if(*(buf->cursor+1) < '0' || *(buf->cursor+1) > '9') {
        buf->parseFlag = -1;
        buf->parsedLen = 0;
        buf->corruptMessagelen = 0;
        logFile(buf->messageBeginpos,INIT_BUFFER_SIZE - buf->parsedLen);
        logFile(buf->messageBeginpos, messageLen);
        return -1;
    }
    if(*(buf->cursor+2) < '0' || *(buf->cursor+2) > '9') {
        buf->parseFlag = -1;        buf->parsedLen = 0;
        buf->corruptMessagelen = 0;
        logFile(buf->messageBeginpos,INIT_BUFFER_SIZE - buf->parsedLen);
        logFile(buf->messageBeginpos, messageLen);
        return -1;
    }
    if(*(buf->cursor+3) < '0' || *(buf->cursor+3) > '9') {
        buf->parseFlag = -1;        buf->parsedLen = 0;
        buf->corruptMessagelen = 0;
        logFile(buf->messageBeginpos,INIT_BUFFER_SIZE - buf->parsedLen);
        logFile(buf->messageBeginpos, messageLen);
        return -1;
    }
    messageLen = ((int)(*buf->cursor-'0'))*1000 + ((int)(*(buf->cursor+1)-'0'))*100 + ((int)(*(buf->cursor+2)-'0'))*10 + ((int)(*(buf->cursor+3)-'0'))*1;
    if(parseRemainlen < messageLen - 2) {
        buf->parseFlag = 1;
        buf->parsedLen -= 2;
        if(repairLastCorruptMessage(buf, parseRemainlen + 2) == -1)
            return -1;
        return -1;
    }

    buf->parsedLen += 4;
    buf->cursor += 4;
    buf->copyMessageSeq++;
    if(buf->copyMessageSeq == 29999999) writeLog(1, "reach 29999999");
    return parseOneMessage(buf);
}

int parseOneMessage(buffer *buf) {
    int argLen = 0, preArgLen = 0;
    int separLen = 0;
    int argCnt = 0;

    buf->argBeginpos = buf->cursor;
    while(buf->parsedLen != buf->readDatalen && buf->cursor) {
        if((*buf->cursor == '\t') && argLen == 0) {
            buf->cursor++;
            preArgLen++;
        }
        else if((*buf->cursor == '\t') && argLen != 0) {
            buf->argBeginpos += (preArgLen + separLen);
            buf->parsedLen += (preArgLen + separLen);
            memset(buf->argv[argCnt], '\0', ARGUMENTCNT);
            memcpy(buf->argv[argCnt], buf->argBeginpos, argLen);
            buf->cursor++; 
            preArgLen = argLen;
            argLen = 0;
            separLen = 1;
            argCnt++;
            if(argCnt > 3) {
                buf->parseFlag = -1;
                break;
            }
        }
        else if(*buf->cursor == '!') {
            buf->argBeginpos += (preArgLen + separLen);
            buf->parsedLen += (preArgLen + separLen);
            memcpy(buf->argv[argCnt], buf->argBeginpos, argLen);
            argCnt++; 
            buf->cursor++;
            buf->parsedLen++;
            if(*buf->cursor == '\n') {
                buf->cursor++; buf->parsedLen++;
            }
            break;
        }
        else {
            buf->cursor++;
            argLen++;
        }
    }

    buf->parseFlag = 0;

    buf->argCnt = argCnt;
    buf->parseMessageSeq++;
    return argCnt;
} 

int decod(buffer *buf) {
    int commandCnt = 0;
    valObject *obj = NULL;
    commandCnt = catchOneMessagefromBuffer(buf) / 3;
    return commandCnt;
}
int encod(buffer *buf, char **argv) {
    printf("TO DO: Call encod\n");
    return 1;
}
