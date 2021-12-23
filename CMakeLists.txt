cmake_minimum_required(VERSION 3.21)
project(bbt)

option(BUILD_TEST "Build test" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    INTERFACE include/
)

# Test cases
if (BUILD_TEST)
  add_subdirectory(test)
endif ()
