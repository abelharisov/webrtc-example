#ifndef DINS_REQUESTSENDERINTERFACE_HPP
#define DINS_REQUESTSENDERINTERFACE_HPP

#include <string>

namespace call
{

class RequestSenderInterface
{
public:
  virtual ~RequestSenderInterface() = default;

  virtual void sendRequest(const std::string& address, const std::string& path, const std::string& data) = 0;
};

}

#endif //DINS_REQUESTSENDERINTERFACE_HPP
