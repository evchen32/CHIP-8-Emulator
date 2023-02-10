#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <bitset>
#include <SDL2/SDL.h>
#include <stack>
#include <string>
#include "Timer.h"

using namespace std;

class Chip8 {
    private: 
        int pc;
        uint16_t idx; // idx reg will be set by games
        vector<uint8_t> ram;
        stack<int> stk;
        uint8_t delayReg, soundReg;

    public:
        Chip8() {
            ram = vector<uint8_t>(4096); // 4kB or 4096 bytes large
            pc = 200; // CHIP-8 program starts here
            delayReg = 0;
            soundReg = 0;

            // load font data - popular convention to populate from 0x50 - 0x9F (big endian)
            string line;
            fstream file;
            auto it = ram.begin() + 80;

            file.open("font.txt");
            
            while(getline(file, line)) {
                *it = std::stoi(line, nullptr, 0); // converts to hex 
                it++;
            }

            file.close();
            
        }

        /**
         * Fetch instruction + move PC 
         */
        uint16_t fetch() {
            uint16_t inst = 0;
            inst |= ram[pc];
            inst = inst << 8;
            inst |= ram[pc + 1];

            pc += 2;
            return inst;
        }


        /**
         * Get the current delay reg value
         */
        uint8_t getDelayReg() {
            return delayReg;
        }

        /**
         * Decrement the delay reg
         */
        void minusDelay() {
            delayReg--;
        }

        /**
         * Get the current sound reg value
         */
        uint8_t getSoundReg() {
            return soundReg;
        }

        /**
         * Decrement the sound reg
         * TODO: Add sound
         */
        void minusSound() {
            soundReg--;
        }

        /*
         * Function for keypresses
         */

};

int main(int argc, char* argv[]) {
    Chip8 inst = Chip8();
    /*for(int i = 80; i < 160; i++) {
        bitset<8> x(inst.ram[i]);
        cout << std::hex << x << endl;
    }*/

    // Initialize display
    SDL_Window * window = nullptr;

    const int SCREEN_WIDTH = 64;
    const int SCREEN_HEIGHT = 32;
    const int SCREEN_FPS = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        cout << "SDL_Init Error: " << SDL_GetError() << endl;
        return 1;
    }
    
    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(window == nullptr) {
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

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



        // Render frame
        SDL_UpdateWindowSurface(window);

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

    // Clean-up
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}