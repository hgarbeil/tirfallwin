#include "mainprocess.h"
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QMessageBox>

//extern int ns_inst, nl_inst ;

MainProcess::MainProcess(QObject *parent) :
    QThread(parent)
{
    workdir = "" ;
    bbHotFile = "" ;
    bbColdFile = "" ;
    scanFile = "" ;
    startBin = 30 ;
    stopBin = 239 ;
    lFlag = false ;

    waveNames.clear() ;
    temps.clear() ;
    tHot = 50. ;
    tCold = 20. ;
    prdlg = new ProcessResultsDlg () ;
    gp = new GeneProcess () ;
    so = new suchi_offsets () ;
    wc = new Wavecal () ;

    prdlg->setWavecal (wc) ;
    prdlg->setGeneProc(gp) ;


    prdlg->setSO (so) ;

    prdlg->show () ;

    connect (this, SIGNAL(updateProgress(int)), prdlg,SLOT (updateProgress(int))) ;
    TProc = NULL ;
    
}

void MainProcess::setBlackBodies (char *bbcoldFile, char *bbhotFile, float TCold, float THot){

    tCold = TCold ;
    tHot = THot ;
    this->bbColdFile = bbcoldFile ;
    this->bbHotFile = bbhotFile ;
}

void MainProcess::setBlackBodies (QString bbcoldFile, QString bbhotFile, float TCold, float THot){

    tCold = TCold ;
    tHot = THot ;
    this->bbColdFile = bbcoldFile ;
    this->bbHotFile = bbhotFile ;
}

void MainProcess::setScanFile (QString sf){
    scanFile = sf ;
}

void MainProcess::setTProcess (TProcess *t){
    TProc = t ;
}

void MainProcess::parseTProcess () {
    //setBlackBodies(TProc->);
    this->setBlackBodies (TProc->coldFile, TProc->hotFile,
        TProc->coldTemp, TProc->hotTemp) ;
    this->setScanFile (TProc->scanFile) ;
    this->setSegWaveFile (TProc->segfile, TProc->wavefile) ;
    xoffAvg = TProc->xoff ;
    yoffAvg = TProc->yoff ;

}

void MainProcess::startProcess () {
    int i ;
    QString outpref ;
    gp->readSegmentFile (this->segFile) ;
    gp->nptsSeg = TProc->nsampsInterf ;
    gp->readWaveFile (this->waveFile ) ;
    for (i=0; i<80; i++) {
        wc->wavelens.append(gp->wavelen[i]) ;

    }
    gp->setScanFile (scanFile) ;
    outpref = scanFile.left (scanFile.lastIndexOf('.')) ;
    so->setOutprefix (outpref) ;
    gp->setOutpref (outpref) ;


}

void MainProcess::setSegWaveFile (char *sf, char *wf) {
    QString temp ;
    temp = sf ;
    segFile = temp.trimmed() ;
    temp = wf ;
    waveFile = temp.trimmed() ;
    qDebug () << "Segfile is : " << segFile  ;
    qDebug () << "Wavefile is : "<< waveFile ;
}

void MainProcess::readProcessFile (QString qstr)
{
    QString str ;
    QStringList strList ;
    int i, val, leftFlag, nWaveFiles, npts ;
    float seg_yintcpt, seg_slope ;



    QFile qf (qstr) ;
    qf.open (QIODevice::ReadOnly) ;
    if (!qf.exists()){
        QMessageBox::information(0, "File open issue", "File not found" ) ;
        qf.close() ;
        return ;

    }
    // get workdir
    QTextStream qts (&qf) ;
    str = qts.readLine() ;
    QDir qd (str) ;
    if (qd.exists()) workdir = str ;
    
    // get the profile reconstructor
    str = qts.readLine () ;
    
    strList = str.split (" ") ;
    nWaveFiles = strList[0].toInt() ;
    seg_yintcpt = strList[1].toFloat() ;
    seg_slope = strList[2].toFloat() ;
    npts = strList[3].toInt() ;
    leftFlag = strList[4].toInt() ;
    if (leftFlag ==1) lFlag = true ;
    else lFlag = false ;

    // offsets, check if const offset
    val = strList[5].toInt() ;
    if (val ==1) {
            constFlag = true ;
            xoffAvg = strList[6].toFloat() ;
            yoffAvg = strList[7].toFloat() ;
    } else constFlag = false ;

    //gp->setSegParamsNum(startBin, stopBin, lFlag);
    gp->setSegParamsNew (seg_yintcpt, seg_slope, npts, lFlag) ;
    //wc->setStartNum(startBin, stopBin, lFlag);
    if (nWaveFiles !=0) {
        waveNames.clear() ;
        temps.clear() ;
        for (i=0; i<nWaveFiles; i++) {
            str = qts.readLine() ;
            strList = str.split(",") ;
            waveNames.append(strList[0]) ;
            temps.append (strList[1].toFloat()) ;
        }
        wc->setWaveFiles (waveNames, temps) ;
        wc->processFull () ;

    } else {
        str = qts.readLine() ;
        gp->readWaveFile (str) ;
        for (i=0; i<80; i++) {
            wc->wavelens.append(gp->wavelen[i]) ;
        }
    }
    // get segment file
    str = qts.readLine () ;
    qDebug () << "reading segment file " << str ;
    gp->readSegmentFile (str) ;


    // get blackbodies
    str = qts.readLine() ;
    strList = str.split(",");
    bbHotFile = QString ("%1/%2").arg(this->workdir).arg(strList[0]) ;
    tHot = strList[1].toFloat() ;
    str = qts.readLine() ;
    strList = str.split(",");
    bbColdFile = QString ("%1/%2").arg(this->workdir).arg(strList[0]) ;
    tCold = strList[1].toFloat() ;

    // the gen the scan image
    str = qts.readLine() ;
    scanFile = QString ("%1/%2").arg(this->workdir).arg(str) ;
    QString outPref = qts.readLine() ;
    gp->setOutpref (outPref) ;
    so->setOutprefix (outPref) ;
    

}


void MainProcess::readProcessFileMany (QString qstr)
{
    QString str ;
    QStringList strList ;
    int i, val, leftFlag, nWaveFiles, npts ;
    float seg_yintcpt, seg_slope ;



    QFile qf (qstr) ;
    qf.open (QIODevice::ReadOnly) ;
    if (!qf.exists()){
        QMessageBox::information(0, "File open issue", "File not found" ) ;
        qf.close() ;
        return ;

    }
    // get workdir
    QTextStream qts (&qf) ;
    str = qts.readLine() ;
    QDir qd (str) ;
    if (qd.exists()) workdir = str ;

    // get the profile reconstructor
    str = qts.readLine () ;

    strList = str.split (" ") ;
    nWaveFiles = strList[0].toInt() ;
    seg_yintcpt = strList[1].toFloat() ;
    seg_slope = strList[2].toFloat() ;
    npts = strList[3].toInt() ;
    leftFlag = strList[4].toInt() ;
    if (leftFlag ==1) lFlag = true ;
    else lFlag = false ;

    // offsets, check if const offset
    val = strList[5].toInt() ;
    if (val ==1) {
            constFlag = true ;
            xoffAvg = strList[6].toFloat() ;
            yoffAvg = strList[7].toFloat() ;
    } else constFlag = false ;

    //gp->setSegParamsNum(startBin, stopBin, lFlag);
    gp->setSegParamsNew (seg_yintcpt, seg_slope, npts, lFlag) ;
    //wc->setStartNum(startBin, stopBin, lFlag);
    if (nWaveFiles !=0) {
        waveNames.clear() ;
        temps.clear() ;
        for (i=0; i<nWaveFiles; i++) {
            str = qts.readLine() ;
            strList = str.split(",") ;
            waveNames.append(strList[0]) ;
            temps.append (strList[1].toFloat()) ;
        }
        wc->setWaveFiles (waveNames, temps) ;
        wc->processFull () ;

    } else {
        str = qts.readLine() ;
        gp->readWaveFile (str) ;
        for (i=0; i<80; i++) {
            wc->wavelens.append(gp->wavelen[i]) ;
        }
    }
    // get segment file
    str = qts.readLine () ;
    qDebug () << "reading segment file " << str ;
    gp->readSegmentFile (str) ;


    // get blackbodies

    str = qts.readLine() ;

    strList = str.split(",");
    bbHotFile = QString ("%1/%2").arg(this->workdir).arg(strList[0]) ;
    tHot = strList[1].toFloat() ;
    str = qts.readLine() ;
    strList = str.split(",");
    bbColdFile = QString ("%1/%2").arg(this->workdir).arg(strList[0]) ;
    tCold = strList[1].toFloat() ;

    // the gen the scan image and the output prefix...
    while (1) {
        str = qts.readLine() ;
        if (str.length() < 5) break ;
        strList = str.split (" ") ;
        scanFile = QString ("%1/%2").arg(this->workdir).arg(strList[0]) ;
        QString outPref  = QString ("%1/%2").arg(this->workdir).arg(strList[1]) ;
        gp->setOutpref (outPref) ;
        so->setOutprefix (outPref) ;
        this->run () ;
   }

}



void MainProcess::run ()
{
    int i ;

    emit (updateProgress(0)) ;
    gp->readBBFiles (bbHotFile, bbColdFile, tHot, tCold) ;
    gp->processBB() ;
    gp->genBBCurves() ;

    emit (updateProgress(1)) ;
    // scanfile
    QFile qf0 (scanFile) ;
    if (!qf0.exists()){
        qDebug() << "Can not find scan file : " << scanFile ;
    }

    sf = new SuchiFile (scanFile) ;
    sf->readSuchiData() ;
    prdlg->setScanFile(sf) ;
    emit (updateProgress(2)) ;
    so->constOffsetFlag = constFlag ;
    // resample
    if (this->constFlag) {
        emit (updateProgress(3)) ;
        so->resampleArray(sf->indat, xoffAvg, yoffAvg,sf->nbands) ;
        //totFrames = so->totFrames ;
        emit (updateProgress(4)) ;
    }
    else {
        QString offFile = QString ("%1/offsets.txt").arg(this->workdir) ;
        xoffArr = new float [sf->nbands+20] ;
        yoffArr = new float [sf->nbands+20] ;
        so->calcOffsets (sf->indat, xoffArr, yoffArr, 0, sf->nbands-1) ;
        emit (updateProgress(3)) ;
        ///ui->aveXOffLE->setText (QString::number(so->ave_x)) ;
        // write offsetArray to text file
        QFile qf (offFile) ;
        qf.open (QIODevice::WriteOnly) ;
        QTextStream qts (&qf) ;
        //ui->aveYOffLE->setText (QString::number(so->ave_y)) ;
        so->resampleArray (sf->indat, xoffArr, yoffArr,sf->nbands) ;
        QString str ;
        for (i=0; i<sf->nbands; i++){
            str = QString ("%1 %2\r\n").arg(xoffArr[i], yoffArr[i]) ;
            qts << str.toLatin1().data() ;

        }
        qf.close() ;
        emit (updateProgress(4)) ;
        //delete [] xoffArr ;
        //delete [] yoffArr ;

    }

    gp->processCube (so) ;
    emit (updateProgress(5)) ;
    gp->calcEmissivity() ;
    emit (updateProgress(6)) ;
    //prdlg->loadScanImage (0) ;
}

void MainProcess::HideMainProc()
{
    //ui->
}

void MainProcess::ShowMainProc()
{
    //show();
}
