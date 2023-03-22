#pragma once

#include "bsp_config.h"
#include "IO_def.h"
#include "sys.h"
#include "core_cm3.h"

#ifdef USE_HAL_DRIVER
#define BOARD_LED0_Toggle (HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_11))


#endif