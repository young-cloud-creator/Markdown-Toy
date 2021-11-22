QT       += core gui webenginewidgets core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

INCLUDEPATH +=
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    latex.cpp \
    main.cpp \
    mainwindow.cpp \
    markdown.cpp \
    word.cpp

HEADERS += \
    latex.h \
    mainwindow.h \
    markdown.h \
    word.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES +=

win32: LIBS += -L$$PWD/../../../Qt/quazip_qt6/lib/ -lquazip

INCLUDEPATH += $$PWD/../../../Qt/quazip_qt6/include
DEPENDPATH += $$PWD/../../../Qt/quazip_qt6/include
