//
// Created by sam on 05.10.18.
//

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include "CPU.h"

bool inline CPU::generatesHalfCarry(const CPU::byte &value1, const CPU::byte &value2) {
    return (((value1 & 0x0F) + (value2 & 0x0F) & 0x10) == 0x10);
}

bool inline CPU::generatesCarry(const int &value1, const int &value2) {
    if (value2 > 0) {
        return (value1 + value2 > UINT8_MAX);
    }
    return (value1 + value2 < 0);
}

bool inline CPU::generatesHalfCarry16(const uint16_t &value1, const uint16_t &value2) {
    return (((value1 & 0x0F00) + (value2 & 0x0F00) & 0x1000) == 0x1000);
}

bool inline CPU::generatesCarry16(const int &value1, const int &value2) {
    if (value2 > 0) {
        return (value1 + value2 > UINT16_MAX);
    }
    return (value1 + value2 < 0);
}

bool CPU::getFlag(const char &name) const {
    switch(name) {
        default: std::cout << "Invalid flag name " << name << std::endl; break;
        case 'Z': return (bool)(F & 0x80);
        case 'N': return (bool)(F & 0x40);
        case 'H': return (bool)(F & 0x20);
        case 'C': return (bool)(F & 0x10);
    }
    return false;
}

void CPU::setFlag(const char &name, const bool &value) {
    switch(name) {
        default: std::cout << "Invalid flag name " << name << std::endl; break;
        case 'Z': F ^= (byte)((-value ^ F) & (1 << 7)); break;
        case 'N': F ^= (byte)((-value ^ F) & (1 << 6)); break;
        case 'H': F ^= (byte)((-value ^ F) & (1 << 5)); break;
        case 'C': F ^= (byte)((-value ^ F) & (1 << 4)); break;
    }
}

uint16_t CPU::getRegValue16(const std::string &name) {
    if (name == "BC") { return (B << 8) + C;}
    if (name == "DE") { return (D << 8) + E;}
    if (name == "HL") { return (H << 8) + L;}
    if (name == "HL-") {
        uint16_t value = (H << 8) + L;
        DEC16("HL");
        return value;
    }
    if (name == "HL+") {
        uint16_t value = (H << 8) + L;
        INC16("HL");
        return value;
    }
    if (name == "SP") {
        return CPU::SP;
    }
    std::cout << "Register name " << name << " is invalid" << std::endl;
    return 0;
}

void CPU::JR(const int8_t &r8) {
    PC += r8;
}

void CPU::JR(const std::string &instruction, const int8_t &r8) {
    bool flagvalue = true;
    if (instruction.front() == 'N') flagvalue = false;
    if (getFlag(instruction.back()) == flagvalue) {
        PC += r8;
    }
}

void CPU::JP(const uint16_t &a16) {
    PC = (uint16_t)(a16 - 1);
}

void CPU::JP(const std::string &instruction, const uint16_t &a16) {
    bool flagvalue = true;
    if (instruction.front() == 'N') flagvalue = false;
    if (getFlag(instruction.back()) == flagvalue) {
        PC = (uint16_t)(a16 - 1);
    }
}

void CPU::LD(byte& reg, const CPU::byte &value) {
    reg = value;
}

void CPU::LD16() {
    SP = getRegValue16("HL");
}

void CPU::LD16(const int8_t &value) {
    uint16_t newValue = SP + value;
    LD16("HL", newValue);
    setFlag('Z', false);
    setFlag('N', false);
    setFlag('H', generatesHalfCarry16(SP, (uint16_t)value));
    setFlag('C', generatesCarry16(SP, value));
}

void CPU::LD16(const std::string &name, const uint16_t &value) {
    if (name == "BC") {
        B = (byte)(value >> 8);
        C = (byte)(value);
        return;
    }
    if (name == "DE") {
        D = (byte)(value >> 8);
        E = (byte)(value);
        return;
    }
    if (name == "HL") {
        H = (byte)(value >> 8);
        L = (byte)(value);
        return;
    }
    if (name == "AF") {
        A = (byte)(value >> 8);
        F = (byte)(value);
    }
    if (name == "SP") {
        SP = value;
        return;
    }
}

void CPU::LD(byte& reg1, byte& reg2) {
    reg1 = reg2;
}

void CPU::PUSH(const byte& reg1, const byte& reg2) {
    mmu->writeByte((uint16_t)(SP - 1), reg1);
    mmu->writeByte((uint16_t)(SP - 2), reg2);
    SP -= 2;
}

void CPU::PUSH(const uint16_t &d16) {
    mmu->writeByte((uint16_t)(SP - 1), (byte)(d16 >> 8));
    mmu->writeByte((uint16_t)(SP - 2), (byte)(d16));
    SP -= 2;
}

void CPU::POP(byte& reg1, byte& reg2) {
    LD(reg2, mmu->readByte(SP));
    LD(reg1, mmu->readByte((uint16_t)(SP + 1)));
    SP += 2;
}

void CPU::INC8(byte& reg) {
    byte newValue = (byte)(reg + 1);
    setFlag('Z', newValue == 0);
    setFlag('N', false);
    setFlag('H', (newValue & 0x0F) == 0);
    LD(reg, newValue);
}

void CPU::INC16(const std::string &name) {
    LD16(name, (uint16_t)(getRegValue16(name) + 1));
}

void CPU::DEC8(byte& reg) {
    byte newValue = (byte)(reg - 1);
    setFlag('Z', newValue == 0);
    setFlag('N', true);
    setFlag('H', ((reg ^ newValue ^ 0x01) & 0x10) == 0x10);
    LD(reg, newValue);
}

void CPU::DEC16(const std::string &name) {
    LD16(name, (uint16_t)(getRegValue16(name) - 1));
}

void CPU::SUB(const byte &value) {
    byte oldValue = A;
    byte newValue = (byte)(oldValue - value);
    setFlag('Z', newValue == 0);
    setFlag('N', true);
    setFlag('H', generatesHalfCarry(oldValue, -value));
    setFlag('C', generatesCarry(oldValue, -value));
    LD(A, newValue);
}

void CPU::SBC(const CPU::byte &value) {
    SUB(value);
    LD(A, (byte)(A - getFlag('C')));
}

void CPU::ADD(const CPU::byte &value) {
    byte oldValue = A;
    byte newValue = (byte)(oldValue + value);
    setFlag('Z', newValue == 0);
    setFlag('N', false);
    setFlag('H', generatesHalfCarry(oldValue, value));
    setFlag('C', generatesCarry(oldValue, value));
    LD(A, newValue);
}

void CPU::ADC(const CPU::byte &value) {
    ADD(value);
    LD(A, (byte)(A + getFlag('C')));
}

void CPU::AND(const CPU::byte &value) {
    byte newValue = (byte)(A & value);
    setFlag('Z', newValue == 0);
    setFlag('N', false);
    setFlag('H', true);
    setFlag('C', false);
    LD(A, newValue);
}

void CPU::OR(const CPU::byte &value) {
    byte newValue = (byte)(A | value);
    setFlag('Z', newValue == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', false);
    LD(A, newValue);
}

void CPU::XOR(const CPU::byte &value) {
    byte newValue = (byte)(A ^ value);
    setFlag('Z', newValue == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', false);
    LD(A, newValue);
}

void CPU::CP(const CPU::byte &value) {
    byte oldValue = A;
    byte newValue = (byte)(oldValue - value);
    setFlag('Z', newValue == 0);
    setFlag('N', true);
    setFlag('H', generatesHalfCarry(oldValue, -value));
    setFlag('C', generatesCarry(oldValue, -value));
    LD(A, newValue);
}

void CPU::ADD16(const int8_t &value) {
    uint16_t newValue = SP + value;
    setFlag('Z', false);
    setFlag('N', false);
    setFlag('H', generatesHalfCarry16(SP, (uint16_t)value));
    setFlag('C', generatesCarry16(SP, value));
    SP = newValue;
}

void CPU::ADD16(const std::string &name) {
    uint16_t oldValue = getRegValue16("HL");
    uint16_t value = getRegValue16(name);
    uint16_t newValue = oldValue + value;
    setFlag('N', false);
    setFlag('H', generatesHalfCarry16(SP, value));
    setFlag('C', generatesCarry16(SP, value));
    LD16("HL", newValue);
}

void CPU::RLCA() {
    byte value = A;
    setFlag('C', value >> 7);
    bool C = getFlag('C');
    value <<= 1;
    value ^= (-C ^ value) & 1;  // Set first bit to new C
    LD(A, value);
}

void CPU::RLA() {
    byte value = A;
    bool C = getFlag('C');
    setFlag('C', value >> 7);
    value <<= 1;
    value ^= (-C ^ value) & 1;  // Set first bit to old C
    LD(A, value);
}

void CPU::RRCA() {
    byte value = A;
    setFlag('C', (bool)(value & 1));
    bool C = getFlag('C');
    value >>= 1;
    value ^= (-C ^ value) & (1UL << 7); // Set 7th bit to new C
    LD(A, value);
}

void CPU::RRA() {
    byte value = A;
    bool C = getFlag('C');
    setFlag('C', (bool)(value & 1));
    value >>= 1;
    value ^= (-C ^ value) & (1UL << 7); // Set 7th bit to old C
    LD(A, value);
}

void CPU::CALL(const uint16_t &a16) {
    PUSH(PC);
    PC = (uint16_t)(a16 - 1);
}

void CPU::CALL(const std::string &instruction, const uint16_t &a16) {
    bool flagvalue = true;
    if (instruction.front() == 'N') flagvalue = false;
    if (getFlag(instruction.back()) == flagvalue) {
        CALL (a16);
    }
}

void CPU::RET() {
    PC = (uint16_t)(mmu->readByte((uint16_t)(SP+1)) << 8) | mmu->readByte(SP);
    SP += 2;
}

void CPU::RET(const std::string &instruction) {
    bool flagvalue = true;
    if (instruction.front() == 'N') flagvalue = false;
    if (getFlag(instruction.back()) == flagvalue) {
        RET();
        return;
    }
}

void CPU::SCF() {
    if (A > 99) {

    }
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', true);
}

void CPU::DAA() {
    byte correction = 0;
    byte temp = A;
    if (temp == 0) {
        setFlag('Z', true);
        return;
    }
    if (getFlag('H') || temp > 9) {
        correction += 6;
    }
    if (getFlag('C') || temp > 0x99) {
        correction += 0x60;
        setFlag('C', true);
    }
    temp += getFlag('N') ? -correction : correction;
    LD(A, (byte)(temp & 0xFF));
    setFlag('H', false);
}

void CPU::CPL() {
    setFlag('N', true);
    setFlag('H', true);
}

void CPU::CCF() {
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', !getFlag('C'));
}

void CPU::RST(const uint16_t &a16) {
    PUSH(PC);
    PC = a16;
}

void CPU::RL(byte& reg) {
    byte value = reg;
    byte result = (value << 1) | getFlag('C');
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x80) >> 7);
    LD(reg, result);
}

void CPU::RL() {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = (value << 1) | getFlag('C');
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x80) >> 7);
    mmu->writeByte(address, result);
}

void CPU::execute() {
    byte instruction = mmu->readByte(PC);
//    if (PC >= 0x0100) {
//        cout << "Executing instruction " << hex << (int)instruction << endl;
//    }
    switch (instruction) {
        default: cerr << "Instruction " << hex << (int)instruction << " not recognized." << endl; assert(false); break;
        case 0x00: {break;}    // NOP
        case 0x10: {break;}    // STOP = NOP
        case 0x20: {JR("NZ", mmu->readByte(++PC)); break;}
        case 0x30: {JR("NC", mmu->readByte(++PC)); break;}
        case 0x01: {LD16("BC", mmu->readDoubleByte(PC)); break;}
        case 0x11: {LD16("DE", mmu->readDoubleByte(PC)); break;}
        case 0x21: {LD16("HL", mmu->readDoubleByte(PC)); break;}
        case 0x31: {LD16("SP", mmu->readDoubleByte(PC)); break;}
        case 0x02: {mmu->writeByte(getRegValue16("BC"), A); break;}
        case 0x12: {mmu->writeByte(getRegValue16("DE"), A); break;}
        case 0x22: {mmu->writeByte(getRegValue16("HL+"), A); break;}
        case 0x32: {mmu->writeByte(getRegValue16("HL-"), A); break;}
        case 0x03: {INC16("BC"); break;}
        case 0x13: {INC16("DE"); break;}
        case 0x23: {INC16("HL"); break;}
        case 0x33: {INC16("SP"); break;}
        case 0x04: {INC8(B); break;}
        case 0x14: {INC8(D); break;}
        case 0x24: {INC8(H); break;}
        case 0x34: {mmu->writeByte(getRegValue16("HL"), (byte)(getRegValue16("HL"))); INC16("HL"); break;}
        case 0x05: {DEC8(B); break;}
        case 0x15: {DEC8(D); break;}
        case 0x25: {DEC8(H); break;}
        case 0x35: {mmu->writeByte(getRegValue16("HL"), (byte)(getRegValue16("HL"))); DEC16("HL"); break;}
        case 0x06: {LD(B, mmu->readByte(++PC)); break;}
        case 0x16: {LD(D, mmu->readByte(++PC)); break;}
        case 0x26: {LD(H, mmu->readByte(++PC)); break;}
        case 0x36: {mmu->writeByte(getRegValue16("HL"), mmu->readByte(++PC)); break;}
        case 0x07: {RLCA(); break;}
        case 0x17: {RLA(); break;}
        case 0x27: {DAA(); break;}
        case 0x37: {SCF(); break;}
        case 0x08: {uint16_t a16 = mmu->readDoubleByte(PC); mmu->writeDoubleByte(a16, getRegValue16("SP")); break;}
        case 0x18: {JR(mmu->readByte(++PC)); break;}
        case 0x28: {JR("Z", mmu->readByte(++PC)); break;}
        case 0x38: {JR("C", mmu->readByte(++PC)); break;}
        case 0x09: {ADD16("BC"); break;}
        case 0x19: {ADD16("DE"); break;}
        case 0x29: {ADD16("HL"); break;}
        case 0x39: {ADD16("SP"); break;}
        case 0x0A: {LD(A, mmu->readByte(getRegValue16("BC"))); break;}
        case 0x1A: {LD(A, mmu->readByte(getRegValue16("DE"))); break;}
        case 0x2A: {LD(A, mmu->readByte((byte)(getRegValue16("HL")))); INC16("HL"); break;}
        case 0x3A: {LD(A, mmu->readByte((byte)(getRegValue16("HL")))); DEC16("HL"); break;}
        case 0x0B: {DEC16("BC"); break;}
        case 0x1B: {DEC16("DE"); break;}
        case 0x2B: {DEC16("HL"); break;}
        case 0x3B: {DEC16("SP"); break;}
        case 0x0C: {INC8(C); break;}
        case 0x1C: {INC8(E); break;}
        case 0x2C: {INC8(L); break;}
        case 0x3C: {INC8(A); break;}
        case 0x0D: {DEC8(C); break;}
        case 0x1D: {DEC8(E); break;}
        case 0x2D: {DEC8(L); break;}
        case 0x3D: {DEC8(A); break;}
        case 0x0E: {LD(C, mmu->readByte(++PC)); break;}
        case 0x1E: {LD(E, mmu->readByte(++PC)); break;}
        case 0x2E: {LD(L, mmu->readByte(++PC)); break;}
        case 0x3E: {LD(A, mmu->readByte(++PC)); break;}
        case 0x0F: {RRCA(); break;}
        case 0x1F: {RRA(); break;}
        case 0x2F: {CPL(); break;}
        case 0x3F: {CCF(); break;}
        case 0x40: {LD(B, B); break;}
        case 0x50: {LD(D, B); break;}
        case 0x60: {LD(H, B); break;}
        case 0x70: {mmu->writeByte(mmu->readByte(getRegValue16("HL")), B); break;}
        case 0x41: {LD(B, C); break;}
        case 0x51: {LD(D, C); break;}
        case 0x61: {LD(H, C); break;}
        case 0x71: {mmu->writeByte(getRegValue16("HL"), C); break;}
        case 0x42: {LD(B, D); break;}
        case 0x52: {LD(D, D); break;}
        case 0x62: {LD(H, D); break;}
        case 0x72: {mmu->writeByte(getRegValue16("HL"), D); break;}
        case 0x43: {LD(B, E); break;}
        case 0x53: {LD(D, E); break;}
        case 0x63: {LD(H, E); break;}
        case 0x73: {mmu->writeByte(getRegValue16("HL"), E); break;}
        case 0x44: {LD(B, H); break;}
        case 0x54: {LD(D, H); break;}
        case 0x64: {LD(H, H); break;}
        case 0x74: {mmu->writeByte(getRegValue16("HL"), H); break;}
        case 0x45: {LD(B, L); break;}
        case 0x55: {LD(D, L); break;}
        case 0x65: {LD(H, L); break;}
        case 0x75: {mmu->writeByte(getRegValue16("HL"), L); break;}
        case 0x46: {LD(B, mmu->readByte(getRegValue16("HL"))); break;}
        case 0x56: {LD(D, mmu->readByte(getRegValue16("HL"))); break;}
        case 0x66: {LD(H, mmu->readByte(getRegValue16("HL"))); break;}
        case 0x76: {return;}                // HALT
        case 0x47: {LD(B, A); break;}
        case 0x57: {LD(D, A); break;}
        case 0x67: {LD(H, A); break;}
        case 0x77: {mmu->writeByte(getRegValue16("HL"), A); break;}
        case 0x48: {LD(C, B); break;}
        case 0x58: {LD(E, B); break;}
        case 0x68: {LD(L, B); break;}
        case 0x78: {LD(A, B); break;}
        case 0x49: {LD(C, C); break;}
        case 0x59: {LD(E, C); break;}
        case 0x69: {LD(L, C); break;}
        case 0x79: {LD(A, C); break;}
        case 0x4A: {LD(C, D); break;}
        case 0x5A: {LD(E, D); break;}
        case 0x6A: {LD(L, D); break;}
        case 0x7A: {LD(A, D); break;}
        case 0x4B: {LD(C, E); break;}
        case 0x5B: {LD(E, E); break;}
        case 0x6B: {LD(L, E); break;}
        case 0x7B: {LD(A, E); break;}
        case 0x4C: {LD(C, H); break;}
        case 0x5C: {LD(E, H); break;}
        case 0x6C: {LD(L, H); break;}
        case 0x7C: {LD(A, H); break;}
        case 0x4D: {LD(C, L); break;}
        case 0x5D: {LD(E, L); break;}
        case 0x6D: {LD(L, L); break;}
        case 0x7D: {LD(A, L); break;}
        case 0x4E: {LD(C, mmu->readByte(getRegValue16("HL"))); break;}
        case 0x5E: {LD(E, mmu->readByte(getRegValue16("HL"))); break;}
        case 0x6E: {LD(L, mmu->readByte(getRegValue16("HL"))); break;}
        case 0x7E: {LD(A, mmu->readByte(getRegValue16("HL"))); break;}
        case 0x4F: {LD(C, A); break;}
        case 0x5F: {LD(E, A); break;}
        case 0x6F: {LD(L, A); break;}
        case 0x7F: {LD(A, A); break;}
        case 0x80: {ADD(B); break;}
        case 0x90: {SUB(B); break;}
        case 0xA0: {AND(B); break;}
        case 0xB0: {OR(B); break;}
        case 0x81: {ADD(C); break;}
        case 0x91: {SUB(C); break;}
        case 0xA1: {AND(C); break;}
        case 0xB1: {OR(C); break;}
        case 0x82: {ADD(D); break;}
        case 0x92: {SUB(D); break;}
        case 0xA2: {AND(D); break;}
        case 0xB2: {OR(D); break;}
        case 0x83: {ADD(E); break;}
        case 0x93: {SUB(E); break;}
        case 0xA3: {AND(E); break;}
        case 0xB3: {OR(E); break;}
        case 0x84: {ADD(H); break;}
        case 0x94: {SUB(H); break;}
        case 0xA4: {AND(H); break;}
        case 0xB4: {OR(H); break;}
        case 0x85: {ADD(L); break;}
        case 0x95: {SUB(L); break;}
        case 0xA5: {AND(L); break;}
        case 0xB5: {OR(L); break;}
        case 0x86: {ADD(mmu->readByte(getRegValue16("HL"))); break;}
        case 0x96: {SUB(mmu->readByte(getRegValue16("HL"))); break;}
        case 0xA6: {AND(mmu->readByte(getRegValue16("HL"))); break;}
        case 0xB6: {OR(mmu->readByte(getRegValue16("HL"))); break;}
        case 0x87: {ADD(A); break;}
        case 0x97: {SUB(A); break;}
        case 0xA7: {AND(A); break;}
        case 0xB7: {OR(A); break;}
        case 0x88: {ADC(B); break;}
        case 0x98: {SBC(B); break;}
        case 0xA8: {XOR(B); break;}
        case 0xB8: {CP(B); break;}
        case 0x89: {ADC(C); break;}
        case 0x99: {SBC(C); break;}
        case 0xA9: {XOR(C); break;}
        case 0xB9: {CP(C); break;}
        case 0x8A: {ADC(D); break;}
        case 0x9A: {SBC(D); break;}
        case 0xAA: {XOR(D); break;}
        case 0xBA: {CP(D); break;}
        case 0x8B: {ADC(E); break;}
        case 0x9B: {SBC(E); break;}
        case 0xAB: {XOR(E); break;}
        case 0xBB: {CP(E); break;}
        case 0x8C: {ADC(H); break;}
        case 0x9C: {SBC(H); break;}
        case 0xAC: {XOR(H); break;}
        case 0xBC: {CP(H); break;}
        case 0x8D: {ADC(L); break;}
        case 0x9D: {SBC(L); break;}
        case 0xAD: {XOR(L); break;}
        case 0xBD: {CP(L); break;}
        case 0x8E: {ADC(mmu->readByte(getRegValue16("HL"))); break;}
        case 0x9E: {SBC(mmu->readByte(getRegValue16("HL"))); break;}
        case 0xAE: {XOR(mmu->readByte(getRegValue16("HL"))); break;}
        case 0xBE: {CP(mmu->readByte(getRegValue16("HL"))); break;}
        case 0x8F: {ADC(A); break;}
        case 0x9F: {SBC(A); break;}
        case 0xAF: {XOR(A); break;}
        case 0xBF: {CP(A); break;}
        case 0xC0: {RET("NZ"); break;}
        case 0xD0: {RET("NC"); break;}
        case 0xE0: {mmu->writeByte((uint16_t)(0xFF00 + mmu->readByte(++PC)), A); break;}
        case 0xF0: {LD(A, mmu->readByte((uint16_t)(0xFF00 + mmu->readByte(++PC)))); break;}
        case 0xC1: {POP(B, C); break;}
        case 0xD1: {POP(D, E); break;}
        case 0xE1: {POP(H, L); break;}
        case 0xF1: {POP(A, F); break;}
        case 0xC2: {JP("NZ", mmu->readDoubleByte(PC)); break;}
        case 0xD2: {JP("NC", mmu->readDoubleByte(PC)); break;}
        case 0xE2: {mmu->writeByte((uint16_t)(0xFF00 + C), A); break;}
        case 0xF2: {LD(A, mmu->readByte((uint16_t)(0xFF00 + C))); break;}
        case 0xC3: {JP(mmu->readDoubleByte(PC)); break;}
        case 0xF3: {break;} // Disable interrupts
        case 0xC4: {CALL("NZ", mmu->readDoubleByte(PC)); break;}
        case 0xD4: {CALL("NC", mmu->readDoubleByte(PC)); break;}
        case 0xC5: {PUSH(B, C); break;}
        case 0xD5: {PUSH(D, E); break;}
        case 0xE5: {PUSH(H, L); break;}
        case 0xF5: {PUSH(A, F); break;}
        case 0xC6: {ADD(mmu->readByte(++PC)); break;}
        case 0xD6: {SUB(mmu->readByte(++PC)); break;}
        case 0xE6: {AND(mmu->readByte(++PC)); break;}
        case 0xF6: {OR(mmu->readByte(++PC)); break;}
        case 0xC7: {RST(0x00); break;}
        case 0xD7: {RST(0x10); break;}
        case 0xE7: {RST(0x20); break;}
        case 0xF7: {RST(0x30); break;}
        case 0xC8: {RET("Z"); break;}
        case 0xD8: {RET("C"); break;}
        case 0xE8: {ADD16(mmu->readByte(++PC)); break;}
        case 0xF8: {LD16("HL", getRegValue16("SP") + mmu->readByte(++PC)); break;}
        case 0xC9: {RET(); break;}
        case 0xD9: {RET(); break;}   // Enable interrupts
        case 0xE9: {uint16_t a16 = getRegValue16("HL"); JP(mmu->readDoubleByte(a16)); break;}
        case 0xF9: {LD16(); break;}
        case 0xCA: {JP("Z", mmu->readDoubleByte(PC)); break;}
        case 0xDA: {JP("C", mmu->readDoubleByte(PC)); break;}
        case 0xEA: {mmu->writeByte(mmu->readDoubleByte(PC), A); break;}
        case 0xFA: {LD(A, mmu->readByte(mmu->readDoubleByte(PC))); break;}
        case 0xCB: {PREFIX_CB(); break;}     // PREFIX CB
        case 0xFB: {break;}     // Enable interrupts
        case 0xCC: {CALL("Z", mmu->readDoubleByte(PC)); break;}
        case 0xDC: {CALL("C", mmu->readDoubleByte(PC)); break;}
        case 0xCD: {CALL(mmu->readDoubleByte(PC)); break;}
        case 0xCE: {ADC(mmu->readByte(++PC)); break;}
        case 0xDE: {SBC(mmu->readByte(++PC)); break;}
        case 0xEE: {XOR(mmu->readByte(++PC)); break;}
        case 0xFE: {CP(mmu->readByte(++PC)); break;}
        case 0xCF: {RST(0x8); break;}
        case 0xDF: {RST(0x18); break;}
        case 0xEF: {RST(0x28); break;}
        case 0xFF: {RST(0x38); break;}
    }
    ++PC;
    if (i == 7) {
        cout << hex << 0xFF00 + (int)mmu->readByte(PC - 1) << endl;
        cout << "Executing instruction " << hex << (int)instruction << endl;
    }
    if (PC > 0x0100 && i <= 10000) {
        f << hex << "PC: " << setfill('0') << setw(4) << PC << "  ";
        f << "A: " << setfill('0') << setw(2) <<  (int)A << "  ";
        f << "B: " << setfill('0') << setw(2) << (int)B << "  ";
        f << "C: " << setfill('0') << setw(2) << (int)C << "  ";
        f << "D: " << setfill('0') << setw(2) << (int)D << "  ";
        f << "E: " << setfill('0') << setw(2) << (int)E << "  ";
        f << "H: " << setfill('0') << setw(2) << (int)H << "  ";
        f << "L: " << setfill('0') << setw(2) << (int)L << "  ";
//        cout << "~Z: " << (int)getFlag('Z') << " ";
//        cout << "~N: " << (int)getFlag('N') << " ";
//        cout << "~H: " << (int)getFlag('H') << " ";
//        cout << "~C: " << (int)getFlag('C') << " ";
        f << "SP: " << SP << "  ";
        f << "F: " << setfill('0') << setw(2) << (int)F << "   " << dec << this->i << endl;
        ++this->i;
    }
//    if ((int) PC >= 0x0100 ) {
//        this_thread::sleep_for(1s);
//    }
    if (PC == 0x0100) {
        mmu->setBoot(false);
//            cout << "DID IT" << endl;
        return;
    }
    if (i == 10001) {
        f.close();
        Utils::compareFiles();
        assert(false);
        i++;
    }
}

void CPU::PREFIX_CB() {
    ++PC;
    byte instruction = mmu->readByte(PC);
//    cout << "Executing P_CB instruction " << hex << (int)instruction << endl;
    switch(instruction) {
        default: cerr << "Instruction " << hex << (int)instruction << " not recognized." << endl; assert(false); break;
        case 0x00: RLC(B); break;
        case 0x01: RLC(C); break;
        case 0x02: RLC(D); break;
        case 0x03: RLC(E); break;
        case 0x04: RLC(H); break;
        case 0x05: RLC(L); break;
        case 0x06: RLC(); break;
        case 0x07: RLC(A); break;
        case 0x08: RRC(B); break;
        case 0x09: RRC(C); break;
        case 0x0A: RRC(D); break;
        case 0x0B: RRC(E); break;
        case 0x0C: RRC(H); break;
        case 0x0D: RRC(L); break;
        case 0x0E: RRC(); break;
        case 0x0F: RRC(A); break;
        case 0x10: RL(B); break;
        case 0x11: RL(C); break;
        case 0x12: RL(D); break;
        case 0x13: RL(E); break;
        case 0x14: RL(H); break;
        case 0x15: RL(L); break;
        case 0x16: RL(); break;
        case 0x17: RL(A); break;
        case 0x18: RR(B); break;
        case 0x19: RR(C); break;
        case 0x1A: RR(D); break;
        case 0x1B: RR(E); break;
        case 0x1C: RR(H); break;
        case 0x1D: RR(L); break;
        case 0x1E: RR(); break;
        case 0x1F: RR(A); break;
        case 0x20: SLA(B); break;
        case 0x21: SLA(C); break;
        case 0x22: SLA(D); break;
        case 0x23: SLA(E); break;
        case 0x24: SLA(H); break;
        case 0x25: SLA(L); break;
        case 0x26: SLA(); break;
        case 0x27: SLA(A); break;
        case 0x28: SRA(B); break;
        case 0x29: SRA(C); break;
        case 0x2A: SRA(D); break;
        case 0x2B: SRA(E); break;
        case 0x2C: SRA(H); break;
        case 0x2D: SRA(L); break;
        case 0x2E: SRA(); break;
        case 0x2F: SRA(A); break;
        case 0x30: SWAP(B); break;
        case 0x31: SWAP(C); break;
        case 0x32: SWAP(D); break;
        case 0x33: SWAP(E); break;
        case 0x34: SWAP(H); break;
        case 0x35: SWAP(L); break;
        case 0x36: SWAP(); break;
        case 0x37: SWAP(A); break;
        case 0x38: SRL(B); break;
        case 0x39: SRL(C); break;
        case 0x3A: SRL(D); break;
        case 0x3B: SRL(E); break;
        case 0x3C: SRL(H); break;
        case 0x3D: SRL(L); break;
        case 0x3E: SRL(); break;
        case 0x3F: SRL(A); break;
        case 0x40: BIT(0, B); break;
        case 0x41: BIT(0, C); break;
        case 0x42: BIT(0, D); break;
        case 0x43: BIT(0, E); break;
        case 0x44: BIT(0, H); break;
        case 0x45: BIT(0, L); break;
        case 0x46: BIT(0); break;
        case 0x47: BIT(0, A); break;
        case 0x48: BIT(1, B); break;
        case 0x49: BIT(1, C); break;
        case 0x4A: BIT(1, D); break;
        case 0x4B: BIT(1, E); break;
        case 0x4C: BIT(1, H); break;
        case 0x4D: BIT(1, L); break;
        case 0x4E: BIT(1); break;
        case 0x4F: BIT(1, A); break;
        case 0x50: BIT(2, B); break;
        case 0x51: BIT(2, C); break;
        case 0x52: BIT(2, D); break;
        case 0x53: BIT(2, E); break;
        case 0x54: BIT(2, H); break;
        case 0x55: BIT(2, L); break;
        case 0x56: BIT(2); break;
        case 0x57: BIT(2, A); break;
        case 0x58: BIT(3, B); break;
        case 0x59: BIT(3, C); break;
        case 0x5A: BIT(3, D); break;
        case 0x5B: BIT(3, E); break;
        case 0x5C: BIT(3, H); break;
        case 0x5D: BIT(3, L); break;
        case 0x5E: BIT(3); break;
        case 0x5F: BIT(3, A); break;
        case 0x60: BIT(4, B); break;
        case 0x61: BIT(4, C); break;
        case 0x62: BIT(4, D); break;
        case 0x63: BIT(4, E); break;
        case 0x64: BIT(4, H); break;
        case 0x65: BIT(4, L); break;
        case 0x66: BIT(4); break;
        case 0x67: BIT(4, A); break;
        case 0x68: BIT(5, B); break;
        case 0x69: BIT(5, C); break;
        case 0x6A: BIT(5, D); break;
        case 0x6B: BIT(5, E); break;
        case 0x6C: BIT(5, H); break;
        case 0x6D: BIT(5, L); break;
        case 0x6E: BIT(5); break;
        case 0x6F: BIT(5, A); break;
        case 0x70: BIT(6, B); break;
        case 0x71: BIT(6, C); break;
        case 0x72: BIT(6, D); break;
        case 0x73: BIT(6, E); break;
        case 0x74: BIT(6, H); break;
        case 0x75: BIT(6, L); break;
        case 0x76: BIT(6); break;
        case 0x77: BIT(6, A); break;
        case 0x78: BIT(7, B); break;
        case 0x79: BIT(7, C); break;
        case 0x7A: BIT(7, D); break;
        case 0x7B: BIT(7, E); break;
        case 0x7C: BIT(7, H); break;
        case 0x7D: BIT(7, L); break;
        case 0x7E: BIT(7); break;
        case 0x7F: BIT(7, A); break;
        case 0x80: RES(0, B); break;
        case 0x81: RES(0, C); break;
        case 0x82: RES(0, D); break;
        case 0x83: RES(0, E); break;
        case 0x84: RES(0, H); break;
        case 0x85: RES(0, L); break;
        case 0x86: RES(0); break;
        case 0x87: RES(0, A); break;
        case 0x88: RES(1, B); break;
        case 0x89: RES(1, C); break;
        case 0x8A: RES(1, D); break;
        case 0x8B: RES(1, E); break;
        case 0x8C: RES(1, H); break;
        case 0x8D: RES(1, L); break;
        case 0x8E: RES(1); break;
        case 0x8F: RES(1, A); break;
        case 0x90: RES(2, B); break;
        case 0x91: RES(2, C); break;
        case 0x92: RES(2, D); break;
        case 0x93: RES(2, E); break;
        case 0x94: RES(2, H); break;
        case 0x95: RES(2, L); break;
        case 0x96: RES(2); break;
        case 0x97: RES(2, A); break;
        case 0x98: RES(3, B); break;
        case 0x99: RES(3, C); break;
        case 0x9A: RES(3, D); break;
        case 0x9B: RES(3, E); break;
        case 0x9C: RES(3, H); break;
        case 0x9D: RES(3, L); break;
        case 0x9E: RES(3); break;
        case 0x9F: RES(3, A); break;
        case 0xA0: RES(4, B); break;
        case 0xA1: RES(4, C); break;
        case 0xA2: RES(4, D); break;
        case 0xA3: RES(4, E); break;
        case 0xA4: RES(4, H); break;
        case 0xA5: RES(4, L); break;
        case 0xA6: RES(4); break;
        case 0xA7: RES(4, A); break;
        case 0xA8: RES(5, B); break;
        case 0xA9: RES(5, C); break;
        case 0xAA: RES(5, D); break;
        case 0xAB: RES(5, E); break;
        case 0xAC: RES(5, H); break;
        case 0xAD: RES(5, L); break;
        case 0xAE: RES(5); break;
        case 0xAF: RES(5, A); break;
        case 0xB0: RES(6, B); break;
        case 0xB1: RES(6, C); break;
        case 0xB2: RES(6, D); break;
        case 0xB3: RES(6, E); break;
        case 0xB4: RES(6, H); break;
        case 0xB5: RES(6, L); break;
        case 0xB6: RES(6); break;
        case 0xB7: RES(6, A); break;
        case 0xB8: RES(7, B); break;
        case 0xB9: RES(7, C); break;
        case 0xBA: RES(7, D); break;
        case 0xBB: RES(7, E); break;
        case 0xBC: RES(7, H); break;
        case 0xBD: RES(7, L); break;
        case 0xBE: RES(7); break;
        case 0xBF: RES(7, A); break;
        case 0xC0: RES(0, B); break;
        case 0xC1: RES(0, C); break;
        case 0xC2: RES(0, D); break;
        case 0xC3: RES(0, E); break;
        case 0xC4: RES(0, H); break;
        case 0xC5: RES(0, L); break;
        case 0xC6: RES(0); break;
        case 0xC7: RES(0, A); break;
        case 0xC8: RES(1, B); break;
        case 0xC9: RES(1, C); break;
        case 0xCA: RES(1, D); break;
        case 0xCB: RES(1, E); break;
        case 0xCC: RES(1, H); break;
        case 0xCD: RES(1, L); break;
        case 0xCE: RES(1); break;
        case 0xCF: RES(1, A); break;
        case 0xD0: RES(2, B); break;
        case 0xD1: RES(2, C); break;
        case 0xD2: RES(2, D); break;
        case 0xD3: RES(2, E); break;
        case 0xD4: RES(2, H); break;
        case 0xD5: RES(2, L); break;
        case 0xD6: RES(2); break;
        case 0xD7: RES(2, A); break;
        case 0xD8: RES(3, B); break;
        case 0xD9: RES(3, C); break;
        case 0xDA: RES(3, D); break;
        case 0xDB: RES(3, E); break;
        case 0xDC: RES(3, H); break;
        case 0xDD: RES(3, L); break;
        case 0xDE: RES(3); break;
        case 0xDF: RES(3, A); break;
        case 0xE0: RES(4, B); break;
        case 0xE1: RES(4, C); break;
        case 0xE2: RES(4, D); break;
        case 0xE3: RES(4, E); break;
        case 0xE4: RES(4, H); break;
        case 0xE5: RES(4, L); break;
        case 0xE6: RES(4); break;
        case 0xE7: RES(4, A); break;
        case 0xE8: RES(5, B); break;
        case 0xE9: RES(5, C); break;
        case 0xEA: RES(5, D); break;
        case 0xEB: RES(5, E); break;
        case 0xEC: RES(5, H); break;
        case 0xED: RES(5, L); break;
        case 0xEE: RES(5); break;
        case 0xEF: RES(5, A); break;
        case 0xF0: RES(6, B); break;
        case 0xF1: RES(6, C); break;
        case 0xF2: RES(6, D); break;
        case 0xF3: RES(6, E); break;
        case 0xF4: RES(6, H); break;
        case 0xF5: RES(6, L); break;
        case 0xF6: RES(6); break;
        case 0xF7: RES(6, A); break;
        case 0xF8: RES(7, B); break;
        case 0xF9: RES(7, C); break;
        case 0xFA: RES(7, D); break;
        case 0xFB: RES(7, E); break;
        case 0xFC: RES(7, H); break;
        case 0xFD: RES(7, L); break;
        case 0xFE: RES(7); break;
        case 0xFF: RES(7, A); break;
    }
}

void CPU::BIT(const byte& value, const byte& reg) {
    setFlag('Z', ((reg >> value) & 0x01) == 0);
    setFlag('N', false);
    setFlag('H', true);
}

void CPU::BIT(const byte& bit) {
    setFlag('Z', ((mmu->readByte(getRegValue16("HL")) >> bit) & 0x01) == 0);
    setFlag('N', false);
    setFlag('H', true);
}

void CPU::RLC(byte& reg) {
    byte value = reg;
    byte result = (byte)((value << 1) | ((value & 0x80) >> 7)); // Receives bit 7 as first bit
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x80) >> 7);
    LD(reg, result);
}

void CPU::RLC() {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = (byte)((value << 1) | ((value & 0x80) >> 7)); // Receives bit 7 as first bit
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x80) >> 7);
    mmu->writeByte(address, result);
}

void CPU::RR(byte& reg) {
    byte value = reg;
    byte result = (value >> 1) | (getFlag('C') << 7);
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x01));
    LD(reg, result);
}

void CPU::RR() {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = (value >> 1) | (getFlag('C') << 7);
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x01));
    mmu->writeByte(address, result);
}

void CPU::RRC(byte& reg) {
    byte value = reg;
    byte result = (byte)((value >> 1) | (value << 7));
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x01));
    LD(reg, result);
}

void CPU::RRC() {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = (byte)((value >> 1) | (value << 7));
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x01));
    mmu->writeByte(address, result);
}

void CPU::SRA(byte& reg) {
    byte value = reg;
    byte result = value >> 1 | (byte)(value & 0x80);
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x01));
    LD(reg, result);
}

void CPU::SRA() {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = value >> 1 | (byte)(value & 0x80);
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x01));
    mmu->writeByte(address, result);
}

void CPU::SLA(byte& reg) {
    byte value = reg;
    byte result = value << 1 | (byte)(value & 0x01);
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x80) >> 7);
    LD(reg, result);
}

void CPU::SLA() {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = value << 1 | (byte)(value & 0x01);
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x80) >> 7);
    mmu->writeByte(address, result);
}

void CPU::SWAP(byte& reg) {
    byte value = reg;
    byte result = (byte)(((value & 0xF0) >> 4) | ((value & 0x0F) << 4));
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', false);
    LD(reg, result);
}

void CPU::SWAP() {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = (byte)(((value & 0xF0) >> 4) | ((value & 0x0F) << 4));
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', false);
    mmu->writeByte(address, result);
}

void CPU::SRL(byte& reg) {
    byte value = reg;
    byte result = value >> 1;
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x01));
    LD(reg, result);
}

void CPU::SRL() {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = value >> 1;
    setFlag('Z', result == 0);
    setFlag('N', false);
    setFlag('H', false);
    setFlag('C', (byte)(value & 0x01));
    mmu->writeByte(address, result);
}

void CPU::RES(const CPU::byte &value, byte& reg) {
    byte regValue = reg;
    byte result = (byte)(regValue & ~(0x01 << value));
    LD(reg, result);
}

void CPU::RES(const CPU::byte &bit) {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = (byte)(value & ~(0x01 << bit));
    mmu->writeByte(address, result);
}

void CPU::SET(const CPU::byte &value, byte& reg) {
    byte regValue = reg;
    byte result = (byte)(regValue | 0x01 << value);
    LD(reg, result);
}

void CPU::SET(const CPU::byte &bit) {
    uint16_t address = getRegValue16("HL");
    byte value = mmu->readByte(address);
    byte result = (byte)(value & 0x01 << bit);
    mmu->writeByte(address, result);
}

uint16_t CPU::getPC() const {
    return PC;
}

void CPU::setPC(uint16_t PC) {
    CPU::PC = PC;
}

bool CPU::checkCycles() {
    if (currCycles >= 70224) {
        currCycles -= 70224;
        return false;
    }
    byte instruction = mmu->readByte(PC);
    currCycles += cycles[instruction];
    return true;
}

unsigned int CPU::getCycles() {
    return currCycles;
}
