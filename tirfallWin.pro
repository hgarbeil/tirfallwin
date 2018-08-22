#-------------------------------------------------
#
# Project created by QtCreator 2016-07-13T05:56:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = tirfallWinIno
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    stdafx.cpp \
    acqDummy320x240.cpp \
    widgetviewfall.cpp \
    widgetviewimage.cpp \
    widgetviewplotfft.cpp \
    widgetviewplotpixel.cpp \
    smc100.cpp \
    bbsr800.cpp \
    calibrator.cpp \
    tirthreads.cpp \
    fileFctns.cpp \
    threads.cpp \
    dlgZoom.cpp \
    acqDummy640x512.cpp \
    acqphoton.cpp \
    COMDevice.cpp \
    bbsr800gpib.cpp \
    zfft.cpp \
    bbdialog.cpp \
    tprocess.cpp \
    mainprocess.cpp \
    suchi_offsets.cpp \
    wavecal.cpp \
    processresultsdlg.cpp \
    acqINO.cpp \
    cmigitsasc.cpp \
    InoSettings.cpp \
    support/myprofileplot.cpp \
    support/qcustomplot.cpp \
    support/plotscaledialog.cpp \
    support/suchi_utils.cpp \
    support/suchiarray.cpp \
    support/suchifile.cpp \
    support/nrutil.c \
    support/svbksb_d.c \
    support/svdcmp_d.c \
    support/svdfit_d.c \
    support/poly.cpp \
    support/geneprocess.cpp \
    support/imwidget.cpp \
    support/dispadjustdialog.cpp

HEADERS  += mainwindow.h \
    stdafx.h \
    global.h \
    defines.h \
    acqDummy320x240.h \
    acqDummy640x512.h \
    acqphoton.h \
    threads.h \
    widgetviewfall.h \
    widgetviewimage.h \
    widgetviewplotfft.h \
    widgetviewplotpixel.h \
    bbsr800.h \
    calibrator.h \
    configCosmos.h \
    cosmos-errno.h \
    dlgZoom.h \
    fileFctns.h \
    sliplib.h \
    smc100.h \
    tirthreads.h \
    COMDevice.h \
    bbsr800gpib.h \
    cmplex.h \
    bbdialog.h \
    tprocess.h \
    mainprocess.h \
    suchi_offsets.h \
    wavecal.h \
    processresultsdlg.h \
    acqINO.h \
    cmigitsasc.h \
    InoSettings.h \
    support/myprofileplot.h \
    support/qcustomplot.h \
    support/nr.h \
    support/nrutil.h \
    support/plotscaledialog.h \
    support/suchi_utils.h \
    support/suchiarray.h \
    support/suchifile.h \
    support/geneprocess.h \
    support/imwidget.h \
    support/dispadjustdialog.h

FORMS    += mainwindow.ui \
    widgetviewfall.ui \
    widgetviewimage.ui \
    widgetviewplotfft.ui \
    widgetviewplotpixel.ui \
    dlgZoom.ui \
    bbdialog.ui \
    processresultsdlg.ui \
    InoSettings.ui \
    support/plotscaledialog.ui \
    support/dispadjustdialog.ui

DEFINES  += _AFXDLL

#win32{
INCLUDEPATH += support
#INCLUDEPATH += \source\fftw-3.3.4-dll64
INCLUDEPATH += "C:\fftw-3.3.5"
##ifdef PLEORA
#INCLUDEPATH += "C:\Program Files (x86)\Pleora Technologies Inc\eBUS SDK\Includes"
##INCLUDEPATH += "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\atlmfc\include"
##INCLUDEPATH += "C:\Program Files (x86)\Microsoft Visual Studio 8\VC\PlatformSDK\Include"
#INCLUDEPATH += "C:\Program Files (x86)\Indigo PT1000-IDG\Includes"
##endif
INCLUDEPATH += "C:\Program Files (x86)\Windows Kits\10\Include\10.0.10586.0\ucrt"
#INCLUDEPATH += "C:\OpenCV-2.2.0\include"
#LIBS += -L"C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86" \
#    -lWinMM

LIBS += -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.10586.0\ucrt\x86" -lWinMM

LIBS += "C:\fftw-3.3.5\libfftw3-3.lib"


LIBS += -L"C:\Program Files (x86)\Pleora Technologies Inc\eBUS SDK\Libraries" \
    -lPvDevice

#LIBS += -L"C:\Program Files (x86)\Indigo PT1000-IDG\Libraries"    \
#-lCyCamLib      \
#-lCy UtilsLib    \
#-lCyComLib      \
#-lCyImgLib

LIBS += -L"C:\source\Microxcam_SDK\SDK\lib"    \
    -lMICROXCAM-384I_CAPI

#LIBS += -LC:/OpenCV2.0/lib  -lcv200  -lhighgui200
#LIBS += -lcxcore200 -lcvaux200
#LIBS += -LC:/OpenCV-2.2.0/lib -lopencv_core220d  -lopencv_highgui220d -lopencv_flann220d -lopencv_legacy220d
#LIBS += cxts200.lib      ml200.lib

#OPENCV_DIR =C:/OpenCV2.0

#LIBS += -LC:\OpenCV2.0\lib -lcv200 opencv_highgui220. opencv_legacy220d.dll
#LIBS += opencv_features2d220d.dll
#}
