cmake_minimum_required(VERSION 3.0)
project(sshcommander)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-O0 -ggdb -std=c++14 -Wall -pthread")

set(qtermwidget5_DIR "../qtermwidget")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(qtermwidget5 REQUIRED)
find_package(Qt5Test REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(LIBSSH2 REQUIRED libssh2)

include_directories(
    ${QTERMWIDGET_INCLUDE_DIRS}
    ${LIBSSH2_INCLUDEDIR}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Network_INCLUDE_DIRS}
    ${Qt5Test_INCLUDE_DIRS}
    ${Qt5Svg_INCLUDE_DIRS})

link_directories("/usr/local/lib")

option(STATIC_BUILD "Create a build where qtermwidget and libssh2 are statically linked")

add_subdirectory(src)
add_subdirectory(tests)

add_test(tests tests/tests)
