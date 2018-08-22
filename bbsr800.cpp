#ifdef _WIN32
#include "afxwin.h"
//#include "windows.h"
//#include "afxmt.h"
#endif
//:O#include "cssl_lib.h"
#include "configCosmos.h"

#include "bbsr800.h"
#include "tirthreads.h"
//extern char LogMessage[512];


BbSr800::BbSr800()
{
    //printf("Starting BB %s\n" , port);
    parm.MonitorInitialized = false;
    parm.ControlInitialized = false;
    parm.YesTemperatureStable = false;
    parm.Terminate = false;
    parm.Busy = false;
}


void BbSr800::Init(int port, /*int baud,*/ char *name)
{
    strcpy(parm.name, name);
    Open(port/*, baud*/);
    StartMonitor();
    StartControl();
}

void BbSr800::Open(int port/*, int baud*/)
{
    BOOL good;

    if (true) {
        // Open the port.
        good = parm.Serial.Open(port, TRUE, TRUE);
        if (!good) {
            CString Mess;
            Mess.Format(_T("Open Ext BB comm port FAILED (port %d)"), port);
            AfxMessageBox(Mess, MB_OK, 0);
            //m_bUseBBUnit1 = FALSE;
        }
        else {
            parm.PortOpen = true;
            good = parm.Serial.SetConfiguration(L"baud=9600 parity=N data=8 stop=1");
            if (!good) {
                //AfxMessageBox ("Set Configuration did not work");
                //m_bUseBBUnit1 = FALSE
                ;
            }
            else {
                good = parm.Serial.SetTimeouts(500, 500, 500);
                if (!good) {
                    //MessageBox ("SetTimeouts did not work");
                    //m_bUseBBUnit1 = FALSE
                    ;
                }
            }
        }
    }

#ifdef LINUXSTUFF
    //:Ocssl_start();
    //    if(strstr(parm.name, "BbUnitB")!=NULL)
    //        parm.Serial=cssl_open(port, 0, 38400,8,0,1);
    //    else
    //:Oparm.Serial=cssl_open(port, baud,8,0,1);

    /*:O    if (parm.Serial) {
        cssl_settimeout(parm.Serial, 0, 0.5);  // .5 second timeout.
        parm.PortOpen = true;
        strcpy(parm.Status, "Open");
    } else {
        //sprintf(LogMessage, "Can't Open Temperature Port: %s" , parm.name);
        //LogCmd(LogMessage);
        parm.PortOpen = false;
        strcpy(parm.Status, "Error");
    }*/
    COSMOS_USLEEP(400000);
    //parm.Serial=cssl_open(port, 0, baud,8,0,1);
    /*
    //Send command to get version.
    strcpy(comStringOut, "\r\nVER?\r\n");
    cssl_putstring(Serial, comStringOut);
    COSMOS_USLEEP(500000);

    // Get the version
    memset (comStringIn, 0, 128);
    cssl_getdata(Serial, (uint8_t*)comStringIn, nBytes);
    COSMOS_USLEEP(500000);
*/
    //Send command to turn on output.
    //    strcpy(parm.comStringOut, "\r\nTEC:OUT 1\r\n");
    //    cssl_putstring(parm.Serial, parm.comStringOut);
    //    COSMOS_USLEEP(300000);
#endif
}

void BbSr800::Kill()
{
    //Set(25);
    //COSMOS_USLEEP(100000);
    parm.Terminate=true;
}


float BbSr800::Get()
{
    //return(0.0);
    //    while(!parm.MonitorInitialized)
    //        COSMOS_USLEEP(1000000);
    if(!parm.MonitorInitialized) return(-99);
    return(parm.PresentTemperature);
}

BOOL BbSr800::IsStable()
{
    if(!parm.MonitorInitialized) return(FALSE);
    return(parm.YesTemperatureStable);
}

short BbSr800::Set(float DesiredTemperature)
{
    if(DesiredTemperature == parm.DesiredTemperature) return(0);

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // This needs timeout and way to reconnect!!!!!!
    // And maybe separate thread
    //while(!parm.ControlInitialized) COSMOS_USLEEP(100000);
    if(!parm.ControlInitialized) return(-98);

    parm.DesiredTemperature = DesiredTemperature;
#ifdef __unix__
    sem_post (&parm.SetTemperature_count);
#else
    ::SetEvent(parm.SetTemperature_count);
#endif

    return(0);
}


void BbSr800::Monitor()
{
    DWORD nBytes;
    //int nBytes;
    //char buffer[256];
#ifdef __unix__
    timeval startTime;
#else
    //time_t startTime;
#endif

    //sem_init (&parm.GetTemperature_count, 0, 0);

    while(!parm.Terminate) {
        //sem_wait (&parm.GetTemperature_count);

        if(parm.PortOpen) {
            //If the temperature is being set, skip getting the temperature this time.
            if(!parm.Busy) {
                parm.Busy = true;


                // Clear out buffer
                /*:O                cssl_settimeout(parm.Serial, 0, 0.1);
                cssl_getdata(parm.Serial, (uint8_t*)buffer, 250);
                cssl_settimeout(parm.Serial, 0, 0.5);*/


                strcpy(parm.comStringOut, "ISTEMPERATURESTABLE\r\n");
                parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                COSMOS_USLEEP(100000);

                // Get the STABLE result
                parm.comStringIn[0]=0;
                memset (parm.serialdata, 0, 128);
                while( (strstr(parm.comStringIn, "\n")==NULL) ) {
                    *parm.serialdata = 0;
                    //:OnBytes = cssl_getdata(parm.Serial, (uint8_t*)parm.serialdata, 1);
                    parm.Serial.Read(parm.serialdata, 1, &nBytes);
                    if(nBytes==1) {
                        strcat(parm.comStringIn, parm.serialdata);
                    }
                    COSMOS_USLEEP(10000);
                }

                if(strncmp(parm.comStringIn, "1", 1) == 0) {
                    parm.YesTemperatureStable = true;
                } else {
                    parm.YesTemperatureStable = false;
                }


                //Send command to get temperature.
                strcpy(parm.comStringOut, "GETTEMPERATURE\r\n");
                //:Ocssl_putstring(parm.Serial, parm.comStringOut);*/
                parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                COSMOS_USLEEP(100000);

                // Get the Temperature
                parm.comStringIn[0]=0;
                memset (parm.serialdata, 0, 128);
                //                tic(&startTime);
                while( (strstr(parm.comStringIn, "\n")==NULL) ) {
                    /*                    if(toc(startTime)>4) {
                        sprintf(LogMessage, "Timeout getting temperature: %s" , parm.name);
                        LogCmd(LogMessage);
                        break;
                    }*/
                    *parm.serialdata = 0;
                    //:OnBytes = cssl_getdata(parm.Serial, (uint8_t*)parm.serialdata, 1);
                    parm.Serial.Read(parm.serialdata, 1, &nBytes);
                    if(nBytes==1) {
                        strcat(parm.comStringIn, parm.serialdata);
                    }
                    COSMOS_USLEEP(10000);
                }

                // Parse the temperature.
                parm.NewLinePos = (strstr(parm.comStringIn, "\r"));
                if(parm.NewLinePos!=NULL) {
                    *parm.NewLinePos = (char)0;
                }
                if(strlen(parm.comStringIn) > 0) {
                    parm.PresentTemperature = (float)atof( parm.comStringIn );
                } else {
                    parm.PresentTemperature = -999;
                }

                parm.MonitorInitialized = true;
                parm.Busy = false;
                COSMOS_USLEEP(400000);
                //printf("Temperature = %3.2f\n" , parm.PresentTemperature);
            } else {
                COSMOS_USLEEP(200000);
            }
        } else {
            COSMOS_USLEEP(2000000);
        }
    }

    //:Oif (parm.Serial) cssl_close(parm.Serial);
}

void BbSr800::Control()
{
    DWORD nBytes;

#ifdef __unix__
    sem_init (&parm.SetTemperature_count, 0, 0);
#else
    VERIFY(parm.SetTemperature_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    parm.ControlInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetTemperature_count);
#else
        WaitForSingleObject(parm.SetTemperature_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the temperature (put a timeout here)
            while(parm.Busy) {
                COSMOS_USLEEP(100000);
                if(parm.Terminate) return;
            }
            parm.Busy = true;
            if(parm.Terminate) break;

            // Set the temperature.
            //sprintf (comStringOut, "TEC:T %d;TEC:OUT 1\r\n", (int)DesiredTemperature0);
            sprintf( parm.comStringOut, "SETTEMPERATURE %d\r\n", (int)(parm.DesiredTemperature+.5) );
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            //:OCOSMOS_USLEEP(400000);
            //cssl_putstring(parm.Serial, parm.comStringOut);
            //COSMOS_USLEEP(200000);

            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);

            parm.Busy = false;

            //:O char LogMessage[256];
            //:O sprintf(LogMessage, "Set external BB - %f", parm.DesiredTemperature);
            //:PLogCmd(LogMessage);
        }
    }
}
