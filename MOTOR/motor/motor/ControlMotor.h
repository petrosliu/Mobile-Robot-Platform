#ifndef _CONTROLMOTOR_H_
#define _CONTROLMOTOR_H_
#include<math.h>
#include "CRs232.h"

int motor(CRs232 SerialPort, int motorId, int actionType, int motorSpeed);
double  getMotorAngle(CRs232 SerialPort, int motorId);

#endif