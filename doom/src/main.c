#include <stdio.h>
#include <SDL.h>
#include <stdlib.h>

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800
#define MAX_NUM_WALLS 10

struct Coord {
  int x;
  int y;
};

struct Line {
  struct Coord * first;
  struct Coord * second;
};

struct Map {
  struct Line lines[MAX_NUM_WALLS];
};

float get_projection(float x, float y) {
  return (SCREEN_WIDTH / 2.0f) * (x / y) + (SCREEN_WIDTH / 2.0f);
}

int get_height(double x, double y) {
  const double distance = sqrt(pow(x, 2) + pow(y, 2));
  const double minDistance = 20.0f;
  double height = (SCREEN_HEIGHT / 2.0f) - (distance / minDistance) * (SCREEN_HEIGHT / 2.0f);

  if (height < 0.0f) {
    return 0.0f;
  }

  return (int) height;
}

void draw_wall(SDL_Renderer * renderer, struct Coord p1, struct Coord p2) {
  int first_height = get_height(p1.x, p1.y);
  int second_height = get_height(p2.x, p2.y);
  float first_projection = get_projection(p1.x, p1.y);
  float second_projection = get_projection(p2.x, p2.y);
  SDL_Vertex vertices[4] = {
    { { (float) first_projection, (float) ((SCREEN_HEIGHT / 2.0) + (first_height / 2.0)) }, { 255, 0, 0, 255 }, { 0 }},
    { { (float) first_projection, (float) ((SCREEN_HEIGHT / 2.0) - (first_height / 2.0)) }, { 255, 0, 0, 255 }, { 0 }},
    { { (float) second_projection, (float) ((SCREEN_HEIGHT / 2.0) + (second_height / 2.0)) }, { 0, 0, 255, 255 }, { 0 }},
    { { (float) second_projection, (float) ((SCREEN_HEIGHT / 2.0) - (second_height / 2.0)) }, { 0, 0, 255, 255 }, { 0 }},
  };

  const int indices[6] = {
     0,1,2,
     1,2,3
  };

  SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
}

void draw_vert_line(SDL_Renderer * renderer, int x, int y) {

  int height = get_height(x, y);
  int projection = (int) get_projection(x, y);
  SDL_RenderDrawLine(renderer, projection, (SCREEN_HEIGHT / 2) + height / 2, projection, (SCREEN_HEIGHT / 2) - height / 2);
}

int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
    printf("SDL_Init failed with error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_Window* window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  int running = 1;

  while (running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if ((SDL_QUIT == ev.type)) {
        running = 0;
        break;
      }
    }

    // SDL_Vertex triangle[3] = {
    //   { { 400.0, 150.0 }, { 255, 0, 0, 255 }, { 0 }},
    //   { { 200.0, 450.0 }, { 0, 0, 255, 255 }, { 0 }},
    //   { { 600.0, 450.0 }, { 0, 255, 0, 255 }, { 0 }},
    // };


    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    struct Coord p1 = {-2, 8};
    struct Coord p2 = {0, 10};
    struct Coord p3 = {4, 11};
    struct Coord p4 = {5, 15};

    draw_wall(renderer, p1, p2);
    draw_wall(renderer, p3, p4);
    // SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
    // SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
