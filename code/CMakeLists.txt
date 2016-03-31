cmake_minimum_required(VERSION 3.4)
project(JAM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

message(STATUS "Running cmake...")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

find_package(Boost 1.58.0 COMPONENTS signals)
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

set(DEV_C_SOURCES src/dev/dev_c.cpp)
add_executable(jam-dev_c ${DEV_C_SOURCES})

# Executables depend on BOOST:
set(DEV_H_SOURCES src/dev/dev_h.cpp)
add_executable(jam-dev_h ${DEV_H_SOURCES})

if(Boost_FOUND)
    target_link_libraries(jam-dev_h ${Boost_LIBRARIES})
endif()