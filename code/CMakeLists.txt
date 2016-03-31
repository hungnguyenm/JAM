cmake_minimum_required(VERSION 3.4)
project(JAM)

message(STATUS "Running cmake...")

find_package(Boost 1.58.0 COMPONENTS system filesystem REQUIRED)
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp)
add_executable(JAM ${SOURCE_FILES})

set(DEV_B_SOURCES src/dev/dev_b.cpp)
add_executable(DEV_B ${DEV_B_SOURCES})

set(DEV_C_SOURCES src/dev/dev_c.cpp)
add_executable(DEV_C ${DEV_C_SOURCES})

set(DEV_H_SOURCES src/dev/dev_h.cpp)
add_executable(DEV_H ${DEV_H_SOURCES})