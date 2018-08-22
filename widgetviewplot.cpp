#include "widgetviewplot.h"
#include "ui_widgetviewplot.h"

#include <QPainter>
#include <QImage>


extern QImage *QImagePlot;
extern unsigned int *PlotData;

WidgetViewPlot::WidgetViewPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetViewPlot)
{
    ui->setupUi(this);
}

WidgetViewPlot::~WidgetViewPlot()
{
    delete ui;
}


void WidgetViewPlot::paintEvent (QPaintEvent* event)
{
//    QPoint point(0,0);
//    //point.setX(0);
//    //point.setY(0);

    QPainter painter(this);

    painter.setPen(Qt::blue);
    painter.drawRect(1,1,100,100);
}
