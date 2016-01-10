/*
* I got this red-black tree data stracture codes from the Internet,
* but don't konw who wrote them, just state it here.
* If you you have any idea about the author, please email to 
* yinlong.lee @ hotmail.com 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rbtree.h"


static rb_node_t* rb_rotate_left(rb_node_t* node, rb_node_t* root) {
    rb_node_t* right = node->right;
    if ((node->right = right->left)) {
        right->left->parent = node;
    }  
    right->left = node;
    if ((right->parent = node->parent)) {
        if(node == node->parent->right) {
            node->parent->right = right;  
        }  
        else {
            node->parent->left = right;  
        }  
    }  
    else {
        root = right;  
    }  
    node->parent = right;
    return root;  
}  

static rb_node_t* rb_rotate_right(rb_node_t* node, rb_node_t* root) { 
    rb_node_t* left = node->left;  
    if ((node->left = left->right)) {
        left->right->parent = node;  
    }  
    left->right = node;  
    if ((left->parent = node->parent)) {
        if (node == node->parent->right) {
            node->parent->right = left;  
        }  
        else {
            node->parent->left = left;  
        }  
    }  
    else { 
        root = left;  
    }  
    node->parent = left;  
    return root;  
}  

static rb_node_t* rb_insert_rebalance(rb_node_t *node, rb_node_t *root) {
    rb_node_t *parent, *gparent, *uncle, *tmp;
    while ((parent = node->parent) && parent->color == RED) {
        gparent = parent->parent;
        if (parent == gparent->left) {
            uncle = gparent->right;
            if (uncle && uncle->color == RED) {
                uncle->color = BLACK;
                parent->color = BLACK;
                gparent->color = RED;    
                node = gparent;
            }  
            else {
                if (parent->right == node) {
                    root = rb_rotate_left(parent, root);
                    tmp = parent;  
                    parent = node;  
                    node = tmp;
                }  
                parent->color = BLACK;
                gparent->color = RED;
                root = rb_rotate_right(gparent, root);
            }  
        }   
        else {   
            uncle = gparent->left;
            if (uncle && uncle->color == RED) {
                uncle->color = BLACK;  
                parent->color = BLACK;  
                gparent->color = RED;  
                node = gparent;
            }  
            else {
                if (parent->left == node) {
                    root = rb_rotate_right(parent, root);
                    tmp = parent;  
                    parent = node;  
                    node = tmp;
                }   
                parent->color = BLACK;  
                gparent->color = RED;  
                root = rb_rotate_left(gparent, root);
            }  
        }  
    }  
   
    root->color = BLACK;
    return root;
}  

static rb_node_t* rb_search_auxiliary(key_t key, rb_node_t* root, rb_node_t** save)  
{  
    rb_node_t *node = root, *parent = NULL;  
    int ret;  
    while (node) {
        parent = node;  
        ret = node->key - key;  
        if (0 < ret) {
            node = node->left;  
        }  
        else if (0 > ret) {
            node = node->right;  
        }  
        else { 
            return node;  
        }  
    }  
   
    if (save) {
        *save = parent;  
    }  
    return NULL;  
}  

static rb_node_t* rb_new_node(key_t key, void *data) {
    rb_node_t *node = (rb_node_t*)malloc(sizeof(struct rb_node_t));
    if (!node) {
        printf("malloc error!\n");
        exit(-1);
    }
    memset(node, '\0', sizeof(struct rb_node_t));
    node->key = key, node->data = data;
    return node;
}

rb_node_t* rb_insert(key_t key, void *data, rb_node_t* root)  
{  
    rb_node_t *parent = NULL, *node;  
    parent = NULL;  
    if ((node = rb_search_auxiliary(key, root, &parent))) {
        return root;  
    }  
    node = rb_new_node(key, data);
    node->parent = parent;     
    node->left = node->right = NULL;  
    node->color = RED;  
    if (parent) {
        if (parent->key > key) { 
            parent->left = node;  
        }  
        else { 
            parent->right = node;  
        }  
    }  
    else { 
        root = node;  
    }  
    return rb_insert_rebalance(node, root);
}

rb_node_t* rb_search(key_t key, rb_node_t* root) {
    return rb_search_auxiliary(key, root, NULL);  
}  

static rb_node_t* rb_erase_rebalance(rb_node_t *node, rb_node_t *parent, rb_node_t *root)  
{  
    rb_node_t *other, *o_left, *o_right;
    while ((!node || node->color == BLACK) && node != root) {
        if (parent->left == node) {
            other = parent->right;  
            if (other->color == RED) {
                other->color = BLACK;    
                parent->color = RED;
                root = rb_rotate_left(parent, root);
                other = parent->right;
            }  
            if ((!other->left || other->left->color == BLACK) &&  
                (!other->right || other->right->color == BLACK)) {
                other->color = RED;
                node = parent;
                parent = node->parent;
            }  
            else {
                if (!other->right || other->right->color == BLACK) { 
                    if ((o_left = other->left)) {
                        o_left->color = BLACK;
                    }   
                    other->color = RED;
                    root = rb_rotate_right(other, root);
                    other = parent->right;
  
                }  
                other->color = parent->color;
                parent->color = BLACK;
                if (other->right) {
                    other->right->color = BLACK;
                }  
                root = rb_rotate_left(parent, root);
                node = root;
                break;  
            }  
        }  
        else {
            other = parent->left;  
            if (other->color == RED) {
                other->color = BLACK;  
                parent->color = RED;  
                root = rb_rotate_right(parent, root);  
                other = parent->left;  
            }  
            if ((!other->left || other->left->color == BLACK) &&  
                (!other->right || other->right->color == BLACK)) { 
                other->color = RED;  
                node = parent;  
                parent = node->parent;  
            }  
            else { 
                if (!other->left || other->left->color == BLACK) {
                    if ((o_right = other->right)) {
                        o_right->color = BLACK;  
                    }  
                    other->color = RED;  
                    root = rb_rotate_left(other, root);  
                    other = parent->left;  
                }  
                other->color = parent->color;  
                parent->color = BLACK;  
                if (other->left) { 
                    other->left->color = BLACK;  
                }  
                root = rb_rotate_right(parent, root);  
                node = root;  
                break;  
            }  
        }  
    }  
   
    if (node) {
        node->color = BLACK;
    }    
    return root;
}  

rb_node_t* rb_erase(key_t key, rb_node_t *root) { 
    rb_node_t *child, *parent, *old, *left, *node;  
    color_t color;  
    if (!(node = rb_search_auxiliary(key, root, NULL))) {
        printf("key %d is not exist!\n",key);  
        return root;  
    }  
    old = node;  
    if (node->left && node->right) {
        node = node->right;  
        while ((left = node->left) != NULL) { 
            node = left;  
        }  
        child = node->right;  
        parent = node->parent;  
        color = node->color;  
        if (child) { 
            child->parent = parent;  
        }  
        if (parent) { 
            if (parent->left == node) { 
                parent->left = child;  
            }  
            else { 
                parent->right = child;  
            }  
        }  
        else { 
            root = child;  
        }  
    
        if (node->parent == old) { 
            parent = node;  
        }  
        node->parent = old->parent;  
        node->color = old->color;  
        node->right = old->right;  
        node->left = old->left;  
        if (old->parent) { 
            if (old->parent->left == old) { 
                old->parent->left = node;  
            }  
            else { 
                old->parent->right = node;  
            }  
        }   
        else { 
            root = node;  
        }  
    
        old->left->parent = node;  
        if (old->right) { 
            old->right->parent = node;  
        }  
    }  
    else { 
        if (!node->left) { 
            child = node->right;  
        }  
        else if (!node->right) { 
            child = node->left;  
        }  
        parent = node->parent;  
        color = node->color;  
        if (child) { 
            child->parent = parent;  
        }  
        if (parent) { 
            if (parent->left == node) { 
                parent->left = child;  
            }  
            else { 
                parent->right = child;  
            }  
        }  
        else { 
            root = child;  
        }  
    }  
   
    free(old);  
    if (color == BLACK) { 
        root = rb_erase_rebalance(child, parent, root);
    }  
    return root;  
}  

