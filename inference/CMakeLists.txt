cmake_minimum_required(VERSION 3.10)

project(logic)

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
target_link_libraries(logic PUBLIC rzlogic)