#include "widgetviewfall.h"
#include "ui_widgetviewfall.h"

#include <QPainter>
#include <QImage>


extern QImage *QImageFall;


WidgetViewFall::WidgetViewFall(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetViewFall)
{
    ui->setupUi(this);
}

WidgetViewFall::~WidgetViewFall()
{
    delete ui;
}


void WidgetViewFall::paintEvent (QPaintEvent* event)
{
    QPoint point(0,0);
    //point.setX(0);
    //point.setY(0);

    QPainter painter(this);
    painter.drawImage(point,*QImageFall);
}
