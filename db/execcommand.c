/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "object.h"
#include "dict.h"
#include "execcommand.h"
#include "server.h"
#include "log.h"

extern dict *db[THREADCNT];
extern pthread_mutex_t lock[THREADCNT];

int chrtoint(char *str) {
    int size = 0;
    unsigned int intval = 0;
    int i = 0;
    size = strlen(str);
    for(i = 0; i<size; i++) {
        intval += *(str+i)-'0';
    }
    return intval;
}

static int setCommand(char *key, valObject *val) {
    int ret = -1,dbindex = -1;
    dbindex = chrtoint(key)%(THREADCNT);
    pthread_mutex_lock(&lock[dbindex]);
    ret = dictAdd(db[dbindex], key, val);
    pthread_mutex_unlock(&lock[dbindex]);
    if(ret < 0)
        return ret;
    else
        return -1;
}
static valObject *lookupKey(dict *db, char *key) {
    dictEntry *de = NULL;
    de = dictFind(db,key);
    if(de)
        return de->val;
    else
        return NULL;
}

int execSetCommand(char (*argv)[ARGUMENTCNT]) {
    valObject *val = createObj(argv[2]);
    return setCommand(argv[1], val);
}
valObject *execGetCommand(char (*argv)[ARGUMENTCNT]) {
    int dbindex = -1;
    valObject *val;
    dbindex = chrtoint(argv[1])%(THREADCNT);
    val = lookupKey(db[dbindex], argv[1]);
    return val;
}

void execShutdownCommand(dict *db) {
    writeLog(1, "start to clear db...");
    dictRelease(db);
    writeLog(1, "db cleared...");

/*
*  TODO
*  Implement all connections free here.
*/
    sleep(5);
    writeLog(1, "start to shutdown...");
    exit(1);
}
