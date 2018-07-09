#ifndef DINS_FRAMECONSUMERINTERFACE_HPP
#define DINS_FRAMECONSUMERINTERFACE_HPP

#include <vector>
#include <cstdint>

namespace render
{

using FrameData = std::vector<uint8_t>;

class FrameConsumerInterface
{
public:
  virtual ~FrameConsumerInterface() = default;

  virtual void onFrame(const FrameData& data, int width, int height) = 0;
};

}


#endif //DINS_FRAMECONSUMERINTERFACE_HPP
