#include "chrono"
#include <Chip8.hpp>
#include <DisplayRenderer.hpp>
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Please call the program as: " << argv[0] << " [ROM path]" << std::endl;
        throw std::runtime_error("ROM file not specified.");
    }

    const char* romPath = argv[1];

    DisplayRenderer displayRenderer { WIDTH, HEIGHT, 20 };
    Chip8 chip8 {};
    chip8.loadROM(romPath);
    int pitch = sizeof(chip8.display[0]) * WIDTH;

    bool running;
    int delayMs = 50;
    auto lastRefreshTime = std::chrono::high_resolution_clock::now();
    do {
        running = displayRenderer.processInput();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastRefreshTime).count();

        if (dt > delayMs) {
            lastRefreshTime = currentTime;
            chip8.cycle();
            displayRenderer.update(chip8.display, pitch);
        }
    } while (running);

    return 0;
}
