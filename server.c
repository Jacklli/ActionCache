/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache 
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include "buffer.h"
#include "event.h"
#include "aepoll.h"
#include "timer.h"
#include "tcpsocket.h"
#include "conn.h"
#include "rbtree.h"
#include "server.h"
#include "dict.h"
#include "log.h"


/* these global variables are exposed to all threads */
/* they are all immutable */
dict *db[THREADCNT] = {NULL};
eventLoop *globalEloop[THREADCNT] = {NULL};
connTree *globalconnTree[THREADCNT] = {NULL};
int serverport = PORT;
int listenfd = 0;
pthread_mutex_t lock[THREADCNT];
pthread_mutex_t eloopidLock;
int eloopid = 0;
extern dictType dDictType;


/* each thread will have its private eventLoop & 
 db (which is exposed to other thread through global *db[] array) */
Server *initServerPrivate() {
    pthread_attr_t pattr;
    size_t size = 0;
    int cpuNum = 0;
    int cpuSeq = 0;
    char *message = NULL;

cpu_set_t mask;

    Server *server = (Server *)malloc(sizeof(struct Server));
    //init server network
    server->neterr = NULL;
    server->port = serverport;  //temp use for 33060
    server->bindaddr = NULL;
    server->listenfd = listenfd;  

    //init conection tree, each thread has its own connection ree
    pthread_mutex_lock(&eloopidLock);
    server->eLoop = globalEloop[eloopid];
    server->tree = globalconnTree[eloopid];
    eloopid++;
    pthread_mutex_unlock(&eloopidLock);

    cpuNum = sysconf(_SC_NPROCESSORS_CONF);
    printf("cpuNum is:%d\n", cpuNum);
    cpuSeq = (eloopid - 1) % cpuNum;

CPU_ZERO(&mask);      
CPU_SET(cpuSeq, &mask);      //绑定到cpu eloopid,eloopid与server的线程序号相等

if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) == -1) {
    printf("set affinity failed..\n");  
    strerror(errno);
    printf("err msg is: %d\n", errno);
}

CPU_ZERO(&mask);  

if(pthread_getaffinity_np(pthread_self(), sizeof(mask), &mask) == -1) {
    printf("get affinity failed..\n");  
}

if(CPU_ISSET(cpuSeq, &mask)) {
    printf("new thread %d run on processor %d\n", pthread_self(), cpuSeq);

} else {
    printf("set CPU fialed\n");
}
    return server;

}

int mainLoop(Server *server) {
    int j = 0, numevents = 0, processed = 0;
    struct timeval tvp;

/* main loop */
    while(1) {
        tvp.tv_sec = 0;
        tvp.tv_usec = 10;
        numevents = 0;
        numevents = aePollPoll(server->eLoop, &tvp);
        for (j = 0; j < numevents; j++) {
            fileEvent *fe = &server->eLoop->fEvents[server->eLoop->fired[j].fd];
            int mask = server->eLoop->fired[j].mask;
            int fd = server->eLoop->fired[j].fd;

            /* note the fe->mask & mask & ... code: maybe an already processed
             * event removed an element that fired and we still didn't
             * processed, so we check if the event is still valid. */
            if (fe->mask & mask & READABLE)
                fe->rfileProc(server->tree, server->eLoop, fd, mask);
            processed++;
        }
    }
    return 1;
}
void *runServer() {
    Server *server = NULL;
    if(!(server = initServerPrivate())) {
        printf("initServerPrivate error!\n");
    }
    if(mainLoop(server) != 1);
}

void *serverCron() {
    int i = 0, keys = 0;
    while(1) {
        keys = 0;
        for(i = 0;i < THREADCNT; i++) {
           if(db[i]) {
                keys += dictSize(db[i]);
           }
        }
        writeLog(1, "There are %d keys stored.", keys);
        usleep(2000000);
    }
}

void *acceptThread() {
    int i = 0;
    size_t size = 0;
    eventLoop *acceptEloop = createEventLoop();

/* only __this__ thread can accept */
    if (createFileEvent(acceptEloop, listenfd, READABLE, tcpAcceptCallback) == -1
        || aePollCreate(acceptEloop) == -1) return NULL;  //aePollCreate(server->eLoop)只需要在这里执行一次，以后add读写事件就不用再次Add
    if (aePollAddEvent(acceptEloop, listenfd, READABLE) == -1) {
        printf("add tcpAcceptCallback failed!\n");
        return NULL;
    }

    int j = 0, numevents = 0, processed = 0;
    struct timeval tvp;

/* main loop */
    while(1) {
        tvp.tv_sec = 0;
        tvp.tv_usec = 1;
        numevents = 0;
        numevents = aePollPoll(acceptEloop, &tvp);
        for (j = 0; j < numevents; j++) {
            fileEvent *fe = &acceptEloop->fEvents[acceptEloop->fired[j].fd];
            int mask = acceptEloop->fired[j].mask;
            int fd = acceptEloop->fired[j].fd;

            /* note the fe->mask & mask & ... code: maybe an already processed
             * event removed an element that fired and we still didn't
             * processed, so we check if the event is still valid. */
            if (fe->mask & mask & READABLE)
                fe->rfileProc(NULL, acceptEloop, fd, mask);
            processed++;
        }
    }
}

int main() {
    int i = 0, j = 0;
    pthread_t tid[THREADCNT+2];

    listenfd = tcpServer(NULL,serverport , NULL);   // init for the global variable listenfd,
                                                    // the first NULL refers to server->neterr,
    writeLog(1,"starting server...%s","server started.");
    logFile("starting server...", 0);
    for(i = 0; i < THREADCNT; i++) {
        db[i] = dictCreate(&dDictType, NULL);           //init global variable db[i]
    }
    for(i = 0;i<THREADCNT;i++) {
        globalEloop[i] = createEventLoop();
        aePollCreate(globalEloop[i]) == -1;
        globalconnTree[i] = (connTree *)malloc(sizeof(connTree));
        globalconnTree[i]->root = NULL;
        globalconnTree[i]->connCnt = 0;    //init connCnt is 0
        globalconnTree[i]->ins = rb_insert;
        globalconnTree[i]->del = rb_erase;
        globalconnTree[i]->get = rb_search;
    }

    pthread_mutex_init(&eloopidLock, NULL);
    pthread_create(&tid[THREADCNT], NULL, acceptThread, NULL);
    for(i = 0; i < THREADCNT; i++) {
        pthread_mutex_init(&lock[i], NULL);
        pthread_create(&tid[i], NULL, runServer, NULL);
    }
    pthread_mutex_init(&lock[THREADCNT], NULL);
    pthread_create(&tid[THREADCNT+1], NULL, serverCron, NULL);
    while(j < THREADCNT) {
        pthread_join(tid[j], NULL);
        j++;
    }
    pthread_join(tid[THREADCNT+1], NULL);

    pthread_join(tid[THREADCNT], NULL); //for accept thread
    return 1;
}
