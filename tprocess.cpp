#include "tprocess.h"
#include <QDebug>

int ns_inst ;
int nl_inst ;

TProcess::TProcess(QObject *parent) : QObject(parent)
{
    hotFile = "" ;
    hotTemp = 0 ;
    coldFile = "" ;
    coldTemp = 0 ;
    xoff = 0. ;
    yoff = 1. ;

}


//The setBBTemp method is used to store temporary variables
// when this is used, the filename has not yet been generated
// but will be assigned by DoAcquireManualBB and at that time
// the bbTemp is assigned to hot or cold bb via setBlackBody
void TProcess::setBBTemp (float temp, bool flag) {
    hotFlag = flag ;
    bbTemp = temp ;

}


// The hot and cold black bodies are assigned along with their temperatures
// using this method.
void TProcess::setBlackBody (QString fname, float temp, bool hotFlag){
    if (hotFlag){
        hotFile = fname ;
        hotTemp = temp ;
        
    }
    else {
        coldFile = fname ;
        coldTemp = temp ;
    }
    qDebug () << "Hot File : " << hotFile ;
    qDebug () << "hot temp : " << hotTemp ;
    qDebug () << "Cold File : " << coldFile ;
    qDebug () << "cold temp : " << coldTemp ;

    
}


// Variables set from the tirprocess.ini file read
void TProcess::setConfigParams(char *wf, char *sf, int samps, float xf, float yf) {
    strcpy (wavefile, wf) ;
    strcpy (segfile, sf) ;
    nsampsInterf = samps ;
    xoff = xf ;
    yoff = yf ;
}
