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

add_custom_target(
    update_git_commit_id
    COMMAND ../update_git_commit_id.sh
)

add_executable(sshcommander
    main.cpp
    AboutDialog.cpp
    AWSConnector.cpp
    AWSInfoWidget.cpp
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
add_dependencies(sshcommander update_git_commit_id)
