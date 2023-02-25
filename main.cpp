#include <iostream>
#include <SDL2/SDL.h>
#include "Chip8.h"
#include "Timer.h"
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    Chip8 inst = Chip8();

    ifstream file("Breakout.ch8", ifstream::binary);
    uint16_t idx = 0x200;

    char byte;
    while(file.read(&byte,sizeof(byte))) {
        //cout << std::hex << (byte & 0xFF) << endl;
        inst.writeRAM(idx,(uint8_t)byte);
        idx++;
    }
    
    file.close();

    
    
    // Initialize display
    const int SCREEN_WIDTH = 64;
    const int SCREEN_HEIGHT = 32;
    const int SCREEN_FPS = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

    // Initialize FPS
    Timer fpsTimer, capTimer;
    int frameCtr = 0;

    // Start running display
    //SDL_Delay(5000);
    bool isRunning = true;
    SDL_Event ev;

    fpsTimer.start(); // start fps timer

    // Each loop iteration is a cycle - 1 frame update
    while(isRunning) {
        capTimer.start();

        // polling for events also updates the keyboard state with pumpEvents()
        while(SDL_PollEvent(&ev) != 0) {
            if(ev.type == SDL_QUIT) {
                isRunning = false;
            } else if(ev.type == SDL_KEYDOWN) {
                inst.keyPress(ev.key.keysym.scancode);
            }
        }

        // CHIP-8 F-D-X - On avg. 11.67 instructions are executed per cycle (processor run @ 700 inst./sec.)
        for(int i = 0; i < 11; i++) {
            uint16_t out = inst.fetch();
            inst.decodeExec(out);
        }

        // Update delay and sound regs
        if(inst.getDelayReg() > 0) {
            inst.minusDelay();
        }

        if(inst.getSoundReg() > 0) {
            inst.minusSound();
        }

        

        // Check and wait if frame finished early - ensures 60 fps
        int frameTicks = capTimer.getTicks();
        if(frameTicks < SCREEN_TICKS_PER_FRAME) {
            SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
        }
    }

    return 0;
}