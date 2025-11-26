cmake_minimum_required(VERSION 3.10)
project(rzlogic)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

enable_testing()
add_subdirectory(tests)
 
add_library(
    rzlogic
    src/parser.cpp
    src/logic.cpp
)

target_include_directories(rzlogic PUBLIC include)

pybind11_add_module(
    rzlogic-pybind
    src/bindings.cpp
)

target_link_libraries(rzlogic-pybind PUBLIC rzlogic pybind11::headers)