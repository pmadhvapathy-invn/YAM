/*
 * CFile1.c
 *
 * Created: 1/21/2019 1:47:52 PM
 *  Author: pmadhvapathy
 */ 
#include <asf.h>
#include <stdio.h>
#include <stdlib.h>
#include "protocol_functions.h"
#include "inv_uart.h"
#include "inv_spi.h"
#include "yamtests.h"
#include "bsp.h"




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

void initBSP(bool bRev)
{
	ioport_set_pin_dir(CRYSTAL_RST, IOPORT_DIR_OUTPUT); //
	ioport_set_pin_level(CRYSTAL_RST, IOPORT_PIN_LEVEL_HIGH); //
	
	ext_int_initialize(ext_interrupt_handler);
	
	if (bRev)
	  spi_master_init1(); //for ver-B and D+ board EVB
	 else
	  spi_master_init(); //for ver-A board EVB and on board sensor

}

void processUartData(void)
{
	uint8_t  cmdId;

	
	xSemaphoreTake( sema_main_rx_uart, portMAX_DELAY);
	

	
	cmdId = main_rx_buf[0]; //when using teraterm
	
	printf(" Running test %c  ... Press ^X to exit test and enter new test key\r\n", cmdId); 
	
	switch (cmdId)
	{
		case '1':  //command to run IAM-20685 Test code
		{
			TestMain();	
			break;
		}
		case '3':// command for IAM-20685
		{
			IAM20685Original();
			break;
		}
		
		case '5':// command for IAM-20685
		{
			IAM20685();
			break;
		}
		case '9'://command for IAM-20685
		{
			IAM20685_PRM();
			break;
		}
		
		
		default:
		{
			printf ("default  case");
			break;
		}
	}
}

#define CTRLX 0x18
bool userexit()
{
	bool retVal = false;
	uint8_t c;
	if (xSemaphoreTake( sema_main_rx_uart, 0))  // sample, dont wait.  There's always a next time
	{
	  c = main_rx_buf[0];
	  if (c == CTRLX)
	    retVal = true;
	}
	return retVal;
}
