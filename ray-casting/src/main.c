#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL.h>

#define SCREEN_HEIGHT 216
#define SCREEN_WIDTH 384
#define MAP_HEIGHT 8
#define MAP_WIDTH 8
#define FOV 45
#define PLAYER_SPEED 0.04f
#define DEG_TO_RAD(degrees) ((degrees) * M_PI / 180.0)
#define NEG_VEC(v2)

typedef float f32;
typedef double f64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef struct vf2d_s { f32 x, y; } vf2d;
typedef struct vi2d_s { i32 x, y; } vi2d;

static struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  u32 buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
  u8 running;
  
  struct {
    vf2d pos;
    vf2d dir;
  } player;
} state;

static u8 MAPDATA[MAP_HEIGHT * MAP_WIDTH] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 2, 2, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1, // Here
  1, 0, 2, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
};

vf2d normalize_vector(vf2d vector) {
  f32 magnitude = (f32) sqrt(vector.x * vector.x + vector.y * vector.y);
  return (vf2d) {vector.x / magnitude, vector.y / magnitude};
}

vf2d rotate_vector(vf2d vector, f32 angle) {
  f32 x_prime = vector.x * cos(angle) - vector.y * sin(angle);
  f32 y_prime = vector.x * sin(angle) + vector.y * cos(angle);
  return (vf2d) {x_prime, y_prime};
}


f32 calculate_angle(u16 column) {
  return (column - SCREEN_WIDTH / 2.0f) / (SCREEN_WIDTH / 2.0f) * (FOV / 2.0f);
}

vf2d find_intersection(f32 offset_angle) {
  vf2d player_pos = state.player.pos;
  vf2d direction = normalize_vector(rotate_vector(state.player.dir, DEG_TO_RAD(offset_angle)));
  vf2d step_size = { sqrt(1 + (direction.y / direction.x) * (direction.y / direction.x)), sqrt(1 + (direction.x / direction.y) * (direction.x / direction.y)) };

  vi2d cell_location = { (u32) state.player.pos.x, (u32) state.player.pos.y };
  vf2d ray_length;

  vi2d step;

  if (direction.x < 0) {
    step.x = -1;
    ray_length.x = (player_pos.x - (f32) cell_location.x) * step_size.x;
  } else {
    step.x = 1;
    ray_length.x = ((f32) (cell_location.x + 1) - player_pos.x) * step_size.x;
  }

  if (direction.y < 0) {
    step.y = -1;
    ray_length.y = (player_pos.y - (f32) cell_location.y) * step_size.y;
  } else {
    step.y = 1;
    ray_length.y = ((f32) (cell_location.y + 1) - player_pos.y) * step_size.y;
  }

  u8 tile_found = 0;
  f32 max_distance = 100.0f;
  f32 distance = 0.0f;
  while (!tile_found && distance < max_distance) {
    if (ray_length.x < ray_length.y) {
      cell_location.x += step.x;
      distance = ray_length.x;
      ray_length.x += step_size.x;
    } else {
      cell_location.y += step.y;
      distance = ray_length.y;
      ray_length.y += step_size.y;
    }

    if (cell_location.x >= 0 && cell_location.x < MAP_WIDTH && cell_location.y >= 0 && cell_location.y < MAP_HEIGHT) {
      if (MAPDATA[cell_location.y * MAP_WIDTH + cell_location.x] != 0) {
        tile_found = 1;
      }
    }
  }

  vf2d intersection = {1000.0f, 1000.0f};
  if (tile_found) {
    intersection.x = player_pos.x + direction.x * distance;
    intersection.y = player_pos.y + direction.y * distance;
  }

  printf("cell location: {%d, %d}\n", cell_location.x, cell_location.y);
  printf("cell intersection: {%f, %f}\n", intersection.x, intersection.y);
  return intersection;
}

void update_display() {
  // for(u16 i = SCREEN_WIDTH / 2; i <= SCREEN_WIDTH / 2; i++) {
  for(u16 i = 0; i < SCREEN_WIDTH; i++) {
    f32 offset_angle = calculate_angle(i);
    // printf("%f\n", offset_angle);
    vf2d point = find_intersection(offset_angle);
    // f32 euclidean_distance = sqrt(pow(point.x - state.player.pos.x, 2) + pow(point.y - state.player.pos.y, 2));
    // f32 distance = cos(DEG_TO_RAD(offset_angle)) * euclidean_distance;

    // INTERSECTION CALCULATION
    vf2d player_pos = state.player.pos;
    vf2d direction = normalize_vector(rotate_vector(state.player.dir, DEG_TO_RAD(offset_angle)));
    vf2d step_size = { sqrt(1 + (direction.y / direction.x) * (direction.y / direction.x)), sqrt(1 + (direction.x / direction.y) * (direction.x / direction.y)) };

    vi2d cell_location = { (u32) state.player.pos.x, (u32) state.player.pos.y };
    vf2d ray_length;

    vi2d step;

    if (direction.x < 0) {
      step.x = -1;
      ray_length.x = (player_pos.x - (f32) cell_location.x) * step_size.x;
    } else {
      step.x = 1;
      ray_length.x = ((f32) (cell_location.x + 1) - player_pos.x) * step_size.x;
    }

    if (direction.y < 0) {
      step.y = -1;
      ray_length.y = (player_pos.y - (f32) cell_location.y) * step_size.y;
    } else {
      step.y = 1;
      ray_length.y = ((f32) (cell_location.y + 1) - player_pos.y) * step_size.y;
    }

    u8 tile_found = 0;
    u8 side = 0;
    f32 max_distance = 100.0f;
    f32 distance = 0.0f;
    
    while (!tile_found && distance < max_distance) {
      if (ray_length.x < ray_length.y) {
        cell_location.x += step.x;
        distance = ray_length.x;
        ray_length.x += step_size.x;
        side = 0;
      } else {
        cell_location.y += step.y;
        distance = ray_length.y;
        ray_length.y += step_size.y;
        side = 1;
      }

      if (cell_location.x >= 0 && cell_location.x < MAP_WIDTH && cell_location.y >= 0 && cell_location.y < MAP_HEIGHT) {
        if (MAPDATA[cell_location.y * MAP_WIDTH + cell_location.x] != 0) {
          tile_found = 1;
        }
      }
    }

    vf2d intersection = {1000.0f, 1000.0f};
    if (tile_found) {
      intersection.x = player_pos.x + direction.x * distance;
      intersection.y = player_pos.y + direction.y * distance;
    }


    // DRAWING

    f32 height = SCREEN_HEIGHT / distance;
    i32 top = (SCREEN_HEIGHT / 2) - ((u32) (height / 2));
    if (top < 0) {
      top = 0;
    }
    i32 bottom = (SCREEN_HEIGHT / 2) + ((u32) (height / 2));
    if (bottom >= SCREEN_HEIGHT) bottom = SCREEN_HEIGHT - 1;
    printf("%d: %d, %d, point: {%f, %f}, color: %d\n", i, top, bottom, point.x, point.y, MAPDATA[(u16) point.y * MAP_WIDTH + (u16) point.x]);
    u32 color;

    switch (MAPDATA[cell_location.y * MAP_WIDTH + cell_location.x]) {
      case 1:
        color = 0xff00ff00;
        break;
      case 2:
        color = 0xff0000ff;
        break;
    }

    if (side) {
      const u32
        br = ((color & 0xFF00FF) * 0xC0) >> 8,
        g  = ((color & 0x00FF00) * 0xC0) >> 8;

      color = 0xFF000000 | (br & 0xFF00FF) | (g & 0x00FF00);
    }


    for (i32 h = 0; h < top; h++) {
      state.buffer[SCREEN_WIDTH * h + i] = 0xff000000;
    }
    for (i32 h = top; h < bottom; h++) {
      state.buffer[SCREEN_WIDTH * h + i] = color;
    }
    for (i32 h = bottom; h < SCREEN_HEIGHT; h++) {
      state.buffer[SCREEN_WIDTH * h + i] = 0xff000000;
    }
  }
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
  state.player.pos = (vf2d) {5.0f, 5.0f};
  state.player.dir = (vf2d) {1.0f, 0.0f};
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
        }
      }
    }

    const u8 *keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_UP]) {
      state.player.pos.x += state.player.dir.x * PLAYER_SPEED;
      state.player.pos.y += state.player.dir.y * PLAYER_SPEED;
    }

    if (keystate[SDL_SCANCODE_DOWN]) {
      state.player.pos.x -= state.player.dir.x * PLAYER_SPEED;
      state.player.pos.y -= state.player.dir.y * PLAYER_SPEED;
    }

    if (keystate[SDL_SCANCODE_LEFT]) {
      state.player.dir = rotate_vector(state.player.dir, (f32) DEG_TO_RAD(-1.5));
    }

    if (keystate[SDL_SCANCODE_RIGHT]) {
      state.player.dir = rotate_vector(state.player.dir, (f32) DEG_TO_RAD(1.5));
    }


    update_display();
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
