#include <iostream>
#include <SDL3/SDL.h>

int DELAY = 1000;
int WIDTH = 400;
int HEIGHT = 400;

int main() {
    SDL_Window *window = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    window = SDL_CreateWindow("SDL Example",
                WIDTH,
                HEIGHT,
                0);


    if (window == NULL) {
    fprintf(stderr, "SDL window failed to initialise: %s\n", SDL_GetError());
    return 1;
    }

    SDL_Delay(DELAY);

    SDL_DestroyWindow(window);

    SDL_Quit(); 

    return 0;
}