/*
 * protocol_functions.c
 *
 * Created: 7/14/2017 10:09:38 AM
 *  Author: TDK InvenSense
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


extern SemaphoreHandle_t sema_main_rx_uart;
extern uint8_t main_rx_buf[MAXDATABUF];

extern SemaphoreHandle_t sema_main_tx_uart;
extern uint8_t main_tx_buf[MAXDATABUF];

extern SemaphoreHandle_t sema_sensor_int_pb03;
//SensorDesc_t sensor_desc;


void ext_interrupt_handler(void)
{
    xSemaphoreGive(sema_sensor_int_pb03);
}

void ext_int_initialize(void (*handler_function)(void))
{
	/* Enable the peripheral clock for the MAG extension board interrupt pin. */
	pmc_enable_periph_clk(PIN_EXT_INTERRUPT_ID);

	/* Configure PIOs as input pins. */
	pio_configure(PIN_EXT_INTERRUPT_PIO, PIN_EXT_INTERRUPT_TYPE, PIN_EXT_INTERRUPT_MASK, PIN_EXT_INTERRUPT_ATTR);

	/* Initialize PIO interrupt handler, interrupt on rising edge. */
	pio_handler_set(PIN_EXT_INTERRUPT_PIO, PIN_EXT_INTERRUPT_ID, PIN_EXT_INTERRUPT_MASK,
	PIN_EXT_INTERRUPT_ATTR, (void (*) (uint32_t, uint32_t))handler_function);

	/* Initialize and enable push button (PIO) interrupt. */
	pio_handler_set_priority(PIN_EXT_INTERRUPT_PIO, PIN_EXT_INTERRUPT_IRQn, 0);
	pio_enable_interrupt(PIN_EXT_INTERRUPT_PIO, PIN_EXT_INTERRUPT_MASK);

}

void IAM20685_chipunlock() //Crystal-2 chip unlock, if locked
{
	uint8_t value[20];
	
	//unlock chip and change to SPI3	
	value[0] = 0x19; //CMD:00193247
	value[1] = 0x32; 
	value[2] = 0x47; 
	spi_master_write_register(0x00, 3, &value);		
	delay_us(10);	
	value[0] = 0x02; //CMD:0002334C
	value[1] = 0x33;
	value[2] = 0x4C; 
	spi_master_write_register(0x00, 3, &value);	
	delay_us(10);	
	value[0] = 0x01; //	CMD:0001334F
	value[1] = 0x33;
	value[2] = 0x4F;
	spi_master_write_register(0x00, 3, &value);	
	delay_us(10);	
	value[0] = 0x04; //	CMD:0004334A	
	value[1] = 0x33;
	value[2] = 0x4A;
	spi_master_write_register(0x00, 3, &value);
	delay_us(10);	
	value[0] = 0x00; //	CMD:0300334D
	value[1] = 0x33;
	value[2] = 0x4D;
	spi_master_write_register(0x03, 3, &value);
	delay_us(10);	
	value[0] = 0x80; //	CMD:01803347
	value[1] = 0x33;
	value[2] = 0x47;
	spi_master_write_register(0x01, 3, &value);	
	delay_us(10);	
	value[0] = 0x80; //	CMD:02803344
	value[1] = 0x33;
	value[2] = 0x44;
	spi_master_write_register(0x02, 3, &value);
	delay_us(10);	
	value[0] = 0x92; //	CMD:00923244
	value[1] = 0x32;
	value[2] = 0x44;
	spi_master_write_register(0x00, 3, &value);
	delay_us(10);	
	value[0] = 0x01; //	CMD:0001334F
	value[1] = 0x33;
	value[2] = 0x4F;
	spi_master_write_register(0x00, 3, &value);
	delay_us(10);	
}

void IAM20685_writeunlock() //Crystal-2 chip unlock, if locked
{
	uint8_t value[20];
	
	//unlock write chip
	spi_master_read_register(0xE4, 0, &value);//CMD:E4000288
	value[0] = 0x02;
	value[1] = 0x88;
	spi_master_write_register(0x00, 2, &value);	
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, &value);//CMD:E400018B
	value[0] = 0x01;
	value[1] = 0x8B;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, &value);//CMD:E400048E
	value[0] = 0x04;
	value[1] = 0x8E;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, &value);//CMD:E40300AD
	value[0] = 0x00;
	value[1] = 0xAD;
	spi_master_write_register(0x03, 2, &value);
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, &value);//CMD:E4018017
	value[0] = 0x80;
	value[1] = 0x17;
	spi_master_write_register(0x01, 2, &value);
	delay_us(10);
	
	spi_master_read_register(0xE4, 0, &value);//CMD:E4028030
	value[0] = 0x80;
	value[1] = 0x30;
	spi_master_write_register(0x02, 2, &value);
	delay_us(10);	
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
void IAM20685() //command=5, Crystal-2
{
	uint8_t value[20];
	uint8_t result = 0;
	int gx, gy, gz, ax, ay, az, t1, t2 ;
	
	ioport_set_pin_dir(CRYSTAL_RST, IOPORT_DIR_OUTPUT); //
	ioport_set_pin_level(CRYSTAL_RST, IOPORT_PIN_LEVEL_HIGH); //
	
	ext_int_initialize(ext_interrupt_handler);
	
	spi_master_init1(); //for ver-B and D+ board EVB
	//spi_master_init(); //for ver-A board EVB and on board sensor
	
	IAM20685_chipunlock();	
	
	value[0] = 0x00; //	CMD:2C0000F3
	value[1] = 0x00;
	value[2] = 0xF3;
	spi_master_write_register(0x2C, 3, &value);
	delay_us(10);
	spi_master_read_register(0x2c, 2, &value);
	delay_us(10);			
	printf("IAM20685 WHOAMI = %04x\r\n", (value[0] << 8 | value[1]));
	
	//while(1){};
	
	
		
	IAM20685_writeunlock();	
	
	spi_master_read_register(0xB0, 0, &value);//CMD:B0 00 00 F9, G and A YZ filter 60hz/60hz
	value[0] = 0x00;
	value[1] = 0xF9;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	spi_master_read_register(0xB8, 0, &value);//CMD:B8 00 00 A3, G and A X filter 60hz/60hz
	value[0] = 0x00;
	value[1] = 0xA3;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
		
	//set accel FSR=2.048g
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 06 62 change to bank6
	value[0] = 0x06;
	value[1] = 0x62;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	/*spi_master_read_register(0xD0, 0, &value);//CMD:D0 00 04 62
	value[0] = 0x04;
	value[1] = 0x62;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);	*/
	
	//accel FSR=4g
		spi_master_read_register(0xD0, 0, &value);//CMD:D0 00 07 61
		value[0] = 0x07;
		value[1] = 0x61;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
	
	//set gyro FSR=1966dps
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 07 63 change to bank7
	value[0] = 0x07;
	value[1] = 0x63;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	spi_master_read_register(0xD0, 0, &value);//CMD:D0 00 02 64
	value[0] = 0x02;
	value[1] = 0x64;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);	
	
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 00 64 change back to bank0
	value[0] = 0x00;
	value[1] = 0x64;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);	

	while(1)
	{		
	delay_us(10);
	if (1){
	//if (int_trigger){

	value[0] = 0x00; //	CMD:000000F1
	value[1] = 0x00;
	value[2] = 0xF1;
	spi_master_write_register(0x00, 3, &value);
	delay_us(5);
	spi_master_read_register(0x00, 2, &value);
	gx = (short)(value[0] << 8 | value[1]);
	delay_us(5);	
	
	value[0] = 0x00; //	CMD:040000DC
	value[1] = 0x00;
	value[2] = 0xDC;
	spi_master_write_register(0x04, 3, &value);
	delay_us(5);
	spi_master_read_register(0x04, 2, &value);
	gy = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	value[0] = 0x00; //	CMD:080000AB
	value[1] = 0x00;
	value[2] = 0xAB;
	spi_master_write_register(0x08, 3, &value);
	delay_us(5);
	spi_master_read_register(0x08, 2, &value);
	gz = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	value[0] = 0x00; //	CMD:0C000086
	value[1] = 0x00;
	value[2] = 0x86;
	spi_master_write_register(0x0C, 3, &value);
	delay_us(5);
	spi_master_read_register(0x0C, 2, &value);
	t1 = (short)(value[0] << 8 | value[1]);
	delay_us(5);

	value[0] = 0x00; //	CMD:10000045
	value[1] = 0x00;
	value[2] = 0x45;
	spi_master_write_register(0x10, 3, &value);	
	delay_us(5);	
	spi_master_read_register(0x10, 2, &value);
	ax = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	value[0] = 0x00; //	CMD:14000068
	value[1] = 0x00;
	value[2] = 0x68;
	spi_master_write_register(0x14, 3, &value);	
	delay_us(5);	
	spi_master_read_register(0x14, 2, &value);
	ay = (short)(value[0] << 8 | value[1]);
	delay_us(5);
		
	value[0] = 0x00; //	CMD:1800001F
	value[1] = 0x00;
	value[2] = 0x1F;
	spi_master_write_register(0x18, 3, &value);	
	delay_us(5);	
	spi_master_read_register(0x18, 2, &value);
	az = (short)(value[0] << 8 | value[1]);	
	delay_us(5);	
		
	value[0] = 0x00; //	CMD:1C000032
	value[1] = 0x00;
	value[2] = 0x32;
	spi_master_write_register(0x1C, 3, &value);
	delay_us(5);
	spi_master_read_register(0x1C, 2, &value);
	t2 = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	printf("%d,%d,  %d,%d,%d,  %d,%d,%d\r\n", t1, t2, ax, ay, az, gx, gy, gz);
	//int_trigger = 0;		
		}
	}
}

unsigned char IAM20685_crc_calculate(unsigned long data)
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

void IAM20685_McLaren_seq1() //command=2, Crystal-2
{
	uint8_t value[20];
	uint8_t result = 0;
	int gx, gy, gz, ax, ay, az, t1, t2 ;
	
	ioport_set_pin_dir(CRYSTAL_RST, IOPORT_DIR_OUTPUT); //
	ioport_set_pin_level(CRYSTAL_RST, IOPORT_PIN_LEVEL_HIGH); //
	
	ext_int_initialize(ext_interrupt_handler);
	
	spi_master_init1(); //for ver-B and D+ board EVB
	//spi_master_init(); //for ver-A board EVB and on board sensor
	
	IAM20685_chipunlock();	
	
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 00 64 change back to bank0
	value[0] = 0x00;
	value[1] = 0x64;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	value[0] = 0x00; //	CMD:2C0000F3
	value[1] = 0x00;
	value[2] = 0xF3;
	spi_master_write_register(0x2C, 3, &value);
	delay_us(10);
	spi_master_read_register(0x2c, 2, &value);
	delay_us(10);			
	printf("IAM20685 WHOAMI = %04x\r\n", (value[0] << 8 | value[1]));
		
	IAM20685_writeunlock();	
/*	
	spi_master_read_register(0xB0, 0, &value);//CMD:B0 00 00 F9, G and A YZ filter 60hz/60hz
	value[0] = 0x00;
	value[1] = 0xF9;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	spi_master_read_register(0xB8, 0, &value);//CMD:B8 00 00 A3, G and A X filter 60hz/60hz
	value[0] = 0x00;
	value[1] = 0xA3;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
		*/
	//Read gyro alarm status
	spi_master_read_register(0x44, 0, &value);//CMD:44 00 00 36
	value[0] = 0x00;
	value[1] = 0x36;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//T_code sequence
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 02 88 --> T-code 010
	value[0] = 0x02;
	value[1] = 0x88;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 01 8B --> T-code 001
	value[0] = 0x01;
	value[1] = 0x8B;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 04 8E --> T-code 100
	value[0] = 0x04;
	value[1] = 0x8E;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//set gyro FSR - Change to bank 7
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 07 63 change to bank7
	value[0] = 0x07;
	value[1] = 0x63;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//set gyro FSR=??dps
	spi_master_read_register(0xD0, 0, &value);//CMD:D0 00 00 66
	value[0] = 0x00;
	value[1] = 0x66;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);		

	//T_code sequence
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 02 88 --> T-code 010
	value[0] = 0x02;
	value[1] = 0x88;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 01 8B --> T-code 001
	value[0] = 0x01;
	value[1] = 0x8B;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 04 8E --> T-code 100
	value[0] = 0x04;
	value[1] = 0x8E;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
		
	//set accel FSR - Change to bank 6
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 06 62 change to bank6
	value[0] = 0x06;
	value[1] = 0x62;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//accel FSR=4g
	spi_master_read_register(0xD0, 0, &value);//CMD:D0 00 07 61
	value[0] = 0x07;
	value[1] = 0x61;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);

	//T_code sequence
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 02 88 --> T-code 010
	value[0] = 0x02;
	value[1] = 0x88;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 01 8B --> T-code 001
	value[0] = 0x01;
	value[1] = 0x8B;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 04 8E --> T-code 100
	value[0] = 0x04;
	value[1] = 0x8E;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//Read gyro FSR - Change to bank 7
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 07 63 change to bank7
	value[0] = 0x07;
	value[1] = 0x63;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//Read gyro FSR
	spi_master_read_register(0xD0, 0, &value);//CMD:50 00 00 AF
	value[0] = 0x00;
	value[1] = 0xAF; //IAM20685_crc_calculate(0x500000);
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//T_code sequence
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 02 88 --> T-code 010
	value[0] = 0x02;
	value[1] = 0x88;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 01 8B --> T-code 001
	value[0] = 0x01;
	value[1] = 0x8B;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 04 8E --> T-code 100
	value[0] = 0x04;
	value[1] = 0x8E;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//Read accel FSR - Change to bank 6
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 06 62 change to bank6
	value[0] = 0x06;
	value[1] = 0x62;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//Read accel FSR
	spi_master_read_register(0xD0, 0, &value);//CMD:50 00 00 AF
	value[0] = 0x07;
	value[1] = 0xAF; //IAM20685_crc_calculate(0x500000);//0x61;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
		
	/*spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 00 64 change back to bank0
	value[0] = 0x00;
	value[1] = 0x64;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);	*/

	while(1)
	{		
	delay_us(10);
	if (1){
	//if (int_trigger){

	value[0] = 0x00; //	CMD:000000F1
	value[1] = 0x00;
	value[2] = 0xF1;
	spi_master_write_register(0x00, 3, &value);
	delay_us(5);
	spi_master_read_register(0x00, 2, &value);
	gx = (short)(value[0] << 8 | value[1]);
	delay_us(5);	
	
	value[0] = 0x00; //	CMD:040000DC
	value[1] = 0x00;
	value[2] = 0xDC;
	spi_master_write_register(0x04, 3, &value);
	delay_us(5);
	spi_master_read_register(0x04, 2, &value);
	gy = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	value[0] = 0x00; //	CMD:080000AB
	value[1] = 0x00;
	value[2] = 0xAB;
	spi_master_write_register(0x08, 3, &value);
	delay_us(5);
	spi_master_read_register(0x08, 2, &value);
	gz = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	value[0] = 0x00; //	CMD:0C000086
	value[1] = 0x00;
	value[2] = 0x86;
	spi_master_write_register(0x0C, 3, &value);
	delay_us(5);
	spi_master_read_register(0x0C, 2, &value);
	t1 = (short)(value[0] << 8 | value[1]);
	delay_us(5);

	value[0] = 0x00; //	CMD:10000045
	value[1] = 0x00;
	value[2] = 0x45;
	spi_master_write_register(0x10, 3, &value);	
	delay_us(5);	
	spi_master_read_register(0x10, 2, &value);
	ax = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	value[0] = 0x00; //	CMD:14000068
	value[1] = 0x00;
	value[2] = 0x68;
	spi_master_write_register(0x14, 3, &value);	
	delay_us(5);	
	spi_master_read_register(0x14, 2, &value);
	ay = (short)(value[0] << 8 | value[1]);
	delay_us(5);
		
	value[0] = 0x00; //	CMD:1800001F
	value[1] = 0x00;
	value[2] = 0x1F;
	spi_master_write_register(0x18, 3, &value);	
	delay_us(5);	
	spi_master_read_register(0x18, 2, &value);
	az = (short)(value[0] << 8 | value[1]);	
	delay_us(5);	
		
	value[0] = 0x00; //	CMD:1C000032
	value[1] = 0x00;
	value[2] = 0x32;
	spi_master_write_register(0x1C, 3, &value);
	delay_us(5);
	spi_master_read_register(0x1C, 2, &value);
	t2 = (short)(value[0] << 8 | value[1]);
	delay_us(5);
	
	printf("READ FSR ONCE: %d,%d,  %d,%d,%d,  %d,%d,%d\r\n", t1, t2, ax, ay, az, gx, gy, gz);
	//int_trigger = 0;		
		}
	}
}

void IAM20685_McLaren_seq2() //command=2, Crystal-2
{
	uint8_t value[20];
	uint8_t result = 0;
	int gx, gy, gz, ax, ay, az, t1, t2 ;
	
	ioport_set_pin_dir(CRYSTAL_RST, IOPORT_DIR_OUTPUT); //
	ioport_set_pin_level(CRYSTAL_RST, IOPORT_PIN_LEVEL_HIGH); //
	
	ext_int_initialize(ext_interrupt_handler);
	
	spi_master_init1(); //for ver-B and D+ board EVB
	//spi_master_init(); //for ver-A board EVB and on board sensor
	
	IAM20685_chipunlock();	
	
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 00 64 change back to bank0
	value[0] = 0x00;
	value[1] = 0x64;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	value[0] = 0x00; //	CMD:2C0000F3
	value[1] = 0x00;
	value[2] = 0xF3;
	spi_master_write_register(0x2C, 3, &value);
	delay_us(10);
	spi_master_read_register(0x2c, 2, &value);
	delay_us(10);			
	printf("IAM20685 WHOAMI = %04x\r\n", (value[0] << 8 | value[1]));
		
	IAM20685_writeunlock();	
/*	
	spi_master_read_register(0xB0, 0, &value);//CMD:B0 00 00 F9, G and A YZ filter 60hz/60hz
	value[0] = 0x00;
	value[1] = 0xF9;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	spi_master_read_register(0xB8, 0, &value);//CMD:B8 00 00 A3, G and A X filter 60hz/60hz
	value[0] = 0x00;
	value[1] = 0xA3;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
		*/
	//Read gyro alarm status
	spi_master_read_register(0x44, 0, &value);//CMD:44 00 00 36
	value[0] = 0x00;
	value[1] = 0x36;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//T_code sequence
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 02 88 --> T-code 010
	value[0] = 0x02;
	value[1] = 0x88;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 01 8B --> T-code 001
	value[0] = 0x01;
	value[1] = 0x8B;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 04 8E --> T-code 100
	value[0] = 0x04;
	value[1] = 0x8E;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//set gyro FSR - Change to bank 7
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 07 63 change to bank7
	value[0] = 0x07;
	value[1] = 0x63;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//set gyro FSR=??dps
	spi_master_read_register(0xD0, 0, &value);//CMD:D0 00 00 66
	value[0] = 0x00;
	value[1] = 0x66;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);		

	//T_code sequence
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 02 88 --> T-code 010
	value[0] = 0x02;
	value[1] = 0x88;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 01 8B --> T-code 001
	value[0] = 0x01;
	value[1] = 0x8B;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 04 8E --> T-code 100
	value[0] = 0x04;
	value[1] = 0x8E;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
		
	//set accel FSR - Change to bank 6
	spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 06 62 change to bank6
	value[0] = 0x06;
	value[1] = 0x62;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);
	
	//accel FSR=4g
	spi_master_read_register(0xD0, 0, &value);//CMD:D0 00 07 61
	value[0] = 0x07;
	value[1] = 0x61;
	spi_master_write_register(0x00, 2, &value);
	delay_us(10);

	while(1)
	{	
		//T_code sequence
		spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 02 88 --> T-code 010
		value[0] = 0x02;
		value[1] = 0x88;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
		spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 01 8B --> T-code 001
		value[0] = 0x01;
		value[1] = 0x8B;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
		spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 04 8E --> T-code 100
		value[0] = 0x04;
		value[1] = 0x8E;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
	
		//Read gyro FSR - Change to bank 7
		spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 07 63 change to bank7
		value[0] = 0x07;
		value[1] = 0x63;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
	
		//Read gyro FSR
		spi_master_read_register(0xD0, 0, &value);//CMD:50 00 00 AF
		value[0] = 0x00;
		value[1] = 0xAF; //IAM20685_crc_calculate(0x500000);
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
	
		//T_code sequence
		spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 02 88 --> T-code 010
		value[0] = 0x02;
		value[1] = 0x88;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
		spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 01 8B --> T-code 001
		value[0] = 0x01;
		value[1] = 0x8B;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
		spi_master_read_register(0xE4, 0, &value);//CMD:E4 00 04 8E --> T-code 100
		value[0] = 0x04;
		value[1] = 0x8E;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
	
		//Read accel FSR - Change to bank 6
		spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 06 62 change to bank6
		value[0] = 0x06;
		value[1] = 0x62;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
	
		//Read accel FSR
		spi_master_read_register(0xD0, 0, &value);//CMD:50 00 00 AF
		value[0] = 0x07;
		value[1] = 0xAF; //IAM20685_crc_calculate(0x500000);//0x61;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);
		
		/*spi_master_read_register(0xFC, 0, &value);//CMD:FC 00 00 64 change back to bank0
		value[0] = 0x00;
		value[1] = 0x64;
		spi_master_write_register(0x00, 2, &value);
		delay_us(10);	*/
	
		delay_us(10);
		if (1){
		//if (int_trigger){

		value[0] = 0x00; //	CMD:000000F1
		value[1] = 0x00;
		value[2] = 0xF1;
		spi_master_write_register(0x00, 3, &value);
		delay_us(5);
		spi_master_read_register(0x00, 2, &value);
		gx = (short)(value[0] << 8 | value[1]);
		delay_us(5);	
	
		value[0] = 0x00; //	CMD:040000DC
		value[1] = 0x00;
		value[2] = 0xDC;
		spi_master_write_register(0x04, 3, &value);
		delay_us(5);
		spi_master_read_register(0x04, 2, &value);
		gy = (short)(value[0] << 8 | value[1]);
		delay_us(5);
	
		value[0] = 0x00; //	CMD:080000AB
		value[1] = 0x00;
		value[2] = 0xAB;
		spi_master_write_register(0x08, 3, &value);
		delay_us(5);
		spi_master_read_register(0x08, 2, &value);
		gz = (short)(value[0] << 8 | value[1]);
		delay_us(5);
	
		value[0] = 0x00; //	CMD:0C000086
		value[1] = 0x00;
		value[2] = 0x86;
		spi_master_write_register(0x0C, 3, &value);
		delay_us(5);
		spi_master_read_register(0x0C, 2, &value);
		t1 = (short)(value[0] << 8 | value[1]);
		delay_us(5);

		value[0] = 0x00; //	CMD:10000045
		value[1] = 0x00;
		value[2] = 0x45;
		spi_master_write_register(0x10, 3, &value);	
		delay_us(5);	
		spi_master_read_register(0x10, 2, &value);
		ax = (short)(value[0] << 8 | value[1]);
		delay_us(5);
	
		value[0] = 0x00; //	CMD:14000068
		value[1] = 0x00;
		value[2] = 0x68;
		spi_master_write_register(0x14, 3, &value);	
		delay_us(5);	
		spi_master_read_register(0x14, 2, &value);
		ay = (short)(value[0] << 8 | value[1]);
		delay_us(5);
		
		value[0] = 0x00; //	CMD:1800001F
		value[1] = 0x00;
		value[2] = 0x1F;
		spi_master_write_register(0x18, 3, &value);	
		delay_us(5);	
		spi_master_read_register(0x18, 2, &value);
		az = (short)(value[0] << 8 | value[1]);	
		delay_us(5);	
		
		value[0] = 0x00; //	CMD:1C000032
		value[1] = 0x00;
		value[2] = 0x32;
		spi_master_write_register(0x1C, 3, &value);
		delay_us(5);
		spi_master_read_register(0x1C, 2, &value);
		t2 = (short)(value[0] << 8 | value[1]);
		delay_us(5);
	
		printf("REPEAT READ FSR: %d,%d,  %d,%d,%d,  %d,%d,%d\r\n", t1, t2, ax, ay, az, gx, gy, gz);
		//int_trigger = 0;		
		}
	}
}


void processUartData(void)
{
   uint8_t  cmdId;

   
   xSemaphoreTake( sema_main_rx_uart, portMAX_DELAY);
   cmdId = main_rx_buf[0]; //when using teraterm
   
    switch (cmdId)
    {
	    case 50://ascii "2" command for IAM-20685 McLaren testing
	    {
		    IAM20685_McLaren_seq1();
		    break;
	    }
	    case 51://ascii "2" command for IAM-20685 McLaren testing
	    {
		    IAM20685_McLaren_seq2();
		    break;
	    }
		case 53://ascii "5" command for IAM-20685
		{
			IAM20685();
			break;
		}
		default:
		{
			break;
		}
	}
}
