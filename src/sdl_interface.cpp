#include <iostream>
#include <SDL3/SDL.h>
#include "sdl_interface.hpp"

int SDLInterface::create_window() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    window = SDL_CreateWindow("SDL Example", WIDTH, HEIGHT, 0);
    surface = SDL_GetWindowSurface(window);
    

    if (window == NULL) {
        fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
        return 1;
    }
    return 0;
}

void SDLInterface::destroy_window() {
    SDL_DestroyWindow(window);

    SDL_Quit(); 
}

void SDLInterface::draw_display(uint32_t (&display)[DISPLAY_HEIGHT][DISPLAY_WIDTH]) {
    SDL_Rect rect;
    rect.w = 20;
    rect.h = 20;
    for(int y = 0; y < DISPLAY_HEIGHT; y++){
        for(int x = 0; x < DISPLAY_WIDTH; x++) {
            rect.x = x*20;
            rect.y = y*20;
            SDL_FillSurfaceRect(surface, &rect, display[y][x] * 0xFFFFFFFF);
        }
    }

    SDL_UpdateWindowSurface(window);
}

bool SDLInterface::poll_exit() {
    return exit;
}

void SDLInterface::process_events() {
    while(SDL_PollEvent(&ev) != 0){
        if (ev.type == SDL_EVENT_QUIT) {
            exit = true;
        }
        else if (ev.type == SDL_EVENT_KEY_DOWN) {
            process_key(ev.key.key, 1);
        }
        else if (ev.type == SDL_EVENT_KEY_UP) {
            process_key(ev.key.key, 0);
        }
    }
}

bool (&SDLInterface::get_keys())[16] {
    return keys;
}

void SDLInterface::process_key(SDL_Keycode keycode, int down) {
    switch (keycode)
    {
        case SDLK_X:
            keys[0] = down;
            break;
        case SDLK_1:
            keys[1] = down;
            break;
        case SDLK_2:
            keys[2] = down;
            break;
        case SDLK_3:
            keys[3] = down;
            break;
        case SDLK_Q:
            keys[4] = down;
            break;
        case SDLK_W:
            keys[5] = down;
            break;
        case SDLK_E:
            keys[6] = down;
            break;
        case SDLK_A:
            keys[7] = down;
            break;
        case SDLK_S:
            keys[8] = down;
            break;
        case SDLK_D:
            keys[9] = down;
            break;
        case SDLK_Z:
            keys[10] = down;
            break;
        case SDLK_B:
            keys[11] = down;
            break;
        case SDLK_4:
            keys[12] = down;
            break;
        case SDLK_R:
            keys[13] = down;
            break;
        case SDLK_F:
            keys[14] = down;
            break;
        case SDLK_V:
            keys[15] = down;
            break;
        default:
            break;
    }
}