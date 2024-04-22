cmake_minimum_required(VERSION 3.20.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(BUILD_VERSION 3.1.4 CACHE STRING "ReMix version number")
project(ReMix VERSION ${BUILD_VERSION})

set(ReMix_VERSION_2 "v${ReMix_VERSION}")
string(REPLACE "." "," RC_VERSION "${ReMix_VERSION}")
add_compile_definitions(RC_VERSION_STRING=\"${ReMix_VERSION}\" RC_VERSION=${RC_VERSION})
add_compile_definitions(REMIX_VERSION=\"${ReMix_VERSION}\")
add_compile_definitions(REMIX_DEVMODE=0)

find_package(Qt6 COMPONENTS Core Widgets Gui Network Network Concurrent Xml REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui/ ui/widgets/)
set(SRC_LIST src/ src/widgets/ src/thread src/views src/packethandlers src/upnp )
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES src/packethandlers/wospackethandler.cpp
        src/packethandlers/toypackethandler.cpp
        src/packethandlers/w97packethandler.cpp
        src/views/usersortproxymodel.cpp
        src/widgets/cmdtabledelegate.cpp
        src/views/plrsortproxymodel.cpp
        src/widgets/settingswidget.cpp
        src/widgets/remixtabwidget.cpp
        src/thread/mastermixthread.cpp
        src/widgets/plrlistwidget.cpp
        src/views/listeventfilter.cpp
        src/views/tbleventfilter.cpp
        src/widgets/userdelegate.cpp
        src/widgets/remixwidget.cpp
        src/widgets/ruleswidget.cpp
        src/widgets/colorwidget.cpp
        src/widgets/motdwidget.cpp
        src/thread/writethread.cpp
        src/thread/udpthread.cpp
        src/cmdtableoverride.cpp
        src/createinstance.cpp
        src/appeventfilter.cpp
        src/packethandler.cpp
        src/campexemption.cpp
        src/packetforge.cpp
        src/selectworld.cpp
        src/cmdhandler.cpp
        src/sortutil.cpp
        src/cmdtable.cpp
        src/chatview.cpp
        src/settings.cpp
        src/runguard.cpp
        src/randdev.cpp
        src/sendmsg.cpp
        src/server.cpp
        src/player.cpp
        src/helper.cpp
        src/logger.cpp
        src/theme.cpp
        src/remix.cpp
        src/user.cpp
        src/main.cpp)
set(HEADERS src/packethandlers/wospackethandler.hpp
        src/packethandlers/toypackethandler.hpp
        src/packethandlers/w97packethandler.hpp
        src/views/usersortproxymodel.hpp
        src/widgets/cmdtabledelegate.hpp
        src/views/plrsortproxymodel.hpp
        src/widgets/settingswidget.hpp
        src/widgets/remixtabwidget.hpp
        src/thread/mastermixthread.hpp
        src/widgets/plrlistwidget.hpp
        src/views/listeventfilter.hpp
        src/views/tbleventfilter.hpp
        src/widgets/userdelegate.hpp
        src/widgets/remixwidget.hpp
        src/widgets/ruleswidget.hpp
        src/widgets/colorwidget.hpp
        src/widgets/motdwidget.hpp
        src/thread/writethread.hpp
        src/thread/udpthread.hpp
        src/cmdtableoverride.hpp
        src/createinstance.hpp
        src/appeventfilter.hpp
        src/packethandler.hpp
        src/campexemption.hpp
        src/packetforge.hpp
        src/selectworld.hpp
        src/cmdhandler.hpp
        src/prototypes.hpp
        src/sortutil.hpp
        src/cmdtable.hpp
        src/chatview.hpp
        src/settings.hpp
        src/runguard.hpp
        src/randdev.hpp
        src/sendmsg.hpp
        src/server.hpp
        src/player.hpp
        src/helper.hpp
        src/logger.hpp
        src/theme.hpp
        src/remix.hpp
        src/user.hpp)

set(UPNP_SOURCES src/upnp/upnpwanservice.cpp
                 src/upnp/upnpdiscover.cpp
                 src/upnp/upnpservice.cpp
                 src/upnp/upnpdevice.cpp
                 src/upnp/upnpsoap.cpp
                 src/upnp/upnpreply.cpp
                 src/upnp/upnp.cpp)

set(UPNP_HEADERS src/upnp/upnpwanservice.hpp
                 src/upnp/upnpdiscover.hpp
                 src/upnp/upnpservice.hpp
                 src/upnp/upnpdevice.hpp
                 src/upnp/upnpsoap.hpp
                 src/upnp/upnpreply.hpp
                 src/upnp/upnp.hpp)


set(UI ui/widgets/settingswidget.ui
    ui/widgets/chatviewwidget.ui
    ui/widgets/plrlistwidget.ui
    ui/widgets/remixwidget.ui
    ui/widgets/ruleswidget.ui
    ui/widgets/colorwidget.ui
    ui/widgets/motdwidget.ui
    ui/cmdtableoverride.ui
    ui/createinstance.ui
    ui/selectworld.ui
    #ui/upnpdevice.ui
    ui/settings.ui
    ui/sendmsg.ui
    ui/logger.ui
    ui/remix.ui
    ui/user.ui)

set(RC resources/icons.qrc
    resources/ReMix.rc )

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${UPNP_SOURCES} ${UPNP_HEADERS} ${UI} ${RC} )
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        SUFFIX _${BUILD_VERSION}.exe
)

target_link_libraries(${PROJECT_NAME} Qt::Widgets)
target_link_libraries(${PROJECT_NAME} Qt::Network)
target_link_libraries(${PROJECT_NAME} Qt::Core)
target_link_libraries(${PROJECT_NAME} Qt::Gui)
target_link_libraries(${PROJECT_NAME} Qt::Concurrent)
target_link_libraries(${PROJECT_NAME} Qt::Xml)

target_include_directories(${PROJECT_NAME} PUBLIC src)
