/*
 * CFile1.c
 *
 * Created: 2/5/2019 2:13:29 PM
 *  Author: pmadhvapathy
 */ 
#include <asf.h>
#include "protocol_functions.h"
#include "delay.h"
#include "IAM20685Reg.h"
#include "inv_spi.h"
#include "delay.h"
#include "crc.h"
#include "yamTests.h"
#include "arm_math.h"
#include "utils.h"
#include "yamAlarm.h"

struct alarmMskStsType alarmMskStsTable[] = {  
							{	{bSMmsk1, SMmsk1, almMskGZquadAdc},
								{bGa1, Ga1, alarmGZquadadc},
								grpQuadadc, false, false
							},  /* GyroQuadADC Z SM04 */
							{
								{bSMmsk1, SMmsk1, almMskGYquadAdc},
								{bGa1, Ga1, alarmGYquadadc},
								grpQuadadc, false, false
							}, /* GyroQuadADC Y SM04 */
							{
								{bSMmsk1, SMmsk1, almMskGXquadAdc},
								{bGa1, Ga1, alarmGXquadadc},
								grpQuadadc, false, false
							}, /* GyroQuadADC X SM04 */
							{
								{bSMmsk1, SMmsk1, almMskGZdrfreqmeas},
								{bGa1, Ga1, alarmGZdrfreqmeas},
								grpDrvfreq, false, false
							}, /* GyroDrvFreqMeasurement Z SM03 */
							{
								{bSMmsk1, SMmsk1, almMskGYdrfreqmeas},
								{bGa1, Ga1, alarmGYdrfreqmeas},
								grpDrvfreq, false, false
							}, /* GyroDrvFreqMeasurement Y SM03 */
							{
								{bSMmsk1, SMmsk1, almMskGXdrfreqmeas},
								{bGa1, Ga1, alarmGXdrfreqmeas},
								grpDrvfreq, false, false
							}, /* GyroDrvFreqMeasurement X SM03 */
							{
								{bSMmsk1, SMmsk1, almMskSysClk},
								{bOa2, Oa2, alarmSysClk},
								grpDrvfreq, false, false
							}, /* Sysclk  SM03 */
							{
								{bSMmsk1, SMmsk1, almMskRcosc1Freqmeas},
								{bOa1, Oa1, alarmRcosc1Freqmeas},
								grpDrvfreq, false, false
							}, /* RCosc1  SM03 */
							{
								{bSMmsk4, SMmsk4, almMskRcosc2Freqmeas},
								{bOa1, Oa1, alarmRcosc2Freqmeas},
								grpDrvfreq, false, false
							}, /* RCosc2  SM03 */							
							{
								{bSMmsk4, SMmsk4, almMskGZdcst},
								{bGa2, Ga2, alarmGZdcst},
								grpGdcst, false, false
							}, /* Gyro Self-Test Z SM36 */
							{
								{bSMmsk4, SMmsk4, almMskGYdcst},
								{bGa2, Ga2, alarmGYdcst},
								grpGdcst, false, false
							}, /* Gyro Self-Test Y SM36 */
							{
								{bSMmsk4, SMmsk4, almMskGXdcst},
								{bGa2, Ga2, alarmGXdcst},
								grpGdcst, false, false
							}, /* Gyro Self-Test X SM36 */
							{
								{bSMmsk2, SMmsk2, almMskGyroVref},
								{bGa2, Ga2, alarmGVref},
								grpVref, false, false
							}, /* Gyro Vref SM14 */
							{
								{bSMmsk2, SMmsk2, almMskGZdrclk},
								{bGa2, Ga2, alarmGZdrclk},
								grpDrvclk, false, false
							}, /* Gyro Drive Clk Z  SM07 */
							{
								{bSMmsk2, SMmsk2, almMskGYdrclk},
								{bGa2, Ga2, alarmGYdrclk},
								grpDrvclk, false, false
							}, /* Gyro Drive Clk Y  SM07 */
							{
								{bSMmsk2, SMmsk2, almMskGXdrclk},
								{bGa2, Ga2, alarmGXdrclk},
								grpDrvclk, false, false
							}, /* Gyro Drive Clk X  SM07 */
							{
								{bSMmsk3, SMmsk3, almMskVrefshldZ},
								{bAa1, Aa1, alarmVrefshldZ},
								grpVrefshldZ, false, false
							}, /* Gyro Vref Shield Z  SM17 */
							{
								{bSMmsk3, SMmsk3, almMskVrefshldXY},
								{bAa1, Aa1, alarmVrefshldXY},
								grpVrefshldXY, false, false
							}, /* Gyro Vref Shield XY  SM17 */
							{
								{bSMmsk3, SMmsk3, almMskAccelCp},
								{bAa1, Aa1, alarmAccelCp},
								grpAcp, false, false
							}, /* Accel Drive Voltage  SM18 */
							{
								{bSMmsk2, SMmsk2, almMskAccelVref},
								{bAa1, Aa1, alarmAccelVref},
								grpVref, false, false
							}, /* Accel VRef  SM14 */
							{
								{bSMmsk5, SMmsk5, almMskAZdcst},
								{bAa2, Aa2, alarmAZdcst},
								grpAdcst, false, false
							}, /* Accel Self-Test Z SM16 */
							{
								{bSMmsk5, SMmsk5, almMskAYdcst},
								{bAa2, Aa2, alarmAYdcst},
								grpAdcst, false, false
							}, /* Accel Self-Test Y SM16 */
							{
								{bSMmsk5, SMmsk5, almMskAXdcst},
								{bAa2, Aa2, alarmAXdcst},
								grpAdcst, false, false
							}, /* Accel Self-Test X SM16 */
							{
								{bSMmsk3, SMmsk3, almMskOtpCrc},
								{bOa1, Oa1, alarmOtpCrc},
								grpOtpCRC, false, false
							}, /* OTP CRC  SM26 */
							{
								{bSMmsk4, SMmsk4, almMskOtpCpy},
								{bOa2, Oa2, alarmOtpCpy},
								grpOtpcpy, false, false
							}, /* OTP Cpy  SM26 */
							{
								{bSMmsk3, SMmsk3, almMskOtpReg},
								{bOa1, Oa1, alarmOtpReg},
								grpOtpreg, false, false
							}, /* OTP Reg  SM26 */
							{
								{bSMmsk4, SMmsk4, almMskBg},
								{bOa1, Oa1, alarmBg},
								grpbg, false, false
							}, /* Band Gap  SM34 */
							{
								{bSMmsk3, SMmsk3, almMskCpVref},
								{bOa1, Oa1, alarmCpVref},
								grpVref, false, false
							}, /* Cp VRef SM14 */
							{
								{bSMmsk2, SMmsk2, almMskTempVref},
								{bOa1, Oa1, alarmTempVref},
								grpVref, false, false
							}, /* Temp VRef SM14 */
							{
								{bSMmsk2, SMmsk2, almMskVdd},
								{bOa1, Oa1, alarmVdd},
								grpExtVdd, false, false
							}, /* External VDD SM13 */
							{
								{bSMmsk2, SMmsk2, almMskVddio},
								{bOa1, Oa1, alarmVddio},
								grpVddio, false, false
							}, /*  External VDDIO SM13 */
							{
								{bSMmsk2, SMmsk2, almMskDvdd},
								{bOa1, Oa1, alarmDvdd},
								grpDvdd, false, false
							}, /*  Digital DVdd SM12 */
							{
								{bSMmsk2, SMmsk2, almMskAvdd},
								{bOa1, Oa1, alarmAvdd},
								grpAvdd, false, false
							}, /*  Digital AVdd SM11 */
							{
								{bSMmsk2, SMmsk2, almMskVddmaster},
								{bOa1, Oa1, alarmVddmaster},
								grpVddmaster, false, false
							}, /*  Digital VddMaster SM10 */
							{
								{bSMmsk2, SMmsk2, almMskTemp12},
								{bOa1, Oa1, alarmTemp12},
								grpTemp, false, false
							}, /*  Temp Delta SM08 */
							{
								{bSMmsk2, SMmsk2, almMskTempDsp},
								{bOa1, Oa1, alarmTempDsp},
								grpTemp, false, false
							}, /*  Temp dsp SM08 */
							{
								{bSMmsk4, SMmsk4, almMskRegcheck},
								{bOa2, Oa2, alarmRegcheck},
								grpRegcheck, false, false
							}, /*  Reg Check SM33 */
							{
								{bSMmsk4, SMmsk4, almMskSpiClkcnt},
								{bOa2, Oa2, alarmSpiClkcnt},
								grpspiclk, false, false
							}, /*  SP Clk Cnt SM33 */
							{
								{bSMmsk4, SMmsk4, almMskSpiCmddecod},
								{bOa2, Oa2, alarmSpiCmddecod},
								grpspicmddec, false, false
							}, /*  SP Clk Cnt SM33 */
							{
								{bSMmsk4, SMmsk4, almMskSpiCrc},
								{bOa2, Oa2, alarmSpiCrc},
								grpspicrc, false, false
							}, /*  SP Clk CRC SM33 */
							{
								{bSMmsk3, SMmsk3, almMskAhbBus},
								{bOa2, Oa2, alarmAhbbus},
								grpAhb, false, false
							}, /*  AHB Bus SM33 */


}; //alarmStsMskTable



/////////////////////////////////////////////////////////////
struct alarmEnLtchType alarmEnLtchTable[] = { 
										{  {bSMen2, SMen2, fenDrvfreq},
										   {bSMle1, SMle1, fleGyrodrfreqmeas}
										},/*DrvFreq SM03*/
										{   {bSMen1, SMen1, fenQuadadc},
											{bSMle1, SMle1, fleGyroQuadAdc}
									    },/*Quadrature  SM04*/
										{   {bSMen1, SMen1, fAlwaysEnabled}, 
											{bSMle2, SMle2, fleGyroDrclk}
										}, /*DrvClk  SM07*/ // edit below
										{   {bSMen1, SMen1, fenTemp},
											{bSMle2, SMle2, fleGyroDrclk}
									    },/* Temp SM08*/
										{   {bSMen1, SMen1, fenVddmaster}, 
											{bSMle2, SMle2, fleVddmaster}
										}, /* Master Regulator SM10 */
										{   {bSMen1, SMen1, fenAvdd},
										    {bSMle2, SMle2, fleAvdd}
										}, /* AVDD SM11 */
										{
											{bSMen1, SMen1, fenDvdd},
											{bSMle2, SMle2, fleDvdd}
										}, /* DVDD SM12 */
										{
											{bSMen1, SMen1, fenExtVdd},
											{bSMle2, SMle2, fleExtVdd}
										}, /* External VDD SM13 */		
										{
											{bSMen1, SMen1, fenVddio},
											{ bSMle2, SMle2, fleVddio}
										}, /* Vddio SM13 */
										{
											{bSMen1, SMen1, fenVref},
											{bSMle2, SMle2, fleVref}
										}, /* VRef SM14 */
										{
											{bSMen1, SMen1, fenAdcst},
											{bSMle2, SMle2, fleAdcst}
										}, /* Accel Self-Test SM16 */
										{
											{bSMen1, SMen1, fenVrefshld},
											{bSMle2, SMle2, fleVrefshldXY}
										}, /* VrefShield XY, SM17a */
										{
											{bSMen1, SMen1, fenVrefshld},
											{bSMle2, SMle2, fleVrefshldZ}
										}, /* VrefShield XY, SM17b */
										{
											{bSMen1, SMen1, fenAcp},
											{bSMle2, SMle2, fleAcp}
										}, /* Accel Drive Voltage, SM18 */
										{
											{bSMen1, SMen1, fenOtpcrc},
											{bSMle2, SMle2, fleotpcrc}
										}, /* OTP CRC, SM26 */  /* Dont Exist */
										{
											{bSMen1, SMen1, fenOtpcpy},
											{bSMle2, SMle2, fleotpcpy}
										}, /* OTP CPY, SM26 */  /* Dont Exist */
										{
											{bSMen1, SMen1, fenOtpreg},
											{bSMle2, SMle2, fleotpreg}
										}, /* OTP Reg, SM26 */  /* Dont Exist */
										{
											{bSMen2, SMen2, fenAhb},
											{bSMle2, SMle2, fleAhb}
										}, /* AHB Bus, SM29 */  
										{
											{bSMen1, SMen1, fenspi},
											{bSMle1, SMle1, fleSpiClkcnt}
										}, /* SPI ClkCount, SM30 */
										{
											{bSMen1, SMen1, fenspi},
											{bSMle1, SMle1, fleSpiCrc}
										}, /* SPI CRC, SM30 */
										{
											{bSMen1, SMen1, fenspi},
											{bSMle1, SMle1, fleSpiCmddecod}
										}, /* SPI Cmd Decoded, SM30 */
										{
											{bSMen2, SMen2, fenRegcheck},
											{bSMle1, SMle1, fleRegchk}
										}, /* Continuous Register Check, SM33 */
										{
											{bSMen2, SMen2, fenBg},
											{bSMle2, SMle2, fleBg}
										}, /* Band gap, SM34 */
										{
											{bSMen2, SMen2, fenGdcst},
											{bSMle2, SMle2, fleGdcst}
										}, /* Gyro Self-Test, SM36 */								
}; //alarmEnLtchTable
								    




// Mask/Unmask global for AlarmB pin. Returns previous value
bool maskUnmaskAllAlarmB (bool maskiftrueElseUnmask)
{
	bool retVal = false;
	int16_t regVal, tempVal, val2Write;
	
	regVal = readReg(1, SMmsk5);
	tempVal = regVal & fSMalarmBpin;
	
	retVal = tempVal ? true : false;
	
	val2Write = maskiftrueElseUnmask ? 1 : 0;
	yamWrite(1, SMmsk5, val2Write, fSMalarmBpin, preserve);  // an additional read for RMW - could be avoided by doing RMW here

	return retVal;
}

//******* Event enable/disable

#define EVENT_ENABLE 1
#define _MASK_ENABLE 0

bool grpMemberNeedsIt(int elGrp, bool enableLatch)
{
	bool found = false;
	bool enOrLch;
	int i;
	for (i=0, found=false; (found == false) && (i<sizeof(alarmMskStsTable)/sizeof(struct alarmMskStsType)); i++) {
		if (alarmMskStsTable[i].almGrp == elGrp) {  
			enOrLch = enableLatch == false ? alarmMskStsTable[i].wantEnable : alarmMskStsTable[i].wantLatch;
			if  (enOrLch) {
				found = true; // another grp member wants it enabled, sorry
			}
		}
	}
	return found;	
}

void enableDisableEvGrp (enum eventEnableCmdType ev, int grpNum        /* inputs */
						, uint16_t *v, bool *wantflag, bool *newdata   /* outputs */
						)
{
  	if (ev != keepEnState) {
	  	if (ev == EVENT_ENABLE) {
		  	*wantflag = true;
		  	*v = EVENT_ENABLE;
		  	*newdata = true;
	  	}
	  	else  {
		  	// check all the others in group before turning off
		  	*wantflag = false;
		  	if (!grpMemberNeedsIt(grpNum, false)) { // disable 'cause no one else in grp wants it enabled
			  	*v = !EVENT_ENABLE;
			  	*newdata = true;
		  	}
	  	}
  	}	
}

// one event per call --> inefficient for driver, ok for test
void enableIndividualEvent(enum alarmEventType ev, enum eventEnableCmdType evMsk, enum eventEnableCmdType evEnable, enum eventEnableCmdType lchEnable)
{
	int i;
	int enableLatchgrp;
	//uint16_t value2Write=0;
	struct alarmEnLtchType enLch;
	struct regfullAddrType regfull[3];
	uint16_t val[3];
	const int iMsk=0, iEn=1, iLch=2;
	bool  wrt[3];
	
	wrt[0]=wrt[1]=wrt[2]=false;
	
	
	// Mask
	if (evMsk != keepEnState) {
		 val[iMsk] = evMsk == enable ?  _MASK_ENABLE : !_MASK_ENABLE;
		 regfull[iMsk] = alarmMskStsTable[ev].pinMask;
		 wrt[iMsk] = true;
	}
	
	if ((evEnable == keepEnState) && (lchEnable == keepEnState))
	  return;  // simpler to exit here than to keep checking condition below.
	  
	// enable & latch eagerly, disable & unlatch with caution
	// Enable & Latch common
	enableLatchgrp = alarmMskStsTable[ev].almGrp;
	enLch = alarmEnLtchTable[enableLatchgrp];

	regfull[iEn] = enLch.enable;
	regfull[iLch] = enLch.latch;
	
	enableDisableEvGrp (evEnable, enableLatchgrp /*inputs*/, &val[iEn], &alarmMskStsTable[ev].wantEnable, &wrt[iEn]   /* outputs */);
	enableDisableEvGrp (lchEnable, enableLatchgrp /*inputs*/, &val[iLch], &alarmMskStsTable[ev].wantLatch, &wrt[iLch]   /* outputs */);

    // now write the registers	
	if (wrt[iMsk] || wrt[iEn] || wrt[iLch])
	   bankselect (regfull[iEn].b);  // latch, enable and msk currently all in Bank 1.  yamWrite ignores bank for now.
	  
	for (i=iMsk; i<=iLch; i++)
	 yamWrite(regfull[i].b, regfull[i].r, val[i], regfull->regfld, preserve);
	 
	if (wrt[iMsk] || wrt[iEn] || wrt[iLch])
	   bankselect (0);  // restore to 0 (default)
	
}

// *  report top level SM err status
uint16_t SMtopstatus ()
{
	uint16_t errbits;
	enum errWriteType rwerr;
	
	bankselect(bSMs);
	
	errbits = ReadReg(SMs, &rwerr);	
	errbits &= fSMs;
	
	bankselect(0);
	
    return errbits;
}

static char *gyroErrMsg[] =  { "SM04: Gyro Quad Signal Amplitude [Z]", "SM04: Gyro Quad Signal Amplitude [Y]", "SM04: Gyro Quad Signal Amplitude [X]"
							 , "SM03: Gyro PLL Drive Freq Measurement[Z]", "SM03: Gyro PLL Drive Freq Measurement[Y]", "SM03: Gyro PLL Drive Freq Measurement[X]"
							 , "SM36: Gyro DC Self-Test [Z]", "SM36: Gyro DC Self-Test [Y]", "SM36: Gyro DC Self-Test [X]"
							 , "SM14: Gyro VRef"
							 , "SM07: Gyro DriveClock PLL lock lost [Z|Y|X]", "SM07: Gyro DriveClock PLL lock lost [Y]", "SM07: Gyro DriveClock PLL lock lost [X]"
							 };
static char *accelErrMsg[] = { "SM17: Accel VRef Shield Z", "SM17: Accel VRef Shield XY", "SM18: Accel CP Drive Voltage", "SM14: Accel Vref"
							 , "SM16: Accel DC Self-Test [X]", "SM16: Accel DC Self-Test [Y]", "SM16: Accel DC Self-Test [Z]"
							 };
static char *otherErrMsg[] = { "SM26: OTP CRC", "SM26: OTP REG", "SM34: Band Gap", "SM14: CP Vref", "SM14: Temp Vref", "SM13: Vdd", "SM13: Vddio", "SM12: DVdd"
	                        , "SM11: AVdd", "SM10: Vdd Master", "SM08 Temp Delta High", "SM08: Temp1 High", "SM03: RC Osc 2 frequency",  "SM03: RC Osc 1 frequency"
							, "SM03: Sys Clk Frequency", "SM26: OTP load test Timeout", "SM33: Register(s) integrity"
							, "SM30: SPI Clk Count", "SM30: Redundant SPI encode-decode mismatch", "SM30: SPI CRC"
							, "SM29: Unexpected Address/Data on Ahb Marix"};


static char almMsgbuf[1000];
static int almMsgIndex=0;
const char crlf[] = "\r\n"; 

void almMsgbufclear() { almMsgIndex=0;}
void almMsgAppend(char * p, bool addNewline) 
{ 
	sprintf(almMsgbuf+almMsgIndex, "%s", p); 
	almMsgIndex += strlen(p); 
	if (addNewline) {
	  sprintf(almMsgbuf+almMsgIndex, "%s", crlf); 
	  almMsgIndex+= strlen(crlf);
	}
}
void almDataAppend(uint32_t dp, bool addNewline)
{

	  char buff[20];
	  sprintf(buff, "%04X", dp);
      sprintf(almMsgbuf+almMsgIndex, "%s", buff); 
	  almMsgIndex += strlen(buff);
	
	  
	if (addNewline) {
	  sprintf(almMsgbuf+almMsgIndex, "%s", crlf); 
	  almMsgIndex+= strlen(crlf);
	}
	
}
void almMsgPrint() {printf("%s", almMsgbuf); almMsgbufclear();}


uint16_t parseStsbits(uint16_t sts, uint16_t validFld, char * msgs[])
{
		int i, j, bitfld;
		
		sts &= validFld;
		for (i=15, j=-1; i>=0; i--)
		{
			bitfld = 1<<i;
			if (bitfld & validFld)
			{
				j++;
				if (bitfld & sts)
				{
					almMsgAppend("Received Status Event, ", false);
					almMsgAppend(msgs[j], true);
				}
			}
		}
		
		return j;  // valid event bits parsed
}
void alarmHandler(uint16_t errGrp)
{
	uint16_t eventSts;
	enum errWriteType rwerr=noProtocolErr;
	int eventsParsed;
	
	switch (errGrp)
	{
	  case ferGany:
	    eventSts = ReadReg(Ga1, &rwerr);
		eventsParsed = parseStsbits(eventSts, fGa1, gyroErrMsg);
	    eventSts = ReadReg(Ga2, &rwerr);
	    parseStsbits(eventSts, fGa2, &gyroErrMsg[eventsParsed]);
	  break;
	  
	  case ferAany:
	    eventSts = ReadReg(Aa1, &rwerr);
	    eventsParsed = parseStsbits(eventSts, fAa1, accelErrMsg);
	    eventSts = ReadReg(Aa2, &rwerr);
	    parseStsbits(eventSts, fAa2, &accelErrMsg[eventsParsed]);
	  break;
	  
	  case ferTany:  // Handle All others
	    eventSts = ReadReg(Oa1, &rwerr);
	    eventsParsed = parseStsbits(eventSts, fOa1, accelErrMsg);
	    eventSts = ReadReg(Oa2, &rwerr);
	    parseStsbits(eventSts, fOa2, &accelErrMsg[eventsParsed]);
	  break;
	  
	  default:  // default case should be blank, but need to force read of status for test
	  	    eventSts = ReadReg(Ga1, &rwerr);
	  	    eventsParsed = parseStsbits(eventSts, fGa1, gyroErrMsg);
	  	    eventSts = ReadReg(Ga2, &rwerr);
	  	    parseStsbits(eventSts, fGa2, &gyroErrMsg[eventsParsed]);
			eventSts = ReadReg(Aa1, &rwerr);
			eventsParsed = parseStsbits(eventSts, fAa1, accelErrMsg);
			eventSts = ReadReg(Aa2, &rwerr);
			parseStsbits(eventSts, fAa2, &accelErrMsg[eventsParsed]);
			eventSts = ReadReg(Oa1, &rwerr);
			eventsParsed = parseStsbits(eventSts, fOa1, accelErrMsg);
			eventSts = ReadReg(Oa2, &rwerr);
			parseStsbits(eventSts, fOa2, &accelErrMsg[eventsParsed]);
	  break;	
	}
}

void demuxTop(uint16_t err)
{
  uint16_t anyerr = err & ferAny;
  uint16_t errcopy = err;
  //uint16_t gyroErr = err & ((ferGX | ferGY | ferGZ));
  //uint16_t accelErr = err & ((ferAX | ferAY | ferAZ));
  //uint16_t tempErr = err & ((ferT1| ferT2));
  
	
  if (!anyerr && err) { almMsgAppend("All errors Or-ed=0, but Groups show errors: ", false); almDataAppend (errcopy, true); }

//  if (err & ferGany)
    alarmHandler (err & ferGany);
//  if (err & ferAany)
    alarmHandler (err & ferAany);
//  if (err & ferTany)
    alarmHandler (err & ferTany);
}

void getEvents()
{
	uint16_t errTop;
	errTop = SMtopstatus();
almMsgbufclear();
//	if (errTop)
		demuxTop(errTop);
	
	
	
almMsgPrint();
}

