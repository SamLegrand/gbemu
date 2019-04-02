//
// Created by sam on 05.10.18.
//

#ifndef GAMEBOYEMULATOR_CPU_H
#define GAMEBOYEMULATOR_CPU_H


#include <cstdint>
#include <map>
#include <iostream>
#include <thread>
#include "MMU.h"
#include <assert.h>
#include "Utils.h"
#include  <iomanip>

class CPU {
    typedef unsigned char byte;
private:
    ofstream f;
    unsigned int currCycles = 0;
    unsigned int i = 1;
    byte A, B, C, D, E, F, H, L;
    uint16_t PC;
    uint16_t SP;
    MMU* mmu;
    uint16_t e = 0;
    //                 X0  X1  X2  X3  X4  X5  X6  X7  X8  X9  XA  XB  XC  XD  XE  XF
    uint16_t cycles[256] = { 4, 12,  8,  8,  4,  4,  8,  4, 20,  8,  8,  8,  4,  4,  8,  4, // 0X
                        0, 12,  8,  8,  4,  4,  8,  4, 12,  8,  8,  8,  4,  4,  8,  4, // 1X
                        0, 12,  8,  8,  4,  4,  8,  4,  0,  8,  8,  8,  4,  4,  8,  4, // 2X
                        0, 12,  8,  8, 12, 12, 12,  4,  0,  8,  8,  8,  4,  4,  8,  4, // 3X
                        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, // 4X
                        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, // 5X
                        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, // 6X
                        8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4, // 7X
                        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, // 8X
                        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, // 9X
                        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, // AX
                        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4, // BX
                        0, 12,  0, 16,  0, 16,  8, 16,  0, 16,  0,  4,  0, 24,  8, 16, // CX
                        0, 12,  0,  e,  0, 16,  8, 16,  0, 16,  0,  e,  0,  e,  8, 16, // DX
                        12, 12, 8,  e,  e, 16,  8, 16, 16,  4, 16,  e,  e,  e,  8, 16, // EX
                        12, 12, 8,  4,  e, 16,  8, 16, 12,  8, 16,  4,  e,  e,  8, 16};// FX

    bool generatesHalfCarry(const byte &value1, const byte &value2);
    bool generatesCarry(const int &value1, const int &value2);
    bool generatesHalfCarry16(const uint16_t &value1, const uint16_t &value2);
    bool generatesCarry16(const int &value1, const int &value2);

public:
    void setFlag(const char &name, const bool &value);
    bool getFlag(const char &name) const;
    explicit CPU(MMU* mmuptr) : PC(0), SP(0), mmu(mmuptr) {A = B = C = D = E = F = H = L = 0; f.open("../output.txt");};

    // LD16, PUSH, POP
    void LD16();                    // LD SP, HL
    void LD16(const int8_t &value); // LD HL, SP + r8
    void LD16(const std::string &name, const uint16_t &value);
    void PUSH(const byte& reg1, const byte& reg2);
    void PUSH(const uint16_t &d16);
    void POP(byte& reg1, byte& reg2);

    // LD
    void LD(byte& reg, const byte &value);
    void LD(byte& reg1 , byte& reg2);

    // Arithmetic (16-bit)
    void INC16(const std::string &name);
    void DEC16(const std::string &name);
    void ADD16(const int8_t &value);        // ADD SP, r8
    void ADD16(const std::string &name);    // ADD HL, ..

    // Arithmetic (8-bit)
    void INC8(byte& reg);
    void DEC8(byte& reg);
    void SUB(const byte &value);
    void SBC(const byte &value);
    void ADD(const byte &value);
    void ADC(const byte &value);
    void AND(const byte &value);
    void OR(const byte &value);
    void XOR(const byte &value);
    void CP(const CPU::byte &value);
    void DAA(); // Look into this...
    void SCF();
    void CPL();
    void CCF();

    // JR, JP, RET, ...
    void JR(const int8_t &r8);
    void JR(const std::string &instruction, const int8_t &r8);
    void JP(const uint16_t &a16);
    void JP(const std::string &instruction, const uint16_t &a16);
    void CALL(const uint16_t &a16);
    void CALL(const std::string &instruction, const uint16_t &a16);
    void RET();
    void RET(const std::string &instruction);
    void RST(const uint16_t &a16);

    // Bit-wise operations
    void RLCA();
    void RLA();
    void RRCA();
    void RRA();

    // PREFIX CB OPS
    void PREFIX_CB();
    void BIT(const byte& bit);
    void BIT(const byte& value, const byte& reg);
    void RL();
    void RL(byte& reg);
    void RLC();
    void RLC(byte& reg);
    void RR();
    void RR(byte& reg);
    void RRC();
    void RRC(byte& reg);
    void SLA();
    void SLA(byte& reg);
    void SRA();
    void SRA(byte& reg);
    void SWAP();
    void SWAP(byte& reg);
    void SRL();
    void SRL(byte& reg);
    void RES(const byte& bit);
    void RES(const byte& value, byte& reg);
    void SET(const byte& bit);
    void SET(const byte& value, byte& reg);

    uint16_t getRegValue16(const std::string &name);
//    void setReg(std::string name, byte value);
    void execute();

    uint16_t getPC() const;

    void setPC(uint16_t PC);

    bool checkCycles();

    unsigned int getCycles();
};


#endif //GAMEBOYEMULATOR_CPU_H
