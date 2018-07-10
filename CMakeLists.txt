cmake_minimum_required(VERSION 3.0)
project(dins)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
if (WIN32)
    add_definitions(-DWEBRTC_WIN)
endif(WIN32)
if (UNIX)
    add_definitions(-DWEBRTC_POSIX)
endif(UNIX)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    link_directories(libs/webrtc/lib/x64/Debug)
    link_directories(libs/webrtc/lib/x86/Debug)
else()
    link_directories(libs/webrtc/lib/x64/Release)
    link_directories(libs/webrtc/lib/x86/Release)
endif()

add_subdirectory(libs/poco)
add_subdirectory(libs/sdl)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(libs/sdl/include)
include_directories(libs/webrtc/include)

execute_process(
        COMMAND tar -xf webrtc-16937-cf6f3f6-linux-x64.tar.gz -C .
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/webrtc
)
execute_process(
        COMMAND tar -xf webrtc-16937-cf6f3f6-linux-x86.tar.gz -C .
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/webrtc
)

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
        src/render/FrameProducer.cpp
        src/render/FrameProducer.hpp
        src/render/SDLRenderer.cpp
        src/render/SDLRenderer.hpp
        src/render/FrameConsumerInterface.hpp
)

target_link_libraries(
        dins
        Poco::Net
        Poco::JSON
        Poco::Util
        Poco::Foundation
        SDL2
        webrtc_full
        X11
)
