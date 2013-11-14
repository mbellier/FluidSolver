QT +=  core gui widgets opengl xml

TARGET = FluidSolver
TEMPLATE = app

LFLAGS = -static-libgcc

HEADERS += \
    solver/Segment.hpp \
    solver/Obstacles.hpp \
    solver/Matrix3D.hpp \
    solver/Matrix2D.hpp \
    solver/Matrix.hpp \
    solver/FluidSolver2D.hpp \
    solver/FloatMatrix2D.hpp \
    display/ui_Dialog.h \
    display/SimplePrint.hpp \
    display/Print.hpp \
    display/ParticlesPrint.hpp \
    display/GUI.hpp \
    display/Dialog.hpp \
    display/CurvePrint.hpp \
    display/ColorPrint.hpp \
    config.hpp \
    display/Leap.h \
    display/LeapMath.h \
    display/LeapMotion.h

SOURCES += \
    solver/Segment.cpp \
    solver/Obstacles.cpp \
    solver/FluidSolver2D.cpp \
    solver/FloatMatrix2D.cpp \
    display/SimplePrint.cpp \
    display/Print.cpp \
    display/ParticlesPrint.cpp \
    display/main.cpp \
    display/GUI.cpp \
    display/Dialog.cpp \
    display/CurvePrint.cpp \
    display/ColorPrint.cpp \
    config.cpp \
    display/LeapMotion.cpp

FORMS += \
    display/Dialog.ui

OTHER_FILES += \
    display/Leap.i

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -lLeap
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -lLeapd
else:unix: LIBS += -L$$PWD/ -lLeap

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/
