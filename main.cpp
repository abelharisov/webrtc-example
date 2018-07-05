#include <iostream>
#include <string>
#include <Poco/Net/TCPServer.h>

class EchoConnection: public Poco::Net::TCPServerConnection {
public:
    EchoConnection(const Poco::Net::StreamSocket& s): TCPServerConnection(s) { }

    void run() {
        Poco::Net::StreamSocket& ss = socket();
        try {
            char buffer[256];
            int n = ss.receiveBytes(buffer, sizeof(buffer));
            while (n > 0) {
                ss.sendBytes(buffer, n);
                n = ss.receiveBytes(buffer, sizeof(buffer));
            }
        }
        catch (Poco::Exception& exc)
        { std::cerr << "EchoConnection: " << exc.displayText() << std::endl; }
    }
};

int main(int argc, char* argv[]) {
    Poco::Net::TCPServerConnectionFactory::Ptr factory(new Poco::Net::TCPServerConnectionFactoryImpl<EchoConnection>());
    Poco::Net::TCPServer server(factory);

    if (argc <= 1) {
        throw std::runtime_error("You should specify port to listen");
    }

    int port = std::stoi(std::string(argv[1]));

    if ((port < 1) || (port > 65535)) {
        throw std::runtime_error("Error: not a valid port");
    }

    std::cout << "hello" << std::endl;

    return 0;
}