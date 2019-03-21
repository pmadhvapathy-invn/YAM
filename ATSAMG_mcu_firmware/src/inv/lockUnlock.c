/*
 * CFile1.c
 *
 * Created: 1/22/2019 2:25:54 PM
 *  Author: pmadhvapathy
 */ 

#include <asf.h>
#include "protocol_functions.h"
#include "inv_uart.h"
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


void IAM20685_chipunlock() //Crystal-2 chip unlock, if locked
{
	uint8_t value[20];
	
	//unlock chip and change to SPI3
	value[0] = 0x19; //CMD:00193247 //CMD:0002334C //	CMD:0001334F //	CMD:0004334A //	CMD:0300334D //	CMD:01803347 //	CMD:02803344 //	CMD:00923244 //	CMD:0001334F
	value[1] = 0x32;
	value[2] = 0x47;
	//uint8_t c = IAM20685_crc_calculate(0x000233);
	spi_master_write_register(0x00, 3, value);
	
	delay_us(10);
	value[0] = 0x02; //CMD:0002334C //	CMD:0001334F //	CMD:0004334A //	CMD:0300334D //	CMD:01803347 //	CMD:02803344 //	CMD:00923244 //	CMD:0001334F
	value[1] = 0x33;
	value[2] = 0x4C;
	spi_master_write_register(0x00, 3, value);
	delay_us(10);
	value[0] = 0x01; //	CMD:0001334F //	CMD:0004334A //	CMD:0300334D //	CMD:01803347 //	CMD:02803344 //	CMD:00923244 //	CMD:0001334F
	value[1] = 0x33;
	value[2] = 0x4F;
	spi_master_write_register(0x00, 3, value);
	delay_us(10);
	value[0] = 0x04; //	CMD:0004334A //	CMD:0300334D //	CMD:01803347 //	CMD:02803344 //	CMD:00923244 //	CMD:0001334F
	value[1] = 0x33;
	value[2] = 0x4A;
	spi_master_write_register(0x00, 3, value);
	delay_us(10);
	value[0] = 0x00; //	CMD:0300334D //	CMD:01803347 //	CMD:02803344 //	CMD:00923244 //	CMD:0001334F
	value[1] = 0x33;
	value[2] = 0x4D;
	spi_master_write_register(0x03, 3, value);
	delay_us(10);
	value[0] = 0x80; //	CMD:01803347 //	CMD:02803344 //	CMD:00923244 //	CMD:0001334F
	value[1] = 0x33;
	value[2] = 0x47;
	spi_master_write_register(0x01, 3, value);
	delay_us(10);
	value[0] = 0x80; //	CMD:02803344 //	CMD:00923244 //	CMD:0001334F
	value[1] = 0x33;
	value[2] = 0x44;
	spi_master_write_register(0x02, 3, value);
	delay_us(10);
	value[0] = 0x92; //	CMD:00923244 //	CMD:0001334F
	value[1] = 0x32;
	value[2] = 0x44;
	spi_master_write_register(0x00, 3, value);
	delay_us(10);
	value[0] = 0x01; //	CMD:0001334F
	value[1] = 0x33;
	value[2] = 0x4F;
	spi_master_write_register(0x00, 3, value);
	delay_us(10);
}

void IAM20685_chipunlock_PRM()
{
	LKUNLKWrite(0x00, 0x193247);  //CMD:00193247
	LKUNLKWrite(0x00, 0x02334C);  //CMD:0002334C
	LKUNLKWrite(0x00, 0x01334F);  //CMD:0001334F
	LKUNLKWrite(0x00, 0x04334A);  //CMD:0004334A
	LKUNLKWrite(0x03, 0x00334D);  //CMD:0300334D
	LKUNLKWrite(0x01, 0x803347);  //CMD:01803347
	LKUNLKWrite(0x02, 0x803344);  //CMD:02803344
	LKUNLKWrite(0x00, 0x923244);  //CMD:00923244
	LKUNLKWrite(0x00, 0x01334F);  //CMD:0001334F
	delay_us(15);
}




void IAM20685_writeunlock() //Crystal-2 chip unlock, if locked
{
	uint8_t value[20];

	//unlock write chip
	spi_master_read_register(0xE4, 0, value);//CMD:E4000288
	value[0] = 0x02;
	value[1] = 0x88;
	//delay_us(1);
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, value);//CMD:E400018B
	value[0] = 0x01;
	value[1] = 0x8B;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, value);//CMD:E400048E
	value[0] = 0x04;
	value[1] = 0x8E;
	spi_master_write_register(0x00, 2, value);
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, value);//CMD:E40300AD
	value[0] = 0x00;
	value[1] = 0xAD;
	spi_master_write_register(0x03, 2, value);
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, value);//CMD:E4018017
	value[0] = 0x80;
	value[1] = 0x17;
	spi_master_write_register(0x01, 2, value);
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, value);//CMD:E4028030
	value[0] = 0x80;
	value[1] = 0x30;
	spi_master_write_register(0x02, 2, value);
	delay_us(10);
}

void IAM20685_writeunlock_PRM() //Crystal-2 chip unlock, if locked
{
	regAddrType r = 0x19;
	indirectWrite(r, 0x0002);
	indirectWrite(r, 0x0001);
	indirectWrite(r, 0x0004);
	
	indirectWrite(r, 0x0300);
	indirectWrite(r, 0x0180);
	indirectWrite(r, 0x0280);
	delay_us(15);
		
}