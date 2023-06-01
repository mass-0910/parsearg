cmake_minimum_required(VERSION 3.14)

project(parsearg)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(parsearg STATIC
    parsearg.cpp
)
target_include_directories(parsearg PUBLIC
    ${PROJECT_SOURCE_DIR}
)

option(TEST_ENABLE "The flag to enable googletest" OFF)

if(TEST_ENABLE)
    # Test configurations
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    if(WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif(WIN32)

    # We don't want to install GoogleTest libs when we run `cmake --install`
    set(INSTALL_GTEST OFF CACHE BOOL "Enable installation of GoogleTest." FORCE)

    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_subdirectory(test)
endif()