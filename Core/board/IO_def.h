#pragma once
	#if defined(STM32F103xE)
		#include "stm32f1xx.h"
		#include "stm32f1xx_ll_gpio.h"
		#include "main.h"
	#endif

	#if defined(STM32F767xx)
		#include "stm32f767xx.h"
		#include "main.h"
	#endif
	#if defined(STM32F103xB)
		#include "stm32f1xx.h"
	#endif
#include "bsp_config.h"
#include "bit_io.h"
#define PA GPIOA
#define PB GPIOB
#define PC GPIOC
#define PD GPIOD
#define PE GPIOE
#define PF GPIOF
#define PG GPIOG
#define PI GPIOI
#define PH GPIOH

#define Pin0 GPIO_PIN_0
#define Pin1 GPIO_PIN_1
#define Pin2 GPIO_PIN_2
#define Pin3 GPIO_PIN_3
#define Pin4 GPIO_PIN_4
#define Pin5 GPIO_PIN_5
#define Pin6 GPIO_PIN_6
#define Pin7 GPIO_PIN_7
#define Pin8 GPIO_PIN_8
#define Pin9 GPIO_PIN_9
#define Pin10 GPIO_PIN_10
#define Pin11 GPIO_PIN_11
#define Pin12 GPIO_PIN_12
#define Pin13 GPIO_PIN_13
#define Pin14 GPIO_PIN_14
#define Pin15 GPIO_PIN_15



#ifndef __cplusplus

enum ioevent
{
	io_nochange=0,
	io_rising_edge,
	io_falling_edge
};
typedef enum ioevent io_event_t;




/*Basic struct of define IO in group*/
typedef struct io_b
{
	GPIO_TypeDef* port;
	uint32_t			order;
}io_base,*io_base_p;

struct group_b
{
	io_base* iob;
//	const io_base iog;
//	union 
//	{
//		const io_base* iop;
//		const io_base iog;
//	}io_part;
	uint32_t Mode;
	uint32_t Pull;
	uint32_t Speed;
	#ifdef STM32F767xx
	uint32_t Alternate;
	#endif
	uint32_t number;				//IO组内端口数量
	struct group_b* next_group;
};
typedef struct group_b io_group;
/*Invivadual IO define*/
typedef struct io_i
{
	GPIO_TypeDef* port;
	uint32_t			order;
	uint32_t Mode;
	uint32_t Pull;
	uint32_t Speed;
}io_id;

//extern const io_base muscle_in[12];
//extern const io_base muscle_out[12];
void io_init(const io_base *io);
void io_group_init(const io_group *p);
io_event_t io_scan(io_base* iob,uint8_t his_sta);
//void io_control(const io_base *p,char x);
//inline uint8_t io_read(const io_base *p);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*		Inline functions		*/
__STATIC_INLINE void io_control(const io_base *p,char x)
{
	x?\
	HAL_GPIO_WritePin(p->port,p->order,GPIO_PIN_SET):\
	HAL_GPIO_WritePin(p->port,p->order,GPIO_PIN_RESET);
//	p->port->BSRR=\
//	x?\
//	p->order:\
//	(uint32_t)p->order << 16u;
}

__STATIC_INLINE uint8_t io_read(const io_base *p)
{
	if((p->port->IDR&p->order)!=0)return 1;
	else return 0;
}

#endif
#ifdef __cplusplus
class group{};
#endif





