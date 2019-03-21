/*
 * inv_uart.h
 *
 * Created: 7/11/2017 3:48:58 PM
 *  Author: TDK InvenSense
 */ 


#ifndef INV_UART_H_
#define INV_UART_H_

#include <stdint.h>
/* Configuration for console uart IRQ */
#define CONSOLE_UART_IRQn           FLEXCOM0_IRQn

/* Configuration for console uart IRQ handler */
#define console_uart_irq_handler    FLEXCOM0_Handler

void configure_console(void);
void console_uart_irq_handler(void);
void uart_main_dma_puts(uint32_t size);
bool keypress (uint8_t * chptr);

#endif /* INV_UART_H_ */