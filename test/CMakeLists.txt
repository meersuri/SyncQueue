enable_testing()

add_executable(
    tests
    test_sync_queue.cpp
    )

target_link_libraries(
    tests
    GTest::gtest_main
    )

target_include_directories(
    tests
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    )

target_compile_options(
    tests
    PRIVATE
    -g
    -Wall
    -Wextra
    -pedantic
    )

include(GoogleTest)
gtest_discover_tests(tests)

