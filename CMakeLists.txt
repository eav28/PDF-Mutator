cmake_minimum_required(VERSION 3.16)
project(data LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared -Wall -O3 -fpermissive")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cppcodec)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/AFLplusplus-dependencies)

add_library(data SHARED qpdf_data_mutator.cc mutation-library.cc)

add_library(structure SHARED qpdf_structure_mutator.cc mutation-library.cc)

# Add include files for AFL++ and QPDF

target_include_directories(data PRIVATE "./../qpdf/include")
target_include_directories(data PRIVATE "./../AFLplusplus/include")

target_include_directories(structure PRIVATE "./../qpdf/include")
target_include_directories(structure PRIVATE "./../AFLplusplus/include")

# Link against the QPDF library

target_link_libraries(data PRIVATE "${CMAKE_SOURCE_DIR}/../qpdf/build/libqpdf/libqpdf.so.29.10.0")

target_link_libraries(structure PRIVATE "${CMAKE_SOURCE_DIR}/../qpdf/build/libqpdf/libqpdf.so.29.10.0")

# Define POINTERHOLDER_TRANSITION to suppress the warning
# Adjust the value if you are transitioning your use of PointerHolder

target_compile_definitions(data PRIVATE POINTERHOLDER_TRANSITION=0)

target_compile_definitions(structure PRIVATE POINTERHOLDER_TRANSITION=0)
