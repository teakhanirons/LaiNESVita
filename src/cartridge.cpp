#include <cstdio>
#include <unistd.h>
#include "include/apu.hpp"
#include "include/cpu.hpp"
#include "include/gui.hpp"
#include "include/mappers/mapper0.hpp"
#include "include/mappers/mapper1.hpp"
#include "include/mappers/mapper2.hpp"
#include "include/mappers/mapper3.hpp"
#include "include/mappers/mapper4.hpp"
#include "include/ppu.hpp"
#include "include/cartridge.hpp"

#include <psp2/kernel/clib.h> 
#include <psp2/io/fcntl.h> 

namespace Cartridge {


Mapper* mapper = nullptr;  // Mapper chip.

/* PRG-ROM access */
template <bool wr> u8 access(u16 addr, u8 v) {
    if (!wr) return mapper->read(addr);
    else     return mapper->write(addr, v);
}
template u8 access<0>(u16, u8); template u8 access<1>(u16, u8);

/* CHR-ROM/RAM access */
template <bool wr> u8 chr_access(u16 addr, u8 v) {
    if (!wr) return mapper->chr_read(addr);
    else     return mapper->chr_write(addr, v);
}
template u8 chr_access<0>(u16, u8); template u8 chr_access<1>(u16, u8);

void signal_scanline() { mapper->signal_scanline(); }

/* Load the ROM from a file. */
void load(std::string fileName) {
    int n = fileName.length(); 
    char char_array[n + 1]; 
    strcpy(char_array, fileName.c_str()); 
    sceClibPrintf("Loading: %s\n", char_array);

    SceUID fd = sceIoOpen(char_array, SCE_O_RDONLY, 0);
    if (fd < 0) sceClibPrintf("Loading failed: %s\n", char_array);
    else {
        int size = sceIoLseek(fd, 0, SCE_SEEK_END);
        sceIoLseek(fd, 0, SCE_SEEK_SET);
        u8* rom = new u8[size];
        int read = sceIoRead(fd, rom, size);

        sceIoClose(fd); 
        sceClibPrintf("ROM loaded with size: %d\n", size);

        int mapperNum = (rom[7] & 0xF0) | (rom[6] >> 4);
        if (loaded()) delete mapper;
        sceClibPrintf("Mapper: %i\n", mapperNum);
        switch (mapperNum) {
            case 0:  mapper = new Mapper0(rom); break;
            case 1:  mapper = new Mapper1(rom); break;
            case 2:  mapper = new Mapper2(rom); break;
            case 3:  mapper = new Mapper3(rom); break;
            case 4:  mapper = new Mapper4(rom); break;
        }

        CPU::power();
        PPU::reset();
        APU::reset();
    }
}

bool loaded() { return mapper != nullptr; }


}
