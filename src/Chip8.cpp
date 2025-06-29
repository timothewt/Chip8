#include "Chip8.hpp"
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc = ROM_START_ADDRESS;

    for (int i = 0; i < FONTSET_SIZE; ++i)
        memory[FONTSET_START_ADDRESS + i] = fontset[i];

    setupOpcodeTable();
}

void Chip8::loadROM(char const* path)
{
    if (!std::filesystem::exists(path))
        throw std::runtime_error(std::string("ROM file does not exist:") + path);

    uintmax_t size = std::filesystem::file_size(path);
    if (size > 4096 - ROM_START_ADDRESS)
        throw std::runtime_error(std::string("ROM file too large:") + path);

    std::ifstream file(path, std::ios::binary);
    file.read(reinterpret_cast<char*>(memory.data() + ROM_START_ADDRESS), size);
}

void Chip8::cycle()
{
    opcode = (memory[pc] << 8u) | memory[pc + 1];
    pc += 2;

    decodeAndExecute();
}

void Chip8::updateTimers()
{
    if (delayTimer > 0)
        --delayTimer;
    if (soundTimer > 0)
        --soundTimer;
}

void Chip8::setupOpcodeTable()
{
    table0[0xE0] = [this]() { OP_00E0(); };
    table0[0xEE] = [this]() { OP_00EE(); };

    table8[0x0] = [this]() { OP_8XY0(); };
    table8[0x1] = [this]() { OP_8XY1(); };
    table8[0x2] = [this]() { OP_8XY2(); };
    table8[0x3] = [this]() { OP_8XY3(); };
    table8[0x4] = [this]() { OP_8XY4(); };
    table8[0x5] = [this]() { OP_8XY5(); };
    table8[0x6] = [this]() { OP_8XY6(); };
    table8[0x7] = [this]() { OP_8XY7(); };
    table8[0xE] = [this]() { OP_8XYE(); };

    tableE[0xE] = [this]() { OP_EX9E(); };
    tableE[0x1] = [this]() { OP_EXA1(); };

    tableF[0x07] = [this]() { OP_FX07(); };
    tableF[0x0A] = [this]() { OP_FX0A(); };
    tableF[0x15] = [this]() { OP_FX15(); };
    tableF[0x18] = [this]() { OP_FX18(); };
    tableF[0x1E] = [this]() { OP_FX1E(); };
    tableF[0x29] = [this]() { OP_FX29(); };
    tableF[0x33] = [this]() { OP_FX33(); };
    tableF[0x55] = [this]() { OP_FX55(); };
    tableF[0x65] = [this]() { OP_FX65(); };

    opcodeTable[0x0] = [this]() { table0[getNN()](); };
    opcodeTable[0x1] = [this]() { OP_1NNN(); };
    opcodeTable[0x2] = [this]() { OP_2NNN(); };
    opcodeTable[0x3] = [this]() { OP_3XNN(); };
    opcodeTable[0x4] = [this]() { OP_4XNN(); };
    opcodeTable[0x5] = [this]() { OP_5XY0(); };
    opcodeTable[0x6] = [this]() { OP_6XNN(); };
    opcodeTable[0x7] = [this]() { OP_7XNN(); };
    opcodeTable[0x8] = [this]() { table8[getN()](); };
    opcodeTable[0x9] = [this]() { OP_9XY0(); };
    opcodeTable[0xA] = [this]() { OP_ANNN(); };
    opcodeTable[0xB] = [this]() { OP_BNNN(); };
    opcodeTable[0xC] = [this]() { OP_CXNN(); };
    opcodeTable[0xD] = [this]() { OP_DXYN(); };
    opcodeTable[0xE] = [this]() { tableE[getN()](); };
    opcodeTable[0xF] = [this]() { tableF[getNN()](); };
}

void Chip8::decodeAndExecute()
{
    uint8_t opType = (opcode & 0xF000u) >> 12u;
    opcodeTable[opType]();
}

uint8_t Chip8::getX()
{
    return (opcode & 0x0F00u) >> 8u;
}

uint8_t Chip8::getY()
{
    return (opcode & 0x00F0u) >> 4u;
}

uint8_t Chip8::getN()
{
    return opcode & 0x000F;
}

uint8_t Chip8::getNN()
{
    return opcode & 0x00FF;
}

uint16_t Chip8::getNNN()
{
    return opcode & 0x0FFF;
}

void Chip8::OP_00E0()
{
    display.fill(0);
}

void Chip8::OP_00EE()
{
    --sp;
    pc = stack[sp];
}

void Chip8::OP_1NNN()
{
    pc = getNNN();
}

void Chip8::OP_2NNN()
{
    stack[sp++] = pc;
    pc = getNNN();
}

void Chip8::OP_3XNN()
{
    if (registers[getX()] == (getNN()))
        pc += 2;
}

void Chip8::OP_4XNN()
{
    if (registers[getX()] != (getNN()))
        pc += 2;
}

void Chip8::OP_5XY0()
{
    if (registers[getX()] == registers[getY()])
        pc += 2;
}

void Chip8::OP_6XNN()
{
    registers[getX()] = getNN();
}

void Chip8::OP_7XNN()
{
    registers[getX()] += getNN();
}

void Chip8::OP_8XY0()
{
    registers[getX()] = registers[getY()];
}

void Chip8::OP_8XY1()
{
    registers[getX()] |= registers[getY()];
}

void Chip8::OP_8XY2()
{
    registers[getX()] &= registers[getY()];
}

void Chip8::OP_8XY3()
{
    registers[getX()] ^= registers[getY()];
}

void Chip8::OP_8XY4()
{
    uint16_t sum = registers[getX()] + registers[getY()];
    registers[0xF] = sum > 255u;
    registers[getX()] = sum & 0xFF;
}

void Chip8::OP_8XY5()
{
    uint8_t vx = registers[getX()];
    uint8_t vy = registers[getY()];
    registers[0xF] = vx > vy;
    registers[getX()] = vx - vy;
}

void Chip8::OP_8XY6()
{
    if (replaceVXWhenShift)
        registers[getX()] = registers[getY()];
    registers[0xF] = registers[getX()] & 0x1u;
    registers[getX()] >>= 1;
}

void Chip8::OP_8XY7()
{
    uint8_t vx = registers[getX()];
    uint8_t vy = registers[getY()];
    registers[0xF] = vy > vx;
    registers[getX()] = vy - vx;
}

void Chip8::OP_8XYE()
{
    if (replaceVXWhenShift)
        registers[getX()] = registers[getY()];
    registers[0xF] = (registers[getX()] & 0x80) >> 7u;
    registers[getX()] <<= 1;
}

void Chip8::OP_9XY0()
{
    if (registers[getX()] != registers[getY()])
        pc += 2;
}

void Chip8::OP_ANNN()
{
    index = getNNN();
}

void Chip8::OP_BNNN()
{
    uint8_t registerIdx = modernBXNN ? getX() : 0;
    pc = getNNN() + registers[registerIdx];
}

void Chip8::OP_CXNN()
{
    registers[getX()] = randByte(randGen) & getNN();
}

void Chip8::OP_DXYN()
{
    uint8_t x = registers[getX()] % WIDTH;
    uint8_t y = registers[getY()] % HEIGHT;
    registers[0xF] = 0;
    for (uint16_t row = 0; row < (getN()); ++row) {
        uint8_t spriteByte = memory[index + row];
        for (uint8_t col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* currentPixel = &display[(y + row) * WIDTH + (x + col)];

            if (spritePixel) {
                if (*currentPixel == 0xFFFFFFFFu)
                    registers[0xF] = 1;
                *currentPixel ^= 0xFFFFFFFFu;
            }

            if (x + col >= WIDTH)
                break;
        }
        if (y + row >= HEIGHT)
            break;
    }
}

void Chip8::OP_EX9E()
{
    if (keys[registers[getX()]])
        pc += 2;
}

void Chip8::OP_EXA1()
{
    if (!keys[registers[getX()]])
        pc += 2;
}

void Chip8::OP_FX07()
{
    registers[getX()] = delayTimer;
}

void Chip8::OP_FX0A()
{
    for (uint8_t i = 0; i < 16; i++) {
        if (keys[i]) {
            registers[getX()] = i;
            return;
        }
    }
    pc -= 2;
}

void Chip8::OP_FX15()
{
    delayTimer = registers[getX()];
}

void Chip8::OP_FX18()
{
    soundTimer = registers[getX()];
}

void Chip8::OP_FX1E()
{
    uint16_t sum = index + registers[getX()];
    registers[0xF] = sum > 0xFFFu;
    index = sum & 0xFFFu;
}

void Chip8::OP_FX29()
{
    index = FONTSET_START_ADDRESS + 5 * registers[getX()];
}

void Chip8::OP_FX33()
{
    const uint8_t VX = registers[getX()];
    memory[index] = VX / 100;
    memory[index + 1] = (VX / 10) % 10;
    memory[index + 2] = VX % 10;
}

void Chip8::OP_FX55()
{
    for (uint8_t i = 0; i <= getX(); ++i) {
        memory[index + i] = registers[i];
    }
}

void Chip8::OP_FX65()
{
    for (uint8_t i = 0; i <= getX(); ++i) {
        registers[i] = memory[index + i];
    }
}
