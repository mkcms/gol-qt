cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(gameoflife CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
add_executable(gameoflife main.cpp)
target_link_libraries(gameoflife Qt5::Widgets)
