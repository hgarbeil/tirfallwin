#ifndef WIDGETVIEWPLOTFFT_H
#define WIDGETVIEWPLOTFFT_H

#include <QWidget>

namespace Ui {
class WidgetViewPlotFft;
}

class WidgetViewPlotFft : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetViewPlotFft(QWidget *parent = 0);
    ~WidgetViewPlotFft();
    
    void paintEvent (QPaintEvent* event);

private:
    Ui::WidgetViewPlotFft *ui;
};

#endif // WIDGETVIEWPLOTFFT_H
