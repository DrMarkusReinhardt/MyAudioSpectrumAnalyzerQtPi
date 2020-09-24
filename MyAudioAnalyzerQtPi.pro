include( $${PWD}/myprojects.pri )

QT += core gui
QT += charts
QT += widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14
QMAKE_CXXFLAGS_RELEASE += -O2

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += /usr/include/arm-linux-gnueabihf/qt5

SOURCES += \
    main.cpp \
    SimMainWindow.cpp \
    SignalPlotView.cpp  \
    SpectrumPlotView.cpp  \
    plot2D.cpp \
    SpectrumCalculator.cpp \
    callout.cpp \
    knob.cpp \
    THDCalculator.cpp \
    THDHandler.cpp \
    StoreSignalSpectrumData.cpp \
    SpectrumAverager.cpp \
    VectorIIRFilter.cpp

HEADERS += \
    SimMainWindow.h \
    SignalPlotView.h  \
    SpectrumPlotView.h  \
    plot2D.h \
    PortaudioHandler.h \
    RandomGenerator.h \
    DefineDataSizes.h \
    SpectrumCalculator.h \
    DTFT.h \
    callout.h \
    SpectrumParameter.h \
    knob.h \
    THDCalculator.h \
    THDHandler.h \
    StoreSignalSpectrumData.h \
    SpectrumAverager.h \
    VectorIIRFilter.h

# INCLUDEPATH += /usr/include/sigc++-2.0/
# INCLUDEPATH += /usr/lib/x86_64-linux-gnu/sigc++-2.0/include
LIBS += -llapack -lGL -lGLU -lX11 -lXcursor -lrt -lm -pthread -lasound -ljack -lportaudio
LIBS += -lqwt

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# FORMS += \
#     form.ui
