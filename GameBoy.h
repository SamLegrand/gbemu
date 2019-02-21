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
    explicit GameBoy(sf::RenderWindow& w) : cpu(&mmu), mmu(w) {};
    void executeFrame();

private:
    CPU cpu;
    MMU mmu;
};


#endif //GAMEBOYEMULATOR_GAMEBOY_H
