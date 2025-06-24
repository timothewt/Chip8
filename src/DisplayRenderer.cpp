#include "SDL2/SDL.h"
#include <DisplayRenderer.hpp>

DisplayRenderer::DisplayRenderer(int width, int height, int pixelSize)
    : window(nullptr)
    , renderer(nullptr)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(width * pixelSize, height * pixelSize, 0, &window, &renderer);
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

bool DisplayRenderer::processInput()
{
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            return false;
    }
    return true;
}
