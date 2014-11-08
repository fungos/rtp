#-------------------------------------------------
#
# Project created by QtCreator 2014-07-17T11:17:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets concurrent

TARGET = RTM
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
		mainwindow.cpp \
	imageselectlabel.cpp \
	ialgorithm.cpp \
	algorithmcopy.cpp \
	algorithmsort.cpp \
	algorithmswap.cpp

HEADERS  += mainwindow.h \
	imageselectlabel.h \
	ialgorithm.h \
	algorithmcopy.h \
	pixel.h \
	algorithmsort.h \
	algorithmswap.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    README.md
