cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(gameoflife CXX)
set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Boost REQUIRED COMPONENTS)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt5_wrap_ui(QT_UI_HEADERS resources/mainwindow.ui)
qt5_add_resources(QT_RESOURCES resources/resources.qrc)

file(GLOB SOURCES src/*.cpp)

add_executable(gameoflife ${SOURCES} ${QT_UI_HEADERS} ${QT_RESOURCES})
target_link_libraries(gameoflife Qt5::Widgets ${Boost_LIBRARIES})
