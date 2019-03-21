/*
 * IncFile1.h
 *
 * Created: 1/21/2019 1:48:21 PM
 *  Author: pmadhvapathy
 */ 


#ifndef BSP_H_
#define BSP_H_

#define boardSPIRev 1

void ext_int_initialize(void (*handler_function)(void));
void ext_interrupt_handler(void);
void initBSP(bool boardRev);
bool userexit();
void processUartData(void);

extern SemaphoreHandle_t sema_main_rx_uart;
extern uint8_t main_rx_buf[MAXDATABUF];

extern SemaphoreHandle_t sema_main_tx_uart;
extern uint8_t main_tx_buf[MAXDATABUF];

extern SemaphoreHandle_t sema_sensor_int_pb03;
//SensorDesc_t sensor_desc;


#endif /* BSP_H_ */