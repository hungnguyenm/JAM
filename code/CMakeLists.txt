cmake_minimum_required(VERSION 3.4)
project(JAM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

message(STATUS "Running cmake...")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

find_package(Boost 1.58.0 COMPONENTS thread)
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(SOURCE_FILES src/main.cpp)
add_executable(jam-main ${SOURCE_FILES})

set(DEV_B_SOURCES src/dev/dev_b.cpp)
add_executable(jam-dev_b ${DEV_B_SOURCES})


# Executables depend on BOOST:
set(DEV_H_HEADERS include/config.h include/payload.h include/udp_wrapper.h)
set(DEV_H_SOURCES src/dev/dev_h.cpp src/payload.cpp src/udp_wrapper.cpp)
add_executable(jam-dev_h_1 ${DEV_H_HEADERS} ${DEV_H_SOURCES})
add_executable(jam-dev_h_2 ${DEV_H_HEADERS} ${DEV_H_SOURCES})

set(DEV_C_SOURCES src/dev/dev_c.cpp src/user_handler.cpp src/callable.cpp include/callable.h include/user_handler.h)
add_executable(jam-dev_c ${DEV_C_SOURCES})

if(Boost_FOUND)
    target_link_libraries(jam-dev_h_1 ${Boost_LIBRARIES})
    target_link_libraries(jam-dev_h_2 ${Boost_LIBRARIES})
    target_link_libraries(jam-dev_c ${Boost_LIBRARIES})
endif()