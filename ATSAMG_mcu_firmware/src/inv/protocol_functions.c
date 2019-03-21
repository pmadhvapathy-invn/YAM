/*
 * protocol_functions.c
 *
 * Created: 7/14/2017 10:09:38 AM
 *  Author: TDK InvenSense
 */ 

#include <asf.h>
#include <stdio.h>
#include <stdlib.h>
#include "protocol_functions.h"
#include "inv_uart.h"
#include "inv_i2c.h"
#include "inv_spi.h"
#include "arm_version.h"
#include "math.h"
#include "delay.h"
#include "IAM20685Reg.h"
#include "yamtests.h"
#include "bsp.h"
#include "crc.h"
#include "lockUnlock.h"
#include "yamAPI.h"
#include "utils.h"


const AccelFsrtype  accelFSRdefault = afsr16;
const GyroFSRtype   gyroFSRdefault  = gfsr1966;


// poll keyboard to check for ctrl-c  : dont know how to do this yet
void boardAndChipInit( bool boardRev)
{

			initBSP(boardRev);
			
			IAM20685_chipunlock_PRM();

}


void IAM20685_PRM()//command=9
{
		//uint8_t result = 0;
		int16_t readArr[20];
		uint16_t  whoami;
		int fsrindx;

		//uint16_t temp, temp1, temp2;


       boardAndChipInit(boardSPIRev);
		
		WhoAmIType okNok =	getWhoAmI(&whoami);
		if (okNok == YamNotOK) { printf("Yam identity crisis: %04X  :(   press any key to continue: ",  whoami);  getchar(); printf("..thanks \r\n");  }
		
		IAM20685_writeunlock_PRM();
		
		SetFilter(Z, g60hz, a60hz);
		SetFilter(Y, g60hz, a60hz);
		SetFilter(X, g60hz, a60hz);

		AccelFSRset(accelFSRdefault, flr4);
		GyroFSRset(gyroFSRdefault);

		bankselect(bFxd);

		setPrintUnits(rawbits);
		resetPrintHdrCount();
		printf("\r\n");
		fsrindx=0;
		for (int i=0;  !userexit();i++)
		{
			//printHdr(i);
			if (i%100 == 99)
			{
				AccelFsrtype  setfsr;
				AccellRestype setflr;

				fsrindx = (fsrindx+1)%8;
				setfsr = accelAllowedFSRcombo[fsrindx].afsr;
				setflr = accelAllowedFSRcombo[fsrindx].alrfsr;
			
				AccelFSRset (setfsr, setflr);
			}
			ReadAxesAndTemp(readArr, readArr+3, readArr+6, readArr+9, readArr+FXDARRINDEX);
			printAxes(readArr, readArr+3, readArr+6, readArr+9, readArr+FXDARRINDEX);
		}
		
		printf("User Ctrl-X, exited test \r\n");
}

