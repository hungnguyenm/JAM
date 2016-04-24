cmake_minimum_required(VERSION 3.0)
project(JAM)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

message(STATUS "Running cmake...")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

# Prepare BOOST
find_package(Boost 1.50.0 COMPONENTS system atomic chrono date_time thread)
if (Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})
else ()
    set(BOOST_INCLUDE_DIR /usr/include)
    find_package(Boost 1.50.0 COMPONENTS system atomic chrono date_time thread)
    if (Boost_FOUND)
        message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
        message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
        message(STATUS "Boost_VERSION: ${Boost_VERSION}")

        include_directories(${Boost_INCLUDE_DIRS})
    endif ()
endif ()

# Prepare OpenSSL
find_package(OpenSSL)
if (OPENSSL_FOUND)
    message(STATUS "OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
    include_directories(${OPENSSL_INCLUDE_DIR})
endif ()

# Prepare sources
set(MAIN_HEADERS include/config.h include/concurrent_queue.h include/concurrent_ticket.h
        include/central_queues.h include/serializer_helper.h include/stream_communicator.h
        include/leader_manager.h include/payload.h include/udp_wrapper.h include/user_handler.h
        include/client_manager.h include/client_info.h include/hold_queue.h include/jam.h)
set(MAIN_SOURCES src/leader_manager.cpp src/payload.cpp src/central_queues.cpp src/serializer_helper.cpp
        src/udp_wrapper.cpp src/user_handler.cpp src/client_info.cpp src/client_manager.cpp
        src/hold_queue.cpp src/jam.cpp)

set(MAIN_SECURE_HEADERS include/config.h include/concurrent_queue.h include/concurrent_ticket.h
        include/central_queues.h include/serializer_helper.h include/stream_communicator.h
        include/leader_manager.h include/payload_secure.h include/udp_wrapper.h include/user_handler.h
        include/client_manager.h include/client_info.h include/hold_queue.h include/jam.h)
set(MAIN_SECURE_SOURCES src/leader_manager.cpp src/payload_secure.cpp src/central_queues.cpp
        src/serializer_helper.cpp src/udp_wrapper.cpp src/user_handler.cpp src/client_info.cpp
        src/client_manager.cpp src/hold_queue.cpp src/jam.cpp)

# Executables - non-secure
add_executable(dchat ${MAIN_HEADERS} ${MAIN_SOURCES} src/main.cpp include/StressTester.cpp src/stress_tester.cpp include/stress_tester.h)
set_target_properties(dchat PROPERTIES COMPILE_FLAGS "-DSPECLAB")

add_executable(dchatd ${MAIN_HEADERS} ${MAIN_SOURCES} src/main.cpp)
set_target_properties(dchatd PROPERTIES COMPILE_FLAGS "-DSPECLAB -DDEBUG")

add_executable(jam_1 ${MAIN_HEADERS} ${MAIN_SOURCES} src/dev/dev_h_1.cpp)
set_target_properties(jam_1 PROPERTIES COMPILE_FLAGS "-DDEBUG")
add_executable(jam_2 ${MAIN_HEADERS} ${MAIN_SOURCES} src/dev/dev_h_2.cpp)
set_target_properties(jam_2 PROPERTIES COMPILE_FLAGS "-DDEBUG")
add_executable(jam_3 ${MAIN_HEADERS} ${MAIN_SOURCES} src/dev/dev_h_3.cpp)
set_target_properties(jam_3 PROPERTIES COMPILE_FLAGS "-DDEBUG")

# Executables - secure
if (OPENSSL_FOUND)
    add_executable(dchats ${MAIN_SECURE_HEADERS} ${MAIN_SECURE_SOURCES} src/main.cpp)
    set_target_properties(dchats PROPERTIES COMPILE_FLAGS "-DSPECLAB -DSECURE")

    add_executable(dchatds ${MAIN_SECURE_HEADERS} ${MAIN_SECURE_SOURCES} src/main.cpp)
    set_target_properties(dchatds PROPERTIES COMPILE_FLAGS "-DSPECLAB -DDEBUG -DSECURE")

    add_executable(jams_1 ${MAIN_SECURE_HEADERS} ${MAIN_SECURE_SOURCES} src/dev/dev_h_1.cpp)
    set_target_properties(jams_1 PROPERTIES COMPILE_FLAGS "-DDEBUG -DSECURE")
endif ()

# Test executables
add_executable(test-udp_wrapper ${MAIN_HEADERS} ${MAIN_SOURCES} test/test_udp_wrapper.cpp)
set_target_properties(test-udp_wrapper PROPERTIES COMPILE_FLAGS "-DDEBUG")

add_executable(test-hold_queue ${MAIN_HEADERS} ${MAIN_SOURCES} test/test_hold_queue.cpp)
set_target_properties(test-hold_queue PROPERTIES COMPILE_FLAGS "-DDEBUG")

# Group targets
ADD_CUSTOM_TARGET(speclab)
ADD_DEPENDENCIES(speclab dchat dchatd)

ADD_CUSTOM_TARGET(local)
ADD_DEPENDENCIES(local jam_1 jam_2 jam_3)

if (OPENSSL_FOUND)
    ADD_CUSTOM_TARGET(speclab-secure)
    ADD_DEPENDENCIES(speclab-secure dchats dchatds)

    ADD_CUSTOM_TARGET(local-secure)
    ADD_DEPENDENCIES(local-secure jams_1)
endif ()

if (Boost_FOUND)
    target_link_libraries(dchat ${Boost_LIBRARIES})
    target_link_libraries(dchatd ${Boost_LIBRARIES})

    target_link_libraries(jam_1 ${Boost_LIBRARIES})
    target_link_libraries(jam_2 ${Boost_LIBRARIES})
    target_link_libraries(jam_3 ${Boost_LIBRARIES})

    target_link_libraries(test-udp_wrapper ${Boost_LIBRARIES})
    target_link_libraries(test-hold_queue ${Boost_LIBRARIES})

    if (OPENSSL_FOUND)
        target_link_libraries(dchats ${Boost_LIBRARIES})
        target_link_libraries(dchatds ${Boost_LIBRARIES})

        target_link_libraries(jams_1 ${Boost_LIBRARIES})

        target_link_libraries(dchats ${OPENSSL_LIBRARIES})
        target_link_libraries(dchatds ${OPENSSL_LIBRARIES})

        target_link_libraries(jams_1 ${OPENSSL_LIBRARIES})
    endif ()
endif ()
