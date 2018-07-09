cmake_minimum_required(VERSION 3.10)
project(dins)

set(CMAKE_CXX_STANDARD 11)

if (WIN32)
    add_definitions(-DWEBRTC_WIN)
endif(WIN32)

if (UNIX)
    add_definitions(-DWEBRTC_POSIX)
endif(UNIX)

find_package(Poco REQUIRED COMPONENTS Foundation Util Net JSON)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${Poco_INCLUDE_DIRS})

link_directories(${CMAKE_SOURCE_DIR}/webrtc/lib/x64/Debug)

add_executable(
        dins
        src/main.cpp
        src/call/CallController.cpp
        src/call/CallController.hpp
        src/call/RequestSenderInterface.hpp
        src/network/Server.cpp
        src/network/Server.hpp
        src/network/RequestHandlerInterface.hpp
        src/network/HttpRequestHandlerFactory.cpp
        src/network/HttpRequestHandlerFactory.hpp
        src/network/HttpRequestHandler.cpp
        src/network/HttpRequestHandler.hpp
        src/network/HttpClient.cpp
        src/network/HttpClient.hpp
)

target_include_directories(dins PRIVATE webrtc/include)
target_link_libraries(
        dins
        ${Poco_LIBRARIES}
        webrtc_full
        X11
)
