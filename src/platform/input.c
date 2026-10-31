#include <genesis.h>
#include "input.h"

void input_init(void)
{
    JOY_setSupport(PORT_1, JOY_SUPPORT_3BTN);
}

InputState input_update(void)
{
    const u16 buttons = JOY_readJoypad(JOY_1);
    InputState input;
    input.horizontal = ((buttons & BUTTON_RIGHT) != 0) - ((buttons & BUTTON_LEFT) != 0);
    input.vertical = ((buttons & BUTTON_DOWN) != 0) - ((buttons & BUTTON_UP) != 0);
    input.jump_held = (buttons & BUTTON_A) != 0;
    input.start_held = (buttons & BUTTON_START) != 0;
    return input;
}
