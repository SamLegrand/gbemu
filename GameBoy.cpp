//
// Created by sam on 21.02.19.
//

#include "GameBoy.h"

void GameBoy::skipBoot() {
    cpu.setPC(0x0100);
    cpu.A = 0x01;
    cpu.F = 0xB0;
    cpu.B = 0x00;
    cpu.C = 0x13;
    cpu.D = 0x00;
    cpu.E = 0xD8;
    cpu.H = 0x01;
    cpu.L = 0x4D;
    cpu.SP = 0xFFFE;

    mmu->writeByte(0xFF10, 0x80);
    mmu->writeByte(0xFF11, 0xBF);
    mmu->writeByte(0xFF12, 0xF3);
    mmu->writeByte(0xFF14, 0xBF);
    mmu->writeByte(0xFF16, 0x3F);
    mmu->writeByte(0xFF19, 0xBF);
    mmu->writeByte(0xFF1A, 0x7F);
    mmu->writeByte(0xFF1B, 0xFF);
    mmu->writeByte(0xFF1C, 0x9F);
    mmu->writeByte(0xFF1E, 0xBF);
    mmu->writeByte(0xFF20, 0xFF);
    mmu->writeByte(0xFF23, 0xBF);
    mmu->writeByte(0xFF24, 0x77);
    mmu->writeByte(0xFF25, 0xF3);
    mmu->writeByte(0xFF26, 0xF1); // F0 bij de super gameboy
    mmu->writeByte(0xFF40, 0x91);
    mmu->writeByte(0xFF47, 0xFC);
    mmu->writeByte(0xFF48, 0xFF);
    mmu->writeByte(0xFF49, 0xFF);
}

void GameBoy::executeFrame() {
    int noops = 0;
    unsigned int cyclesBefore = 0;
    unsigned int cyclesAfter = 0;
    while (cpu.checkCycles()) {
        cyclesAfter = cpu.getCycles();
//        if (noops >= 5) {
//            cout << "Too many noops" << endl;
//            assert(false);
//        }
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
