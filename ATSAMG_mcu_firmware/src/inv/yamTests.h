/*
 * IncFile1.h
 *
 * Created: 1/17/2019 5:29:00 PM
 *  Author: pmadhvapathy
 */ 


#ifndef YAMTESTS_H_
#define YAMTESTS_H_
#include "yamAPI.h"

typedef enum WhoAmIType1 {YamNotOK = false, YamOK = true} WhoAmIType;
typedef struct accelFSRtuple {AccelFsrtype afsr; AccellRestype alrfsr;} accelFSRtuple1;
struct accelFSRtuple accelAllowedFSRcombo [];


uint16_t TestTestReg(int iter);												// test register test
uint16_t TestWhoAmI(uint16_t whoYamSupposedToBe, int iter);					// who am i test
uint16_t TestBankReg(int iter);												// Bank register test
uint16_t LrAccelTestbydiff(int iter);										// Low Res Accel Resolution Test against normal Accel
uint16_t AccelNormtest(int iter);											// Normal and Low Res Accel Norm == 1g test
uint16_t PowerupResetRegTest(int iter);										// Test power up and reset values of registers

void TestMain();  // main test entry

#endif /* YAMTESTS_H_ */