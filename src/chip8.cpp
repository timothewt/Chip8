#include "chip8.hpp"
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

Chip8::Chip8()
    : random_generator(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc = ROM_START_ADDRESS;

    for (int i = 0; i < FONTSET_SIZE; ++i)
        memory[FONTSET_START_ADDRESS + i] = fontset[i];

    setup_opcode_table();
}

void Chip8::load_rom(char const* path)
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

    decode_and_execute();
}

void Chip8::update_timers()
{
    if (delay_timer > 0)
        --delay_timer;
    if (sound_timer > 0)
        --sound_timer;
}

void Chip8::setup_opcode_table()
{
    table0[0xE0] = [this]() { op_00e0(); };
    table0[0xEE] = [this]() { op_00ee(); };

    table8[0x0] = [this]() { op_8xy0(); };
    table8[0x1] = [this]() { op_8xy1(); };
    table8[0x2] = [this]() { op_8xy2(); };
    table8[0x3] = [this]() { op_8xy3(); };
    table8[0x4] = [this]() { op_8xy4(); };
    table8[0x5] = [this]() { op_8xy5(); };
    table8[0x6] = [this]() { op_8xy6(); };
    table8[0x7] = [this]() { op_8xy7(); };
    table8[0xE] = [this]() { op_8xye(); };

    tableE[0xE] = [this]() { op_ex9e(); };
    tableE[0x1] = [this]() { op_exa1(); };

    tableF[0x07] = [this]() { op_fx07(); };
    tableF[0x0A] = [this]() { op_fx0a(); };
    tableF[0x15] = [this]() { op_fx15(); };
    tableF[0x18] = [this]() { op_fx18(); };
    tableF[0x1E] = [this]() { op_fx1e(); };
    tableF[0x29] = [this]() { op_fx29(); };
    tableF[0x33] = [this]() { op_fx33(); };
    tableF[0x55] = [this]() { op_fx55(); };
    tableF[0x65] = [this]() { op_fx65(); };

    opcode_table[0x0] = [this]() { table0[get_nn()](); };
    opcode_table[0x1] = [this]() { op_1nnn(); };
    opcode_table[0x2] = [this]() { op_2nnn(); };
    opcode_table[0x3] = [this]() { op_3xnn(); };
    opcode_table[0x4] = [this]() { op_4xnn(); };
    opcode_table[0x5] = [this]() { op_5xy0(); };
    opcode_table[0x6] = [this]() { op_6xnn(); };
    opcode_table[0x7] = [this]() { op_7xnn(); };
    opcode_table[0x8] = [this]() { table8[get_n()](); };
    opcode_table[0x9] = [this]() { op_9xy0(); };
    opcode_table[0xA] = [this]() { op_annn(); };
    opcode_table[0xB] = [this]() { op_bnnn(); };
    opcode_table[0xC] = [this]() { op_cxnn(); };
    opcode_table[0xD] = [this]() { op_dxyn(); };
    opcode_table[0xE] = [this]() { tableE[get_n()](); };
    opcode_table[0xF] = [this]() { tableF[get_nn()](); };
}

void Chip8::decode_and_execute()
{
    uint8_t op_type = (opcode & 0xF000u) >> 12u;
    opcode_table[op_type]();
}

uint8_t Chip8::get_x()
{
    return (opcode & 0x0F00u) >> 8u;
}

uint8_t Chip8::get_y()
{
    return (opcode & 0x00F0u) >> 4u;
}

uint8_t Chip8::get_n()
{
    return opcode & 0x000F;
}

uint8_t Chip8::get_nn()
{
    return opcode & 0x00FF;
}

uint16_t Chip8::get_nnn()
{
    return opcode & 0x0FFF;
}

void Chip8::op_00e0()
{
    display.fill(0);
}

void Chip8::op_00ee()
{
    --sp;
    pc = stack[sp];
}

void Chip8::op_1nnn()
{
    pc = get_nnn();
}

void Chip8::op_2nnn()
{
    stack[sp++] = pc;
    pc = get_nnn();
}

void Chip8::op_3xnn()
{
    if (registers[get_x()] == get_nn())
        pc += 2;
}

void Chip8::op_4xnn()
{
    if (registers[get_x()] != get_nn())
        pc += 2;
}

void Chip8::op_5xy0()
{
    if (registers[get_x()] == registers[get_y()])
        pc += 2;
}

void Chip8::op_6xnn()
{
    registers[get_x()] = get_nn();
}

void Chip8::op_7xnn()
{
    registers[get_x()] += get_nn();
}

void Chip8::op_8xy0()
{
    registers[get_x()] = registers[get_y()];
}

void Chip8::op_8xy1()
{
    registers[get_x()] |= registers[get_y()];
}

void Chip8::op_8xy2()
{
    registers[get_x()] &= registers[get_y()];
}

void Chip8::op_8xy3()
{
    registers[get_x()] ^= registers[get_y()];
}

void Chip8::op_8xy4()
{
    uint16_t sum = registers[get_x()] + registers[get_y()];
    registers[0xF] = sum > 255u;
    registers[get_x()] = sum & 0xFF;
}

void Chip8::op_8xy5()
{
    uint8_t vx = registers[get_x()];
    uint8_t vy = registers[get_y()];
    registers[0xF] = vx > vy;
    registers[get_x()] = vx - vy;
}

void Chip8::op_8xy6()
{
    if (replace_vx_when_shift)
        registers[get_x()] = registers[get_y()];
    registers[0xF] = registers[get_x()] & 0x1u;
    registers[get_x()] >>= 1;
}

void Chip8::op_8xy7()
{
    uint8_t vx = registers[get_x()];
    uint8_t vy = registers[get_y()];
    registers[0xF] = vy > vx;
    registers[get_x()] = vy - vx;
}

void Chip8::op_8xye()
{
    if (replace_vx_when_shift)
        registers[get_x()] = registers[get_y()];
    registers[0xF] = (registers[get_x()] & 0x80) >> 7u;
    registers[get_x()] <<= 1;
}

void Chip8::op_9xy0()
{
    if (registers[get_x()] != registers[get_y()])
        pc += 2;
}

void Chip8::op_annn()
{
    index = get_nnn();
}

void Chip8::op_bnnn()
{
    uint8_t register_idx = modern_bxnn ? get_x() : 0;
    pc = get_nnn() + registers[register_idx];
}

void Chip8::op_cxnn()
{
    registers[get_x()] = random_byte(random_generator) & get_nn();
}

void Chip8::op_dxyn()
{
    uint8_t x = registers[get_x()] % WIDTH;
    uint8_t y = registers[get_y()] % HEIGHT;
    registers[0xF] = 0;
    for (uint16_t row = 0; row < get_n(); ++row) {
        uint8_t sprite_byte = memory[index + row];
        for (uint8_t col = 0; col < 8; ++col) {
            uint8_t sprite_pixel = sprite_byte & (0x80u >> col);
            uint32_t* current_pixel = &display[(y + row) * WIDTH + (x + col)];

            if (sprite_pixel) {
                if (*current_pixel == 0xFFFFFFFFu)
                    registers[0xF] = 1;
                *current_pixel ^= 0xFFFFFFFFu;
            }

            if (x + col >= WIDTH)
                break;
        }
        if (y + row >= HEIGHT)
            break;
    }
}

void Chip8::op_ex9e()
{
    if (keys[registers[get_x()]])
        pc += 2;
}

void Chip8::op_exa1()
{
    if (!keys[registers[get_x()]])
        pc += 2;
}

void Chip8::op_fx07()
{
    registers[get_x()] = delay_timer;
}

void Chip8::op_fx0a()
{
    for (uint8_t i = 0; i < 16; i++) {
        if (keys[i]) {
            registers[get_x()] = i;
            return;
        }
    }
    pc -= 2;
}

void Chip8::op_fx15()
{
    delay_timer = registers[get_x()];
}

void Chip8::op_fx18()
{
    sound_timer = registers[get_x()];
}

void Chip8::op_fx1e()
{
    uint16_t sum = index + registers[get_x()];
    registers[0xF] = sum > 0xFFFu;
    index = sum & 0xFFFu;
}

void Chip8::op_fx29()
{
    index = FONTSET_START_ADDRESS + 5 * registers[get_x()];
}

void Chip8::op_fx33()
{
    const uint8_t vx = registers[get_x()];
    memory[index] = vx / 100;
    memory[index + 1] = (vx / 10) % 10;
    memory[index + 2] = vx % 10;
}

void Chip8::op_fx55()
{
    for (uint8_t i = 0; i <= get_x(); ++i) {
        memory[index + i] = registers[i];
    }
}

void Chip8::op_fx65()
{
    for (uint8_t i = 0; i <= get_x(); ++i) {
        registers[i] = memory[index + i];
    }
}
