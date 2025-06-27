#pragma once
#include "SDL2/SDL.h"

class DisplayRenderer {
public:
    DisplayRenderer(int width, int height, int pixelSize);
    ~DisplayRenderer();
    void update(const uint32_t* buffer, int pitch);
    /**
     * @return true if the player continues false if he leaves
     */
    bool processInput(uint8_t* keys);
    void playBeep();
    void stopBeep();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event e;
    SDL_Texture* texture;
    SDL_AudioDeviceID audioDevice;

    void retrieveKeys(uint8_t* keys);
};
