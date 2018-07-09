#include "HttpClient.hpp"

#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <iostream>

namespace network
{

void HttpClient::sendRequest(const std::string &address, const std::string &path, const std::string &data)
{
  std::string uri = std::string("http://") + address + std::string("/") + path;

  Poco::Net::SocketAddress socketAddress(address);
  Poco::Net::HTTPClientSession session(socketAddress);
  Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri);
  request.setContentLength(data.size());
  auto& outStream = session.sendRequest(request);
  outStream << data;

  Poco::Net::HTTPResponse response;
  session.receiveResponse(response);

  if (response.getStatus() != response.HTTP_OK) {
    throw std::runtime_error("Bad http response");
  }
}

}
