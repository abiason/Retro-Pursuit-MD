#ifndef RP_NAVIGATION_H
#define RP_NAVIGATION_H
#include "level.h"
enum { NAVIGATION_MAX_NODES = 16 };
typedef struct { int16_t x, feet_y; } NavigationNode;
typedef struct { uint16_t from, to; int16_t ladder_index; } NavigationEdge;
typedef struct {
    const NavigationNode *nodes;
    uint16_t node_count;
    const NavigationEdge *edges;
    uint16_t edge_count;
} NavigationGraph;
extern const NavigationGraph navigation_test;
int16_t navigation_nearest(const NavigationGraph *graph, int16_t x, int16_t feet_y);
int16_t navigation_next_hop(const NavigationGraph *graph, uint16_t from, uint16_t to);
const NavigationEdge *navigation_edge(const NavigationGraph *graph, uint16_t from, uint16_t to);
int16_t navigation_escape_goal(const NavigationGraph *graph, int16_t player_x, int16_t player_feet);
#endif
