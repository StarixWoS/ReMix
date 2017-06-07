#-------------------------------------------------
#
# Project created by QtCreator 2014-09-28T23:51:10
#
#-------------------------------------------------

VERSION = 1.0.0
QT += core \
      gui \
      widgets \
      network \
      concurrent

TEMPLATE = app

INCLUDEPATH += "./src" \
               "./plugin/src"

UI_DIR = "./src"

CONFIG += C++14

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS = -std=c++14 -s
    TARGET = "../bin/ReMix-Release-Qt$$QT_VERSION"
    DESTDIR = "./build-$$QT_VERSION/release"
    RCC_DIR = "./build-$$QT_VERSION/release/rcc"
    MOC_DIR = "./build-$$QT_VERSION/release/moc"
    OBJECTS_DIR = "./build-$$QT_VERSION/release/obj"
} else {
    QMAKE_CXXFLAGS = -std=c++14 -Wall
    TARGET = "../bin/ReMix-Debug-Qt$$QT_VERSION"
    DESTDIR = "./build-$$QT_VERSION/debug"
    RCC_DIR = "./build-$$QT_VERSION/debug/rcc"
    MOC_DIR = "./build-$$QT_VERSION/debug/moc"
    OBJECTS_DIR = "./build-$$QT_VERSION/debug/obj"
}

ICON = "./resources/ReMix.ico"
win32:RC_FILE += "./resources/ReMix.rc"

#Unfortunately, there is no way to declare specific defines for release/build.
#DEFINES += DECRYPT_PACKET_PLUGIN=1 \ #Tell Qt to load the Packet decryption plugin.
#           USE_MULTIWORLD_FEATURE=1 #the server supports Multi-World hosting.

SOURCES += src/tblview/usersortproxymodel.cpp\
           src/tblview/plrsortproxymodel.cpp \
           src/tblview/tbleventfilter.cpp \
           src/widgets/settingswidget.cpp \
           src/widgets/messageswidget.cpp \
           src/widgets/remixtabwidget.cpp \
           src/widgets/plrlistwidget.cpp \
           src/widgets/userdelegate.cpp \
           src/widgets/remixwidget.cpp \
           src/widgets/ruleswidget.cpp \
           src/createinstance.cpp \
           src/packethandler.cpp \
           src/selectworld.cpp \
           src/cmdhandler.cpp \
           src/serverinfo.cpp \
           src/comments.cpp \
           src/settings.cpp \
           src/randdev.cpp \
           src/sendmsg.cpp \
           src/server.cpp \
           src/player.cpp \
           src/helper.cpp \
           src/rules.cpp \
           src/remix.cpp \
           src/user.cpp \
           src/main.cpp

HEADERS += src/tblview/usersortproxymodel.hpp\
           src/tblview/plrsortproxymodel.hpp \
           src/tblview/tbleventfilter.hpp \
           src/widgets/settingswidget.hpp \
           src/widgets/messageswidget.hpp \
           src/widgets/remixtabwidget.hpp \
           src/widgets/plrlistwidget.hpp \
           src/widgets/userdelegate.hpp \
           src/widgets/remixwidget.hpp \
           src/widgets/ruleswidget.hpp \
           src/createinstance.hpp \
           src/packethandler.hpp \
           src/selectworld.hpp \
           src/cmdhandler.hpp \
           src/serverinfo.hpp \
           src/prototypes.hpp \
           src/includes.hpp \
           src/comments.hpp \
           src/settings.hpp \
           src/randdev.hpp \
           src/sendmsg.hpp \
           src/server.hpp \
           src/player.hpp \
           src/helper.hpp \
           src/rules.hpp \
           src/remix.hpp \
           src/user.hpp

FORMS += ui/widgets/settingswidget.ui \
         ui/widgets/messageswidget.ui \
         ui/widgets/plrlistwidget.ui \
         ui/widgets/remixwidget.ui \
         ui/widgets/ruleswidget.ui \
         ui/createinstance.ui \
         ui/selectworld.ui \
         ui/comments.ui \
         ui/settings.ui \
         ui/sendmsg.ui \
         ui/remix.ui \
         ui/user.ui

RESOURCES += resources/icons.qrc
