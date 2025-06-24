#include <Chip8.hpp>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>

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

    for (int i = 0; i < FONTSET_SIZE; ++i) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

void Chip8::loadROM(char const* path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        std::streampos size = file.tellg();

        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (int i = 0; i < size; ++i) {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    } else {
        throw std::runtime_error(std::string("Failed to open ROM file:") + path);
    }
}

void Chip8::cycle()
{
    opcode = (memory[pc] << 8u) | memory[pc + 1];
    pc += 2;

    decodeAndExecute();

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
        break;
    case 1:
        OP_1NNN();
        break;
    case 6:
        OP_6XNN();
        break;
    case 7:
        OP_7XNN();
        break;
    case 0xA:
        OP_ANNN();
        break;
    case 0xD:
        OP_DXYN();
        break;
    }
}

void Chip8::OP_00E0()
{
    memset(display, 0, sizeof(display));
}

void Chip8::OP_1NNN()
{
    pc = opcode & 0x0FFFu;
}

void Chip8::OP_6XNN()
{
    registers[(opcode & 0x0F00u) >> 8u] = opcode & 0x00FFu;
}

void Chip8::OP_7XNN()
{
    registers[(opcode & 0x0F00u) >> 8u] += opcode & 0x00FFu;
}

void Chip8::OP_ANNN()
{
    index = opcode & 0x0FFFu;
}

void Chip8::OP_DXYN()
{
    uint8_t x = registers[(opcode & 0x0F00u) >> 8u] % WIDTH;
    uint8_t y = registers[(opcode & 0x00F0u) >> 4u] % HEIGHT;
    registers[0xF] = 0;
    for (uint16_t row = 0; row < (opcode & 0x000Fu); ++row) {
        uint8_t spriteByte = memory[index + row];
        for (uint8_t col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* currentPixel = &display[(y + row) * WIDTH + (x + col)];

            if (spritePixel) {
                if (*currentPixel == 0xFFFFFFFFu)
                    registers[0xF] = 1;
                *currentPixel ^= 0xFFFFFFFFu;
            }

            if (x + col == WIDTH)
                break;
        }
        if (y + row == HEIGHT)
            break;
    }
}
