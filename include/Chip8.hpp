/**
 * @file Chip8.hpp
 * @brief Declaration of the Chip8 class.
 *
 * @author Timothé Watteau
 */
#pragma once
#include "cstdint"
#include <random>

const uint8_t WIDTH = 64, HEIGHT = 32;

class Chip8 {
public:
    uint32_t display[WIDTH * HEIGHT] {}; /**< Current display of the Chip-8. */
    uint8_t soundTimer {}; /**< Sound timer. Playing a sound while it is over 0. */
    uint8_t keys[16] {}; /**< Current state of the keys (1 if pressed, 0 otherwise). */

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
    void updateTimers();

private:
    uint8_t memory[4096] {}; /**< 4kB memory of the Chip-8. */
    uint16_t pc {}; /**< Program counter, pointing on the current instruction in the memory. */
    uint16_t index {}; /**< Index register, pointing at locations in the memory */
    uint16_t stack[16] {}; /**< Stack of instructions. */
    uint8_t sp {}; /**< Stack pointer. */
    uint8_t delayTimer {}; /**< Delay timer decremented at each display refresh. */
    uint8_t registers[16] {}; /**< General purpose variable registers. */
    uint16_t opcode; /**< Current operation code read from the memory. */
    std::default_random_engine randGen; /**< Random generator. */
    std::uniform_int_distribution<uint8_t> randByte; /**< Generates a random byte. */

    bool replaceVXWhenShift { false }; /**< Whether to replace VX at operations 8XY6/8XYE, as in some implementations. */
    bool modernBXNN { false }; /**< "Modern" way to do the BNNN operation. */

    /**
     * @brief Decodes and execute the current instruction.
     */
    void decodeAndExecute();

    /**
     * @brief Extracts the X component (second nibble) from the current opcode.
     *
     * @return The X value (0x0 to 0xF).
     */
    uint8_t getX();

    /**
     * @brief Extracts the Y component (third nibble) from the current opcode.
     *
     * @return The Y value (0x0 to 0xF).
     */
    uint8_t getY();

    /**
     * @brief Extracts the N component (fourth nibble) from the current opcode.
     *
     * @return The N value (0x0 to 0xF).
     */
    uint8_t getN();

    /**
     * @brief Extracts the NN component (last byte) from the current opcode.
     *
     * @return The NN value (0x00 to 0xFF).
     */
    uint8_t getNN();

    /**
     * @brief Extracts the NNN component (lowest 12 bits) from the current opcode.
     *
     * @return The NNN address (0x000 to 0xFFF).
     */
    uint16_t getNNN();

    /**
     * All the operations availables. Please refer to https://tobiasvl.github.io/blog/write-a-chip-8-emulator/ for more
     * information.
     */
    void OP_00E0();
    void OP_00EE();
    void OP_1NNN();
    void OP_2NNN();
    void OP_3XNN();
    void OP_4XNN();
    void OP_5XY0();
    void OP_6XNN();
    void OP_7XNN();
    void OP_8XY0();
    void OP_8XY1();
    void OP_8XY2();
    void OP_8XY3();
    void OP_8XY4();
    void OP_8XY5();
    void OP_8XY6();
    void OP_8XY7();
    void OP_8XYE();
    void OP_9XY0();
    void OP_ANNN();
    void OP_BNNN();
    void OP_CXNN();
    void OP_DXYN();
    void OP_EX9E();
    void OP_EXA1();
    void OP_FX07();
    void OP_FX0A();
    void OP_FX15();
    void OP_FX18();
    void OP_FX1E();
    void OP_FX29();
    void OP_FX33();
    void OP_FX55();
    void OP_FX65();
};
