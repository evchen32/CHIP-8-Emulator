#include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
#include<bitset>

using namespace std;

class Chip8 {
    
    int pc, idx; // idx reg will be set by games
    //vector<char> ram;

    public:
        vector<char> ram;

        Chip8() {
            ram = vector<char>(4096); // 4kB or 4096 bytes large
            pc = 200; // CHIP-8 program starts here

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
            cout << "" << endl;

        }

};

int main() {
    Chip8 inst = Chip8();
    for(int i = 80; i < 160; i++) {
        bitset<8> x(inst.ram[i]);
        cout << std::hex << x << endl;
    }
}