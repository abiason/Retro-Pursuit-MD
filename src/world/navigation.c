#include "navigation.h"
static const NavigationNode nodes[] = {
    {32, 192}, {64, 192}, {288, 192},
    {32, 144}, {64, 144}, {240, 144}, {288, 144},
    {32, 96}, {240, 96}, {288, 96}
};
static const NavigationEdge edges[] = {
    {0, 1, -1}, {1, 2, -1}, {3, 4, -1}, {4, 5, -1},
    {5, 6, -1}, {7, 8, -1}, {8, 9, -1}, {1, 4, 0}, {5, 8, 1}
};
const NavigationGraph navigation_test = {
    nodes, sizeof(nodes) / sizeof(nodes[0]), edges, sizeof(edges) / sizeof(edges[0])
};
static int32_t magnitude(int32_t value) { return value < 0 ? -value : value; }
static uint16_t valid(const NavigationGraph *g)
{
    return g && g->nodes && g->node_count && g->node_count <= NAVIGATION_MAX_NODES &&
        (!g->edge_count || g->edges);
}
int16_t navigation_nearest(const NavigationGraph *g, int16_t x, int16_t feet_y)
{
    uint16_t i;
    int16_t best = -1;
    int32_t distance = 0x7FFFFFFF;
    if (!valid(g)) return -1;
    for (i = 0; i < g->node_count; ++i) {
        int32_t candidate = magnitude((int32_t)g->nodes[i].x - x) +
                            magnitude((int32_t)g->nodes[i].feet_y - feet_y) * 4;
        if (candidate < distance) { distance = candidate; best = (int16_t)i; }
    }
    return best;
}
const NavigationEdge *navigation_edge(const NavigationGraph *g, uint16_t from, uint16_t to)
{
    uint16_t i;
    if (!valid(g) || from >= g->node_count || to >= g->node_count) return 0;
    for (i = 0; i < g->edge_count; ++i) {
        const NavigationEdge *e = &g->edges[i];
        if ((e->from == from && e->to == to) || (e->from == to && e->to == from)) return e;
    }
    return 0;
}
int16_t navigation_next_hop(const NavigationGraph *g, uint16_t from, uint16_t to)
{
    int16_t parent[NAVIGATION_MAX_NODES];
    uint16_t queue[NAVIGATION_MAX_NODES], head = 0, tail = 0, i;
    int16_t step;
    if (!valid(g) || from >= g->node_count || to >= g->node_count) return -1;
    if (from == to) return (int16_t)from;
    for (i = 0; i < g->node_count; ++i) parent[i] = -1;
    queue[tail++] = from;
    parent[from] = (int16_t)from;
    while (head < tail) {
        uint16_t node = queue[head++];
        for (i = 0; i < g->edge_count; ++i) {
            const NavigationEdge *e = &g->edges[i];
            uint16_t next;
            if (e->from == node) next = e->to;
            else if (e->to == node) next = e->from;
            else continue;
            if (next >= g->node_count || parent[next] >= 0) continue;
            parent[next] = (int16_t)node;
            queue[tail++] = next;
        }
    }
    if (parent[to] < 0) return -1;
    step = (int16_t)to;
    while (parent[step] != (int16_t)from) step = parent[step];
    return step;
}
int16_t navigation_escape_goal(const NavigationGraph *g, int16_t x, int16_t feet_y)
{
    int16_t best = -1;
    int32_t distance = -1;
    uint16_t i;
    if (!valid(g)) return -1;
    for (i = 0; i < g->node_count; ++i) {
        int32_t candidate = magnitude((int32_t)g->nodes[i].x - x) +
                            magnitude((int32_t)g->nodes[i].feet_y - feet_y) * 4;
        if (candidate > distance) { distance = candidate; best = (int16_t)i; }
    }
    return best;
}
