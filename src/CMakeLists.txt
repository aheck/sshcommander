cmake_minimum_required(VERSION 3.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "-O1 -ggdb -std=c++11 -Wall")

find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5Xml)
find_package(qtermwidget5)

include_directories(${QTERMWIDGET_INCLUDE_DIRS})
link_directories("/usr/local/lib")

add_executable(sshcommander
    main.cpp
    AWSConnector.cpp
    AWSWidget.cpp
    CustomTabBar.cpp
    CustomTabWidget.cpp
    InstanceItemModel.cpp
    MachineInfoWidget.cpp
    MainWindow.cpp
    NewDialog.cpp
    SSHConnectionEntry.cpp
    SSHConnectionItemModel.cpp
)
target_link_libraries(sshcommander Qt5::Widgets Qt5::Network Qt5::Xml ${QTERMWIDGET_LIBRARIES})
