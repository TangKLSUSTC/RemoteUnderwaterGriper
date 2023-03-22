/*
*************************************************************************
*											BCL
*						SUSTech Robotic Institute
*
*Filename		: listener_private.h
*Programmer	: Kailuan Tang
*Description: This file is the head file of private listener based on 
*							rt-thread RTOS.
*
*Version		:	V1.0
*************************************************************************
*/
#pragma once

#include "rtthread.h"
#include "main.h"
#include "IO_config.h"
#include "sys.h"
#if defined(USE_LISTENER)

#define LISTENER_PRIORITY 4
#define LISTENER_STACK 128
#define LISTENER_TICK 5


enum IO_event
{
	io_low=0,
	io_high,
	io_rising_edge,
	io_falling_edge
};
typedef enum IO_event io_event_t;

enum USART_event
{
	usart_rxcp=0,
	usart_idle,
	usart_rxerr
};
typedef enum USART_event usart_event_t;

enum device_type
{
	dev_null=0x11,
	gpio_device=0xa1,
	usart_device,
	can_device,
	i2c_device,
	spi_device
};
typedef enum device_type device_t;

struct io_scan
{
	uint8_t number;
	uint32_t io_state;
};

/*
state:	when device is gpio, the i-th bit of state is its on/off state.
				when device is others devices, the state is corresponding device state.
*/
struct listener_base
{
	uint8_t number:8;
	device_t device:8;
	uint16_t *device_data;
	uint32_t state;
	uint32_t his_sta;
	uint32_t xor_sta;
	union
	{
		io_group* piop;
		USART_TypeDef* puart;
		CAN_TypeDef* pcan;
		
		SPI_TypeDef* pspi;
	}device_handler;
	struct listener_base* next_lis;
};
typedef struct listener_base listener_t;

/*
	Extern varibles
*/
extern rt_thread_t listener_thread;
extern rt_event_t listener_event;
extern rt_mailbox_t listener_mailbox;

/*
	Function prototypes
*/
void listener_init();
void listener_main(void* param);

void IO_listener_init();
void IO_listen(listener_t* lis);
io_event_t IO_scan(io_base* iob);


/*	listener uses singly linked list	*/

/************************************************************************
*find_next_listener	: find if there exists next listener
*
*INPUT: listener_t* lis	:	listener struct pointer
*
*OUTPUT: uint8_t	:		0:	no next listener
*											1:	exist next listener
*
*WARNINGS: you need to 
*
========================================================================*/
__STATIC_INLINE uint8_t find_next_listener(listener_t* lis)
{
	if(lis->next_lis==lis)return 0;
	else return 1;
}

__STATIC_INLINE void add_next_listener(listener_t* lis_old, listener_t* lis_next)
{
	
}

__STATIC_INLINE void insert_listener(listener_t* lis_old, listener_t* lis_in)
{
	lis_in->next_lis=lis_old->next_lis;
	lis_old->next_lis=lis_in;
}

#endif





