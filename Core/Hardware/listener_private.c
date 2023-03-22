/*
*************************************************************************
*											BCL
*						SUSTech Robotic Institute
*
*Filename		: listener_private.c
*Programmer	: Kailuan Tang
*Description: This file is the source file of private listener based on 
*							rt-thread RTOS.
*
*Version		:	V1.0
*************************************************************************
*/


#include "listener_private.h"
#if defined(USE_LISTENER)
/*
	Declare thread_cb, mailbox and event struct.
*/
rt_thread_t listener_thread=RT_NULL;
rt_event_t key_event=RT_NULL  ;
rt_mailbox_t listener_mailbox=RT_NULL;

/*
	Declare listener control block struct
	
*/
listener_t gpio_listener0;
/************************************************************************
*listener_init	: initialize private listener thread
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function must be called after system boots.
*
========================================================================*/
void listener_init()
{
	/*	Initialize event set and mailbox first*/
	
	listener_event=rt_event_create("dev_event",RT_IPC_FLAG_FIFO);

	IO_listener_init();
	listener_thread=rt_thread_create("listener",listener_main,RT_NULL,\
										LISTENER_STACK,\
										LISTENER_PRIORITY,\
										LISTENER_TICK);
	if(listener_thread!=RT_NULL)
	{
		rt_thread_startup(listener_thread);
	}
}

/************************************************************************
*listener_main	: listener thread entry function, you can redefine this
*									function in another place.
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is defined with weak identifier, if it is not
*					 redefined, it won't cause any fault.
*
========================================================================*/
__WEAK void listener_main(void* param)
{
	while(1)
	{
		
		
		
		rt_thread_delay(5);
	}
}

/************************************************************************
*IO_listener_init	: initialize IO_listener, you can redefine this function
*										in another place.
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: you need to 
*
========================================================================*/
__WEAK void IO_listener_init()
{
	gpio_listener0.device=dev_null;
	gpio_listener0.device_data=NULL;
	gpio_listener0.number=0;
	gpio_listener0.state=0;
	gpio_listener0.his_sta=0;
	gpio_listener0.next_lis=&gpio_listener0;
}

/************************************************************************
*IO_listener	: listen io event, rising edge and falling edge
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: none
*
========================================================================*/
__WEAK void IO_listener(listener_t* lis, io_group* iog)
{
	/*
		If there exists another gpio listener struct
	*/
	listener_t* lis_tmp=lis;
	RT_ASSERT(lis_tmp->device==gpio_device);
	io_event_t event;
	do
	{
		/*
			Scan IO list
		*/
		for(uint8_t i=0;i<lis_tmp->number;i++)
		{
			event=IO_scan(&(lis_tmp->device_handler.piop->iob[i]));
			write_bit(lis_tmp->state,i,event);
			write_bit(lis_tmp->xor_sta,i,(event^read_bit(lis_tmp->his_sta,i)));
		}
		/*
			Put state scanned just now into history sta
		*/
		lis_tmp->his_sta=lis_tmp->state;
		lis_tmp=lis_tmp->next_lis;
	}while(find_next_listener(lis_tmp));
	
}


__WEAK io_event_t IO_scan(io_base* iob)
{
	uint8_t sta=2;
	sta=io_read(iob);
	if(sta==0)return io_low;
	else if(sta==1)return io_high;
}


#endif

