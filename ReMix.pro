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
MOC_DIR = build
RCC_DIR = build
OBJECTS_DIR = build

CONFIG += static c++11
QMAKE_CXXFLAGS = -std=c++11 -static-libstdc++ -static-libgcc -static -Os -s

SOURCES += src/main.cpp\
           src/remix.cpp \
           src/player.cpp \
           src/messages.cpp \
           src/preferences.cpp \
           src/bannedip.cpp \
           src/bannedsernum.cpp \
           src/usermessage.cpp \
           src/server.cpp

HEADERS += src/player.hpp \
           src/remix.hpp \
           src/messages.hpp \
           src/preferences.hpp \
           src/bannedip.hpp \
           src/bannedsernum.hpp \
           src/usermessage.hpp \
           src/server.hpp \

FORMS   += ui/remix.ui \
           ui/messages.ui \
           ui/bannedip.ui \
           ui/bannedsernum.ui \
           ui/usermessage.ui
