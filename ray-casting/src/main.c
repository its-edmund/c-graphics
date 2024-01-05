#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL.h>

#define SCREEN_HEIGHT 180
#define SCREEN_WIDTH 320
#define FOV 45
#define PLAYER_SPEED 5

typedef float f32;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef struct point_s { f32 x, y; } point;

static struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  u32 buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
  u8 running;
  
  struct {
    point pos;
    f32 dir;
  } player;
} state;

static u8 MAPDATA[8 * 8] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 2, 2, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1, // Here
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
};

f32 calculate_angle(u16 column) {
  return (column - SCREEN_WIDTH / 2.0f) / (SCREEN_WIDTH / 2.0f) * (FOV / 2.0f);
}

point find_intersection(f32 offset_angle) {
  f32 angle = offset_angle + state.player.dir;
  f32 slope = (f32) tan(angle * M_PI / 180.0);
  printf("Slope: %f", slope);
  for (u16 i = 0; i < 50; i++) {
    u16 x, y;
    if (angle <= 270 && angle >= 90) {
      x = state.player.pos.x - i;
      y = state.player.pos.y - (i * slope);
    } else {
      x = state.player.pos.x + i;
      y = state.player.pos.y + (i * slope);
    }
    printf("Walking: {%d, %d}\n", x, y);
    if (MAPDATA[(((u16) x / 10) * 8) + ((u16) y / 10)] != 0) {
      return (point) {x, y};
    }
  }
  return (point) {1000, 1000};
}

void update_display() {
  // u16 i = SCREEN_WIDTH / 2;
  // f32 offset_angle = calculate_angle(i);
  // point point = find_intersection(offset_angle);
  // printf("Player: {%f, %f}\n", state.player.pos.x, state.player.pos.y);
  // printf("Intersection :{%f, %f}\n", point.x, point.y);
  // f32 distance = sqrt(pow(point.x - state.player.pos.x, 2.0f) + pow(point.y - state.player.pos.y, 2.0f));
  // f32 height = (1 - fminf(distance, 100.0f) / 100.0f) * SCREEN_HEIGHT;
  // for (int h = (SCREEN_HEIGHT / 2) - ((int) height / 2); h < (SCREEN_HEIGHT / 2) + ((int) height / 2); h++) {
  //   state.buffer[SCREEN_WIDTH * h + i] = 0xff00ff00;
  // }
  for(u16 i = 0; i < SCREEN_WIDTH; i++) {
    f32 offset_angle = calculate_angle(i);
    point point = find_intersection(offset_angle);
    printf("%f, %f\n", point.x, point.y);
    f32 distance = sqrt(pow(point.x - state.player.pos.x, 2.0f) + pow(point.y - state.player.pos.y, 2.0f));
    f32 height = (1 - fminf(distance, 100.0f) / 100.0f) * SCREEN_HEIGHT;
    u16 top = (SCREEN_HEIGHT / 2) - ((int) height / 2);
    u16 bottom = (SCREEN_HEIGHT / 2) + ((int) height / 2);
    for (int h = 0; h < top; h++) {
      state.buffer[SCREEN_WIDTH * h + i] = 0xff000000;
    }
    for (int h = top; h < bottom; h++) {
      switch (MAPDATA[(((u16) point.x / 10) * 8) + ((u16) point.y / 10)]) {
        case 1:
          state.buffer[SCREEN_WIDTH * h + i] = 0xff00ff00;
          break;
        case 2:
          state.buffer[SCREEN_WIDTH * h + i] = 0xff0000ff;
          break;
      }
    }
    for (int h = bottom; h < SCREEN_HEIGHT; h++) {
      state.buffer[SCREEN_WIDTH * h + i] = 0xff000000;
    }
  }
  printf("\n");
}

void update_minimap() {

}

int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    printf("SDL_Init had an error: %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  state.window = SDL_CreateWindow("Totally Wolfenstein", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * 4, SCREEN_HEIGHT * 4, SDL_WINDOW_SHOWN);
  state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  state.player.pos = (point) {40.0f, 30.0f};
  state.texture = SDL_CreateTexture(state.renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
  state.running = 1;

  // u32 last_ticks = SDL_GetTicks();

  while (state.running) {
    SDL_Event ev;
    // u32 current_ticks = SDL_GetTicks();

    // f32 delta_time = (current_ticks - last_ticks) * 00.1f;

    while (SDL_PollEvent(&ev)) {
      if ((SDL_QUIT == ev.type)) {
        state.running = 0;
        break;
      } else if (SDL_KEYDOWN == ev.type) {
        switch(ev.key.keysym.sym) {
          case SDLK_ESCAPE:
            state.running = 0;
            break;
          case SDLK_LEFT:
            state.player.dir = (((int) state.player.dir - 3) % 360 + 360) % 360;
            break;
          case SDLK_RIGHT:
            state.player.dir = (((int) state.player.dir + 3) % 360 + 360) % 360;
            break;
        }
      }
    }

    update_display();
    printf("%f\n", state.player.dir);
    SDL_UpdateTexture(state.texture, NULL, state.buffer, SCREEN_WIDTH * 4);
    SDL_RenderClear(state.renderer);
    SDL_RenderCopyEx(state.renderer, state.texture, NULL, NULL, 0, NULL, SDL_FLIP_VERTICAL);
    SDL_RenderPresent(state.renderer);
  }

  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_DestroyTexture(state.texture);
  SDL_Quit();
}
