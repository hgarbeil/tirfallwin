#include <unistd.h> //for usleep
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "cssl_lib.h"
#include "deviceFctns.h"

void FindRotationAPort(int *RotationAPortNum, int *BbPortNum)
{
    int i, nBytes = 255;
    char strPort[32];
    char comString[256];
    cssl_t *SerialCheck;

    // Find SMC100CC RotationA Stage
    for(i=0 ; i<16 ; i++) {
        if( i!=*BbPortNum ) {
            sprintf(strPort, "/dev/ttyUSB%d", i);
            SerialCheck=cssl_open(strPort,0, 57600,8,0,1);

            usleep(20000);
            if(SerialCheck) {
                cssl_settimeout(SerialCheck, 0, 1);  // 1 second timeout

/*                // Start by homing the slide stage. New
                sprintf (comString, "1OR?\r\n");	//SMC100 home
                cssl_putstring(SerialCheck,comString);
                usleep(50000);

                memset (comString, 0, 255);
                cssl_getdata(SerialCheck, (uint8_t*)comString, nBytes);*/



                // New stage seems to ignore first command, so send this twice.
                strcpy(comString, "1TP?\r\n");
                cssl_putstring(SerialCheck,comString);
                usleep(50000);

                memset (comString, 0, 255);
                cssl_getdata(SerialCheck, (uint8_t*)comString, nBytes);



                // Is this a Motion Stage Controller
                strcpy(comString, "1TP?\r\n");
                cssl_putstring(SerialCheck,comString);
                usleep(50000);

                memset (comString, 0, 255);
                cssl_getdata(SerialCheck, (uint8_t*)comString, nBytes);


                if(strncmp(comString, "1TP", 3) == 0) {
                    // It is!
                    *RotationAPortNum = i;
                    cssl_drain(SerialCheck);
                    cssl_close(SerialCheck);
                    break;
                }
                cssl_drain(SerialCheck);
                cssl_close(SerialCheck);
            }
        }
    }
}


void FindBbPort(int *RotationAPortNum, int *BbPortNum)
{
    int i, nBytes = 255;
    char strPort[32];
    char comString[256];
    cssl_t *SerialCheck;

    // Find Arroyo Temperature Controller
    for(i=0 ; i<16 ; i++) {
        if( i!=*RotationAPortNum ) {
            sprintf(strPort, "/dev/ttyUSB%d", i);
            SerialCheck=cssl_open(strPort,0, 38400,8,0,1);

            usleep(10000);
            if(SerialCheck) {
                cssl_settimeout(SerialCheck, 0, 1);  // 1 second timeout!

                // Is this a Temperature (BB) Controller
                strcpy(comString, "*IDN?\r\n");
                cssl_putstring(SerialCheck,comString);
                usleep(50000);
                memset(comString, 0, 255);
                cssl_getdata(SerialCheck, (uint8_t*)comString, nBytes);


                // Takes a second time to get a reply if we did RotationA first.
                strcpy(comString, "*IDN?\r\n");
                cssl_putstring(SerialCheck,comString);
                usleep(50000);
                memset(comString, 0, 255);
                cssl_getdata(SerialCheck, (uint8_t*)comString, nBytes);


                if(strncmp(comString, "Arroyo", 6) == 0) {
                    // It is!
                    *BbPortNum = i;
                    cssl_drain(SerialCheck);
                    cssl_close(SerialCheck);
                    break;
                }
                cssl_drain(SerialCheck);
                cssl_close(SerialCheck);
            }
        }
    }
}


bool OpenRotationA(cssl_t **SerialRotationA, int RotationAPortNum, bool *RotationABusy)
{
    int nBytes = 255;
    char comString[256];
    char strPort[32];

    // ***Open RotationA Stage
    sprintf(strPort, "/dev/ttyUSB%d", RotationAPortNum);
    *SerialRotationA=cssl_open(strPort,0, 57600,8,0,1);
    if (!*SerialRotationA) {
        printf("%s\n",cssl_geterrormsg());
        return(false);
    } else {
        cssl_settimeout(*SerialRotationA, 0, 0.5);  // .05 second timeout.

        // Don't do this.  It is already done in the find routine, and interferes with the findroutine homing
        // Start by homing the slide stage.
        sprintf (comString, "1OR?\r\n");	//SMC100 home
        cssl_putstring(*SerialRotationA,comString);

        // Throw away response
        usleep(40000);
        cssl_getdata(*SerialRotationA, (uint8_t*)comString, nBytes);

        //Then send the command to get RotationA Position (So it will be ready when we ask for it.)
        strcpy(comString, "1TP\r\n");	//SMC100 get position string
        cssl_putstring(*SerialRotationA,comString);

        // Throw away response
        usleep(40000);
        cssl_getdata(*SerialRotationA, (uint8_t*)comString, nBytes);

        *RotationABusy=false;
        return(true);
    }
}

bool OpenBb(cssl_t **SerialBb, int BbPortNum, bool *BbBusy)
{
    int nBytes = 255;
    char comString[256];
    char strPort[32];

    // ***Open BB
    sprintf(strPort, "/dev/ttyUSB%d", BbPortNum);
    *SerialBb=cssl_open(strPort,0, 38400,8,0,1);
    if (!*SerialBb) {
        printf("%s\n",cssl_geterrormsg());
        return(false);
    } else {

        cssl_settimeout(*SerialBb, 0, 0.5);  // .05 second timeout.
        // Start by requesting the Black Body temperature (so it will be ready later).
        sprintf (comString, "TEC:T?\r\n");
        cssl_putstring(*SerialBb,comString);

        // Throw away response
        usleep(40000);
        cssl_getdata(*SerialBb, (uint8_t*)comString, nBytes);

        *BbBusy=false;
        return(true);
    }
}
