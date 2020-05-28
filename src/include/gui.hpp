#pragma once
#include <string>
#include <Nes_Apu.h>
#include "common.hpp"

namespace GUI {

// Screen size:
const unsigned WIDTH  = 256;
const unsigned HEIGHT = 240;

void init();
void toggle_pause();
void run();

u8 get_joypad_state(int n);
void new_frame(u32* pixels);
void new_samples(const blip_sample_t* samples, size_t count);

}
