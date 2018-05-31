#-------------------------------------------------
#
# Project created by QtCreator 2018-02-22T23:40:55
#
#-------------------------------------------------

QT       += core gui
QT += opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = CBReader
TEMPLATE = app

#SUBDIRS = src/unrar src
#src.depends = src/unrar

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        CbrMainWindow.cpp \
        graphicsview.cpp \
        ZipReader.cpp \
    CbrSlider.cpp \
    CbrFolderTree.cpp \
    CbrFileTree.cpp

HEADERS += \
        CbrMainWindow.h \
        graphicsview.hpp \
        ZipReader.h \
        asmOpenCV.h \
    CbrSlider.h \
    CbrFileTree.h \
    CbrFolderTree.h

FORMS += \
        CbrMainWindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/./release/ -lquazipd
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/./debug/ -lquazipd

INCLUDEPATH += $$PWD/exe
INCLUDEPATH += $$PWD/debug
DEPENDPATH += $$PWD/debug

INCLUDEPATH += C:/Software/opencv/build/include
INCLUDEPATH += C:/Software/opencv/build/include/opencv
INCLUDEPATH += C:/Software/opencv/build/include/opencv2
# below maybe needed
#LIBS += C:\Software\opencv\build\x64\vc14\lib\opencv_world331.lib
#LIBS += C:\Software\opencv\build\x64\vc14\lib\opencv_world331d.lib

#CONFIG(debug,debug|release) {
#LIBS += -LD:/tmp/opencv/lib \
#    -llibopencv_core331.dll \
#    -llibopencv_imgproc331.dll \
##    -libopencv_imgproc331 \
#    -llibopencv_features2d331.dll \
#    -llibopencv_imgcodecs331.dll
##    -libopencv_calib3d331 \
##    -llibopencv_highgui331.dll
#    -llibopencv_calib3d331.dll
#    -llibopencv_videoio331.dll
#}
#else {
#LIBS += -LD:/tmp/opencv/lib/ \
##    -llibopencv_core331.dll \
#    -llibopencv_imgproc331.dll \
##    -libopencv_imgproc331 \
#    -llibopencv_features2d331.dll \
#    -llibopencv_imgcodecs331.dll
##    -libopencv_calib3d331 \
##    -llibopencv_highgui331.dll
#    -llibopencv_calib3d331.dll
#    -llibopencv_videoio331.dll
#}


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/opencvlib/ -llibopencv_imgproc331.dll
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/opencvlib/ -llibopencv_imgproc331.dll

#INCLUDEPATH += $$PWD/opencvlib
#DEPENDPATH += $$PWD/opencvlib

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/opencvlib/ -llibopencv_highgui331.dll
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/opencvlib/ -llibopencv_highgui331.dll

#INCLUDEPATH += $$PWD/opencvlib
#DEPENDPATH += $$PWD/opencvlib

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/opencvlib/ -llibopencv_features2d331.dll
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/opencvlib/ -llibopencv_features2d331.dll

#INCLUDEPATH += $$PWD/opencvlib
#DEPENDPATH += $$PWD/opencvlib

RESOURCES += \
    cbr.qrc

