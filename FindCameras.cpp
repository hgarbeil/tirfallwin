//////////////////////////////////////////////////////////////////////
//
//  FindCameras.cpp - Implementation
//
//////////////////////////////////////////////////////////////////////

#include <unistd.h> //for usleep
#include <QMessageBox>
#include <QMutex>
#include <QWaitCondition>

#include "FindCameras.h"
#include "gige_lib.h"
#include "defines.h"



//////////////////////////////////////////////////////////////////////
// CFindCameras constructor
CFindCameras::CFindCameras()
{
}

//////////////////////////////////////////////////////////////////////

CFindCameras::~CFindCameras()
{
}



//////////////////////////////////////////////////////////////////////

int CFindCameras::FindCameras()
{
    uint16_t i;
    //int16_t MC320Index;
    vector<gige_acknowledge_ack> gige_list;
    struct timeval StartTime;
    struct timeval PresentTime;
    int64_t Duration, seconds, useconds;

    // Get time for timeout
    gettimeofday(&StartTime, NULL);

    do {
        gige_list = gige_discover();
        if(!gige_list.empty()) {
            break;
        }

        // 2000ms timeout
        gettimeofday(&PresentTime, NULL);
        seconds  = PresentTime.tv_sec  - StartTime.tv_sec;
        useconds = PresentTime.tv_usec - StartTime.tv_usec;
        Duration = seconds*1000000.0 + useconds;
        if(Duration > 2000000) break;   // Timeout
    } while(true);


    if(gige_list.empty()) {
        //printf("No Cameras found. Check Firewall.");
        //return(TESTPATTERN320X240);
        return(TESTPATTERN640X512);
    }


    // Determine which item in the list is the MC320.
    //MC320Index = -1;
    for(i=0 ; i<gige_list.size() ; i++) {
        if( strncmp(gige_list[i].model, "MC320", 5) == 0 ) {
            return(MC320);
        }
        if( strncmp(gige_list[i].model, "PT1000-CL4", 8) == 0 ) {
            return(TAU640);
        }
        if( strncmp(gige_list[i].model, "PV640", 5) == 0 ) {
            return(PV640);
        }
    }

    return(TESTPATTERN320X240);
    //return(TESTPATTERN640X512);
}

//////////////////////////////////////////////////////////////////////
