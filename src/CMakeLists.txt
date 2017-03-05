cmake_minimum_required(VERSION 3.0)

add_custom_target(
    update_git_commit_id
    COMMAND ../update_git_commit_id.sh
)

QT5_ADD_RESOURCES (ProgramResources
    sshcommander.qrc
)

add_executable(sshcommander
    main.cpp
    AboutDialog.cpp
    Applet.cpp
    AppletWidget.cpp
    AWSCache.cpp
    AWSConnector.cpp
    AWSImage.cpp
    AWSInfoWidget.cpp
    AWSInstance.cpp
    AWSRoute.cpp
    AWSRouteTable.cpp
    AWSTag.cpp
    AWSResponseParsers.cpp
    AWSSecurityGroup.cpp
    AWSSubnet.cpp
    AWSVpc.cpp
    AWSWidget.cpp
    ConnectionListView.cpp
    ConnectionListWidget.cpp
    CustomTabBar.cpp
    DisabledWidget.cpp
    ImageDialog.cpp
    InactiveSessionWidget.cpp
    InstanceItemModel.cpp
    MachineInfoWidget.cpp
    MainWindow.cpp
    NewDialog.cpp
    NotesEditor.cpp
    Preferences.cpp
    PreferencesDialog.cpp
    RouteTableViewWidget.cpp
    SecurityGroupsDialog.cpp
    SSHConnection.cpp
    SSHConnectionEntry.cpp
    SSHConnectionItemModel.cpp
    SSHConnectionManager.cpp
    SubnetDialog.cpp
    TabbedTerminalWidget.cpp
    TerminalViewWidget.cpp
    TagsDialog.cpp
    TagsViewWidget.cpp
    VpcDialog.cpp
    ${ProgramResources}
)

if(STATIC_BUILD)
    target_link_libraries(sshcommander ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES}
        -ldl libqtermwidget5.a libssh2.a libz.a libcrypt.a libcrypto.a libssl.a)
else()
    target_link_libraries(sshcommander ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES}
        ${QTERMWIDGET_LIBRARIES} ${LIBSSH2_LIBRARIES})
endif()

add_dependencies(sshcommander update_git_commit_id)
