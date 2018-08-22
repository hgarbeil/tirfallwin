#ifdef _WIN32
#include "afxwin.h"
#endif
//:O#include "cssl_lib.h"
#include "widgetviewplotfft.h"
#include "ui_widgetviewplotfft.h"

#include <QPainter>
#include <QImage>
#include "acqDummy320x240.h"
#include "acqDummy640x512.h"
#include "acqphoton.h"
#include "acqino.h"
#include "COMDevice.h"
#include "global.h"
#include "defines.h"

#ifdef __unix__
#include "acqmc320.h"
#include "acqtau.h"
#include "acqpv640.h"
#include <semaphore.h>
#endif


//extern unsigned short m_nPhotonFrameBuffer[];


WidgetViewPlotFft::WidgetViewPlotFft(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetViewPlotFft)
{
    ui->setupUi(this);
/*
    for(int i=0 ; i<128 ; i++) {
        FftData[i] = 128-i;
    }*/
}

WidgetViewPlotFft::~WidgetViewPlotFft()
{
    delete ui;
}


void WidgetViewPlotFft::paintEvent (QPaintEvent* event)
{
int i;
int x1, y1, x2, y2;
//int MinVal, MaxVal;
int AutoMinVal, AutoMaxVal;
//    QPoint point(0,0);
//    //point.setX(0);
//    //point.setY(0);
short int NumPoints=255;


    QPainter painter(this);
    painter.setPen(Qt::blue);
    //painter.drawRect(10,10,100,160);

//return;
/*    for(i=0 ; i<128 ; i++) {
        //FftData[i] = 128-i;
        FftData[0] = 0;
        //FftData[i] = (double)m_nPhotonFrameBuffer[i+100+320*2];
    }
*/
/*
    //Get Min and Max of image
    AutoMinVal = 65535;
    AutoMaxVal = 0;
    for (i=15 ; i<128 ; i++) {
        if( AutoMaxVal < (int)(FFT[i] + 0.5) ) {
            AutoMaxVal = (int)(FFT[i] + 0.5);
        }
    }
    AutoMaxVal++;
    MinVal = 0;
*/

    //Get Min and Max of pixplot
    AutoMinVal = 65535;
    AutoMaxVal = 0;
    for (i=0 ; i<NumPoints ; i++) {
        //if( AutoMinVal > m_nPhotonFrameBuffer[i] ) AutoMinVal = m_nPhotonFrameBuffer[i];
        if( AutoMaxVal < FftData[i] ) {
            AutoMaxVal = FftData[i];
        }
        if( (AutoMinVal > FftData[i]) && (FftData[i]!=0) ) {
            AutoMinVal = FftData[i];
        }

        if(AutoMinVal == 65535) {
            AutoMinVal = 0;
        }
    }
    AutoMaxVal++;
    MaxValFft = AutoMaxVal;
    MinValFft = AutoMinVal;




    double scale2 = (float)(256.0/(double)(MaxValFft-MinValFft));
    for(int i=0 ; i<127 ; i++) {
        x1 = (int)(i*2.5+.5);
        y1 = FftData[i];
        x2 = (int)(i*2.5+2.5+.5);
        y2 = FftData[i+1];
        painter.drawLine(x1, y1, x2, y2);
    }

}
