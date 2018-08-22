#ifndef DEVICEFCTNS_H
#define DEVICEFCTNS_H

#include "cssl_lib.h"

void FindBbPort(int *RotationAPortNum, int *BbPortNum);
void FindRotationAPort(int *RotationAPortNum, int *BbPortNum);
bool OpenRotationA(cssl_t **SerialRotationA, int RotationAPortNum, bool *RotationABusy);
bool OpenBb(cssl_t **SerialBb, int BbPortNum, bool *BbBusy);

#endif // DEVICEFCTNS_H
