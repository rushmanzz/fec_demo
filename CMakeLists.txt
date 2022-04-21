cmake_minimum_required(VERSION 2.8.7)
project(fec)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -D_DEBUG -std=c++11 -pthread")


set(SERVER_1 test/server_1_1.cpp)
SET(CLIENT_1 test/client_1_1.cpp)
set(TCP_SERVER_1 test/tcp_server_1.cpp)
set(TCP_CLIENT_1 test/tcp_client_1.cpp)


set(SOURCE_FILES)

file(GLOB_RECURSE fec_sources ${PROJECT_SOURCE_DIR}/fec/*.cpp)
file(GLOB_RECURSE conf_sources ${PROJECT_SOURCE_DIR}/util/*.cpp)
file(GLOB_RECURSE udp_sources ${PROJECT_SOURCE_DIR}/include/*.cpp)
list(APPEND SOURCE_FILES ${fec_sources} ${conf_sources} ${udp_sources})

include_directories(${PROJECT_SOURCE_DIR}/third_party)
include_directories(${PROJECT_SOURCE_DIR}/fec)
include_directories(${PROJECT_SOURCE_DIR}/util)
include_directories(${PROJECT_SOURCE_DIR}/include/)


add_executable(udp_server_1 ${SOURCE_FILES} ${SERVER_1})
add_executable(udp_client_1 ${SOURCE_FILES} ${CLIENT_1})
add_executable(tcp_server_1 ${SOURCE_FILES} ${TCP_SERVER_1})
add_executable(tcp_client_1 ${SOURCE_FILES} ${TCP_CLIENT_1})
