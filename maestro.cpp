#include "maestro.h"


#define LSB7(x) (x & 0x7F)
#define USB7(x) ((x >> 7) & 0x7F)

Maestro::Maestro()
{
    //printf("Starting BB %s\n" , port);
    parm.MonitorInitialized = false;
    parm.ControlInitialized = false;
    parm.Terminate = false;
    parm.Busy = false;
}


void Maestro::Init(char *port)
{
    Open(port);
    StartMonitor();
    StartControl();
}

void Maestro::Open(char *port)
{
    cssl_start();

    parm.Serial=cssl_open(port, 0, 57600,8,0,1);
    if (parm.Serial) {
        cssl_settimeout(parm.Serial, 0, 0.5);  // .5 second timeout.
        parm.PortOpen = true;
        strcpy(parm.Status, "Open");
    } else {
        parm.PortOpen = false;
        strcpy(parm.Status, "Error");
    }
    usleep(300000);

    MaestroGoHome();
}

void Maestro::Kill()
{
    parm.Terminate=true;
}

/*
void Maestro::Illuminator(unsigned short *PresentPositionCh0, unsigned short *PresentPositionCh1)
{
    while(!parm.MonitorInitialized) usleep(100000);

    *PresentPositionCh0 = parm.PresentPositionCh0;
    *PresentPositionCh1 = parm.PresentPositionCh1;
}
*/


void Maestro::Get(unsigned short *PresentPositionCh0, unsigned short *PresentPositionCh1)
{
    while(!parm.MonitorInitialized) usleep(100000);

    *PresentPositionCh0 = parm.PresentPositionCh0;
    *PresentPositionCh1 = parm.PresentPositionCh1;
}

void Maestro::Set(unsigned char DesiredChannel, unsigned short DesiredPosition)
{
    while(!parm.ControlInitialized) usleep(100000);

    parm.DesiredChannel = DesiredChannel;
    parm.DesiredPosition = DesiredPosition;
    sem_post (&parm.SetPosition_count);
}

void Maestro::SetIlluminator(unsigned char DesiredChannel, unsigned short OnOff, unsigned short PowerLevel)
{
    while(!parm.ControlInitialized) usleep(100000);

    parm.DesiredChannel = DesiredChannel;
    parm.OnOff = OnOff;
    parm.PowerLevel = PowerLevel;
    sem_post (&parm.SetPosition_count);
}


void Maestro::Monitor()
{
    //int nBytes = 255;
    //sem_init (&parm.GetPosition_count, 0, 0);

    while(!parm.Terminate) {
        //sem_wait (&parm.GetPosition_count);

        if(parm.PortOpen) {
            //If the Position is being set, skip getting the Position this time.
            if(!parm.Busy) {
                parm.Busy = true;

                // Get the Maestro position
                parm.PresentPositionCh0 = GetMaestroPos(0);
                parm.PresentPositionCh1 = GetMaestroPos(1);

                parm.MonitorInitialized = true;
                parm.Busy = false;
                usleep(500000);
                //printf("Position = %3.2f\n" , parm.PresentPosition);
            }
        }
    }    
    if (parm.Serial) cssl_close(parm.Serial);
}

void Maestro::Control()
{
    sem_init (&parm.SetPosition_count, 0, 0);
    parm.ControlInitialized = true;

    while(!parm.Terminate) {
        sem_wait (&parm.SetPosition_count);
        if(parm.Terminate) break;

        if(parm.PortOpen) {
            // Wait if BB monitor is getting the Position (put a timeout here)
            while(parm.Busy) {
                usleep(100000);
            }
            parm.Busy = true;

            // Set the Position.
            if(parm.DesiredChannel==8) {
                // For now Illuminator is always on Ch 8
                IlluminatorControl(parm.OnOff, parm.PowerLevel);
            } else {
                SetMaestroPos(parm.DesiredChannel, parm.DesiredPosition);
            }

            usleep(200000);
            parm.Busy = false;
        }
    }
}

void Maestro::MaestroGoHome(void)
{
    uint8_t comString[64];

    comString[0] = 0xA2;
    cssl_putdata(parm.Serial, comString, 1);
}

void Maestro::SetMaestroPos(unsigned char Channel, unsigned short Position)
{
    uint8_t comString[64];

    comString[0] = 0x84;
    comString[1] = Channel;
    comString[2] = LSB7(Position);
    comString[3] = USB7(Position);
    cssl_putdata(parm.Serial, comString, 4);
}

// 0 - 3968
void Maestro::SetMaestroSpeed(unsigned char Channel, unsigned short Speed)
{
    uint8_t comString[64];

    comString[0] = 0x87;
    comString[1] = Channel;
    comString[2] = LSB7(Speed);
    comString[3] = USB7(Speed);
    cssl_putdata(parm.Serial, comString, 4);
}

// 0 - 255
void Maestro::SetMaestroAcceleration(unsigned char Channel, unsigned short Acceleration)
{
    uint8_t comString[64];

    comString[0] = 0x89;
    comString[1] = Channel;
    comString[2] = LSB7(Acceleration);
    comString[3] = USB7(Acceleration);
    cssl_putdata(parm.Serial, comString, 4);
}

unsigned short Maestro::GetMaestroPos(unsigned char Channel)
{
    uint8_t comString[64];

    comString[0] = 0x90;
    comString[1] = Channel;
    cssl_putdata(parm.Serial, comString, 2);
    usleep(50000);
    cssl_getdata(parm.Serial, comString, 2);
    return( (unsigned char)comString[1]*256 + (unsigned char)comString[0] );
}

void Maestro::IlluminatorControl(unsigned short OnOff, unsigned short PowerLevel)
{
    unsigned char Channel = 8;
    uint8_t comString[64];

    comString[0] = 0x91;
    comString[1] = Channel;
    comString[2] = OnOff;
    comString[3] = PowerLevel;
    cssl_putdata(parm.Serial, comString, 4);
}
