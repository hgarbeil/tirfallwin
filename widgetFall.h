#ifndef WIDGETVIEW_H
#define WIDGETVIEW_H

#include <QWidget>

namespace Ui {
class WidgetView;
}

class WidgetView : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetView(QWidget *parent = 0);
    ~WidgetView();
    
    void paintEvent (QPaintEvent* event);

private:
    Ui::WidgetView *ui;
};

#endif // WIDGETVIEW_H
