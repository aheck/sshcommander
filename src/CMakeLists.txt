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
    AWSConnector.cpp
    AWSInfoWidget.cpp
    AWSInstance.cpp
    AWSTag.cpp
    AWSResponseParsers.cpp
    AWSSecurityGroup.cpp
    AWSWidget.cpp
    CustomListView.cpp
    CustomTabBar.cpp
    CustomTabWidget.cpp
    DisabledWidget.cpp
    InactiveSessionWidget.cpp
    InstanceItemModel.cpp
    MachineInfoWidget.cpp
    MainWindow.cpp
    NewDialog.cpp
    NotesEditor.cpp
    Preferences.cpp
    PreferencesDialog.cpp
    SecurityGroupsDialog.cpp
    SSHConnectionEntry.cpp
    SSHConnectionItemModel.cpp
    TagsDialog.cpp
    ${ProgramResources}
)

target_link_libraries(sshcommander Qt5::Widgets Qt5::Network ${QTERMWIDGET_LIBRARIES})
add_dependencies(sshcommander update_git_commit_id)
