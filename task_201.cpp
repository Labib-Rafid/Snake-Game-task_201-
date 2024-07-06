#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

#define SCREEN_WIDTH 1080
#define SCREEN_HEIGHT 680
#define SNAKE_SIZE 20
#define INITIAL_SNAKE_LENGTH 3
#define SNAKE_SPEED 7

enum GameState {
    MAIN_MENU,
    GAMEPLAY,
    GAME_OVER
};

struct SnakeSegment {
    int x, y;
};

struct Food {
    int x, y;
};

struct Button {
    SDL_Rect rect;
    std::string text;
    bool isHovered;
};

bool initializeSDL(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "SDL initialization failed: \n" << SDL_GetError() << endl;
        return false;
    }

    if (TTF_Init() == -1) {
        cout << "TTF initialization failed: \n" << TTF_GetError() << endl;
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
    for (size_t i = 0; i < snake.size(); ++i) {
        SDL_Rect segmentRect = { snake[i].x, snake[i].y, SNAKE_SIZE, SNAKE_SIZE };

        if (i == 0) {
            // Head of the snake
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Blue color for the head
            SDL_RenderFillRect(renderer, &segmentRect);

            // Draw a dot in the middle of the head
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for the dot
            int dotSize = SNAKE_SIZE / 4;
            SDL_Rect dotRect = { snake[i].x + SNAKE_SIZE / 2 - dotSize / 2,
                                 snake[i].y + SNAKE_SIZE / 2 - dotSize / 2,
                                 dotSize, dotSize };
            SDL_RenderFillRect(renderer, &dotRect);
        } else {
            // Body of the snake
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green color for the body
            SDL_RenderFillRect(renderer, &segmentRect);
        }
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

    for (size_t i = 1; i < snake.size(); ++i) {
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

void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, TTF_Font* font, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dstrect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderButton(SDL_Renderer* renderer, Button& button, TTF_Font* font) {
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Color hoverColor = {200, 200, 200, 255};
    SDL_Color color = button.isHovered ? hoverColor : textColor;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &button.rect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &button.rect);
    renderText(renderer, button.text, button.rect.x + 10, button.rect.y + 10, font, color);
}

void renderMainMenu(SDL_Renderer* renderer, TTF_Font* font, std::vector<Button>& buttons) {
    for (auto& button : buttons) {
        renderButton(renderer, button, font);
    }
}

void renderGameOver(SDL_Renderer* renderer, TTF_Font* font, int points, std::vector<Button>& buttons) {
    SDL_Color textColor = {255, 255, 255, 255};
    renderText(renderer, "Game Over", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 200, font, textColor);
    renderText(renderer, "Score: " + to_string(points), SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 150, font, textColor);
    for (auto& button : buttons) {
        renderButton(renderer, button, font);
    }
}

bool isMouseOverButton(Button& button, int mouseX, int mouseY) {
    return (mouseX > button.rect.x && mouseX < button.rect.x + button.rect.w &&
            mouseY > button.rect.y && mouseY < button.rect.y + button.rect.h);
}

void resetGame(vector<SnakeSegment>& snake, char& currentDirection, Food& food, int& points) {
    snake.clear();
    int initialX = SCREEN_WIDTH / 2;
    int initialY = SCREEN_HEIGHT / 2;
    for (int i = 0; i < INITIAL_SNAKE_LENGTH; ++i) {
        snake.push_back({initialX - i * SNAKE_SIZE, initialY});
    }

    repositionFood(food);
    currentDirection = 'R';
    points = 0;
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (!initializeSDL(&window, &renderer)) {
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("font2.ttf", 40);
    if (!font) {
        cout << "Font loading failed: \n" << TTF_GetError() << endl;
        return 1;
    }

    srand(time(0)); // Seed the random number generator

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
    int points = 0;
    GameState gameState = MAIN_MENU;

    vector<Button> buttons = {
        {{SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 50, 200, 50}, "Play Game", false},
        {{SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 10, 200, 50}, "Options", false},
        {{SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 70, 200, 50}, "Exit", false}
    };

    vector<Button> gameOverButtons = {
        {{SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 200, 50}, "Play Again", false},
        {{SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 60, 200, 50}, "Exit", false}
    };

    while (running) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if (gameState == MAIN_MENU) {
                    for (auto& button : buttons) {
                        if (button.isHovered) {
                            if (button.text == "Play Game") {
                                gameState = GAMEPLAY;
                                resetGame(snake, currentDirection, food, points);
                            } else if (button.text == "Options") {
                                // Handle options click
                            } else if (button.text == "Exit") {
                                running = false;
                            }
                        }
                    }
                } else if (gameState == GAME_OVER) {
                    for (auto& button : gameOverButtons) {
                        if (button.isHovered) {
                            if (button.text == "Play Again") {
                                gameState = GAMEPLAY;
                                resetGame(snake, currentDirection, food, points);
                            } else if (button.text == "Exit") {
                                running = false;
                            }
                        }
                    }
                }
            } else if (event.type == SDL_KEYDOWN && gameState == GAMEPLAY) {
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

        // Update button hover states
        for (auto& button : buttons) {
            button.isHovered = isMouseOverButton(button, mouseX, mouseY);
        }

        for (auto& button : gameOverButtons) {
            button.isHovered = isMouseOverButton(button, mouseX, mouseY);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (gameState == MAIN_MENU) {
            renderMainMenu(renderer, font, buttons);
        } else if (gameState == GAMEPLAY) {
            moveSnake(snake, currentDirection, grow);
            grow = false;

            if (checkFoodCollision(snake, food)) {
                repositionFood(food);
                grow = true;
                points += 10;
            }

            if (checkSelfCollision(snake) || checkBorderCollision(snake)) {
                gameState = GAME_OVER;
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            drawSnake(renderer, snake);
            drawFood(renderer, food);

            SDL_Color textColor = {255, 255, 255, 255};
            renderText(renderer, "Score: " + to_string(points), 10, 10, font, textColor);
        } else if (gameState == GAME_OVER) {
            renderGameOver(renderer, font, points, gameOverButtons);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / SNAKE_SPEED);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
