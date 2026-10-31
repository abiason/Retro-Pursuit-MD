#include "physics.h"
#include "collision.h"
enum { MAX_SPEED = 256, ACCELERATION = 32, FRICTION = 48,
       GRAVITY = 24, JUMP_SPEED = 896, MAX_FALL_SPEED = 1024, CLIMB_SPEED = 192 };
static int16_t approach(int16_t current, int16_t target, int16_t amount)
{
    if (current < target) return current + amount > target ? target : current + amount;
    if (current > target) return current - amount < target ? target : current - amount;
    return current;
}
static int16_t find_ladder(const Player *player, const LevelConfig *level, int8_t vertical)
{
    const int32_t center = player->x + PLAYER_WIDTH * POSITION_ONE / 2;
    const int32_t feet = player->y + PLAYER_HEIGHT * POSITION_ONE;
    uint16_t i;
    for (i = 0; i < level->ladder_count; ++i) {
        const Ladder *ladder = &level->ladders[i];
        const int32_t top = (int32_t)ladder->top * POSITION_ONE;
        const int32_t bottom = (int32_t)ladder->bottom * POSITION_ONE;
        if (center < (int32_t)ladder->x * POSITION_ONE ||
            center >= (int32_t)(ladder->x + ladder->width) * POSITION_ONE) continue;
        if ((vertical < 0 && feet > top && feet <= bottom) ||
            (vertical > 0 && feet >= top && feet < bottom)) return (int16_t)i;
    }
    return -1;
}
static uint16_t climb(Player *player, const LevelConfig *level, int8_t horizontal, int8_t vertical)
{
    const Ladder *ladder;
    int32_t feet;
    if (player->ladder_index >= (int16_t)level->ladder_count)
        player->ladder_index = -1;
    if (player->ladder_index < 0 && vertical)
        player->ladder_index = find_ladder(player, level, vertical);
    if (player->ladder_index < 0) return 0;
    /* Sideways exit is allowed only when the body is clear of solid floors. */
    if (horizontal && !collision_player_overlaps(player, level)) {
        player->ladder_index = -1;
        return 0;
    }
    ladder = &level->ladders[player->ladder_index];
    player->x = ((int32_t)ladder->x + (ladder->width - PLAYER_WIDTH) / 2) * POSITION_ONE;
    player->vx = 0;
    player->vy = vertical * CLIMB_SPEED;
    player->grounded = 0;
    player->y += player->vy;
    feet = player->y + PLAYER_HEIGHT * POSITION_ONE;
    /* The aligned ladder shaft is the only trigger that bypasses floor solids. */
    if (feet <= (int32_t)ladder->top * POSITION_ONE) {
        player->y = (ladder->top - PLAYER_HEIGHT) * POSITION_ONE;
        player->vy = 0;
        player->ladder_index = -1;
        player->grounded = 1;
    } else if (feet >= (int32_t)ladder->bottom * POSITION_ONE) {
        player->y = (ladder->bottom - PLAYER_HEIGHT) * POSITION_ONE;
        player->vy = 0;
        player->ladder_index = -1;
        player->grounded = 1;
    }
    return 1;
}
void physics_step_with_speed(Player *player, const LevelConfig *level, int8_t horizontal,
                             int8_t vertical, uint16_t jump_pressed, int16_t speed)
{
    const int16_t direction = (horizontal > 0) - (horizontal < 0);
    vertical = (vertical > 0) - (vertical < 0);
    if (climb(player, level, horizontal, vertical)) return;
    if (speed <= 0 || speed > MAX_SPEED) speed = MAX_SPEED;
    player->vx = approach(player->vx, direction * speed,
                          direction ? ACCELERATION : FRICTION);
    if (jump_pressed && player->grounded) {
        player->vy = -JUMP_SPEED;
        player->grounded = 0;
    }
    player->vy += GRAVITY;
    if (player->vy > MAX_FALL_SPEED) player->vy = MAX_FALL_SPEED;
    collision_move(player, level);
}

void physics_step(Player *player, const LevelConfig *level, int8_t horizontal,
                  int8_t vertical, uint16_t jump_pressed)
{
    physics_step_with_speed(player, level, horizontal, vertical, jump_pressed, MAX_SPEED);
}
