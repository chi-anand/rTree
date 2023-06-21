#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "r_tree.h"
#include <string.h>
#include <limits.h>


NODE create_new_leaf_node()
{
    NODE new_leaf = (NODE) malloc(sizeof(struct node));
    new_leaf -> is_leaf = true;
    new_leaf -> count = 0;
    new_leaf -> parent = NULL;
    for(int i = 0; i < M; ++i)
    {
        (new_leaf -> regions)[i] = NULL;
        (new_leaf -> children)[i] = NULL;
        (new_leaf -> objects)[i]  = NULL;
    }
    return new_leaf;
} 

NODE create_new_internal_node()
{
    NODE new_internal = (NODE) malloc(sizeof(struct node));
    new_internal -> is_leaf = false;
    new_internal -> count = 0;
    new_internal -> parent = NULL;
    for(int i = 0; i < M; ++i)
    {
        (new_internal -> regions)[i] = NULL;
        (new_internal -> children)[i] = NULL;
        (new_internal -> objects)[i]  = NULL;
    }
    return new_internal;
} 

R_TREE create_new_r_tree()
{
    R_TREE new_r_tree = (R_TREE) malloc(sizeof(struct r_tree));
    new_r_tree -> height = 0;
    new_r_tree -> rect =NULL;
    new_r_tree -> root = create_new_leaf_node();
    return new_r_tree;
}


RECT create_new_rect(int min_x, int min_y, int max_x, int max_y )
{
    RECT new_rect = (RECT) malloc(sizeof(struct rectangle));
    new_rect -> min_x = min_x;
    new_rect -> min_y = min_y;
    new_rect -> max_x = max_x;
    new_rect -> max_y = max_y;
    return new_rect;
}

OBJ create_new_object(int x, int y)
{
    OBJ new_object = (OBJ) malloc(sizeof(struct object));
    new_object -> x = x;
    new_object -> y = y;
    return new_object;
}


bool is_node_empty(NODE node)
{
    return node -> count == 0;
}

bool is_node_full(NODE node)
{
    return node -> count == M;
}

void insert_object_into_node(NODE node, OBJ object, RECT rect)
{
    int i = 0;
    while((node -> objects)[i] != NULL)
        ++i;
    (node -> objects)[i] = object;
    (node -> regions)[i] = rect;
    node -> count += 1;
}

void insert_region_into_node(NODE parent_node, NODE child_node, RECT region)
{
    int i = 0;
    while((parent_node -> children)[i] != NULL)
        ++i;
    (parent_node -> regions)[i] = region;
    (parent_node -> children)[i] = child_node;
    parent_node -> count += 1;
    child_node -> parent = parent_node;
}

int area_rect(RECT rect)
{
    return (rect -> max_x - rect -> min_x) * (rect -> max_y - rect -> min_y);
}
int increase_in_area(RECT rect1, RECT rect2)
{
    int min_x = (int) fmin(rect1 -> min_x, rect2 -> min_x);
    int min_y = (int) fmin(rect1 -> min_y, rect2 -> min_y);
    int max_x = (int) fmax(rect1 -> max_x, rect2 -> max_x);
    int max_y = (int) fmax(rect1 -> max_y, rect2 -> max_y);
    return (max_x - min_x) * (max_y - min_y) -  area_rect(rect1);

}
NODE choose_leaf(NODE node, OBJ object) 
{
    if(node -> is_leaf)
        return node;
    int min_enlargement = INT_MAX;
    RECT obj_rect = create_new_rect(object -> x, object -> y, object -> x, object -> y);
    int index = -1;
    for(int i = 0; i < node -> count; ++i )
    {
        if(increase_in_area((node -> regions)[i], obj_rect) < min_enlargement)
        {
            min_enlargement = increase_in_area((node -> regions)[i], obj_rect);
            index = i;
        }
        else if(increase_in_area((node -> regions)[i], obj_rect) == min_enlargement)
        {
            index = area_rect((node -> regions)[index]) < area_rect((node -> regions)[i]) ? index : i;
        }
    }
    free(obj_rect);
    return choose_leaf((node -> children)[index], object);
}

RECT bounding_box(NODE node)
{
    int i = 0;
    int min_x = INT_MAX;
    int min_y = INT_MAX;
    int max_x = INT_MIN;
    int max_y = INT_MIN;
    while(i < M && (node -> regions)[i] != NULL)
    {
        min_x = (int)fmin(min_x, (node -> regions)[i] -> min_x);
        min_y = (int)fmin(min_y, (node -> regions)[i] -> min_y);
        max_x = (int)fmax(max_x, (node -> regions)[i] -> max_x);
        max_y = (int)fmax(max_y, (node -> regions)[i] -> max_y);
        ++i;
    }
    return create_new_rect(min_x,min_y,max_x,max_y);
}


int * pick_seeds(NODE node, RECT rect)
{
    int max_d = INT_MIN;
    int index1 = -1;
    int index2 = -1;
    for(int i = 0; i < M; ++i)
    {
        int d = increase_in_area((node -> regions)[i], rect) - area_rect(rect);
        if(d > max_d)
        {
            max_d = d;
            index1 = i;
            index2 = M;
        }
    }
    for(int i = 0; i < M; ++i)
    {
        for(int j = i + 1; j < M; ++j)
        {
            int d = increase_in_area((node -> regions)[i],(node -> regions)[j] ) - area_rect((node -> regions)[j]);
            if(d > max_d)
            {
                max_d = d;
                index1 = i;
                index2 = j;
            }
        }
    }
    int* pair = (int *)malloc(sizeof(int) * 2);
    pair[0] = index1;
    pair[1] = index2;
    return pair;
}
bool search_in_node(NODE node, RECT rect)
{
    int i = 0;
    while(i < M && (node -> regions)[i] != NULL)
    {
        if((node -> regions)[i] == rect)
            return true;
        ++i;
    }
    return false;
}
int pick_next(NODE node1, NODE node2, NODE node, RECT rect)
{
    int * indexes = (int *) malloc(sizeof(int) * (M - 1));
    int count = 0;
    for(int i = 0; i < M; ++i)
    {
        if(!search_in_node(node1,(node -> regions)[i]) && !search_in_node(node2, (node -> regions)[i]))
            indexes[count++] = i;
    }
    if(!search_in_node(node1,rect) && !search_in_node(node2, rect))
            indexes[count++] = M;
    if(count == 0)
        return -1;
    int max_d = INT_MIN;
    int final_index = -1;
    RECT bound_node1 = bounding_box(node1);
    RECT bound_node2 = bounding_box(node2);
    for(int i = 0; i < count; ++i )
    {
        if(indexes[i] == M)
        {
            int d = (int)fabs(increase_in_area(bound_node1,rect) - increase_in_area(bound_node2,rect));
            if(max_d < d)
            {
                max_d = d;
                final_index = M;
               
            } 
        }
        else
        {
            int d = (int)fabs(increase_in_area(bound_node1,(node -> regions)[indexes[i]]) - increase_in_area(bound_node2,(node -> regions)[indexes[i]]));
            if(max_d < d)
            {
                max_d = d;
                final_index = indexes[i];
            } 
        }
        
    }
    free(bound_node1);
    free(bound_node2);
    return final_index;
}
NODE * quadratic_split_leaf_node(NODE node, OBJ object)
{
    NODE node1 = create_new_leaf_node();
    NODE node2 = create_new_leaf_node();
    RECT obj_rect = create_new_rect(object -> x, object -> y, object -> x, object -> y);
    int * pair = pick_seeds(node, obj_rect);
    insert_object_into_node(node1, (node -> objects)[pair[0]], (node -> regions)[pair[0]]);
    if(pair[1] == M)
        insert_object_into_node(node2, object, obj_rect);
    else
        insert_object_into_node(node2, (node -> objects)[pair[1]],(node -> regions)[pair[1]]);
    
    int index = -1;
    while(pick_next(node1,node2,node,obj_rect) != -1)
    {
        index = pick_next(node1,node2,node,obj_rect);
        NODE final_node;
        if(M + 1 - node2 -> count == m)
            final_node = node1;
        else if(M + 1 - node1 -> count == m)
            final_node = node2;
        else 
        {
            int d1,d2;
            RECT bound_node1 = bounding_box(node1);
            RECT bound_node2 = bounding_box(node2);
            if(index != M)
            {
                d1 = increase_in_area(bound_node1,(node -> regions)[index]);
                d2 = increase_in_area(bound_node2,(node -> regions)[index]);
            }
            else
            {
                d1 = increase_in_area(bound_node1,obj_rect);
                d2 = increase_in_area(bound_node2,obj_rect);
            }
            
            if(d1 != d2)
                final_node = d1 < d2 ? node1 : node2;
            else 
            {
                int area1 = area_rect(bound_node1);
                int area2 = area_rect(bound_node2);
                if(area1 != area2)
                    final_node = area1 < area2 ? node1 : node2;
                else
                    final_node = node1 -> count <= node2 -> count ? node1 : node2;
            }
            free(bound_node1);
            free(bound_node2);
        }
        if(index != M)
            insert_object_into_node(final_node, (node -> objects)[index], (node -> regions)[index]);
        else 
            insert_object_into_node(final_node, object, obj_rect);
    }
    NODE * splitted_nodes = (NODE *)malloc(sizeof(NODE) * 2);
    splitted_nodes[0] = node1;
    splitted_nodes[1] = node2;
    return splitted_nodes;
}
void adjust_tree(R_TREE r_tree, NODE node1, NODE node2, NODE node)
{
    if(node == r_tree -> root)
        if(node1 == NULL && node2 == NULL)
        {
            free(r_tree -> rect);
            r_tree -> rect = bounding_box(node);
        }   
        else
        {
            NODE new_root = create_new_internal_node();
            insert_region_into_node(new_root, node1, bounding_box(node1));
            insert_region_into_node(new_root, node2, bounding_box(node2));
            r_tree -> height  = r_tree -> height + 1;
            r_tree -> root = new_root;
            free(r_tree -> rect);
            r_tree -> rect = bounding_box(new_root);
            free(node);
        }
    else
    {
        NODE parent = node -> parent;
        int i = 0;
        while(i < M && (parent -> children)[i] != NULL)
        {
            if((parent -> children)[i] == node)
                break;
            ++i;
        }
        if(node1 == NULL && node2 == NULL)
        {
            free((parent -> regions)[i]);
            (parent -> regions)[i] = bounding_box(node);
            adjust_tree(r_tree, NULL, NULL, parent);
        }
        else
        {
            (parent -> children)[i] = node1;
            node1 -> parent = parent;
            free(node);
            free((parent -> regions)[i]);
            (parent -> regions)[i] = bounding_box(node1);
            if(parent -> count == M)
            {
                NODE * nodes = quadratic_split_internal_node(parent, bounding_box(node2), node2);
                adjust_tree(r_tree, nodes[0],nodes[1], parent);
            }
            else
            {
                insert_region_into_node(parent,node2,bounding_box(node2));
                adjust_tree(r_tree, NULL, NULL, parent);
            }
        }
    }

}

NODE * quadratic_split_internal_node(NODE node, RECT rect, NODE child)
{
    NODE node1 = create_new_internal_node();
    NODE node2 = create_new_internal_node();
    int * pair = pick_seeds(node, rect);
    insert_region_into_node(node1, (node -> children)[pair[0]], (node -> regions)[pair[0]]);
    if(pair[1] == M)
        insert_region_into_node(node2, child, rect);
    else
        insert_region_into_node(node2, (node -> children)[pair[1]],(node -> regions)[pair[1]]);
    
    int index = -1;
    while(pick_next(node1,node2,node, rect) != -1)
    {
        index = pick_next(node1,node2,node,rect);
        NODE final_node;
        if(M + 1 - node2 -> count == m)
            final_node = node1;
        else if(M + 1 - node1 -> count == m)
            final_node = node2;
        else 
        {
            int d1,d2;
            RECT bound_node1 = bounding_box(node1);
            RECT bound_node2 = bounding_box(node2);
            if(index != M)
            {
                d1 = increase_in_area(bound_node1,(node -> regions)[index]);
                d2 = increase_in_area(bound_node2,(node -> regions)[index]);
            }
            else
            {
                d1 = increase_in_area(bound_node1, rect);
                d2 = increase_in_area(bound_node2, rect);
            }
            if(d1 != d2)
                final_node = d1 < d2 ? node1 : node2;
            else 
            {
                int area1 = area_rect(bound_node1);
                int area2 = area_rect(bound_node2);
                if(area1 != area2)
                    final_node = area1 < area2 ? node1 : node2;
                else
                    final_node = node1 -> count <= node2 -> count ? node1 : node2;
            }
            free(bound_node1);
            free(bound_node2);
        }
        if(index != M)
            insert_region_into_node(final_node, (node -> children)[index], (node -> regions)[index]);
        else 
            insert_region_into_node(final_node, child, rect);
        
    }
    NODE * splitted_nodes = (NODE *)malloc(sizeof(NODE) * 2);
    splitted_nodes[0] = node1;
    splitted_nodes[1] = node2;
    return splitted_nodes;
}

void print_node(NODE node)
{
    if(node == NULL)return;
    int i = 0;
    while(i < M && (node -> regions)[i] != NULL)
    {
        printf("[%d %d %d %d] ", (node -> regions)[i] -> min_x, (node -> regions)[i] -> min_y, (node -> regions)[i] -> max_x, (node -> regions)[i] -> max_y);
        ++i;
    }
    printf("\n");
}
void insert_in_r_tree(R_TREE r_tree, OBJ object)
{
    NODE node = choose_leaf(r_tree -> root, object);
    if(node -> count == M)
    {
        NODE * nodes = quadratic_split_leaf_node(node,object);
        adjust_tree(r_tree, nodes[0],nodes[1],node);

    }
    else
    {
        insert_object_into_node(node, object, create_new_rect(object -> x, object -> y, object -> x, object -> y));
        adjust_tree(r_tree, NULL, NULL, node);
    }
}
void pre_order_traversal(NODE node, int depth) 
{
    if (node == NULL) 
        return;
    for(int i =0; i < depth; ++i )
        printf("  ");
    // Check if the node is a leaf node or an internal node
    if (node -> is_leaf) 
    {
        printf("Leaf Node: ");
        int i =0;
        while(i < M && (node -> regions)[i] != NULL)
        {
            printf("[(%d, %d)]", node->objects[i]->x, node->objects[i]->y);
            if (i < node -> count - 1) 
                printf(", ");
            ++i;
        }
        printf("\n");
    } 
    else 
    {
        printf("Internal Node: ");
        int i = 0;
        while(i < M && (node -> regions)[i] != NULL)
        {
            printf("[(%d, %d), (%d, %d)]", node->regions[i]->min_x, node->regions[i]->min_y, node->regions[i]->max_x, node->regions[i]->max_y);
            if (i < node -> count - 1) 
                printf(", ");
            ++i;
        }
        printf("\n");
        i = 0;
        while(i < M && (node -> regions)[i] != NULL)
        {
            pre_order_traversal(node -> children[i], depth + 1);
            ++i;
        }
            
    }
}
int main(int argc, char* argv[])
{
    FILE *fp;
    fp = fopen(argv[1], "r");
    if(fp == NULL)
    {
        printf("Error opening file");
        exit(1);
    }
    R_TREE r_tree = create_new_r_tree();
    char *line = malloc(100); 
    while(fgets(line, 100, fp) != NULL)
    {
        char *token;
        token = strtok(line, " ");
        int x = atoi(token);
        token = strtok(NULL, " ");
        int y = atoi(token);
        insert_in_r_tree(r_tree, create_new_object(x,y));
        if(r_tree -> height == 0)
            r_tree -> height += 1;
    }
    pre_order_traversal(r_tree -> root, 0);
    fclose(fp);
    return 0;
}