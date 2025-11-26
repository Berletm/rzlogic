include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP 0
)

add_executable(
    run_tests
    test_parser.cpp
    test_pnf.cpp
    test_snf.cpp
    test_cnf.cpp
    test_unification.cpp
    test_resolution.cpp
    test_all.cpp
    utils.cpp
)

target_link_libraries(run_tests gtest gtest_main rzlogic)

add_test(NAME logic_parser_tests COMMAND run_tests)