cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

project(sevensegment_app C CXX ASM)

add_subdirectory(include)

add_executable(${PROJECT_NAME} app/main.cpp source/sevensegment.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE sevensegment_headers)


