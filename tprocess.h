#ifndef TPROCESS_H
#define TPROCESS_H

#include <QObject>

class TProcess : public QObject
{
    Q_OBJECT
public:
    explicit TProcess(QObject *parent = 0);
    QString hotFile, coldFile, scanFile ;
    char wavefile[240], segfile [240] ;
    float hotTemp, coldTemp, bbTemp, xoff, yoff ;
    int nsampsInterf ;
    bool hotFlag ;
    void setBBTemp (float temp, bool flag) ;
    void setBlackBody (QString fname, float temp, bool hotFlag) ;
    void setConfigParams (char *wavefile, char *segfile, int samps, float ave_x, float ave_y) ;

signals:

public slots:
};

#endif // TPROCESS_H
