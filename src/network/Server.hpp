#ifndef DINS_SERVER_H
#define DINS_SERVER_H

#include <Poco/Net/HTTPServer.h>

namespace network
{

class RequestHandlerInterface;

class Server
{
public:
  Server(int port, RequestHandlerInterface &requestHandler);

  void start();

  void stop();

private:
  Poco::Net::HTTPServer server;
};

}

#endif //DINS_SERVER_H
