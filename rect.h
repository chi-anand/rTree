#ifndef RECT_H
#define RECT_H

struct rectangle
{
    int min_x;
    int min_y;
    int max_x;
    int max_y;
};
typedef struct rectangle* RECT;

#endif