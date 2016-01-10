/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *makeCommand(char *command) {
    int totLen = 0,srcLen = 0;
    char head[8];
    char *fullcommand = malloc((totLen = (srcLen = strlen(command))+7+2));    //7 for head, 2 for tail!
    char *cpycommand = malloc(srcLen);
    strcpy(cpycommand, command);
    char *cursor = cpycommand;
    while(cursor && *cursor != '\0') {
        if(*cursor=='\t' || *cursor =='\r' || *cursor == ' ' || *cursor == '\b')
            *cursor = '\t';
        cursor++;
    }
    fullcommand[0] = '*';
    fullcommand[1] = '\t';
    fullcommand[2] = ((strlen(command)+7+2)/1000)%10 + '0';
    fullcommand[3] = ((strlen(command)+7+2)/100)%10 + '0';
    fullcommand[4] = ((strlen(command)+7+2)/10)%10 + '0';
    fullcommand[5] = ((strlen(command)+7+2)/1)%10 + '0';
    fullcommand[6] = '\t';
    fullcommand[7] = '\0';
    strcat(fullcommand, cpycommand);
    fullcommand[totLen - 2] = '\t';   
    fullcommand[totLen - 1] = '!';
    return fullcommand;
}
