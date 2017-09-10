#-------------------------------------------------
#
# Project created by QtCreator 2014-09-28T23:51:10
#
#-------------------------------------------------

VERSION = 2.1.9 # major.minor.patch

QT += core \
      gui \
      widgets \
      network \
      concurrent

TEMPLATE = app

INCLUDEPATH += "./src" \
               "./plugin/src"

UI_DIR = "./src"
CONFIG += strict_c++ c++14

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS += -std=c++14 -s
    TARGET = "../bin/ReMix-$$VERSION-Release-Qt$$QT_VERSION"
    DESTDIR = "./build-$$QT_VERSION/release"
    RCC_DIR = "./build-$$QT_VERSION/release/rcc"
    MOC_DIR = "./build-$$QT_VERSION/release/moc"
    OBJECTS_DIR = "./build-$$QT_VERSION/release/obj"
} else {
    QMAKE_CXXFLAGS += -std=c++14 -Wall
    TARGET = "../bin/ReMix-$$VERSION-Debug-Qt$$QT_VERSION"
    DESTDIR = "./build-$$QT_VERSION/debug"
    RCC_DIR = "./build-$$QT_VERSION/debug/rcc"
    MOC_DIR = "./build-$$QT_VERSION/debug/moc"
    OBJECTS_DIR = "./build-$$QT_VERSION/debug/obj"
}

DEFINES += REMIX_VERSION=\\\"$${VERSION}\\\"
win32:QMAKE_TARGET_COMPANY = AHitB
win32:QMAKE_TARGET_PRODUCT = ReMix-$${VERSION}
win32:QMAKE_TARGET_DESCRIPTION = "A replacement for the synthetic-reality Mix Game Server!"

win32:RC_ICONS += "./resources/ReMix.ico"
else:ICON += "./resources/ReMix.ico"

SOURCES += src/tblview/usersortproxymodel.cpp\
           src/tblview/plrsortproxymodel.cpp \
           src/tblview/tbleventfilter.cpp \
           src/widgets/settingswidget.cpp \
           src/widgets/remixtabwidget.cpp \
           src/widgets/plrlistwidget.cpp \
           src/widgets/userdelegate.cpp \
           src/widgets/remixwidget.cpp \
           src/widgets/ruleswidget.cpp \
           src/widgets/motdwidget.cpp \
           src/createinstance.cpp \
           src/packethandler.cpp \
           src/packetforge.cpp \
           src/selectworld.cpp \
           src/cmdhandler.cpp \
           src/serverinfo.cpp \
           src/chatview.cpp \
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
           src/upnp.cpp \
           src/main.cpp \
    src/appeventfilter.cpp

HEADERS += src/tblview/usersortproxymodel.hpp\
           src/tblview/plrsortproxymodel.hpp \
           src/tblview/tbleventfilter.hpp \
           src/widgets/settingswidget.hpp \
           src/widgets/remixtabwidget.hpp \
           src/widgets/plrlistwidget.hpp \
           src/widgets/userdelegate.hpp \
           src/widgets/remixwidget.hpp \
           src/widgets/ruleswidget.hpp \
           src/widgets/motdwidget.hpp \
           src/createinstance.hpp \
           src/packethandler.hpp \
           src/packetforge.hpp \
           src/selectworld.hpp \
           src/themechange.hpp \
           src/cmdhandler.hpp \
           src/serverinfo.hpp \
           src/prototypes.hpp \
           src/chatview.hpp \
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
           src/user.hpp \
           src/upnp.hpp \
    src/appeventfilter.hpp

FORMS += ui/widgets/settingswidget.ui \
         ui/widgets/plrlistwidget.ui \
         ui/widgets/remixwidget.ui \
         ui/widgets/ruleswidget.ui \
         ui/widgets/motdwidget.ui \
         ui/createinstance.ui \
         ui/selectworld.ui \
         ui/chatview.ui \
         ui/comments.ui \
         ui/settings.ui \
         ui/sendmsg.ui \
         ui/remix.ui \
         ui/user.ui

RESOURCES += resources/icons.qrc

DISTFILES +=
