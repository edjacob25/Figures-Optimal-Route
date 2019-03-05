#include <list>
#include <limits>
#include <queue>
#include <cmath>
#include <iostream>
#include <fstream>

struct point_
{
    float x;
    float y;
    struct point_ *last;
    struct point_ *next;
} point;
typedef struct point_ *point_p;

struct node_
{
    float heuristic;
    float weight;
    point_p point;
    struct node_ *parent;
} node;
typedef struct node_ *node_p;

struct edge_
{
    float m;
    float b;
    bool vertical;
    point_p limit_a;
    point_p limit_b;
} edge;
typedef struct edge_ *edge_p;

struct poligon_
{
    int sides;
    point_p start;
} poligon;
typedef struct poligon_ *poligon_p;

#define MAX numeric_limits<float>::infinity()