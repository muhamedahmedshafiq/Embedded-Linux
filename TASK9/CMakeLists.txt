cmake_minimum_required(VERSION 3.10)

# Set compilers before project() for cross-compilation
set(CMAKE_C_COMPILER aarch64-rpi3-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-rpi3-linux-gnu-g++)

project(sevensegment_app C CXX ASM)

set(VARIP 192.168.1.61)


add_subdirectory(source)

add_executable(${PROJECT_NAME} app/main.cpp)


target_link_libraries(${PROJECT_NAME} PRIVATE sevensegment_lib)


add_custom_target(upload
  
    COMMAND scp ${CMAKE_BINARY_DIR}/${PROJECT_NAME} pi@${VARIP}:/home/pi/TASK9/
    
   
    COMMAND scp ${CMAKE_BINARY_DIR}/source/libsevensegment_lib.so pi@${VARIP}:/home/pi/TASK9/
    
    DEPENDS ${PROJECT_NAME} sevensegment_lib
    COMMENT "Uploading app and shared library to Pi..."
)