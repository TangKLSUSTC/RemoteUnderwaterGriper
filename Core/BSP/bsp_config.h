#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F103xE
	#define DOFLYE_F103ZET
	#define USE_GPIOA
	#define USE_GPIOB
	#define USE_GPIOC
	#define USE_GPIOD
	#define USE_GPIOE
	#define USE_GPIOF
	#define USE_GPIOG
	#define USE_CAN1
#endif

#ifdef STM32F103xB

	#define MINI_F103C8T6
	
	#define USE_GPIOA
	#define USE_GPIOB
	#define USE_GPIOC
	
	#define USE_LISTENER
#endif

#ifdef STM32F767xx
	#define LC_tech_F767IG
	#define USE_GPIOA
	#define USE_GPIOB
	#define USE_GPIOC
	#define USE_GPIOD
	#define USE_GPIOE
	#define USE_GPIOF
	#define USE_GPIOG
	#define USE_GPIOH
	#define USE_GPIOI
	#define USE_CAN1
#endif



#ifdef __cplusplus
}
#endif







