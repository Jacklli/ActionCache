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
#include <stdlib.h>
#include "object.h"

valObject *createObj(char *val) {
    int size = 0;
    valObject *obj = NULL;
    int objsize = 0; 
    objsize = sizeof(*obj);
    obj = (valObject *)malloc(objsize);
    size = strlen(val);
    memset(obj, '\0', size);
    obj->ptr = malloc(size);
    memset(obj->ptr, '\0', size);
    strcpy(obj->ptr, val);
    obj->type = 0;
    obj->refcount = 0;
    return obj;
}
int freeStringObject(valObject *o) {
    free(o->ptr);
    o->ptr = NULL;
}
