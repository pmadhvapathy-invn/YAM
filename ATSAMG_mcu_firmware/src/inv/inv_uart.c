/*
 * uart.c
 *
 * Created: 7/11/2017 11:31:37 AM
 *  Author: TDK InvenSense
 */ 

 
/************************ UART ************************************************/
#include <asf.h>
#include "../src/inv/inv_uart.h"
#include "inv_i2c.h"
#include "protocol_functions.h"
//#include "../src/ASF/common/services/serial/sam_uart/uart_serial.h"
//#include "ASF/sam/drivers/pdc/pdc.h"
#include "stdio_serial.h"
//#include "ASF/thirdparty/freertos/freertos-8.0.1/Source/include/semphr.h"
//#include "ASF/thirdparty/freertos/freertos-8.0.1/Source/include/task.h"

/* Pdc transfer buffer */
//uint8_t g_uc_pdc_buffer[BUFFER_SIZE];
/* PDC data packet for transfer */
pdc_packet_t g_pdc_usart_packet;
/* Pointer to USART PDC register base */
Pdc *g_p_usart_pdc;

extern SemaphoreHandle_t sema_main_rx_uart;
extern uint8_t main_rx_buf[MAXDATABUF];

extern SemaphoreHandle_t sema_main_tx_uart;
extern uint8_t main_tx_buf[MAXDATABUF];

uint8_t main_rx_buf_ind = 0;

void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONF_UART_ID);
	usart_serial_init(CONF_UART, (usart_serial_options_t *)&uart_serial_options);

	stdio_serial_init(CONF_UART, &uart_serial_options);

	usart_enable_rx(CONF_UART);
	usart_enable_tx(CONF_UART);

	/* Enable UART IRQ */
	//usart_enable_interrupt(CONF_UART, (US_IER_RXRDY | US_IER_ENDTX));
	usart_enable_interrupt(CONF_UART, US_IER_RXRDY );
	/* Enable UART interrupt */
	NVIC_SetPriority(CONSOLE_UART_IRQn, 0);

	/* Enable UART interrupt */
	NVIC_EnableIRQ(CONSOLE_UART_IRQn);

}
bool gpio_level_0 = false;
/**
 * \brief Interrupt handler for USART interrupt.
 */
void console_uart_irq_handler(void)
{
	uint32_t ul_status;
	
	/* Read USART Status. */
	ul_status = usart_get_status(CONF_UART);

	if((ul_status &  US_CSR_RXRDY ))
	{
		uint8_t rxbyte;
		usart_serial_getchar(CONF_UART, &rxbyte);
		main_rx_buf[0] = rxbyte;
		//main_rx_buf[main_rx_buf_ind++] = rxbyte;
		//if ((main_rx_buf[0]) == main_rx_buf_ind)
		//{
			xSemaphoreGive(sema_main_rx_uart);
			//main_rx_buf_ind = 0;
		//}
		//else
			//main_rx_buf_ind++;
	}
	if((ul_status &  US_CSR_ENDTX ))
	{
		xSemaphoreGive(sema_main_tx_uart);
	}
}

void uart_main_dma_puts(uint32_t size)
{
	ioport_set_pin_level(LED0_GPIO, IOPORT_PIN_LEVEL_HIGH);
	/* Get pointer to USART PDC register base */
	g_p_usart_pdc = usart_get_pdc_base(CONF_UART);

	/* Initialize PDC data packet for transfer */
	g_pdc_usart_packet.ul_addr = (uint32_t) main_tx_buf;
	g_pdc_usart_packet.ul_size = size;

	xSemaphoreTake( sema_main_tx_uart, portMAX_DELAY);
	
	/* Configure PDC for data receive */
	pdc_tx_init(g_p_usart_pdc, &g_pdc_usart_packet, NULL);

	/* Enable PDC transfers */
	pdc_enable_transfer(g_p_usart_pdc, PERIPH_PTCR_TXTEN);
	
}

/**
 * \brief poll once for key press.
 */
bool keypress (uint8_t * chptr)
{
	bool ret = false;
	/* Read USART Status. */
	uint32_t ul_status = usart_get_status(CONF_UART);
	if((ul_status &  US_CSR_RXRDY ))
	{
	   usart_serial_getchar(CONF_UART, chptr);
	   xSemaphoreGive(sema_main_rx_uart);
	   ret = true;
	}
	 
	return ret;
}