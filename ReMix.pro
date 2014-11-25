#-------------------------------------------------
#
# Project created by QtCreator 2014-09-28T23:51:10
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = ReMix
TEMPLATE = app

UI_DIR = src
MOC_DIR = build/moc
RCC_DIR = build/rcc
OBJECTS_DIR = build/obj

#CONFIG += static c++11
QMAKE_CXXFLAGS = -std=c++11 #-static-libstdc++ -static-libgcc -static -Os -s

SOURCES += src/main.cpp\
           src/remix.cpp \
           src/player.cpp \
           src/messages.cpp \
           src/usermessage.cpp \
           src/server.cpp \
           src/serverinfo.cpp \
           src/helper.cpp \
           src/randdev.cpp \
           src/admin.cpp \
           src/adminhelper.cpp \
           src/bandialog.cpp \
           src/settings.cpp

HEADERS += src/player.hpp \
           src/remix.hpp \
           src/messages.hpp \
           src/usermessage.hpp \
           src/server.hpp \
           src/serverinfo.hpp \
           src/helper.hpp \
           src/randdev.hpp \
           src/admin.hpp \
           src/adminhelper.hpp \
           src/bandialog.hpp \
           src/settings.hpp

FORMS   += ui/remix.ui \
           ui/messages.ui \
           ui/usermessage.ui \
           ui/admin.ui \
           ui/bandialog.ui \
           ui/settings.ui
