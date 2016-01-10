/*
* Copyright 2015, Liyinlong.  All rights reserved.
* https://github.com/Jacklli/ActionCache
*
* Use of this source code is governed by GPL v2 license
* that can be found in the License file.
*
* Author: Liyinlong (yinlong.lee at hotmail.com)
*/

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stdlib.h>


#define   STRING   0
#define   LIST     1
#define   HASH     2
#define   SET      3

typedef struct valObject {
    void *ptr;
    unsigned char type;    //0->string, 1->list, 2->hash, 3->set 
    int refcount;
} valObject;

valObject *createObj(char *val);
int freeStringObject(valObject *o);

#endif
