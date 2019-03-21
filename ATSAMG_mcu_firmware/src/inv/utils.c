/*
 * CFile1.c
 *
 * Created: 1/25/2019 1:53:30 PM
 *  Author: pmadhvapathy
 */ 
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "yamAPI.h"
#include "IAM20685Reg.h"

const HdrFreq = 100;

#define ATBUFSZ 100
static char atbuf[ATBUFSZ];
#define GTBUFSZ 100
static char gtbuf[GTBUFSZ];

static printUnitsType printUnits = rawbits;

static float flAz, flAy, flAx;
static float flGz ,flGy, flGx;
static float flT1, flT2, fTdiff;
static float frAz, frAy, frAx;
static float fdAz, fdAy, fdAx;

const char afres[]  = "%7.3f ";
const char gfres[]  = "%7.3f ";
const char  tfmt[]  = "%4.1f ";
const char amres[]  = "%5.1f ";
const char gmres[]  = "%5.1f ";

void setPrintUnits(printUnitsType printFmt)
{
	printUnits = printFmt;	
	if (printUnits == milliUnits) { 
		 strlcpy(atbuf, amres, ATBUFSZ); 
		 strlcat(atbuf, amres, ATBUFSZ); 
		 strlcat(atbuf, amres, ATBUFSZ); 
		 
		 strlcpy(gtbuf, gmres, GTBUFSZ);
		 strlcat(gtbuf, gmres, GTBUFSZ);
		 strlcat(gtbuf, gmres, GTBUFSZ); 
    }
	else { 
		strlcpy(atbuf, afres, ATBUFSZ); 
		strlcat(atbuf, afres, ATBUFSZ); 
		strlcat(atbuf, afres, ATBUFSZ); 
			 
		strlcpy(gtbuf, gfres, GTBUFSZ);
		strlcat(gtbuf, gfres, GTBUFSZ);
		strlcat(gtbuf, gfres, GTBUFSZ);	
	}

}

const char fESC[] =  "\033";
const char fBLK[] = "\033[30 ";
const char fRED[] = "\033[31 "; 
const char fGRN[] = "\033[32 ";
const char fYLW[] = "\033[33 ";
const char fBLU[] = "\033[34 ";
const char fMGN[] = "\033[35 ";
const char fCYN[] = "\033[96 ";
const char fWHT[] = "\033[7 ";
const char fDFL[] = "\033[39 ";

const char bESC[] =  "\033";
const char bBLK[] = "\033[30 ";
const char bRED[] = "\033[31 "; 
const char bGRN[] = "\033[32 ";
const char bYLW[] = "\033[33 ";
const char bBLU[] = "\033[34 ";
const char bMGN[] = "\033[35 ";
const char bCYN[] = "\033[36 ";
const char bWHT[] = "\033[37 ";
const char bDFL[] = "\033[39 ";

#define SFT %8.3f
#define TFT %2.1f

	
static int printAxesCount=0;
void resetPrintHdrCount() {	printAxesCount=0;}
void printAxes (int16_t *accelp, int16_t *accelLowresp, int16_t *gyrop, int16_t *temperaturep, int16_t *anythingelsep)
{
	if(HdrFreq && printAxesCount%HdrFreq == 0) {
		if (accelp)					printComponentHeader(Af);
		if (accelLowresp)			printComponentHeader(Al);
		if (accelp && accelLowresp) printComponentHeader(dA);
		if (gyrop)					printComponentHeader(Gp);
		if (temperaturep)			printComponentHeader(Tp);
		if(anythingelsep)			printComponentHeader(Op);
		
		printf("\r\n");
	}
	
	printAxesCount++;
	if (printUnits == rawbits)
	{
		if (accelp) 
		   printf ("%7d %7d %7d ", *(accelp+2), *(accelp+1), *(accelp));
		if (accelLowresp) 
		   printf ("%7d %7d %7d ", *(accelLowresp+2), *(accelLowresp+1), *(accelLowresp));
		if (gyrop) 
		   printf ("%7d %7d %7d ", *(gyrop+2), *(gyrop+1), *(gyrop));	   
		if (temperaturep) 
		   printf ("%7d %7d %7d ", *(temperaturep), *(temperaturep+1), *(temperaturep+2));
		if (anythingelsep) 
		   printf ("%04x", (uint16_t)(*anythingelsep));
	}
	else
	{
		if (accelp) {
			flAz = (float)accel2gee(*(accelp+2), false); 
			flAy = (float)accel2gee(*(accelp+1), false);
			flAx = (float)accel2gee(*(accelp+0), false);

			printf(atbuf, flAz, flAy, flAx);
		}
		if (accelLowresp) {
			frAz  = (float)accel2gee(*(accelLowresp+2), true);
			frAy  = (float)accel2gee(*(accelLowresp+1), true);
			frAx  = (float)accel2gee(*(accelLowresp+0), true);

			printf(atbuf, frAz, frAy, frAx);
		}
		if (accelp && accelLowresp) {
			fdAz = floatAbs(flAz - frAz);
			fdAy = floatAbs(flAy - frAy);
			fdAx = floatAbs(flAx - frAx);

			printf(atbuf, fdAz, fdAy, fdAx);
		}
		if (gyrop) {
			flGz = (float)gyro2dps(*(gyrop+2));
			flGy = (float)gyro2dps(*(gyrop+2));
			flGx = (float)gyro2dps(*(gyrop+2));			
			
			printf(gtbuf, flGz, flGy, flGx);
		}
		if (temperaturep) {
			flT1 = (float)temp2Centigrade(*(temperaturep));
			flT2 = (float)temp2Centigrade(*(temperaturep+1));
			fTdiff = (float)temp2Centigrade(*(temperaturep+2)) - 25.0;			
		    
			printf("%3.1f  %3.1f  %3.1f", flT1, flT2, fTdiff);
		}
		if(anythingelsep) {
				printf("    %04X ", (uint16_t)(*anythingelsep));
		}
	}
	printf("\r\n");
}

void printAccel(int16_t * arr)
{
			printf
			(   " %8d  %8d  %8d %8d  %8d  %8d  %4d %4d %04d  %04x\r\n"
			,arr[2], arr[1], arr[0]     // accel
			,arr[5], arr[4], arr[3]     // accel low res
			,arr[9], arr[10], arr[11]   // temp and delta
			,(uint16_t)  arr[FXDARRINDEX]
			);
}

void printHdr(int i)
{
	if ((i%HdrFreq==0) )
	{
		if (printUnits == rawbits) {
			printf("%s%s%s%s%s%s%s%s%s\r\n", "", "      Az", "      Ay" ,"      Ax", "      Gz", "      Gy", "      Gx", "   T1 ", "  T2 ");
			printf("%s%s%s%s%s%s%s%s%s\r\n", "  ", " -----  ", " -----  " ," -----  ", " -----  ", " -----  ", " -----  ", " --  ", " --  ");
		}
		else {
			printf("%s%s%s%s%s%s%s%s%s\r\n", "    ", "Az       ", "Ay       " ,"Ax       ", "Gz       ", "Gy       ", "Gx     ", "T1    ", "T2 ");
			printf("%s%s%s%s%s%s%s%s%s\r\n", "", " ------  ", " ------- " ," ------- ", " ------- ", " ------- ", " ------- ", " ---    ", "--- ");
		}
		printf("\r\n");
	}
}

static	char nosp[] = "";
static	char left[] = "  ";
static	char mid4[] = "    ";
static	char mid2[] = "  ";
static	char mid1[] = " ";

void printComponentHeader(printCompType comp)
{
	if (!HdrFreq)
	 return;  // dont waste time
	if (printAxesCount%HdrFreq==0)
	{
		//printf("%s", fGRN);
		switch (comp)
		{
			case Af:  
			   printf("%s %s %s %s %s %s %s", left, "Ax", mid4, "Ay", mid4, "Az", mid4);
			break;
			case Al:
			   printf("%s %s %s %s %s %s %s", nosp, "Axlr", mid2, "Aylr", mid2, "Azlr", mid1);
			break;
			case Gp:
			   printf("%s %s %s %s %s %s %s", left, "Gx", mid4, "Gy", mid4, "Gz", mid2);
			break;
			case Tp:
			   printf("%s %s %s %s %s %s %s", nosp, "T1", mid2, "T2", mid2, "dT", mid2);
			break;
			case dA:
			   printf("%s %s %s %s %s %s %s", nosp, "AxDif", mid1, "AyDif", mid1, "AzDif", nosp);
			break;
			
			default:
			   printf("%s%s",mid2, "Other");
			break;		
		}
		//printf("%s", fWHT);

	}
}


//* floating absolute val function . Not available from softfp lib
float floatAbs ( float f)
{
	if (f < 0.0f)  
	  return (-f);
	else 
	 return f;
}


uint16_t lsbSet(uint16_t v)
{
	int i;
	uint16_t retval;
	
	for (i=0; i<16; i++)
	{
		if ((v>>i)&1)
		break;
	}
	if (i==16)
	{  printf("bitfield is empty\r\n");  printf("Press any key to continue \r\n"); getchar(); }
	
	retval	= i;
	return i;
}