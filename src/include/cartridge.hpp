#pragma once
#include "common.hpp"
#include <string>

namespace Cartridge {
 
template <bool wr> u8     access(u16 addr, u8 v = 0);
template <bool wr> u8 chr_access(u16 addr, u8 v = 0);
void signal_scanline();
void load(std::string fileName);
bool loaded();


}
