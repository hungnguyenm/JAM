cmake_minimum_required(VERSION 3.4)
project(JAM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

message(STATUS "Running cmake...")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -g -std=c++11")

find_package(Boost 1.58.0 COMPONENTS system thread)
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(SOURCE_FILES src/main.cpp)
add_executable(jam-main ${SOURCE_FILES})

set(DEV_B_HEADERS include/client_info.h include/client_manager.h include/payload.h include/config.h)
set(DEV_B_SOURCES src/dev/dev_b.cpp src/client_manager.cpp src/client_info.cpp src/payload.cpp src/leader_manager.cpp include/leader_manager.h src/serializer_helper.cpp include/serializer_helper.h)
add_executable(jam-dev_b ${DEV_B_HEADERS} ${DEV_B_SOURCES})

# Executables depend on BOOST:
set(DEV_H_HEADERS include/config.h include/concurrent_queue.h include/concurrent_ticket.h
        include/central_queues.h include/payload.h include/udp_wrapper.h
        include/user_handler.h include/client_manager.h include/client_info.h include/jam.h)
set(DEV_H_SOURCES src/payload.cpp src/central_queues.cpp
        src/udp_wrapper.cpp src/user_handler.cpp src/client_info.cpp src/client_manager.cpp src/jam.cpp)
add_executable(jam-dev_h_1 ${DEV_H_HEADERS} ${DEV_H_SOURCES} src/dev/dev_h_1.cpp)
add_executable(jam-dev_h_2 ${DEV_H_HEADERS} ${DEV_H_SOURCES} src/dev/dev_h_2.cpp)

set(DEV_C_SOURCES src/dev/dev_c.cpp src/user_handler.cpp src/callable.cpp src/payload.cpp
        src/central_queues.cpp)
add_executable(jam-dev_c ${DEV_C_SOURCES})

# Test files
set(TEST_CLIENT_MANAGER_SOURCES test/test_client_manager.cpp src/client_manager.cpp)
add_executable(test-client_manager ${TEST_CLIENT_MANAGER_SOURCES})

set(TEST_UDP_WRAPPER_HEADERS include/config.h include/concurrent_queue.h
        include/concurrent_ticket.h include/payload.h include/udp_wrapper.h)
set(TEST_UDP_WRAPPER_SOURCES test/test_udp_wrapper.cpp src/payload.cpp src/udp_wrapper.cpp)
add_executable(test-udp_wrapper ${TEST_UDP_WRAPPER_HEADERS} ${TEST_UDP_WRAPPER_SOURCES})

if(Boost_FOUND)
    target_link_libraries(jam-main ${Boost_LIBRARIES})

    target_link_libraries(jam-dev_h_1 ${Boost_LIBRARIES})
    target_link_libraries(jam-dev_h_2 ${Boost_LIBRARIES})
    target_link_libraries(jam-dev_c ${Boost_LIBRARIES})

    target_link_libraries(test-udp_wrapper ${Boost_LIBRARIES})
endif()
