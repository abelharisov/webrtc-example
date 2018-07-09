#ifndef DINS_SDLRENDERER_HPP
#define DINS_SDLRENDERER_HPP

#include "FrameConsumerInterface.hpp"

namespace render
{

class SDLRenderer : public FrameConsumerInterface
{
public:
  void onFrame(const FrameData &data, int width, int height) override;
};

}


#endif //DINS_SDLRENDERER_HPP
