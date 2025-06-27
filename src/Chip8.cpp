#include "chrono"
#include "cstring"
#include "fstream"
#include "iostream"
#include "stdexcept"
#include "vector"
#include <Chip8.hpp>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONTSET_SIZE = 80;

uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80 // F
};

Chip8::Chip8()
    : randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc = START_ADDRESS;

    for (int i = 0; i < FONTSET_SIZE; ++i)
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
}

void Chip8::loadROM(char const* path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        throw std::runtime_error(std::string("Failed to open ROM file:") + path);
        return;
    }
    std::streampos size = file.tellg();
    if (size > 4096 - START_ADDRESS)
        throw std::runtime_error(std::string("Failed to open ROM file:") + path + std::string(". ROM too large."));

    std::vector<char> buffer(size);
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), size);

    for (int i = 0; i < size; ++i)
        memory[START_ADDRESS + i] = buffer[i];
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

void Chip8::decodeAndExecute()
{
    uint8_t opType = (opcode & 0xF000u) >> 12u;

    switch (opType) {
    case 0:
        if (opcode == 0x00E0)
            OP_00E0();
        else if (opcode == 0x00EE)
            OP_00EE();
        break;
    case 1:
        OP_1NNN();
        break;
    case 2:
        OP_2NNN();
        break;
    case 3:
        OP_3XNN();
        break;
    case 4:
        OP_4XNN();
        break;
    case 5:
        if (!getN())
            OP_5XY0();
        break;
    case 6:
        OP_6XNN();
        break;
    case 7:
        OP_7XNN();
        break;
    case 8:
        switch (getN()) {
        case 0:
            OP_8XY0();
            break;
        case 1:
            OP_8XY1();
            break;
        case 2:
            OP_8XY2();
            break;
        case 3:
            OP_8XY3();
            break;
        case 4:
            OP_8XY4();
            break;
        case 5:
            OP_8XY5();
            break;
        case 6:
            OP_8XY6();
            break;
        case 7:
            OP_8XY7();
            break;
        case 0xE:
            OP_8XYE();
            break;
        }
        break;
    case 9:
        if (!getN())
            OP_9XY0();
        break;
    case 0xA:
        OP_ANNN();
        break;
    case 0xB:
        OP_BNNN();
        break;
    case 0xC:
        OP_CXNN();
        break;
    case 0xD:
        OP_DXYN();
        break;
    case 0xE:
        switch (getNN()) {
        case 0x9E:
            OP_EX9E();
            break;
        case 0xA1:
            OP_EXA1();
            break;
        }
        break;
    case 0xF:
        switch (getNN()) {
        case 0x07:
            OP_FX07();
            break;
        case 0x0A:
            OP_FX0A();
            break;
        case 0x15:
            OP_FX15();
            break;
        case 0x18:
            OP_FX18();
            break;
        case 0x1E:
            OP_FX1E();
            break;
        case 0x29:
            OP_FX29();
            break;
        case 0x33:
            OP_FX33();
            break;
        case 0x55:
            OP_FX55();
            break;
        case 0x65:
            OP_FX65();
            break;
        }
        break;
    }
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
    memset(display, 0, sizeof(display));
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
