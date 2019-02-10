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
    GPU(sf::RenderWindow& w, MMU* m);
    byte& operator[](const uint16_t&);
    void write(const uint16_t &address, const uint8_t &value);
    void controlDisplay(const byte& value);

private:
    MMU* mmu;
    sf::RenderWindow& window;
    byte VRAM[8192];    // 0x8000 to 0x9FFF
    byte OAM[160];      // Object Attribute Memory containing Sprite Attribute Table (0xFE00 to 0xFE9F)
};


#endif //GAMEBOYEMULATOR_GPU_H
