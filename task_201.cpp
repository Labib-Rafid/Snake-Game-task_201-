#include <SDL2/SDL.h>
#include<iostream>
#include<cmath>
using namespace std;

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 520

bool initializeSDL(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "SDL initialization failed: \n" << SDL_GetError();
        return false;
    }

    *window = SDL_CreateWindow("Circle Drawing", 
                                SDL_WINDOWPOS_CENTERED, 
                                SDL_WINDOWPOS_CENTERED, 
                                SCREEN_WIDTH, 
                                SCREEN_HEIGHT, 
                                SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        cout << "Window Creation failed: \n" << SDL_GetError();
        return false;
    }

    *renderer = SDL_CreateRenderer(*window,
                                    -1, 
                                    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (*renderer == NULL) {
        cout << "Renderer Creation Failed: \n" << SDL_GetError();
        return false;
    }

    return true;
}


int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!initializeSDL(&window, &renderer)) {
        return 1;
    }

    SDL_Event event;
    bool running = true;
    

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}