//
// Created by sam on 05.10.18.
//

#ifndef GAMEBOYEMULATOR_MMU_H
#define GAMEBOYEMULATOR_MMU_H


#include <cstdint>
#include <iostream>
#include <fstream>
#include "GPU.h"

using namespace std;

class MMU {
    typedef unsigned char byte;
public:
    explicit MMU();
    void loadROM(const std::string& filename);
    void loadBIOS(const std::string& filename);
    uint16_t readDoubleByte(uint16_t &address);
    void writeDoubleByte(uint16_t &address, const uint16_t &value);
    byte readByte(const uint16_t &address);
    void writeByte(const uint16_t &address, const uint8_t &value);
    void setBoot(const bool& toSet);
    bool getBoot() const;
    void setGPU(GPU* g);

private:
    GPU* gpu;
    bool boot;
    byte BIOS[256] = {0};
    byte ROM[32768] = {0};    // Cartridge ROM (0x0000 to 0x7FFF)
    byte WRAM[8192] = {0};    // Stores high scores and saved games (0xA000 to 0xBFFF)
    byte RAM[8192] = {0};     // Working RAM provided by MMU (0xC000 to 0xDFFF)
    byte IO[128] = {0};       // I/O Registers (0xFF00 to 0xFF7F)
    byte HRAM[127] = {0};      // Where stack data is stored
};

#endif //GAMEBOYEMULATOR_MMU_H
