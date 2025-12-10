#include<SDL2/SDL.h>
#include<iostream>
#include <random>
using namespace std;

const int WIDTH = 1200;
const int HEIGHT = 600;
int main(int argc, char* argv[]){
    SDL_Window* window = SDL_CreateWindow("LayMan Grey Noise : Too Random", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    /*
    SDL_Rect rect = {WIDTH / 2, HEIGHT / 2, 50, 50};
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    */
    //generate random sign : 0 -> black, 1->white
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> random_sign_dist(0, 1);

    SDL_Rect rect = {0, 0, 1, 1};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            if(random_sign_dist(gen)){
                rect.x = i;
                rect.y = j; 
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    int running = 1;
    SDL_Event event;
    while(running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = 0;
            }
        }
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
