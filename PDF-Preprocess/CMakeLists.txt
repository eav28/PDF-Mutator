cmake_minimum_required(VERSION 3.16)
project(pdf-preprocess LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../rapidjson/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../cppcodec)

# Specify your source file
add_executable(pdf-preprocess PDF-Preprocess.cc)

# Specify the directory where the QPDF headers are located
# Adjust the path to reflect the actual location of the QPDF headers
target_include_directories(pdf-preprocess PRIVATE "./../../qpdf/include")
target_include_directories(pdf-preprocess PRIVATE "./../../AFLplusplus/include")

# Link against the QPDF library
target_link_libraries(pdf-preprocess PRIVATE "${CMAKE_SOURCE_DIR}/../../qpdf/build/libqpdf/libqpdf.so.29.10.0")

# Define POINTERHOLDER_TRANSITION to suppress the warning
# Adjust the value if you are transitioning your use of PointerHolder
target_compile_definitions(pdf-preprocess PRIVATE POINTERHOLDER_TRANSITION=0)
