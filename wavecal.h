#ifndef WAVECAL_H
#define WAVECAL_H

#include <QObject>
#include <QList>
//#include "Instrument_config.h"

/**
 * @class Wavecal
 * @brief main computational class for Tircis wavelength calibration
 * @ingroup wavecal_suchi
 * @author Harold Garbeil
 * @copyright HIGP/SOEST/UHM
 *
 */
class Wavecal : public QObject
{
    Q_OBJECT
public:
    explicit Wavecal(QObject *parent = 0);
    ~Wavecal() ;
    void setWaveFiles (QList<QString>, QList<float>) ;
    /***
     * function to set the start, stop and flag to extract
     * profile and reconstruct, pixels extracted from each
     * column based upon starting and end pixels and then
     * mirrored based upon the left or right flag
     */
    void setStartStop (int start, int stop, bool leftFlag) ;
    void setOutputFile (QString str) ;
    /***
     * fcn to output results to text file or not
     */
    void setOutputFlag (bool ofFlag) ;
    void setProcessLoc (int) ;
    void loadProfiles() ;
    void process () ;
    /***
     * Called when doing the column calibration
     * Uses 3 columns centered upon the specified one to come up with an average column
     * then used for fitting
     */
    void processFull() ;
    /***
     * processFullArray
     * Called when doing the Full Calibration
     * uses the flattened array, then averaged across the entire detector to come up
     * with an average column. This is used as input for the wavecal fitting.
     */
    void processFullArray() ;

    /***
     * reflection on the start of the segment or the end
     */
    bool leftFlag ;

    bool outfileFlag ;
    double *pixvals, *wavenum, *wavecoefs, *yf ;
    float *profiles, *prof0, *prof1, *firstFile ;
    unsigned short  *wavedata ;
    int procLoc ;
    long npixTot ;
    QString outfile ;
    QList<QString> filenames ;
    QList<float> wavelens ;
    int nfiles, start, stop, ns, nl ;
    
signals:
    
public slots:
    
};

#endif // WAVECAL_H
