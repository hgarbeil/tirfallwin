#ifdef _WIN32
#include "afxwin.h"
#endif
#include <QDebug>
#include "configCosmos.h"
#include "cmigitsasc.h"
#include "mainwindow.h"

#define GPS_FINEALIGN 1
#define GPS_AIRALIGN 2

extern char LogMessage[512];
extern char MyGpsLine[];
extern int GpsPortNum;

extern bool SaveOperationWriting;

struct
{
    unsigned short m_Sync, m_ID, m_Count, m_Flags, m_Checksum;
} sHeader;

struct
{
    int GPSTime1, GPSTime2, UTCTime1, UTCTime2, Latitude, Longitude, Altitude;
    int Pos_x, Pos_y, Pos_z, Vel_E, Vel_N, Vel_U, Acc_E, Acc_N, Acc_U;
    int Pitch, Roll, Heading;
    short Mode, Status, NumSV, NumPos, NumVel, FOM;
    int HorPosErr, VerPosErr, VelErr;
    short EquipmentAvail, EquipmentUsed, MeasState1, CNo1, MeasState2, CNo2, MeasState3, CNo3;
    short MeasState4, CNo4, MeasState5, CNo5;
    unsigned short m_Checksum;
} sTimeMark;

struct
{
    int Time1, Time2, Latitude, Longitude, Altitude, Vel_N, Vel_E, Vel_U, Pitch, Roll, Heading;
    unsigned short m_Checksum;
} sNavSolution;

struct	// Message 3512
{
    unsigned long Time1, Time2;
    long Pitch, Roll, Heading, Vel_N, Vel_E, Vel_U;
    unsigned short m_Checksum;
} sFlightControl;


typedef struct {           /* standard IP Block header words         */
   short     soh_del;      /* should be 81FFh                        */
   short     msg_id;       /* Message ID                             */
   short     word_cnt;     /* Data word count                        */
   unsigned short flags;   /* should be 9200h                        */
   short     hdr_chksum;   /* header 2's complement checksum         */
} MSG_HDR;


CMigitsA::CMigitsA()
{
    parm.MonitorInitialized = false;
    parm.Terminate = false;
    //parm.Busy = false;
}


void CMigitsA::Init(void)
{
    char buffer[512];
    int port=GpsPortNum;
    Open(port);

    StartMonitor();
}

void CMigitsA::Backwards()
{
    int i;
    unsigned short summation;
    MSG_HDR message;
    DWORD nBytes;
    unsigned short dat[23];

    //////////////////////////////////////////////////////////////////////////////////
    //Send 3511 Configuration Control message to tell the C-Migit that it is upside down.
    message.soh_del = (short)0x81ff;
    message.msg_id = 3511;
    message.word_cnt = 22;
    message.flags = 0x8000;
    message.hdr_chksum = -((signed)message.soh_del + (signed)message.msg_id +
            (signed)message.word_cnt + (signed int)message.flags);

    // Copy data into buffer
    parm.Serial.Write(&message, 10, &nBytes);

    //  Initialize dat buffer
    for (i = 0; i < 22; i++) dat[i] = 0;

    // Data Validity, bit0=mode command valid, bit1=lat/long/alt valid, bit5=auto align/nav, bit6=gpsinit valid
    dat[0] = 1;
    if( !parm.GpsBackwards ) {   //Upside down only
        dat[2]  = short(0x4000);
        dat[10] = short(0xC000);
        dat[18] = short(0xC000);
    }
    else {      //Upside down and backwards
        dat[2]  = short(0xC000);
        dat[10] = short(0x4000);
        dat[18] = 0xC000;
    }
    //  Calulate the checksum
    summation = 0;
    for (i = 0; i < 22 ; i++) {
        summation += (signed)dat[i];
    }
    dat[22] = -(summation);

    // Copy data into buffer
    parm.Serial.Write(&dat, 46, &nBytes);
}

void CMigitsA::GroundAlign()
{
    AlignmentMode = GPS_FINEALIGN;
    Backwards();
    Sleep(100);
    CMigitsAlign();
}

void CMigitsA::AirAlign()
{
    AlignmentMode = GPS_AIRALIGN;
    Backwards();
    Sleep(100);
    CMigitsAlign();
}

void CMigitsA::CMigitsAlign()
{
    int i;
    unsigned short summation;
    MSG_HDR message;
    DWORD nBytes;
    unsigned short dat[23];
    time_t ltime;
    struct tm *nt;

    //////////////////////////////////////////////////////////////////////////////////
    //Set to Auto Align/Nav Sequence.
    message.soh_del = (short)0x81ff;
    message.msg_id = 3510;
    message.word_cnt = 21;
    message.flags = 0x9200;
    message.hdr_chksum = -((signed)message.soh_del + (signed)message.msg_id +
            (signed)message.word_cnt + (signed int)message.flags);

    // Copy data into buffer

    parm.Serial.Write(&message, 10, &nBytes);

    //  Initialize dat buffer
    for (i = 0; i < 21; i++) dat[i] = 0;

    int degrees = (int)parm.LatitudeInit;
    int minutes = (int)((parm.LatitudeInit-degrees) * 60);
    int seconds = (int)( ( ((parm.LatitudeInit-degrees) * 60) - minutes ) * 60 + .5);

    dat[2] = degrees;			// lat deg
    dat[3] = minutes;			// lat min
    dat[4] = seconds;			// lat sec

    degrees = (int)parm.LongitudeInit;
    minutes = (int)((parm.LongitudeInit-degrees) * 60);
    seconds = (int)( ( ((parm.LongitudeInit-degrees) * 60) - minutes ) * 60 + .5);

    dat[5] = degrees;				// long deg
    dat[6] = minutes;				// long min
    dat[7] = seconds;				// long sec


    time (&ltime);
    nt = gmtime(&ltime);
    dat[12] = nt->tm_year;		// year
    dat[13] = nt->tm_yday + 1;	// day of year
    dat[14] = nt->tm_hour;		// hours
    dat[15] = nt->tm_min;		// minutes
    dat[16] = nt->tm_sec;		// seconds

    dat[17] = (int)(parm.Heading * 100 + .5);		// heading times 100



    switch( AlignmentMode ) {
        case GPS_FINEALIGN:
            //Fine Align Code
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Data Validity, bit0=mode command valid, bit1=lat/long/alt valid, bit3=Date/Time valid, bit4=Heading valid
            //bit5=auto align/nav sequence valid, bit6=Auto GPS init valid

            dat[0] = 0x7B;	//0111 1011

            dat[8] = parm.AltitudeInit & 0xFF;			// alt 16 bit coded value LS 8 bits
            dat[9] = (parm.AltitudeInit & 0xFF00) >> 8;	// alt in meters MS 8 bits

            //0000 0001 0111 0100 (hex174) = decimal372  1=Auto GPS init  7=Air Navigation  4=Fine Alignment
            dat[18] = 0x174;	//changed to 0x172 (372) 4/4/04  (was 181)			// auto align/nav
            break;

        case GPS_AIRALIGN:
            //Fine Align Code
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Data Validity, bit0=mode command valid, bit1=lat/long/alt valid, bit3=Date/Time valid, bit4=Heading valid
            //bit5=auto align/nav sequence valid, bit6=Auto GPS init valid

            dat[0] = 0x7B;	//0111 1011

            dat[8] = parm.AltitudeInit & 0xFF;			// alt 16 bit coded value LS 8 bits
            dat[9] = (parm.AltitudeInit & 0xFF00) >> 8;	// alt in meters MS 8 bits

            //0000 0001 0111 0100 (hex175) = decimal372  1=Auto GPS init  7=Air Navigation  5=Air Alignment
            dat[18] = 0x175;	//changed to 0x172 (372) 4/4/04  (was 181)			// auto align/nav
            break;
    }

    //  Calulate the checksum
    summation = 0;
    for (i = 0; i < 21; i++) {
        summation += (signed)dat[i];
    }
    dat[21] = -(summation);

    // Copy data into buffer
    parm.Serial.Write(&dat, 44, &nBytes);
}

void CMigitsA::Open(int port)
{
    BOOL good;

    // Open the port.
    good = parm.Serial.Open(port, TRUE, TRUE);
    if (!good) {
        CString Mess;
        Mess.Format(_T("Open GPS comm port FAILED (port %d)"), port);
        AfxMessageBox(Mess, MB_OK, 0);
    }
    else {
        parm.PortOpen = true;
        good = parm.Serial.SetConfiguration(L"baud=115200 parity=O data=8 stop=1");
        if (!good) {
            //AfxMessageBox ("Set Configuration did not work");
            ;
        }
        else {
            good = parm.Serial.SetTimeouts(0, 100, 100);
            if (!good) {
                //MessageBox ("SetTimeouts did not work");
                ;
            }
        }
    }
}

void CMigitsA::Kill()
{
    parm.Terminate=true;
}


void CMigitsA::Monitor()
{
    //char MyGpsLinel[512];
    char buffer[512];
    char bufferRemainder[512];
    char *NewLinePos;
    int StrPos;
    //timeval startTime;
    DWORD nBytes;
    DWORD TotalBytes;
    //timeval MyStartTime;
    //timeval MyStopTime;
    //int MyInt1, MyInt2;
    unsigned long LengthOfString;
    int CharPtr;
    int count;
    //char lastChk;
    char presentChk;
    int bufptr;
    unsigned char CmigitsData[BUFSIZE];
    double RollSemiCirc;

    //sem_init (&parm.GetTemperature_count, 0, 0);

    if(parm.PortOpen) {


        if(AlignmentMode == GPS_AIRALIGN) {
            AirAlign();
        } else {
            GroundAlign();
        }

        *parm.comStringIn = 0;
        *bufferRemainder = 0;

        while(!parm.Terminate) {
//            // Either Initializing, or something went wrong, read until we get to the end of a message
//            StrPos = strlen(parm.comStringIn)-1;
//            if(StrPos<0) StrPos = 0;

            CmigitsData[0] = 0;
            *parm.serialdata = 0;
            //qDebug() << "Start";
            // Get the Message start code
            //while( (CmigitsData[0]!=0xFF) || (CmigitsData[1]!=0x81) || (CmigitsData[2]!=0xB8) || (CmigitsData[3]!=0x0D) ) {
            while(*(short*)CmigitsData != (short)SYNCWORD) {

                bufptr = 0;
                nBytes = 0;
                while( CmigitsData[0] != 0xFF ) {
                    nBytes = 0;
                    while( nBytes < 1 ) {
                        //parm.Serial.Read(CmigitsData, 1, &nBytes);
                        parm.Serial.Read(CmigitsData, 1, &nBytes);
                        if(nBytes<1) Sleep(10);
                    }
                }
                //memcpy(CmigitsData, &parm.serialdata, nBytes);
                bufptr += nBytes;

                while(bufptr<2) {
                    nBytes = 0;
                    while( nBytes < 1 ) {
                        //parm.Serial.Read(parm.serialdata, 1, &nBytes);
                        parm.Serial.Read(parm.serialdata, 1, &nBytes);
                        if(nBytes<1) Sleep(10);
                    }
                    memcpy(CmigitsData+bufptr, parm.serialdata, nBytes);
                    bufptr += nBytes;
                }
            }

            // Get the header
            while(bufptr<HEADER_SIZE) {
                TotalBytes = 2;
                nBytes = 0;
                //while( nBytes < 1 ) {
                    //parm.Serial.Read(parm.serialdata, 1, &nBytes);
                    parm.Serial.Read(parm.serialdata, HEADER_SIZE-TotalBytes, &nBytes);
                    //if(nBytes<1) Sleep(10);
                //}
                memcpy(CmigitsData+bufptr, parm.serialdata, nBytes);
                bufptr += nBytes;
                TotalBytes += nBytes;
            }

            memcpy( &sHeader.m_Sync, CmigitsData, HEADER_SIZE );
            //printf("ChkSum=%d" , sHeader.m_Checksum); fflush(stdout);

            //Check header checksum
            if( sHeader.m_Checksum == (0x10000 - sHeader.m_Sync - sHeader.m_ID - sHeader.m_Count - sHeader.m_Flags) | 0xFFFF ) {


                switch (sHeader.m_ID) {
                case TIMEMARK_ID:	//MSG 3
                    parm.RunningToggle = !parm.RunningToggle;
                    TotalBytes = 0;
                    while(bufptr<HEADER_SIZE+TIMEMARK_SIZE) {
                        nBytes = 0;
                        //while( nBytes < 1 ) {
                            //parm.Serial.Read(parm.serialdata, 1, &nBytes);
                            parm.Serial.Read(parm.serialdata, TIMEMARK_SIZE-TotalBytes, &nBytes);
                            //if(nBytes<1) Sleep(10);
                        //}
                        memcpy(CmigitsData+bufptr, parm.serialdata, nBytes);
                        bufptr += nBytes;
                        TotalBytes += nBytes;
                    }
                    memcpy( &sTimeMark.GPSTime1, CmigitsData+HEADER_SIZE, TIMEMARK_SIZE );

                    //Check flight control checksum
                    if( sTimeMark.m_Checksum == Checksum( (unsigned short*) &sTimeMark, (TIMEMARK_SIZE-2)/2) ) {
                        parm.GpsValid = sTimeMark.Status & 0x0001;
                        parm.Satellites = sTimeMark.NumSV;
                        switch(sTimeMark.Mode) {
                        case 1:
                            strcpy(parm.CurrentMode, "Test"); break;
                        case 2:
                            strcpy(parm.CurrentMode, "Initialization"); break;
                        case 3:
                            strcpy(parm.CurrentMode, "Not Used"); break;
                        case 4:
                            strcpy(parm.CurrentMode, "Fine Alignment"); break;
                        case 5:
                            strcpy(parm.CurrentMode, "Air Alignment"); break;
                        case 6:
                            strcpy(parm.CurrentMode, "Transfer Alignment"); break;
                        case 7:
                            strcpy(parm.CurrentMode, "Air Navigation"); break;
                        case 8:
                            strcpy(parm.CurrentMode, "Land Navigation"); break;
                        case 9:
                            strcpy(parm.CurrentMode, "GPS Only"); break;
                        default:
                            itoa(sTimeMark.Mode, parm.CurrentMode, 10);
                            sprintf(parm.CurrentMode, "Mode=%s" , parm.CurrentMode);
                        }

                        //Show bitwise status for the 5 SVs channels (Sat1Valid=1 means Tracking).
                        parm.Sat1Valid = (   5 == ( (sTimeMark.MeasState1>>5) & 7 )   );
                        parm.Sat2Valid = (   5 == ( (sTimeMark.MeasState2>>5) & 7 )   );
                        parm.Sat3Valid = (   5 == ( (sTimeMark.MeasState3>>5) & 7 )   );
                        parm.Sat4Valid = (   5 == ( (sTimeMark.MeasState4>>5) & 7 )   );
                        parm.Sat5Valid = (   5 == ( (sTimeMark.MeasState5>>5) & 7 )   );

                    } else {
                        printf("Timemark ChkSum Bad");
                    }
                    break;
                case STATUS_ID:	//MSG 3500		TURN OFF
                    break;
                case NAVSOL_ID:	//MSG 3501
                    TotalBytes = 0;
                    while(bufptr<HEADER_SIZE+NAVSOL_SIZE) {
                        nBytes = 0;
                        //while( nBytes < 1 ) {
                        //    //parm.Serial.Read(parm.serialdata, 1, &nBytes);
                            parm.Serial.Read(parm.serialdata, NAVSOL_SIZE-TotalBytes, &nBytes);
                        //    if(nBytes<1) Sleep(10);
                        //}
                        memcpy(CmigitsData+bufptr, parm.serialdata, nBytes);
                        bufptr += nBytes;
                        TotalBytes += nBytes;
                    }
                    memcpy( &sNavSolution.Time1, CmigitsData+HEADER_SIZE, NAVSOL_SIZE );

                    //Check flight control checksum
                    if( sNavSolution.m_Checksum == Checksum( (unsigned short*) &sNavSolution, (NAVSOL_SIZE-2)/2) ) {
                        parm.Latitude = FloatConvert(sNavSolution.Latitude, 0) * 180;
                        parm.Longitude = FloatConvert(sNavSolution.Longitude, 0) * 180;
                        parm.Altitude = (float)FloatConvert(sNavSolution.Altitude, 15);
                    } else {
                        printf("NavSol ChkSum Bad");
                        parm.Pitch = -9;
                        parm.Roll = -9;
                        parm.Heading = -9;
                        parm.Time = -9;
                    }
                    break;
                case DELTA_ID:	//MSG 3502
                    break;
                case FLIGHTCONTROL_ID:	//MSG 3512
                    TotalBytes = 0;
                    while(bufptr<HEADER_SIZE+FLIGHTCONTROL_SIZE) {
                        nBytes = 0;
                        //while( nBytes < 1 ) {
                            //parm.Serial.Read(parm.serialdata, 1, &nBytes);
                        parm.Serial.Read(parm.serialdata, FLIGHTCONTROL_SIZE-TotalBytes, &nBytes);
                        //}
                        memcpy(CmigitsData+bufptr, parm.serialdata, nBytes);
                        bufptr += nBytes;
                        TotalBytes += nBytes;
                    }
                    memcpy( &sFlightControl.Time1, CmigitsData+HEADER_SIZE, FLIGHTCONTROL_SIZE );

                    //Check flight control checksum
                    if( sFlightControl.m_Checksum == Checksum( (unsigned short*) &sFlightControl, (FLIGHTCONTROL_SIZE-2)/2) ) {
                        parm.Pitch = FloatConvert(sFlightControl.Pitch, 0) * 180.0;
                        parm.Roll = FloatConvert(sFlightControl.Roll, 0) * 180.0;
                        parm.Heading = FloatConvert(sFlightControl.Heading, 0) * 180.0;
                        parm.Time = DoubleConvert(sFlightControl.Time1, sFlightControl.Time2, 20);
                        //printf("T=%f\n" , parm.Time); fflush(stdout);
                        //qDebug() << "T=" << parm.Time*10 - 2e+6;
                    } else {
                        printf("Flight ChkSum Bad");
                        parm.Pitch = -9;
                        parm.Roll = -9;
                        parm.Heading = -9;
                        parm.Time = -9;
                    }
                    Sleep(9);
                    break;

                case BITTEST_ID:	//MSG 3503
                    break;
                case ERROR_ID:
                    break;
                case 3601:
                    break;
                case 3603:						// TURN OFF?
                    break;
                default:
                    break;
                }


            } else {
                printf("Header ChkSum Bad");
                //qDebug() << "Header ChkSum Bad";
            }


            //parm.MonitorInitialized = true;
            //parm.Busy = false;
            //Sleep(400);




            /*            CmigitsData[0] = 0;
            *parm.serialdata = 0;
            //qDebug() << "Start";
            // Get the Message start code
            while( (CmigitsData[0]!=0xFF) || (CmigitsData[1]!=0x81) || (CmigitsData[2]!=0xB8) || (CmigitsData[3]!=0x0D) ) {
                bufptr = 0;
                nBytes = 0;
                while( CmigitsData[0] != 0xFF ) {
                    nBytes = 0;
                    while( nBytes < 1 )
                        parm.Serial.Read(CmigitsData, 1, &nBytes);
                }
                //memcpy(CmigitsData, &parm.serialdata, nBytes);
                bufptr += nBytes;

                while(bufptr<4) {
                    nBytes = 0;
                    while( nBytes < 1 )
                        parm.Serial.Read(parm.serialdata, 1, &nBytes);
                    memcpy(CmigitsData+bufptr, parm.serialdata, nBytes);
                    bufptr += nBytes;
                }
            }

            // Get the header
            while(bufptr<HEADER_SIZE) {
                nBytes = 0;
                while( nBytes < 1 )
                    parm.Serial.Read(parm.serialdata, 1, &nBytes);
                memcpy(CmigitsData+bufptr, parm.serialdata, nBytes);
                bufptr += nBytes;
            }

            memcpy( &sHeader.m_Sync, CmigitsData, HEADER_SIZE );

            //Check header checksum
            if( sHeader.m_Checksum == (0x10000 - sHeader.m_Sync - sHeader.m_ID - sHeader.m_Count - sHeader.m_Flags) | 0xFFFF ) {
                while(bufptr<HEADER_SIZE+FLIGHTCONTROL_SIZE) {
                    nBytes = 0;
                    while( nBytes < 1 ) {
                        parm.Serial.Read(parm.serialdata, 1, &nBytes);
                    }
                    memcpy(CmigitsData+bufptr, parm.serialdata, nBytes);
                    bufptr += nBytes;
                }
                memcpy( &sFlightControl.Time1, CmigitsData+HEADER_SIZE, FLIGHTCONTROL_SIZE );

                //Check flight control checksum
                if( sFlightControl.m_Checksum == Checksum( (unsigned short*) &sFlightControl, (FLIGHTCONTROL_SIZE-2)/2) ) {
                    parm.Pitch = FloatConvert(sFlightControl.Pitch, 0) * 180.0;
                    parm.Roll = FloatConvert(sFlightControl.Roll, 0) * 180.0;
                    parm.Heading = FloatConvert(sFlightControl.Heading, 0) * 180.0;

                    parm.Time = DoubleConvert(sFlightControl.Time1, sFlightControl.Time2, 20);

                } else {
                    //printf("Flight ChkSum Bad");
                    qDebug() << "Flight ChkSum Bad";
                }
            } else {
                //printf("Header ChkSum Bad");
                qDebug() << "Header ChkSum Bad";
            }


            parm.MonitorInitialized = true;
            COSMOS_USLEEP(9000);
*/













            /*            // Checking that the last message ended in "\n"
            if( (parm.comStringIn[StrPos] != '\n') || (strstr(bufferRemainder, "\n")!=NULL) ) {
                *parm.serialdata = 0;
                while(*parm.serialdata != '\n') {
                    ;//nBytes = cssl_getdata(parm.Serial, (uint8_t*)parm.serialdata, 1);
                }
                *parm.comStringIn = 0;
            } else {
                strcpy(parm.comStringIn, bufferRemainder);
                //memcpy(parm.comStringIn, bufferRemainder, nBytes);
            }



            // Get the GPS line
          nBytes = 0;
            *parm.serialdata = 0;
            CharPtr = 0;



            //tic(&startTime);
            while( !strstr(parm.serialdata, "\n") ) {

/ *                // Timeout, restart getting messages
                if(toc(startTime)>4) {
                    sprintf(LogMessage, "Timeout getting GPS: %s" , parm.name);
                    Log(LogMessage);
                    break;
                }* /

                //nBytes = cssl_getdata(parm.Serial, (uint8_t*)parm.serialdata, 50);
                if(nBytes<0) {
                    nBytes = 0;
                    parm.serialdata[nBytes] = 0;
                } else {
                    //nTBytes += nBytes;
                    parm.serialdata[nBytes] = 0;
                    strcat(parm.comStringIn, parm.serialdata);
                }

                //usleep(1000);
            }
*/

            /*
            // Assemble data
            NewLinePos = strstr(parm.comStringIn, "\n");
            strcpy(bufferRemainder, NewLinePos+1);  // Save remainder for next time.
            NewLinePos++;
            *NewLinePos = 0;   // chop off remainder
*/





            //tic(&MyStartTime);
            //tic(&MyStopTime);

            //MyInt1 = MyStartTime.tv_sec - startTime.tv_sec;
            //MyInt2 = MyStartTime.tv_usec - startTime.tv_usec;

            // Wait while Save routine is saving
            /*            while(SaveOperationWriting)
                usleep(1000);
*/
            //usleep(4000);

            //            LengthOfString = strlen(parm.comStringIn);
            //            if(LengthOfString>300) LengthOfString=300;

            //            strncpy(MyGpsLineL, parm.comStringIn, LengthOfString);
            //            MyGpsLineL[LengthOfString] = 0;

            //            if(LengthOfString<100) {
            //                qDebug() << QString("StrLen: %1").arg(LengthOfString);
            //            }

            //            if(MyGpsLineL[LengthOfString-5] != '*') {
            //                qDebug() << QString("StrLen: %1").arg(LengthOfString);
            //            }

            //qDebug() << QString("StrLen: %1").arg(LengthOfString);
            //qDebug() << QString("GPS: %1").arg(MyGpsLineL);
        }
    }

    //if (parm.Serial) cssl_close(parm.Serial);
    parm.Serial.Close();
}


// **********************************************
float FloatConvert(long value, long scale)
{
    unsigned long den;

    den = 1UL<<(32-scale-1);
    return (float)((double)value/den);
}

// **********************************************
double DoubleConvert(long value1, unsigned long value2, long scale)
{
    double pow31;
    double pow32;
    double ans;

    pow31 = 2147483648.0;
    pow32 = 4294967296.0;
    ans = value1*pow32 + value2;
    return ans/pow31/pow32*((long)1<<scale);
}

// **********************************************
unsigned short Checksum(unsigned short *pData, long numwords)
{
    long i;
    unsigned short chksum;

    chksum=0;
    for (i=0; i<numwords; i++) {
        chksum -= pData[i];
        //qDebug() << "Dat=" << pData[i];
    }
    //qDebug() << "Chk=" << chksum;
    return chksum;
}
