#ifndef DINS_WEBRTC_FRAME_RENDERER_HPP
#define DINS_WEBRTC_FRAME_RENDERER_HPP

#include <webrtc/media/base/videosinkinterface.h>
#include <webrtc/api/video/video_frame.h>
#include <vector>
#include <webrtc/api/mediastreaminterface.h>

namespace render
{

class FrameConsumerInterface;

class FrameProducer : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
  FrameProducer(FrameConsumerInterface& frameConsumer, rtc::scoped_refptr<webrtc::VideoTrackInterface> track);
  ~FrameProducer() override;
  void OnFrame(const webrtc::VideoFrame &frame) override;

private:
  void resize(int width, int height);

  int width;
  int height;
  std::vector<uint8_t> image;
  rtc::scoped_refptr<webrtc::VideoTrackInterface> track;

  FrameConsumerInterface& frameConsumer;
};

}


/*

  class VideoRenderer : public rtc::VideoSinkInterface<webrtc::VideoFrame> {
   public:
    VideoRenderer(GtkMainWnd* main_wnd,
                  webrtc::VideoTrackInterface* track_to_render);
    virtual ~VideoRenderer();

    // VideoSinkInterface implementation
    void OnFrame(const webrtc::VideoFrame& frame) override;

    const uint8_t* image() const { return image_.get(); }

    int width() const {
      return width_;
    }

    int height() const {
      return height_;
    }

   protected:
    void SetSize(int width, int height);
    std::unique_ptr<uint8_t[]> image_;
    int width_;
    int height_;
    GtkMainWnd* main_wnd_;
    rtc::scoped_refptr<webrtc::VideoTrackInterface> rendered_track_;
  };

 */


#endif //DINS_WEBRTC_FRAME_RENDERER_HPP
