include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

add_executable(
    run_tests 
    test_parser.cpp
    test_pnf.cpp
    ../src/parser.cpp
    ../src/logic.cpp
)

target_include_directories(run_tests PRIVATE ../include/)
target_link_libraries(run_tests gtest gtest_main)

add_test(NAME logic_parser_tests COMMAND run_tests)
