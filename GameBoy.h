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
    explicit GameBoy(sf::RenderWindow& w) : gpu(new GPU(w)), mmu(new MMU()), cpu(mmu)  {
        mmu->setGPU(gpu);
        gpu->setMMU(mmu);
        skipBoot();
    };
    void executeFrame();
    void skipBoot();

private:
    GPU* gpu;
    MMU* mmu;
    CPU cpu;
};


#endif //GAMEBOYEMULATOR_GAMEBOY_H
