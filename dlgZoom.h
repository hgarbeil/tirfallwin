#ifndef DLGZOOM_H
#define DLGZOOM_H

#include <QDialog>
#include <QTimer>

namespace Ui {
    class dlgZoom;
}

class dlgZoom : public QDialog
{
    Q_OBJECT
    
public:
    explicit dlgZoom(QWidget *parent = 0);
    ~dlgZoom();
    
    void paintEvent (QPaintEvent* event);

    uchar *ImgCamera3X;
    QImage *QImageCamera3X;
    QTimer TimerVid;

private:
    Ui::dlgZoom *ui;

private slots:
    //:O
    void onTimer(void);

};

#endif // DLGZOOM_H
