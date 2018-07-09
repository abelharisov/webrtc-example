#include <iostream>
#include <string>

#include <Poco/Event.h>
#include <Poco/NamedEvent.h>
#include <Poco/Process.h>

#include "call/CallController.hpp"
#include "network/Server.hpp"
#include "network/HttpClient.hpp"

int main(int argc, char *argv[])
{
  if (argc <= 1)
  {
    throw std::runtime_error("You should specify port to listen");
  }

  int port = std::stoi(std::string(argv[1]));

  if ((port < 1) || (port > 65535))
  {
    throw std::runtime_error("Error: not a valid port");
  }

  network::HttpClient httpClient;
  call::CallController callController(httpClient);
  network::Server server(port, callController);

  server.start();

  std::string address;
  std::cout << "Call to ip:port: " << std::endl;
  std::cin >> address;

  callController.call(address);

#if defined(POCO_OS_FAMILY_WINDOWS)
  Poco::NamedEvent terminator(ProcessImpl::terminationEventName(Process::id()));
#else
  Poco::Event terminator;
#endif

  terminator.wait();
}