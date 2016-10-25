/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __CONN_H__
#define __CONN_H__

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"
#include "codec.h"
#include "rbtree.h"
#include "object.h"
#include "dict.h"

typedef struct conn {
    int fd;
    buffer *buf;
    int cmdCnt;
    dict *db;
    time_t ctime;           /* connection creation time */
    time_t lastinteraction; /* time of the last interaction, used for timeout */
    int (*encod)(buffer *buf, char **argv);
    int (*decod)(buffer *buf);
    int (*onReadDataCallback)(struct conn *connection, int fd);
    int (*onWriteDataCallback)(int fd);
    int (*onReadMessageCallback)(buffer *buf);
    int (*onWriteMessageCallback)(buffer *buf);
} conn;
typedef struct connTree {
    int connCnt;
    rb_node_t *root;
    rb_node_t *(*ins)(key_t key, void *data, rb_node_t* root);
    rb_node_t *(*del)(key_t key, rb_node_t* root);
    rb_node_t *(*get)(key_t key, rb_node_t* root);
} connTree;

conn *newConn(int fd, connTree *tree);
int freeConn(int fd, connTree *tree);

#endif
