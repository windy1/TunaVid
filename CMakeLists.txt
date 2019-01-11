cmake_minimum_required(VERSION 3.13)
project(TunaVid)

set(CMAKE_CXX_STANDARD 17)

include_directories(/Users/walker/Dev/personal/TunaVid-ui)
link_directories(/Users/walker/Dev/personal/build-TunaVid-ui-Desktop_Qt_5_11_1_clang_64bit-Debug)

add_executable(TunaVid
        src/main.cpp
        src/tuna.h
        src/Server/VideoService.cpp src/Server/VideoService.h
        src/Client/VideoClient.cpp src/Client/VideoClient.h
        src/Connection.cpp src/Connection.h src/Server/User.cpp src/Server/User.h src/tuna.cpp)

target_link_libraries(TunaVid TunaVid-ui)
