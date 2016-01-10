/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by a GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "tcpsocket.h"
#include "conn.h"
#include "execcommand.h"
#include "dict.h"
#include "server.h"
#include "log.h"


extern dict *db[THREADCNT];
extern eventLoop *globalEloop[THREADCNT];
extern connTree *globalconnTree[THREADCNT];
extern pthread_mutex_t connLock[THREADCNT];

static int tcpRead(conn *connection, int fd, buffer *buf) {
    int i = 0;
    int nread = -2;
    valObject *obj = NULL;
    char *message = NULL;

    connection->buf->rpos = connection->buf->rBuf + connection->buf->corruptMessagelen;
    connection->buf->rBuflenRemain = connection->buf->rBuflen - connection->buf->corruptMessagelen;
    memset(connection->buf->rpos, '\0', connection->buf->rBuflenRemain);

    while(1) {
        connection->buf->rBuflenRemain = connection->buf->rBuflen - connection->buf->corruptMessagelen;
        connection->buf->rpos = connection->buf->rBuf + connection->buf->corruptMessagelen;
        memset(connection->buf->rpos, '\0', connection->buf->rBuflenRemain);

        nread = read(fd,buf->rpos,READ_GRANULARITY - connection->buf->corruptMessagelen);
        buf->readDatalen = nread;
        buf->parsedLen = 0; 
        if(nread == 0) return 0;
        if(nread == -1 && errno == EAGAIN) {
            message = strerror(errno);
            return -1;
        }
        connection->commandCnt = connection->decod(connection->buf);
        connection->db = db[atoi(connection->buf->argv[1])%THREADCNT];
        while(connection->buf->parseFlag != 1 && connection->buf->parseFlag != -1) {     
            if(connection->buf->argv[0] && strcmp(connection->buf->argv[0], "set") ==0 ) {
                execSetCommand(connection->buf->argv);
                replay("set ok", fd);
            }
            else if(connection->buf->argv[0] && strcmp(connection->buf->argv[0], "get") == 0) {
                obj = execGetCommand(connection->buf->argv);
                if(obj && obj->ptr)
                    replay(obj->ptr, fd);
                else
                    replay("not found.\n", fd);
            }
            else if(connection->buf->argv[0] && strcmp(connection->buf->argv[0], "shutdown") == 0) {
                int i = 0;
                for(i = 0;i < THREADCNT; i++) {
                    if(db[i]) {
                        execShutdownCommand(db[i]);
                    }
                }
            }
            connection->commandCnt = connection->decod(connection->buf);
        }
    }
    return nread;
}
int tcpReadCallback(connTree *tree, eventLoop *eLoop, int fd, int mask) {
    conn *connection = NULL;
    int readlen = 0;
    rb_node_t *node = NULL;
    if(!(node = rb_search(fd, tree->root))) return -1;
    connection = node->data;
    if((readlen = tcpRead(connection, fd, connection->buf)) == 0) {
        deleteFileEvent(eLoop, fd, mask);
        freeConn(fd, tree);
    }
    return 1;
}
static int genericTcpAccept(char *err, int s, struct sockaddr *sa, socklen_t *len) {
    int fd;
    while(1) {
        fd = accept(s,sa,len);
        if (fd == -1) {
            if (errno == EINTR)
                continue;
            else {
                return -1;
            }
        }
        break;
    }
    return fd;
}
static int tcpAccept(char *err, int s, int *port) {
    int fd;
    struct sockaddr_in sa;
    socklen_t salen = sizeof(sa);
    if ((fd = genericTcpAccept(err,s,(struct sockaddr*)&sa,&salen)) == -1)
        return -1;
    if((tcpNonBlock(err,fd)) < 0) {
        writeLog(1,"%s", "set socket nonBlock failed!");
        return -1;
    }
    return fd;
}
int tcpAcceptCallback(connTree *tree, eventLoop *eLoop, int fd, int mask) {
    char *err = NULL;
    int *port = NULL;
    int connfd = -1;
    conn *connection = NULL;
    rb_node_t* root;

int i = 0,minconns = 0,minconnsindex = 0;


    if ((connfd = tcpAccept(err, fd, port)) > 0) {
        writeLog(1, "new connection forked from listenfd %d, connfd is %d, processed by thread %lu",fd, connfd, pthread_self());
        for(i = 1,minconnsindex; i < THREADCNT; i++) {
            if((globalconnTree[i] && globalconnTree[i-1] &&  \
                globalconnTree[i]->connCnt < globalconnTree[i-1]->connCnt) || !globalconnTree[i-1])
                minconnsindex = i;
        }
        if (createFileEvent(globalEloop[minconnsindex], connfd, READABLE, tcpReadCallback) == -1) return -1;
        if (aePollAddEvent(globalEloop[minconnsindex], connfd, READABLE) == -1) {
            writeLog(1,"%s" "add tcpReadCallback failed!");
            return -1;
        }
        if(!(connection = newConn(connfd, globalconnTree[minconnsindex]))) {
            writeLog(1, "%s", "create connection error!\n");
            return -1;
        }
        return connfd;
    }
    return connfd;
}
static int createSocket(char *err, int domain) {
    int s, on = 1;
    if ((s = socket(domain, SOCK_STREAM, 0)) == -1) {
        return -1;
    }

    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        return -1;
    }
    return s;
}
static int tcpListen(char *err, int s, struct sockaddr *sa, socklen_t len) {
    if (bind(s,sa,len) == -1) {
        close(s);
        return -1;
    }
    if (listen(s, 511) == -1) {
        close(s);
        return -1;
    }
    return 1;
}
int tcpServer(char *err, int port, char *bindaddr) {
    int s;
    struct sockaddr_in sa;
    if ((s = createSocket(err,AF_INET)) == -1)
        return -1;
    memset(&sa,0,sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bindaddr && inet_aton(bindaddr, &sa.sin_addr) == 0) {
        close(s);
        return -1;
    }

    if((tcpNonBlock(err,s)) < 0) {
        writeLog(1,"%s", "set socket nonBlock failed!");
        return -1;
    }

    if (tcpListen(err,s,(struct sockaddr*)&sa,sizeof(sa)) == -1)
        return -1;
    return s;
}
int tcpWrite(int fd, char *buf, int count) {
    int nwritten, totlen = 0;
    while(totlen != count) {
        nwritten = write(fd,buf,count-totlen);
        if(nwritten == 0)
            return totlen;
        if(nwritten == -1)
            return -1;
        totlen += nwritten;
        buf += nwritten;
    }
    return totlen;
}
static int tcpSetBlock(char *err, int fd, int non_block) {
    int flags;
    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        return -1;
    }
    if (non_block)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        return -1;
    }
    return 1;
}
int tcpNonBlock(char *err, int fd) {
    return tcpSetBlock(err,fd,1);
}
int tcpBlock(char *err, int fd) {
    return tcpSetBlock(err,fd,0);
}
static int tcpDelayCtl(char *err, int fd, int val) {
    if(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val)) == -1)
        return -1;
    return 1;
}
int enableTcpNoDelay(char *err, int fd) {
    return tcpDelayCtl(err, fd, 1);
}

int disableTcpNoDelay(char *err, int fd) {
    return tcpDelayCtl(err, fd, 0);
}
