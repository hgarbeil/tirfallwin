#include "widgetviewimage.h"
#include "ui_widgetviewimage.h"

#include <QPainter>
#include <QImage>
#include <QMouseEvent>

#include <acqINO.h>

extern unsigned short m_nPhotonFrameBuffer[];
extern int MouseX, MouseY;
extern long MouseIntensity;


extern QImage *QImageCamera;


WidgetViewImage::WidgetViewImage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetViewImage)
{
    ui->setupUi(this);
}

WidgetViewImage::~WidgetViewImage()
{
    delete ui;
}


void WidgetViewImage::paintEvent (QPaintEvent *event)
{
    QPoint point(0,0);
    //point.setX(0);
    //point.setY(0);

    QPainter painter(this);
    painter.drawImage(point,*QImageCamera);
}

void WidgetViewImage::mouseMoveEvent (QMouseEvent *event)
{
    if( (event->x()>=0) && (event->y()>=0) ) {
        MouseX = event->x();
        MouseY = event->y();

        MouseIntensity = m_nPhotonFrameBuffer[MouseY*IMAGEWIDTH+MouseX];
    }
}

void WidgetViewImage::mousePressEvent (QMouseEvent *event)
{
    if( (event->x()>=0) && (event->y()>=0) ) {
        MouseX = event->x();
        MouseY = event->y();

        MouseIntensity = m_nPhotonFrameBuffer[MouseY*IMAGEWIDTH+MouseX];
    }
}
