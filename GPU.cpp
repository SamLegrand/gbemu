//
// Created by sam on 05.10.18.
//

#include <assert.h>
#include "GPU.h"
#include "MMU.h"

GPU::GPU(sf::RenderWindow& w) : window(w) {image.create(256, 256);}

void GPU::write(const uint16_t &address, const uint8_t &value) {
    (*this)[address] = value;
    if (address == 0xFF44) {
        controlDisplay(value);
    }
}

void GPU::controlDisplay(const byte& value) {
//    byte screen[65536];
    uint16_t windowMap; // 1024
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
//    byte scrollX = mmu->readByte(0xFF43);
//    byte scrollY = mmu->readByte(0xFF42);
//    byte y = lineNr + scrollY;
//
    if (bgEnabled) {
//        for (byte pixelX = 0; pixelX < 160; pixelX += 8) {
//            byte x = pixelX + scrollX;
//            byte tileX = x/8;
//            byte tileY = y/8;
//            uint16_t bgMapAddress = bgMap + 32*tileY + tileX;
//            uint16_t tileNr = (*this)[bgMapAddress]*16 + tiles;
//            for (int j = tileNr; j < tileNr + 16; j += 2) {
//                unsigned int pixel = 0;
//                unsigned int count = 7;
//                for (int k = 0x80; k > 0; k /= 2) {
//                    sf::Color c;
//                    int colorNr;
//                    int number = (((*this)[j + 1] & k) >> (count - 1)) + (((*this)[j] & k) >> count);
//                    --count;
//                    colorNr = mmu->readByte(0xFF47) & (0x03 << number);
//                    switch(colorNr) {
//                        default: break;
//                        case 0: c = sf::Color::White; break;
//                        case 1: c = sf::Color(0x67, 0x67, 0x67); break;
//                        case 2: c = sf::Color(0xb6, 0xb6, 0xb6); break;
//                        case 3: c = sf::Color::Black; break;
//                    }
////                    cout << dec << i%32*8 + pixel << ", ";
////                    cout << (i - bgMap)/32*8 + lineNr << ": ";
////                    cout << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << endl;
//                    image.setPixel(bgMapAddress%32*8 + pixel, lineNr, c);
//                    ++pixel;
//                }
//            }
//        }
        for (int i = bgMap; i < bgMap + 1024; ++i) {
            uint16_t tileNr = (*this)[i]*16 + tiles;
            if (tiles == 0x8800) {
                tileNr += 128;
            }
            for (int j = tileNr; j < tileNr + 16; j += 2) {
                unsigned int line = (j - tileNr)/2;
                if (line == lineNr) {
//                    j - line*2 = tileNr;
                    unsigned int pixel = 0;
                    unsigned int count = 7;
                    for (int k = 0x80; k > 0; k /= 2) {
                        sf::Color c;
                        int colorNr;
                        int number = (((*this)[j + 1] & k) >> (count - 1)) + (((*this)[j] & k) >> count);
                        --count;
                        colorNr = mmu->readByte(0xFF47) & (0x03 << number);
                        switch(colorNr) {
                            default: break;
                            case 0: c = sf::Color::White; break;
                            case 1: c = sf::Color(0x67, 0x67, 0x67); break;
                            case 2: c = sf::Color(0xb6, 0xb6, 0xb6); break;
                            case 3: c = sf::Color::Black; break;
                        }
//                    cout << dec << i%32*8 + pixel << ", ";
//                    cout << (i - bgMap)/32*8 + lineNr << ": ";
//                    cout << (int)c.r << ", " << (int)c.g << ", " << (int)c.b << endl;
                        image.setPixel(i%32*8 + pixel, (i - bgMap)/32*8 + lineNr , c);
                        ++pixel;
                    }
                }
            }
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
        image.create(256, 256);
        lineNr = 0;
    }
}

void GPU::addCycles(unsigned int num) {
    currCycles += num;
}
