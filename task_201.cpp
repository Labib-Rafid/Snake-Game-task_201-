#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 520
#define SNAKE_SIZE 20
#define INITIAL_SNAKE_LENGTH 3
#define SNAKE_SPEED 7

struct SnakeSegment {
    int x, y;
};

struct Food {
    int x, y;
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

void drawFood(SDL_Renderer* renderer, Food& food) {
    SDL_Rect foodRect = { food.x, food.y, SNAKE_SIZE, SNAKE_SIZE };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for food
    SDL_RenderFillRect(renderer, &foodRect);
}

void moveSnake(vector<SnakeSegment>& snake, char direction, bool grow) {
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

    snake.insert(snake.begin(), {newX, newY});

    if (!grow) {
        snake.pop_back();
    }
}

bool checkFoodCollision(vector<SnakeSegment>& snake, Food& food) {
    return (snake.front().x == food.x && snake.front().y == food.y);
}

void repositionFood(Food& food) {
    food.x = (rand() % (SCREEN_WIDTH / SNAKE_SIZE)) * SNAKE_SIZE;
    food.y = (rand() % (SCREEN_HEIGHT / SNAKE_SIZE)) * SNAKE_SIZE;
}

bool checkSelfCollision(vector<SnakeSegment>& snake) {
    int headX = snake.front().x;
    int headY = snake.front().y;

    for (int i = 1; i < snake.size(); ++i) {
        if (snake[i].x == headX && snake[i].y == headY) {
            return true;
        }
    }
    return false;
}

bool checkBorderCollision(vector<SnakeSegment>& snake) {
    int headX = snake.front().x;
    int headY = snake.front().y;

    // Check if snake's head is out of bounds
    if (headX < 0 || headX >= SCREEN_WIDTH || headY < 0 || headY >= SCREEN_HEIGHT) {
        return true;
    }

    return false;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!initializeSDL(&window, &renderer)) {
        return 1;
    }

    //srand(time(0)); // Seed the random number generator

    vector<SnakeSegment> snake;
    int initialX = SCREEN_WIDTH / 2;
    int initialY = SCREEN_HEIGHT / 2;
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
        snake.push_back({initialX - i * SNAKE_SIZE, initialY});
    }

    Food food;
    repositionFood(food);

    char currentDirection = 'R';
    SDL_Event event;
    bool running = true;
    bool grow = false;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (currentDirection != 'D')
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

        moveSnake(snake, currentDirection, grow);
        grow = false;

        if (checkFoodCollision(snake, food)) {
            repositionFood(food);
            grow = true;
        }

        if(checkSelfCollision(snake)){
            return 1;
        }
        if(checkBorderCollision(snake)){
            cout << "Game Over! Snake collided with window border." << endl;
            running = false;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawSnake(renderer, snake);
        drawFood(renderer, food);

        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / SNAKE_SPEED);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
