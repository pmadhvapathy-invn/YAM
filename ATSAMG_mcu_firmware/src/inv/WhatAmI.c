/*
 * WhatAmI.c
 *
 * Created: 1/15/2019 2:42:59 PM
 *  Author: pmadhvapathy
 */ 
#include <asf.h>
#include "protocol_functions.h"
#include "inv_uart.h"
#include "inv_i2c.h"
#include "inv_spi.h"
#include "arm_version.h"
#include "math.h"
#include "delay.h"
#include "IAM20685Reg.h"
#include "yamtests.h"
#include "lockUnlock.h"
#include "bsp.h"
#include "utils.h"
#include "yamAPI.h"
#include "yamAlarm.h"

#include <stdio.h>
#include <stdlib.h>



static char tbuf[100];
int16_t regvalArr[20];
int16_t sgnvalArr[20];

void achievedsuccess ()
{
	printf("IAM 20685, the Auto Chiiiip\n");
	getchar();
}

void yamMsg (char * s, yErrType next, uint32_t err)
{
	printf("%s ", s); 
	if (err) { printf("got Error %d", err);}
	printf("\r\n");

	if (next==xit)
	  exit(0);
	if (next == yamwait){
	  printf(" Press any key.. : "); getchar(); printf(".. thanks\r\n");
	}
}

//extern char tbuf[];
//static	int i;
static uint16_t temp;

bool testIterControl (uint16_t iter, char *p)
{
	bool runtest;
	if (iter == 0) { runtest = false; sprintf (tbuf, "Skipping Test: %s", p); yamMsg(tbuf, keepgoin, 0);}
	else		   { runtest = true;  sprintf (tbuf, "Running Test: %s", p); yamMsg(tbuf, keepgoin, 0);}
	return runtest;
}

const breakIter = 65535;
uint16_t TestTestReg( int iter)
{
	enum errWriteType err = 0;
	int i;
	int16_t anothertemp;
	uint32_t protoresponse;
	enum errWriteType  protoErr=noProtocolErr; 
	
	if (!testIterControl(iter, "TestTestReg")) return 0;

	for (;iter--;)
	{
		sprintf(tbuf, "iter: %d ", iter);
		yamMsg(tbuf, keepgoin, 0);

		for (i= 0; i <= 0xFFFF; i++)
		{
			if (i==breakIter)
			  printf("Start Salae capture \r\n");
			if (i%20000  == 1) printf ("testing value %d\r\n", i);
//			protoErr = WriteReg(Tr, i);
WriteReg(Arev, 0x1234);  // intervening write
			protoErr = WriteReg(Tr, 0x5678);
			if (protoErr & 0)
			  printf ("Write PrtoError: %d\r\n", protoErr);
		  
//			regWrite(Tr, i);
//WriteReg(Arev, 0x1234);  // intervening write
            temp = ReadReg(Tr, &protoErr);
			if (protoErr) { uint16_t wmi;
			  //printf ("Read PrtoError: %d\r\n", protoErr);
			  getWhoAmI(&wmi) ;
			  //printf("I Am : %04X \r\n", wmi);
			}
//WriteReg(Z_flt, 0x1234);  // post write

//			temp = readReg(0, Tr);
//anothertemp = ReadReg(Z_flt, &protoErr);
//			if (protoErr)
//			printf ("Read PrtoError: %d\r\n", protoErr);
		    
			if (0x5678 != temp) //(i != temp)
			{
			  sprintf(tbuf, "i: %04X  Tr: %04X \r\n", i, temp) ;
			  err++;
			  yamMsg(tbuf, keepgoin, err);
			}    
		}
	}

	yamMsg("Test Register test done", (err || protoErr) ? yamwait : keepgoin, err ? err : protoErr); 
	return err;
}


uint16_t TestWhoAmI(uint16_t whoYamSupposedToBe, int iter)
{
	enum errWriteType err = 0;
	uint16_t anotherTemp ;
	
	if (!testIterControl(iter, "TestWhoAmI")) return 0;

	 for (; iter>=0; iter--)
	 {  sprintf(tbuf, "iter: %d", iter); yamMsg(tbuf, keepgoin, 0); 
WriteReg(Tr, 0x1234);
		 temp = ReadReg(Fxd, &err);		 
		 if (temp != whoYamSupposedToBe)
		 {
			sprintf(tbuf, "Identity Crisis on iter:%d Am : %04X, should be: %04X", iter, temp, whoYamSupposedToBe);
			err++;
			yamMsg(tbuf, keepgoin, err);
		 }	 

	 }
	 
	 yamMsg("WhoAmI test done", err ? yamwait : keepgoin, err);
	 return err;
}

uint16_t TestBankReg(int iter)
{
	uint16_t err = 0;
	int errCount = 0;
	int errMax = 10;
	int i;
	
	if (!testIterControl(iter, "TestBankReg")) return 0;

    uint16_t currbank = 0;
	
		for (i= 0; i < 65536; i++)
		{
			yamWrite(0, Bank, i, ALLREGBITS, false);
			temp = yamRead(0, Bank); //  readReg(0, Tr);
		    
			if (i != temp)
			{
				sprintf(tbuf, "i: %04X  Bank: %04X \r\n", i, temp) ;
				err++;
				yamMsg(tbuf, keepgoin, err);
			}    
			else if ((errCount < errMax) && (( i>1 && i<6) || (i>7))) 
			{
				  sprintf(tbuf, "Error: illegal bank should not be allowed but is @ Bank value: %d", i);
				  err++;
				  yamMsg(tbuf, keepgoin, err);
				  errCount++;
				  if(errCount == (errMax-1)) {
				   				  sprintf(tbuf, "Suppressing too many error messages");
				   				  yamMsg(tbuf, keepgoin, errCount);
				  }

			}
		}
		

	yamMsg("Test Bank Register done", err ? yamwait : keepgoin, err); 
	return err;

}

 float spec_deltaT = 1.5f; 


uint16_t TestTempSensor(int iter)
{
	uint16_t err=0;
	float t1_f, t2_f, tDiff_f, tDelta_f;

	if (!testIterControl(iter, "TestTempSensor")) return 0;

	for ( ; iter; iter--)
	{
		ReadTempSensors(regvalArr);
		t1_f = temp2Centigrade(regvalArr[0]);
		t2_f = temp2Centigrade(regvalArr[1]);
		tDiff_f = t1_f-t2_f;
		tDiff_f = floatAbs (tDiff_f);  // computed diff
		
	
		tDelta_f = temp2Centigrade(regvalArr[2]) - TEMP_SENSOR_ROOMTEMP;
        tDelta_f = floatAbs (tDelta_f);  // read diff
		
		//printf("T1,T2: %4.2f, %4.2f, tdc, tdr %4.2f, %4.2f\r\n", t1_f, t2_f, tDiff_f, tDelta_f);
		
		if (tDelta_f > spec_deltaT)
		{
		  sprintf(tbuf, "Delta-T register OOSpec %f:  Computed %f    Spec:  %f", tDelta_f, tDiff_f, spec_deltaT);
		  err++;
		  yamMsg(tbuf, keepgoin, err);
		}
		if (tDiff_f > spec_deltaT)
		{
			sprintf(tbuf, "T1-T2 OOSpec %f:  Computed %f   Spec:  %f", tDelta_f, tDiff_f, spec_deltaT);
			err++;
			yamMsg(tbuf, keepgoin, err);
		}

	}
	
	yamMsg("TestTempSensor Done", err ? yamwait : keepgoin, err); 

	return err;
};

int16_t LrAccelTestBatchLen = 1000;  // Num samples to average for noise estimation

const float lowResTolerance = 0.015f;


uint16_t LrAccelTestbydiff(int iter)
{
	uint16_t afsr, alrfsr, gfsr;
	uint16_t err=0;
	int i, j;
	float average[6];
	float absdiff;
	int fsrRatio;
	int hinrgi[3], lonrgi[3];
	int hidiff, lodiff;
	
	if (!testIterControl(iter, "LrAccelTestbydiff")) return 0;

	getFSRval(&afsr, &alrfsr, &gfsr);
	
	fsrRatio = alrfsr/afsr;
	
	for ( ; iter>=0; iter--) {
		printf("Low Res Accel Test Iter: %d\r\n", iter);
		for(int axis =0; axis < 3; axis++)
				hinrgi[axis] = lonrgi[axis]= 0;

		for (i=LrAccelTestBatchLen; i>=0; i--) {
			ReadAxesAndTemp(regvalArr, regvalArr+3, NULL, NULL, NULL);
			ReadAxesAndTemp(regvalArr+6, regvalArr+9, NULL, NULL, NULL);
			for(int axis=0; axis<3; axis++) {
				hidiff = regvalArr[6+axis] - regvalArr[0+axis];
				lodiff = regvalArr[9+axis] - regvalArr[3+axis];
				hinrgi[axis] += abs(hidiff);
				lonrgi[axis] += abs(lodiff);
			}  // axis loop
		} // av len loop
		
		for (int axis=0; axis<3; axis++)
		{
			float nrgiratio = (float)hinrgi[axis] / (float) lonrgi[axis];
			float diff2fsr = 1.0f - nrgiratio/(float)fsrRatio;
			diff2fsr = floatAbs(diff2fsr);
			if (diff2fsr > lowResTolerance) {
			   err++;
			   sprintf(tbuf, "%s: %5.3f Tolerance:  %5.3f", "Low Resolution Accel exceeding FSR setting tolerance", diff2fsr, lowResTolerance);
			   yamMsg(tbuf, keepgoin, err);
			}
		}
		if (err)
		{
			printf("Bit Nergi levels: \r\n");
			printf("Hi: ");
			for (int axis=0; axis<3; axis++)
			printf(" %5d" , hinrgi[axis]);
			printf("\r\nLo: ");
			for (int axis=0; axis<3; axis++)
			printf(" %5d" , lonrgi[axis]);		
		}	
	} // iter loop
	yamMsg("LrAccelTestbydiff Done", err ? yamwait : keepgoin, err); 
	return err;
}

struct accelFSRtuple accelAllowedFSRcombo []= { {afsr16, flr32},  {afsr16, flr64}, {afsr32, flr32}, {afsr32, flr64}
, {afsr2, flr4},  {afsr2, flr16}, {afsr4, flr4}, { afsr4, flr8}
};

struct accelFSRtuple accelnormalFSRcombo []= { {afsr2, flr4}, {afsr4, flr4}, {afsr16, flr32}, {afsr32, flr32} };

enum gyroFSRtype1 gyroFSRtbl[] = { gfsr61, gfsr92, gfsr123, gfsr184, gfsr246, gfsr328,  gfsr369, gfsr491
								   , gfsr655, gfsr737, gfsr983, gfsr1311, gfsr1966, gfsr2949
};
	
void setnewAccelfsrFromTable(int fsrTableindx )
{
	AccelFsrtype  setfsr;
	AccellRestype setflr;
	uint16_t afsr, alrfsr;

	setfsr = accelnormalFSRcombo[fsrTableindx].afsr;
	setflr = accelnormalFSRcombo[fsrTableindx].alrfsr;
				
	AccelFSRset (setfsr, setflr);
	getFSRval(&afsr, &alrfsr, NULL);
//	printf("testing {Accel FSR, Lowres FSR}: %2d, %2d\r\n", afsr, alrfsr);
	
	delay_ms(200);
				
}

enum accelfilterBWType1 accelFilters[] = {a10hz, a46hz, a60hz, a250hz, a300hz, a400hz};
enum gyrofilterBWType1 gyroFilters[] = {g10hz, g12_5hz, g27hz, g30hz, g46hz, g60hz};
	
#define MAXSAMPLELEN 2049
typedef struct noiseMeasure { uint32_t sad; float meanX; float meanY; float meanZ; float fnormX; float fnormY; float fnormZ; } noiseMasureType;
struct noiseMeasure sadas( int16_t *p, int sampleNum)
{
	int totalSamples;
	static uint32_t sadass;
	static int32_t sum[3], samples[3][MAXSAMPLELEN];
	float  norm[3], mean[3]; 
	static int16_t v[3];
	static individualNorm[3];
	int i, si;
	int16_t *pIn=p;
	struct noiseMeasure retVal = {0, 0.0f, 0.0f, 0.0f};
	
	if (!p) {
		totalSamples = sampleNum > MAXSAMPLELEN ? MAXSAMPLELEN : sampleNum;
		for (i=0; i<3; i++) mean[i] = (float) sum[i]/ (float) totalSamples;
		for (i=0; i<3; i++) { norm[i]=0.0f;
			for (si=0; si<totalSamples; si++)
			  norm[i] +=  floatAbs((float) samples[i][si] - mean[i]);	
		}
		for (i=0; i<3; i++)  norm[i] /= (float) sampleNum;
		retVal.sad = sadass; 
		retVal.fnormX = norm[0]; retVal.fnormY = norm[1]; retVal.fnormZ = norm[2];
		retVal.meanX = mean[0]; retVal.meanY = mean[1]; retVal.meanZ = mean[2];
		
		return retVal;
	}  
	else if (sampleNum <= MAXSAMPLELEN)  //  the rest discarded
	{   for (pIn=p, i=0; i<3; i++) samples[i][sampleNum] = *pIn++;
	    if (!sampleNum) { sadass=0; for (pIn=p, i=0; i<3; i++) {sum[i] = *pIn++;} }
	    else {  
		  for (pIn=p, i=0; i<3; i++) {
			 individualNorm[i] = (uint32_t) abs(pIn[i] - v[i]);  
			 sadass += individualNorm[i];	
			 sum[i] += pIn[i];	  
		  }
		  for (pIn=p, i=0; i<3; i++) { v[i] = *pIn++; }
	    }
	}
	return retVal;
} 

void printNoise (struct noiseMeasure n, bool Accel_gyro, bool hiLores, uint32_t sampleLen)
{
  	float fMeanX, fMeanY, fMeanZ, fnormX, fnormY, fnormZ;
	float howsad;
	uint16_t afsr, gfsr, filter;
	struct filterInfoType  accelFilterInfo, gyroFilterInfo;
	char fltrStr[11];
	
	getFSRval(&afsr, &gfsr, NULL);
	getCurrFilter(&gyroFilterInfo, &accelFilterInfo, NULL);
	
	sprintf (fltrStr, "%s", Accel_gyro ? accelFilterInfo.str : gyroFilterInfo.str);
	howsad = (float) n.sad/ (float) sampleLen;
	if (Accel_gyro) {
		fMeanX = faccel2gee(n.meanX, hiLores) * 1000.0;
		fMeanY = faccel2gee(n.meanY, hiLores) * 1000.0;
		fMeanZ = faccel2gee(n.meanZ, hiLores) * 1000.0;
		fnormX = faccel2gee(n.fnormX, hiLores) * 1000.0;
		fnormY = faccel2gee(n.fnormY, hiLores) * 1000.0;
		fnormZ = faccel2gee(n.fnormZ, hiLores) * 1000.0;
	
	}
	else {
		fMeanX = fgyro2dps(n.meanX) * 1000.0;
		fMeanY = fgyro2dps(n.meanY) * 1000.0;
		fMeanZ = fgyro2dps(n.meanZ) * 1000.0;
		fnormX = fgyro2dps(n.fnormX) * 1000.0;
		fnormY = fgyro2dps(n.fnormY) * 1000.0;
		fnormZ = fgyro2dps(n.fnormZ) * 1000.0;
	}
	 
	printf("[fsr, filt] = %2d ", Accel_gyro ? afsr : gfsr); 
	if (Accel_gyro) printf("gee "); else printf("dps ");
	printf("%s bw SADAS { Mean XYZ}:, { AMD XYZ}: %6.2f,  %6.2f, %6.2f, %6.2f  %6.2f, %6.2f, %6.2f  \r\n", fltrStr, howsad, fMeanX, fMeanY, fMeanZ, fnormX, fnormY, fnormZ);
}

uint16_t TestAccelFilterbyNoise( int iter)
{
	uint16_t err=0;
	int i, afsrindx, afsrTblLen;
	struct noiseMeasure	noise;
	int ai;
	int si;
#define TESTACCELFILTERBYNOISE_SAMPLESIZE 1000

	if (!testIterControl(iter, "TestAccelFilterbyNoise")) return 0;
	
	afsrTblLen = sizeof(accelnormalFSRcombo) / sizeof(accelnormalFSRcombo[0]);

	for ( ; iter>=0; iter--) {  //printf(" TestAccelFilterbyNoise: %d\r\n", iter);
		for (afsrindx=0; afsrindx<afsrTblLen; afsrindx++)
		{
		   setnewAccelfsrFromTable (afsrindx);		   
		   for(ai=0; ai < (sizeof(accelFilters)/sizeof(accelFilters[0])); ai++)
		   {
				if (iter%100 == 99) printf("TestAccelFilterbyNoise Iter: %d, accelfilter : %d\r\n", iter, ai);
	
				SetFilter(X, gyroFilters[0], accelFilters[ai]);
				SetFilter(Y, gyroFilters[0], accelFilters[ai]);
				SetFilter(Z, gyroFilters[0], accelFilters[ai]);
				delay_ms(200);
				
				for (si=0; si<TESTACCELFILTERBYNOISE_SAMPLESIZE+1; si++)
				{
					ReadAxesAndTemp(regvalArr, NULL, NULL, NULL, NULL);			
					noise = sadas( si == TESTACCELFILTERBYNOISE_SAMPLESIZE ? NULL : regvalArr, si);
				}
				printNoise(noise, true, false, TESTACCELFILTERBYNOISE_SAMPLESIZE);
			}  // filter loop
		  } // Afsr loop				
	}  // iter loop
	
	yamMsg("TestAccelFilterbyNoise Done", err ? yamwait : keepgoin, err);

	return err;
}

uint16_t TestGyroFilterbyNoise( int iter)
{
	uint16_t err=0;
	int i, gfsrindx, gfsrTblLen;
	uint32_t norm, normG;
	struct noiseMeasure	noise;
	int gi;
	int si;
	#define TESTGYROFILTERBYNOISE_SAMPLESIZE 1000

	if (!testIterControl(iter, "TestGyroFilterbyNoise")) return 0;
	gfsrTblLen = sizeof(gyroFSRtbl) / sizeof(gyroFSRtbl[0]);

	for ( ; iter>=0; iter--) {  printf(" TestGyroFilterbyNoise: %d\r\n", iter);
		for (gfsrindx=0; gfsrindx<gfsrTblLen; gfsrindx++)
		{
			GyroFSRset ( gyroFSRtbl[gfsrindx]);
			for(gi=0; gi < (sizeof(gyroFilters)/sizeof(gyroFilters[0])); gi++)
			{
				if (iter%100 == 99) printf("TestGyroFilterbyNoise Iter: %d, gyrofilter : %d\r\n", iter, gi);
				
				SetFilter(X, gyroFilters[gi], accelFilters[0]);
				SetFilter(Y, gyroFilters[gi], accelFilters[0]);
				SetFilter(Z, gyroFilters[gi], accelFilters[0]);
				delay_ms(200);
	
	
				for (si=0; si<TESTACCELFILTERBYNOISE_SAMPLESIZE+1; si++)
				{
					ReadAxesAndTemp(NULL, NULL, regvalArr, NULL, NULL);
					noise = sadas( si == TESTGYROFILTERBYNOISE_SAMPLESIZE ? NULL : regvalArr, si);
				}
				printNoise(noise, false, false, TESTGYROFILTERBYNOISE_SAMPLESIZE);

			}  // filter loop
		} // Afsr loop
	}  // iter loop
	
	yamMsg("TestGyroFilterbyNoise Done", err ? yamwait : keepgoin, err);

	return err;
}


const   float normTolerance =  0.015f;
static	float normHi, normLo;
static  float normAxis[6];
static 	float ftemp;




uint16_t AccelFSRtest(int iter)
{
	uint16_t err=0;
	uint16_t afsr, alrfsr;
	AccelFsrtype  setfsr;
	AccellRestype setflr;
	int32_t fsrRatio;
	float normA[3], normAlr[3];
	int i, fsrindx, fsrTblLen;
	fsrTblLen = sizeof(accelAllowedFSRcombo) / sizeof(accelAllowedFSRcombo[0]);
	if (!testIterControl(iter, "AccelFSRtest")) return 0;

	SetFilter (X,  g10hz, a10hz);
	SetFilter (Y,  g10hz, a10hz);
	SetFilter (Z,  g10hz, a10hz);
	
//	{ enum errWriteType e;  uint16_t yzf = ReadReg(0x0C, &e);  uint16_t xf = ReadReg(0x0E, &e); printf("Filter Regs: {X, YZ} = %04X, %04X\r\n ", xf, yzf);}
	
	for ( ; iter>=0; iter--) {  printf(" Accel FSR Test Iteration: %d\r\n", iter);
		for (fsrindx=0; fsrindx<fsrTblLen; fsrindx++) 
		{
			setfsr = accelAllowedFSRcombo[fsrindx].afsr;
			setflr = accelAllowedFSRcombo[fsrindx].alrfsr;
			
			AccelFSRset (setfsr, setflr);
			getFSRval(&afsr, &alrfsr, NULL);
			fsrRatio = alrfsr/afsr;
			
            printf("testing {Accel FSR, Lowres FSR, Ratio }: %2d, %2d, %2d \r\n", afsr, alrfsr, fsrRatio); 
			if (i%100 == 0) printf("Accel FSR Test Iter: %d\r\n", iter);
			delay_ms(2000);
			ReadAxesAndTemp(regvalArr, regvalArr+3, NULL, NULL, NULL);
			normHi = accelnormfloat(regvalArr[0], regvalArr[1], regvalArr[2], false, normA, normA+1, normA+2);
			normLo = accelnormfloat(regvalArr[3], regvalArr[4], regvalArr[5], true, normAlr, normAlr+1, normAlr+2);
		
			err += normHi > (1.0f + normTolerance) ? 1 : normHi < (1.0f - normTolerance) ? 1 : 0;
			err += normLo > (1.0f + normTolerance) ? 1 : normLo < (1.0f - normTolerance) ? 1 : 0;
		
			if (err) {
				printf("%s  Regular Accel Norm  %4.2f  LowRes Accel Norm %4.2f,  Tolerance: %4.3f", "AccelNorm OOSpec: ", normHi, normLo, normTolerance);
				printf("\r\n");
				printf("%s %4.3f  %4.3f  %4.3f ", "Axes norms[Z, X, Y]: ", normA[2], normA[1], normA[0]);
				printf("%s %4.3f  %4.3f  %4.3f ", "Axes norms[Z, X, Y]: ", normAlr[2], normAlr[1], normAlr[0]);
				printf("\r\n");
			}
			if (err) {
				sprintf(tbuf, "%s %d", "AccelFSRtest Iteration done with error count: ", err);
				yamMsg(tbuf, keepgoin, err);
			}
		} // fsr loop
	} // iter loop
	yamMsg("AccelFSRtest Done", err ? yamwait : keepgoin, err);
	return err;
}

uint16_t AccelNormtest(int iter)
{
	uint16_t err=0;
	uint16_t afsr, alrfsr;
	int32_t fsrRatio;
	float normA[3], normAlr[3];
	int i;
	
	if (!testIterControl(iter, "AccelNormtest")) return 0;

	getFSRval(&afsr, &alrfsr, NULL);
	fsrRatio = alrfsr/afsr;
	
	for ( ; iter>=0; iter--) {
		if (i%100 == 0) printf("Accel Norm Test Iter: %d\r\n", iter);
		ReadAxesAndTemp(regvalArr, regvalArr+3, NULL, NULL, NULL);
		normHi = accelnormfloat(regvalArr[0], regvalArr[1], regvalArr[2], false, normA, normA+1, normA+2);
		normLo = accelnormfloat(regvalArr[3], regvalArr[4], regvalArr[5], true, normAlr, normAlr+1, normAlr+2);
		 
		 err += normHi > (1.0f + normTolerance) ? 1 : normHi < (1.0f - normTolerance) ? 1 : 0;
		 err += normLo > (1.0f + normTolerance) ? 1 : normLo < (1.0f - normTolerance) ? 1 : 0;	 
		 
		 if (err) {
			 printf("%s  Regular Accel Norm  %4.2f  LowRes Accel Norm %4.2f,  Tolerance: %4.3f", "AccelNorm OOSpec: ", normHi, normLo, normTolerance);
			 printf("\r\n");
			 printf("%s %4.3f  %4.3f  %4.3f ", "Axes norms[Z, X, Y]: ", normA[2], normA[1], normA[0]);			
			 printf("%s %4.3f  %4.3f  %4.3f ", "Axes norms[Z, X, Y]: ", normAlr[2], normAlr[1], normAlr[0]);
			 printf("\r\n");
	     }
		 if (err) {
			sprintf(tbuf, "%s %d", "AccelNormTest Iteration done with error count: ", err);
			yamMsg(tbuf, keepgoin, err); 	
		 }	
	} // iter loop
	yamMsg("AccelNormTest Done", err ? yamwait : keepgoin, err); 
	return err;
}



uint16_t testPowerupResetRegTest(int iter)
{
	enum errWriteType err=0;
	uint16_t regval, expval;
    int i;
	int testBank=0; //should start from 0 on power up
    int regBank;
	int arrlen = numRegArray();
	 
	if (!testIterControl(iter, "testPowerupResetRegTest")) return 0;

	printf ("Number of registers: %d\r\n", arrlen);
    for ( ; iter; iter--)
	{
		printf ("Power up Reset register test Iter: %d\r\n", iter);
		for(i=0; i<arrlen; i++)
		{
		  printf("iter, index %d %d\r\n", iter, i);
		  regBank = regArray[i].b;
		  if(regBank != testBank)
		  {
			 bankselect(regBank);
			 testBank = regBank;
			 printf("Changed Bank to %d\r\n", regBank);
		  }
		  regval = (uint32_t) ReadReg(regArray[i].r, &err);
		  //regval = (uint32_t) yamRead(0, regArray[i].r);
	  
		  regval &= regArray[i].regfld;
		  expval = regArray[i].defval & regArray[i].regfld;
		  if (regval != expval)
		  {
			 printf("index: %d, Register {b,o} %02X %02X reads %04X instead of %04X \r\n", i, regArray[i].b, regArray[i].r, regval, expval);  
			 err++;
		  }
		}
	}
	bankselect(0);  // restore bank 0
		
    yamMsg("testPowerupResetRegTest Done", err ? yamwait : keepgoin, err); 
    return err;
}

uint16_t IDTest(int iter)
{
	uint16_t err=0;
	enum errWriteType rwerr=noProtocolErr;
	uint8_t gyroRev, accelRev, sensorRev;
	uint32_t serialNum;
	int saved_iter;
	int16_t v1, v2, v3, v4;  // read values
	int16_t w1, w2, w3, w4;  // write values
	int16_t v1w1m, v2w2m;  // mask for valid bits
	  
	if (!testIterControl(iter, "IDTest")) return 0;

	for (saved_iter=iter; iter; iter--) {
		ReadID (&gyroRev, &accelRev, &sensorRev, &serialNum);
		printf(" Gyro Rev: %3d, AccelRev: %3d, SensorRev: %3d, Serial# %24X = %8d (in decimal) \r\n", gyroRev, accelRev, sensorRev, serialNum, serialNum);	
	}
	
	v1w1m = fArev | fGrev;
	v2w2m = fSrev;
	
	for ( iter = saved_iter; iter; iter--) {  printf("ID Test iter: %d\r\n", iter);
		for(int i=0; i<260; i++)
		{
			
			w1 = i;				w1 &=  v1w1m;		
			w2 = (i+1)%65536;   w2 &=  v2w2m;
			w3 = (i+2)%65536; 
			w4 = (i+3)%65536;
			rwerr = WriteReg (Arev, w1);
			//rwerr = WriteReg (Srev, w2);
			rwerr = WriteReg (SerIdmsb, w3);
			rwerr = WriteReg (SerIdlsb, w4);
		
			v1 = ReadReg(Arev, &rwerr);  v1 &= v1w1m;
			//v2 = ReadReg(Srev, &rwerr);  v2 &= v2w2m;
			v3 = ReadReg(SerIdmsb, &rwerr);
			v4 = ReadReg(SerIdlsb, &rwerr);
		
			if (v1 != w1)  { err++; sprintf(tbuf, "A/G Rev RW mismatch. W,R = %04x, %04x \r\n", w1, v1); yamMsg(tbuf, keepgoin, err);}
			//if (v2 != w2)  { err++; sprintf(tbuf, "Sensor Rev RW mismatch. W,R = %04x, %04x \r\n", w2, v2); yamMsg(tbuf, keepgoin, err);}
			if (v3 != w3)  { err++; sprintf(tbuf, "SerialID MSB RW mismatch. W,R = %04x, %04x \r\n", w3, v3); yamMsg(tbuf, keepgoin, err);}
			if (v4 != w4)  { err++; sprintf(tbuf, "SerialID LSB RW mismatch. W,R = %04x, %04x \r\n", w4, v4); yamMsg(tbuf, keepgoin, err);}
		}
	}
	
	yamMsg("IDTest Done", err ? yamwait : keepgoin, err); 
	return 0;  // there are no bad IDs
}

uint16_t testpowerUpSelfTest(int iter)
{
  uint16_t err=0;


  if (!testIterControl(iter, "testpowerUpSelfTest")) return 0;

  printf ("power up self test started \r\n");  
  for ( ; iter--; )
     getEvents();	

  yamMsg("testpowerUpSelfTest Done", err ? yamwait : keepgoin, err); 

  return err;
}

uint16_t testnowdoSelfTest(int iter)
{
  uint16_t err=0;
  printf ("Software invoked self test started \r\n"); 

  if (!testIterControl(iter, "testnowdoSelfTest")) return 0;

  for ( ; iter; iter--)  { printf("Iteration %d\r\n", iter); 
	  selfteststart(); 
	  getEvents();	
  }
  yamMsg("testnowdoSelfTest Done", err ? yamwait : keepgoin, err); 
  return err;
}


uint16_t testResetSelfTest(int iter)
{
	uint16_t err=0;
	enum errWriteType rwerr = noProtocolErr;
	int savediter = iter;

    if (!testIterControl(iter, "testResetSelfTest")) return 0;

	printf ("Software invoked self test started \r\n");
	selftestDisable(true);
	uint16_t  test = ReadReg (0x0E, &rwerr);
	printf(" ST Disable Register:  %04X \r\n", test);
	for ( ; iter; iter--)  { printf("Soft Reset Iteration %d\r\n", iter);
		resetChip(SOFT_RESET);
		getEvents();
	}
	
	for ( iter = savediter; iter; iter--)  { printf("Hard Reset Iteration %d\r\n", iter);
		resetChip(HARD_RESET| SOFT_RESET);
		getEvents();
	}
	
    yamMsg("testResetSelfTest Done", err ? yamwait : keepgoin, err); 
	return err;
}

uint16_t testSelfTestAndXfilterRegister(int iter)
{
	int16_t dis=Bit14, en=0;
	uint16_t  v, yamVal, rrgval;
	uint16_t err=0;
	enum errWriteType rwerr=noProtocolErr;
	uint16_t mask = fSt_dis | fX_flt;
	
    if (!testIterControl(iter, "testSelfTestAndXfilterRegister")) return 0;
	
	for (  ; iter ; iter--)
	{
		printf ("Iter:  %d\r\n", iter);
		for (v=0;  v<=0x7FFF; v+=0x100)
		{
			v &= mask;
			//v=0x0000;
			rwerr = WriteReg(X_flt, v);
			WriteReg(Tr, 0x1234);  //intervening write
			yamVal = ReadReg(X_flt, &rwerr);	
			yamVal &=mask;		
			//rrgval = readReg(bX_flt, X_flt);
			//rrgval &=mask;
			if (yamVal != v)
			{
			  printf("[Write, Read, ReadReg]	= %04X, %04X \r\n", v, yamVal);
			 if (yamVal != v) err++;
			}			
		}		

	}	
    yamMsg("testSelfTestAndXfilterRegister Done", err ? yamwait : keepgoin, err); 
	return err;
}

uint16_t testYZfiltReg(int16_t iter)
{
	uint16_t err = 0;
	enum errWriteType rwerr = noProtocolErr;
	int i;

    if (!testIterControl(iter, "testYZfiltReg")) return 0;

	yamMsg("Testing YZ Filter Register", keepgoin, 0);
	for (;iter--;)
	{
		sprintf(tbuf, "iter: %d ", iter);
		yamMsg(tbuf, keepgoin, 0);

		for (i= 0; i < (int) Bit12; i++)
		{
			rwerr = WriteReg(Y_flt, i);
//			WriteReg(X_flt, 0x1234);
			temp = ReadReg(Y_flt, &rwerr);
			
			if (i != temp)
			{
				sprintf(tbuf, "i: %04X  Tr: %04X \r\n", i, temp) ;
				err++;
				yamMsg(tbuf, keepgoin, err);
			}
		}
	}

  //  printf("Test YZ Filter Register test done"); if (err) { printf ("Enter any key "); getchar(); printf("thanks \r\n"); }
//	yamMsg(tbuf, err ? yamwait : keepgoin);
	yamMsg("Test YZ Filter Register test done", err ? yamwait : keepgoin, err);
	return err;	
}


uint16_t testSMEreg(int16_t iter)
{
	uint16_t err = 0;
	enum errWriteType rwerr = noProtocolErr;
	uint16_t j = 1;
	int i;

    if (!testIterControl(iter, "testSMEreg")) return 0;
	
	for (;iter--;)
	{
		sprintf(tbuf, "iter: %d ", iter);
		yamMsg(tbuf, keepgoin, 0);

		for (i=0; i<16; i++)
		{
			WriteReg(Y_flt, i);
			WriteReg(X_flt, 0x1234);
			temp = ReadReg(SMen1, &rwerr);			
			sprintf(tbuf, "i: %04X  SME: %04X \r\n", i, temp) ;
			yamMsg(tbuf, keepgoin, 0);
			//	err++;
			j=1<<i;
			temp &= (~j);
			rwerr = WriteReg(SMen1, temp);		
		}

		for (i=0, j=0; i<16; i++)
		{
			WriteReg(Y_flt, i);
			WriteReg(X_flt, 0x1234);
			temp = ReadReg(SMen1, &rwerr);			
			sprintf(tbuf, "i: %04X  SME: %04X \r\n", i, temp) ;
			yamMsg(tbuf, keepgoin, 0);
			//	err++;
			j=1<<i;
			temp |= j;
			rwerr = WriteReg(SMen1, temp);		
		}
	}
	
	bankselect(0);
	yamMsg("SME Register test done", err ? yamwait : keepgoin, err);

	return err;	


}


int testWhoAmIcount=10;
int testTestRegcount=1;
int testTempSensorcount=1;
int testLrAccelcount = 1;
int testAccelNormcount =1;
int testAccelFSRcount =1;
int testpowerupResetRegcount = 0;
int testIDcount = 10;
int testpowerupSelfTestcount = 0;
int testnowdoSelfTestcount = 0;
int testSelfTestAndXfilterRegistercount = 0;
int testResetSelfTestcount = 0;
int testYZfiltRegcount = 1;
int testSMEregcount = 1;
int testAccelFilterbyNoisecount = 1;
int testGyroFilterbyNoisecount = 1;


// Test launcher
void TestMain()
{
	int err=0;
	initBSP(boardSPIRev);	
	IAM20685_chipunlock_PRM();
	IAM20685_writeunlock_PRM();
delay_ms(100);
 
 enableProtocolHistory(true);
   err +=  TestAccelFilterbyNoise(testAccelFilterbyNoisecount);
   err +=  TestGyroFilterbyNoise(testGyroFilterbyNoisecount);
  enableProtocolHistory(false);
	
	 
	 err +=	AccelFSRtest(testAccelFSRcount);
		 err += IDTest(testIDcount);
   err += testYZfiltReg(testYZfiltRegcount);

	err += TestTestReg(testTestRegcount);

err += testPowerupResetRegTest(testpowerupResetRegcount);
   err += testYZfiltReg(testYZfiltRegcount);
	err += TestTestReg(testTestRegcount);

	err += TestWhoAmI(FIXEDYAM, testWhoAmIcount);
	err += testSMEreg(testSMEregcount);
	err += IDTest(testIDcount);
	
	err += testSelfTestAndXfilterRegister(testSelfTestAndXfilterRegistercount);
	err += testpowerUpSelfTest(testpowerupSelfTestcount);
	err += testnowdoSelfTest(testnowdoSelfTestcount);
	err += testResetSelfTest(testResetSelfTestcount);
	err += AccelNormtest(testAccelNormcount);
	err += LrAccelTestbydiff(testLrAccelcount);
	err += TestTempSensor(testTempSensorcount);
	err += TestWhoAmI(FIXEDYAM, testWhoAmIcount);
	
	
	sprintf(tbuf, "Total Number of Errors:  %d", err);	
    yamMsg(tbuf, keepgoin, err);
}