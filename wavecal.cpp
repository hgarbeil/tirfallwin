#include "wavecal.h"
#include "suchifile.h"
#include "suchi_utils.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

using namespace suchi_utils ;
extern int ns_inst, nl_inst ;

Wavecal::Wavecal(QObject *parent) :
    QObject(parent)
{
    nfiles = 0 ;
    npixTot = ns_inst * nl_inst  ;
    profiles = 0L ;
    wavenum = 0l;
    pixvals = 0l ;

    wavecoefs = new double [2] ;
    wavedata = 0l;

    leftFlag = false ;
    outfileFlag = true ;
    outfile = "waves.txt" ;
    yf = 0L ;
    firstFile = 0L ;

    procLoc = 166 ;
    prof0 = 0 ;
    prof1 = 0 ;
    profiles = 0L ;
    ns = ns_inst ;
    nl = nl_inst ;
    firstFile = new float [ns*nl] ;
}

Wavecal::~Wavecal () {
    if (firstFile) delete [] firstFile ;
    if (wavedata) delete [] wavedata ;
    if (prof0) delete [] prof0 ;
    if (prof1) delete [] prof1 ;
    if (profiles) delete [] profiles ;
}

void Wavecal::setProcessLoc (int x){
    procLoc = x ;
}

void Wavecal::setOutputFlag (bool b){
    outfileFlag = b ;
}

void Wavecal::setStartStop (int start, int stop, bool  leftFlag){
    this->start = start ;
    this->stop = stop ;
    this->leftFlag = leftFlag ;
}



void Wavecal::setOutputFile(QString str){
    outfile = str ;
}

void Wavecal::setWaveFiles (QList<QString> f, QList<float> wavelens){
    int i ;
    long is, npixTotB =npixTot * 100 ;

    this->filenames = f ;
    this->wavelens = wavelens ;
    nfiles = filenames.count() ;
    if (pixvals){
        delete [] pixvals ;

    }
    if (wavenum) {
        delete [] wavenum ;

    }
    if (yf){
        delete [] yf ;
    }
    pixvals = new double [nfiles] ;
    wavenum = new double [nfiles] ;
    yf = new double [nfiles] ;

    nfiles = filenames.length() ;
    wavedata = new unsigned short [npixTotB * nfiles] ;
    // read in the first file
    for (i=0; i<nfiles; i++){
        SuchiFile *suchi = new SuchiFile(filenames[i]) ;
        suchi->readSuchiData() ;
        for (is=0;is<npixTotB; is++) wavedata[i*npixTotB+is] = suchi->indat[is] ;
        if (i==0)
        flattenData(suchi->indat, firstFile, ns, nl, 100) ;
        delete suchi ;
    }
}

void Wavecal::loadProfiles() {

    int     i, j, nsamps, proc0, proc1 ;
    int     endrow, startrow  ;
    float   subloc ;
    float   wnum, wmic ;
    startrow = start ;
    endrow = stop ;

    process () ;
    return ;

    nsamps = (endrow - startrow +1) * 2 ;
    float   * tmpprof = new float [nl * nfiles] ;
    float   *tmparr = new float [nsamps] ;

    double  *tmp = new double [nsamps] ;


    profiles = new float [nfiles * nsamps] ;
    prof0 = new float [nfiles * nl *10] ;
    prof1 = new float [nfiles * nl] ;
    proc0 = procLoc -1 ;
    proc1 = procLoc+1 ;
    proc0 = (proc0<0? 0 : proc0) ;
    proc1 = (proc1>=ns? ns-1 : proc1) ;

    for (i=0; i<nfiles;i++) {
        //SuchiFile *suchi = new SuchiFile(filenames[i]) ;
        //qDebug ()<< "Opening  : " << filenames [i] ;
        //suchi->readSuchiData() ;
        extractAvgProfile (wavedata + i * npixTot*100, tmpprof +  nl*i, ns, nl, 100, proc0, proc1) ;
        // get the column of data
        procColumn (tmpprof + nl*i, tmparr, startrow, endrow, leftFlag) ;
        for (j=0; j<nsamps; j++) {
            tmp[j] = tmparr[j] ;
            prof0[i*nsamps+j] = tmparr[j] ;
        }
    }

    delete [] tmpprof ;
    delete [] tmparr ;
    delete [] tmp ;


}


void Wavecal::process() {

    int     i, j, nsamps, proc0, proc1 ;
    float    endrow, startrow  ;
    float   subloc ;
    float   wnum, wmic ;
    startrow = start ;
    endrow = stop ;

    nsamps = (endrow - startrow +1) * 2 ;
    float   *tmpprof = new float [nl * nfiles] ;
    float   *tmparr = new float [nsamps] ;

    double  *tmp = new double [nsamps] ;


    if (prof0 != 0) {
        delete [] prof0 ;
        delete [] prof1 ;
        delete [] profiles ;
    }




    profiles = new float [nfiles * nsamps] ;
    prof1 = new float [nfiles * nsamps] ;
    prof0 = new float [nfiles * nl] ;

    proc0 = procLoc -1 ;
    proc1 = procLoc+1;
    proc0 = (proc0<0? 0 : proc0) ;
    proc1 = (proc1>=ns? ns-1 : proc1) ;

    for (i=0; i<nfiles;i++) {
        //SuchiFile *suchi = new SuchiFile(filenames[i]) ;
        //qDebug ()<< "Opening  : " << filenames [i] ;
        //suchi->readSuchiData() ;
        // get the column of data
        extractAvgProfile (&wavedata[i*npixTot*100], tmpprof + nl *i, ns, nl, 100, proc0, proc1) ;
        // process, returning fft, etc...
        procColumn_noFFT (tmpprof+ nl*i, tmparr, startrow, endrow, leftFlag) ;


        for (j=0; j<nl; j++) prof0 [i*nl+j]=tmpprof[i*nl+j] ;
        for (j=0; j<nsamps; j++) {
            tmp[j] = tmparr[j] ;
            prof1[i*nsamps+j] = tmparr[j] ;

        }
        procColumn (tmpprof + nl*i, tmparr, startrow, endrow, leftFlag) ;
        for (j=0; j<nsamps; j++) {
            tmp[j] = tmparr[j] ;
            //prof1[i*nsamps+j] = tmparr[i*nl+j] ;
            profiles[i*nsamps+j] = tmparr[j] ;
        }
        // then fft
        //fftcall (tmp, fftarr, nsamps, -1) ;
        //getMag (fftarr, profiles+i*nsamps, nsamps) ;

                //suchi->flattenData (suchi->indat, frames+ns*nl*i) ;

        //getMaxSubpix (profiles+i*nsamps, nsamps, &subloc, 5, 32) ;
        getMaxSubpix (tmparr, 180, &subloc, 15, 180) ;
        printf ("max of %d : %f\r\n", i, subloc) ;
        wavenum[i]= 10000. / wavelens[i]  ;
        pixvals[i] = subloc ;



    }

    /// fit polynomial to describe relationship of wavenumber to bandnumber (pixel)
    polyfit (pixvals, wavenum, nfiles, 1, wavecoefs, yf) ;

    QFile qf (outfile) ;
    qf.open (QIODevice::WriteOnly | QIODevice::Text) ;
    QTextStream qts (&qf) ;
    for (i=0; i<180; i++){
        wnum = wavecoefs[0] + (float)i * wavecoefs[1] ;
        wmic = 10000. / wnum ;
        qts << i << "\t" << wnum <<"\t" << wmic << endl ;
    }
    qf.close () ;
    //QFile qf ("/home/harold/profile");
    //qf.open (QIODevice::WriteOnly) ;
    //qf.write ((char *)tmpprof, nl * nfiles * 4) ;
    //qf.close () ;
    delete [] tmpprof ;
    delete [] tmp ;
    delete [] tmparr ;
}



void Wavecal::processFullArray() {

    int     i, j, nsamps, proc0, proc1 ;
    float     endrow, startrow  ;
    float   subloc ;
    float   wnum, wmic ;
    startrow = start ;
    endrow = stop ;

    nsamps = (endrow - startrow +1) * 2 ;
    float   *tmpprof = new float [nl * nfiles] ;
    float   *tmparr = new float [nsamps] ;

    double  *tmp = new double [nsamps] ;


    if (prof0 != 0) {
        delete [] prof0 ;
        delete [] prof1 ;
        delete [] profiles ;
    }




    profiles = new float [nfiles * nsamps] ;
    prof1 = new float [nfiles * nsamps] ;
    prof0 = new float [nfiles * nl] ;


    proc0 = procLoc -1 ;
    proc1 = procLoc+1 ;
    proc0 = (proc0<0? 0 : proc0) ;
    proc1 = (proc1>=ns? ns-1 : proc1) ;


    proc0 = 5 ;
    proc1 = ns-5 ;
    for (i=0; i<nfiles;i++) {
        //SuchiFile *suchi = new SuchiFile(filenames[i]) ;
        //qDebug ()<< "Opening  : " << filenames [i] ;
        //suchi->readSuchiData() ;
        // get the column of data
        extractAvgProfile (&wavedata[i*npixTot*100], tmpprof + nl *i, ns, nl, 100, proc0, proc1) ;
        // process, returning fft, etc...
        procColumn_noFFT (tmpprof+ nl*i, tmparr, startrow, endrow, leftFlag) ;


        for (j=0; j<nl; j++) prof0 [i*nl+j]=tmpprof[i*nl+j] ;
        for (j=0; j<nsamps; j++) {
            tmp[j] = tmparr[j] ;
            prof1[i*nsamps+j] = tmparr[j] ;

        }
        procColumn (tmpprof + nl*i, tmparr, startrow, endrow, leftFlag) ;
        for (j=0; j<nsamps; j++) {
            tmp[j] = tmparr[j] ;
            //prof1[i*nsamps+j] = tmparr[i*nl+j] ;
            profiles[i*nsamps+j] = tmparr[j] ;
        }
        // then fft
        //fftcall (tmp, fftarr, nsamps, -1) ;
        //getMag (fftarr, profiles+i*nsamps, nsamps) ;

                //suchi->flattenData (suchi->indat, frames+ns*nl*i) ;

        //getMaxSubpix (profiles+i*nsamps, nsamps, &subloc, 5, 32) ;
        getMaxSubpix (tmparr, 180, &subloc, 5, 180) ;
        printf ("max of %d : %f\r\n", i, subloc) ;
        wavenum[i]= 10000. / wavelens[i]  ;
        pixvals[i] = subloc ;



    }

    /// fit polynomial to describe relationship of wavenumber to bandnumber (pixel)
    polyfit (pixvals, wavenum, nfiles, 1, wavecoefs, yf) ;

    if (this->outfileFlag){
    QFile qf (outfile) ;
    qf.open (QIODevice::WriteOnly | QIODevice::Text) ;
    QTextStream qts (&qf) ;
    for (i=0; i<200; i++){
        wnum = wavecoefs[0] + (float)i * wavecoefs[1] ;
        wmic = 10000. / wnum ;
        qts << i << "\t" << wnum <<"\t" << wmic << endl ;
    }
    qf.close () ;
    }
    delete [] tmpprof ;
    delete [] tmp ;
    delete [] tmparr ;
}


/***
 * not fully implemented but can be kicked in to generate a calibration for each column of the interferometer
 * currently averaging some or all lines to come up with a single cal for the image
 */

void Wavecal::processFull() {

    int     i, j, is, jcol, nsamps, npix ;
    int     endrow, startrow, jcol0, jcol1  ;
    float   subloc ;
    float   wnum, wmic ;
    startrow = start ;
    endrow = stop ;

    nsamps = (endrow - startrow +1) * 2 ;
    float   * tmpprof = new float [nl * nfiles] ;
    float   *tmparr = new float [nsamps] ;
    float   *fftmag = new float [nsamps] ;
    double  *tmp = new double [nsamps] ;
    double  *fftarr = new double [nsamps* 2] ;
    npix = ns * nl ;
    long npixb = ns * nl * 100 ;


    profiles = new float [nfiles * nsamps] ;
    prof0 = new float [nfiles * nsamps] ;
    unsigned short *indata = new unsigned short [nfiles * npixb] ;
/*
    for (i=0; i<nfiles;i++) {
        //SuchiFile *suchi = new SuchiFile(filenames[i]) ;
        //qDebug ()<< "Opening  : " << filenames [i] ;
        //suchi->readSuchiData() ;
        for (is=0; is< npixb; is++){
            indata[i*npixb+is] = suchi->indat[is] ;
        }
   }
*/
    // output ascii file...
    if (outfileFlag){

    }

   /// column by column go through the array to get a wavecal for each image
   for (jcol=1; jcol<ns-1; jcol++) {
       for (i=0; i<nfiles; i++){
           jcol0 = jcol - 1 ;
           if (jcol0 < 0) jcol0 = 0 ;
           jcol1 = jcol+1 ;
           if (jcol1 >= ns) jcol1 = ns-1 ;

           extractAvgProfile (&wavedata[i*npixb], tmpprof +  nl*i, ns, nl, 100, jcol0, jcol1) ;
        // get the column of data
        procColumn (tmpprof + nl*i, tmparr, startrow, endrow, leftFlag) ;

        for (j=0; j<nsamps; j++) {
            tmp[j] = tmparr[j] ;
            prof0[i*nsamps+j] = tmparr[j] ;
        }
        // then fft
        fftcall (tmp, fftarr, nsamps, -1) ;
        getMag (fftarr, profiles+i*nsamps, nsamps) ;

                //suchi->flattenData (suchi->indat, frames+ns*nl*i) ;

        getMaxSubpix (profiles+i*nsamps, nsamps, &subloc, 5, 180) ;
        printf ("max of %d : %f\r\n", i, subloc) ;
        wavenum[i]= 10000. / wavelens[i]  ;
        pixvals[i] = subloc ;
       }




        /// fit polynomial to describe relationship of wavenumber to bandnumber (pixel)
        polyfit (pixvals, wavenum, nfiles, 1, wavecoefs, yf) ;
        //if (outfileFlag)
           // qts << jcol << "\t" <<wavecoefs[0] << "\t" << wavecoefs[1] << endl ;
   }





    //QFile qf ("/home/harold/profile");
    //qf.open (QIODevice::WriteOnly) ;
    //qf.write ((char *)tmpprof, nl * nfiles * 4) ;
    //qf.close () ;
}
