cmake_minimum_required(VERSION 3.10)

project(logic)

enable_testing()
add_subdirectory(tests)

add_executable(logic
            src/main.cpp
            src/parser.cpp
            src/logic.cpp
)

target_include_directories(logic PRIVATE include)
