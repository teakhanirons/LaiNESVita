#include <algorithm>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include "include/cartridge.hpp"
#include "include/menu.hpp"
#include "include/gui.hpp"

#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/io/fcntl.h>
#include <psp2/kernel/clib.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/sysmem.h>
#include <fnblit.h>

#define UP 1
#define DOWN 2
#define SELECT 3

#define WHITE 0xFFFFFFFF
#define BLACK 0x00000000
#define RED   0xFF0000FF
#define BLUE  0xFFFF0000

#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

#define FB_WIDTH 960
#define FB_HEIGHT 544
#define FB_LEN (ALIGN(FB_WIDTH, 64) * FB_HEIGHT * 4)
#define FB_PITCH ALIGN(FB_WIDTH, 64)

namespace GUI {

using namespace std;
int cursor = 0;
int length = 0;

struct SceIoDirenter {
     char d_name[256];  
     struct SceIoDirenter * next;   
 };

SceIoDirenter * head = NULL;

char * getItem(int number) {
  SceIoDirenter * currenter = head;
  for (int i = 0; i < number - 1; i++) currenter = currenter->next;
  return currenter->d_name;
}

void Menu::update(int a) {
    if (a == DOWN) {
        cursor++;
        if (cursor >= length) cursor = 0;
    } else if (a == UP) {
        cursor--;
        if (cursor < 0) cursor = length - 1;
        if (cursor < 0) cursor = 0;
    }
    render();
    if(length > 0) {
        if (a == SELECT) { 
            char path[256] = "ux0:data/LaiNES/\0";
            strcat(path, getItem(cursor + 1));
            sceClibPrintf("Loading from path: %s\n", path);
            Cartridge::load(path); 
            GUI::toggle_pause();
            memset(fb_base, 0x00, FB_LEN);
        }
    }
}

void Menu::render() {
        for (int i = 0; i < FB_WIDTH; i++) {
            for (int j = 0; j < length; j++) {
                fb_base[(j + 2) * 14 * FB_PITCH + i] = BLACK;
                fb_base[(j + 3) * 14 * FB_PITCH + i] = BLACK;
            }
            fb_base[(cursor + 2) * 14 * FB_PITCH + i] = RED;
            fb_base[(cursor + 3) * 14 * FB_PITCH + i] = RED;
        }
}

FileMenu::FileMenu() {

    sceIoMkdir("ux0:data/LaiNES", 0777);

    head = (SceIoDirenter *) malloc(sizeof(SceIoDirenter));
    SceIoDirenter * current = head;

    int dfd;
    dfd = sceIoDopen("ux0:data/LaiNES/");
    if(dfd >= 0) { 
        int res = 1;
        while(res > 0) {
            SceIoDirent dir;
            res = sceIoDread(dfd, &dir);
            if(res > 0) {
                memcpy(current->d_name, dir.d_name, strlen(dir.d_name)+1);
                current->next = NULL;
                current->next = (SceIoDirenter *) malloc(sizeof(SceIoDirenter));
                current = current->next;
                length = length + 1;
            }
        }
        sceIoDclose(dfd);
        sceClibPrintf("File count: %d\n", length);
    }

    void *sfn_file = malloc(0x100000);
    SceUID sfn_file_fd = sceIoOpen("app0:res/gohu.sfn", SCE_O_RDONLY, 0);
    int bytes_read = sceIoRead(sfn_file_fd, sfn_file, 0x100000);
    sceIoClose(sfn_file_fd);

    fnblit_set_font(sfn_file);
    fnblit_set_fg(WHITE);
    fnblit_set_bg(BLACK);

    SceUID mem_id = sceKernelAllocMemBlock(
        "LaiNES",
        SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW,
        ALIGN(FB_LEN, 0x40000), 
        NULL);
    
    sceKernelGetMemBlockBase(mem_id, (void**)&fb_base);
    fnblit_set_fb(fb_base, FB_PITCH, FB_WIDTH, FB_HEIGHT);
    fnblit_printf(960 / 2 - strlen("LaiNES") * 7 / 2, 0, "LaiNES");
    for(int i = 0; i < length; i++) {
        char name[256] = "";
        strcat(name, getItem(i + 1));
        sceClibPrintf("Found a file: %s\n", name);
        fnblit_printf(960 / 2 - strlen(name) * 7 / 2, (i + 2) * 14, name);
    }

    SceDisplayFrameBuf fb = {
        sizeof(fb),
        fb_base,
        FB_PITCH,
        SCE_DISPLAY_PIXELFORMAT_A8B8G8R8,
        FB_WIDTH,
        FB_HEIGHT
    };
    sceDisplaySetFrameBuf(&fb, SCE_DISPLAY_SETBUF_NEXTFRAME);
    sceDisplayWaitVblankStart();
    render();
}


}
