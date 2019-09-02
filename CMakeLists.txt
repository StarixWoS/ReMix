cmake_minimum_required(VERSION 3.12)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -s")

set(BUILD_VERSION 2.4.9 CACHE STRING "ReMix version number")
project(ReMix VERSION ${BUILD_VERSION})

set(ReMix_VERSION_2 "v${ReMix_VERSION}")
string(REPLACE "." "," RC_VERSION "${ReMix_VERSION},0")
add_compile_definitions(RC_VERSION_STRING=\"${ReMix_VERSION}\" RC_VERSION=${RC_VERSION})
add_compile_definitions(REMIX_VERSION=\"${ReMix_VERSION}\")

find_package(Qt5 COMPONENTS Core Widgets Gui Network Network Concurrent REQUIRED)

set(CMAKE_AUTOUIC_SEARCH_PATHS ui/ ui/widgets/)
set(SRC_LIST src/ src/widgets/)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SOURCES src/views/loggersortproxymodel.cpp
        src/widgets/commentviewwidget.cpp
        src/views/usersortproxymodel.cpp
        src/views/plrsortproxymodel.cpp
        src/widgets/settingswidget.cpp
        src/widgets/remixtabwidget.cpp
        src/widgets/plrlistwidget.cpp
        src/views/tbleventfilter.cpp
        src/widgets/userdelegate.cpp
        src/widgets/remixwidget.cpp
        src/widgets/ruleswidget.cpp
        src/widgets/motdwidget.cpp
        src/createinstance.cpp
        src/appeventfilter.cpp
        src/packethandler.cpp
        src/worldshuffler.cpp
        src/packetforge.cpp
        src/selectworld.cpp
        src/cmdhandler.cpp
        src/serverinfo.cpp
        src/cmdtable.cpp
        src/chatview.cpp
        src/comments.cpp
        src/settings.cpp
        src/runguard.cpp
        src/randdev.cpp
        src/sendmsg.cpp
        src/server.cpp
        src/player.cpp
        src/helper.cpp
        src/logger.cpp
        src/theme.cpp
        src/rules.cpp
        src/remix.cpp
        src/user.cpp
        src/upnp.cpp
        src/main.cpp)
set(HEADERS src/views/loggersortproxymodel.hpp
        src/widgets/commentviewwidget.hpp
        src/views/usersortproxymodel.hpp
        src/views/plrsortproxymodel.hpp
        src/widgets/settingswidget.hpp
        src/widgets/remixtabwidget.hpp
        src/widgets/plrlistwidget.hpp
        src/views/tbleventfilter.hpp
        src/widgets/userdelegate.hpp
        src/widgets/remixwidget.hpp
        src/widgets/ruleswidget.hpp
        src/widgets/motdwidget.hpp
        src/createinstance.hpp
        src/appeventfilter.hpp
        src/packethandler.hpp
        src/worldshuffler.hpp
        src/packetforge.hpp
        src/selectworld.hpp
        src/cmdhandler.hpp
        src/serverinfo.hpp
        src/prototypes.hpp
        src/cmdtable.hpp
        src/chatview.hpp
        src/comments.hpp
        src/settings.hpp
        src/runguard.hpp
        src/randdev.hpp
        src/sendmsg.hpp
        src/server.hpp
        src/player.hpp
        src/helper.hpp
        src/logger.hpp
        src/theme.hpp
        src/rules.hpp
        src/remix.hpp
        src/user.hpp
        src/upnp.hpp)

set(UI ui/widgets/commentviewwidget.ui
    ui/widgets/settingswidget.ui
    ui/widgets/chatviewwidget.ui
    ui/widgets/plrlistwidget.ui
    ui/widgets/remixwidget.ui
    ui/widgets/ruleswidget.ui
    ui/widgets/motdwidget.ui
    ui/createinstance.ui
    ui/selectworld.ui
    ui/chatview.ui
    ui/comments.ui
    ui/settings.ui
    ui/sendmsg.ui
    ui/logger.ui
    ui/remix.ui
    ui/user.ui)

set(RC resources/icons.qrc
    resources/ReMix.rc)

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${UI} ${RC})
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        SUFFIX _${BUILD_VERSION}.exe
)

target_include_directories(${PROJECT_NAME} PUBLIC src)

target_link_libraries(ReMix Qt5::Widgets)
target_link_libraries(ReMix Qt5::Network)
target_link_libraries(ReMix Qt5::Core)
target_link_libraries(ReMix Qt5::Gui)
target_link_libraries(ReMix Qt5::Concurrent)
