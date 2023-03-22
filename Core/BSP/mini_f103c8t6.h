/*
*************************************************************************
*											BCL
*						SUSTech Robotic Institute
*
*Filename		: mini_f103c8t6.h
*Programmer	: Kailuan Tang
*Description: This file is head file of board support pack of mini_f103c8t6
*							board.
*
*Version		:	V1.0
*************************************************************************
*/
#pragma once


#include "bsp_config.h"
#include "IO_def.h"
#include "core_cm3.h"


#define BOARD_LED0_Toggle (HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13))
extern const io_base led;
void board_led_init();
