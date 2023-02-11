#ifndef CHIP8_H
#define CHIP8_H

#include <SDL2/SDL.h>
#include <vector>
#include <stack>

class Chip8 {
    private: 
        uint16_t pc; // CHIP-8 program starts @ 0x200
        uint16_t idxReg; // idx reg will be set by games
        std::vector<uint8_t> ram; // 4kB or 4096 bytes large
        std::stack<int> stk;
        uint8_t delayReg, soundReg, V0, V1, V2, V3, V4, V5, V6,
                V7, V8, V9, VA, VB, VC, VD, VE, VF;

        SDL_Window * window;
        SDL_Renderer * renderer;
        SDL_Surface * surface;
        const int SCREEN_WIDTH = 64;
        const int SCREEN_HEIGHT = 32;
        const int SCREEN_FPS = 60;
        const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

    public:
        Chip8();

        /**
         * Read from memory
         */
        uint8_t readRAM(int idx);

        /**
         * Write to memory
         */
        void writeRAM(int idx, uint8_t val);

        /**
         * Fetch instruction + move PC 
         */
        uint16_t fetch();

        /**
         * Decode and execute instruction  
         */
        void decodeExec(uint16_t inst);

        /**
         * Select a register 
         */
         uint8_t* mux(int reg);


        /**
         * Get the current delay reg value
         */
        uint8_t getDelayReg();

        /**
         * Decrement the delay reg
         */
        void minusDelay();

        /**
         * Get the current sound reg value
         */
        uint8_t getSoundReg();

        /**
         * Decrement the sound reg
         * TODO: Add sound
         */
        void minusSound();


        ~Chip8();

        /*
         * Function for keypresses
         */

};
#endif