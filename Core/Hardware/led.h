#ifndef _LED_H
#define _LED_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/6/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//LED�˿ڶ���
//#define LED0(n)		(n?HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET):HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_SET))
//#define LED0_Toggle (HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_11)) //LED0�����ƽ��ת


__weak void LED_Init(void); //LED��ʼ������
#endif
