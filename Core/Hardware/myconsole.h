#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include <rtthread.h>
#include <string.h>
#include "sys.h"
#include "main.h"
#include "usart.h"
#include "my_ringbuf_8bit.h"


#ifdef STM32F767xx
#include "stm32f7xx_hal_usart.h"
#include "test_ring.h"
#endif

#ifdef STM32F103xE
#include "stm32f1xx_hal_usart.h"
#endif


void my_console_init(void);
void rt_hw_console_output(const char* str);
void usart_init(uint32_t bound);

extern uint8_t aRxBuffer[RXBUFFERSIZE];
extern UART_HandleTypeDef UART1_Handler;


#ifdef __cplusplus
}
#endif












