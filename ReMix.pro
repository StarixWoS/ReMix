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
Release:QMAKE_CXXFLAGS = -std=c++14 -Os -s
Release:TARGET = "../bin/ReMix-Release-Qt$$QT_VERSION"
Release:DESTDIR = "./build-$$QT_VERSION/release"
Release:RCC_DIR = "./build-$$QT_VERSION/release/rcc"
Release:MOC_DIR = "./build-$$QT_VERSION/release/moc"
Release:OBJECTS_DIR = "./build-$$QT_VERSION/release/obj"

Debug:QMAKE_CXXFLAGS = -std=c++14 -Wall
Debug:TARGET = "../bin/ReMix-Debug-Qt$$QT_VERSION"
Debug:DESTDIR = "./build-$$QT_VERSION/debug"
Debug:RCC_DIR = "./build-$$QT_VERSION/debug/rcc"
Debug:MOC_DIR = "./build-$$QT_VERSION/debug/moc"
Debug:OBJECTS_DIR = "./build-$$QT_VERSION/debug/obj"

ICON = "./resources/ReMix.ico"
win32:RC_FILE += "./resources/ReMix.rc"

#Unfortunately, there is no way to declare specific defines for release/build.
DEFINES += USE_DARK_FUSION_UI=1 #Tell Qt to use a Non-Native UI color scheme.
#DECRYPT_PACKET_PLUGIN=1 \ #Tell Qt to load the Packet decryption plugin.
#USE_MULTIWORLD_FEATURE=1 #the server supports Multi-World hosting.

SOURCES += src/main.cpp \
           src/remix.cpp \
           src/player.cpp \
           src/messages.cpp \
           src/usermessage.cpp \
           src/server.cpp \
           src/serverinfo.cpp \
           src/helper.cpp \
           src/randdev.cpp \
           src/admin.cpp \
           src/bandialog.cpp \
           src/settings.cpp \
           src/cmdhandler.cpp \

SOURCES += src/tblview/adminsortproxymodel.cpp \
           src/tblview/plrsortproxymodel.cpp \
           src/tblview/ipsortproxymodel.cpp \
           src/tblview/snsortproxymodel.cpp \
           src/tblview/tbleventfilter.cpp

HEADERS += src/player.hpp \
           src/remix.hpp \
           src/messages.hpp \
           src/usermessage.hpp \
           src/server.hpp \
           src/serverinfo.hpp \
           src/helper.hpp \
           src/randdev.hpp \
           src/admin.hpp \
           src/bandialog.hpp \
           src/settings.hpp \
           src/includes.hpp \
           src/prototypes.hpp \
           src/cmdhandler.hpp \

HEADERS += src/tblview/adminsortproxymodel.hpp \
           src/tblview/plrsortproxymodel.hpp \
           src/tblview/ipsortproxymodel.hpp \
           src/tblview/snsortproxymodel.hpp \
           src/tblview/tbleventfilter.hpp

FORMS += ui/remix.ui \
         ui/messages.ui \
         ui/usermessage.ui \
         ui/admin.ui \
         ui/bandialog.ui \
         ui/settings.ui

RESOURCES += resources/icons.qrc
