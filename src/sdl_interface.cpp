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
