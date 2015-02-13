#-------------------------------------------------
#
# Project created by QtCreator 2014-09-28T23:51:10
#
#-------------------------------------------------

VERSION = 1.0.0
QT += core \
      gui \
      widgets \
      network

TEMPLATE = app

INCLUDEPATH += "./plugin/src"
UI_DIR = "./src"

CONFIG += C++14

CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS = -std=c++14 -Os -s
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
DEFINES += USE_DARK_FUSION_UI=1 #Tell Qt to use a Non-Native UI color scheme.
#DECRYPT_PACKET_PLUGIN=1 \ #Tell Qt to load the Packet decryption plugin.
#USE_MULTIWORLD_FEATURE=1 #the server supports Multi-World hosting.

SOURCES += src/tblview/adminsortproxymodel.cpp \
           src/tblview/plrsortproxymodel.cpp \
           src/tblview/dvsortproxymodel.cpp \
           src/tblview/ipsortproxymodel.cpp \
           src/tblview/snsortproxymodel.cpp \
           src/tblview/tbleventfilter.cpp \
           src/widgets/dabanwidget.cpp \
           src/widgets/dvbanwidget.cpp \
           src/widgets/ipbanwidget.cpp \
           src/widgets/snbanwidget.cpp \
           src/packethandler.cpp \
           src/cmdhandler.cpp \
           src/serverinfo.cpp \
           src/bandialog.cpp \
           src/messages.cpp \
           src/comments.cpp \
           src/settings.cpp \
           src/randdev.cpp \
           src/server.cpp \
           src/player.cpp \
           src/helper.cpp \
           src/admin.cpp \
           src/remix.cpp \
           src/main.cpp

HEADERS += src/tblview/adminsortproxymodel.hpp \
           src/tblview/plrsortproxymodel.hpp \
           src/tblview/dvsortproxymodel.hpp \
           src/tblview/ipsortproxymodel.hpp \
           src/tblview/snsortproxymodel.hpp \
           src/tblview/tbleventfilter.hpp \
           src/widgets/dabanwidget.hpp \
           src/widgets/dvbanwidget.hpp \
           src/widgets/ipbanwidget.hpp \
           src/widgets/snbanwidget.hpp \
           src/packethandler.hpp \
           src/cmdhandler.hpp \
           src/serverinfo.hpp \
           src/prototypes.hpp \
           src/bandialog.hpp \
           src/includes.hpp \
           src/messages.hpp \
           src/comments.hpp \
           src/settings.hpp \
           src/randdev.hpp \
           src/server.hpp \
           src/player.hpp \
           src/helper.hpp \
           src/admin.hpp \
           src/remix.hpp

FORMS += ui/widgets/dabanwidget.ui \
         ui/widgets/dvbanwidget.ui \
         ui/widgets/ipbanwidget.ui \
         ui/widgets/snbanwidget.ui \
         ui/bandialog.ui \
         ui/messages.ui \
         ui/comments.ui \
         ui/settings.ui \
         ui/admin.ui \
         ui/remix.ui

RESOURCES += resources/icons.qrc
