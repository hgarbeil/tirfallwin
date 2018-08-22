#include <QPainter>
#include <QImage>
#include "defines.h"

#include "dlgZoom.h"
#include "ui_dlgZoom.h"


#define uInt32 unsigned int


extern uInt32 AcqImgWidth;
extern uInt32 AcqImgHeight;
extern uInt32 AcqImgSize;


#define ZOOM 3
extern uchar *ImgCamera;


dlgZoom::dlgZoom(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgZoom)
{
    ui->setupUi(this);
    setWindowTitle(tr("Magnify Image"));

    ImgCamera3X = new uchar[3L*AcqImgSize*ZOOM*ZOOM];
    if (ImgCamera3X==NULL) return;
    QImageCamera3X = new QImage(ImgCamera3X, AcqImgWidth/2*ZOOM, AcqImgHeight/2*ZOOM, AcqImgWidth/2*3*ZOOM, QImage::Format_RGB888);
    if (QImageCamera3X==NULL) return;

    connect(&TimerVid, SIGNAL(timeout()), this, SLOT(onTimer()));   // Black Body timer

    // *** Start timer
    TimerVid.start(50);
}

dlgZoom::~dlgZoom()
{
    if(QImageCamera3X) delete QImageCamera3X;
    if(ImgCamera3X) delete ImgCamera3X;
    delete ui;
}

void dlgZoom::onTimer(void)
{
    repaint();
}


void dlgZoom::paintEvent (QPaintEvent* event){
    //int i, j;
    uchar *pImgCamera;
    uchar *pImgCamera3X;
    uchar *StartOfLine;
    QPoint point(0,0);

    if (QImageCamera3X==NULL) return;
    if (ImgCamera3X==NULL) return;


    pImgCamera = ImgCamera;
    pImgCamera3X = ImgCamera3X;

    for(unsigned i=0 ; i<AcqImgHeight ; i++) {
        StartOfLine = pImgCamera3X;

        // Zoom width of original QImage 3 times
        for(unsigned j=0 ; j<AcqImgWidth ; j++) {
            memset(pImgCamera3X, *pImgCamera, 3*ZOOM);
            pImgCamera3X+=(3*ZOOM);;
            pImgCamera+=3;
        }

        // Copy the identical next two lines (assuming zoom=3)
        for(unsigned j=0 ; j<(ZOOM-1) ; j++) {
            memcpy(pImgCamera3X, StartOfLine, AcqImgWidth*3*ZOOM);
            pImgCamera3X+=AcqImgWidth*3*ZOOM;
        }
    }

    QPainter painter(this);
    painter.drawImage(point,*QImageCamera3X);
}
