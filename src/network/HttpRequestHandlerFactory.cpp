#include "HttpRequestHandlerFactory.hpp"

#include "HttpRequestHandler.hpp"

namespace network
{

HttpRequestHandlerFactory::HttpRequestHandlerFactory(RequestHandlerInterface &requestHandler)
    : requestHandler(requestHandler)
{}

Poco::Net::HTTPRequestHandler* HttpRequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest &request)
{
  return new HttpRequestHandler(requestHandler);
}

}
