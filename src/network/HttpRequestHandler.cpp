#include "HttpRequestHandler.hpp"

#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/URI.h>
#include <iostream>

#include "RequestHandlerInterface.hpp"

namespace network
{

HttpRequestHandler::HttpRequestHandler(network::RequestHandlerInterface &requestHandler)
    : requestHandler(requestHandler)
{}

void HttpRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response)
{
  std::cout << "new request" << std::endl;
  Poco::URI uri(request.getURI());
  auto path = uri.getPath();
  auto caller = request.clientAddress().toString();
  bool result = false;

  auto length = request.getContentLength();
  std::string data(length, 0);
  request.stream().read(&data[0], length);

  try
  {
    if (path == "/call")
    {
      result = requestHandler.onCallRequest(caller, data);
    }
    else if (path == "/answer")
    {
      result = requestHandler.onAnswerRequest(caller, data);
    }
    else if (path == "/iceCandidate")
    {
      result = requestHandler.onIceCandidateRequest(caller, data);
    }
  }
  catch (const std::exception& exception)
  {
    std::cerr << "Error during handling request: " << exception.what() << std::endl;
    result = false;
  }

  if (result)
  {
    response.setStatus(response.HTTP_OK);
  }
  else
  {
    std::cerr << "request handling fail" << std::endl;
    response.setStatus(response.HTTP_BAD_REQUEST);
  }

  response.add("Content-Length", "0");
  response.send();
}

}
