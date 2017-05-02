cmake_minimum_required(VERSION 3.0)

include_directories("../src")

add_executable(tests main.cpp
    InstanceItemModelTests.cpp

    ../src/applets/AWSInfoApplet.cpp
    ../src/applets/NotesEditor.cpp
    ../src/applets/MachineInfoApplet.cpp
    ../src/applets/PortsApplet.cpp
    ../src/applets/PortsItemModel.cpp
    ../src/applets/RoutesApplet.cpp
    ../src/applets/RoutesItemModel.cpp
    ../src/applets/ProcessesApplet.cpp
    ../src/applets/ProcessesItemModel.cpp
    ../src/AboutDialog.cpp
    ../src/Applet.cpp
    ../src/AppletWidget.cpp
    ../src/AWSCache.cpp
    ../src/AWSConnector.cpp
    ../src/AWSConsoleWindow.cpp
    ../src/AWSImage.cpp
    ../src/AWSInstance.cpp
    ../src/AWSRoute.cpp
    ../src/AWSRouteTable.cpp
    ../src/AWSTag.cpp
    ../src/AWSResponseParsers.cpp
    ../src/AWSSecurityGroup.cpp
    ../src/AWSSubnet.cpp
    ../src/AWSVpc.cpp
    ../src/AWSWidget.cpp
    ../src/ConnectionListView.cpp
    ../src/ConnectionListWidget.cpp
    ../src/CustomTabBar.cpp
    ../src/DisabledWidget.cpp
    ../src/ImageDialog.cpp
    ../src/InactiveSessionWidget.cpp
    ../src/InstanceItemModel.cpp
    ../src/MainWindow.cpp
    ../src/NewDialog.cpp
    ../src/Preferences.cpp
    ../src/PreferencesDialog.cpp
    ../src/RouteTableViewWidget.cpp
    ../src/SecurityGroupsDialog.cpp
    ../src/SSHConnection.cpp
    ../src/SSHConnectionEntry.cpp
    ../src/SSHConnectionItemModel.cpp
    ../src/SSHConnectionManager.cpp
    ../src/SubnetDialog.cpp
    ../src/TabbedTerminalWidget.cpp
    ../src/TerminalViewWidget.cpp
    ../src/TagsDialog.cpp
    ../src/TagsViewWidget.cpp
    ../src/VpcDialog.cpp
)

target_link_libraries(tests ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Test_LIBRARIES}
    ${QTERMWIDGET_LIBRARIES} ${LIBSSH2_LIBRARIES})
