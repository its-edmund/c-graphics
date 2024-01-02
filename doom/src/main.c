#include <stdint.h>
#include <stdio.h>
#include <SDL.h>
#include <stdlib.h>
#include <math.h>

typedef float f32;
typedef double f64;
typedef uint32_t u32;
typedef uint64_t u64;
typedef struct point_s { f32 x, y; } point;

#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800
#define MAX_NUM_WALLS 10
#define PLAYER_SPEED 10.0f


struct Line {
  struct point *first;
  struct point *second;
};

struct Map {
  struct Line lines[MAX_NUM_WALLS];
};

static struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  
  struct {
    point pos;
    f32 x_velo;
    f32 y_velo;
    f32 dir;
  } player;
} state;

float get_projection(float x, float y) {
  // const double distance = sqrt(pow(x + state.player.pos.x, 2) + pow(y + state.player.pos.y, 2));
  return (SCREEN_WIDTH / 2.0f) * ((x + state.player.pos.x) / (y + state.player.pos.y)) + (SCREEN_WIDTH / 2.0f);
}

int get_height(double x, double y) {
  (void) x;
  // const double distance = sqrt(pow(x + state.player.pos.x, 2) + pow(y + state.player.pos.y, 2));
  // const double minDistance = 20.0f;
  if (y < 0) {
    return 0;
  }
  
  double height = (SCREEN_HEIGHT / 2.0f) * (2.0f / (y + state.player.pos.y));

  if (height < 0.0f) {
    return 0.0f;
  }

  return (int) height;
}

void draw_wall(point p1, point p2) {
  int first_height = get_height(p1.x, p1.y);
  int second_height = get_height(p2.x, p2.y);
  f32 first_projection = get_projection(p1.x, p1.y);
  f32 second_projection = get_projection(p2.x, p2.y);
  
  f64 angle = atan((p1.y - p2.y) / (p1.x - p2.x)) * (180/M_PI);

  if (angle < 0) {
    angle = 180 + angle;
  } else if (angle < 90) {
    angle = 0;
  }


  SDL_Vertex vertices[4] = {
    // { { (float) first_projection, (float) ((SCREEN_HEIGHT / 2.0) + (first_height / 2.0)) }, { 255, 255, 255, 255 }, { 0 }},
    // { { (float) first_projection, (float) ((SCREEN_HEIGHT / 2.0) - (first_height / 2.0)) }, { 255, 255, 255, 255 }, { 0 }},
    // { { (float) second_projection, (float) ((SCREEN_HEIGHT / 2.0) + (second_height / 2.0)) }, { 255, 255, 255, 255 }, { 0 }},
    // { { (float) second_projection, (float) ((SCREEN_HEIGHT / 2.0) - (second_height / 2.0)) }, { 255, 255, 255, 255 }, { 0 }},
    { { (float) first_projection, (float) (SCREEN_HEIGHT / 2.0f) + first_height }, { 255 - angle, 255 - angle, 255 - angle, 255 }, { 0 }},
    { { (float) first_projection, (float) (SCREEN_HEIGHT / 2.0f) - first_height }, { 255 - angle, 255 - angle, 255 - angle, 255 }, { 0 }},
    { { (float) second_projection, (float) (SCREEN_HEIGHT / 2.0f) + second_height }, { 255 - angle, 255 - angle, 255 - angle, 255 }, { 0 }},
    { { (float) second_projection, (float) (SCREEN_HEIGHT / 2.0f) - second_height }, { 255 - angle, 255 - angle, 255 - angle, 255 }, { 0 }},
  };

  const int indices[6] = {
     0,1,2,
     1,2,3
  };

  if ((first_projection < 0 && second_projection < 0) || (first_projection > SCREEN_WIDTH && second_projection > SCREEN_WIDTH) || (p2.y < state.player.pos.y && p1.y < state.player.pos.y)) {
    return;
  }
  SDL_RenderGeometry(state.renderer, NULL, vertices, 4, indices, 6);
}

// void draw_vert_line(SDL_Renderer * renderer, f32 x, f32 y) {
//   int height = get_height(x, y);
//   f32 projection = get_projection(x, y);
//   SDL_RenderDrawLine(renderer, (int) projection, (SCREEN_HEIGHT / 2) + height / 2, (int) projection, (SCREEN_HEIGHT / 2) - height / 2);
// }

int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING) > 0) {
    printf("SDL_Init failed with error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  state.window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  state.player.pos = (point) {0.0f, 0.0f};

  int running = 1;

  u32 last_ticks = SDL_GetTicks();

  while (running) {
    SDL_Event ev;

    u32 current_ticks = SDL_GetTicks();

    f32 delta_time = (current_ticks - last_ticks) * 0.001f;

    last_ticks = current_ticks;

    while (SDL_PollEvent(&ev)) {
      if ((SDL_QUIT == ev.type)) {
        running = 0;
        break;
      } else if (SDL_KEYDOWN == ev.type) {
        switch (ev.key.keysym.sym) {
          case SDLK_w:
            state.player.y_velo = -PLAYER_SPEED;
            break;
          case SDLK_s:
            state.player.y_velo = PLAYER_SPEED;
            break;
          case SDLK_a:
            state.player.x_velo = PLAYER_SPEED;
            break;
          case SDLK_d:
            state.player.x_velo = -PLAYER_SPEED;
            break;
        }
      } else if (SDL_KEYUP == ev.type) {
        switch (ev.key.keysym.sym) {
          case SDLK_w:
          case SDLK_s:
            state.player.y_velo = 0;
            break;
          case SDLK_a:
          case SDLK_d:
            state.player.x_velo = 0;
            break;
        }
      }
    }
    
    state.player.pos.x += delta_time * state.player.x_velo;
    state.player.pos.y += delta_time * state.player.y_velo;

    // SDL_Vertex triangle[3] = {
    //   { { 400.0, 150.0 }, { 255, 0, 0, 255 }, { 0 }},
    //   { { 200.0, 450.0 }, { 0, 0, 255, 255 }, { 0 }},
    //   { { 600.0, 450.0 }, { 0, 255, 0, 255 }, { 0 }},
    // };


    SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
    SDL_RenderClear(state.renderer);
    SDL_SetRenderDrawColor(state.renderer, 255, 0, 0, 255);

    point p1 = {-2.0f, 8.0f};
    point p2 = {0.0f, 10.0f};
    // point p5 = {0.0f, 10.0f};
    // point p6 = {2.0f, 12.0f};
    point p3 = {4.0f, 11.0f};
    point p4 = {2.0f, 15.0f};

    draw_wall(p1, p2);
    // draw_wall(p5, p6);
    draw_wall(p3, p4);
    // SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
    // SDL_RenderGeometry(renderer, NULL, triangle, 3, NULL, 0);
    SDL_RenderPresent(state.renderer);
  }

  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();
}
