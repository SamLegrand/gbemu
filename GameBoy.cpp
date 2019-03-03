//
// Created by sam on 21.02.19.
//

#include "GameBoy.h"

void GameBoy::executeFrame() {
    int noops = 0;
    while (cpu.checkCycles()) {
        if (noops >= 5) {
            cout << "Too many noops" << endl;
            assert(false);
        }
        cpu.execute();
        cout << cpu.getPC() << endl;
        byte instruction = mmu->readByte(cpu.getPC());
        if (instruction == 0x00) {
            ++noops;
        }
    }
}
