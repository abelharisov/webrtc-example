#ifndef DINS_CONNECTIONFACTORY_HPP
#define DINS_CONNECTIONFACTORY_HPP

#include <Poco/Net/HTTPServerConnectionFactory.h>

namespace network
{

class RequestHandlerInterface;

class HttpRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
  explicit HttpRequestHandlerFactory(RequestHandlerInterface &requestHandler);

  Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request) override ;

private:
  RequestHandlerInterface &requestHandler;
};

}

#endif //DINS_CONNECTIONFACTORY_HPP
