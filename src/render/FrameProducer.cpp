#include "FrameProducer.hpp"

#include <webrtc/common_video/libyuv/include/webrtc_libyuv.h>

#include "FrameConsumerInterface.hpp"

namespace render
{

FrameProducer::FrameProducer(FrameConsumerInterface& frameConsumer, rtc::scoped_refptr<webrtc::VideoTrackInterface> track)
    : width(0),
      height(0),
      track(track),
      frameConsumer(frameConsumer)
{
  track->AddOrUpdateSink(this, rtc::VideoSinkWants());
}

FrameProducer::~FrameProducer()
{
  track->RemoveSink(this);
}

void FrameProducer::OnFrame(const webrtc::VideoFrame &frame)
{
  auto frameBuffer = frame.video_frame_buffer();
  resize(frameBuffer->width(), frameBuffer->height());

  webrtc::ConvertFromI420(frame, webrtc::kBGRA, 0, &image[0]);

  frameConsumer.onFrame(image, frameBuffer->width(), frameBuffer->height());
}

void FrameProducer::resize(int width, int height)
{
  if (this->width == width && this->height == height)
  {
    return;
  }

  this->width = width;
  this->height = height;
  image = std::vector<uint8_t>(width * height * 4);
}

}
