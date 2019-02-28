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

class CPU {
    typedef unsigned char byte;
private:
    unsigned int currCycles = 0;
    byte reg[8] = {0};
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
    explicit CPU(MMU* mmuptr) : PC(0), SP(0), mmu(mmuptr) {};

    // LD16, PUSH, POP
    void LD16();                    // LD SP, HL
    void LD16(const int8_t &value); // LD HL, SP + r8
    void LD16(const std::string &name, const uint16_t &value);
    void PUSH(const std::string &name);
    void PUSH(const uint16_t &d16);
    void POP(const std::string &name);
    uint16_t POP();

    // LD
    void LD(const std::string &name, const byte &value);
    void LD(const std::string &name1, const std::string &name2);

    // Arithmetic (16-bit)
    void INC16(const std::string &name);
    void DEC16(const std::string &name);
    void ADD16(const int8_t &value);        // ADD SP, r8
    void ADD16(const std::string &name);    // ADD HL, ..

    // Arithmetic (8-bit)
    void INC8(const std::string &name);
    void DEC8(const std::string &name);
    void SUB(const byte &value);
    void SUB(const std::string &name);
    void SBC(const byte &value);
    void SBC(const std::string &name);
    void ADD(const byte &value);
    void ADD(const std::string &name);
    void ADC(const byte &value);
    void ADC(const std::string &name);
    void AND(const byte &value);
    void AND(const std::string &name);
    void OR(const byte &value);
    void OR(const std::string &name);
    void XOR(const byte &value);
    void XOR(const std::string &name);
    void CP(const CPU::byte &value);
    void CP(const std::string &name);
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
    void BIT(const byte& value, const std::string& name);
    void RL();
    void RL(const std::string& name);
    void RLC();
    void RLC(const std::string& name);
    void RR();
    void RR(const std::string& name);
    void RRC();
    void RRC(const std::string& name);
    void SLA();
    void SLA(const std::string& name);
    void SRA();
    void SRA(const std::string& name);
    void SWAP();
    void SWAP(const std::string& name);
    void SRL();
    void SRL(const std::string& name);
    void RES(const byte& bit);
    void RES(const byte& value, const std::string& name);
    void SET(const byte& bit);
    void SET(const byte& value, const std::string& name);

    byte getRegValue(const std::string &name) const;
    uint16_t getRegValue16(const std::string &name);
//    void setReg(std::string name, byte value);
    void execute();

    uint16_t getPC() const;

    void setPC(uint16_t PC);

    bool checkCycles();
};


#endif //GAMEBOYEMULATOR_CPU_H
