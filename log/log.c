/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void writeLog(int level, const char *fmt, ...) {
    va_list ap;
    FILE *fp = stdout;

    va_start(ap, fmt);
    if (level >= 0) {
        char *c = ".-";
        char buf[64];
        time_t now;

        now = time(NULL);
        strftime(buf,64,"%d %b %H:%M:%S",localtime(&now));
        fprintf(fp,"[ %s ] %c ",buf,c[level]);
        vfprintf(fp, fmt, ap);
        fprintf(fp,"\n");
        fflush(fp);
    }
    va_end(ap);
}

void logFile(char *msg, int size) {
    int ret = 0;
    char *name = "./server.log";
    FILE *fp = NULL;
    if(!(fp = fopen(name, "a+"))) {
        writeLog(1, "open log file error!");
        exit(-1);
    }
    ret = fwrite(msg,size > 0 ? size : strlen(msg), 1, fp);
    ret = fwrite("\n",1,1,fp);
    fflush(fp);
    fclose(fp);
}
