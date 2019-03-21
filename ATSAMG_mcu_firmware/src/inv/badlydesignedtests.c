/*
 * CFile1.c
 *
 * Created: 1/30/2019 1:19:08 PM
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

#include <stdio.h>
#include <stdlib.h>

typedef enum  yerrType1 {xit, yamwait, keepgoin} yErrType;

static char tbuf[100];
int16_t regvalArr[20];
int16_t sgnvalArr[20];


int16_t LrAccelTestAverageLen = 1000;  // Num samples to average for noise estimation
float noiseTolerancelsb[] = {2.0, 4.0, 8.0, 16.0, 32.0};
int diffToleranceLSB = 10;
int32_t diff[3];

uint16_t LrAccelTest(int iter)
{
	uint16_t afsr, alrfsr, gfsr;
	uint16_t err=0;
	int i, j;
	float average[6];
	float absdiff;
	int fsrRatio, index;
	
	getFSRval(&afsr, &alrfsr, &gfsr);
	
	fsrRatio = alrfsr/afsr;
	index = lsbSet(fsrRatio) - 1;  //noise table index
	if (index > 4)  index = 4;
	for ( ; iter>=0; iter--) {
		printf("Low Res Accel Test Iter: %d\r\n", iter);
		int sum[6] = {0, 0, 0, 0, 0, 0};
		for (i=LrAccelTestAverageLen; i>=0; i--) {
			ReadAxesAndTemp(regvalArr, regvalArr+3, NULL, NULL, NULL);
			for(j=0; j<3; j++) {
				diff[j] = regvalArr[j] - fsrRatio*regvalArr[j+3];
				diff[j] = abs(diff[j]);
				if (diff[j] > diffToleranceLSB) {
					printf ("Accel outputs far off. Difference: %d \r\n", diff[j]);
					err++;
				}
			}
		}
	}
	return err;
}


#define NUMBITSNERGI 15

uint16_t LrAccelTestbyBinergi(int iter)
{
	uint16_t afsr, alrfsr, gfsr;
	uint16_t err=0;
	int i, j;
	float average[6];
	float absdiff;
	int fsrRatio, index;
	int hinrgi[3][NUMBITSNERGI], lonrgi[3][NUMBITSNERGI];
	int hidiff, lodiff;
	
	getFSRval(&afsr, &alrfsr, &gfsr);
	
	fsrRatio = alrfsr/afsr;
	index = lsbSet(fsrRatio) - 1;  //noise table index
	if (index > 4)  index = 4;
	
	for ( ; iter>=0; iter--) {
		printf("Low Res Accel Test Iter: %d\r\n", iter);
		for(int axis =0; axis < 3; axis++)
		for(j=0; j<NUMBITSNERGI; j++)
		hinrgi[axis][j] = lonrgi[axis][j] = 0;

		for (i=LrAccelTestAverageLen; i>=0; i--) {
			ReadAxesAndTemp(regvalArr, regvalArr+3, NULL, NULL, NULL);
			ReadAxesAndTemp(regvalArr+6, regvalArr+9, NULL, NULL, NULL);
			for(int axis=0; axis<3; axis++) {
				hidiff = regvalArr[6+axis] ^ regvalArr[0+axis];
				lodiff = regvalArr[9+axis] ^ regvalArr[3+axis];
				for(j=0; j<NUMBITSNERGI; j++) {
					hinrgi[axis][j] += hidiff>>j & 1;
					lonrgi[axis][j] += lodiff>>j & 1;
				}
			}  // axis loop
		} // av len loop
		printf("Bit Nergi levels: \r\n");
		for (int axis=0; axis<3; axis++) {
			printf("\r\n Hi: ");
			for (j=0; j < NUMBITSNERGI; j++)	{ printf(" %5d" , hinrgi[axis][j]); }
			printf("\r\n Lo: ");
			for (j=0; j < NUMBITSNERGI; j++)	{ printf(" %5d" , lonrgi[axis][j]); }
		}
		printf("\r\npress a key to continue\r\n"); getchar();
	} // iter loop
	return err;
}