#include "SDLRenderer.hpp"

#include <iostream>
#include <SDL.h>
#include <thread>

namespace render
{

SDLRenderer::SDLRenderer()
    : isInit(false)
{}

void SDLRenderer::init(int width, int height)
{
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
    return;
  }

  window = SDL_CreateWindow("WebRTC example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
  renderer = SDL_CreateRenderer(window, -1, 0);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_TARGET, width, height);

  std::thread([=]() {
    SDL_Event event;

    while (true)
    {
      SDL_PollEvent(&event);

      if (event.type == SDL_QUIT)
        break;

      if (event.type == SDL_USEREVENT)
      {
        SDL_UpdateTexture(texture, nullptr, &lastFrame[0], width * 4);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
      }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
  }).detach();

  isInit = true;
}

void SDLRenderer::onFrame(const render::FrameData &data, int width, int height)
{
  if (!isInit)
  {
    init(width, height);
  }

  lastFrame = data;

  SDL_Event event;
  event.type = SDL_USEREVENT;
  SDL_PushEvent(&event);

  std::cout << "SDLRenderer::onFrame" << std::endl;
}

}
