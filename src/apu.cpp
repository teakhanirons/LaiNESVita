#include "include/gui.hpp"
#include "include/cpu.hpp"
#include "include/apu.hpp"
#include <dolcesdk.h>
#include <psp2/audioout.h>

namespace APU {


Nes_Apu apu;
Blip_Buffer buf;

const int OUT_SIZE = 4096;
blip_sample_t outBuf[OUT_SIZE];

SceUID audio_thread_id = -1;
int ready = 0;
int port = -1;

int audio_thread(unsigned int args, void* argp) {
	while(1) {
		if(ready) {
		int ret = 0;
	if(port < 0) {
		port = sceAudioOutOpenPort(
		SCE_AUDIO_OUT_PORT_TYPE_MAIN,
		buf.read_samples(outBuf, OUT_SIZE),
		48000,
		SCE_AUDIO_OUT_MODE_MONO);
	}
			ret = sceAudioOutOutput(port, outBuf);
			sceClibPrintf("out1 0x%x\n", ret);
			ret = sceAudioOutOutput(port, NULL);
			sceClibPrintf("out2 0x%x\n", ret);
			buf.clear();
			//outBuf, buf.read_samples(outBuf, OUT_SIZE);
			ready = 0;
		}
		sceKernelDelayThread(1 / 60 * 1000 * 1000);
	}
	return 0;
}

void init()
{
    buf.sample_rate(48000);
    buf.clock_rate(1789773);

    apu.output(&buf);
    apu.dmc_reader(CPU::dmc_read);

audio_thread_id = sceKernelCreateThread("audio_thread", audio_thread, 0x40, 0x10000, 0, 0, NULL);
sceKernelStartThread(audio_thread_id, 0, NULL);

}

void reset()
{
    apu.reset();
    buf.clear();
}

template <bool write> u8 access(int elapsed, u16 addr, u8 v)
{
    if (write)
        apu.write_register(elapsed, addr, v);
    else if (addr == apu.status_addr)
        v = apu.read_status(elapsed);

    return v;
}
template u8 access<0>(int, u16, u8); template u8 access<1>(int, u16, u8);

void run_frame(int elapsed)
{
    apu.end_frame(elapsed);
    buf.end_frame(elapsed);

    if (buf.samples_avail() >= OUT_SIZE && !ready) ready = 1;
        
}


}
