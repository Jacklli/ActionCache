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
#include "rbtree.h"
#include "conn.h"

rb_node_t *func(int fd, conn *connection, rb_node_t *root) {
    return root;
}

int main()  
{  
    int i, count = 100;  
    key_t key;  
    rb_node_t* root = NULL, *node = NULL;  
      
    srand(time(NULL));  
    for (i = 1; i < count; ++i) {
        key = rand() % count;  
        if ((root = rb_insert(key, func, root))) {
            printf("[i = %d] insert key %d success!\n", i, key);  
        }  
        else {
            printf("[i = %d] insert key %d error!\n", i, key);  
            exit(-1);  
        }  
        if ((node = rb_search(key, root))) {
            printf("[i = %d] search key %d success!\n", i, key);  
        }  
        else { 
            printf("[i = %d] search key %d error!\n", i, key);  
            exit(-1);  
        }  
        if (!(i % 10)) { 
            if ((root = rb_erase(key, root))) {
                printf("[i = %d] erase key %d success\n", i, key);  
            }  
            else { 
                printf("[i = %d] erase key %d error\n", i, key);  
            }  
        }  
    }  
    return 0;  
}  
