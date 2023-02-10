#include "Chip8.h"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

Chip8::Chip8() : ram(4096), pc(200), delayReg(0), soundReg(0)
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
}

/**
 * Read from memory
 */
uint8_t Chip8::readRAM(int idx) {
    return ram[idx];
}

/**
 * Fetch instruction + move PC
 */
uint16_t Chip8::fetch()
{
    uint16_t inst = 0;
    inst |= ram[pc];
    inst = inst << 8;
    inst |= ram[pc + 1];

    pc += 2;
    return inst;
}

/**
 * Decode and execute instruction
 */
void Chip8::decodeExec(uint16_t inst)
{
    // First nibble determines the instruction
    uint16_t firstNibble = (inst & 0xF000) >> 12;

    /*switch (firstNibble)
    {
    }*/
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

/*
 * Function for keypresses
 */