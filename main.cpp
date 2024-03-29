#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <bits/stdc++.h>
#undef main

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;
const int TILE_SIZE = 20;
int score=0;
int bonusFoodTimer=0;
bool pause;
bool gameOver=false;
bool quit=false;
Mix_Music* bgm;
Mix_Chunk* eatSound;

class Snake {
public:
    Snake();
    void handleInput(SDL_Event &e);
    void move();
    void render(SDL_Renderer *renderer);
    bool checkCollision();
    void spawnFood();
    void drawWall();

private:
    SDL_Rect food;
    std::vector<SDL_Rect> body;
    int direction; // 0: up, 1: down, 2: left, 3: right
    bool bonusFoodActive;
    SDL_Rect bonusFood;
};

Snake::Snake() {
    SDL_Rect head = {0, TILE_SIZE*3, TILE_SIZE, TILE_SIZE};
    body.push_back(head);

    // 2 segment with head
    SDL_Rect body1 = {head.x,head.y,TILE_SIZE,TILE_SIZE};
    body.push_back(body1);
    SDL_Rect body2 = {head.x,head.y,TILE_SIZE,TILE_SIZE};
    body.push_back(body2);
    direction = 3; // Start moving to the right
    spawnFood();
    bonusFoodActive = false;
}

void Snake::handleInput(SDL_Event &e) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                if (direction != 1){
                    direction = 0;
                    if (pause) pause= !pause;
                }
                break;
            case SDLK_DOWN:
                if (direction != 0){ 
                    direction = 1;
                    if (pause) pause= !pause;
                }
                break;
            case SDLK_LEFT:
                if (direction != 3){ 
                    direction = 2;
                    if (pause) pause= !pause;
                }
                break;
            case SDLK_RIGHT:
                if (direction != 2){
                    direction = 3;
                    if (pause) pause= !pause;
                }

                break;
            case SDLK_SPACE:
            pause=!pause;
            break;

        }
    }
}

void Snake::move() {
    SDL_Rect newHead = body.front();

    switch (direction) {
        case 0: // Up
            newHead.y -= TILE_SIZE;
            break;
        case 1: // Down
            newHead.y += TILE_SIZE;
            break;
        case 2: // Left
            newHead.x -= TILE_SIZE;
            break;
        case 3: // Right
            newHead.x += TILE_SIZE;
            break;
    }
 
    if (newHead.x < 0) {
        newHead.x = SCREEN_WIDTH - TILE_SIZE;
    } else if (newHead.x > SCREEN_WIDTH) {
        newHead.x = 0;
    }

    body.insert(body.begin(), newHead);

    if (newHead.x == food.x && newHead.y == food.y) {
        score+=1;
        Mix_PlayChannel(-1,eatSound,0);
        spawnFood();
    }
    else {
       body.pop_back();
    }

    // Check for collision with bonus food
    if (bonusFoodActive && newHead.x < bonusFood.x + bonusFood.w && newHead.x + newHead.w > bonusFood.x &&
        newHead.y < bonusFood.y + bonusFood.h && newHead.y + newHead.h > bonusFood.y) {
        score+=10;
        body.push_back(SDL_Rect{-20, -20, TILE_SIZE, TILE_SIZE});
        bonusFoodActive = false;
    }

    if (bonusFoodActive && SDL_GetTicks() > bonusFoodTimer) {
        bonusFoodActive = false;
    }

    if (checkCollision()) {
        std::cout << "Your Score is " << score << std::endl;
        std::cout << "Game Over!" << std::endl;
        gameOver=true;
    }
}

void Snake::render(SDL_Renderer *renderer) {
    //body color
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255); 
    for (int i = 1; i < body.size(); ++i) {

        SDL_RenderFillRect(renderer, &body[i]);   
    }
    //head color
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &body[0]);
    //food color
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &food);

    if (bonusFoodActive) {
        //bonus food color
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &bonusFood);
    }

    // draw walls
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect wall1 = {TILE_SIZE*3,TILE_SIZE*6, TILE_SIZE*6, TILE_SIZE};
    SDL_Rect wall2 = {TILE_SIZE*3, TILE_SIZE*6 , TILE_SIZE, TILE_SIZE*6};
    SDL_Rect wall3 = {SCREEN_WIDTH-TILE_SIZE*9, SCREEN_HEIGHT-TILE_SIZE*4, TILE_SIZE*6, TILE_SIZE};
    SDL_Rect wall4 = {SCREEN_WIDTH-TILE_SIZE*4, SCREEN_HEIGHT-TILE_SIZE*9, TILE_SIZE, TILE_SIZE*6};
    SDL_Rect wall5 = {SCREEN_WIDTH/2-TILE_SIZE*6,SCREEN_HEIGHT/2+TILE_SIZE*3,TILE_SIZE*11,TILE_SIZE};
    SDL_Rect wall7 = {SCREEN_WIDTH/2-TILE_SIZE,SCREEN_HEIGHT/2+TILE_SIZE*3-TILE_SIZE*5,TILE_SIZE,TILE_SIZE*11};

    SDL_RenderFillRect(renderer, &wall1);
    SDL_RenderFillRect(renderer, &wall2);
    SDL_RenderFillRect(renderer, &wall3);
    SDL_RenderFillRect(renderer, &wall4);
    SDL_RenderFillRect(renderer, &wall5);
    SDL_RenderFillRect(renderer, &wall7);
    //down of the score wall 6 
    SDL_SetRenderDrawColor(renderer,255,0,0,255);
    SDL_Rect wall6 = {0,TILE_SIZE*3,SCREEN_WIDTH,TILE_SIZE/5};
    SDL_RenderFillRect(renderer, &wall6);
    
}

bool Snake::checkCollision() {
    SDL_Rect head = body.front();

    for (auto it = body.begin() + 1; it != body.end(); ++it) {//body collied .
        if (head.x == it->x && head.y == it->y) {
            return true;
        }
    }

   if (head.y < 0 || head.y >= SCREEN_HEIGHT) {// up down collied.
        return true; }
    //obstacle collied.
    SDL_Rect wall1 = {TILE_SIZE*3,TILE_SIZE*6, TILE_SIZE*6, TILE_SIZE};
    SDL_Rect wall2 = {TILE_SIZE*3, TILE_SIZE*6 , TILE_SIZE, TILE_SIZE*6};
    SDL_Rect wall3 = {SCREEN_WIDTH-TILE_SIZE*9, SCREEN_HEIGHT-TILE_SIZE*4, TILE_SIZE*6, TILE_SIZE};
    SDL_Rect wall4 = {SCREEN_WIDTH-TILE_SIZE*4, SCREEN_HEIGHT-TILE_SIZE*9, TILE_SIZE, TILE_SIZE*6};
    SDL_Rect wall5 = {SCREEN_WIDTH/2-TILE_SIZE*6,SCREEN_HEIGHT/2+TILE_SIZE*3,TILE_SIZE*11,TILE_SIZE};
    SDL_Rect wall7 = {SCREEN_WIDTH/2-TILE_SIZE,SCREEN_HEIGHT/2+TILE_SIZE*3-TILE_SIZE*5,TILE_SIZE,TILE_SIZE*11};
    SDL_Rect wall6 = {0,50,SCREEN_WIDTH,TILE_SIZE/5};

    if ((head.x < wall1.x + wall1.w && head.x + head.w > wall1.x && head.y < wall1.y + wall1.h && head.y + head.h > wall1.y) ||
        (head.x < wall2.x + wall2.w && head.x + head.w > wall2.x && head.y < wall2.y + wall2.h && head.y + head.h > wall2.y) ||
        (head.x < wall3.x + wall3.w && head.x + head.w > wall3.x && head.y < wall3.y + wall3.h && head.y + head.h > wall3.y) ||
        (head.x < wall4.x + wall4.w && head.x + head.w > wall4.x && head.y < wall4.y + wall4.h && head.y + head.h > wall4.y) ||
        (head.x < wall5.x + wall5.w && head.x + head.w > wall5.x && head.y < wall5.y + wall5.h && head.y + head.h > wall5.y) ||
        (head.x < wall7.x + wall7.w && head.x + head.w > wall7.x && head.y < wall7.y + wall7.h && head.y + head.h > wall7.y) ||
        (head.x < wall6.x + wall6.w && head.x + head.w > wall6.x && head.y < wall6.y + wall6.h && head.y + head.h > wall6.y)) {
        return true;
    }

    return false;
}

void Snake::spawnFood() {
    SDL_Rect wall1 = {TILE_SIZE*3,TILE_SIZE*6, TILE_SIZE*6, TILE_SIZE};
    SDL_Rect wall2 = {TILE_SIZE*3, TILE_SIZE*6 , TILE_SIZE, TILE_SIZE*6};
    SDL_Rect wall3 = {SCREEN_WIDTH-TILE_SIZE*9, SCREEN_HEIGHT-TILE_SIZE*4, TILE_SIZE*6, TILE_SIZE};
    SDL_Rect wall4 = {SCREEN_WIDTH-TILE_SIZE*4, SCREEN_HEIGHT-TILE_SIZE*9, TILE_SIZE, TILE_SIZE*6};
    SDL_Rect wall5 = {SCREEN_WIDTH/2-TILE_SIZE*6,SCREEN_HEIGHT/2+TILE_SIZE*3,TILE_SIZE*11,TILE_SIZE};
    SDL_Rect wall7 = {SCREEN_WIDTH/2-TILE_SIZE,SCREEN_HEIGHT/2+TILE_SIZE*3-TILE_SIZE*5,TILE_SIZE,TILE_SIZE*11};
    SDL_Rect wall6 = {0,50,SCREEN_WIDTH,TILE_SIZE/5};
    do {
        food.x = rand() % (SCREEN_WIDTH/ TILE_SIZE) * TILE_SIZE;
        food.y = rand() % ((SCREEN_HEIGHT-TILE_SIZE*3)/TILE_SIZE) * TILE_SIZE;
        //No food spawn in obstcle
    } while ((food.x >= wall1.x && food.x < wall1.x + wall1.w && food.y >= wall1.y && food.y < wall1.y + wall1.h) ||
             (food.x >= wall2.x && food.x < wall2.x + wall2.w && food.y >= wall2.y && food.y < wall2.y + wall2.h) ||
             (food.x >= wall3.x && food.x < wall3.x + wall3.w && food.y >= wall3.y && food.y < wall3.y + wall3.h) ||
             (food.x >= wall4.x && food.x < wall4.x + wall4.w && food.y >= wall4.y && food.y < wall4.y + wall4.h) ||
             (food.x >= wall5.x && food.x < wall5.x + wall5.w && food.y >= wall5.y && food.y < wall5.y + wall5.h) ||
             (food.x >= wall7.x && food.x < wall7.x + wall7.w && food.y >= wall7.y && food.y < wall7.y + wall7.h) ||
             (food.x >= 0 && food.x < SCREEN_WIDTH && food.y >= 0 && food.y < 60) ||
             
             std::any_of(body.begin(), body.end(), [this](const SDL_Rect &segment) {
                 return food.x == segment.x && food.y == segment.y;
             }));

    food.w = TILE_SIZE;
    food.h = TILE_SIZE;

    //bonus food create
    if (score % 1 == 0) {
        bonusFoodActive = true;
        bonusFoodTimer = SDL_GetTicks() + 7000;// mili second 

    do {
    bonusFood.x = rand() % (SCREEN_WIDTH / TILE_SIZE) * TILE_SIZE;
    bonusFood.y = rand() % ((SCREEN_HEIGHT-TILE_SIZE*4) / TILE_SIZE) * TILE_SIZE;
    } while ((bonusFood.x >= wall1.x && bonusFood.x < wall1.x + wall1.w && bonusFood.y >= wall1.y && bonusFood.y < wall1.y + wall1.h) ||
     (bonusFood.x >= wall2.x && bonusFood.x < wall2.x + wall2.w && bonusFood.y >= wall2.y && bonusFood.y < wall2.y + wall2.h) ||
     (bonusFood.x >= wall3.x && bonusFood.x < wall3.x + wall3.w && bonusFood.y >= wall3.y && bonusFood.y < wall3.y + wall3.h) ||
     (bonusFood.x >= wall4.x && bonusFood.x < wall4.x + wall4.w && bonusFood.y >= wall4.y && bonusFood.y < wall4.y + wall4.h) ||
     (bonusFood.x >= wall5.x && bonusFood.x < wall5.x + wall5.w && bonusFood.y >= wall5.y && bonusFood.y < wall5.y + wall5.h) ||
     (bonusFood.x >= wall7.x && bonusFood.x < wall7.x + wall7.w && bonusFood.y >= wall7.y && bonusFood.y < wall7.y + wall7.h) ||
     (bonusFood.x >= 0 && bonusFood.x < SCREEN_WIDTH && bonusFood.y >= 0 && bonusFood.y < 60) ||

     std::any_of(body.begin(), body.end(), [this](const SDL_Rect &segment) {
         return bonusFood.x == segment.x && bonusFood.y == segment.y;
     }));
    bonusFood.w=TILE_SIZE*2;
    bonusFood.h=TILE_SIZE*2;
    }

}

void renderScore(SDL_Renderer* renderer, TTF_Font* font, int score) {
    
    // score color
    SDL_Color fontColor = {255,0, 255, 255};
    SDL_Surface* surface1 = TTF_RenderText_Solid(font, ("SCORE:  " + std::to_string(score)).c_str(), fontColor);
    if (surface1) {
        SDL_Texture* text1 = SDL_CreateTextureFromSurface(renderer, surface1);
       
        if (text1) {
            SDL_Rect textRect={SCREEN_WIDTH/2 - 100,10,200,50};   //text where set
            SDL_RenderCopy(renderer, text1, nullptr, &textRect);
            SDL_DestroyTexture(text1);
        }
        
        SDL_FreeSurface(surface1);
    }
}

void displayGameOver(SDL_Renderer* renderer, TTF_Font* font, int finalScore) {
    SDL_SetRenderDrawColor(renderer, 255,255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = {255, 0, 255, 255};
    std::string m = "GAME OVER";
    SDL_Surface* surface = TTF_RenderText_Solid(font, m.c_str(), textColor);
    SDL_Color textColor1 = {0,0, 255, 255};
    SDL_Surface* surface3 = TTF_RenderText_Solid(font, ("YOUR FINAL SCORE:  " + std::to_string(score)).c_str(), textColor1);
    
    if (surface) {
        SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Texture* text3 = SDL_CreateTextureFromSurface(renderer, surface3);
        if (text) {
            SDL_Rect textRect;
            textRect.x = SCREEN_WIDTH / 4;
            textRect.y = SCREEN_HEIGHT / 5;
            textRect.w = SCREEN_WIDTH / 2.5;
            textRect.h = SCREEN_HEIGHT / 5;

            SDL_RenderCopy(renderer, text, nullptr, &textRect);

            SDL_DestroyTexture(text);
        }

        if(text3){
            SDL_Rect textRect3={SCREEN_WIDTH/5,SCREEN_HEIGHT/2,SCREEN_WIDTH/1.5,SCREEN_HEIGHT/4};
            SDL_RenderCopy(renderer, text3, nullptr, &textRect3);

            SDL_DestroyTexture(text3);
        }
        SDL_FreeSurface(surface);
        SDL_FreeSurface(surface3);

    }

    SDL_RenderPresent(renderer);
    SDL_Delay(5000);
    exit(0);
}


int main(int argc, char *argv[]) {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Mix_Init(MIX_INIT_MP3);
    //IMG_Init(IMG_INIT_JPG);  // Initialize SDL2_image for PNG images

   //SDL_Surface* backgroundImageSurface = IMG_Load("Julfikar Picture.jpg");
  // SDL_Texture* backgroundImageTexture = SDL_CreateTextureFromSurface(renderer, backgroundImageSurface);

    
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    bgm = Mix_LoadMUS("memories.mp3");
    Mix_PlayMusic(bgm, -1);

    eatSound = Mix_LoadWAV("eating sound.mp3");
    if (!eatSound) {
    std::cerr << "Failed to load eat sound: " << Mix_GetError() << std::endl;
    Mix_Quit();
    SDL_Quit();
    return 1;
}


    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    // Load font
    TTF_Font* font = TTF_OpenFont("font.ttf", 50);
    if (!font) {
    std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
    TTF_Quit();
    return 1;
}
    
    Snake snake;

    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            snake.handleInput(e);
        }
        if (!pause && !gameOver) {
        snake.move();

        if (snake.checkCollision()) {
            gameOver = true;
        }
    
    
    }

            // Render the background image
       // SDL_RenderCopy(renderer, backgroundImageTexture, NULL, NULL);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        snake.render(renderer);
        renderScore(renderer, font, score);
        if(gameOver)
        displayGameOver(renderer, font, score);
    

        
        SDL_RenderPresent(renderer);
        SDL_Delay(120);
    }

    //SDL_FreeSurface(backgroundImageSurface);
    //SDL_DestroyTexture(backgroundImageTexture);
    //IMG_Quit();
    Mix_FreeMusic(bgm);
    Mix_FreeChunk(eatSound);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
    Mix_Quit();

    return 0;
}
