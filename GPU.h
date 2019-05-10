//
// Created by sam on 05.10.18.
//

#ifndef GAMEBOYEMULATOR_GPU_H
#define GAMEBOYEMULATOR_GPU_H

#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;
class MMU;

class GPU {
    using byte = unsigned char;
public:
    GPU(sf::RenderWindow& w);
    byte& operator[](const uint16_t&);
    void write(const uint16_t &address, const uint8_t &value);
    void controlDisplay(const byte& value);
    void setMMU(MMU* m);
    void checkCycles();
    void drawLine();
    void addCycles(unsigned int num);
    byte getBackgroundPixel(byte x, byte y);

private:
    unsigned int currCycles = 0;
    unsigned int lineNr = 0;
    bool bgEnabled = false;
    uint16_t bgMap;
    uint16_t tiles;
    uint16_t windowMap;
    MMU* mmu;
    sf::RenderWindow& window;
    sf::Image image;
    byte VRAM[8192];    // 0x8000 to 0x9FFF
    byte OAM[160];      // Object Attribute Memory containing Sprite Attribute Table (0xFE00 to 0xFE9F)
};


#endif //GAMEBOYEMULATOR_GPU_H
