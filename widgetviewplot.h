#ifndef WIDGETVIEWPLOT_H
#define WIDGETVIEWPLOT_H

#include <QWidget>

namespace Ui {
class WidgetViewPlot;
}

class WidgetViewPlot : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetViewPlot(QWidget *parent = 0);
    ~WidgetViewPlot();
    
    void paintEvent (QPaintEvent* event);

private:
    Ui::WidgetViewPlot *ui;
};

#endif // WIDGETVIEWPLOT_H
