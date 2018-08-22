#ifndef SUCHI_OFFSETS_H
#define SUCHI_OFFSETS_H

#include <QObject>

class suchi_offsets : public QObject
{
    Q_OBJECT
public:
    explicit suchi_offsets(QObject *parent = 0);
    void defineWindow (int x0, int y0, int nx, int ny, int samps, int lines) ;
    void calcOffsets (unsigned short *idat, float *xoff, float *yoff, int startFrame, int numFrames) ;
    void setConstantFlag (bool b) ;
    void resampleArray (unsigned short *indat, float xoff, float yoff, int numFrames) ;
    void resampleArray (unsigned short *indat, float *xoff, float *yoff, int numFrames) ;
    void setOutprefix (QString) ;
    float *outarr ;
    float ave_x, ave_y, *xoffArr, *yoffArr ;
    int nbands, startx, starty, nswin, nlwin, nsamps, nlines ;
    long totFrames ;
    bool constOffsetFlag ;
    QString outPrefix ;
signals:
    
public slots:
    
};

#endif // SUCHI_OFFSETS_H
