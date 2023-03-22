#ifndef _SYS_H
#define _SYS_H

#ifdef STM32F767xx
	#include "stm32f7xx.h"
	#include "core_cm7.h"
	#include "stm32f7xx_hal.h"
#endif
#if defined(STM32F103xE)||defined(STM32F103xB)
	#include "stm32f1xx_hal.h"
	#if defined(STM32F103xE)
	#include "stm32f103xe.h"
	#endif
	#if defined(STM32F103xB)
	#include "stm32f103xb.h"
	#endif
	#include "core_cm3.h"
#endif

/*Inlcude files are depended on enabled peripherals*/
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"
#include "led.h"
#include "usart.h"
#include "controller.h"

//0,��֧��os
//1,֧��os
#define SYSTEM_SUPPORT_OS		0		//����ϵͳ�ļ����Ƿ�֧��OS	
///////////////////////////////////////////////////////////////////////////////////
//����һЩ���õ��������Ͷ̹ؼ��� 
//typedef int32_t  s32;
//typedef int16_t s16;
//typedef int8_t  s8;

//typedef const int32_t sc32;  
//typedef const int16_t sc16;  
//typedef const int8_t sc8;  

//typedef __IO int32_t  vs32;
//typedef __IO int16_t  vs16;
//typedef __IO int8_t   vs8;

//typedef __I int32_t vsc32;  
//typedef __I int16_t vsc16; 
//typedef __I int8_t vsc8;   

//typedef uint32_t  uint32_t;
//typedef uint16_t uint16_t;
//typedef uint8_t  uint8_t;

//typedef const uint32_t uc32;  
//typedef const uint16_t uc16;  
//typedef const uint8_t uc8; 

//typedef __IO uint32_t  vuint32_t;
//typedef __IO uint16_t vuint16_t;
//typedef __IO uint8_t  vuint8_t;

//typedef __I uint32_t vuc32;  
//typedef __I uint16_t vuc16; 
//typedef __I uint8_t vuc8;  

#define ON	1
#define OFF	0
#define Write_Through() (*(__IO uint32_t*)0XE000EF9C=1UL<<2) //Cache͸дģʽ ��SCB->CACR|=1<<2;����һ��
 
void Cache_Enable(void);                                    //ʹ��STM32F7��L1-Cahce
void Stm32_Clock_Init(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq); //����ϵͳʱ��
uint8_t Get_ICahceSta(void);//�ж�I_Cache�Ƿ��
uint8_t Get_DCahceSta(void);//�ж�I_Dache�Ƿ��
void SystemClock_Config(void);
//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(uint32_t addr);	//���ö�ջ��ַ 
#endif

