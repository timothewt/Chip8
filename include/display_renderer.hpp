/**
 * @file DisplayRenderer.hpp
 * @brief Declaration of the DisplayRenderer class.
 *
 * @author Timothé Watteau
 */
#pragma once
#include <SDL2/SDL.h>
#include <array>

/**
 * @brief Display Renderer class. Used to display the current content of the display buffer of the Chip-8.
 */
class DisplayRenderer {
public:
    /**
     * @brief Constructor for the Display Renderer.
     *
     *  Initializes all SDL components (i.e., the window, renderer, texture and sound device).
     *
     * @param width Width of the emulated screen.
     * @param height Height of the emulated screen.
     * @param pixel_size The number of displayed pixels on the screen per emulated screen pixel.
     *
     */
    DisplayRenderer(int width, int height, int pixel_size);
    ~DisplayRenderer();
    /**
     * @brief Updates the current render.
     *
     * Updates the current render based on the current content of the Chip-8 display.
     *
     * @param buffer Current Chip-8 display buffer.
     * @param pitch Number of bytes per pixel data in the buffer.
     */
    void update(const uint32_t* buffer, int pitch);
    /**
     * @brief Processes the current input from the player.
     *
     * @param keys The array storing the keys state (1 for pressed, else 0) in the Chip-8.
     * @return false if the player closed the window, true otherwise.
     */
    bool process_input(std::array<uint8_t, 16>& keys);
    /**
     * @brief Starts a beeping sound.
     */
    void play_beep();
    /**
     * @brief Stops the beeping sound.
     */
    void stop_beep();

private:
    SDL_Window* window; /**< SDL window insance. */
    SDL_Renderer* renderer; /**< SDL renderer instance. */
    SDL_Texture* texture; /**< SDL texture, used to copy and render the Chip-8 display buffer. */
    SDL_AudioDeviceID audio_device { 0 }; /**< SDL audio device used to make a beeping sound. */
    SDL_Event e; /**< SDL event, used to poll the quit event. */

    /**
     * @brief Retrieves the state of all keys for the Chip-8.
     *
     * @param keys The array storing the keys state (1 for pressed, else 0) in the Chip-8.
     */
    void retrieve_keys(std::array<uint8_t, 16>& keys);
};
