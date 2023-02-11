#include <iostream>
#include <SDL2/SDL.h>
#include "Chip8.h"
#include "Timer.h"
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    Chip8 inst = Chip8();

    ifstream file("IBM Logo.ch8", ifstream::binary);
    uint16_t idx = 0x200;

    char byte;
    while(file.read(&byte,sizeof(byte))) {
        //cout << std::hex << (byte & 0xFF) << endl;
        inst.writeRAM(idx,(uint8_t)byte);
        idx++;
    }
    
    file.close();

    
    
    // Initialize display
    //SDL_Window * window = nullptr;

    const int SCREEN_WIDTH = 64;
    const int SCREEN_HEIGHT = 32;
    const int SCREEN_FPS = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

    /*if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "SDL_Init Error: " << SDL_GetError() << endl;
        return 1;
    }
    
    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == nullptr) {
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }*/

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

        while(SDL_PollEvent(&ev) != 0) {
            
            if(ev.type == SDL_QUIT) {
                isRunning = false;
            } else if(ev.type == SDL_KEYDOWN) {
                switch(ev.key.keysym.scancode) {
                    case SDL_SCANCODE_1:
                        cout << "1" << endl; 
                        break;
                    case SDL_SCANCODE_2:
                        cout << "2" << endl; 
                        break;
                    case SDL_SCANCODE_3:
                        cout << "3" << endl; 
                        break;
                    case SDL_SCANCODE_4:
                        cout << "C" << endl; 
                        break;
                    case SDL_SCANCODE_Q:
                        cout << "4" << endl; 
                        break;
                    case SDL_SCANCODE_W:
                        cout << "5" << endl; 
                        break;
                    case SDL_SCANCODE_E:
                        cout << "6" << endl; 
                        break;
                    case SDL_SCANCODE_R:
                        cout << "D" << endl; 
                        break;
                    case SDL_SCANCODE_A:
                        cout << "7" << endl; 
                        break;
                    case SDL_SCANCODE_S:
                        cout << "8" << endl; 
                        break;
                    case SDL_SCANCODE_D:
                        cout << "9" << endl; 
                        break;
                    case SDL_SCANCODE_F:
                        cout << "E" << endl; 
                        break;
                    case SDL_SCANCODE_Z:
                        cout << "A" << endl; 
                        break;
                    case SDL_SCANCODE_X:
                        cout << "0" << endl; 
                        break;
                    case SDL_SCANCODE_C:
                        cout << "B" << endl; 
                        break;
                    case SDL_SCANCODE_V:
                        cout << "F" << endl; 
                        break;
                }
                
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