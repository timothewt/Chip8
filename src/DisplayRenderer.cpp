#include "SDL2/SDL.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include <DisplayRenderer.hpp>

DisplayRenderer::DisplayRenderer(int width, int height, int pixelSize)
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip-8 Emulator", 100, 100, width * pixelSize, height * pixelSize, SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

DisplayRenderer::~DisplayRenderer()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);
    SDL_Quit();
}

void DisplayRenderer::update(const uint32_t* buffer, int pitch)
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool DisplayRenderer::processInput(uint8_t* keys)
{
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            return false;
    }
    retrieveKeys(keys);
    return true;
}

void DisplayRenderer::retrieveKeys(uint8_t* keys)
{

    const Uint8* state = SDL_GetKeyboardState(NULL);

    keys[0] = state[SDL_SCANCODE_1];
    keys[1] = state[SDL_SCANCODE_2];
    keys[2] = state[SDL_SCANCODE_3];
    keys[3] = state[SDL_SCANCODE_4];
    keys[4] = state[SDL_SCANCODE_Q];
    keys[5] = state[SDL_SCANCODE_W];
    keys[6] = state[SDL_SCANCODE_E];
    keys[7] = state[SDL_SCANCODE_R];
    keys[8] = state[SDL_SCANCODE_A];
    keys[9] = state[SDL_SCANCODE_S];
    keys[0xA] = state[SDL_SCANCODE_D];
    keys[0xB] = state[SDL_SCANCODE_F];
    keys[0xC] = state[SDL_SCANCODE_Z];
    keys[0xD] = state[SDL_SCANCODE_X];
    keys[0xE] = state[SDL_SCANCODE_C];
    keys[0xF] = state[SDL_SCANCODE_V];
}
