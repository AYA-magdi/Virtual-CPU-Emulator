#ifndef VOLE_MACHINE_H
#define VOLE_MACHINE_H

#include <iostream>
#include <vector>
#include <bitset>
#include <fstream>
#include <string>
#include <iomanip>

// Forward declarations of all classes
class Memory;
class RegisterBank;
class ALU;
class CU;
class CPU;
class VOLEMachine;

// Memory Class
class Memory
{
public:
    Memory(int size);
    int get(int address) const;
    void set(int address, int value);
    bool loadProgramFromFile(const std::string &filename);
    void printMemory() const;

private:
    std::vector<int> memory;
};

// Register Class
class RegisterBank
{
public:
    RegisterBank(int numRegisters);
    int get(int reg) const;
    void set(int reg, int value);

    int &getRef(int reg);

    void printRegisters() const;

private:
    std::vector<int> registers;
};

// ALU Class
class ALU
{
public:
    int add(int a, int b) const;
    int bitwiseOr(int a, int b) const;
    int bitwiseAnd(int a, int b) const;
    int bitwiseXor(int a, int b) const;
    void rotate(int &reg, int x) const;
};

// CU Class
class CU
{
public:
    int fetchInstruction(int pc, const Memory &memory) const;
    void decodeAndExecute(int instruction, int &pc, RegisterBank &registers, Memory &memory, ALU &alu, bool &halted);
    void printFinalState(int finalPC, const RegisterBank &registers, const Memory &memory) const;
};

// CPU Class
class CPU
{
public:
    CPU(int memorySize, int numRegisters);
    void loadProgram(const std::string &filename);
    void run();

private:
    Memory memory;
    RegisterBank registers;
    ALU alu;
    CU controlUnit;
    int pc;
    bool halted;
};

// VOLEMachine Class
class VOLEMachine
{
public:
    VOLEMachine(int memorySize, int numRegisters);
    bool loadProgramFromFile(const std::string &filename);
    void run();
    void executeInstruction(int instruction, int opcode, int r, int s, int t); // New function for explaining instructions
private:
    CPU cpu;
};

#endif
