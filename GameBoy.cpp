//
// Created by sam on 21.02.19.
//

#include "GameBoy.h"

void GameBoy::executeFrame() {
    cpu.execute();
    cout << cpu.getPC() << endl;
}
