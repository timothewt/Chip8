/**
 * @file Chip8.hpp
 * @brief Declaration of the Chip8 class.
 *
 * @author Timothé Watteau
 */
#pragma once
#include <array>
#include <cstdint>
#include <functional>
#include <random>
#include <unordered_map>

constexpr uint8_t WIDTH = 64; /**< Screen width in game pixels */
constexpr uint8_t HEIGHT = 32; /**< Screen height in game pixels */
constexpr uint16_t ROM_START_ADDRESS = 0x200; /**< Start address in memory to start saving the ROM from. */
constexpr uint8_t FONTSET_START_ADDRESS = 0x50; /**< Start address in memory to store the font from. */
constexpr uint8_t FONTSET_SIZE = 80; /**< Size of the font set. */

constexpr std::array<uint8_t, FONTSET_SIZE> fontset = {
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

class Chip8 {
public:
    std::array<uint32_t, WIDTH * HEIGHT> display {}; /**< Current display of the Chip-8. */
    uint8_t sound_timer {}; /**< Sound timer. Playing a sound while it is over 0. */
    std::array<uint8_t, 16> keys {}; /**< Current state of the keys (1 if pressed, 0 otherwise). */

    /**
     * @brief Constructor of the Chip-8.
     *
     * Instantiates the random generates and loads the font in memory.
     */
    Chip8();

    /**
     * @brief Loads a ROM into memory.
     *
     * @param filename Path of the Chip-8 ROM file (.ch8).
     * @throws std::runtime_error if the ROM file could not be opened, or if it is too large.
     */
    void loadROM(char const* filename);
    /**
     * @brief Makes a fetch->decode->execute cycle.
     */
    void cycle();
    /**
     * @brief Updates the timers (delayTimer and soundTimer).
     */
    void update_timers();

private:
    std::array<uint8_t, 4096> memory {}; /**< 4kB memory of the Chip-8. */
    uint16_t pc {}; /**< Program counter, pointing on the current instruction in the memory. */
    uint16_t index {}; /**< Index register, pointing at locations in the memory */
    std::array<uint16_t, 16> stack {}; /**< Stack of instructions. */
    uint8_t sp {}; /**< Stack pointer. */
    uint8_t delay_timer {}; /**< Delay timer decremented at each display refresh. */
    std::array<uint8_t, 16> registers {}; /**< General purpose variable registers. */
    uint16_t opcode; /**< Current operation code read from the memory. */
    std::default_random_engine random_generator; /**< Random generator. */
    std::uniform_int_distribution<uint8_t> random_byte; /**< Generates a random byte. */

    bool replace_vx_when_shift { false }; /**< Whether to replace VX at operations 8XY6/8XYE. */
    bool modern_bxnn { false }; /**< "Modern" way to do the BNNN operation. */

    std::unordered_map<uint8_t, std::function<void()>> opcode_table {}; /**< Maps the operation codes to instructions */
    std::unordered_map<uint8_t, std::function<void()>> table0 {}; /**< Maps the opcodes 0 to the correct instruction. */
    std::unordered_map<uint8_t, std::function<void()>> table8 {}; /**< Maps the opcodes 8 to the correct instruction. */
    std::unordered_map<uint8_t, std::function<void()>> tableE {}; /**< Maps the opcodes E to the correct instruction. */
    std::unordered_map<uint8_t, std::function<void()>> tableF {}; /**< Maps the opcodes F to the correct instruction. */

    /**
     * @brief Sets up the operation codes table used when decoding.
     */
    void setup_opcode_table();
    /**
     * @brief Decodes and execute the current instruction.
     */
    void
    decode_and_execute();
    /**
     * @brief Extracts the X component (second nibble) from the current opcode.
     *
     * @return The X value (0x0 to 0xF).
     */
    uint8_t get_x();
    /**
     * @brief Extracts the Y component (third nibble) from the current opcode.
     *
     * @return The Y value (0x0 to 0xF).
     */
    uint8_t get_y();
    /**
     * @brief Extracts the N component (fourth nibble) from the current opcode.
     *
     * @return The N value (0x0 to 0xF).
     */
    uint8_t get_n();
    /**
     * @brief Extracts the NN component (last byte) from the current opcode.
     *
     * @return The NN value (0x00 to 0xFF).
     */
    uint8_t get_nn();
    /**
     * @brief Extracts the NNN component (lowest 12 bits) from the current opcode.
     *
     * @return The NNN address (0x000 to 0xFFF).
     */
    uint16_t get_nnn();
    /**
     * All the operations availables. Please refer to https://tobiasvl.github.io/blog/write-a-chip-8-emulator/ for more
     * information.
     */
    void op_00e0();
    void op_00ee();
    void op_1nnn();
    void op_2nnn();
    void op_3xnn();
    void op_4xnn();
    void op_5xy0();
    void op_6xnn();
    void op_7xnn();
    void op_8xy0();
    void op_8xy1();
    void op_8xy2();
    void op_8xy3();
    void op_8xy4();
    void op_8xy5();
    void op_8xy6();
    void op_8xy7();
    void op_8xye();
    void op_9xy0();
    void op_annn();
    void op_bnnn();
    void op_cxnn();
    void op_dxyn();
    void op_ex9e();
    void op_exa1();
    void op_fx07();
    void op_fx0a();
    void op_fx15();
    void op_fx18();
    void op_fx1e();
    void op_fx29();
    void op_fx33();
    void op_fx55();
    void op_fx65();
};
