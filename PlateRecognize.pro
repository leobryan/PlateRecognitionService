#-------------------------------------------------
#
# Project created by QtCreator 2019-06-21T15:54:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlateRecognize
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        charsegment_v3.cpp \
        main.cpp \
        matswitch.cpp \
        platecategory_svm.cpp \
        platechar_svm.cpp \
        platelocator_v3.cpp \
        platerecognition_v3.cpp \
        service.cpp

HEADERS += \
        charsegment_v3.h \
        matswitch.h \
        platecategory.h \
        platecategory_svm.h \
        platechar_svm.h \
        platelocator_v3.h \
        platerecognition_v3.h \
        service.h

FORMS += \
        service.ui

INCLUDEPATH += /usr/local/include
INCLUDEPATH += /usr/local/include/opencv4

LIBS += -L/usr/local/lib/ -lopencv_core.4.1.0 \
                          -lopencv_imgproc.4.1.0 \
                          -lopencv_highgui.4.1.0 \
                          -lopencv_calib3d.4.1.0 \
                          -lopencv_dnn.4.1.0 \
                          -lopencv_features2d.4.1.0 \
                          -lopencv_flann.4.1.0 \
                          -lopencv_imgcodecs.4.1.0 \
                          -lopencv_ml.4.1.0 \
                          -lopencv_objdetect.4.1.0


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
