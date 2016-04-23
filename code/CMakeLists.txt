cmake_minimum_required(VERSION 3.4)
project(JAM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

message(STATUS "Running cmake...")
#
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -g -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -g -std=c++11 -lboost_system")

find_package(Boost 1.58.0 COMPONENTS system thread)
if (Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})
endif ()

set(MAIN_HEADERS include/config.h include/concurrent_queue.h include/concurrent_ticket.h
        include/central_queues.h include/serializer_helper.h include/stream_communicator.h
        include/leader_manager.h include/payload.h include/udp_wrapper.h include/user_handler.h include/client_manager.h
        include/client_info.h include/hold_queue.h include/jam.h)
set(MAIN_SOURCES src/leader_manager.cpp src/payload.cpp src/central_queues.cpp src/serializer_helper.cpp
        src/udp_wrapper.cpp src/user_handler.cpp src/client_info.cpp src/client_manager.cpp
        src/hold_queue.cpp src/jam.cpp)

# Executables depend on BOOST:
add_executable(jam-main ${MAIN_HEADERS} ${MAIN_SOURCES} src/main.cpp)

add_executable(jam-dev_h_1 ${MAIN_HEADERS} ${MAIN_SOURCES} src/dev/dev_h_1.cpp)
add_executable(jam-dev_h_2 ${MAIN_HEADERS} ${MAIN_SOURCES} src/dev/dev_h_2.cpp)
add_executable(jam-dev_h_3 ${MAIN_HEADERS} ${MAIN_SOURCES} src/dev/dev_h_3.cpp)

# Test files
set(TEST_CLIENT_MANAGER_SOURCES test/test_client_manager.cpp src/client_manager.cpp)
add_executable(test-client_manager ${TEST_CLIENT_MANAGER_SOURCES})

set(TEST_UDP_WRAPPER_HEADERS include/config.h include/concurrent_queue.h
        include/concurrent_ticket.h include/payload.h include/udp_wrapper.h)
set(TEST_UDP_WRAPPER_SOURCES test/test_udp_wrapper.cpp src/payload.cpp src/udp_wrapper.cpp)
add_executable(test-udp_wrapper ${TEST_UDP_WRAPPER_HEADERS} ${TEST_UDP_WRAPPER_SOURCES})

add_executable(test-hold_queue ${MAIN_HEADERS} ${MAIN_SOURCES} test/test_hold_queue.cpp)

if (Boost_FOUND)
    target_link_libraries(jam-main ${Boost_LIBRARIES})

    target_link_libraries(jam-dev_h_1 ${Boost_LIBRARIES})
    target_link_libraries(jam-dev_h_2 ${Boost_LIBRARIES})
    target_link_libraries(jam-dev_h_3 ${Boost_LIBRARIES})

    target_link_libraries(test-udp_wrapper ${Boost_LIBRARIES})
    target_link_libraries(test-hold_queue ${Boost_LIBRARIES})
endif ()
