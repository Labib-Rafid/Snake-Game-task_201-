#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

using namespace std;

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 520
#define SNAKE_SIZE 20
#define INITIAL_SNAKE_LENGTH 3
#define SNAKE_SPEED 1

struct SnakeSegment {
    int x, y;
    //SnakeSegment(int _x, int _y) : x(_x), y(_y) {}
};

bool initializeSDL(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "SDL initialization failed: \n" << SDL_GetError() << endl;
        return false;
    }

    *window = SDL_CreateWindow("Snake Game",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               SCREEN_WIDTH,
                               SCREEN_HEIGHT,
                               SDL_WINDOW_SHOWN);
    if (*window == NULL) {
        cout << "Window Creation failed: \n" << SDL_GetError() << endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window,
                                   -1,
                                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (*renderer == NULL) {
        cout << "Renderer Creation Failed: \n" << SDL_GetError() << endl;
        return false;
    }

    return true;
}

void drawSnake(SDL_Renderer* renderer, vector<SnakeSegment>& snake) {
    for (auto& segment : snake) {
        SDL_Rect segmentRect = { segment.x, segment.y, SNAKE_SIZE, SNAKE_SIZE };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for snake
        SDL_RenderFillRect(renderer, &segmentRect);
    }
}

void moveSnake(vector<SnakeSegment>& snake, char direction) {
    // Move snake by adding a new head segment in the current moving direction
    int newX = snake.front().x;
    int newY = snake.front().y;

    switch (direction) {
        case 'U':
            newY -= SNAKE_SIZE;
            break;
        case 'D':
            newY += SNAKE_SIZE;
            break;
        case 'L':
            newX -= SNAKE_SIZE;
            break;
        case 'R':
            newX += SNAKE_SIZE;
            break;
        default:
            break;
    }

    // Insert new head segment at the beginning of the snake vector
    snake.insert(snake.begin(), {newX, newY});

    // Remove the last segment to simulate movement
    snake.pop_back();
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!initializeSDL(&window, &renderer)) {
        return 1;
    }

    // Initialize snake with initial segments
    vector<SnakeSegment> snake;
    int initialX = SCREEN_WIDTH / 2;
    int initialY = SCREEN_HEIGHT / 2;
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
        snake.push_back({initialX - i * SNAKE_SIZE, initialY});
    }

    char currentDirection = 'R'; // Snake starts by moving right
    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (currentDirection != 'D') // Prevent opposite direction movement
                            currentDirection = 'U';
                        break;
                    case SDLK_DOWN:
                        if (currentDirection != 'U')
                            currentDirection = 'D';
                        break;
                    case SDLK_LEFT:
                        if (currentDirection != 'R')
                            currentDirection = 'L';
                        break;
                    case SDLK_RIGHT:
                        if (currentDirection != 'L')
                            currentDirection = 'R';
                        break;
                    default:
                        break;
                }
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Move snake
        moveSnake(snake, currentDirection);

        // Draw snake
        drawSnake(renderer, snake);

        // Update screen
        SDL_RenderPresent(renderer);

        // Add delay to control snake's movement speed
        SDL_Delay(1000 / SNAKE_SPEED);
    }

    // Cleanup and exit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
