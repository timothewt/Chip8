#include "chip8.hpp"
#include "display_renderer.hpp"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 2 && argc != 3) {
        std::cout << "Usage: " << argv[0] << " <ROM path> [CPU Frequency]" << std::endl;
        throw std::runtime_error("ROM file not specified.");
    }
    int cpu_frequency {};
    if (argc == 3)
        cpu_frequency = std::stoi(argv[2]);
    else
        cpu_frequency = 500;

    const char* rom_path = argv[1];

    DisplayRenderer display_renderer { WIDTH, HEIGHT, 20 };
    Chip8 chip8 {};
    chip8.loadROM(rom_path);
    int pitch = sizeof(chip8.display[0]) * WIDTH;

    bool running;
    float cpu_delay_ms = 1000.0f / cpu_frequency;
    std::cout << cpu_delay_ms;
    float timer_delay_ms = 16.67f;
    auto last_timers_time = std::chrono::high_resolution_clock::now();
    auto last_cpu_time = std::chrono::high_resolution_clock::now();
    do {
        running = display_renderer.process_input(chip8.keys);

        auto current_time = std::chrono::high_resolution_clock::now();

        float cpu_dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cpu_time).count();
        if (cpu_dt >= cpu_delay_ms) {
            last_cpu_time = current_time;
            chip8.cycle();
        }

        float timers_dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_timers_time).count();
        if (timers_dt >= timer_delay_ms) {
            last_timers_time = current_time;
            chip8.update_timers();
            display_renderer.update(chip8.display.data(), pitch);
            if (chip8.sound_timer > 0)
                display_renderer.play_beep();
            else
                display_renderer.stop_beep();
        }

    } while (running);

    return 0;
}
