#ifndef DINS_HTTPCLIENT_HPP
#define DINS_HTTPCLIENT_HPP

#include "call/RequestSenderInterface.hpp"

namespace network
{

class HttpClient : public call::RequestSenderInterface
{
public:
  void sendRequest(const std::string &address, const std::string &path, const std::string &data) override;
};

}


#endif //DINS_HTTPCLIENT_HPP
