cmake_minimum_required(VERSION 3.14)

project(StrawberryRemote VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Network Protobuf)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Network Protobuf)

qt_add_protobuf(RemoteMessages
    GENERATE_PACKAGE_SUBFOLDERS
    PROTO_FILES
        proto/RemoteMessages.proto)

target_sources(RemoteMessages
  PRIVATE

)

add_executable(StrawberryRemote
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        connectionstatus.ui
        connectionstatus.h
        connectionstatus.cpp
        controller.h
        controller.cpp
        outgoingmsg.h
        outgoingmsg.cpp
        incomingmsg.h
        incomingmsg.cpp
        connection.h
        connection.cpp
        player.h
        player.cpp
        player.ui
)

target_include_directories(RemoteMessages PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(StrawberryRemote
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Protobuf
    RemoteMessages)


include(GNUInstallDirs)
install(TARGETS StrawberryRemote
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
