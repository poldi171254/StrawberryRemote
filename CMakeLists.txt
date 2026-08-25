cmake_minimum_required(VERSION 3.16)

project(StrawberryRemote LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Core Network Protobuf)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Network Protobuf)

add_library(RemoteMessages STATIC)

qt_add_protobuf(RemoteMessages
  PROTO_FILES RemoteMessages.proto
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
        protocolconstants.h
        player.ui
        messagewindow.h messagewindow.cpp messagewindow.ui
        tokenprompt.cpp tokenprompt.h tokenprompt.ui
)

target_link_libraries(StrawberryRemote
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Protobuf
    RemoteMessages
)

include(GNUInstallDirs)
install(TARGETS StrawberryRemote
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
