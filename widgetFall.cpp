#include "widgetView.h"
#include "ui_widgetView.h"

#include <QPainter>
#include <QImage>


extern QImage *image;


WidgetView::WidgetView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetView)
{
    ui->setupUi(this);
}

WidgetView::~WidgetView()
{
    delete ui;
}


void WidgetView::paintEvent (QPaintEvent* event)
{
    QPoint point(0,0);
    //point.setX(0);
    //point.setY(0);

    QPainter painter(this);
    painter.drawImage(point,*image);
}
