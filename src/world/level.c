#include "level.h"
static const WorldRect solids[] = {
    {8, 48, 8, 152}, {304, 48, 8, 152}, {8, 40, 304, 8},
    {16, 192, 288, 8}, {16, 144, 288, 8}, {16, 96, 288, 8},
    {144, 184, 16, 8}, {128, 136, 16, 8}
};
static const Ladder ladders[] = { {64, 16, 144, 192}, {240, 16, 96, 144} };
const LevelConfig level_test = {
    solids, sizeof(solids) / sizeof(solids[0]),
    ladders, sizeof(ladders) / sizeof(ladders[0]), 32, 168
};
