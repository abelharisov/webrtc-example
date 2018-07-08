#ifndef DINS_CALLCONTROLLER_HPP
#define DINS_CALLCONTROLLER_HPP

#include <string>
#include <webrtc/api/peerconnectioninterface.h>

#include "network/RequestHandlerInterface.hpp"

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
  explicit CallController(RequestSenderInterface& requestSender);

  void call(const std::string& callee);

  // RequestHandlerInterface
  bool onCallRequest(const std::string &caller, const std::string &data) override;
  bool onAnswerRequest(const std::string &caller, const std::string &data) override;
  bool onIceCandidateRequest(const std::string &caller, const std::string &data) override;

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
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> createPeerConnection();
  void configureOutputMedia(rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection);

  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peerConnectionFactory;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection;

  RequestSenderInterface& requestSender;
  State state;
  std::string remoteAddress;
};

}

#endif //DINS_CALLCONTROLLER_HPP
