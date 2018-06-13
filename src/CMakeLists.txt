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
    applets/AWSInfoApplet.cpp
    applets/FileBrowserApplet.cpp
    applets/FileTransfersApplet.cpp
    applets/FileTransfersItemModel.cpp
    applets/NotesEditor.cpp
    applets/MachineInfoApplet.cpp
    applets/PortsApplet.cpp
    applets/PortsItemModel.cpp
    applets/PortsNewTunnelDialog.cpp
    applets/ProcessesApplet.cpp
    applets/ProcessesItemModel.cpp
    applets/RoutesApplet.cpp
    applets/RoutesItemModel.cpp
    applets/SFTPDirectoryDialog.cpp
    applets/SFTPFilesystemModel.cpp
    applets/SSHFilesystemApplet.cpp
    applets/SSHFilesystemItemModel.cpp
    applets/SSHFilesystemNewDialog.cpp
    applets/TunnelsApplet.cpp
    applets/TunnelsItemModel.cpp
    applets/TunnelsNewDialog.cpp
    AboutDialog.cpp
    Applet.cpp
    AppletWidget.cpp
    AWSCache.cpp
    AWSConnector.cpp
    AWSConsoleWindow.cpp
    AWSImage.cpp
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
    DetachedSessionWidget.cpp
    DetachedTerminalWindow.cpp
    DisabledWidget.cpp
    DirEntry.cpp
    FileTransferJob.cpp
    FileTransferWorker.cpp
    ImageDialog.cpp
    InactiveSessionWidget.cpp
    InstanceItemModel.cpp
    MainWindow.cpp
    NewDialog.cpp
    Preferences.cpp
    PreferencesDialog.cpp
    PreferencesListWidget.cpp
    RichTextDelegate.cpp
    RouteTableViewWidget.cpp
    SecurityGroupsDialog.cpp
    SSHConnection.cpp
    SSHConnectionEntry.cpp
    SSHConnectionItemModel.cpp
    SSHConnectionManager.cpp
    SSHFilesystemManager.cpp
    SSHTermWidget.cpp
    SubnetDialog.cpp
    TabbedTerminalWidget.cpp
    TerminalContainer.cpp
    TerminalViewWidget.cpp
    TagsDialog.cpp
    TagsViewWidget.cpp
    TunnelManager.cpp
    Util.cpp
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
