cmake_minimum_required(VERSION 3.14)
project(CheckersGame VERSION 1.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
set(GAME_LIB_SOURCES
    src/checkers.cpp
    include/checkers.hpp
)
add_library(CheckersLib ${GAME_LIB_SOURCES})
target_include_directories(CheckersLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(CheckersGame src/main.cpp)
target_link_libraries(CheckersGame PRIVATE CheckersLib)
if(WIN32)
    target_link_options(CheckersGame PRIVATE "-mconsole")
endif()
add_executable(CheckersTests tests/test_main.cpp)
target_link_libraries(CheckersTests PRIVATE CheckersLib gtest_main)
include(GoogleTest)
gtest_discover_tests(CheckersTests)