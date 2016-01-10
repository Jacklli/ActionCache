/*
* I got this red-black tree data stracture codes from the Internet,
* but don't konw who wrote them, just state it here.
* If you you have any idea about the author, please email to 
* yinlong.lee @ hotmail.com 
*/

#ifndef __RBTREE_H__
#define __RBTREE_H__

typedef int key_t;
typedef enum color_t {
    RED = 0,
    BLACK = 1
} color_t;

typedef struct rb_node_t {
    struct rb_node_t *left, *right, *parent;
    key_t key;
    void *data;
    color_t color;
} rb_node_t;

rb_node_t* rb_insert(key_t key, void *data, rb_node_t* root);
rb_node_t* rb_search(key_t key, rb_node_t* root);
rb_node_t* rb_erase(key_t key, rb_node_t *root);


#endif
