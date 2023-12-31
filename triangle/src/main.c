#include <stdio.h>
#include <SDL.h>

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_EVERYTHING) > 0)
  {
    printf("SDL_Init failed");
  }
  SDL_Window* window = SDL_CreateWindow("SDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  SDL_Vertex vertices[3] = {
    { { 400, 150 }, { 255, 0, 0, 255 }, { 0 }},
    { { 200, 450 }, { 0, 0, 255, 255 }, { 0 }},
    { { 600, 450 }, { 0, 255, 0, 255 }, { 0 }},
  };
  
  int running = 1;
  while (running) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      if ((SDL_QUIT == ev.type) || (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode)) {
        running = 0;
        break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
