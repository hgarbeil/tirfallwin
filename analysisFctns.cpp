#include "mainwindow.h"
#include <math.h>
#include <semaphore.h>
#include "defines.h"
#include "acqmc320.h"
#include "acqtau.h"
#include "acqDummy320x240.h"
#include "acqDummy640x512.h"
//#include "AcqDummy640.h"
#include "global.h"
#include "analysisFctns.h"



typedef struct { float r, i; } cmplex;
int zfft(cmplex*, int, int);


void DoFFT(unsigned short *m_nPhotonFrameBuffer)
{
    int i;
    int len, flag;
    int	FFTRow;
    int	FFTPixelToSubtract;
    float MiddleLineData[AcqImgWidth];
    cmplex fdat[AcqImgWidth];
    double FFT[AcqImgWidth];


    //    //Check for invalid input values.
    //    FFTRow = GetDlgItemInt(IDC_FFTROW, &ReturnValue, FALSE);
    //    if( FFTRow>255 || FFTRow<0 ) {
    FFTRow = 128;
    //        SetDlgItemText(IDC_FFTROW, "128");
    //    }

    //    FFTPixelToSubtract = GetDlgItemInt(IDC_FFTCOL, &ReturnValue, FALSE);
    //    if( FFTPixelToSubtract>319 || FFTPixelToSubtract<0 ) {
    FFTPixelToSubtract = 40;
    //        SetDlgItemText(IDC_FFTCOL, "40");
    //    }

    //Subtract the data value at "FFTPixelToSubtract" from the data and multiply by the Sawtooth wave.
    for(i=0 ; i<AcqImgWidth ; i++) {
        MiddleLineData[i] = (float)(m_nPhotonFrameBuffer[i+AcqImgWidth*FFTRow]
                                    - m_nPhotonFrameBuffer[FFTPixelToSubtract+AcqImgWidth*FFTRow])
                * SawToothWave[i];
    }

    //Put data into complex array.
    for(i=0 ; i<AcqImgWidth ; i++) {
        fdat[i].r =	MiddleLineData[i];	//(float)m_nPhotonFrameBuffer[currBufNum%5][i+IMAGEWIDTH*256/2];
        fdat[i].i = (float)0;
    }

    //Do the FFT.
    len = 320; flag = 0;
    zfft(&fdat[0], len, flag);

    for( i=0 ; i<AcqImgWidth ; i++ ) {
        FFT[i] = sqrt( pow(fdat[i].r, 2) + pow(fdat[i].i, 2) );
    }

    //***********************************************************************************************************
    //***********************************************************************************************************
    //***********************************************************************************************************
    //***********************************************************************************************************

    //Get Min and Max of plot
    FFTMaxVal = -99999;
    for (i=15 ; i<128 ; i++) {
        if( FFTMaxVal < (int)(FFT[i] + 0.5) ) {
            FFTMaxVal = (int)(FFT[i] + 0.5);
        }
    }
    FFTMaxVal++;
}
