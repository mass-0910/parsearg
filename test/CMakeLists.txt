add_executable(test_
    test.cpp
)

target_include_directories(test_ PRIVATE
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(test_
    GTest::gtest_main
    parsearg
)

set_target_properties(test_
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

include(GoogleTest)
gtest_discover_tests(test_)
