cmake_minimum_required(VERSION 3.10)

project(logic)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

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

add_executable(logic
            src/main.cpp
)

pybind11_add_module(rzresolution src/bindings.cpp)
target_include_directories(rzresolution PUBLIC include)
target_link_libraries(rzresolution PUBLIC rzlogic pybind11::headers)

target_link_libraries(logic PUBLIC rzlogic pybind11::module)

set(INIT_PY ${CMAKE_CURRENT_BINARY_DIR}/__init__.py)
if(NOT EXISTS ${INIT_PY})
file(WRITE ${INIT_PY} "")
endif()
