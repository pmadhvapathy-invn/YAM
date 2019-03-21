/*
 * CFile1.c
 *
 * Created: 2/5/2019 2:36:38 PM
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
#include "yamAPI.h"

const pwr2raise15 = (1<<15);
extern const LOWER8Of32;
extern const LOWER16Of32;

static uint16_t filterTable[] =
{
	1,  2,  3,  4,  5,  6,      // gyro 10
	7,  8,  9, 10, 11, 12,      // gyro 12.5
	13, 14, 15, 19, 20, 21,     // gyro 27
	22, 23, 24, 25, 26, 27,     // gyro 30
	28, 29, 30, 31, 35, 36,     // gyro 46
	37, 38, 39, 40, 41, 42     // gyro 60
};


uint16_t afsrValTab[] = {2, 4, 16, 32} ;
uint16_t aflrValTab[] = {4, 8, 16, 32, 64};
uint16_t gfsrValTab[] = {491, 983, 1966, 2949, 328, 655, 1311, 1966, 92, 184, 369, 737, 61, 123, 246, 491};

	
static AccelFsrtype		currentAccelFSR;
static AccellRestype	currentlowresAccelFSR;
static GyroFSRtype		currentGyroFSR;





void saveAccelfsr(AccelFsrtype afsr,  AccellRestype alrfsr)
{
	currentAccelFSR = afsr;
	currentlowresAccelFSR = alrfsr;
}

static void savegyroFSR (GyroFSRtype fsr )
{
	currentGyroFSR = fsr;
}

void getFSRval(uint16_t *afsrp,  uint16_t *alrfsrp, uint16_t *gfsrp)
{
	if (afsrp)
	  *afsrp = afsrValTab[currentAccelFSR];
	if (alrfsrp)
	  *alrfsrp = aflrValTab[currentlowresAccelFSR];
	if (gfsrp)
	  *gfsrp = gfsrValTab[currentGyroFSR];
}
//**********************************
void AccelFSRset (AccelFsrtype afsr, AccellRestype alrfsr)
{
	uint16_t regVal;
	enum errWriteType rwerr = noProtocolErr;
	switch(afsr)
	{
		case afsr2:
		    regVal=5;
			if (alrfsr == flr4) regVal=4;
        break;
		case afsr4:
			regVal=7;
			if (alrfsr == flr4) regVal=6;
		break;	
		case afsr16:
			regVal=1;
			if (alrfsr == flr32) regVal=0;
		break;		
		case afsr32:
		default:
			regVal=3;
			if (alrfsr == flr32) regVal=2;
		break;

	}
	
	// 	read and save bank
	//uint16_t currbank = yamRead(0, Bank);
	
	bankselect(bFSa);
	rwerr = WriteReg(FSa, regVal);
	bankselect(0);
		
	saveAccelfsr(afsr,  alrfsr);
}


//**********************************
void GyroFSRset(GyroFSRtype fsr )
{
	enum errWriteType rwerr=noProtocolErr;
	bankselect(bFSg);
	rwerr = WriteReg(FSg, fsr);
	bankselect(0);
	savegyroFSR( fsr);  // save to avoid reading from chip for mdps conversion.
}

//**********************************
void ReadAxesAndTemp(int16_t *accelp, int16_t *accelLowresp, int16_t *gyrop, int16_t *temperaturep, int16_t *anythingelsep)
{
	regAddrType reg;
    WhoAmIType err=YamOK;
	enum errWriteType rwerr = noProtocolErr;
	
	if (anythingelsep)  
	   err = getWhoAmI((uint16_t*) anythingelsep);
	if (accelp)
		ReadAccel(accelp);
	if (gyrop) 
	 	for (reg=Gx; reg<=Gz; reg++, gyrop++)
	 		*gyrop = ReadReg(reg, &rwerr);
    if (temperaturep)
	   ReadTempSensors(temperaturep);
	if (accelLowresp)
	    ReadAccelLowRes(accelLowresp);

}

//**********************************
void ReadTempSensors(int16_t *readArr)
{
	enum errWriteType rwerr = noProtocolErr;
	readArr[0] = ReadReg(T1, &rwerr);
	readArr[1] = ReadReg(T2, &rwerr);
	readArr[2] = ReadReg(T12, &rwerr);
}

void ReadAccel(int16_t *p)
{
	regAddrType reg;
	enum errWriteType rwerr = noProtocolErr;
	for (reg=Ax; reg<=Az; reg++, p++)
	*p = ReadReg(reg, &rwerr);

}

void ReadAccelredundant (int16_t *p)
{
	
}
//**********************************
void ReadAccelLowRes(int16_t * p)
{
	enum errWriteType rwerr = noProtocolErr;
	for(int addr=Axlr; addr <= Azlr; addr++ , p++)
	  *p = ReadReg(addr, &rwerr);
}
//**********************************
WhoAmIType getWhoAmI(uint16_t * wmi)
{
	WhoAmIType ret = YamOK;
	enum errWriteType rwerr = noProtocolErr;
	uint16_t myID;
	myID = ReadReg(Fxd, &rwerr); 
	*wmi = myID;
	
	if (myID != FIXEDYAM)
	  ret = YamNotOK;
	  
	return ret;

}
//***
static uint16_t alignfield2mask(uint16_t v2shift, uint16_t bitfield)
{
	int i;
	uint16_t retval;
	
	for (i=0; i<16; i++)
	{
		if ((bitfield>>i)&1)
			break;
	}
	if (i==16) 
	  {  printf("bitfield is empty\r\n");  getchar(); printf("Press any key to continue \r\n"); }
	
	retval	= v2shift<<i;
	return retval;
}

static gyrofilterBWType currentGyroFilter=0;
static accelfilterBWType currentAccelFilter=0;

static char *gyrofilterString[]=  {"10Hz", "12Hz", "27Hz", "30Hz", "46Hz", "60Hz"} ;
static char *accelfilterString[]=  {" 10Hz", " 46Hz", " 60Hz", "250Hz", "300Hz", "400Hz"} ;
static float gyrofilterBW[] = {10.0, 12.5, 27.0, 46.0, 60.0};
static float accelfilterBW[] = {10.0, 46.0, 60.0, 250.0, 300.0, 400.0};


	
void getCurrFilter(struct filterInfoType * gfp, struct filterInfoType *afp, struct filterInfoType *afstr)
{
  if (gfp) {
	  gfp->filterIndex = currentGyroFilter;
	  gfp->BW = gyrofilterBW[currentGyroFilter];
	  gfp->str = gyrofilterString[currentGyroFilter];  
  }
  if (afp) {
	  afp->filterIndex = currentAccelFilter;
	  afp->BW = accelfilterBW[currentAccelFilter];
	  afp->str = accelfilterString[currentAccelFilter];
  }
  if (afstr) {
	  yamMsg("Unimplemented Low Res Filter Return", yamwait, 0);
  }
  
  
}
//**********************************
void SetFilter (AxisType axis,  gyrofilterBWType gyroflt, accelfilterBWType accelflt)
{
    uint16_t index =  gyroflt*6 + accelflt;  // relies on enums starting from 0
	uint16_t regCode, regbitfield, value;
	uint16_t mask, temp;
	regAddrType regaddr;
	enum errWriteType rwerr=noProtocolErr;
	
	if(currentBank() != bZ_flt)  // all filter settings in bank 0, so ok to check bank for z-filter only
	  bankselect(bZ_flt);
	  
	regCode = filterTable[index];
	
	mask		= axis == X ? fX_flt : axis == Y ? fY_flt : fZ_flt;
	regaddr		= axis == X ?  X_flt : axis == Y ?  Y_flt :  Z_flt;
								 
	regbitfield = alignfield2mask(regCode, mask);
	temp = ReadReg(regaddr, &rwerr);
	temp &= ~mask;
	temp |= regbitfield & mask;
	rwerr = WriteReg(regaddr, temp);
	currentGyroFilter = gyroflt;
	currentAccelFilter = accelflt;
}

static float bits2real(int16_t bits, float fsrVal)
{
  float f = (float)bits*(float)fsrVal/(float)pwr2raise15;
  return f;
}

float afsrTab[] = {2.048, 4.096, 16.384, 32.768};  // depends on AccelFsrType not changing definition of members.
float alrfsrTab[] = {4.096, 8.192, 16.384, 32.768, 65.536};  // depends on AccelFsrType not changing definition of members.
//**********************************************
float accel2gee (int16_t accelVal, bool hiLoRes)
{
	float fsrVal = hiLoRes == 0 ? afsrTab[currentAccelFSR] : alrfsrTab[currentlowresAccelFSR];
	float accelgee = bits2real(accelVal, fsrVal);
	
	return (accelgee);  
}

float faccel2gee(float fAccel, bool hiLoRes)
{
	float fsrVal = hiLoRes == 0 ? afsrTab[currentAccelFSR] : alrfsrTab[currentlowresAccelFSR];
	float faccelgee = fAccel*fsrVal/(float)pwr2raise15;
	return (faccelgee);	
}




float gfsrTab[] = {491.0, 983.0, 1966.0, 2949.0, 328.0, 655.0, 1311.0, 1966.0, 92.0, 184.0, 369.0, 737.0, 61.0, 123.0, 246.0, 491.0};
//**********************************************
float gyro2dps (int16_t gyroVal)
{
	float fsrVal = gfsrTab[currentGyroFSR];
	float gyrodps = bits2real(gyroVal, fsrVal);
	
	return (gyrodps); 
}
float fgyro2dps (float gyroVal)
{
	float fsrVal = gfsrTab[currentGyroFSR];
    float gyrodps = (float)gyroVal*fsrVal/(float)pwr2raise15;
	
	return (gyrodps);
}
float temp2Centigrade(int16_t temperatureBits)
{
	float f =  temperatureBits/TEMP2CENTgrad  + TEMP_SENSOR_ROOMTEMP;	
	return(f);
}

float accelnormfloat(int16_t x1, int16_t  x2, int16_t x3, bool Lores  // input
                     ,float *nfx1, float *nfx2, float *nfx3   //output
					)
{
	float fx1, fx2, fx3, fNorm;

	
	fx1 = accel2gee(x1, Lores);
	fx2 = accel2gee(x2, Lores);
	fx3 = accel2gee(x3, Lores);
	
	if (nfx1) *nfx1 = floatAbs(fx1);
	if (nfx2) *nfx2 = floatAbs(fx2);
	if (nfx3) *nfx3 = floatAbs(fx3);
	
	
	fNorm = fx1*fx1 + fx2*fx2 + fx3*fx3;
	fNorm = sqrtf(fNorm);
	return fNorm;
}

//********************************
void ReadID(uint8_t *gyroRevp, uint8_t *accelRevp, uint8_t *sensorRevp, uint32_t *serialIDp)
{
	int32_t regVal;
	uint16_t b2s;
	enum errWriteType rwerr = noProtocolErr;
		
	if (accelRevp || gyroRevp)
		regVal = ReadReg(Arev, &rwerr);
	
	if (accelRevp) {
		b2s = lsbSet(fArev);
		*accelRevp = (regVal >> b2s) & (fArev>>b2s);
	}
	if (gyroRevp) {
		b2s = lsbSet(fGrev);
		*gyroRevp = (regVal >> b2s) & (fGrev>>b2s);
	}
	
	if (sensorRevp) {
		regVal = ReadReg(Srev, &rwerr); 
		b2s = lsbSet(fSrev);
		*sensorRevp = (regVal >> b2s) & (fSrev>>b2s);
	}
	
	if (serialIDp) {
		int16_t msb = ReadReg(SerIdmsb, &rwerr); 
		int16_t lsb  = ReadReg(SerIdlsb, &rwerr); 
		// short-circuit the bit shift - all bits are valid
		*serialIDp = ((uint32_t)msb)<<16 | ((uint32_t)lsb & LOWER16Of32);
	}
	
	
}
