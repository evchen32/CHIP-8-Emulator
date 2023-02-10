#ifndef CHIP8_H
#define CHIP8_H

#include <SDL2/SDL.h>
#include <vector>
#include <stack>

class Chip8 {
    private: 
        int pc; // CHIP-8 program starts @ 200
        uint16_t idx; // idx reg will be set by games
        std::vector<uint8_t> ram; // 4kB or 4096 bytes large
        std::stack<int> stk;
        uint8_t delayReg, soundReg;

    public:
        Chip8();

        /**
         * Read from memory
         */
        uint8_t readRAM(int idx);

        /**
         * Fetch instruction + move PC 
         */
        uint16_t fetch();

        /**
         * Decode and execute instruction  
         */
        void decodeExec(uint16_t inst);


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

        /*
         * Function for keypresses
         */

};
#endif