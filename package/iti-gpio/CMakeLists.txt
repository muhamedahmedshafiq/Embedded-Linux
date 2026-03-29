cmake_minimum_required(VERSION 3.16)
project(iti-gpio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

file(WRITE ${CMAKE_BINARY_DIR}/qml.qrc
"<RCC>\n<qresource prefix=\"/\">\n<file>Main.qml</file>\n</qresource>\n</RCC>\n")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Main.qml
    ${CMAKE_BINARY_DIR}/Main.qml
    COPYONLY
)

add_executable(iti-gpio
    main.cpp
    ${CMAKE_BINARY_DIR}/qml.qrc
)

target_link_libraries(iti-gpio PRIVATE Qt6::Quick)

install(TARGETS iti-gpio DESTINATION /usr/bin)
