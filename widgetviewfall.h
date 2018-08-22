#ifndef WIDGETVIEWFALL_H
#define WIDGETVIEWFALL_H

#include <QWidget>

namespace Ui {
class WidgetViewFall;
}

class WidgetViewFall : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetViewFall(QWidget *parent = 0);
    ~WidgetViewFall();
    
    void paintEvent (QPaintEvent* event);

private:
    Ui::WidgetViewFall *ui;
};

#endif // WIDGETVIEWFALL_H
