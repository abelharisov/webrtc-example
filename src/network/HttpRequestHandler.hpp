#ifndef DINS_HTTPREQUESTHANDLER_HPP
#define DINS_HTTPREQUESTHANDLER_HPP

#include <Poco/Net/HTTPRequestHandler.h>

namespace network
{

class RequestHandlerInterface;

class HttpRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
  explicit HttpRequestHandler(RequestHandlerInterface& requestHandler);

  void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;

private:
  RequestHandlerInterface& requestHandler;
};

}

#endif //DINS_HTTPREQUESTHANDLER_HPP
