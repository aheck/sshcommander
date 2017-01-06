cmake_minimum_required(VERSION 3.0)

include_directories("../src")

add_executable(tests main.cpp
    InstanceItemModelTests.cpp

    ../src/AboutDialog.cpp
    ../src/AWSCache.cpp
    ../src/AWSConnector.cpp
    ../src/AWSInfoWidget.cpp
    ../src/AWSInstance.cpp
    ../src/AWSTag.cpp
    ../src/AWSResponseParsers.cpp
    ../src/AWSSecurityGroup.cpp
    ../src/AWSWidget.cpp
    ../src/ConnectionListView.cpp
    ../src/ConnectionListWidget.cpp
    ../src/CustomTabBar.cpp
    ../src/DisabledWidget.cpp
    ../src/InactiveSessionWidget.cpp
    ../src/InstanceItemModel.cpp
    ../src/MachineInfoWidget.cpp
    ../src/MainWindow.cpp
    ../src/NewDialog.cpp
    ../src/NotesEditor.cpp
    ../src/Preferences.cpp
    ../src/PreferencesDialog.cpp
    ../src/SecurityGroupsDialog.cpp
    ../src/SSHConnection.cpp
    ../src/SSHConnectionEntry.cpp
    ../src/SSHConnectionItemModel.cpp
    ../src/SSHConnectionManager.cpp
    ../src/TabbedTerminalWidget.cpp
    ../src/TerminalViewWidget.cpp
    ../src/TagsDialog.cpp
)

target_link_libraries(tests ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Test_LIBRARIES}
    ${QTERMWIDGET_LIBRARIES} ${LIBSSH2_LIBRARIES})
