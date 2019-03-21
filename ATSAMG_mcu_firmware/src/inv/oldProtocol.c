/*
 * CFile1.c
 *
 * Created: 1/22/2019 2:25:01 PM
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
#include <stdio.h>
#include <stdlib.h>
#include "IAM20685Reg.h"
#include "yamtests.h"
#include "bsp.h"
#include "crc.h"
#include "lockUnlock.h"
#include "utils.h"
#include "yamAPI.h"

extern const AccelFsrtype  accelFSRdefault;
extern const GyroFSRtype   gyroFSRdefault ;


#if 0
#define a0 0x00
#define v0 0x00
#define v1 0xF9

#define a1 0x00
#define g0 0x00
#define g1 0xA3
#else
#define a0 0x37
#define v0 0x41
#define v1 0xA1

#define a1 0x79
#define g0 0x00
#define g1 0x0F
#endif



void IAM20685Original() //command=5, Crystal-2
{
	uint8_t value[20];
	//uint8_t result = 0;
	int gx, gy, gz, ax, ay, az, t1, t2 ;
	
	ioport_set_pin_dir(CRYSTAL_RST, IOPORT_DIR_OUTPUT); //
	ioport_set_pin_level(CRYSTAL_RST, IOPORT_PIN_LEVEL_HIGH); //
	
	ext_int_initialize(ext_interrupt_handler);
	
	spi_master_init1(); //for ver-B and D+ board EVB
	//spi_master_init(); //for ver-A board EVB and on board sensor
	
	IAM20685_chipunlock_PRM();	
	
	value[0] = 0x00; //	CMD:2C0000F3
	value[1] = 0x00;
	value[2] = 0xF3;
	spi_master_write_register(0x2C, 3, value);
	delay_us(10);
	spi_master_read_register(0x2c, 2, value);
	delay_us(10);			
	
	uint16_t whoami = (short)((value[0] << 8 | value[1]) & 0x0000FFFF); //!PRM
	if (whoami != 0xAA55)
	{
		printf("IAM20685 WHOAMI = %04x\r\n", (value[0] << 8 | value[1]));
        getchar();
	}
	
	//while(1){};
	
	
	IAM20685_writeunlock_PRM();	
	
	spi_master_read_register(0xB0, 0, value);//CMD:B0 00 00 F9, G and A YZ filter 60hz/60hz
	value[0] = v0;
	value[1] = v1;
	spi_master_write_register(a0, 2, value);
	delay_us(10);
	
	spi_master_read_register(0xB8, 0, value);//CMD:B8 00 00 A3, G and A X filter 60hz/60hz
	value[0] = g0;
	value[1] = g1;
	spi_master_write_register(a1, 2, value);
	delay_us(10);
		
	//set accel FSR=2.048g
	spi_master_read_register(0xFC, 0, value);//CMD:FC 00 06 62 change to bank6
	value[0] = 0x06;
	value[1] = 0x62;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	
	#if 1
	spi_master_read_register(0xD0, 0, value);//CMD:D0 00 04 62
	value[0] = 0x04;
	value[1] = 0x62;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);	
	#else
	//accel FSR=4g
		spi_master_read_register(0xD0, 0, &value);//CMD:D0 00 07 61
		value[0] = 0x07;
		value[1] = 0x61;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
	#endif
	//set gyro FSR=1966dps
	spi_master_read_register(0xFC, 0, value);//CMD:FC 00 07 63 change to bank7
	value[0] = 0x07;
	value[1] = 0x63;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	
	spi_master_read_register(0xD0, 0, value);//CMD:D0 00 02 64
	value[0] = 0x02;
	value[1] = 0x64;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);	
	
	spi_master_read_register(0xFC, 0, value);//CMD:FC 00 00 64 change back to bank0
	value[0] = 0x00;
	value[1] = 0x64;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);	

	while(1)
	{		
	delay_us(10);
	if (1){
	//if (int_trigger){

	value[0] = 0x00; //	CMD:000000F1
	value[1] = 0x00;
	value[2] = 0xF1;
	spi_master_write_register(0x00, 3, value);
	delay_us(5);
//	spi_master_read_register(0x00, 2, value);
	spi_master_read_register(0x00, 3, value); //!PRM
	gx = (short)(value[0] << 8 | value[1]);
	delay_us(5);	
	
	value[0] = 0x00; //	CMD:040000DC
	value[1] = 0x00;
	value[2] = 0xDC;
	spi_master_write_register(0x04, 3, value);
	delay_us(5);
	spi_master_read_register(0x04, 3, value);
	gy = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	value[0] = 0x00; //	CMD:080000AB
	value[1] = 0x00;
	value[2] = 0xAB;
	spi_master_write_register(0x08, 3, value);
	delay_us(5);
	spi_master_read_register(0x08, 3, value);
	gz = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	value[0] = 0x00; //	CMD:0C000086
	value[1] = 0x00;
	value[2] = 0x86;
	spi_master_write_register(0x0C, 3, value);
	delay_us(5);
	spi_master_read_register(0x0C, 3, value);
	t1 = (short)(value[0] << 8 | value[1]);
	delay_us(5);

	value[0] = 0x00; //	CMD:10000045
	value[1] = 0x00;
	value[2] = 0x45;
	spi_master_write_register(0x10, 3, value);	
	delay_us(5);	
	spi_master_read_register(0x10, 3, value);
	ax = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	value[0] = 0x00; //	CMD:14000068
	value[1] = 0x00;
	value[2] = 0x68;
	spi_master_write_register(0x14, 3, value);	
	delay_us(5);	
	spi_master_read_register(0x14, 3, value);
	ay = (short)(value[0] << 8 | value[1]);
	delay_us(5);
		
	value[0] = 0x00; //	CMD:1800001F
	value[1] = 0x00;
	value[2] = 0x1F;
	spi_master_write_register(0x18, 3, value);	
	delay_us(5);	
	spi_master_read_register(0x18, 3, value);
	az = (short)(value[0] << 8 | value[1]);	
	delay_us(5);	
		
	value[0] = 0x00; //	CMD:1C000032
	value[1] = 0x00;
	value[2] = 0x32;
	spi_master_write_register(0x1C, 3, value);
	delay_us(5);
	spi_master_read_register(0x1C, 3, value);
	t2 = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	printf("%d,%d,  %d,%d,%d,  %d,%d,%d\r\n", t1, t2, ax, ay, az, gx, gy, gz);
	//int_trigger = 0;		
		}
	}
}




void IAM20685() //command=5, Crystal-2
{
	uint8_t value[20];
	//uint8_t result = 0;
	//int gx, gy, gz, ax, ay, az, t1, t2;
	int16_t readArr[20] , whoami;
	
	printf("Original\r\n");
	ioport_set_pin_dir(CRYSTAL_RST, IOPORT_DIR_OUTPUT); //
	ioport_set_pin_level(CRYSTAL_RST, IOPORT_PIN_LEVEL_HIGH); //
	
	ext_int_initialize(ext_interrupt_handler);
	
	spi_master_init1(); //for ver-B and D+ board EVB
	//spi_master_init(); //for ver-A board EVB and on board sensor
	
	//spiSpy(true);
	IAM20685_chipunlock_PRM();
	//spiSpy(false);
	//printf("Unlock done: \r\n");
	//getchar();
	
	value[0] = 0x00; //	CMD:2C0000F3
	value[1] = 0x00;
	value[2] = 0xF3;
	spi_master_write_register(0x2C, 3, value);
	delay_us(10);
	spi_master_read_register(0x2c, 2, value);
	delay_us(10);
	printf("IAM20685 WHOAMI = %04x\r\n", (value[0] << 8 | value[1]));
	
	//spiSpy(true);
	#if 0
	printf("current write unlock ");
	IAM20685_writeunlock();
	#else
	printf("API write unlock ");
	IAM20685_writeunlock_PRM();
	#endif
	//spiSpy(false);
	
	spi_master_read_register(0xB0, 0, value);//CMD:B0 00 00 F9, G and A YZ filter 60hz/60hz
	value[0] = 0x00;
	value[1] = 0xF9;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	

	spi_master_read_register(0xB8, 0, value);//CMD:B8 00 00 A3, G and A X filter 60hz/60hz
	value[0] = 0x00;
	value[1] = 0xA3;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	
	//set accel FSR=2.048g

	spi_master_read_register(0xFC, 0, value);//CMD:FC 00 06 62 change to bank6
	value[0] = 0x06;
	value[1] = 0x62;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	
	#if 1
	spi_master_read_register(0xD0, 0, value);//CMD:D0 00 04 62
	value[0] = 0x04;
	value[1] = 0x62;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	#else
	//accel FSR=4g
	spi_master_read_register(0xD0, 0, value);//CMD:D0 00 07 61
	value[0] = 0x07;
	value[1] = 0x61;
	//		value[0] = 0x00;
	//		value[1] = 0x00;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	#endif


	//set gyro FSR=1966dps
	spi_master_read_register(0xFC, 0, value);//CMD:FC 00 07 63 change to bank7
	value[0] = 0x07;
	value[1] = 0x63;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	
	spi_master_read_register(0xD0, 0, value);//CMD:D0 00 02 64
	value[0] = 0x02;
	value[1] = 0x64;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	
	spi_master_read_register(0xFC, 0, value);//CMD:FC 00 00 64 change back to bank0
	value[0] = 0x00;
	value[1] = 0x64;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);

regAddrType reg;

	printf("$$$ \r\n");
	for(int i = 0; !userexit(); i++)  // forever
	{
		//    if (i==0) spiSpy(true);
		//   else if (i==1) { spiSpy(false); getchar(); }
		printHdr(i);
		for (reg=Gx; reg<=T2; reg++)
		{
			readArr[reg] = readReg(0, reg);
			whoami = readReg(0, Fxd);
		}
		printf("%7d %7d %7d %7d %7d %7d %4d %4d %04x\r\n",	 readArr[Az], readArr[Ay], readArr[Ax]
		,readArr[Gz], readArr[Gy], readArr[Gx]
		,readArr[T1], readArr[T2], readArr[FXDARRINDEX]
		);
	}
			
}




