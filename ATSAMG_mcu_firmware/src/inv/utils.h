/*
 * IncFile1.h
 *
 * Created: 1/25/2019 1:57:00 PM
 *  Author: pmadhvapathy
 */ 


#ifndef UTILS_H_
#define UTILS_H_


#include <asf.h>
#include "protocol_functions.h"
#include "inv_uart.h"
#include "inv_i2c.h"
#include "inv_spi.h"
#include "arm_version.h"
#include "math.h"
#include "delay.h"
#include <stdio.h>
#include <stdlib.h>
#include "IAM20685Reg.h"
#include "yamtests.h"
#include "bsp.h"
#include "crc.h"
#include "lockUnlock.h"
#include "yamTests.h"


typedef enum printUnitsType1 {rawbits, milliUnits, maxiUnits} printUnitsType;
typedef enum printCompType1 {Af=0, Al, dA, Gp, Tp, Op} printCompType;

void printAccel(int16_t * arr);
void printHdr(int i);
void setPrintUnits(printUnitsType printFmt);
void printAxes(int16_t *accelp, int16_t *accelLowresp, int16_t *gyrop, int16_t *temperaturep, int16_t *anythingelsep);
void printComponentHeader(printCompType comp);
void resetPrintHdrCount();
float floatAbs ( float f);
uint16_t lsbSet(uint16_t v);

#endif /* UTILS_H_ */