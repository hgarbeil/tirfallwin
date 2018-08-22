#ifndef WIDGETVIEWIMAGE_H
#define WIDGETVIEWIMAGE_H

#include <QWidget>

namespace Ui {
class WidgetViewImage;
}

class WidgetViewImage : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetViewImage(QWidget *parent = 0);
    ~WidgetViewImage();
    
    void paintEvent (QPaintEvent* event);
    void mouseMoveEvent (QMouseEvent* event);
    void mousePressEvent (QMouseEvent* event);

private:
    Ui::WidgetViewImage *ui;
};

#endif // WIDGETVIEWIMAGE_H
