#include "vOLE_machine.h"

// Memory Class Implementation
Memory::Memory(int size) : memory(size, 0) {}

int Memory::get(int address) const
{
    return memory[address];
}

void Memory::set(int address, int value)
{
    memory[address] = value;
}

bool Memory::loadProgramFromFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    int address = 0;
    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
        if (line.length() != 4)
        {
            std::cerr << "Invalid instruction length: '" << line << "'. Expected 4 hex digits." << std::endl;
            return false;
        }

        try
        {
            int byte1 = std::stoi(line.substr(0, 2), nullptr, 16);
            int byte2 = std::stoi(line.substr(2, 2), nullptr, 16);
            memory[address++] = byte1;
            memory[address++] = byte2;
        }
        catch (...)
        {
            std::cerr << "Invalid instruction in file: '" << line << "'. Expected a hexadecimal number." << std::endl;
            return false;
        }
    }

    file.close();
    return true;
}

void Memory::printMemory() const
{
    std::cout << "Memory:\n";
    for (size_t i = 0; i < memory.size(); ++i)
    {
        std::cout << "Memory[" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << i << "] = "
                  << std::setfill('0') << std::setw(2) << memory[i] << "\n";
    }
}

// RegisterBank Class Implementation
RegisterBank::RegisterBank(int numRegisters) : registers(numRegisters, 0) {}

int RegisterBank::get(int reg) const
{
    return registers[reg];
}

void RegisterBank::set(int reg, int value)
{
    registers[reg] = value;
}

void RegisterBank::printRegisters() const
{
    std::cout << "Registers:\n";
    for (size_t i = 0; i < registers.size(); ++i)
    {
        std::cout << "Register[" << i << "] = " << std::hex << std::uppercase << std::setfill('0')
                  << std::setw(2) << registers[i] << "\n";
    }
}
int &RegisterBank::getRef(int reg)
{
    return registers[reg];
}

// ALU Class Implementation
int ALU::add(int a, int b) const
{
    return (a + b) & 0xFF;
}

int ALU::bitwiseOr(int a, int b) const
{
    return a | b;
}

int ALU::bitwiseAnd(int a, int b) const
{
    return a & b;
}

int ALU::bitwiseXor(int a, int b) const
{
    return a ^ b;
}

void ALU::rotate(int &reg, int x) const
{
    std::bitset<8> bits(reg);
    bits = (bits >> x) | (bits << (8 - x));
    reg = static_cast<int>(bits.to_ulong());
}

// CU Class Implementation
int CU::fetchInstruction(int pc, const Memory &memory) const
{
    return (memory.get(pc) << 8) | memory.get(pc + 1);
}
void CU::decodeAndExecute(int instruction, int &pc, RegisterBank &registers, Memory &memory, ALU &alu, bool &halted)
{
    int opcode = (instruction >> 12) & 0xF;
    int r = (instruction >> 8) & 0xF;
    int s = (instruction >> 4) & 0xF;
    int t = instruction & 0xF;
    int address = ((s << 4) | t) & 0xFF;

    // Add a message to explain the operation
    std::cout << "Executing Instruction: Opcode " << std::hex << std::uppercase << std::setfill('0') << std::setw(1) << opcode << " - ";

    switch (opcode)
    {
    case 1:
        std::cout << "Load data from memory address 0x" << std::hex << std::setw(2) << address << " into Register[" << r << "]\n";
        registers.set(r, memory.get(address));
        break;
    case 2:
        std::cout << "Load immediate value 0x" << std::hex << address << " into Register[" << r << "]\n";
        registers.set(r, address);
        break;
    case 3:
        std::cout << "Store value from Register[" << r << "] into memory address 0x" << std::hex << std::setw(2) << address << "\n";
        memory.set(address, registers.get(r));
        break;
    case 4:
        std::cout << "Copy value from Register[" << s << "] into Register[" << t << "]\n";
        registers.set(t, registers.get(s));
        break;
    case 5:
        std::cout << "Add values from Register[" << s << "] and Register[" << t << "] and store result in Register[" << r << "]\n";
        registers.set(r, alu.add(registers.get(s), registers.get(t)));
        break;
    case 6:
        std::cout << "Add values from Register[" << s << "] and Register[" << t << "] and store result in Register[" << r << "]\n";
        registers.set(r, alu.add(registers.get(s), registers.get(t)));
        break;
    case 7:
        std::cout << "Bitwise OR between Register[" << s << "] and Register[" << t << "] and store result in Register[" << r << "]\n";
        registers.set(r, alu.bitwiseOr(registers.get(s), registers.get(t)));
        break;
    case 8:
        std::cout << "Bitwise AND between Register[" << s << "] and Register[" << t << "] and store result in Register[" << r << "]\n";
        registers.set(r, alu.bitwiseAnd(registers.get(s), registers.get(t)));
        break;
    case 9:
        std::cout << "Bitwise XOR between Register[" << s << "] and Register[" << t << "] and store result in Register[" << r << "]\n";
        registers.set(r, alu.bitwiseXor(registers.get(s), registers.get(t)));
        break;
    case 0xA:
        std::cout << "Rotate Register[" << r << "] by " << s << " positions\n";
        alu.rotate(registers.getRef(r), s); // Use getRef to get a reference to the register
        break;
    case 0xB:
        std::cout << "Branch to memory address 0x" << std::hex << std::setw(2) << address << " if Register[" << r << "] is 0\n";
        if (registers.get(r) == 0)
        {
            pc = address;
        }
        break;
    case 0xC:
        std::cout << "Halt execution\n";
        halted = true;
        break;
    default:
        std::cerr << "Unknown opcode: " << std::hex << std::setw(1) << opcode << "\n";
        halted = true;
    }
}

void CU::printFinalState(int finalPC, const RegisterBank &registers, const Memory &memory) const
{
    std::cout << "Final Program Counter (PC) = " << std::hex << std::uppercase << std::setfill('0')
              << std::setw(2) << finalPC << "\n";
    registers.printRegisters();
    memory.printMemory();
}

// CPU Class Implementation
CPU::CPU(int memorySize, int numRegisters) : memory(memorySize), registers(numRegisters), pc(0), halted(false) {}

void CPU::loadProgram(const std::string &filename)
{
    memory.loadProgramFromFile(filename);
}

void CPU::run()
{
    int finalPC = pc;
    while (!halted)
    {
        int instruction = controlUnit.fetchInstruction(pc, memory);
        pc += 2;
        controlUnit.decodeAndExecute(instruction, pc, registers, memory, alu, halted);
        if (halted)
        {
            finalPC = pc;
            break;
        }
    }
    controlUnit.printFinalState(pc - 2, registers, memory);
}

// VOLEMachine Class Implementation
VOLEMachine::VOLEMachine(int memorySize, int numRegisters) : cpu(memorySize, numRegisters) {}

bool VOLEMachine::loadProgramFromFile(const std::string &filename)
{
    cpu.loadProgram(filename);
    return true;
}

void VOLEMachine::run()
{
    cpu.run();
}
