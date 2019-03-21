/*
 * inv_i2c.c
 *
 * Created: 7/13/2017 10:19:41 AM
 *  Author: TDK InvenSense
 */ 

 #include <asf.h>
 #include "twi.h"
 #include "inv_i2c.h"
 #include "inv_uart.h"
 //#include "flexcom.h"
 //#include "conf_board.h"

 /* On Atmel platform, Slave Address should be set to 0x68 as pin SA0 is logic low */
 #define ICM_I2C_ADDR     0x68 // 0x69 /* I2C slave address for ICM20603 */
 #define DATA_ACCURACY_MASK  ((uint32_t)0x7)

 twi_options_t opt;
 twi_packet_t packet_tx, packet_rx;

 void i2c_master_initialize(void)
 {
	 /* Insert application code here, after the board has been initialized. */
	 /* Enable the peripheral and set TWI mode. */
	 flexcom_enable(BOARD_FLEXCOM_TWI);
	 flexcom_set_opmode(BOARD_FLEXCOM_TWI, FLEXCOM_TWI);
	 
	 /* Configure the options of TWI driver */
	 opt.master_clk = sysclk_get_peripheral_hz();
	 opt.speed = TWI_CLK;

	 //const char str1[] = "-E-\tTWI master initialization failed.\r\n";

	 //twi_master_init(BOARD_BASE_TWI, &opt);
	 if (twi_master_init(BOARD_BASE_TWI, &opt) != TWI_SUCCESS) {
		 //usart_serial_write_packet(CONF_UART, (const uint8_t*)str1, sizeof(str1) - 1);
	}
 }

 void i2c_master_deinit(void)
 {
	 /* Disable the peripheral and TWI mode. */
	 flexcom_disable(BOARD_FLEXCOM_TWI);

	 twi_disable_master_mode(BOARD_BASE_TWI);

 }
 unsigned long i2c_master_read_register(unsigned char Address, unsigned char RegisterAddr, unsigned short RegisterLen, unsigned char *RegisterValue)
 {
	 twi_packet_t packet_read;

	 if(Address == NULL){
		 Address = ICM_I2C_ADDR;	/* On Atmel platform, Slave Address should be set to 0x68 as pin SA0 is logic low */
	 }
	 
	 packet_read.chip = Address;
	 packet_read.addr[0] = RegisterAddr;
	 packet_read.addr_length = 1;
	 packet_read.buffer = RegisterValue;
	 packet_read.length = RegisterLen;
	 
	 if(twi_master_read((Twi*)BOARD_BASE_TWI, &packet_read) == TWI_SUCCESS){
		 return TWI_SUCCESS;
	 }
	 return TWI_BUSY;
 }

 unsigned long i2c_master_write_register(unsigned char Address, unsigned char RegisterAddr, unsigned short RegisterLen, const unsigned char *RegisterValue)
 {
	 twi_packet_t packet_write;

	 if(Address == NULL){
		 Address = ICM_I2C_ADDR; /* On Atmel platform, Slave Address should be set to 0x68 as pin SA0 is logic low */
	 }
	 packet_write.chip = Address;
	 packet_write.addr[0] = RegisterAddr;
	 packet_write.addr_length = 1;
	 packet_write.buffer = RegisterValue;
	 packet_write.length = RegisterLen;

	 return twi_master_write((Twi*)BOARD_BASE_TWI, &packet_write);
 }
