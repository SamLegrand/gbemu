//
// Created by sam on 21.02.19.
//

#include "GameBoy.h"

void GameBoy::executeFrame() {
    int noops = 0;
    unsigned int cyclesBefore = 0;
    unsigned int cyclesAfter = 0;
    while (cpu.checkCycles()) {
        cyclesAfter = cpu.getCycles();
        if (noops >= 5) {
            cout << "Too many noops" << endl;
            assert(false);
        }
        cpu.execute();
        unsigned int cycles = cyclesAfter - cyclesBefore;
        gpu->addCycles(cycles);
        gpu->checkCycles();
//        cout << "PC: " << hex << cpu.getPC() << endl;
        byte instruction = mmu->readByte(cpu.getPC());
        if (instruction == 0x00) {
            ++noops;
        }
        cyclesBefore = cpu.getCycles();
    }
}
