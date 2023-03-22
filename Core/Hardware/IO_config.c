#include "IO_config.h"
#ifdef __cplusplus
extern "C" {
#endif
	#ifdef USE_HAL_DRIVER
				/*	Muscles IO config	*/
/*++++++++++++++++++++++++++++++++++++++*/
io_base muscle_in[4]=\
{
	{PB,Pin3},
	{PB,Pin4},
	{PA,Pin6},
	{PA,Pin7}
};
io_group mi={muscle_in,\
										GPIO_MODE_OUTPUT_PP,\
										GPIO_PULLDOWN,\
										GPIO_SPEED_FREQ_MEDIUM,4,&mo};
io_base muscle_out[4]=\
{
	{PB,Pin0},
	{PB,Pin1},
	{PC,Pin14},
	{PC,Pin15}
};
io_group mo={muscle_out,\
										GPIO_MODE_OUTPUT_PP,\
										GPIO_PULLDOWN,\
										GPIO_SPEED_FREQ_MEDIUM,4,&pp};
				/*	Pump control pin	*/
/*++++++++++++++++++++++++++++++++++++++*/
io_base pump[2]=\
{
	{PB,Pin12},
	{PB,Pin13}
};
io_group pp={pump,\
										GPIO_MODE_OUTPUT_PP,\
										GPIO_PULLDOWN,\
										GPIO_SPEED_FREQ_MEDIUM,2};
			 /*	 Griper control pin	*/
/*++++++++++++++++++++++++++++++++++++++*/
io_base griper_open={PB,Pin15};
io_base griper_close={PB,Pin14};

			 /*Input Pins*/
/*======================================*/

			 /*	 Griper control pin	*/
/*++++++++++++++++++++++++++++++++++++++*/
io_base griper_switch[2]=\
{
	{PB,Pin7},
	{PB,Pin8}
};
io_group gswitch={griper_switch,\
												GPIO_MODE_INPUT,\
												GPIO_PULLDOWN,\
												GPIO_SPEED_FREQ_HIGH,2,&arm_flex};
io_base arm_flex_pin[2]=\
{
	{PB,Pin5},
	{PB,Pin6}
};
io_group arm_flex={arm_flex_pin,\
												GPIO_MODE_INPUT,\
												GPIO_PULLDOWN,\
												GPIO_SPEED_FREQ_HIGH,2,&arm_flex};
	#endif
#ifdef USE_FULL_LL_DRIVER
	
#endif
#ifdef __cplusplus
}
#endif