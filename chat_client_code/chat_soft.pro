#-------------------------------------------------
#
# Project created by QtCreator 2021-03-01T17:08:37
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = chat_soft
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        widget.cpp \
    chatlist.cpp \
    addfriend.cpp \
    creategroup.cpp \
    addgroup.cpp \
    privatechat.cpp \
    groupchat.cpp \
    sendthread.cpp \
    recvthread.cpp

HEADERS  += widget.h \
    chatlist.h \
    addfriend.h \
    creategroup.h \
    addgroup.h \
    privatechat.h \
    groupchat.h \
    sendthread.h \
    recvthread.h

FORMS    += widget.ui \
    chatlist.ui \
    addfriend.ui \
    creategroup.ui \
    addgroup.ui \
    privatechat.ui \
    groupchat.ui
