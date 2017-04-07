cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(gameoflife CXX)
set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_ui(QT_UI_HEADERS resources/mainwindow.ui)

file(GLOB SOURCES src/*.cpp)

find_package(Qt5Widgets REQUIRED)
add_executable(gameoflife ${SOURCES} ${QT_UI_HEADERS})
target_link_libraries(gameoflife Qt5::Widgets)
