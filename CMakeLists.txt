cmake_minimum_required(VERSION 3.0)
project(sshcommander)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "-O1 -ggdb -std=c++14 -Wall -pthread")

find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(qtermwidget5)
find_package(Qt5Test)

find_package(PkgConfig)
pkg_search_module(LIBSSH2 REQUIRED libssh2)

include_directories(${QTERMWIDGET_INCLUDE_DIRS})
link_directories("/usr/local/lib")

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

add_test(tests tests/tests)
