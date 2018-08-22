/** 
 * mainprocess
 **/
#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include <QThread>

#include "wavecal.h"
#include "geneprocess.h"
#include "suchi_offsets.h"
#include "suchifile.h"
#include "processresultsdlg.h"
#include "tprocess.h"
//#include "Instrument_config.h"
/**
 * MainProcess class
 * @brief Main processing class for Tircis_Process software.
 * @section DESCRIPTION
 * This class contains the members and methods to take the Tircis 
 * scan file from raw image to a radiance file, an emissivity file,
 * and a temperature file.
 */
class MainProcess : public QThread
{
    Q_OBJECT
public:
	/**
	 * Constructor called by main.cpp, initializes and creates and
	 * displays the GUI.
	 */
    explicit MainProcess(QObject *parent = 0);
    Wavecal *wc ;
    GeneProcess *gp ;
    TProcess *TProc ;
    suchi_offsets *so ;
    SuchiFile *sf ;
    QList<float> temps ;
    QList<QString> waveNames ;
    QString workdir, bbHotFile, bbColdFile, scanFile ;
    QString segFile, waveFile ;
    float tHot, tCold ;
    float xoffAvg, yoffAvg, *xoffArr, *yoffArr ;
    float startBin, stopBin ;
    bool lFlag, constFlag ;
    void readProcessFile (QString filename) ;
    void readProcessFileMany (QString filename) ;
    void parseTProcess () ;
    void startProcess () ;
    void setTProcess (TProcess *tp) ;
    void setSegWaveFile (char *sf, char *wf) ;
    void setBlackBodies (char *bbcoldFile, char *bbhotFile, float TCold, float THot) ;
    void setBlackBodies (QString bbcoldFile, QString bbhotFile, float TCold, float THot) ;
    void setScanFile (QString sf) ;
    void run () ;
    void HideMainProc();
    void ShowMainProc();
    ProcessResultsDlg *prdlg ;
signals:
    void updateProgress (int state) ;

public slots:

};

#endif // MAINPROCESS_H
