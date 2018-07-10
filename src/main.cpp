#include <iostream>
#include <string>

#include <Poco/Event.h>
#include <Poco/NamedEvent.h>
#include <Poco/Process.h>

#if defined(POCO_OS_FAMILY_WINDOWS)
#include <Poco/Process_WIN32.h>
#endif

#include "call/CallController.hpp"
#include "network/Server.hpp"
#include "network/HttpClient.hpp"
#include "render/SDLRenderer.hpp"

int main(int argc, char *argv[])
{
  if (argc <= 1)
  {
    std::cout << "You should specify port to listen" << std::endl;
    return EXIT_FAILURE;
  }

  int port = std::stoi(std::string(argv[1]));

  render::SDLRenderer renderer;
  network::HttpClient httpClient;
  call::CallController callController(renderer, httpClient, std::to_string(port));
  network::Server server(port, callController);

  server.start();

  std::string address;
  std::cout << "Enter address to call (ip:port): ";
  std::cin >> address;

  callController.call(address);

#if defined(POCO_OS_FAMILY_WINDOWS)
  Poco::NamedEvent terminator(Poco::ProcessImpl::terminationEventName(Poco::Process::id()));
#else
  Poco::Event terminator;
#endif

  terminator.wait();

  return EXIT_SUCCESS;
}