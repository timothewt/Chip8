#include "display_renderer.hpp"
#include <SDL2/SDL.h>
#include <array>
#include <cmath>
#include <vector>

DisplayRenderer::DisplayRenderer(int width, int height, int pixel_size)
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip-8 Emulator", 100, 100, width * pixel_size, height * pixel_size, SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);

    SDL_InitSubSystem(SDL_INIT_AUDIO);

    SDL_AudioSpec desired_spec, obtained_spec;
    desired_spec.freq = 44100;
    desired_spec.format = AUDIO_S16SYS;
    desired_spec.channels = 1;
    desired_spec.samples = 512;
    desired_spec.callback = nullptr;

    audio_device = SDL_OpenAudioDevice(nullptr, 0, &desired_spec, &obtained_spec, 0);

    SDL_PauseAudioDevice(audio_device, 0);
}

DisplayRenderer::~DisplayRenderer()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);
    SDL_CloseAudioDevice(audio_device);
    SDL_Quit();
}

void DisplayRenderer::update(const uint32_t* buffer, int pitch)
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool DisplayRenderer::process_input(std::array<uint8_t, 16>& keys)
{
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT)
            return false;
    }
    retrieve_keys(keys);
    return true;
}

void DisplayRenderer::play_beep()
{
    if (!audio_device)
        return;

    const int sample_rate = 44100;
    const int freq = 440;
    const int duration = 100;
    const int samples = (sample_rate * duration) / 1000;

    std::vector<int16_t> buffer(samples);
    for (int i = 0; i < samples; i++)
        buffer[i] = static_cast<int16_t>(32767 * 0.3 * sin(2.0 * M_PI * freq * (double)i / sample_rate));

    SDL_QueueAudio(audio_device, buffer.data(), buffer.size() * sizeof(int16_t));
}

void DisplayRenderer::stop_beep()
{
    SDL_ClearQueuedAudio(audio_device);
}

void DisplayRenderer::retrieve_keys(std::array<uint8_t, 16>& keys)
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
