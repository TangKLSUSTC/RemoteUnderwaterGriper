#include "LC_tech_F767.h"

const io_base led={PD,Pin11};
const io_base led_ex={PB,Pin13};

void board_led_init()
{
	io_init(&led);
	io_control(&led,0);
	io_init(&led_ex);
	io_control(&led_ex,0);
}

void soft_reset(void)
{
	__NVIC_SystemReset();
}