cmake_minimum_required(VERSION 3.21)
project(bbt)

set(CMAKE_CXX_STANDARD 11)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${CMAKE_PROJECT_NAME}>
        $<INSTALL_INTERFACE:include/${CMAKE_PROJECT_NAME}>
        )
