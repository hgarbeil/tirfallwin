#ifdef _WIN32
#include "afxwin.h"
//#include "afxmt.h"
//#include "windows.h"
#endif
#include "configCosmos.h"
#ifdef __unix__
#include "cssl_lib.h"
#include <sys/time.h>
#endif
#include "calibrator.h"
#include "defines.h"
#include "tirthreads.h"
#include "math.h"
#include "iostream"
#include "string"

extern float DesiredTemperaturePaddleCold, DesiredTemperaturePaddleHot;
extern float DesiredTemperatureRotorCold, DesiredTemperatureRotorHot;

extern bool PresentPositionPaddleCold, PresentPositionPaddleHot, PresentPositionPaddleAmbient;
extern char PresentPositionRotor[64];

//using std
//#include "waxviewP2.h"
//#ifdef IMAGESNAPSHOT
//#include "CalibratordefinesC.h"
//#else
//#include "CalibratordefinesF.h"
//#endif

extern int IlluminatorStatus;

#define LSB7(x) (x & 0x7F)
#define USB7(x) ((x >> 7) & 0x7F)

#define int64_t long long

Calibrator::Calibrator()
{
    //printf("Starting BB %s\n" , port);
    parm.MonitorInitialized = false;
    parm.ControlInitialized = false;
    parm.Terminate = false;
    parm.Busy = false;
}

// We write 8 bit counts
int Calibrator::Celcius2Counts(float Celcius)
{
    //return(Celcius);
    // Do 3rd order regression on natural log (of counts) then raise to power of e.
    return( (int)(exp(4.93945518-0.051246464*Celcius+0.000161755*pow(Celcius,2)-0.000000388375*pow(Celcius,3))+0.5) );
    //return( (int)(exp(7.710282192-0.051028811*Celcius+0.000154295*pow(Celcius,2)-0.000000312751*pow(Celcius,3))+0.5) );
}

// We read 12 bit counts
float Calibrator::Counts2Celcius(int Counts)
{
    //return(Counts);
    // Take natural log of counts, do 3rd order regression on the natural log of counts vs temperature to get a good fit.
    //return( 138.231 - 39.03287568*log(Counts) + 2.75763298*pow(log(Counts),2) - 0.105465338*pow(log(Counts),3) );
    //return( 275.721 - 59.58214073*log(Counts) + 4.089222175*pow(log(Counts),2) - 0.129675063*pow(log(Counts),3) );
    return( (float)(299.081 - 68.4488*log(Counts) + 5.466188*pow(log(Counts),2) - 0.20528*pow(log(Counts),3) ) );
}


float Calibrator::HexstrToCelcius(char *HexStr)
{
    float Celcius;
    char *endPtr = 0;

    if( (strncmp(HexStr, "0xf", 3)==0) || (strncmp(HexStr, "0xF", 3)==0) ) {
        Celcius = (float)( (strtol(HexStr, &endPtr, 16) - 0x10000) * .0625 );
    } else {
        Celcius = (float)( strtol(HexStr, &endPtr, 16) * .0625 );
    }

    return(Celcius);
}


void Calibrator::Init(int port)
{
    Open(port);
    StartMonitor();
    StartControlPosPaddleCold();
    StartControlPosPaddleHot();
    StartControlPosPaddleAmbient();
    StartControlPosRotor();
    StartControlTempPaddleCold();
    StartControlTempPaddleHot();
    StartControlTempRotorCold();
    StartControlTempRotorHot();
    StartControlPowerPaddleCold();
    StartControlPowerPaddleHot();
    StartControlPowerRotorCold();
    StartControlPowerRotorHot();
}

void Calibrator::Open(int port)
{
    //DWORD nBytes;
    BOOL good;

    if (true) {
        // Open the port.
        good = parm.Serial.Open(port, TRUE, TRUE);
        if (!good) {
            CString Mess;
            Mess.Format(_T("Open Calibrator comm port FAILED (port %d)"), port);
            AfxMessageBox(Mess, MB_OK, 0);
        }
        else {
            parm.PortOpen = true;
            good = parm.Serial.SetConfiguration(L"baud=115200 parity=N data=8 stop=1");
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

    //:Oparm.Serial=cssl_open(port, 115200,8,0,1);
    /*:Oif (parm.Serial) {
        cssl_settimeout(parm.Serial, 0, 0.5);  // .5 second timeout.
        parm.PortOpen = true;
        strcpy(parm.Status, "Open");
    } else {
        parm.PortOpen = false;
        strcpy(parm.Status, "Error");
    }*/
    COSMOS_USLEEP(300000);
#endif

    //CalibratorGoHome();
    // Set positions instead of going home.
}

void Calibrator::Kill()
{
    parm.Terminate=true;
}

/*
void Calibrator::Illuminator(unsigned short *PresentPositionCh0, unsigned short *PresentPositionCh1)
{
    while(!parm.MonitorInitialized) COSMOS_USLEEP(100000);

    *PresentPositionCh0 = parm.PresentPositionCh0;
    *PresentPositionCh1 = parm.PresentPositionCh1;
}
*/


short Calibrator::Get(float *TemperaturePaddleCold, float *TemperaturePaddleHot,
                      float *TemperatureRotorCold, float *TemperatureRotorHot, float *TemperatureSensor)
{
    //int i;
    float Temperature;
    //while(!parm.MonitorInitialized) COSMOS_USLEEP(100000);
    if(!parm.MonitorInitialized) return(-99);


    Temperature = Counts2Celcius(parm.AdcValue[0]);
    if(Temperature<100) *TemperatureRotorCold = Temperature;
    Temperature = Counts2Celcius(parm.AdcValue[1]);
    if(Temperature<100) *TemperatureRotorHot = Temperature;
    Temperature = Counts2Celcius(parm.AdcValue[2]);
    if(Temperature<100) *TemperaturePaddleCold = Temperature;
    Temperature = Counts2Celcius(parm.AdcValue[3]);
    if(Temperature<100) *TemperaturePaddleHot = Temperature;

    //    *TemperatureRotorCold = parm.AdcValue[0];
    //    *TemperatureRotorHot = parm.AdcValue[1];
    //    *TemperaturePaddleCold = parm.AdcValue[2];
    //    *TemperaturePaddleHot = parm.AdcValue[3];

    TemperatureSensor[0] = parm.TemperatureSensor[4];  // Labeled as 6
    TemperatureSensor[1] = parm.TemperatureSensor[5];  // Labeled as 9
    TemperatureSensor[2] = parm.TemperatureSensor[6];  // Labeled as 11
    TemperatureSensor[3] = parm.TemperatureSensor[7];  // Labeled as 12
    TemperatureSensor[4] = parm.TemperatureSensor[14]; // Labeled as 13
    TemperatureSensor[5] = parm.TemperatureSensor[15]; // Labeled as 15

    return(0);
}

short Calibrator::Set(unsigned char DesiredChannel, float DesiredValue)
{
    //while(!parm.ControlInitialized) COSMOS_USLEEP(100000);
    if(!parm.ControlInitialized) return(-98);
    //return(0);

    switch(DesiredChannel) {
    case PADDLEPOSITIONCOLD:
        parm.SetPositionPaddleCold = (int)DesiredValue;
#ifdef __unix__
        sem_post (&parm.SetPositionPaddleCold_count);
#else
        SetEvent (parm.SetPositionPaddleCold_count);
#endif
        break;
    case PADDLEPOSITIONHOT:
        parm.SetPositionPaddleHot = (int)DesiredValue;
#ifdef __unix__
        sem_post (&parm.SetPositionPaddleHot_count);
#else
        SetEvent (parm.SetPositionPaddleHot_count);
#endif
        break;
    case PADDLEPOSITIONAMBIENT:
        parm.SetPositionPaddleAmbient = (int)DesiredValue;
#ifdef __unix__
        sem_post (&parm.SetPositionPaddleAmbient_count);
#else
        SetEvent (parm.SetPositionPaddleAmbient_count);
#endif
        break;
    case ROTORPOSITION:
        parm.SetPositionRotor = (int)DesiredValue;
#ifdef __unix__
        sem_post (&parm.SetPositionRotor_count);
#else
        SetEvent (parm.SetPositionRotor_count);
#endif
        break;
    case ROTORTEMPERATURECOLD:
        parm.DacValue[0] = Celcius2Counts(DesiredValue);
#ifdef __unix__
        sem_post (&parm.SetTemperatureRotorCold_count);
#else
        SetEvent (parm.SetTemperatureRotorCold_count);
#endif
        break;
    case ROTORTEMPERATUREHOT:
        parm.DacValue[1] = Celcius2Counts(DesiredValue);
#ifdef __unix__
        sem_post (&parm.SetTemperatureRotorHot_count);
#else
        SetEvent (parm.SetTemperatureRotorHot_count);
#endif
        break;
    case PADDLETEMPERATURECOLD:
        parm.DacValue[2] = Celcius2Counts(DesiredValue);
#ifdef __unix__
        sem_post (&parm.SetTemperaturePaddleCold_count);
#else
        SetEvent (parm.SetTemperaturePaddleCold_count);
#endif
        break;
    case PADDLETEMPERATUREHOT:
        parm.DacValue[3] = Celcius2Counts(DesiredValue);
#ifdef __unix__
        sem_post (&parm.SetTemperaturePaddleHot_count);
#else
        SetEvent (parm.SetTemperaturePaddleHot_count);
#endif
        break;

    case ROTORPOWERCOLD:
        parm.PowValue[0] = (int)DesiredValue;
#ifdef __unix__
        sem_post (&parm.SetPowerRotorCold_count);
#else
        SetEvent (parm.SetPowerRotorCold_count);
#endif
        break;
    case ROTORPOWERHOT:
        parm.PowValue[1] = (int)DesiredValue;
#ifdef __unix__
        sem_post (&parm.SetPowerRotorHot_count);
#else
        SetEvent (parm.SetPowerRotorHot_count);
#endif
        break;
    case PADDLEPOWERCOLD:
        parm.PowValue[2] = (int)DesiredValue;    // the variable names can be not arrays
#ifdef __unix__
        sem_post (&parm.SetPowerPaddleCold_count);
#else
        SetEvent (parm.SetPowerPaddleCold_count);
#endif
        break;
    case PADDLEPOWERHOT:
        parm.PowValue[3] = (int)DesiredValue;    // the variable names can be not arrays
#ifdef __unix__
        sem_post (&parm.SetPowerPaddleHot_count);
#else
        SetEvent (parm.SetPowerPaddleHot_count);
#endif
        break;
    }

    return(0);
}

/*void Calibrator::SetIlluminator(unsigned short OnOff, unsigned short PowerLevel)
{
    while(!parm.ControlInitialized) COSMOS_USLEEP(100000);

    //parm.DesiredChannel = DesiredChannel;
    //parm.OnOff = OnOff;
    //parm.PowerLevel = PowerLevel;
    sem_post (&parm.SetPosition7_count);
    if(PowerLevel==0) {
        IlluminatorStatus = 0;
    } else {
        IlluminatorStatus = 1;
    }
}
*/


void Calibrator::ReadComLine(int Timeout)
{
    DWORD nBytes;
#ifdef __unix__
    timeval startTime;
#else
    time_t startTime;
#endif

    COSMOS_USLEEP(20000);  // Wait for response to be available

    parm.comStringIn[0]=0;
    memset (parm.serialdata, 0, 128);
    tic(&startTime);
    while( (strstr(parm.comStringIn, "\n")==NULL) ) {
        if(toc(startTime)>Timeout) {
            //sprintf(LogMessage, "Timeout getting temperature: %s" , parm.name);
            //LogCmd(LogMessage);
            break;
        }
        *parm.serialdata = 0;
        //:OnBytes = cssl_getdata(parm.Serial, (uint8_t*)parm.serialdata, 1);
        parm.Serial.Read(parm.serialdata, 1, &nBytes);
        if(nBytes==1) {
            strcat(parm.comStringIn, parm.serialdata);
        }
        COSMOS_USLEEP(10000);
    }
}


void Calibrator::Monitor()
{
    int i;
    //char buffer[255];
    DWORD nBytes;
    //sem_init (&parm.GetPosition_count, 0, 0);
    //struct timeval startTime;
    //struct timeval PresentTime;
    //int64_t Duration, seconds, useconds;
    //return;
    while(!parm.Terminate) {
        //sem_wait (&parm.GetPosition_count);

        if(parm.PortOpen) {
            //If the Position is being set, skip getting the Position this time.
            if(!parm.Busy) {
                parm.Busy = true;

                // Clear out buffer
                //:Ocssl_settimeout(parm.Serial, 0, 0.1);
                //:Oi = cssl_getdata(parm.Serial, (uint8_t*)buffer, 512);
                //:Ocssl_settimeout(parm.Serial, 0, 0.5);

                parm.Serial.Read(parm.serialdata, 512, &nBytes);



                //Send command to get temperature.
                for(i=0 ; i<4 ; i++) {
                    sprintf(parm.comStringOut, "ADC %d\r\n" , i+1);
                    //:Ocssl_putstring(parm.Serial, parm.comStringOut);
                    parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                    /*                    parm.comStringIn[0]=0;
                    memset (parm.serialdata, 0, 128);
                    //tic(&startTime);
                    while( (strstr(parm.comStringIn, "\n")==NULL) ) {
                        //if(toc(startTime)>2) {
                        //    //sprintf(LogMessage, "Timeout getting temperature: %s" , parm.name);
                        //    //LogCmd(LogMessage);
                        //    break;
                       // }
                        *parm.serialdata = 0;
                        nBytes = cssl_getdata(parm.Serial, (uint8_t*)parm.serialdata, 1);
                        if(nBytes==1) {
                            strcat(parm.comStringIn, parm.serialdata);
                        }
                        COSMOS_USLEEP(10000);
                    }
*/
                    ReadComLine(10);

                    // Parse the temperature.
                    parm.NewLinePos = (strstr(parm.comStringIn, "\r"));
                    if(parm.NewLinePos!=NULL) {
                        *parm.NewLinePos = (char)0;
                    }
                    if(strlen(parm.comStringIn) > 0) {
                        parm.AdcValue[i] = atoi( &parm.comStringIn[6] );
                    } else {
                        parm.AdcValue[i] = -999;
                    }
                }


                //Send command to get temperature from sensors.
                for(i=0 ; i<16 ; i++) {
                    sprintf(parm.comStringOut, "TMP %d\r\n" , i+1);
                    //:Ocssl_putstring(parm.Serial, parm.comStringOut);
                    parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                    ReadComLine(10);

                    // Parse the temperature.
                    parm.NewLinePos = (strstr(parm.comStringIn, "\r"));
                    if(parm.NewLinePos!=NULL) {
                        *parm.NewLinePos = (char)0;
                    }
                    if(strlen(parm.comStringIn) > 0) {
                        parm.TemperatureSensor[i] = HexstrToCelcius( &parm.comStringIn[7] );
                    } else {
                        parm.TemperatureSensor[i] = -999;
                    }

                    if(parm.TemperatureSensor[i]==4095) parm.TemperatureSensor[i] = -99;
                }


                parm.MonitorInitialized = true;
                parm.Busy = false;
                COSMOS_USLEEP(500000);
                //printf("Position = %3.2f\r\n" , parm.PresentPosition);
            } else {
                COSMOS_USLEEP(200000);
            }
        }
    }
    //:Oif (parm.Serial) cssl_close(parm.Serial);
}


void Calibrator::ControlPosPaddleCold()
{
#ifdef __unix__
    sem_init (&parm.SetPositionPaddleCold_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetPositionPaddleCold_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    parm.ControlInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPositionPaddleCold_count);
#else
        WaitForSingleObject (parm.SetPositionPaddleCold_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the Position (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Set the Position.
            sprintf(parm.comStringOut, "COLD %d\r\n" , parm.SetPositionPaddleCold);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);
            // Read Reply (Ignore)
            ReadComLine(10);

            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Paddle Cold = %d", parm.SetPositionPaddleCold);
            //:PLogCmd(LogMessage);

            if(parm.SetPositionPaddleCold==1) {
                PresentPositionPaddleCold = 1;
            } else {
                PresentPositionPaddleCold = 0;
            }
        }
    }
}

void Calibrator::ControlPosPaddleHot()
{
#ifdef __unix__
    sem_init (&parm.SetPositionPaddleHot_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetPositionPaddleHot_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    //parm.ControlPaddleInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPositionPaddleHot_count);
#else
        WaitForSingleObject (parm.SetPositionPaddleHot_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the Position (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Set the Position.
            sprintf(parm.comStringOut, "HOT %d\r\n" , parm.SetPositionPaddleHot);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);

            // Read Reply (Ignore)
            ReadComLine(10);

            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Paddle Hot = %d", parm.SetPositionPaddleHot);
            //:PLogCmd(LogMessage);

            if(parm.SetPositionPaddleHot==1) {
                PresentPositionPaddleHot = 1;
            } else {
                PresentPositionPaddleHot = 0;
            }
        }
    }
}

void Calibrator::ControlPosPaddleAmbient()
{
    //struct timeval startTime;

#ifdef __unix__
    sem_init (&parm.SetPositionPaddleAmbient_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetPositionPaddleAmbient_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    //parm.ControlPaddleInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPositionPaddleAmbient_count);
#else
        WaitForSingleObject (parm.SetPositionPaddleAmbient_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if busy
            //tic(&startTime);
            while(parm.Busy) {
                COSMOS_USLEEP(100000);
                /*if(toc(startTime)>10) {
                    break;
                }*/
            }
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Set the Position.
            sprintf(parm.comStringOut, "AMB %d\r\n" , parm.SetPositionPaddleAmbient);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);

            // Read Reply (Ignore)
            ReadComLine(10);

            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Paddle Ambient = %d", parm.SetPositionPaddleAmbient);
            //:PLogCmd(LogMessage);

            if(parm.SetPositionPaddleAmbient==1) {
                PresentPositionPaddleAmbient = 1;
            } else {
                PresentPositionPaddleAmbient = 0;
            }
        }
    }
}




void Calibrator::ControlPosRotor()
{
    //struct timeval startTime;

    /*    // Move the rotor to Pass throuogh position
    sprintf(parm.comStringOut, "AMBC\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    //if(parm.SetPositionRotor!=ROTORAMBIENT)
        //gotoMotionDone

    COSMOS_USLEEP(5000000);
    sprintf(parm.comStringOut, "PASS\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    if(parm.SetPositionRotor==ROTORPASS)
        goto MotionDone;
*/


#ifdef __unix__
    sem_init (&parm.SetPositionRotor_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetPositionRotor_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    //parm.ControlPaddleInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPositionRotor_count);
#else
        WaitForSingleObject (parm.SetPositionRotor_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if busy
            //tic(&startTime);
            while(parm.Busy) {
                COSMOS_USLEEP(100000);
                /*if(toc(startTime)>10) {
                    break;
                }*/
            }
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Set the Position.
            switch(parm.SetPositionRotor) {
            case ROTORPASS:
                sprintf(parm.comStringOut, "AMBC\r\n");
                parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                ReadComLine(10);   // Read Reply (Ignore)

                COSMOS_USLEEP(5000000);
                sprintf(parm.comStringOut, "PASS\r\n");
                parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                ReadComLine(10);   // Read Reply (Ignore)
                strcpy(PresentPositionRotor, "Clear");
                break;
            case ROTORCOLD:
                sprintf(parm.comStringOut, "AMBC\r\n");
                parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                ReadComLine(10);   // Read Reply (Ignore)

                COSMOS_USLEEP(5000000);
                sprintf(parm.comStringOut, "COLDC\r\n");
                parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                ReadComLine(10);   // Read Reply (Ignore)
                strcpy(PresentPositionRotor, "Cold");
                break;
//            case ROTORHOT:
//                sprintf(parm.comStringOut, "HOTC\r\n");
//                break;
            case ROTORAMBIENT:
                sprintf(parm.comStringOut, "HOTC\r\n");
                parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                ReadComLine(10);   // Read Reply (Ignore)

                COSMOS_USLEEP(3000000);
                sprintf(parm.comStringOut, "AMBC\r\n");
                parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
                ReadComLine(10);   // Read Reply (Ignore)
                strcpy(PresentPositionRotor, "Amb");
                break;
            }
            //cssl_putstring(parm.Serial, parm.comStringOut);

            //            sprintf(parm.comStringOut, "AMBC\r\n");
            //            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            //            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //            //LogCmd(parm.comStringOut);
            //            ReadComLine(10);   // Read Reply (Ignore)
            //            //if(parm.SetPositionRotor!=ROTORAMBIENT)
            //            //gotoMotionDone

            //            COSMOS_USLEEP(5000000);
            //            sprintf(parm.comStringOut, "PASS\r\n");
            //            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            //            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //            //LogCmd(parm.comStringOut);
            //            ReadComLine(10);   // Read Reply (Ignore)
            //            if(parm.SetPositionRotor==ROTORPASS) {
            //                strcpy(PresentPositionRotor, "Clear");
            //                goto MotionDone;
            //            }

            //            COSMOS_USLEEP(3000000);
            //            sprintf(parm.comStringOut, "COLDC\r\n");
            //            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            //            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //            //LogCmd(parm.comStringOut);
            //            ReadComLine(10);   // Read Reply (Ignore)
            //            if(parm.SetPositionRotor==ROTORCOLD) {
            //                strcpy(PresentPositionRotor, "Cold");
            //                goto MotionDone;
            //            }

            //            COSMOS_USLEEP(2000000);
            //            sprintf(parm.comStringOut, "HOTC\r\n");
            //            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            //            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //            //LogCmd(parm.comStringOut);
            //            ReadComLine(10);   // Read Reply (Ignore)
            //            if(parm.SetPositionRotor==ROTORHOT) {
            //                strcpy(PresentPositionRotor, "Hot");
            //                goto MotionDone;
            //            }

            //            COSMOS_USLEEP(3000000);
            //            sprintf(parm.comStringOut, "AMBC\r\n");
            //            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            //            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //            //LogCmd(parm.comStringOut);
            //            ReadComLine(10);   // Read Reply (Ignore)
            //            strcpy(PresentPositionRotor, "Amb");

            //MotionDone:

            parm.Busy = false;

            //:PLogCmd(parm.comStringOut);
        }
    }
}

void Calibrator::ControlTempRotorCold()
{
    //return;
    /*    // Turn on.
    while(parm.Busy)
        COSMOS_USLEEP(100000);
    parm.Busy = true;
    COSMOS_USLEEP(100000);
    strcpy(parm.comStringOut, "TEC 1 1\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    ReadComLine(10);   // Read Reply (Ignore)
    parm.Busy = false;
*/
#ifdef __unix__
    sem_init (&parm.SetTemperatureRotorCold_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetTemperatureRotorCold_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    //parm.ControlInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetTemperatureRotorCold_count);
#else
        WaitForSingleObject (parm.SetTemperatureRotorCold_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the Position (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Set the Temperature.
            sprintf(parm.comStringOut, "DAC 1 %d\r\n" , parm.DacValue[0]);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);
            ReadComLine(10);   // Read Reply (Ignore)

            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Set rotor cold - (%d counts)" , parm.DacValue[0]);
            //:PLogCmd(LogMessage);
        }
    }
}

void Calibrator::ControlTempRotorHot()
{
    //return;
    /*    // Turn on.
    while(parm.Busy)
        COSMOS_USLEEP(100000);
    parm.Busy = true;
    COSMOS_USLEEP(100000);
    strcpy(parm.comStringOut, "TEC 2 1\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    ReadComLine(10);   // Read Reply (Ignore)
    parm.Busy = false;*/

#ifdef __unix__
    sem_init (&parm.SetTemperatureRotorHot_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetTemperatureRotorHot_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    //parm.ControlInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetTemperatureRotorHot_count);
#else
        WaitForSingleObject (parm.SetTemperatureRotorHot_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the Position (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Set the Temperature.
            sprintf(parm.comStringOut, "DAC 2 %d\r\n" , parm.DacValue[1]);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);
            ReadComLine(10);   // Read Reply (Ignore)

            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Set rotor hot - (%d counts)" , parm.DacValue[1]);
            //:PLogCmd(LogMessage);
        }
    }
}

void Calibrator::ControlTempPaddleCold()
{    
    //return;
    /*    // Turn on.
    while(parm.Busy)
        COSMOS_USLEEP(100000);
    parm.Busy = true;
    COSMOS_USLEEP(100000);
    strcpy(parm.comStringOut, "TEC 3 1\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    ReadComLine(10);   // Read Reply (Ignore)
    parm.Busy = false;
*/
#ifdef __unix__
    sem_init (&parm.SetTemperaturePaddleCold_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetTemperaturePaddleCold_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    //parm.ControlInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetTemperaturePaddleCold_count);
#else
        WaitForSingleObject (parm.SetTemperaturePaddleCold_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the Position (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Set the Temperature.
            sprintf(parm.comStringOut, "DAC 3 %d\r\n" , parm.DacValue[2]);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            // LogCmd(parm.comStringOut);
            ReadComLine(10);   // Read Reply (Ignore)

            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Set paddle cold - (%d counts)" , parm.DacValue[2]);
            //:PLogCmd(LogMessage);
        }
    }
}

void Calibrator::ControlTempPaddleHot()
{
    //return;
    /*    // Turn on.
    while(parm.Busy)
        COSMOS_USLEEP(100000);
    parm.Busy = true;
    COSMOS_USLEEP(100000);
    strcpy(parm.comStringOut, "TEC 4 1\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    ReadComLine(10);   // Read Reply (Ignore)*/

    /*
    strcpy(parm.comStringOut, "TEC 4\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    ReadComLine(50);   // Read Reply (Ignore)

    strcpy(parm.comStringOut, "TEC 4 1\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    ReadComLine(50);   // Read Reply (Ignore)

    strcpy(parm.comStringOut, "TEC 4\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    ReadComLine(50);   // Read Reply (Ignore)


    strcpy(parm.comStringOut, "DAC 4 33\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    ReadComLine(10);   // Read Reply (Ignore)

    strcpy(parm.comStringOut, "DAC 4\r\n");
    cssl_putstring(parm.Serial, parm.comStringOut);
    ReadComLine(10);   // Read Reply (Ignore)
*/
    parm.Busy = false;

#ifdef __unix__
    sem_init (&parm.SetTemperaturePaddleHot_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetTemperaturePaddleHot_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif
    //parm.ControlInitialized = true;

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetTemperaturePaddleHot_count);
#else
        WaitForSingleObject (parm.SetTemperaturePaddleHot_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the Position (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Set the Temperature.
            sprintf(parm.comStringOut, "DAC 4 %d\r\n" , parm.DacValue[3]);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);
            ReadComLine(10);   // Read Reply (Ignore)

            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Set paddle hot - (%d counts)" , parm.DacValue[3]);
            //:PLogCmd(LogMessage);
        }
    }
}

void Calibrator::ControlPowerRotorCold()
{
#ifdef __unix__
    sem_init (&parm.SetPowerRotorCold_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetPowerRotorCold_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPowerRotorCold_count);
#else
        WaitForSingleObject (parm.SetPowerRotorCold_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is busy (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Turn on/off cold rotor.
            sprintf(parm.comStringOut, "TEC 1 %d\r\n" , parm.PowValue[0]);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);
            ReadComLine(10);   // Read Reply (Ignore)
            parm.Busy = false;
            
            char LogMessage[256];
            sprintf(LogMessage, "Rotor Cold Power = %d", parm.PowValue[0]);
            //:PLogCmd(LogMessage);
        }
    }
}

void Calibrator::ControlPowerRotorHot()
{
#ifdef __unix__
    sem_init (&parm.SetPowerRotorHot_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetPowerRotorHot_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPowerRotorHot_count);
#else
        WaitForSingleObject (parm.SetPowerRotorHot_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is busy (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Turn on/off hot rotor.
            sprintf(parm.comStringOut, "TEC 2 %d\r\n" , parm.PowValue[1]);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);
            ReadComLine(10);   // Read Reply (Ignore)
            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Rotor Hot Power = %d", parm.PowValue[1]);
            //:PLogCmd(LogMessage);


            //            if(parm.PowValue[1] == POWERON) {
            //                // Shut off when paddle gets to temperature
            //                while(Counts2Celcius(parm.AdcValue[1]) < Counts2Celcius(parm.DacValue[1]))
            //                    ;
            //                // Turn off hot rotor.
            //                strcpy(parm.comStringOut, "TEC 2 0\r\n");
            //                cssl_putstring(parm.Serial, parm.comStringOut);
            //                sprintf(LogMessage, "Rotor Hot Power AutoOff");
            //                LogCmd(LogMessage);
            //            }
        }
    }
}

void Calibrator::ControlPowerPaddleCold()
{
#ifdef __unix__
    sem_init (&parm.SetPowerPaddleCold_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetPowerPaddleCold_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPowerPaddleCold_count);
#else
        WaitForSingleObject (parm.SetPowerPaddleCold_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is busy (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Turn on/off cold paddle.
            sprintf(parm.comStringOut, "TEC 3 %d\r\n" , parm.PowValue[2]);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);
            ReadComLine(10);   // Read Reply (Ignore)
            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Paddle Cold Power = %d", parm.PowValue[2]);
            //:PLogCmd(LogMessage);
        }
    }
}

void Calibrator::ControlPowerPaddleHot()
{
#ifdef __unix__
    sem_init (&parm.SetPowerPaddleHot_count, 0, 0);
#else
    DWORD nBytes;
    VERIFY(parm.SetPowerPaddleHot_count = CreateEvent(NULL, FALSE, FALSE, NULL));
#endif

    while(!parm.Terminate) {
#ifdef __unix__
        sem_wait (&parm.SetPowerPaddleHot_count);
#else
        WaitForSingleObject (parm.SetPowerPaddleHot_count, INFINITE);
#endif

        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is busy (put a timeout here)
            while(parm.Busy)
                COSMOS_USLEEP(100000);
            parm.Busy = true;
            COSMOS_USLEEP(100000);

            // Turn on/off hot paddle.
            sprintf(parm.comStringOut, "TEC 4 %d\r\n" , parm.PowValue[3]);
            //:Ocssl_putstring(parm.Serial, parm.comStringOut);
            parm.Serial.Write(parm.comStringOut, strlen(parm.comStringOut), &nBytes);
            //LogCmd(parm.comStringOut);
            ReadComLine(10);   // Read Reply (Ignore)
            parm.Busy = false;

            char LogMessage[256];
            sprintf(LogMessage, "Paddle Hot Power = %d", parm.PowValue[3]);
            //:PLogCmd(LogMessage);


            //            if(parm.PowValue[3] == POWERON) {
            //                // Shut off when paddle gets to temperature
            //                while(Counts2Celcius(parm.AdcValue[3]) < Counts2Celcius(parm.DacValue[3]))
            //                    ;
            //                // Turn off hot paddle.
            //                strcpy(parm.comStringOut, "TEC 4 0\r\n");
            //                cssl_putstring(parm.Serial, parm.comStringOut);
            //                sprintf(LogMessage, "Paddle Hot Power AutoOff");
            //                LogCmd(LogMessage);
            //            }
        }
    }
}


/*
void Calibrator::CalibratorGoHome(void)
{
    uint8_t comString[64];

    comString[0] = 0xA2;
    cssl_putdata(parm.Serial, comString, 1);
}

void Calibrator::SetCalibratorPos(unsigned char Channel, unsigned short Position)
{
    uint8_t comString[64];

    comString[0] = 0x84;
    comString[1] = Channel;
    comString[2] = LSB7(Position);
    comString[3] = USB7(Position);
    cssl_putdata(parm.Serial, comString, 4);
}

// 0 - 3968
void Calibrator::SetCalibratorSpeed(unsigned char Channel, unsigned short Speed)
{
    uint8_t comString[64];

    comString[0] = 0x87;
    comString[1] = Channel;
    comString[2] = LSB7(Speed);
    comString[3] = USB7(Speed);
    cssl_putdata(parm.Serial, comString, 4);
}

// 0 - 255
void Calibrator::SetCalibratorAcceleration(unsigned char Channel, unsigned short Acceleration)
{
    uint8_t comString[64];

    comString[0] = 0x89;
    comString[1] = Channel;
    comString[2] = LSB7(Acceleration);
    comString[3] = USB7(Acceleration);
    cssl_putdata(parm.Serial, comString, 4);
}

unsigned short Calibrator::GetCalibratorPos(unsigned char Channel)
{
    uint8_t comString[64];

    comString[0] = 0x90;
    comString[1] = Channel;
    cssl_putdata(parm.Serial, comString, 2);
    COSMOS_USLEEP(50000);
    cssl_getdata(parm.Serial, comString, 2);
    return( (unsigned char)comString[1]*256 + (unsigned char)comString[0] );
}

void Calibrator::IlluminatorControl(unsigned short OnOff, unsigned short PowerLevel)
{
    unsigned char Channel = 3;
    uint8_t comString[64];

    comString[0] = 0x91;
    comString[1] = Channel;
    comString[2] = OnOff;
    comString[3] = PowerLevel;
    cssl_putdata(parm.Serial, comString, 4);
}

void Calibrator::SetNewHome(unsigned char Channel, unsigned short StartupPos)
{
    uint8_t comString[64];

    comString[0] = 0xC0;
    comString[1] = Channel;
    comString[2] = LSB7(StartupPos);
    comString[3] = USB7(StartupPos);
    cssl_putdata(parm.Serial, comString, 4);
}*/
