#include <SDL2/SDL.h>
#include <iostream>
#include "Chip8.h"
#include "Timer.h"

using namespace std;

// Prints out the font
void checkFontMem(Chip8 &inst) {
    // Check from 0x050 to 0x09F
    uint8_t num = 0;

    for(int i = 80; i < 160; i++) {
        num = inst.readRAM(i);
        cout << std::hex << (num & 0xFF) << endl;
    }
}


// Unit testing
int main(int argc, char* argv[]) {
    Chip8 inst = Chip8();
    checkFontMem(inst);
    return 0;
}