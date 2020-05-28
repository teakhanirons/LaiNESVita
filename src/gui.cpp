#include <csignal>
#include "include/apu.hpp"
#include "include/cartridge.hpp"
#include "include/cpu.hpp"
#include "include/menu.hpp"
#include "include/gui.hpp"
#include "include/joypad.hpp"

#include <psp2/kernel/clib.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/io/fcntl.h> 
#include <psp2/ctrl.h> 
#include <psp2/display.h>

#define UP 1
#define DOWN 2
#define SELECT 3

#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

#define FB_WIDTH 960
#define FB_HEIGHT 544
#define FB_LEN (ALIGN(FB_WIDTH, 64) * FB_HEIGHT * 4)
#define FB_PITCH ALIGN(FB_WIDTH, 64)

namespace GUI {

SceCtrlData ctrl_peek, ctrl_press;
void *fb_addr = NULL;

Menu* menu;
FileMenu* fileMenu;

bool pause = true;

void init() {

    APU::init();

    fileMenu = new FileMenu;
    menu = fileMenu;
}

void control_check(int n) {
	ctrl_press = ctrl_peek;
	sceCtrlPeekBufferPositive(n, &ctrl_peek, 1);
	ctrl_press.buttons = ctrl_peek.buttons & ~ctrl_press.buttons;
}

uint8_t get_joypad_state(int n) {

    u8 j = 0;
    int pad = 0;
    if(n == 0) pad = 0;
    else if(n == 1) pad = 2;

    control_check(pad);
    int a, b, select, start, up, down, left, right = 0;

    if(ctrl_press.buttons & SCE_CTRL_CROSS || ctrl_press.buttons & SCE_CTRL_CIRCLE)             a = 1;
    if(ctrl_press.buttons & SCE_CTRL_SQUARE || ctrl_press.buttons & SCE_CTRL_TRIANGLE)          b = 1;
    if(ctrl_press.buttons & SCE_CTRL_SELECT)                                                    select = 1;
    if(ctrl_press.buttons & SCE_CTRL_START)                                                     start = 1;
    if(ctrl_press.buttons & SCE_CTRL_UP)                                                        up = 1;
    if(ctrl_press.buttons & SCE_CTRL_DOWN)                                                      down = 1;
    if(ctrl_press.buttons & SCE_CTRL_LEFT)                                                      left = 1;
    if(ctrl_press.buttons & SCE_CTRL_RIGHT)                                                     right = 1;

    j |= (a)        << 0;  // A.
    j |= (b)        << 1;  // B.
    j |= (select)   << 2;  // Select.
    j |= (start)    << 3;  // Start.
    j |= (up)       << 4;  // Up.
    j |= (down)     << 5;  // Down.
    j |= (left)     << 6;  // Left.
    j |= (right)    << 7;  // Right.

    return j;
}

int widthstart = FB_WIDTH / 2 - WIDTH;
int heightstart = FB_HEIGHT / 2 - HEIGHT;
void new_frame(u32* pixels) {
    for (int i = 0; i < HEIGHT * 2; i = i + 2) {
        for (int j = 0; j < WIDTH * 2; j = j + 2) {
            menu->fb_base[((i + heightstart) * FB_PITCH) + j + widthstart] = pixels[(i / 2 * WIDTH) + j / 2];
            menu->fb_base[((i + 1 + heightstart) * FB_PITCH) + j + widthstart] = pixels[(i / 2 * WIDTH) + j / 2];
            menu->fb_base[((i + heightstart) * FB_PITCH) + j + 1 + widthstart] = pixels[(i / 2 * WIDTH) + j / 2];
            menu->fb_base[((i + 1 + heightstart) * FB_PITCH) + j + 1 + widthstart] = pixels[(i / 2 * WIDTH) + j / 2];
        }
    }
}

void toggle_pause() { pause = false; }

/* Run the emulator */
void run() {

    const int FPS   = 60;
    const int DELAY = 1000.0f / FPS;

    while (true) {
             
        if(pause) {
			control_check(0);
			if(ctrl_press.buttons & SCE_CTRL_UP) menu->update(UP);
			if(ctrl_press.buttons & SCE_CTRL_DOWN) menu->update(DOWN);
			if(ctrl_press.buttons & SCE_CTRL_CROSS || ctrl_press.buttons & SCE_CTRL_CIRCLE || ctrl_press.buttons & SCE_CTRL_START) menu->update(SELECT);
        } else CPU::run_frame();

    }
}


}
