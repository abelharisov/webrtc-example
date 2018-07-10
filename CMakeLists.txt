cmake_minimum_required(VERSION 3.0)
project(webrtc-example)

set(CMAKE_CXX_STANDARD 11)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BITS 64)
else ()
    set(BITS 86)
endif ()

if (WIN32)
    add_definitions(-DWEBRTC_WIN)
	add_definitions(-DNOMINMAX)
	set(LIB_EXTENSION lib)
endif (WIN32)

if (UNIX)
    add_definitions(-DWEBRTC_POSIX)
	add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
	set(LIB_EXTENSION a)
endif (UNIX)

add_subdirectory(libs/poco)
add_subdirectory(libs/sdl)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(libs/sdl/include)
include_directories(libs/webrtc/include)

add_executable(
        webrtc-example
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
        webrtc-example
        Poco::Net
        Poco::JSON
        Poco::Util
        Poco::Foundation
        SDL2
		optimized ${CMAKE_SOURCE_DIR}/libs/webrtc/lib/x${BITS}/Release/libwebrtc_full.${LIB_EXTENSION}
		debug ${CMAKE_SOURCE_DIR}/libs/webrtc/lib/x${BITS}/Debug/libwebrtc_full.${LIB_EXTENSION}
)

if (WIN32)
	target_link_libraries(
		webrtc-example 
		Strmiids.lib 
		Secur32.lib 
		msdmo.lib 
		dmoguids.lib 
		wmcodecdspuuid.lib
	)
endif ()

if (UNIX)
	target_link_libraries(webrtc-example X11)
endif ()	
