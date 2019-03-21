/*
 * inv_i2c.h
 *
 * Created: 7/13/2017 10:19:52 AM
 *  Author: TDK InvenSense
 */ 


#ifndef INV_I2C_H_
#define INV_I2C_H_

void i2c_master_initialize(void);
void i2c_master_deinit(void);
unsigned long i2c_master_read_register(unsigned char Address, unsigned char RegisterAddr, unsigned short RegisterLen, unsigned char *RegisterValue);
unsigned long i2c_master_write_register(unsigned char Address, unsigned char RegisterAddr, unsigned short RegisterLen, const unsigned char *RegisterValue);



#endif /* INV_I2C_H_ */