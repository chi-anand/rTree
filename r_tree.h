#ifndef R_TREE_H
#define R_TREE_H
#include "object.h"
#include "rect.h"
#include "stdbool.h"
#define m 2
#define M 4
struct node
{
    bool is_leaf;
    int count;
    struct node * parent;
    RECT regions[M];
    struct node * children[M];
    OBJ objects[M];
    
};
typedef struct node * NODE;

struct r_tree
{
    int height;
    RECT rect;
    NODE root;
};
typedef struct r_tree * R_TREE;

NODE create_new_leaf_node();
NODE create_new_internal_node();
R_TREE create_new_r_tree();
RECT create_new_rect(int min_x, int min_y, int max_x, int max_y );
OBJ create_new_object(int x, int y);
bool is_node_empty(NODE node);
bool is_node_full(NODE node);
void insert_object_into_node(NODE node, OBJ object, RECT rect);
void insert_region_into_node(NODE node, NODE child, RECT region);
NODE * quadratic_split_internal_node(NODE node, RECT rect, NODE child);

#endif