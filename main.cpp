#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include<SDL2/SDL.h>


#undef main

using namespace std;

void renderPlayer(SDL_Renderer* renderer, SDL_Rect player, int x, int y, int scale, vector<int> tailX, vector<int> tailY, int tailLength) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    player.w = scale;
    player.h = scale;

    // Gets x and y of all tail blocks and renders them
    for (int i = 0; i < tailLength; i++) {
        player.x = tailX[i];
        player.y = tailY[i];
        SDL_RenderFillRect(renderer, &player);
    }

    player.x = x;
    player.y = y;

    SDL_RenderFillRect(renderer, &player);
}

void renderFood(SDL_Renderer* renderer, SDL_Rect food) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &food);
}

void renderScore(SDL_Renderer* renderer, int tailLength, int scale, int wScale) {
    // Dummy function since SDL_ttf is not included
    // You can replace this function with your custom rendering logic if needed
}

bool checkCollision(int foodx, int foody, int playerx, int playery) {
    return (playerx == foodx && playery == foody);
}

pair<int, int> getFoodSpawn(vector<int> tailX, vector<int> tailY, int playerX, int playerY, int scale, int wScale, int tailLength) {
    bool valid = false;
    int x = 0;
    int y = 0;
    srand(time(0));
    x = scale * (rand() % wScale);
    y = scale * (rand() % wScale);
    valid = true;

    // Check all tail blocks and player block
    for (int i = 0; i < tailLength; i++) {
        if ((x == tailX[i] && y == tailY[i]) || (x == playerX && y == playerY)) {
            valid = false;
        }
    }

    if (!valid) {
        pair<int, int> foodLoc;
        foodLoc = make_pair(-100, -100);
        return foodLoc;
    }

    pair<int, int> foodLoc;
    foodLoc = make_pair(x, y);

    return foodLoc;
}

void gameOver(SDL_Renderer* renderer, SDL_Event event, int scale, int wScale, int tailLength) {
    // Dummy function since SDL_ttf is not included
    // You can replace this function with your custom rendering logic if needed
}

void youWin(SDL_Renderer* renderer, SDL_Event event, int scale, int wScale, int tailLength) {
    // Dummy function since SDL_ttf is not included
    // You can replace this function with your custom rendering logic if needed
}

int main(int argc, char* argv[]) {
    // Init everything so we have everything
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;

    // This is the player rectangle, set all values to 0
    SDL_Rect player;
    player.x = 0;
    player.y = 0;
    player.h = 0;
    player.w = 0;

    // tailLength is incremented every time the snake eats food
    int tailLength = 0;

    // Vectors for storage of tail block positions
    vector<int> tailX;
    vector<int> tailY;

    // Size of tiles
    int scale = 24;
    int wScale = 24;

    // Player position variables
    int x = 0;
    int y = 0;
    int prevX = 0;
    int prevY = 0;

    // Movement controls
    bool up = false;
    bool down = false;
    bool right = false;
    bool left = false;

    bool inputThisFrame = false;
    bool redo = false;

    // Food rectangle
    SDL_Rect food;
    food.w = scale;
    food.h = scale;
    food.x = 0;
    food.y = 0;

    pair<int, int> foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
    food.x = foodLoc.first;
    food.y = foodLoc.second;

    // Show the window with these settings and apply a renderer to it
    window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scale * wScale + 1, scale * wScale + 1, SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    float time = SDL_GetTicks() / 100;

    // Main game loop, this constantly runs and keeps everything updated
    while (true) {
        float newTime = SDL_GetTicks() / 150; // This value (150) is the speed at which the blocks are updated
        float delta = newTime - time;
        time = newTime;

        inputThisFrame = false;

        // Check win condition, tail needs to fill all tiles
        if (tailLength >= 575) {
            youWin(renderer, event, scale, wScale, tailLength);
            x = 0;
            y = 0;
            up = false;
            left = false;
            right = false;
            down = false;
            tailX.clear();
            tailY.clear();
            tailLength = 0;
            redo = false;
            foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);

            if (food.x == -100 && food.y == -100) {
                redo = true;
            }

            food.x = foodLoc.first;
            food.y = foodLoc.second;
        }

        // Controls
        if (SDL_PollEvent(&event)) {
            // Simply exit the program when told to
            if (event.type == SDL_QUIT) {
                exit(0);
            }

            // If a key is pressed
            if (event.type == SDL_KEYDOWN && inputThisFrame == false) {
                // Then check for the key being pressed and change direction accordingly
                if (down == false && event.key.keysym.scancode == SDL_SCANCODE_UP) {
                    up = true;
                    left = false;
                    right = false;
                    down = false;
                    inputThisFrame = true;
                } else if (right == false && event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
                    up = false;
                    left = true;
                    right = false;
                    down = false;
                    inputThisFrame = true;
                } else if (up == false && event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                    up = false;
                    left = false;
                    right = false;
                    down = true;
                    inputThisFrame = true;
                } else if (left == false && event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
                    up = false;
                    left = false;
                    right = true;
                    down = false;
                    inputThisFrame = true;
                }
            }
        }

        // The previous position of the player block
        prevX = x;
        prevY = y;

        if (up) {
            y -= delta * scale;
        } else if (left) {
            x -= delta * scale;
        } else if (right) {
            x += delta * scale;
        } else if (down) {
            y += delta * scale;
        }

        if (redo == true) {
            redo = false;
            foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
            food.x = foodLoc.first;
            food.y = foodLoc.second;

            if (food.x == -100 && food.y == -100) {
                redo = true;
            }
        }

        // Collision detection, has played collided with food?
        if (checkCollision(food.x, food.y, x, y)) {
            // Spawn new food after it has been eaten
            foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
            food.x = foodLoc.first;
            food.y = foodLoc.second;

            if (food.x == -100 && food.y == -100) {
                redo = true;
            }

            tailLength++;
        }

        // Only runs in the frames where the player block has moved
        if (delta * scale == 24) {
            // Update tail size and position
            if (tailX.size() != tailLength) {
                tailX.push_back(prevX);
                tailY.push_back(prevY);
            }

            // Loop through every tail block, move all blocks to the nearest block in front
            // This updates the blocks from end (farthest from player block) to the start (nearest to player block)
            for (int i = 0; i < tailLength; i++) {
                if (i > 0) {
                    tailX[i - 1] = tailX[i];
                    tailY[i - 1] = tailY[i];
                }
            }

            if (tailLength > 0) {
                tailX[tailLength - 1] = prevX;
                tailY[tailLength - 1] = prevY;
            }
        }

        // Game over if player has collided with a tail block, also reset everything
        for (int i = 0; i < tailLength; i++) {
            if (x == tailX[i] && y == tailY[i]) {
                gameOver(renderer, event, scale, wScale, tailLength);
                x = 0;
                y = 0;
                up = false;
                left = false;
                right = false;
                down = false;
                tailX.clear();
                tailY.clear();
                tailLength = 0;
                redo = false;

                foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
                if (food.x == -100 && food.y == -100) {
                    redo = true;
                }

                food.x = foodLoc.first;
                food.y = foodLoc.second;
            }
        }

        // Game over if player out of bounds, also resets the game state
        if (x < 0 || y < 0 || x > scale * wScale - scale || y > scale * wScale - scale) {
            gameOver(renderer, event, scale, wScale, tailLength);
            x = 0;
            y = 0;
            up = false;
            left = false;
            right = false;
            down = false;
            tailX.clear();
            tailY.clear();
            tailLength = 0;
            redo = false;
            foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, wScale, tailLength);
            food.x = foodLoc.first;
            food.y = foodLoc.second;

            if (food.x == -100 && food.y == -100) {
                redo = true;
            }
        }

        // Render everything
        renderFood(renderer, food);
        renderPlayer(renderer, player, x, y, scale, tailX, tailY, tailLength);
        renderScore(renderer, tailLength, scale, wScale);

        SDL_RenderDrawLine(renderer, 0, 0, 0, 24 * 24);
        SDL_RenderDrawLine(renderer, 0, 24 * 24, 24 * 24, 24 * 24);
        SDL_RenderDrawLine(renderer, 24 * 24, 24 * 24, 24 * 24, 0);
        SDL_RenderDrawLine(renderer, 24 * 24, 0, 0, 0);

        // Put everything on screen
        // Nothing is actually put on screen until this is called
        SDL_RenderPresent(renderer);

        // Choose a color and fill the entire window with it, this resets everything before the next frame
        // This also gives us the background color
        SDL_SetRenderDrawColor(renderer, 105, 105, 105, 255);
        SDL_RenderClear(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}