#pragma once

#include <cstdint>
#include <SDL3/SDL.h>

class SDLInterface {
private:
    static constexpr unsigned int DELAY = 1000;
    static constexpr unsigned int WIDTH = 1280;
    static constexpr unsigned int HEIGHT = 640;
    static constexpr unsigned int DISPLAY_WIDTH = 64;
    static constexpr unsigned int DISPLAY_HEIGHT = 32;

    bool exit = false;
    SDL_Window* window = nullptr;
    SDL_Surface* surface = nullptr;
    SDL_Event ev;

public:
    // Initialize SDL and create a window
    int create_window();

    // Destroy the SDL window and quit SDL
    void destroy_window();

    // Draw the CHIP-8 display buffer to the SDL surface
    void draw_display(uint32_t (&display)[DISPLAY_HEIGHT][DISPLAY_WIDTH]);

    // Process all events in events queue
    void process_events();

    // Check if program has been exited
    bool poll_exit();
};