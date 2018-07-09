#ifndef DINS_SDLRENDERER_HPP
#define DINS_SDLRENDERER_HPP

#include "FrameConsumerInterface.hpp"

namespace render
{

class SDLRenderer : public FrameConsumerInterface
{
public:
  SDLRenderer();
  void onFrame(const FrameData &data, int width, int height) override;

private:
  void init(int width, int height);

  bool isInit;
  FrameData lastFrame;
};

}


#endif //DINS_SDLRENDERER_HPP
