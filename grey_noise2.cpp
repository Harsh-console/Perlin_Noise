#include<SDL2/SDL.h>
#include<random>
#include<vector>
#include<cmath>
#include<iostream>
using namespace std;

//structs
typedef struct{
    float x;
    float y;
}vector2;

// functions


// Generate Random vector( not necessarily unit vector)
vector2 generateDr(){
    static float valX, valY;
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_real_distribution<float> dist(-1.0f, 1.0f);
    valX = dist(gen);
    valY = dist(gen);
    vector2 grad {valX, valY};
    float vec_mod = pow((pow(grad.x, 2) + pow(grad.y, 2)), 0.5);
    grad.x /= vec_mod;
    grad.y /= vec_mod;
    return grad;
}

// generate grid
void generate_grid(vector<vector<vector2>>& grid, const int numX, const int numY){
    for(int i = 0; i < numX; i++){
        for(int j = 0; j < numY; j++){
            grid[j][i] = generateDr();
        }
    }  
}

float vector2_dist_magnitude(const vector2 &v1, const vector2 &v2){
    float delX = v1.x - v2.x;
    float delY = v1.y - v2.y;
    return hypot(delX, delY);
}

float dot_prod(const vector2& v1, const vector2& v2){
    return (v1.x * v2.x) + (v1.y * v2.y);
}

float interpolation_function(const float& x){
    // f(x) = 6*x^5 - 15*x^4 + 10*x^3
    return (6*pow(x, 5) - 15 * pow(x, 4) + 10*pow(x, 3));
}

// bilinear smoothing
float lerp(const float& a, const float& b, const float& t){
    return (a + t*(b - a));
}

float Noise2D(const vector2 &point__, const int GRID_SIZE, const vector<vector<vector2>>& grid){
    float x = point__.x, y = point__.y;
    static vector2 del_v1, del_v2, del_v3, del_v4;
    static float dot1, dot2, dot3, dot4;
    static float dist1, dist2, dist3, dist4;
    //static float interpolation_val1, interpolation_val2, interpolation_val3, interpolation_val4;
    static float interpolation_valX, interpolation_valY;
    //static float total_interpolation_val;
    static float nx0, nx1;
    static float t_X, t_Y;
    int x0 = x / GRID_SIZE;
    int y0 = y / GRID_SIZE;
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    t_X = (x - (GRID_SIZE * x0)) / GRID_SIZE;
    t_Y = (y - (GRID_SIZE * y0)) / GRID_SIZE;
    // 1(x0, y0)       2(x1, y0)
    //   ---------------
    //  |---------------|
    //  |---------------|
    //  |---------------|
    //  |---------------|
    //  |---------------|
    //  |_______________|
    //  3(x0, y1)       4(x1, y1)

    del_v1.x = x - x0*GRID_SIZE; del_v1.y = y - y0*GRID_SIZE;
    del_v2.x = x - x1*GRID_SIZE; del_v2.y = y - y0*GRID_SIZE;
    del_v3.x = x - x0*GRID_SIZE; del_v3.y = y - y1*GRID_SIZE;
    del_v4.x = x - x1*GRID_SIZE; del_v4.y = y - y1*GRID_SIZE;

    /*
    dist1 = vector2_dist_magnitude(vector2 {x0, y0}, point__) / GRID_SIZE;
    dist2 = vector2_dist_magnitude(vector2 {x1, y0}, point__) / GRID_SIZE;
    dist3 = vector2_dist_magnitude(vector2 {x0, y1}, point__) / GRID_SIZE;
    dist4 = vector2_dist_magnitude(vector2 {x1, y1}, point__) / GRID_SIZE;
    */
    /*
    interpolation_val1 = interpolation_function(dist1);
    interpolation_val2 = interpolation_function(dist2);
    interpolation_val3 = interpolation_function(dist3);
    interpolation_val4 = interpolation_function(dist4);
    */
    interpolation_valX = interpolation_function(t_X);
    interpolation_valY = interpolation_function(t_Y);
    
    vector2 rand_vec1 = grid[x0][y0];
    vector2 rand_vec2 = grid[x1][y0];
    vector2 rand_vec3 = grid[x0][y1];
    vector2 rand_vec4 = grid[x1][y1];
    
    /*
    vector2 rand_vec1 = grid[y0][x0];
    vector2 rand_vec2 = grid[y0][x1];
    vector2 rand_vec3 = grid[y1][x0];
    vector2 rand_vec4 = grid[y1][x1];
    */
    dot1 = dot_prod(del_v1, rand_vec1);
    dot2 = dot_prod(del_v2, rand_vec2);
    dot3 = dot_prod(del_v3, rand_vec3);
    dot4 = dot_prod(del_v4, rand_vec4);

    //total_interpolation_val = ((interpolation_val1 * dot1) + (interpolation_val2 * dot2) + (interpolation_val3 * dot3) + (interpolation_val4 * dot4)) / (interpolation_val1 + interpolation_val2 + interpolation_val3 + interpolation_val4);
    nx0 = lerp(dot1, dot2, t_X);
    nx1 = lerp(dot3, dot4, t_X);
    //return total_interpolation_val;
    return lerp(nx0, nx1, t_Y);

}

int main(int argv, char* argc[]){

    //constants
    const int WIDTH = 800;
    const int HEIGHT = 800;
    const int GRID_SIZE = 50;
    const int numX = (WIDTH / GRID_SIZE) ;
    const int numY = (HEIGHT / GRID_SIZE);
    //const int __grid[numX + 1][numY + 1] = {0};


    //Init SDL
    SDL_Window* window = SDL_CreateWindow("Perlin like Noise Generator!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // generate grid
    vector<vector<vector2>> grid(numY+1, vector<vector2> (numX+1, vector2 {0, 0}));
    generate_grid(grid, numX, numY);

    //find color of each pixel using perlin noise - like function
    vector2 point__ {0, 0};
    float pix_brightness = 0;
    SDL_Rect rect = {0, 0, 1, 1};
    for(int i = 0; i < WIDTH - 1; i++){
        for(int j = 0; j < HEIGHT - 1; j++){
            //cout<<i<<j<<endl;
            point__.x = j; point__.y = i;
            //cout<<".";
            pix_brightness = Noise2D(point__, GRID_SIZE, grid); // give random numbers between -1.0 and 1.0f
            //cout<<"*";

            // transform range of [-1.0, 1.0] to the range [0, 255]
            pix_brightness += 1.0;
            pix_brightness /= 2.0;
            pix_brightness *= 255;

            SDL_SetRenderDrawColor(renderer, pix_brightness, pix_brightness, pix_brightness, i+j);
            rect.x = j;
            rect.y = i;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    cout<<"final pix_brightness"<<pix_brightness<<endl;
    /*
    // Draw the Map
    SDL_Rect rect = {0, 0, GRID_SIZE, GRID_SIZE};
    float brightness__;
    for(int i = 0; i < WIDTH; i++){
        for(int j = 0; j < HEIGHT; j++){
            brightness__ = __pixel[j][i];
            SDL_SetRenderDrawColor(renderer, brightness__, brightness__, brightness__, 255);
            rect.x = i * GRID_SIZE;
            rect.y = j * GRID_SIZE;
            SDL_RenderFillRect(renderer, &rect);
        }
    */
    cout<<"Running Starting now..."<<endl;

    int running = 1;
    SDL_Event event;
    while(running){    
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                running = 0;
            }
        }
        //Present the screen
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
