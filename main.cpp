#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
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