//
// Created by sam on 05.10.18.
//

#include <assert.h>
#include "GPU.h"
#include "MMU.h"

GPU::GPU(sf::RenderWindow& w) : window(w) {image.create(256, 256);}

void GPU::write(const uint16_t &address, const uint8_t &value) {
    (*this)[address] = value;
}

GPU::byte GPU::getBackgroundPixel(GPU::byte x, GPU::byte y) {
    byte tileX = x/8;
    byte tileY = y/8;

    uint16_t tileAddress = windowMap + 32 * tileY + tileX;
    int tileId = (*this)[tileAddress];
//    if (tiles == 0x8800) {
//         tileId += 128;
//    }
    uint16_t tileDataAddress = tileId * 16 + tiles;

    byte pixelX = x % 8;
    byte pixelY = y % 8;

    tileDataAddress += 2 * pixelY;

    byte a = (*this)[tileDataAddress];
    byte b = (*this)[tileDataAddress + 1];
    a >>= 7 - pixelX;
    b >>= 7 - pixelX;

    return (a & 1) + 2*(b & 1);
}

void GPU::controlDisplay(const byte& value) {
//    byte screen[65536];
    bool windowEnabled = (value & 0x20) >> 5;
    bgEnabled = (bool)(value & 0x01);
    if (value & 0x80) {
        // Enable LCD
    }
    else {
        window.clear(sf::Color::White);
        return;
    }
    if (value & 0x40) {
        windowMap = 0x9C00;
        // Select 0x9C00-0x9FFF as window tile map
    }
    else {
        windowMap = 0x9800;
        // Select 0x9800-0x9BFF as window tile map
    }
//    if (value & 0x20) {
//        windowEnabled = true;
//    }
//    else {
//        windowEnabled = false;
//    }
    if (value & 0x10) {
        tiles = 0x8000;
        // Select 0x8000-0x8FFF as tile data
    }
    else {
        tiles = 0x8800;
        // Select 0x8800-0x97FF as tile data
    }
    if (value & 0x08) {
        bgMap = 0x9C00;
        // Select 0x9C00-0x9FFF as BG tile map
    }
    else {
        bgMap = 0x9800;
        // Select 0x9800-0x9BFF as BG tile map
    }
//    if ((*this)[0xFF40] & 0x04) {
//        // Set 8x16 sprite size
//    }
//    else {
//        // Set 8x8 sprite size
//    }
//    if ((*this)[0xFF40] & 0x02) {
//        // Enable sprite display
//    }
//    else {
//        // Disable sprite display
//    }
}

GPU::byte& GPU::operator[](const uint16_t & address) {
    if (address >= 0x8000 && address <= 0x9FFF) {
        return VRAM[address - 0x8000];
    }
    else {
        return OAM[address - 0xFE00];
    }
}

void GPU::setMMU(MMU *m) {
    mmu = m;
}

void GPU::checkCycles() {
    if (currCycles >= 456) {
        currCycles -= 456;
        drawLine();
    }
}

void GPU::drawLine() {
    if (bgEnabled) {
        for (int x = 0; x < 160; ++x) {
            sf::Color c;
            int colorId = getBackgroundPixel(x, lineNr);
            int colorNr = mmu->readByte(0xFF47) >> (2 * colorId) & 0x3;
            switch(colorNr) {
                default: break;
                case 0: c = sf::Color::White; break;
                case 1: c = sf::Color(0x67, 0x67, 0x67); break;
                case 2: c = sf::Color(0xb6, 0xb6, 0xb6); break;
                case 3: c = sf::Color::Black; break;
            }
            image.setPixel(x, lineNr , c);
        }
    }
    sf::Sprite mySprite;
    sf::Texture texture;
    texture.loadFromImage(image);
    mySprite.setTexture(texture);
    mySprite.scale(4, 4);
    window.clear();
    window.draw(mySprite);
    mmu->writeByte(0xFF44, lineNr);
    ++lineNr;
//    cerr << lineNr << endl;
    if (lineNr >= 155) {
//        image.create(256, 256);
        lineNr = 0;
    }
}

void GPU::addCycles(unsigned int num) {
    currCycles += num;
}
