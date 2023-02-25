#include "Chip8.h"
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace std;

Chip8::Chip8() : ram(4096), pc(0x200), pause(false)
{
    // load font data - popular convention to populate from 0x50 - 0x9F (big endian)
    string line;
    fstream file;
    auto it = ram.begin() + 0x50;

    file.open("font.txt");

    while (getline(file, line))
    {
        *it = std::stoi(line, nullptr, 0); // converts to hex
        it++;
    }

    file.close();

    // Initialize window
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        cout << "SDL_Init Error: " << SDL_GetError() << endl;
    }

    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_MAXIMIZED);
    if (window == nullptr)
    {
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
    }

    // Initialize renderer, surface, and texture
    renderer = SDL_CreateRenderer(window, -1, 0);
    surface = SDL_CreateRGBSurface(0,SCREEN_WIDTH,SCREEN_HEIGHT,32,0xFF000000,0x00FF0000,0x0000FF00,0x000000FF); // (r,g,b,a) depth
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0,0,0,0xFF)); // Make the surface pixels all black
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
}

/**
 * Read from memory
 */
uint8_t Chip8::readRAM(int idx)
{
    return ram[idx];
}

/**
 * Write to memory
 */
void Chip8::writeRAM(int idx, uint8_t val)
{
    ram[idx] = val;
}

/**
 * Fetch instruction + move PC
 */
uint16_t Chip8::fetch()
{
    //cout << pc << endl;
    //cout << std::hex << (ram[pc] & 0xFF) << endl;
    //cout << std::hex << (ram[pc+1] & 0xFF) << endl;
    uint16_t inst = 0;
    inst |= ram[pc];
    inst = inst << 8;
    inst |= ram[pc + 1];
    //cout << std::hex << inst << endl;
    pc += 2;
    return inst;
}

/**
 * Decode and execute instruction
 */
void Chip8::decodeExec(uint16_t inst)
{
    // Decode - extract values from opcode
    uint16_t firstNibble = (inst & 0xF000) >> 12; // First nibble determines the instruction
    uint16_t x = (inst & 0x0F00) >> 8;            // second nibble
    uint16_t y = (inst & 0x00F0) >> 4;            // third nibble
    uint16_t n = inst & 0x000F;                   // fourth nibble
    uint16_t kk = inst & 0x00FF;                 // second byte
    uint16_t nnn = inst & 0x0FFF;                // second, third, and fourth nibbles (intermediate memory address)

    uint8_t * Vx = nullptr;
    uint8_t * Vy = nullptr;

    // Decode and Execute
    switch (firstNibble)
    {
    case 0:
        // CLS - clear the display
        if (nnn == 0x0E0)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Select drawing color for renderer
            SDL_RenderClear(renderer);                      // CLS - clear screen
            SDL_RenderPresent(renderer);
        }
        else if (nnn == 0x0EE)
        {
            // RET - return from a subroutine
            pc = stk.top();
            stk.pop();
        }
        break;

    // JP addr
    case 1:
        pc = nnn;
        break;

    // CALL addr - call subroutine at nnn
    case 2:
        stk.push(pc);
        pc = nnn;
        break;

    // SE Vx, byte - skip next instruction if Vx = kk
    case 3:
        Vx = muxReg(x);
        if(*Vx == kk) 
        {
            pc += 2;
        }
        break;
    
    // SNE Vx, byte
    case 4:
        Vx = muxReg(x);
        if(*Vx != kk)
        {
            pc += 2;
        }
        break;

    // SE Vx, Vy
    case 5:
        Vx = muxReg(x);
        Vy = muxReg(y);
        if(*Vx == *Vy) {
            pc += 2;
        }
        break;

    // LD Vx, byte
    case 6:
        Vx = muxReg(x);
        *Vx = kk;
        break;

    // ADD Vx, byte
    case 7:
         Vx = muxReg(x);
        *Vx = *Vx + kk;
        break;

    case 8:
        Vx = muxReg(x);
        Vy = muxReg(y);

        switch(n)
        {   
            // LD Vx, Vy
            case 0:
                *Vx = *Vy;
                break;
            // OR Vx, Vy
            case 1:
                *Vx = *Vx | *Vy;
                break;
            // AND Vx, Vy
            case 2:
                *Vx = *Vx & *Vy;
                break;
            // XOR Vx, Vy
            case 3:
                *Vx = *Vx ^ *Vy;
                break;
            // ADD Vx, Vy
            case 4:
            {
                uint16_t xVal = *Vx;
                uint16_t yVal = *Vy;
                // Trigger flag based on overflow
                if(xVal + yVal > 255) {
                    VF = 1;
                } else {
                    VF = 0;
                }

                *Vx = *Vx + *Vy;
                break;
            }
            // SUB Vx, Vy
            case 5:
                if(*Vx > *Vy) {
                    VF = 1;
                } else {
                    VF = 0;
                }
                *Vx = *Vx - *Vy;
                break;
            // SHR Vx {,Vy} 
            case 6:
                *Vx = *Vx >> 1;
                if(*Vx & 1 == 1) {
                    VF = 1;
                } else {
                    0;
                }

                break;
            
            // SUBN Vx, Vy
            case 7:
                if(*Vy > *Vx) {
                    VF = 1;
                } else {
                    VF = 0;
                }
 
                *Vx = *Vy - *Vx;
                break;
            // SHL Vx {,Vy}
            case 0xE:
                *Vx = *Vx << 1;

                if(*Vx & 0x80 == 0x80) {
                    VF = 1;
                } else {
                    VF = 0;
                }
                break;
        }
        
        break;
    
    // SNE Vx, Vy
    case 9:
        Vx = muxReg(x);
        Vy = muxReg(y);
        if(*Vx != *Vy) {
            pc += 4;
        }
        break;

    // LD I, addr
    case 0xA:
        idxReg = nnn;
        break;

    // JP V0, addr
    case 0xB:
        pc = nnn + *muxReg(0);
        break;

    // RND Vx, byte
    case 0xC:
    {
        Vx = muxReg(x);
        uint8_t randNum = rand() % (kk + 1); // generate random num from 0 - kk
        *Vx = *Vx & randNum;
        break;
    }
    // DRW Vx, Vy, nibble
    case 0xD:
    {
       
        // The starting position will wrap the sprite over the edge of the screen
        // Actual drawing is not wrapped!
        uint8_t startX = *muxReg(x) % SCREEN_WIDTH; // starting x coordinate of sprite
        uint8_t startY = *muxReg(y) % SCREEN_HEIGHT; // starting y coordinate of sprite
        
        VF = 0; // reset VF register
        
        uint8_t spriteRow; // A row of pixels from sprite - each bit represents a pixel on/off
        uint8_t spritePixel; // current sprite pixel
        uint32_t * screenPixel = nullptr; // screen pixel

        
        SDL_LockSurface(surface); // Lock before pixel manipulation
        uint32_t* surfacePixels = (uint32_t*) surface->pixels; // array of pixels on the surface
        uint8_t xCoord, yCoord; // Surface pixel x and y coordinates

        // Read n bytes from memory
        for(uint16_t offset = 0; offset < n; offset++) {
            spriteRow = ram[idxReg + offset];
            yCoord = startY + offset;
            
            // Look at each pixel status in the row
            for(int j = 0; j < 8; j++) {
                xCoord = startX + j;

                spritePixel = ((spriteRow << j) & 0x80) >> 7;

                // Find current screen pixel based on position
                screenPixel = surfacePixels + yCoord * (surface->pitch / 4) + xCoord;
 
                // Begin comparison of screen pixel and current pixel
                if(spritePixel) {
                    uint8_t r, g, b, a;
                    SDL_GetRGBA(*screenPixel, surface->format, &r ,&g, &b, &a);

                    if(r == 0xFF) {
                        // Detected red, the pixel has color!
                       *screenPixel = SDL_MapRGBA(surface->format, 0, 0, 0, 0xFF);
                        VF = 1; 
                    } else {
                        *screenPixel = SDL_MapRGBA(surface->format, 0xFF, 0xFF, 0xFF, 0xFF);
                        VF = 0;
                    }
                }

                if(xCoord == SCREEN_WIDTH) {
                    break;
                }
            }

            yCoord++;
            if(yCoord == SCREEN_HEIGHT) {
                break;
            }
        }

        SDL_UnlockSurface(surface); // Unlock after pixel manipulation

        SDL_UpdateTexture(texture, NULL, surfacePixels, surface->pitch); // update texture with new pixel data
        
        // Clear renderer
        //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw texture
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Update screen
        SDL_RenderPresent(renderer);

        break;
    }
        // SKP
        case 0xE:
        {
            Vx = muxReg(x);
            int scanCode = getScanCode(*Vx);
            const uint8_t * keyStates = SDL_GetKeyboardState(NULL);

            // SKP Vx
            if(kk == 0x9E) {
                if(keyStates[scanCode]) {
                    pc +=2;
                }
            } else if(kk == 0xA1) {
                // SKNP Vx
                if(!keyStates[scanCode]) {
                    pc += 2;
                } 
            }
           break;
        }
        case 0xF:
            Vx = muxReg(x);

            if(kk == 0x07) {
                *Vx = delayReg; // LD Vx, DT
            } else if(kk == 0x0A) {
                pc -= 2; // LD Vx, K - wait for keypress, store value of key in Vx
                pause = true;

                while(SDL_PollEvent(&ev)) {
                    if(ev.type == SDL_KEYDOWN) {
                        pause = false;
                        pc += 2;
                        *Vx = getKeyValue(ev.key.keysym.scancode);
                        break;
                    }
                }
            } else if(kk == 0x15) {
                delayReg = *Vx; // LD DT, Vx
            } else if(kk == 0x18) {
                soundReg = *Vx; // LD ST, Vx
            } else if(kk == 0x1E) {
                Vx = muxReg(x);
                uint16_t xVal = *Vx;
                
                // special case of overflow - Spacefight 2091 relies on this behavior
                if(xVal + idxReg > 0xFFFF) {
                    VF = 1;
                } else {
                    VF = 0;
                }
                idxReg += *Vx; // ADD I, Vx
            } else if(kk == 0x29) {
                // LD F, Vx - set idx to location of sprite for digit Vx
                idxReg = 0x50 + (*Vx & 0xF)*5; // Calculate location of the font - 5 bytes per char font
            } else if(kk == 0x33) {
                // LD B, Vx - Store binary-coded decimal conversion
                uint8_t num = *Vx;
                ram[idxReg] = num / 100;
                ram[idxReg + 1] = (num % 100) / 10;
                ram[idxReg + 2] = num % 10;
            } else if(kk == 0x55) {
                // LD [I], Vx
                for(int i = 0; i <= x; i++) {
                    ram[idxReg + i] = *muxReg(i);
                }

            } else if(kk == 0x65) {
                // LD Vx, [I]
                for(int i = 0; i <= x; i++) {
                    *muxReg(i) = ram[idxReg + i];
                }
            }
            break;
    }
}

/**
 * Get key hex value 
 * 
 */
uint8_t Chip8::getKeyValue(int scanCode) {
   uint8_t keyVal = 0;
    
    switch(scanCode) {
        case SDL_SCANCODE_1:
            keyVal = 0x1;
            break;
        case SDL_SCANCODE_2:
            keyVal = 0x2;
            break;
        case SDL_SCANCODE_3:
            keyVal = 0x3;
            break;
        case SDL_SCANCODE_4:
            keyVal = 0xC;
            break;
        case SDL_SCANCODE_Q:
            keyVal = 0x4;
            break;
        case SDL_SCANCODE_W:
            keyVal = 0x5;
            break;
        case SDL_SCANCODE_E:
            keyVal = 0x6;
            break;
        case SDL_SCANCODE_R:
            keyVal = 0xD;
            break;
        case SDL_SCANCODE_A:
            keyVal = 0x7;
            break;
        case SDL_SCANCODE_S:
            keyVal = 0x8;
            break;
        case SDL_SCANCODE_D:
            keyVal = 0x9;
            break;
        case SDL_SCANCODE_F:
            keyVal = 0xE;
            break;
        case SDL_SCANCODE_Z:
            keyVal = 0xA;
            break;
        case SDL_SCANCODE_X:
            keyVal = 0x0;
            break;
        case SDL_SCANCODE_C:
            keyVal = 0xB;
            break;
        case SDL_SCANCODE_V:
            keyVal = 0xF;
            break;
    }

    return keyVal;
} 

/**
 * Get key scan code
 * 
 */
int Chip8::getScanCode(uint8_t key)
{
    int scanCode = 0;
    
    switch(key) {
        case 0x1:
            scanCode = SDL_SCANCODE_1;
            break;
        case 0x2:
            scanCode = SDL_SCANCODE_2;
            break;
        case 0x3:
            scanCode = SDL_SCANCODE_3;
            break;
        case 0xC:
            scanCode = SDL_SCANCODE_4;
            break;
        case 0x4:
            scanCode = SDL_SCANCODE_Q;
            break;
        case 0x5:
            scanCode = SDL_SCANCODE_W;
            break;
        case 0x6:
            scanCode = SDL_SCANCODE_E;
            break;
        case 0xD:
            scanCode = SDL_SCANCODE_R;
            break;
        case 0x7:
            scanCode = SDL_SCANCODE_A;
            break;
        case 0x8:
            scanCode = SDL_SCANCODE_S;
            break;
        case 0x9:
            scanCode = SDL_SCANCODE_D;
            break;
        case 0xE:
            scanCode = SDL_SCANCODE_F;
            break;
        case 0xA:
            scanCode = SDL_SCANCODE_Z;
            break;
        case 0x0:
            scanCode = SDL_SCANCODE_X;
            break;
        case 0xB:
            scanCode = SDL_SCANCODE_C;
            break;
        case 0xF:
            scanCode = SDL_SCANCODE_V;
            break;
    }

    return scanCode;
}

/**
 * Select a register
 */
uint8_t* Chip8::muxReg(int reg)
{
    switch (reg)
    {
    case 0:
        return &V0;
        break;
    case 1:
        return &V1;
        break;
    case 2:
        return &V2;
        break;
    case 3:
        return &V3;
        break;
    case 4:
        return &V4;
        break;
    case 5:
        return &V5;
        break;
    case 6:
        return &V6;
        break;
    case 7:
        return &V7;
        break;
    case 8:
        return &V8;
        break;
    case 9:
        return &V9;
        break;
    case 0xA:
        return &VA;
        break;
    case 0xB:
        return &VB;
        break;
    case 0xC:
        return &VC;
        break;
    case 0xD:
        return &VD;
        break;
    case 0xE:
        return &VE;
        break;
    case 0xF:
        return &VF;
        break;
    }

    return nullptr;
}

/**
 * Get the current delay reg value
 */
uint8_t Chip8::getDelayReg()
{
    return delayReg;
}

/**
 * Decrement the delay reg
 */
void Chip8::minusDelay()
{
    delayReg--;
}

/**
 * Get the current sound reg value
 */
uint8_t Chip8::getSoundReg()
{
    return soundReg;
}

/**
 * Decrement the sound reg
 * TODO: Add sound
 */
void Chip8::minusSound()
{
    soundReg--;
}

/**
 * Checks if execution is paused
 * 
 */
bool Chip8::isPaused() {
    return pause; 
}

Chip8::~Chip8(){
    // Clean-up
    SDL_DestroyRenderer(renderer);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/*
 * Function for keypresses
 */