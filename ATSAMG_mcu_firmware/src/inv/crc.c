/*
 * CFile1.c
 *
 * Created: 1/21/2019 1:38:14 PM
 *  Author: pmadhvapathy
 */ 
#include <asf.h>
#include "crc.h"

unsigned char IAM20685_crc_calculatefast(unsigned long data)
{
	uint32_t crc;
	uint32_t stepzor = 0x000E8000;
	uint32_t msb = 0x00800000;

	data ^= 0x00C40000;  //steps 24 to 16
	if((data & (msb)) !=0)
	data ^= stepzor;   // step 15
	#if 0
	printcrc(data, 15);
	#endif
	if ((data & (msb>>1)) != 0)
	data ^= (stepzor >> 1);  //step 14
	#if 0
	printcrc(data, 14);
	#endif

	if ((data & (msb>>2)) != 0)
	data ^= (stepzor >> 2);  //step 13
	#if 0
	printcrc(data, 13);
	#endif

	if ((data & (msb >> 3)) != 0)
	data ^= (stepzor >> 3);  //step 12
	#if 0
	printcrc(data, 12);
	#endif

	if ((data & (msb >> 4)) != 0)
	data ^= (stepzor >> 4);  //step 11
	#if 0
	printcrc(data, 11);
	#endif
	if ((data & (msb >> 5)) != 0)
	data ^= (stepzor >> 5);  //step 10
	#if 0
	printcrc(data, 10);
	#endif
	if ((data & (msb >> 6)) != 0)
	data ^= (stepzor >> 6);  //step 9
	#if 0
	printcrc(data, 9);
	#endif

	if ((data & (msb >> 7)) != 0)
	data ^= (stepzor >> 7);  //step 8
	#if 0
	printcrc(data, 8);
	#endif
	if ((data & (msb >> 8)) != 0)
	data ^= (stepzor >> 8);  //step 7
	#if 0
	printcrc(data, 7);
	#endif

	if ((data & (msb >> 9)) != 0)
	data ^= (stepzor >> 9);  //step 6
	#if 0
	printcrc(data, 6);
	#endif

	if ((data & (msb >> 10)) != 0)
	data ^= (stepzor >> 10);  //step 5
	#if 0
	printcrc(data, 5);
	#endif

	if ((data & (msb >> 11)) != 0)
	data ^= (stepzor >> 11);  //step 4
	#if 0
	printcrc(data, 4);
	#endif

	if ((data & (msb >> 12)) != 0)
	data ^= (stepzor >> 12);  //step 3
	#if 0
	printcrc(data, 3);
	#endif

	if ((data & (msb >> 13)) != 0)
	data ^= (stepzor >> 13);  //step 2
	#if 0
	printcrc(data, 2);
	#endif

	if ((data & (msb >> 14)) != 0)
	data ^= (stepzor >> 14);  //step 1
	#if 0
	printcrc(data, 1);
	#endif

	if ((data & (msb >> 15)) != 0)
	data ^= (stepzor >> 15);  //step 0
	#if 0
	printcrc(data, 0);
	#endif



	crc = (data ^ 0x000000FF);
	return (Byte)crc;
}


unsigned char spi3_crc_calculate(unsigned long data)
{
	unsigned char crc_old = 0xFF;
	unsigned char crc_new = 0x00;
	unsigned char temp_calc1;
	unsigned char temp_calc2;
	unsigned long temp_calc3;
	signed char i;

	crc_old = 0xFF;
	crc_new = 0x00;
	for (i = 23; i >= 0; i--)
	{
		temp_calc1 = crc_old & 0x80;
		temp_calc3 = data >> i;
		temp_calc2 = (unsigned char)temp_calc3;
		temp_calc2 &= 0x01;
		if (temp_calc1)
		{
			temp_calc1 = 0x0E;
			temp_calc2 ^= 0x01;
		}
		else temp_calc1 = 0;

		crc_old ^= temp_calc1;
		crc_new = crc_old << 1;
		crc_new += (unsigned long)temp_calc2;
		crc_old = crc_new;
	}

	//printf("CRCCalculate: 0x%x\n", crc_old);
	return (crc_old ^ 0xff);
}

unsigned char IAM20685_crc_calculate1(unsigned long data)
{
	unsigned char crc_old = 0xFF;
	unsigned char crc_new = 0x00;
	unsigned char temp_calc1;
	unsigned char temp_calc2;
	unsigned long temp_calc3;
	signed char i;

	crc_old = 0xFF;
	crc_new = 0x00;
	for (i = 23; i >= 0; i--)
	{
		temp_calc1 = crc_old & 0x80;
		temp_calc3 = data >> i;
		temp_calc2 = (unsigned char)temp_calc3;
		temp_calc2 &= 0x01;
		if (temp_calc1)
		{
			temp_calc1 = 0x0E;
			temp_calc2 ^= 0x01;
		}
		else temp_calc1 = 0;

		crc_old ^= temp_calc1;
		crc_new = crc_old << 1;
		crc_new += (unsigned long)temp_calc2;
		crc_old = crc_new;
	}

	//printf("CRCCalculate: 0x%x\n", crc_old);
	return (crc_old ^ 0xff);
}
