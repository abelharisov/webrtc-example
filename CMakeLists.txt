cmake_minimum_required(VERSION 3.10)
project(dins)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#set(BUILD_SHARED_LIBS OFF)
#set(CMAKE_EXE_LINKER_FLAGS "-static")

if (WIN32)
    add_definitions(-DWEBRTC_WIN)
endif(WIN32)

if (UNIX)
    add_definitions(-DWEBRTC_POSIX)
endif(UNIX)

link_directories(${CMAKE_SOURCE_DIR}/webrtc/lib/x64/Debug)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Poco REQUIRED COMPONENTS Foundation Util Net JSON)
include_directories(${Poco_INCLUDE_DIRS})

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
target_link_libraries(dins webrtc_full ${Poco_LIBRARIES} X11 xcb Xau Xdmcp)