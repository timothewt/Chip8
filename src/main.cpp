#include "chrono"
#include <Chip8.hpp>
#include <DisplayRenderer.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3) {
        std::cout << "Please call the program as: " << argv[0] << " [ROM path] [CPU Frequency [Optional]]" << std::endl;
        throw std::runtime_error("ROM file not specified.");
    }
    int cpuFrequency {};
    if (argc == 3)
        cpuFrequency = std::stoi(argv[2]);
    else
        cpuFrequency = 500;

    const char* romPath = argv[1];

    DisplayRenderer displayRenderer { WIDTH, HEIGHT, 20 };
    Chip8 chip8 {};
    chip8.loadROM(romPath);
    int pitch = sizeof(chip8.display[0]) * WIDTH;

    bool running;
    float cpuDelayMs = 1000.0f / cpuFrequency;
    std::cout << cpuDelayMs;
    float timerDelayMs = 16.67f;
    auto lastTimersTime = std::chrono::high_resolution_clock::now();
    auto lastCpuTime = std::chrono::high_resolution_clock::now();
    do {
        running = displayRenderer.processInput(chip8.keys);

        auto currentTime = std::chrono::high_resolution_clock::now();

        float cpuDt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCpuTime).count();
        if (cpuDt >= cpuDelayMs) {
            lastCpuTime = currentTime;
            chip8.cycle();
        }

        float timersDt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastTimersTime).count();
        if (timersDt >= timerDelayMs) {
            lastTimersTime = currentTime;
            chip8.updateTimers();
            displayRenderer.update(chip8.display, pitch);
        }

    } while (running);

    return 0;
}
