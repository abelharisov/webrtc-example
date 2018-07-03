cmake_minimum_required(VERSION 3.10)
project(dins)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")

link_directories(/home/ruslan/workspace/dins/webrtc/lib/x64/Debug)

add_executable(dins main.cpp)
target_include_directories(dins PRIVATE webrtc/include)
target_link_libraries(dins webrtc_full)
