#ifndef DINS_CONNECTIONLISTENERINTERFACE_HPP
#define DINS_CONNECTIONLISTENERINTERFACE_HPP

namespace network
{

class Connection;

class RequestHandlerInterface
{
public:
  virtual ~RequestHandlerInterface() = default;

  virtual bool onCallRequest(const std::string& caller, const std::string& data) = 0;
  virtual bool onAnswerRequest(const std::string& caller, const std::string& data) = 0;
  virtual bool onIceCandidateRequest(const std::string& caller, const std::string& data) = 0;
};

}

#endif //DINS_CONNECTIONLISTENERINTERFACE_HPP
