/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "inv/inv_uart.h"
#include "inv/inv_i2c.h"
#include "inv/inv_spi.h"
#include "inv/protocol_functions.h"
#include "inv/bsp.h"
#include "FreeRTOS.h"


uint8_t main_rx_buf[MAXDATABUF];
uint8_t main_tx_buf[MAXDATABUF];
Pdc *p_pdc;

SemaphoreHandle_t sema_main_tx_uart;
SemaphoreHandle_t sema_main_rx_uart;

SemaphoreHandle_t sema_sensor_int_pb03;

TaskHandle_t xMainTask;
void vMainTask(void* pvParameters);
#define   mainTastDelay        1 /* scheduler period in ms */

TaskHandle_t xDMATask;
void vDMATask(void* pvParameters);

TaskHandle_t xSensorIntPB03Task;
void vSensorIntPB03Task(void* pvParameters);

int UI_IF = 1; //I2C=1, SPI=0

int main (void)
{
	sysclk_init();
	
	board_init();
	
	/*
	if (UI_IF)
		board_init_I2C();
	else 
		board_init();
*/

	//USART config
	configure_console();
	
	sema_main_rx_uart = xSemaphoreCreateBinary();
	sema_main_tx_uart = xSemaphoreCreateBinary();
	
	p_pdc = usart_get_pdc_base(CONF_UART);
	xTaskCreate( vMainTask  , "vMainTask"   , configMINIMAL_STACK_SIZE+1024, NULL, tskIDLE_PRIORITY+1, &xMainTask);
	xTaskCreate( vSensorIntPB03Task  , "vSensorIntPB03Task"   , configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &xSensorIntPB03Task);
	
	vTaskStartScheduler();	
}

void vMainTask(void* pvParameters)
{
	
	while(1)
	{
		printf("\r\n 1: Testmain 3: Original, 5: Modified,   9: PRM \r\n");
		processUartData();
	}
}

void vDMATask(void* pvParameters)
{
	while(1)
	{
		if(p_pdc->PERIPH_TCR == 0)
			xSemaphoreGive(sema_main_tx_uart);
	}
}

void vSensorIntPB03Task(void* pvParameters)
{
	sema_sensor_int_pb03 = xSemaphoreCreateBinary();
	while(1)
	{
		xSemaphoreTake(sema_sensor_int_pb03, portMAX_DELAY);
		//sensor_irq_proc(PIN_EXT_INTERRUPT_MASK);
	}
}