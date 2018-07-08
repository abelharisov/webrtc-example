#include "Server.hpp"

#include "HttpRequestHandlerFactory.hpp"

namespace network
{

Server::Server(int port, RequestHandlerInterface &requestHandler)
    : server(new HttpRequestHandlerFactory(requestHandler), port)
{}

void Server::start()
{
  server.start();
}

void Server::stop()
{
  server.stop();
}

}
