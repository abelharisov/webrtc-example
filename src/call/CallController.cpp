#include "CallController.hpp"

#include <memory>
#include <sstream>
#include <webrtc/modules/video_capture/video_capture_factory.h>
#include <webrtc/media/engine/webrtcvideocapturerfactory.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>
#include <Poco/JSON/Object.h>

#include "RequestSenderInterface.hpp"

namespace call
{

CallController::CallController(RequestSenderInterface& requestSender)
    : peerConnectionFactory(webrtc::CreatePeerConnectionFactory()),
      requestSender(requestSender),
      state(State::IDLE)
{}

void CallController::call(const std::string &callee)
{
  if (state != State::IDLE)
  {
    return;
  }

  state = State::CALL_OUT;
  remoteAddress = callee;

  peerConnection = createPeerConnection();
  configureOutputMedia(peerConnection);
  peerConnection->CreateOffer(this, nullptr);
}

bool CallController::onCallRequest(const std::string &caller, const std::string &data)
{
  if (state != State::IDLE)
  {
    return false;
  }

  state = State::CALL_IN;
  remoteAddress = caller;

  Poco::JSON::Parser parser;
  auto callData = parser.parse(data).extract<Poco::JSON::Object>();
  auto type = callData.getValue<std::string>("type");
  auto sdp = callData.getValue<std::string>("sdp");

  webrtc::PeerConnectionInterface::RTCConfiguration rtcConfiguration;
  peerConnection = peerConnectionFactory->CreatePeerConnection(rtcConfiguration, nullptr, nullptr, this);
  configureOutputMedia(peerConnection);

  auto sessionDescription = webrtc::CreateSessionDescription(type, sdp, nullptr);
  peerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::create(), sessionDescription);
  peerConnection->CreateAnswer(this, nullptr);

  return true;
}

bool CallController::onAnswerRequest(const std::string &caller, const std::string &data)
{
  Poco::JSON::Parser parser;
  auto callData = parser.parse(data).extract<Poco::JSON::Object>();
  auto type = callData.getValue<std::string>("type");
  auto sdp = callData.getValue<std::string>("sdp");

  auto sessionDescription = webrtc::CreateSessionDescription(type, sdp, nullptr);
  peerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::create(), sessionDescription);

  return true;
}

bool CallController::onIceCandidateRequest(const std::string &caller, const std::string &data)
{
  Poco::JSON::Parser parser;
  auto iceData = parser.parse(data).extract<Poco::JSON::Object>();
  auto sdpMid = iceData.getValue<std::string>("sdpMid");
  auto sdpMLineIndexMid = iceData.getValue<int>("sdpMLineIndexMid");
  auto sdp = iceData.getValue<std::string>("candidate");

  auto iceCandidate = webrtc::CreateIceCandidate(sdpMid, sdpMLineIndexMid, sdp, nullptr);
  peerConnection->AddIceCandidate(iceCandidate);

  return true;
}


void CallController::OnSuccess(webrtc::SessionDescriptionInterface *desc)
{
  peerConnection->SetLocalDescription(DummySetSessionDescriptionObserver::create(), desc);
  std::string description;
  desc->ToString(&description);

  Poco::JSON::Object jsonObject;
  jsonObject.set("type", desc->type());
  jsonObject.set("sdp", description);

  std::stringstream stream;
  Poco::JSON::Stringifier::stringify(jsonObject, stream);

  auto path = (state == State::CALL_OUT ? "call" : "answer");

  requestSender.sendRequest(remoteAddress, path, stream.str());
}

void CallController::OnFailure(const std::string &error)
{
  throw std::runtime_error("Can not create session description: " + error);
}


void CallController::OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state)
{}

void CallController::OnRenegotiationNeeded()
{}

void CallController::OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state)
{}

void CallController::OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state)
{}

void CallController::OnIceCandidate(const webrtc::IceCandidateInterface *candidate)
{
  std::string candidateString;
  candidate->ToString(&candidateString);
  requestSender.sendRequest(remoteAddress, "iceCandidate", candidateString);
}

void CallController::configureOutputMedia(rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection)
{
  std::unique_ptr<cricket::VideoCapturer> videoCapturer;
  cricket::WebRtcVideoDeviceCapturerFactory factory;
  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> deviceInfo(webrtc::VideoCaptureFactory::CreateDeviceInfo());
  for (int i = 0; i < deviceInfo->NumberOfDevices(); ++i)
  {
    std::string name(256, 0);
    std::string id(256, 0);
    deviceInfo->GetDeviceName(i, &name[0], name.size(), &id[0], id.size());
    videoCapturer = factory.Create(cricket::Device(name, id));
    if (videoCapturer) {
      break;
    }
  }

  if (!videoCapturer) {
    throw std::runtime_error("Can not configure video");
  }

  auto audioTrack = peerConnectionFactory->CreateAudioTrack("", peerConnectionFactory->CreateAudioSource(nullptr));
  auto videoTrack = peerConnectionFactory->CreateVideoTrack("", peerConnectionFactory->CreateVideoSource(std::move(videoCapturer), nullptr));
  auto stream = peerConnectionFactory->CreateLocalMediaStream("");
  stream->AddTrack(audioTrack);
  stream->AddTrack(videoTrack);

  peerConnection->AddStream(stream);
}

rtc::scoped_refptr<webrtc::PeerConnectionInterface> CallController::createPeerConnection()
{
  webrtc::PeerConnectionInterface::RTCConfiguration rtcConfiguration;
  webrtc::PeerConnectionInterface::IceServer server;
  server.uri = "stun:stun.l.google.com:19302";
  rtcConfiguration.servers.push_back(server);

  return peerConnectionFactory->CreatePeerConnection(rtcConfiguration, nullptr, nullptr, nullptr, this);
}


void CallController::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
  std::cout << "new remote stream" << std::endl;
}

void CallController::OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{

}

void CallController::OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{

}

int CallController::AddRef() const
{
  return 0;
}

int CallController::Release() const
{
  return 0;
}

void DummySetSessionDescriptionObserver::OnSuccess()
{}

void DummySetSessionDescriptionObserver::OnFailure(const std::string &error)
{
  throw std::runtime_error("Can not set session description: " + error);
}

DummySetSessionDescriptionObserver *DummySetSessionDescriptionObserver::create()
{
  return new rtc::RefCountedObject<DummySetSessionDescriptionObserver>();
}

}
