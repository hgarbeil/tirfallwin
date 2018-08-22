#include "suchi_offsets.h"
//#include "Instrument_config.h"
#include "suchi_utils.h"
//#include "cvprocess.h"
#include <QDebug>
#include <QFile>
extern int ns_inst, nl_inst ;

suchi_offsets::suchi_offsets(QObject *parent) :
    QObject(parent)
{
    ave_x = 0. ;
    ave_y = 0. ;
    totFrames = 0 ;
    outPrefix = "" ;
    nsamps = ns_inst ;
    nlines = nl_inst ;
    nbands = 0 ;
    xoffArr = 0;
    yoffArr = 0l ;
    defineWindow (10, 100, 180,138, nsamps, nlines) ;

}



void suchi_offsets::setConstantFlag (bool f){
    constOffsetFlag = f ;
}

void suchi_offsets::defineWindow (int x0, int y0, int nx, int ny, int nsamps, int nlines){
    startx = x0 ;
    starty = y0 ;
    nswin = nx ;
    nlwin = ny ;
    this->nsamps = nsamps ;
    this->nlines = nlines ;
}


void suchi_offsets::setOutprefix(QString opref) {
    outPrefix = opref ;
}

void suchi_offsets::calcOffsets (unsigned short *idat, float *xoff, float *yoff, int startFrame, int numFrames){

    int i, j,  js,  iFrame, refInc ;
    long il, npixtot ;
    unsigned short *iptr ;
    float *chip0, *chip1, xinc, yinc, x0, y0 ;

    int iterInc = 10 ;
    int nframesCalc = 0 ;
    nbands = numFrames ;
    npixtot = long(nsamps) * nlines ;
    xoffArr = new float [numFrames] ;
    yoffArr = new float [numFrames] ;
    //int nframes = 1 ;

    // NO OPENCV for windows version currently

    /*
    CVProcess *cv = new CVProcess () ;
    chip0 = new float [npixtot] ;
    chip1 = new float [npixtot] ;
    // running offset to keep track
    x0 = 0. ;
    y0 = 0. ;

    xoff[0] = 0. ;
    yoff[0] = 0. ;
    xoffArr[0] = yoffArr[0] = 0. ;
    refInc = 0 ;
    for (il=0; il <npixtot; il++){
        chip0 [il] = idat[il+startFrame * npixtot] ;
    }
    //QFile qf ("/home/harold/chips") ;
    for (i=1; i< numFrames; i++){
        for (il=0; il <npixtot; il++){
            chip1 [il] = idat[(startFrame+i)*npixtot+il] ;
        }

            //qf.open (QIODevice::WriteOnly) ;

            //qf.write((char *) chip1, npixtot * 4) ;


        cv->loadFromFloatArray (chip0, chip1, nswin, nlwin, startx, starty, nsamps, nlines) ;
        if (cv->nMatchPoints > 3) {
        suchi_utils::calcMedian (cv->xyoffs, cv->nMatchPoints, &xinc, &yinc ) ;
        ave_x += xinc / (i - refInc) ;
        ave_y += yinc / (i - refInc) ;
        nframesCalc++ ;
        } else {
            xinc = 0. ;
            yinc = 0. ;
        }
        xoff[i] = xinc + xoff[refInc] ;
        yoff[i] = yinc + yoff[refInc] ;
        xoffArr[i] = xoff[i] ;
        yoffArr[i] = yoff[i] ;
        // check if you need to refresh the reference chip
        if (i%iterInc == 0) {
            refInc = i ;
            for (il=0; il <npixtot; il++) chip0[il] = chip1[il] ;

        }
        qDebug () << "Number of match points : "<<  cv->nMatchPoints << endl ;


    }


    ave_x /= nframesCalc ;
    ave_y /= nframesCalc ;
    //qf.close() ;
    */
}


void suchi_offsets::resampleArray (unsigned short *idat, float *xoff, float *yoff, int nframes){
    int i, j, is, js, samploc, nLines, iframe, newloc0 ;
    int xloc,yloc, zloc ;
    float xoffval ;
    float frac0, frac1, diff ;
    float magoff = fabs (ave_y * nframes )-1 ;
    int iLoc, lastis=0 ;
    totFrames = magoff + nlines  ;
    long npix = nsamps * nlines ;



    nLines = 0 ;

    // do the y offset, which is done on an individual frame
    if (ave_x > 0. || ave_x <0.){
        float *tmpframe0 = new float [npix * totFrames] ;
        // first frame, no offsets applied
        for (i=0;i<npix; i++)*(tmpframe0+i)=*(idat+i) ;
        for (iframe=0; iframe<totFrames; iframe++) {
            xoffval = xoff[iframe] ;
            xloc = int(xoffval) ;
            frac0 = 1. - (xoffval-xloc) ;
            frac1 = 1. -frac0 ;
            for (is=0; is<nlines; is++){

                for (js=0; js<nsamps; js++){
                    newloc0 = js + xloc ;
                    if (newloc0 < 0) newloc0 = 0 ;
                    if (newloc0 >= nsamps-2) newloc0 = nsamps-2 ;
                    *(tmpframe0+iframe*npix+is*nsamps+js) =
                            frac0 * *(idat+iframe*npix+is * nsamps+newloc0)+
                            frac1 * *(idat+iframe*npix+is * nsamps+newloc0+1);
                }

            }

        }
        for (i=0; i<npix * totFrames; i++){
            *(idat+i)=*(tmpframe0+i) ;
        }
        delete [] tmpframe0 ;
    }



    // output array is totFrames x nsamples x nlines
    outarr = new float [npix * totFrames] ;
    float *tmpframe = new float [nsamps * nlines] ;
    if (ave_y > 0) {
        for (i=0; i<magoff; i++){
            for (is=lastis; is<nframes-1; is++){
                if (yoff[is]<=i && yoff[is+1]>i){
                    iLoc = is ;
                    lastis = is ;
                    diff =  (yoff[is+1] - i) + (i - yoff[is]) ;
                    frac0 = (yoff[is+1] -i) / diff ;
                    frac1 = 1. - frac0 ;
                    break ;
                }
            }
            // resample the frames to get a new frame to insert in the stack array
            for (is=0; is<nlines; is++) {
                for (js=0; js<nsamps; js++) {
                    samploc = is * nsamps + js ;
                    tmpframe[is * nsamps + js] = frac0 *  idat[iLoc * npix + samploc ] + (1.-frac0) * idat[(iLoc+1) * npix + samploc ] ;
                }
            }// resample the frames to get a new frame to insert in the stack array
            // now put that frame in the appropriate location
            for (is=0; is<nlines; is++) {
                xloc = is  ;
                yloc = i + nlines - is - nlines;
                if (yloc< 0) continue ;
                if (is==0) nLines++ ;
                for (js=0; js<nsamps; js++){

                    zloc = js ;
                    //outarr[zloc * totFrames * nlines + yloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                    outarr[yloc * npix + zloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                }
            }

        }

    }
    if (ave_y < 0.){
        for (i=0; i<magoff; i++){
            for (is=lastis; is<nframes-1; is++){
                if (-yoff[is]<=i && -yoff[is+1]>i){
                    iLoc = is ;
                    lastis = is ;
                    diff =  (-yoff[is+1] - i) + (i +  yoff[is]) ;
                    frac0 = (-yoff[is+1] -i) / diff ;
                    frac1 = 1. - frac0 ;
                    break ;
                }
            }
            // resample the frames to get a new frame to insert in the stack array
            for (is=0; is<nlines; is++) {
                for (js=0; js<nsamps; js++) {
                    samploc = is * nsamps + js ;
                    tmpframe[is * nsamps + js] = frac0 *  idat[iLoc * npix + samploc ] + (1.-frac0) * idat[(iLoc+1) * npix + samploc ] ;
                }
            }// resample the frames to get a new frame to insert in the stack array

            for (is=0; is<nlines; is++) {
                xloc = is  ;
                yloc = i + is - nlines;

                if (yloc < 0) continue ;
                if (is==0) nLines++ ;
                for (js=0; js<nsamps; js++){

                    zloc = js ;
                    //outarr[zloc * totFrames * nlines + yloc * nlines + xloc]
                    outarr[yloc * npix + zloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                }
            }

        }


   }
   totFrames = nLines ;



    qDebug() << "size of output is " << nlines << " " <<  totFrames  << " "<< nsamps << endl ;
    QString filename = QString ("%1_stacked").arg(outPrefix) ;
    QString outfile_hdr = QString ("%1.hdr").arg(filename) ;
    QFile qf (filename) ;
    qf.open (QIODevice::WriteOnly) ;
    qf.write ((char *)outarr, 2*npix * totFrames) ;
    qf.close() ;

    QFile qf_hdr (outfile_hdr) ;
    qf_hdr.open (QIODevice::WriteOnly) ;
    QTextStream qts (&qf_hdr) ;
    qts << "Envi = \r\ndescription = {\r\nTircis Stacked Cube.}\r\n" ;
    qts << "samples = " << this->nsamps << "\r\n" ;
    qts << "lines = " << this->totFrames << "\r\n" ;
    qts <<  "bands = " << this->nlines << "\r\n" ;
    qts << "header offset = 0" << "\r\n" ;
    qts << "file type = ENVI Standard" << "\r\n" ;
    qts << "data type = 12" << "\r\n" ;
    qts << "interleave = bip" << "\r\n" ;

    qDebug() << "Header file written : " << outfile_hdr << "\r\n" ;


}

// array dimensions are nlines (elems in interferometer) by number of frames in image + 2 * nlines by number of samples
void suchi_offsets::resampleArray (unsigned short *idat, float ave_x, float ave_y,  int nframes )
{
    int i, j, is, js, samploc, xloc, yloc, zloc, nLines ;
    int iframe, newloc0 ;

    float fracLoc, frac0, frac1, xoffval ;
    float magoff = fabs (ave_y * nframes );
    int imagoff = int(magoff) ;
    int iLoc ;
    totFrames = imagoff + nlines ;
    /****** to shorten processing for now *****/
    // if (totFrames>1024) totFrames = 1024 ;
    long npix = nsamps * nlines ;

    nLines = 0 ;

    if (ave_x > 0. || ave_x <0.){
        float *tmpframe0 = new float [npix * totFrames] ;
        // first frame, no offsets applied
        for (i=0;i<npix; i++)*(tmpframe0+i)=*(idat+i) ;
        for (iframe=1; iframe<totFrames; iframe++) {
            xoffval = ave_x * iframe ;
            xloc = int(xoffval) ;
            frac0 = 1. - (xoffval-xloc) ;
            frac1 = 1. - frac0 ;
            for (is=0; is<nlines; is++){

                for (js=0; js<nsamps; js++){
                    newloc0 = js + xloc ;
                    if (newloc0 < 0) newloc0 = 0 ;
                    if (newloc0 >= nsamps-2) newloc0 = nsamps-2 ;
                    *(tmpframe0+iframe*npix+is*nsamps+js) =
                            frac0 * *(idat+iframe*npix+is * nsamps+newloc0)+
                            frac1 * *(idat+iframe*npix+is * nsamps+newloc0+1);
                }

            }

        }
        for (i=0; i<npix * totFrames; i++){
            *(idat+i)=*(tmpframe0+i) ;
        }
        delete [] tmpframe0 ;
    }


    // output array is totFrames x nsamples x nlines
    outarr = new float [npix * totFrames] ;\
    float *tmpframe = new float [npix] ;
    // motion from bottom to top (decreasing in y)
    if (ave_y > 0.) {
        for (i=0; i<imagoff-1; i++){
            fracLoc =  (float (i) / ave_y) ;
            iLoc = (int) fracLoc ;
            frac0 = 1. - (fracLoc - iLoc) ;
            // resample the frames to get a new frame to insert in the stack array
            for (is=0; is<nlines; is++) {
                for (js=0; js<nsamps; js++) {
                    samploc = is * nsamps + js ;
                    if (frac0<.999999 && iLoc < nframes-1)
                        tmpframe[is * nsamps + js] = frac0 *  idat[iLoc * npix + samploc ] + (1.-frac0) * idat[(iLoc+1) * npix + samploc ] ;
                    else
                        tmpframe[is * nsamps + js] = idat[iLoc * npix + samploc ] ;
                }
            }
            // now put that frame in the appropriate location
            for (is=0; is<nlines; is++) {
                xloc = is  ;
                yloc = i + nlines - is - nlines;
                if (yloc< 0) continue ;
                if (is==0) nLines++ ;
                for (js=0; js<nsamps; js++){

                    zloc = js ;
                    //outarr[zloc * totFrames * nlines + yloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                    outarr[yloc * npix + zloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                }
            }

        }


    }

    if (ave_y < 0.) {
            for (i=0; i<magoff; i++){
                fracLoc = -(float (i) / ave_y) ;
                iLoc = (int) fracLoc ;
                frac0 = 1. - (fracLoc - iLoc) ;
                // resample the frames to get a new frame to insert in the stack array
                for (is=0; is<nlines; is++) {
                    for (js=0; js<nsamps; js++) {
                        samploc = is * nsamps + js ;
                        tmpframe[is * nsamps + js] = frac0 *  idat[iLoc * npix + samploc ] + (1.-frac0) * idat[(iLoc+1) * npix + samploc ] ;
                    }
                }
                // now put that frame in the appropriate location
                for (is=0; is<nlines; is++) {
                    xloc = is  ;
                    yloc = i + is - nlines;

                    if (yloc < 0) continue ;
                    if (is==0) nLines++ ;
                    for (js=0; js<nsamps; js++){

                        zloc = js ;
                        //outarr[zloc * totFrames * nlines + yloc * nlines + xloc]
                        outarr[yloc * npix + zloc * nlines + xloc] = tmpframe [is * nsamps + js] ;
                    }
                }

            }


      }
    totFrames = nLines ;
    qDebug() << "size of output is " << nlines << " " <<  totFrames  << " "<< nsamps << endl ;
    QString filename = QString ("%1_stacked").arg(outPrefix) ;
    QString outfile_hdr = QString ("%1.hdr").arg(filename) ;
    QFile qf (filename) ;
    qf.open (QIODevice::WriteOnly) ;
    qf.write ((char *)outarr, 4*npix * totFrames) ;
    qf.close() ;

    QFile qf_hdr (outfile_hdr) ;
    qf_hdr.open (QIODevice::WriteOnly) ;
    QTextStream qts (&qf_hdr) ;
    qts << "Envi = \r\ndescription = {\r\nTircis Stacked Cube.}\r\n" ;
    qts << "samples = " << this->nsamps << "\r\n" ;
    qts << "lines = " << this->totFrames << "\r\n" ;
    qts <<  "bands = " << this->nlines << "\r\n" ;
    qts << "header offset = 0" << "\r\n" ;
    qts << "file type = ENVI Standard" << "\r\n" ;
    qts << "data type = 12" << "\r\n" ;
    qts << "interleave = bip" << "\r\n" ;

    qDebug() << "Header file written : " << outfile_hdr << "\r\n" ;
}

