################################################################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
###################################################################

QWT_ROOT = $${PWD}/../Qwt/qwt-6.1.5
# QWT_ROOT = /usr/lib/arm-linux-gnueabihf/qt5/mkspecs/features
include( $${QWT_ROOT}/qwtconfig.pri )
# include( $${PWD}/qwtbuild.pri )
include( $${QWT_ROOT}/qwtfunctions.pri )

# QWT_OUT_ROOT = $${OUT_PWD}/../..

QWT_OUT_ROOT = $${QWT_ROOT}

TEMPLATE     = app

INCLUDEPATH += /usr/include/qwt
INCLUDEPATH += $${QWT_ROOT}/src
DEPENDPATH  += $${QWT_ROOT}/src

!debug_and_release {

    DESTDIR      = $${PWD}/bin
}
else {
    CONFIG(debug, debug|release) {

        DESTDIR      = $${QWT_OUT_ROOT}/examples/bin_debug
    }
    else {

        DESTDIR      = $${QWT_OUT_ROOT}/examples/bin
    }
}

QMAKE_RPATHDIR *= $${QWT_OUT_ROOT}/lib
# qwtAddLibrary($${QWT_OUT_ROOT}/lib, qwt-)
qwtAddLibrary(/usr/lib, qwt-qt5)

greaterThan(QT_MAJOR_VERSION, 4) {

    QT += printsupport
    QT += concurrent
}

contains(QWT_CONFIG, QwtOpenGL ) {

    QT += opengl
}
else {

    DEFINES += QWT_NO_OPENGL
}

contains(QWT_CONFIG, QwtSvg) {

    QT += svg
}
else {

    DEFINES += QWT_NO_SVG
}


win32 {
    contains(QWT_CONFIG, QwtDll) {
        DEFINES    += QT_DLL QWT_DLL
    }
}
