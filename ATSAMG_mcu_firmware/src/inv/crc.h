/*
 * IncFile1.h
 *
 * Created: 1/21/2019 1:39:09 PM
 *  Author: pmadhvapathy
 */ 


#ifndef CRC_H_
#define CRC_H_

unsigned char IAM20685_crc_calculatefast(unsigned long data);
unsigned char IAM20685_crc_calculate1(unsigned long data);
unsigned char spi3_crc_calculate(unsigned long data);


#endif /* CRC_H_ */