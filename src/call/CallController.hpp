#ifndef DINS_CALLCONTROLLER_HPP
#define DINS_CALLCONTROLLER_HPP

#include <string>
#include <webrtc/api/peerconnectioninterface.h>
#include <Poco/JSON/Object.h>

#include "network/RequestHandlerInterface.hpp"
#include "render/FrameProducer.hpp"

namespace render
{
class FrameConsumerInterface;
}

namespace call
{

enum class State
{
  IDLE,
  CALL_OUT,
  CALL_IN,
};

class DummySetSessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
{
public:
  static DummySetSessionDescriptionObserver* create();
  void OnSuccess() override;
  void OnFailure(const std::string &error) override;
};

class RequestSenderInterface;

class CallController : public network::RequestHandlerInterface, public webrtc::PeerConnectionObserver, public webrtc::CreateSessionDescriptionObserver
{
public:
  CallController(render::FrameConsumerInterface& frameConsumer, RequestSenderInterface& requestSender, const std::string& localPort);

  void call(const std::string& callee);

  // RequestHandlerInterface
  bool onCallRequest(const std::string &callerHost, const std::string &data) override;
  bool onAnswerRequest(const std::string &callerHost, const std::string &data) override;
  bool onIceCandidateRequest(const std::string &callerHost, const std::string &data) override;

  // CreateSessionDescriptionObserver
  void OnSuccess(webrtc::SessionDescriptionInterface *desc) override;
  void OnFailure(const std::string &error) override;

  // PeerConnectionObserver
  void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
  void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
  void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
  void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) override;
  void OnRenegotiationNeeded() override;
  void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
  void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
  void OnIceCandidate(const webrtc::IceCandidateInterface *candidate) override;
  int AddRef() const override;
  int Release() const override;

private:
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> createPeerConnectionFactory();
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> createPeerConnection();
  void configureOutputMedia(rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection);
  Poco::JSON::Object::Ptr parseJson(const std::string& data);

  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection;

  render::FrameConsumerInterface& frameConsumer;
  RequestSenderInterface& requestSender;
  State state;
  std::string remoteAddress;
  std::string localPort;

  std::unique_ptr<render::FrameProducer> frameProducer;
};

}

#endif //DINS_CALLCONTROLLER_HPP
