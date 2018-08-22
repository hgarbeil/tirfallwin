#ifdef _WIN32
#include "afxwin.h"
#endif
//:O#include "cssl_lib.h"

#include "widgetviewplotpixel.h"
#include "ui_widgetviewplotpixel.h"

#include <QPainter>
#include <QImage>
#include <string.h>
#include <memory.h>
#include "acqmc320.h"
#include "acqtau.h"
#include "acqpv640.h"
#include "acqDummy320x240.h"
#include "acqDummy640x512.h"
#include "acqphoton.h"
#include "acqino.h"
#include "COMDevice.h"
#include "global.h"
#include "defines.h"

#ifdef __unix__
//#include <unistd.h> //for usleep
#include <semaphore.h>
#endif


extern unsigned short m_nPhotonFrameBuffer[];
short int NumPoints;

WidgetViewPlotPixel::WidgetViewPlotPixel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetViewPlotPixel)
{
    ui->setupUi(this);

    NumPoints=0;

/*    for(int i=0 ; i<128 ; i++) {
        PixelData[i] = i;
    }*/
}

WidgetViewPlotPixel::~WidgetViewPlotPixel()
{
    delete ui;
}


void WidgetViewPlotPixel::paintEvent (QPaintEvent* event)
{
int i;
int x1, y1, x2, y2;
//int MinVal, MaxVal;
int AutoMinVal, AutoMaxVal;
//    QPoint point(0,0);
//    //point.setX(0);
//    //point.setY(0);

    QPainter painter(this);
    painter.setPen(Qt::blue);
    //painter.drawRect(10,10,160,100);



    //Keep track of how many points are in the pixel plot.
    NumPoints++;

    if( NumPoints > 256 ) {
        for(i=0 ; i<256 ; i++) {
            PixelData[i] = PixelData[i+1];
        }
        //memmove(&PixelData[0], &PixelData[1], (NumPoints-1)*sizeof(short int));
        NumPoints=256;
    }


/*
    // *** Get the pixel value for the requested pixel number.
    //Check for invalid input values.
    YPosition = GetDlgItemInt(IDC_YPOSITION, &ReturnValue, FALSE);
    if( YPosition>IMAGEHEIGHT-1 || YPosition<0 ) {
        YPosition = IMAGEHEIGHT/2;
        sprintf(buffer, "%d" , YPosition);
        SetDlgItemText(IDC_YPOSITION, buffer);
    }
    XPosition = GetDlgItemInt(IDC_XPOSITION, &ReturnValue, FALSE);
    if( XPosition>IMAGEWIDTH-1 || XPosition<0 ) {
        XPosition = IMAGEWIDTH/2;
        sprintf(buffer, "%d" , YPosition);
        SetDlgItemText(IDC_XPOSITION, buffer);
    }
    m_nPixelPlotBuffer[NumPoints-1] = m_nPhotonFrameBuffer[XPosition+IMAGEWIDTH*YPosition];*/

    //PixelData[NumPoints-1] = m_nPhotonFrameBuffer[160+320*20];
    PixelData[NumPoints-1] = m_nPhotonFrameBuffer[160+AcqImgWidth*2];


            //Get Min and Max of pixplot
    AutoMinVal = 65535;
    AutoMaxVal = 0;
    for (i=0 ; i<NumPoints ; i++) {
        //if( AutoMinVal > m_nPhotonFrameBuffer[i] ) AutoMinVal = m_nPhotonFrameBuffer[i];
        if( AutoMaxVal < PixelData[i] ) {
            AutoMaxVal = PixelData[i];
        }
        if( (AutoMinVal > PixelData[i]) /*&& (PixelData[i]!=0)*/ ) {
            AutoMinVal = PixelData[i];
        }

        //if(AutoMinVal == 65535) {
        //    AutoMinVal = 0;
        //}
    }
    AutoMaxVal++;
    MaxValPixel = AutoMaxVal;
    MinValPixel = AutoMinVal;


    // Testing
//    MaxValPixel +=NumPoints;


    double scale2 = (float)(256.0/(double)(MaxValPixel-MinValPixel));
    for(i=0 ; i<NumPoints ; i++) {

        x1 = i*2;
        y1 = 250 -(int)( (PixelData[i]-(float)MinValPixel) * scale2 + 0.5 );
        x2 = (i+1) * 2;
        y2 = 250 -(int)( (PixelData[i+1]-(float)MinValPixel) * scale2 + 0.5 );


//        // Testing
//        y1+=i; y2+=i;


        painter.drawLine(x1, y1, x2, y2);
    }


/*
    for(i=0 ; i<NumPoints ; i++) {
        nextpoint = Bottom2-(int)((m_nPixelPlotBuffer[i]-(float)MinVal) * scale2 + 0.5) - 2;
        if( nextpoint<Top2 ) nextpoint = Top2;
        if( nextpoint>Bottom2 ) nextpoint = Bottom2-2;
        if(i == 0 ) {
            MoveToEx(hdcPaint, 0, 250, &point);
            LineTo(hdcPaint, 0, nextpoint);
        } else {
            MoveToEx(hdcPaint, i-1, lastpoint, &point);
            LineTo(hdcPaint, i, nextpoint);
        }
        lastpoint = nextpoint;
    }*/
}
