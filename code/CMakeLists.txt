cmake_minimum_required(VERSION 3.4)
project(JAM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/client_manager.cpp)
add_executable(JAM ${SOURCE_FILES})

set(DEV_B_SOURCES src/dev/dev_b.cpp)
add_executable(DEV_B ${DEV_B_SOURCES})

set(DEV_C_SOURCES src/dev/dev_c.cpp)
add_executable(DEV_C ${DEV_C_SOURCES})

set(DEV_H_SOURCES src/dev/dev_h.cpp)
add_executable(DEV_H ${DEV_H_SOURCES})