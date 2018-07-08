#include "HttpClient.hpp"

#include <Poco/URI.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>

namespace network
{

void HttpClient::sendRequest(const std::string &address, const std::string &path, const std::string &data)
{
  Poco::URI uri(address);
  uri.setPath(path);

  Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
  Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_POST, uri.toString());
  request.setContentLength(data.size());
  auto& outStream = session.sendRequest(request);
  outStream << data;

  Poco::Net::HTTPResponse response;
  session.receiveResponse(response);

  if (response.getStatus() != response.HTTP_OK) {
    throw std::runtime_error("Bad http request");
  }
}

}
