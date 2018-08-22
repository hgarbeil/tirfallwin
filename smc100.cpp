#ifdef _WIN32
#include "afxwin.h"
//#include "windows.h"
//#include "afxmt.h"
#endif
//:O#include "cssl_lib.h"
#include "configCosmos.h"

#include "smc100.h"
#include "tirthreads.h"
extern float RotationHome;
extern float GoniometerHome;

Smc100::Smc100()
{
    //printf("Starting BB %s\n" , port);
    parm.MonitorInitialized = false;
    parm.ControlInitialized = false;
    parm.Terminate = false;
    parm.Busy = false;
}


void Smc100::Init(int port)
{    
    Open(port);
    StartMonitor();
    StartControlRotation();
    StartControlGoniometer();
}

void Smc100::Open(int port)
{
    DWORD nBytes;
    BOOL good;

    if (true) {
        // Open the port.
        good = parm.Serial.Open(port, TRUE, TRUE);
        if (!good) {
            CString Mess;
            Mess.Format(_T("Open Motion Stage com port FAILED (port %d)"), port);
            AfxMessageBox(Mess, MB_OK, 0);
        }
        else {
            parm.PortOpen = true;
            good = parm.Serial.SetConfiguration(L"baud=19200 parity=N data=8 stop=1");
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

    /*:Oparm.Serial=cssl_open(port, 19200,8,0,1);
    if (parm.Serial) {
        cssl_settimeout(parm.Serial, 0, 0.5);  // .5 second timeout.
        parm.PortOpen = true;
        strcpy(parm.Status, "Open");
    } else {
        parm.PortOpen = false;
        strcpy(parm.Status, "Error");
    }*/
#endif
    COSMOS_USLEEP(300000);

    //Turn on ESP100 (remove this for smc-100).
    strcpy(parm.comStringOut, "1MO\r\n");
    //:Ocssl_putstring(parm.Serial, parm.comStringOut);
    parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
    strcpy(parm.comStringOut, "2MO\r\n");
    //:Ocssl_putstring(parm.Serial, parm.comStringOut);
    parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);

    Home(0);
    Home(1);
}

void Smc100::Kill()
{
    parm.Terminate=true;
    //:Ocssl_close(parm.Serial);
}


void Smc100::Halt()
{
    //Send command to halt.
    strcpy(parm.comStringOut, "1ST\r\n");
    //:Ocssl_putstring(parm.Serial, parm.comStringOut);
    COSMOS_USLEEP(300000);
}

void Smc100::Home(int ChannelNum)
{
    DWORD nBytes;
    // send command to turn on
    sprintf(parm.comStringOut, "%dMO\r\n" , ChannelNum+1);
    //:Ocssl_putstring(parm.Serial, parm.comStringOut);
    parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);

    //Send command to home.
    //strcpy(parm.comStringOut, "1OR?\r\n");
    sprintf(parm.comStringOut, "%dOR1\r\n" , ChannelNum+1);
    //:Ocssl_putstring(parm.Serial, parm.comStringOut);
    parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
}

float Smc100::Get(int ChannelNum)
{
    //while(!parm.MonitorInitialized) COSMOS_USLEEP(100000);
    if(!parm.MonitorInitialized) return(-999);
    if(ChannelNum==0) {
        return(parm.PresentPositionCh0);
    } else {
        return(parm.PresentPositionCh1);
    }
}

void Smc100::Set(float DesiredSpeed, float DesiredPosition, int ChannelNum)
{
    //while(!parm.ControlInitialized) COSMOS_USLEEP(100000);
    if(!parm.ControlInitialized) return;

    if(ChannelNum==0) {
        parm.DesiredSpeedRotation = DesiredSpeed;
        if(DesiredPosition>30) DesiredPosition = 30 + RotationHome;
        if(DesiredPosition<-76) DesiredPosition = -76 + RotationHome;
        parm.DesiredPositionRotation = DesiredPosition;
#ifdef __unix__
        sem_post (&parm.SetPositionRotation_count);
#else
        ::SetEvent(parm.SetPositionRotation_count);
#endif
        COSMOS_USLEEP(50000);
    } else {
        parm.DesiredSpeedGoniometer = DesiredSpeed;
        if(DesiredPosition>4) DesiredPosition = 4 + GoniometerHome;
        if(DesiredPosition<-10) DesiredPosition = -10 + GoniometerHome;
        parm.DesiredPositionGoniometer = DesiredPosition;
#ifdef __unix__
        sem_post (&parm.SetPositionGoniometer_count);
#else
        ::SetEvent(parm.SetPositionGoniometer_count);
#endif
        COSMOS_USLEEP(50000);
    }
}


float Smc100::GetPosition(int ChannelNum/*:O, type_StageInfo parm*/)
{
    DWORD nBytes;
    int length;
    char buffer[256];
    float PresentPosition;
    parm.comStringIn[0]=0;
    memset (parm.serialdata, 0, 128);

    //Send command to get Position.
    //strcpy(parm.comStringOut, "1TP\r\n");
    sprintf(parm.comStringOut, "%dTP\r\n" , ChannelNum+1);
    //:Ocssl_putstring(parm.Serial, parm.comStringOut);
    parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
    COSMOS_USLEEP(100000);

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! PUT TIMEOUT HERE !!!!!!!!!!!!!!!!!
    // Get the Position
    while( (strstr(parm.comStringIn, "\n")==NULL) ) {
        //:Ocssl_getdata(parm.Serial, (uint8_t*)parm.serialdata, 1);
        parm.Serial.Read(parm.serialdata, 1, &nBytes);
        strcat(parm.comStringIn, parm.serialdata);

        COSMOS_USLEEP(10000);
    }

    // Parse the Position.
    if( strlen(parm.comStringIn)>6 ) {
        strcpy(buffer, &parm.comStringIn[0]);
        length = strlen( buffer ) - 2;
        buffer[length] = '\0';
        PresentPosition = (float)atof(buffer);	//SMC100
    }
    else {
        PresentPosition = 9999;
    }

    return(PresentPosition);
}


void Smc100::Monitor()
{
    //sem_init (&parm.GetPosition_count, 0, 0);

    while(!parm.Terminate) {
        //sem_wait (&parm.GetPosition_count);

        if(parm.PortOpen) {
            //If the Position is being set, skip getting the Position this time.
            if(!parm.Busy) {
                parm.Busy = true;
                parm.PresentPositionCh0 = GetPosition(1/*, parm*/);
                parm.PresentPositionCh1 = GetPosition(0/*, parm*/);
                parm.MonitorInitialized = true;
                parm.Busy = false;
                COSMOS_USLEEP(1000000);
            }
        } else {
            COSMOS_USLEEP(2000000);
        }
    }
    //:Oif (parm.Serial) cssl_close(parm.Serial);
}

void Smc100::ControlRotation()
{
    DWORD nBytes;
    //int ChannelNum = 1;
    int ChannelNum = 2;
#ifdef __unix__
    sem_init (&parm.SetPositionRotation_count, 0, 0);
#else
    VERIFY(parm.SetPositionRotation_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    parm.ControlInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPositionRotation_count);
#else
        WaitForSingleObject(parm.SetPositionRotation_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the Position (put a timeout here)
            while(parm.Busy) {
                COSMOS_USLEEP(50000);
                if(parm.Terminate) return;
            }
            parm.Busy = true;
            if(parm.Terminate) break;

            // Set the speed.
            sprintf (parm.comStringOut, "%dVA%.2f\r\n", ChannelNum, parm.DesiredSpeedRotation);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);

            COSMOS_USLEEP(50000);

            // Set the Position.
            sprintf (parm.comStringOut, "%dPA%.3f\r\n", ChannelNum, parm.DesiredPositionRotation);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut); //Rotation
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);

            COSMOS_USLEEP(100000);
        }
        parm.Busy = false;

        char LogMessage[256];
        sprintf(LogMessage, "Set Rotation Stage: %f", parm.DesiredPositionRotation-RotationHome);
        //:PLogCmd(LogMessage);
    }
}

void Smc100::ControlGoniometer()
{
    DWORD nBytes;
    //

    //int ChannelNum = 2;
    int ChannelNum = 1;
#ifdef __unix__
    sem_init (&parm.SetPositionGoniometer_count, 0, 0);
#else
    VERIFY(parm.SetPositionGoniometer_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    parm.ControlInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPositionGoniometer_count);
#else
        WaitForSingleObject(parm.SetPositionGoniometer_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the Position (put a timeout here)
            while(parm.Busy) {
                COSMOS_USLEEP(50000);
                if(parm.Terminate) return;
            }
            parm.Busy = true;

            // Set the speed.
            sprintf (parm.comStringOut, "%dVA%.2f\r\n", ChannelNum, parm.DesiredSpeedGoniometer);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);

            COSMOS_USLEEP(50000);

            // Set the Position.
            sprintf (parm.comStringOut, "%dPA%.3f\r\n", ChannelNum, parm.DesiredPositionGoniometer);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);  //Goniometer
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);

            parm.Busy = false;
            COSMOS_USLEEP(100000);

            char LogMessage[256];
            sprintf(LogMessage, "Set Goniometer: %f", parm.DesiredPositionGoniometer-GoniometerHome);
            //:PLogCmd(LogMessage);
        } else {
            COSMOS_USLEEP(2000000);

        }
    }
}
