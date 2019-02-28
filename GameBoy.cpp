//
// Created by sam on 21.02.19.
//

#include "GameBoy.h"

void GameBoy::executeFrame() {
    while (cpu.checkCycles()) {
        cpu.execute();
        cout << cpu.getPC() << endl;
    }
}
