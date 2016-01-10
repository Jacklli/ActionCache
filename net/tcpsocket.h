/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __TCPSOCKET_H__
#define __TCPSOCKET_H__ 

#include "event.h"
#include "buffer.h"
#include "conn.h"

int tcpServer(char *err, int port, char *bindaddr);
int tcpAcceptCallback(connTree *tree, eventLoop *eLoop, int fd, int mask);
int tcpReadCallback(connTree *tree, eventLoop *eLoop, int fd, int mask);
int tcpWrite(int fd, char *buf, int count);
int tcpNonBlock(char *err, int fd);
int enableTcpNoDelay(char *err, int fd);
int disableTcpNoDelay(char *err, int fd);


#endif
