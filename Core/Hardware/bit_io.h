#pragma once

#include <stdint.h>
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
__STATIC_INLINE uint8_t read_bit(const uint32_t *reg, uint8_t num)
{
	if(num<32)	return (*reg>>num)&0b1;
	else return 2;
}

__STATIC_INLINE void write_bit(uint32_t* reg, uint8_t num, uint32_t x)
{
	if(num<32&&(x==0||x==1))
	{
		switch(x)
		{
			case 0:
				{
					*reg&=~(1<<num);
					break;
				}
			case 1:
				{
					*reg|=(1<<num);
					break;
				}
		}
		return;
	}
	else return;
}

__STATIC_INLINE void AND_bit(uint32_t* reg, uint8_t num, uint32_t x)
{
	if(num<32&&(x==0||x==1))
	{
		switch(x)
		{
			case 0:
				{
					*reg&=~(1<<num);
					break;
				}
			case 1:
				{
					//*reg|=(1<<num);
					break;
				}
		}
		return;
	}
	else return;
}

__STATIC_INLINE void XOR_bit(uint32_t* reg, uint8_t num, uint32_t x)
{
	if(num<32&&(x==0||x==1))
	{
		uint8_t tmp = x^((*reg>>num)&0b1);
		switch(tmp)
		{
			case 0:
				{
					*reg&=~(1<<num);
					break;
				}
			case 1:
				{
					*reg|=(1<<num);
					break;
				}
			return;
		}
	}
	else return;
}

__STATIC_INLINE void OR_bit(uint32_t* reg, uint8_t num, uint32_t x)
{
	if(num<32&&(x==0||x==1))
	{
		switch(x)
		{
			case 0:
				{
					//*reg&=~(1<<num);
					break;
				}
			case 1:
				{
					*reg|=(1<<num);
					break;
				}
		}
		return;
	}
	else return;
}