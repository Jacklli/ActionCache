/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#include <string.h>
#include <pthread.h>
#include "codec.h"
#include "rbtree.h"
#include "conn.h"
#include "buffer.h"
#include "server.h"

int replay(char *buf, int fd) {
    int len = 0;
    if((len = strlen(buf)) != 0)
        write(fd, buf, len);
    return 1;
}

int initBuf(conn *connection) {
    connection->buf = (buffer *)malloc(sizeof(buffer));
    memset(connection->buf, '\0', sizeof(buffer));
    connection->buf->beginrBufpoint = 0;
    connection->buf->cursor = NULL;
    connection->buf->parseBufsize = 0;
    connection->buf->rBuflen = INIT_BUFFER_SIZE;
    connection->buf->rBuflenRemain = INIT_BUFFER_SIZE;
    connection->buf->parsedLen = 0;
    connection->buf->parseFlag = -1;
    connection->buf->argCnt = 0;
    connection->buf->messageBeginpos = 0;
    connection->buf->argBeginpos = NULL;
    connection->buf->rpos = connection->buf->rBuf;
    connection->buf->readDatalen = 0;
    connection->buf->argCnt = 0;
    connection->buf->parseMessageSeq = 0;
    connection->buf->copyMessageSeq = 0;
    connection->buf->corruptMessagelen = 0;
    return 1;
}

conn *newConn(int fd, connTree *tree) {
    int i = 0;
    rb_node_t *root = NULL;
    conn *connection = (conn *)malloc(sizeof(conn));
    memset(connection, '\0', sizeof(conn));
    connection->fd = fd;
    connection->encod = encod;
    connection->decod = decod;
    connection->onWriteDataCallback = NULL;
    connection->onReadMessageCallback = NULL;
    connection->onWriteMessageCallback = NULL;
    if(initBuf(connection) != 1) {
        printf("init Buffer error!\n");
        return NULL;
    }
    connection->cmdCnt = 0;
    if((root = tree->ins(fd, connection, tree->root))) {
        tree->root = root;
        tree->connCnt += 1; 
        return connection;
    }
    return NULL;
}

int freeConn(int fd, connTree *tree) {
    rb_node_t *nd = NULL;
    conn *connection = NULL;
    if(tree->connCnt > 0 && (nd = rb_search(fd, tree->root))) {
        connection = nd->data;
        if(!connection) {
            printf("free connection error!\n");
            return -1;
        }
        tree->connCnt -= 1;
        if(!(tree->root = rb_erase(fd, tree->root)) && tree->connCnt != 0) {
            printf("rb_erase connection node failed!\n");
            return -1;
        }
        free(connection->buf);
        free(connection);
        if(tree->connCnt == 0) tree->root = NULL;
        writeLog(1, "connection fd %d closed.", fd);
    }
    return 1;
}
