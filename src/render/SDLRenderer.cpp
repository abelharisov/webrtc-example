#include "SDLRenderer.hpp"

#include <iostream>

namespace render
{

void SDLRenderer::onFrame(const render::FrameData &data, int width, int height)
{
  std::cout << "SDLRenderer::onFrame" << std::endl;
}

}
