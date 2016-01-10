/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>  
#include <string.h>  
#include <netdb.h>  
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>  
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <time.h>
#include "makecommand.h"

#define SERVPORT 33060
#define SERVIP   "127.0.0.1"
#define MAXDATASIZE 100
#define THREADCNT 10

typedef struct args {
    int arg1;//参数1 
    int arg2;//参数2 
} args;


void *doTest(void *arg) {
    int i = 0, nread = 0;
    int connfd = 0;
    int loopcnt = 0;
    int flags = 0;

char *errmsg = NULL;
    char command_set[50] = "";
    char command_set_key[20] = "";
    char command_set_val[20] = "";
    char readBuf[20];
    char *ptr = NULL;
    struct hostent *host;
    struct sockaddr_in serv_addr;
    host = gethostbyname(SERVIP);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVPORT);
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr.sin_zero), 8);
    if((connfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        printf("socket创建出错！\n");
    }
    struct timeval tv_out;
    tv_out.tv_sec = 1;
    tv_out.tv_usec = 0;
    int ret = -2;
    if((ret = setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out)))==-1) {
        printf("set timeout failed.\n");
    }
    if (connect(connfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) != 0) { 
        printf("connect出错！\n");
        errmsg = strerror(errno);
        printf("Mesg:%s\n",errmsg); //可以看到返回的错误码
    } 
//printf("in thread %lu, arg1 is:%d, arg2 is:%d\n", pthread_self(), ((args *)arg)->arg1, ((args *)arg)->arg2);
    for(loopcnt = ((args *)arg)->arg1; loopcnt < ((args *)arg)->arg2; loopcnt++) {
        strcpy(command_set,"set\ta");
        sprintf(command_set_key,"%d",loopcnt);   //fill key with loopcnt
        strcat(command_set,command_set_key);     //combine commend_get and command_get_key
        strcat(command_set,"\t");
        sprintf(command_set_val,"%d",loopcnt);
        strcat(command_set,command_set_val);
        ptr = makeCommand(command_set);
//        printf("%s\n", ptr);
        write(connfd, ptr, strlen(ptr));
        memset(readBuf,'\0',20);
        read(connfd,readBuf,20);
    }
    close(connfd);
}
int main() {
    int i = 0, j = 0, m = 0;
    pthread_t tid[10];
    struct timeval tstart,tend;
    float timeuse;
    gettimeofday(&tstart,NULL);

printf("\n\n");
printf("start set test with 10 threads.\n");

    args *myarg = NULL;
    for(m = 0; m < 10; m++) {
        myarg = malloc(sizeof(*myarg));
        myarg->arg1 = m*1000000;
        myarg->arg2 = (m+1)*1000000;
//        printf("myarg->arg1 is:%d\n",myarg->arg1);
//        printf("myarg->arg2 is:%d\n",myarg->arg2);
        pthread_create(&tid[m], NULL, doTest, (void *)myarg);
    }
    for(i = 0;i<10;i++) {
        pthread_join(tid[i], NULL);
    }
    gettimeofday(&tend,NULL);
    timeuse=1000000*(tend.tv_sec-tstart.tv_sec)+(tend.tv_usec-tstart.tv_usec);
    timeuse/=1000000;
    printf("15000000 keys get finished in %f seconds\n",timeuse);
    printf("########  The test QPS for get is:%d  #########\n\n\n",(int)(10000000/timeuse));
    return 1;
}
