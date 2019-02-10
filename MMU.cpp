//
// Created by sam on 05.10.18.
//

#include "MMU.h"

MMU::MMU(sf::RenderWindow& w) : boot(true), myGPU(w, this) {
    loadBIOS("../DMG_ROM.bin");
    loadROM("../tetris.gb");
};

MMU::byte MMU::readByte(const uint16_t &address) {
    cout << "Reading from " << address << endl;
    if (boot && address < 0x0100) {
        return BIOS[address];
    }
    else if (address < 0x7FFF) {
        return ROM[address];
    }
    else if (address >= 0xC000 && address <= 0xDFFF) {
        return RAM[address - 0xC000];
    }
    else if ((address >= 0x8000 && address <= 0x9FFF) ||
            (address >= 0xFE00 && address <= 0xFE9F)) {
        return myGPU[address];
    }
    else if (address >= 0xFF00 && address <= 0xFF7F) {
        return IO[address - 0xFF00];
    }
    else if (address <= 0xFFFE && address >= 0xFF80) {
        return HRAM[address - 0xFF80];
    }
//    else if (address == 0xFF44) {
//        return 0;
//    }
    return 0;
}

void MMU::writeByte(const uint16_t &address, const uint8_t &value) {
//    cout << "Writing to " << hex << (int)address << endl;
    if (boot && address < 0x0100) {
        BIOS[address] = value;
    }
    else if (address <= 0x7FFF) {
        ROM[address] = value;
    }
    else if (address >= 0xC000 && address <= 0xDFFF) {
        RAM[address - 0xC000] = value;
    }
    else if ((address <= 0x9FFF && address >= 0x8000) ||
            (address >= 0xFE00 && address <= 0xFE9F)) {
        myGPU.write(address, value);
    }
    else if (address >= 0xFF00 && address <= 0xFF7F) {
        IO[address - 0xFF00] = value;
        if (address == 0xFF40) {
            myGPU.controlDisplay(value);
        }
    }
    else if (address <= 0xFFFE && address >= 0xFF80) {
        HRAM[address - 0xFF80] = value;
    }
}

uint16_t MMU::readDoubleByte(uint16_t &address) {
    if (boot && address < 0x0100) {
        uint16_t value = BIOS[++address];
        value |= BIOS[++address] << 8;
        return value;
    }
    else if (address < 0x7FFF) {
        uint16_t value = ROM[++address];
        value |= ROM[++address] << 8;
        return value;
    }
    return 0;
}

void MMU::loadROM(const std::string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file '" << filename << "'." << endl;
    }
    byte b = 0;
    unsigned int index = 0;
    while (file >> noskipws >> b) {
        ROM[index++] = b;
    }
}

void MMU::loadBIOS(const std::string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file '" << filename << "'." << endl;
    }
//    else {
//        cout << "Loading BIOS..." << endl;
//    }
    byte b = 0;
    unsigned int index = 0;
    while (file >> noskipws >> b) {
        BIOS[index++] = b;
    }
//    for (const auto& myByte : BIOS) {
//        cout << hex << (int)myByte << endl;
//    }
}

void MMU::setBoot(const bool &toSet) {
    boot = toSet;
}

void MMU::writeDoubleByte(uint16_t &address, const uint16_t &value) {
    if (boot && address < 0x0100) {
        BIOS[++address] = (byte)(value & 0x00FF);
        BIOS[++address] = (byte)((value & 0xFF00) >> 8);
    }
    else if (address < 0x7FFF) {
        ROM[++address] = (byte)(value & 0x00FF);
        ROM[++address] = (byte)((value & 0xFF00) >> 8);
    }
}

bool MMU::getBoot() const {
    return boot;
}
