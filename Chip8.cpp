#include "Chip8.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

Chip8::Chip8() : ram(4096), pc(0x200)
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

    // Initialize renderer and surface
    renderer = SDL_CreateRenderer(window, -1, 0);
    surface = SDL_CreateRGBSurface(0,SCREEN_WIDTH,SCREEN_HEIGHT,32,0xFF000000,0x00FF0000,0x0000FF00,0x000000FF); // (r,g,b,a) depth
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

    // Decode and Execute
    switch (firstNibble)
    {
    case 0:
        if (inst == 0x00E0)
        {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Select drawing color for renderer
            SDL_RenderClear(renderer);                      // CLS - clear screen
            SDL_RenderPresent(renderer);
        }
        else
        {
            // RET
        }
        break;

    // JP addr
    case 1:
        pc = nnn;
        break;

    // LD Vx, byte
    case 6:
        Vx = mux(x);
        *Vx = kk;
        break;

    // ADD Vx, byte
    case 7:
         Vx = mux(x);
        *Vx = *Vx + kk;
        break;

    // LD I, addr
    case 0xA:
        idxReg = nnn;
        break;

    // DRW Vx, Vy, nibble
    case 0xD:
       
        // The starting position will wrap the sprite over the edge of the screen
        // Actual drawing is not wrapped!
        uint8_t xCoord = *mux(x) % SCREEN_WIDTH;
        uint8_t yCoord = *mux(y) % SCREEN_HEIGHT;
        VF = 0;
        
        uint8_t pixelRow; // Each bit represents a pixel on/off
        uint8_t curPixel; // current pixel
        uint32_t* p = (uint32_t*) surface->pixels; // array of pixels on the surface
        uint32_t * screenPixel = nullptr; // screen pixel

        SDL_LockSurface(surface); // Lock before pixel manipulation

        // Read n bytes from memory
        for(uint16_t offset = 0; offset < n; offset++) {
            pixelRow = ram[idxReg + offset];
            
            // Look at each pixel status in the row
            for(int j = 0; j < 8; j++) {

                 curPixel = (pixelRow & 0x80) >> 7;

                 // Find current pixel based on position
                 screenPixel = p + yCoord * surface->pitch + xCoord * surface->format->BytesPerPixel;
 
                // Begin comparison of screen pixel and current pixel
                if(curPixel) {
                    uint32_t temp = *screenPixel;
                    uint32_t temp2 = surface->format->Rmask;
                    uint32_t temp3 = *screenPixel & surface->format->Rmask;
                    if(*screenPixel & surface->format->Rmask) {
                        // Detected red, the pixel has color!
                       *screenPixel = 0;
                       *screenPixel |= surface->format->Amask;
                        VF = 1; 
                    } else {
                        *screenPixel = 0xFFFFFFFF;
                    }
                }

                xCoord++;
                pixelRow = pixelRow << 1;
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

        SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Clear renderer
        SDL_RenderClear(renderer);

        // Draw texture
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Update screen
        SDL_RenderPresent(renderer);

        break;
    }
}

/**
 * Select a register
 */
uint8_t* Chip8::mux(int reg)
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

Chip8::~Chip8(){
    // Clean-up
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/*
 * Function for keypresses
 */