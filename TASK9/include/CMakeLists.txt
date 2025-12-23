cmake_minimum_required(VERSION 3.10)

add_library(sevensegment_headers INTERFACE)

message(STATUS "show me ${CMAKE_HOME_DIRECTORY}")

target_include_directories(sevensegment_headers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
