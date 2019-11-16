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
    applets/FileSystemModel.cpp
    applets/FileTransfersApplet.cpp
    applets/FileTransfersApplet.cpp
    applets/FileTransfersItemModel.cpp
    applets/NotesEditor.cpp
    applets/MachineInfoApplet.cpp
    applets/PortsApplet.cpp
    applets/PortsItemModel.cpp
    applets/ProcessesApplet.cpp
    applets/ProcessesItemModel.cpp
    applets/RemoteFileView.cpp
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
    libs/tinyaes/aes.c
    AboutDialog.cpp
    Applet.cpp
    AppletWidget.cpp
    AWS/AWSCache.cpp
    AWS/AWSConnector.cpp
    AWS/AWSImage.cpp
    AWS/AWSInstance.cpp
    AWS/AWSRoute.cpp
    AWS/AWSRouteTable.cpp
    AWS/AWSTag.cpp
    AWS/AWSResponseParsers.cpp
    AWS/AWSSecurityGroup.cpp
    AWS/AWSSubnet.cpp
    AWS/AWSVpc.cpp
    AWSConsoleWindow.cpp
    AWSWidget.cpp
    ConnectionListView.cpp
    ConnectionListWidget.cpp
    CResourceDestructor.cpp
    CustomTabBar.cpp
    DetachedSessionWidget.cpp
    DetachedTerminalWindow.cpp
    DisabledWidget.cpp
    DirEntry.cpp
    ExternalProgramFinder.cpp
    FileTransferJob.cpp
    FileTransferWorker.cpp
    ImageDialog.cpp
    InactiveSessionWidget.cpp
    InstanceItemModel.cpp
    KnownHosts.cpp
    MainWindow.cpp
    NewDialog.cpp
    NotificationManager.cpp
    NotificationPopup.cpp
    Preferences.cpp
    PreferencesDialog.cpp
    PreferencesListWidget.cpp
    PseudoTerminal.cpp
    RichTextDelegate.cpp
    RouteTableViewWidget.cpp
    SecurityGroupsDialog.cpp
    SessionTabBar.cpp
    SSHConnection.cpp
    SSHConnectionEntry.cpp
    SSHConnectionItemModel.cpp
    SSHConnectionManager.cpp
    SSHFilesystemManager.cpp
    SSHTermWidget.cpp
    SvgWidget.cpp
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
        ${Qt5Svg_LIBRARIES} -ldl libqtermwidget5.a libssh2.a libz.a libcrypt.a libcrypto.a libssl.a)
else()
    target_link_libraries(sshcommander ${Qt5Widgets_LIBRARIES} ${Qt5Network_LIBRARIES}
        ${Qt5Svg_LIBRARIES} ${QTERMWIDGET_LIBRARIES} ${LIBSSH2_LIBRARIES})
endif()

add_dependencies(sshcommander update_git_commit_id)
