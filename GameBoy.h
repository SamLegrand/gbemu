//
// Created by sam on 21.02.19.
//

#ifndef GAMEBOYEMULATOR_GAMEBOY_H
#define GAMEBOYEMULATOR_GAMEBOY_H

#include "CPU.h"
#include "GPU.h"
#include "MMU.h"

class GameBoy {
public:
    typedef unsigned char byte;
    explicit GameBoy(sf::RenderWindow& w) : mmu(new MMU(w)), cpu(mmu)  {};
    void executeFrame();

private:
    MMU* mmu;
    CPU cpu;
};


#endif //GAMEBOYEMULATOR_GAMEBOY_H
