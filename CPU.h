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
    sf::RenderWindow window;
    MMU mmu;
    byte reg[8] = {0};
    uint16_t PC;
    uint16_t SP;

    bool generatesHalfCarry(const byte &value1, const byte &value2);
    bool generatesCarry(const int &value1, const int &value2);
    bool generatesHalfCarry16(const uint16_t &value1, const uint16_t &value2);
    bool generatesCarry16(const int &value1, const int &value2);

public:
    void setFlag(const char &name, const bool &value);
    bool getFlag(const char &name) const;
    CPU() : PC(0), SP(0), window(sf::RenderWindow(sf::VideoMode(640, 576), "GameBoy Emulator")), mmu(window) {};

    // LD16, PUSH, POP
    void LD16();                    // LD SP, HL
    void LD16(const int8_t &value); // LD HL, SP + r8
    void LD16(const std::string &name, const uint16_t &value);
    void PUSH(const std::string &name, MMU &mmu);
    void PUSH(const uint16_t &d16, MMU &mmu);
    void POP(const std::string &name, MMU &mmu);
    uint16_t POP(MMU &mmu);

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
    void CALL(const uint16_t &a16, MMU &mmu);
    void CALL(const std::string &instruction, const uint16_t &a16, MMU &mmu);
    void RET(MMU &mmu);
    void RET(const std::string &instruction, MMU &mmu);
    void RST(const uint16_t &a16, MMU &mmu);

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
    void run();
};


#endif //GAMEBOYEMULATOR_CPU_H
