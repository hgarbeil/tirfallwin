//#include <string.h>
#include "BbArroyo.h"

BbArroyo::BbArroyo()
{
    //printf("Starting BB %s\n" , port);
    parm.MonitorInitialized = false;
    parm.ControlInitialized = false;
    parm.Terminate = false;
    parm.Busy = false;
}


void BbArroyo::Init(char *port, int baud, char *name)
{
    strcpy(parm.name, name);
    Open(port, baud);
    StartMonitor();
    StartControl();
}

void BbArroyo::Open(char *port, int baud)
{
    cssl_start();

    //parm.Serial=cssl_open(port, 0, 38400,8,0,1);
    parm.Serial=cssl_open(port, 0, baud,8,0,1);
    if (parm.Serial) {
        cssl_settimeout(parm.Serial, 0, 0.5);  // .5 second timeout.
        parm.PortOpen = true;
        strcpy(parm.Status, "Open");
    } else {
        parm.PortOpen = false;
        strcpy(parm.Status, "Error");
    }
    usleep(300000);
    parm.Serial=cssl_open(port, 0, baud,8,0,1);
/*
    //Send command to get version.
    strcpy(comStringOut, "\r\nVER?\r\n");
    cssl_putstring(Serial, comStringOut);
    usleep(500000);

    // Get the version
    memset (comStringIn, 0, 128);
    cssl_getdata(Serial, (uint8_t*)comStringIn, nBytes);
    usleep(500000);
*/
    //Send command to turn on output.
//    strcpy(parm.comStringOut, "\r\nTEC:OUT 1\r\n");
//    cssl_putstring(parm.Serial, parm.comStringOut);
//    usleep(300000);
}

void BbArroyo::Kill()
{
    parm.Terminate=true;
}


float BbArroyo::Get()
{
//return(0.0);
//    while(!parm.MonitorInitialized)
//        usleep(1000000);
    if(!parm.MonitorInitialized) return(-99);
    return(parm.PresentTemperature);
}

void BbArroyo::Set(float DesiredTemperature)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // This needs timeout and way to reconnect!!!!!!
    // And maybe separate thread
    //while(!parm.ControlInitialized) usleep(100000);
    if(!parm.ControlInitialized) return;

    parm.DesiredTemperature = DesiredTemperature;
    sem_post (&parm.SetTemperature_count);
}


void BbArroyo::Monitor()
{
    int nBytes=255;
    //sem_init (&parm.GetTemperature_count, 0, 0);

    while(!parm.Terminate) {
        //sem_wait (&parm.GetTemperature_count);

        if(parm.PortOpen) {
            //If the temperature is being set, skip getting the temperature this time.
            if(!parm.Busy) {
                parm.Busy = true;
                parm.comStringIn[0]=0;
                memset (parm.serialdata, 0, 128);

                //Send command to get temperature.
                strcpy(parm.comStringOut, "TEC:T?\r\n");
                cssl_putstring(parm.Serial, parm.comStringOut);

                usleep(1000000);

                // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! PUT TIMEOUT HERE !!!!!!!!!!!!!!!!!
                while( (strstr(parm.comStringIn, "\n")==NULL) ) {
                    *parm.serialdata = 0;
                    nBytes = cssl_getdata(parm.Serial, (uint8_t*)parm.serialdata, 1);
                    if(nBytes==1) {
                        strcat(parm.comStringIn, parm.serialdata);
                    }
                    usleep(80000);
                }

                // Parse the temperature.
                parm.NewLinePos = (strstr(parm.comStringIn, "\r"));
                if(parm.NewLinePos!=NULL) {
                    *parm.NewLinePos = (char)0;
                }
                if(strlen(parm.comStringIn) > 0) {
                    parm.PresentTemperature = atof( parm.comStringIn );
                }

                parm.MonitorInitialized = true;
                parm.Busy = false;
                usleep(200000);
                //printf("Temperature = %3.2f\n" , parm.PresentTemperature);
            }
        } else {
            usleep(2000000);
        }
    }

    if (parm.Serial) cssl_close(parm.Serial);
}

void BbArroyo::Control()
{
    sem_init (&parm.SetTemperature_count, 0, 0);
    parm.ControlInitialized = true;

    while(!parm.Terminate) {
        sem_wait (&parm.SetTemperature_count);
        if(parm.Terminate)
            break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the temperature (put a timeout here)
            while(parm.Busy) {
                usleep(100000);
            }
            parm.Busy = true;

            // Set the temperature.
            //sprintf (comStringOut, "TEC:T %d;TEC:OUT 1\r\n", (int)DesiredTemperature0);
            sprintf( parm.comStringOut, "TEC:T %d\r\n", (int)(parm.DesiredTemperature+.5) );
            cssl_putstring(parm.Serial, parm.comStringOut);

            usleep(200000);
            parm.Busy = false;
        }
    }
}
