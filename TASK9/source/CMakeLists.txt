
add_library(sevensegment_lib SHARED sevensegment.cpp)

target_include_directories(sevensegment_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
