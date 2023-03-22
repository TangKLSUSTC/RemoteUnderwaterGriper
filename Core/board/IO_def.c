#include "IO_def.h"
#include "rtthread.h"
#include "bsp_config.h"
#include "bit_io.h"



void io_group_init(const io_group *p)
{
	GPIO_InitTypeDef ini;
	#ifdef STM32F767xx
	if(IS_GPIO_MODE(p->Mode))ini.Mode=p->Mode;
	if(IS_GPIO_PULL(p->Pull))ini.Pull=p->Pull;
	if(IS_GPIO_SPEED(p->Speed))ini.Speed=p->Speed;
	if(p->Alternate!=0xff)			//If alternate mode is configured
	{
		if(IS_GPIO_AF(p->Alternate))ini.Alternate=p->Alternate;
	}
	for(unsigned char i=0;i<p->number;i++)
	{
		if(IS_GPIO_PIN(p->iop[i].order))ini.Pin=p->iop[i].order;
		HAL_GPIO_Init(p->iop[i].port,&ini);
		io_control(&p->iop[i],0);
	}
	#endif
	#if defined(STM32F103xE)||defined(STM32F103xB)
	if(IS_GPIO_MODE(p->Mode))ini.Mode=p->Mode;
		if(IS_GPIO_PULL(p->Pull))ini.Pull=p->Pull;
		if(IS_GPIO_SPEED(p->Speed))ini.Speed=p->Speed;
		for(unsigned char i=0;i<p->number;i++)
		{
			if(IS_GPIO_PIN(p->iob[i].order))ini.Pin=p->iob[i].order;
			HAL_GPIO_Init(p->iob[i].port,&ini);
			io_control(&p->iob[i],0);
		}
		/*	Singly link model test*/
		/*	DO NOT DELETE!	*/
		
//	io_group* iop_tmp1,*iop_tmp2;
//	iop_tmp2=p;
//	do{
//		iop_tmp1=iop_tmp2;
//		if(IS_GPIO_MODE(iop_tmp1->Mode))ini.Mode=iop_tmp1->Mode;
//		if(IS_GPIO_PULL(iop_tmp1->Pull))ini.Pull=iop_tmp1->Pull;
//		if(IS_GPIO_SPEED(iop_tmp1->Speed))ini.Speed=iop_tmp1->Speed;
//		for(unsigned char i=0;i<iop_tmp1->number;i++)
//		{
//			if(IS_GPIO_PIN(iop_tmp1->iob[i].order))ini.Pin=iop_tmp1->iob[i].order;
//			HAL_GPIO_Init(iop_tmp1->iob[i].port,&ini);
//			io_control(&iop_tmp1->iob[i],0);
//		}
//		iop_tmp2=iop_tmp1->next_group;
//	}while((iop_tmp1->next_group!=NULL)||(iop_tmp2!=iop_tmp1));
	#endif
}

/**
*This function is used to init discret io defination. Default mode is 
*output, push pull mode.
* @param io_base struct
* @return none
*
**/
void io_init(const io_base *io)
{
	GPIO_InitTypeDef ini;
	ini.Pull=GPIO_PULLDOWN;
	ini.Mode=GPIO_MODE_OUTPUT_PP;
	ini.Speed=GPIO_SPEED_FREQ_MEDIUM;
	ini.Pin=io->order;
	HAL_GPIO_Init(io->port,&ini);
}
#ifdef USE_HAL_DRIVER
	
#endif
#ifdef USE_FULL_LL_DRIVER
	
#endif

io_event_t io_scan(io_base* iob,uint8_t his_sta)
{
	uint8_t key_sta=0,tmp=0;	
	key_sta=io_read(iob);
	tmp=his_sta^key_sta;
	/*Key state changes*/
	if(tmp==1)
	{
		/*Rising edge*/
		if(his_sta==0)return io_rising_edge;
		else if(his_sta==1)return io_falling_edge;
	}
	else if(tmp==0)return io_nochange;
}


void GPIO_CLK_ENA(void)
{
  /* GPIO Ports Clock Enable */
#ifdef STM32F767xx
	#ifdef USE_GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOB)
	__HAL_RCC_GPIOB_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOC)
	__HAL_RCC_GPIOC_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOD)
	__HAL_RCC_GPIOD_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOE)
	__HAL_RCC_GPIOE_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOF)
	__HAL_RCC_GPIOF_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOG)
	__HAL_RCC_GPIOG_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOH)
	__HAL_RCC_GPIOH_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOI)
	__HAL_RCC_GPIOI_CLK_ENABLE();
	#endif
#endif
#ifdef STM32F103xE
	#ifdef USE_GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOB)
	__HAL_RCC_GPIOB_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOC)
	__HAL_RCC_GPIOC_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOD)
	__HAL_RCC_GPIOD_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOE)
	__HAL_RCC_GPIOE_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOF)
	__HAL_RCC_GPIOF_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOG)
	__HAL_RCC_GPIOG_CLK_ENABLE();
	#endif
#endif
#if defined(STM32F103xB)
	#ifdef USE_GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOB)
	__HAL_RCC_GPIOB_CLK_ENABLE();
	#endif
	#if defined(USE_GPIOC)
	__HAL_RCC_GPIOC_CLK_ENABLE();
	#endif
#endif
}
#ifdef USE_RTT
INIT_BOARD_EXPORT(GPIO_CLK_ENA);
#endif

#ifdef __cplusplus

#endif
