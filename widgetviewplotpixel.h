#ifndef WIDGETVIEWPLOTPIXEL_H
#define WIDGETVIEWPLOTPIXEL_H

#include <QWidget>

namespace Ui {
class WidgetViewPlotPixel;
}

class WidgetViewPlotPixel : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetViewPlotPixel(QWidget *parent = 0);
    ~WidgetViewPlotPixel();
    
    void paintEvent (QPaintEvent* event);

private:
    Ui::WidgetViewPlotPixel *ui;
};

#endif // WIDGETVIEWPLOTPIXEL_H
