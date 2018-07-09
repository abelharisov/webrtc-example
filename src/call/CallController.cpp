#include "CallController.hpp"

#include <memory>
#include <sstream>
#include <webrtc/modules/video_capture/video_capture_factory.h>
#include <webrtc/media/engine/webrtcvideocapturerfactory.h>
#include <webrtc/base/ssladapter.h>
#include <webrtc/base/thread.h>
#include <webrtc/base/physicalsocketserver.h>

#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Stringifier.h>
#include <Poco/JSON/Object.h>
#include <thread>

#include "RequestSenderInterface.hpp"

namespace call
{

CallController::CallController(render::FrameConsumerInterface& frameConsumer, RequestSenderInterface& requestSender, const std::string& localPort)
    : peerConnectionFactory(createPeerConnectionFactory()),
      frameConsumer(frameConsumer),
      requestSender(requestSender),
      state(State::IDLE),
      localPort(localPort)
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

  peerConnection->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
}

bool CallController::onCallRequest(const std::string &callerHost, const std::string &data)
{
  if (state != State::IDLE)
  {
    return false;
  }

  state = State::CALL_IN;

  auto json = parseJson(data);
  auto type = json->getValue<std::string>("type");
  auto sdp = json->getValue<std::string>("sdp");
  auto callerPort = json->getValue<std::string>("port");

  remoteAddress = callerHost + std::string(":") + callerPort;

  peerConnection = createPeerConnection();
  configureOutputMedia(peerConnection);

  auto sessionDescription = webrtc::CreateSessionDescription(type, sdp, nullptr);
  peerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::create(), sessionDescription);
  peerConnection->CreateAnswer(this, nullptr);

  return true;
}

bool CallController::onAnswerRequest(const std::string &callerHost, const std::string &data)
{
  if (remoteAddress.find(callerHost) == std::string::npos) {
    return false;
  }

  auto json = parseJson(data);
  auto type = json->getValue<std::string>("type");
  auto sdp = json->getValue<std::string>("sdp");

  auto sessionDescription = webrtc::CreateSessionDescription(type, sdp, nullptr);
  peerConnection->SetRemoteDescription(DummySetSessionDescriptionObserver::create(), sessionDescription);

  return true;
}

bool CallController::onIceCandidateRequest(const std::string &callerHost, const std::string &data)
{
  if (remoteAddress.find(callerHost) == std::string::npos) {
    return false;
  }

  auto json = parseJson(data);
  auto sdpMid = json->getValue<std::string>("sdpMid");
  auto sdpMLineIndexMid = json->getValue<int>("sdpMLineIndexMid");
  auto sdp = json->getValue<std::string>("candidate");

  auto iceCandidate = webrtc::CreateIceCandidate(sdpMid, sdpMLineIndexMid, sdp, nullptr);

  // TODO run in peer connection thread
  std::thread([this, iceCandidate]() {
    peerConnection->AddIceCandidate(iceCandidate);
  }).detach();

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
  jsonObject.set("port", localPort);

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

  Poco::JSON::Object jsonObject;
  jsonObject.set("sdpMid", candidate->sdp_mid());
  jsonObject.set("sdpMLineIndexMid", candidate->sdp_mline_index());
  jsonObject.set("candidate", candidateString);

  std::stringstream stream;
  Poco::JSON::Stringifier::stringify(jsonObject, stream);

  requestSender.sendRequest(remoteAddress, "iceCandidate", stream.str());
}

void CallController::configureOutputMedia(rtc::scoped_refptr<webrtc::PeerConnectionInterface> peerConnection)
{
  std::unique_ptr<cricket::VideoCapturer> videoCapturer;
  cricket::WebRtcVideoDeviceCapturerFactory factory;
  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> deviceInfo(webrtc::VideoCaptureFactory::CreateDeviceInfo());
  for (int i = 0; i < deviceInfo->NumberOfDevices(); ++i)
  {
    const uint32_t size = 256;
    char name[size] = {0};
    char id[size] = {0};
    deviceInfo->GetDeviceName(i, name, size, id, size);
    videoCapturer = factory.Create(cricket::Device(name, id));
    if (videoCapturer) {
      break;
    }
  }

  if (!videoCapturer) {
    throw std::runtime_error("Can not configure video");
  }

  auto audioTrack = peerConnectionFactory->CreateAudioTrack("audio_label", peerConnectionFactory->CreateAudioSource(cricket::AudioOptions()));
  auto videoTrack = peerConnectionFactory->CreateVideoTrack("video_label", peerConnectionFactory->CreateVideoSource(std::move(videoCapturer), nullptr));

  peerConnection->AddTrack(audioTrack, {});
  peerConnection->AddTrack(videoTrack, {});
}

rtc::scoped_refptr<webrtc::PeerConnectionInterface> CallController::createPeerConnection()
{
  webrtc::PeerConnectionInterface::RTCConfiguration rtcConfiguration;
  rtcConfiguration.enable_dtls_srtp = rtc::Optional<bool>(true);
  webrtc::PeerConnectionInterface::IceServer server;
  server.uri = "stun:stun.l.google.com:19302";
  rtcConfiguration.servers.push_back(server);

  return peerConnectionFactory->CreatePeerConnection(rtcConfiguration, nullptr, nullptr, nullptr, this);
}

rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> CallController::createPeerConnectionFactory()
{
  rtc::InitializeSSL();
  rtc::InitRandom(rtc::Time());
  rtc::ThreadManager::Instance()->WrapCurrentThread();

  auto networkThread = new rtc::Thread();
  auto workerThread = new rtc::Thread();

  if (!networkThread->Start() || !workerThread->Start()) {
    throw std::runtime_error("Cant run rtc threads");
  }

  return webrtc::CreatePeerConnectionFactory(
      networkThread,
      workerThread,
      nullptr,
      nullptr,
      nullptr
  );
}

void CallController::OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream)
{
  if (frameProducer)
  {
    return;
  }

  auto videoTracks = stream->GetVideoTracks();
  if (videoTracks.empty())
  {
    return;
  }

  auto track = videoTracks[0];
  frameProducer = std::unique_ptr<render::FrameProducer>(new render::FrameProducer(frameConsumer, track));
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

Poco::JSON::Object::Ptr CallController::parseJson(const std::string &data)
{
  Poco::JSON::Parser parser;
  auto parsed = parser.parse(data);

  return parsed.extract<Poco::JSON::Object::Ptr>();
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
