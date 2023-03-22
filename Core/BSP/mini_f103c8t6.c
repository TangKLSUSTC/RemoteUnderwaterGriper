/*
*************************************************************************
*											BCL
*						SUSTech Robotic Institute
*
*Filename		: mini_f103c8t6.c
*Programmer	: Kailuan Tang
*Description: This file is source file of board support pack of mini_f103c8t6
*							board.
*
*Version		:	V1.0
*************************************************************************
*/
#include "mini_f103c8t6.h"

/*Led pin on board*/
const io_base led={PC,Pin13};

void board_led_init()
{
	io_init(&led);
	io_control(&led,0);
}

void soft_reset(void)
{
	__NVIC_SystemReset();
}