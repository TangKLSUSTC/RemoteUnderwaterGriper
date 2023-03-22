/*
*************************************************************************
*											BCL
*						SUSTech Robotic Institute
*
*Filename		: mod_F1.c
*Programmer	: Kailuan Tang
*Description: This file is a individual private inter-board communication
							application functions.
*
*Version		:	V1.0
*************************************************************************
*/
#ifdef STM32F103xE
#ifdef __cplusplus
extern "C" {
#endif
#include "mod_F1.h"

#ifdef USE_RTT
	static rt_thread_t mod_conmmu = RT_NULL;
#endif
rt_uint8_t mod_rx_buf[48];
rt_uint8_t mod_tx_buf[48];
uint16_t channel_rx_arr[12];						//接收通道值数组
uint16_t value_rx_arr[12];							//接收控制值数组

static uint8_t channel_tx_arr[24];							//发送通道值
static uint8_t value_tx_arr[24];								//发送控制值

struct integrity_check chk;							//帧校验

struct rt_semaphore mod_sem;
struct rt_ringbuffer mod_rx;						//接收缓存
struct rt_ringbuffer channel_rx;
struct rt_ringbuffer value_rx;
struct rt_ringbuffer mod_tx;					//发送缓存
struct rt_ringbuffer channel_tx;
struct rt_ringbuffer value_tx;
struct mod_cb modci;
struct frame_cb fmci;

/************************************************************************
*mod_init	: initialize private modbus protocol
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function must be called after system boots.
*
========================================================================*/
void mod_init()
{
	rt_ringbuffer_init(&mod_rx,(uint16_t*)mod_rx_buf,48); 
	rt_ringbuffer_init(&channel_rx,channel_rx_arr,12);
	rt_ringbuffer_init(&value_rx,value_rx_arr,12);
	rt_ringbuffer_init(&mod_tx,(uint16_t*)mod_tx_buf,48);
	rt_ringbuffer_init(&channel_tx,(uint16_t*)channel_tx_arr,24);
	rt_ringbuffer_init(&value_tx,(uint16_t*)value_tx_arr,24);
	rt_sem_init(&mod_sem,"modu",0,0);
	modci.use=0;
	modci.length=0;
	modci.CRC_val=0;
	modci.channel=&channel_rx;
	modci.value=&value_rx;
	modci.rx=&mod_rx;
	fmci.txed=0;
	fmci.length=0;
	fmci.CRC_val=0;
	fmci.channel=&channel_tx;
	fmci.value=&value_tx;
	fmci.frame=&mod_tx;
	mod_conmmu=rt_thread_create("mymod",mod_conmmu_func,RT_NULL,1024,4,30);
}

#ifdef USE_RTT
/************************************************************************
*mod_conmmu_func	: RTT OS thread function
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is a thread of RTT OS, if use RTT OS, must define
*					 USE_RTT macro to enable this conditional compiling.
*
*History:		5/6/2020	BCL:　Created
========================================================================*/
void mod_conmmu_func(void* param)
{
	if(rt_sem_take(&mod_sem,RT_WAITING_FOREVER)!=RT_EOK)
	{
		rt_thread_yield();
	}
	frame_process(&modci);
	if(chk.rx_fail==1)
	{
		/*
		Reveiving fails, send retransmit request
		*/
		fmci.sub_cmd=RETX;
		
		/*re_tx func*/
		return;
	}
	switch (modci.main_cmd)
		{
			case SET:
				{
					
						break;
				}
			case READ:
			{
				
				break;
			}
			default:
				break;
		}

	rt_thread_delay(10);
}

#endif




/************************************************************************
*frame_process	: process data frame when receiving is done
*
*INPUT: private modebus control block struct pointer
*
*OUTPUT: none
*
*WARNINGS: This function is called when the hardware receiver irq function
*					 detects a frame tail "0x0a".
*					 This function only put data into channel buffer and value buffer,
*					 without any read operation.
*
*History:		5/6/2020	BCL:　Created
========================================================================*/
void frame_process(struct mod_cb* modcb)
{
	uint8_t tmp[3]={-1,-1};
	chk_init(&chk);
	rt_ringbuffer_getchar(&mod_rx,(uint16_t*)&tmp);
		/*Check if there exist frame head*/
		for(uint8_t i=0;i<2;i++)
		{
			rt_ringbuffer_getchar(&mod_rx,(uint16_t*)&tmp[i]);
		}
	if(tmp[0]==0xff&&tmp[1]==0xff)chk.frame_head=1;
	/*If there exists frame head, go head, else break out of the function*/
	else {chk.rx_fail=1;return;}
	/*find main cmd and sub-cmd*/
		if(chk.frame_head==1)
		{
			rt_ringbuffer_getchar(modcb->rx,(uint16_t*)&tmp[0]);
			switch(tmp[0])
			{
				case SET:	{modcb->main_cmd=SET;		chk.main_cmd=1;	break;}
				case READ:{modcb->main_cmd=READ;	chk.main_cmd=1;	break;}
				default:{return;}
			}
			rt_ringbuffer_getchar(modcb->rx,(uint16_t*)&tmp[1]);
			if(tmp[1]==TAR&&tmp[1]==THR&&tmp[1]==SEN&&tmp[1]==ADJ&&tmp[1]==CENA&&tmp[1]==SNN)
			{
				modcb->sub_cmd=tmp[1];
				chk.sub_cmd=1;
			}
			else {chk.rx_fail=1;return;}
			/*Find channel number and value*/
			modcb->nbrofchn=0;
				/*
					If main cmd is READ, 
					there is no corresponding value with channel number
					Exception: CENA, SNN, they have no channel end flag
				*/
				if(modcb->main_cmd==READ)
				{
					if(modcb->sub_cmd!=SNN&&modcb->sub_cmd!=CENA)					
					{
						do
						{
							rt_ringbuffer_getchar(modcb->rx,(uint16_t*)&tmp[0]);
							rt_ringbuffer_putchar(modcb->channel,tmp[0]);
							modcb->nbrofchn++;
						}while(tmp[0]!=CH_END);
						modcb->length-=1;
					}
				}
				/*
					If main cmd is SET, 
					there are two value after one channel number.
					Read 3 char for one group.
					Exception: CENA has no channel end flag
				*/
				if(modcb->main_cmd==SET)
				{
					
					if(modcb->sub_cmd==TAR&&modcb->sub_cmd==THR)
					{
						do
						{
							for(uint8_t i=0;i<3;i++)
							{
								rt_ringbuffer_getchar(modcb->rx,(uint16_t*)&tmp[i]);
								if(tmp[i]==CH_END){chk.chn_end=1;break;break;}
							}
							rt_ringbuffer_putchar(modcb->channel,tmp[0]);
							uint16_t val=value_cal(tmp[1],tmp[2]);
							rt_ringbuffer_putchar(modcb->value,val);
							modcb->nbrofchn++;
						}while(chk.chn_end==0);
					}
					/*
						Read CENA infomation, two value
					*/
					else if(modcb->sub_cmd==CENA)
					{
						for(uint8_t i=1;i<3;i++)
						{
							rt_ringbuffer_getchar(modcb->rx,(uint16_t*)&tmp[i]);
						}
						uint16_t val=value_cal(tmp[1],tmp[2]);
						/*Write CENA value to modbus control block*/
						modcb->chnena=val;
					}
				}
				/*
				Find CRC value
				*/
				for(uint8_t i=1;i<3;i++)
				{
					rt_ringbuffer_getchar(modcb->rx,(uint16_t*)&tmp[i]);
					/*If frame tail comes ahead of time*/
					if(tmp[i]==0x0d||tmp[i]==0x0a)
					{
						chk.rx_fail=1;
						return;
					}
				}
				modcb->CRC_val=value_cal(tmp[1],tmp[2]);
				for(uint8_t i=1;i<3;i++)
				{
					rt_ringbuffer_getchar(modcb->rx,(uint16_t*)&tmp[i]);
				}
				if(tmp[1]==0x0d&&tmp[2]==0x0a)
				{
					chk.frame_tail=1;
				}
		}
}

/************************************************************************
*chk_init	: initialize integrity check struct
*
*INPUT:		struct integrity_check pointer
*
*OUTPUT:	none
*
*WARNINGS:	This function must be called before each pass of frame
*						processing.
*
*History:		8/6/2020	BCL:　Created
========================================================================*/
void chk_init(struct integrity_check* chk)
{
	chk->frame_head	=0;
	chk->main_cmd		=0;
	chk->sub_cmd		=0;
	chk->chn_and_val=0;
	chk->chn_end		=0;
	chk->CRC_chk		=0;
	chk->frame_tail	=0;
}


/*Set used flag to mod control block*/
/************************************************************************
*set_used	: set modbus control block used bit to 1
*
*INPUT:		struct mod_cb* modcb	:	pointer to modbus control block
*
*OUTPUT:	none
*
*WARNINGS:	This function is called after set_func function.
*
*History:		5/6/2020	BCL:　Created
========================================================================*/
__STATIC_INLINE void set_used(struct mod_cb* modcb)
{
	modcb->use=1;
}


/*Set used flag to mod control block*/
/************************************************************************
*clr_used	: set modbus control block used bit to 0
*
*INPUT:		struct mod_cb* modcb	:	pointer to modbus control block
*
*OUTPUT:	none
*
*WARNINGS:	This function is called after set_func function.
*
*History:		9/6/2020	BCL:　Created
========================================================================*/
__STATIC_INLINE void clr_used(struct mod_cb* modcb)
{
	modcb->use=0;
}


/************************************************************************
*set_func	: set control parameter to muscles control block
*
*INPUT:		struct mod_cb* modcb	:	pointer to modbus control block
*					struct muscle_cb* mucb2	:	pointer to muscle control block
*
*OUTPUT:	none
*
*WARNINGS:	This function is called when SET cmd is detected.
*
*History:		5/6/2020	BCL:　Created
*						9/6/2020	BCL:	Modified
========================================================================*/
void set_func(struct mod_cb* modcb,struct muscle_cb* mucb2)
{
	/*
		If the cmd is used, just quit the function and do nothing.
	*/
	if(modcb->use==1)return;
	/*
		If cub cmd is not CENA and length is not 0, read channel nubmer and
		value from buffer.
	*/
	if(modcb->sub_cmd!=CENA)
	{
		if(modcb->length==0)return;
		uint16_t chn_tmp=0,val_tmp=0;
		for(uint8_t i=0;i<modcb->length;i++)
		{
			rt_ringbuffer_getchar(modcb->channel,&chn_tmp);
			rt_ringbuffer_getchar(modcb->value,&val_tmp);
			/*
				Give the value to muscles control block
			*/
			switch(modcb->sub_cmd)
			{
				case TAR:{mucb2->air_pre[chn_tmp]=val_tmp;break;}
				case THR:{mucb2->threshold[chn_tmp]=val_tmp;break;}
				default:{break;}
			}
		}
	}
	/*
		If the sub cmd is CENA, give the value to muscles control block directly.
	*/
	else if(modcb->sub_cmd==CENA){mucb2->use_of_muscle=modcb->chnena;}
	/*
		After set operation, set used flag to 1.
	*/
	set_used(modcb);
}

__STATIC_INLINE uint8_t assert_frame(uint8_t ch, uint8_t word)
{
	if(ch==word)
		return 1;
	else return 0;
}



/************************************************************************
*value_cal	: calculate 16-bit value from two 8-bit value
*
*INPUT:		uint8_t high --  high 8-bit value 
*					uint8_t low  --	 low 8-bit value
*
*OUTPUT:	uint16_t	tmp  --  16-bit value
*
*WARNINGS:	none
*
*History:		5/6/2020	BCL:　Created
*						8/6/2020	BCL:	Modified
========================================================================*/
__STATIC_INLINE uint16_t value_cal(uint8_t high, uint8_t low)
{
	uint16_t tmp=0;
	tmp=((uint16_t)high<<8)+(uint16_t)low;
	return tmp;
}


/************************************************************************
*value_trans	: generate high 8-bit and low 8-bit value from a 16-bit value
*
*INPUT:		uint16_t value --  value to be generated
*					uint8_t* p  	 --	 pointer to an array of length 2
*														 base addr: high 8-bit value
*														 +1 offset:	low 9-bit value
*
*OUTPUT:	none
*
*WARNINGS:	none
*
*History:		5/6/2020	BCL:　Created
========================================================================*/
__STATIC_INLINE void value_trans(uint16_t value, uint8_t* p)
{
	*p=(uint8_t)((value&0xff00)>>8);
	*(p+1)=(uint8_t)(value&0x00ff);
}


/************************************************************************
*data_tx	: transmit data from frame control block
*
*INPUT:		uint16_t value --  value to be generated
*					uint8_t* p  	 --	 pointer to an array of length 2
*														 base addr: high 8-bit value
*														 +1 offset:	low 9-bit value
*
*OUTPUT:	none
*
*WARNINGS:	none
*
*History:		5/6/2020	BCL:　Created
========================================================================*/
__STATIC_INLINE void data_tx(const struct frame_cb* frame)
{
	uint8_t tx_arr[frame->length];
	for(uint8_t i=0;i<frame->length;i++)
	{
		rt_ringbuffer_getchar(frame->frame,(uint16_t*)&tx_arr[i]);
	}
	hw_tx(tx_arr,frame->length);
}

/*Tx function related to hardware*/
/************************************************************************
*hw_tx	: hardware interface of data transmit
*
*INPUT:		uint8_t* buf   --  array pointer to data
*					uint16_t size  --	 array size
*
*OUTPUT:	none
*
*WARNINGS:	This function is used to transmit data through USART hardware
*						interface.
*						This function is declared with weak identfier, which means you
*						can declare another function with the same name anywhere, and 
*						with different hardware interface.
*
*History:		5/6/2020	BCL:　Created
========================================================================*/
void __attribute__((weak)) hw_tx(const uint8_t* buf,uint16_t size)
{
	uint16_t i;
	for(i=0;i<size;i++)
	{
		COM_PORT->DR = *(buf+i);
		while((COM_PORT->SR&0X40)==0);
	}
}

__STATIC_INLINE void CRC_Cal();

/*Return value to master board*/


__STATIC_INLINE void exchange_channel_list(struct mod_cb* modcb,struct frame_cb* frame)
{
	uint8_t tmp;
	for(uint8_t i = 0; i<modcb->nbrofchn;i++)
	{
		rt_ringbuffer_getchar(modcb->channel,(uint16_t*)&tmp);
	}
}

__STATIC_INLINE void return_preprocess(struct frame_cb* frame,struct muscle_cb* mucb2)
{
	struct mod_cb* modtmp;
	switch(frame->sub_cmd)
	{
		case TAR:
		case THR:
		{
		
		}
	
	}
}


/*format the return frame*/
__STATIC_INLINE void frame_format(struct frame_cb* frame,struct mod_cb* modcb,struct muscle_cb* mucb2)
{
	frame->length=0;
	if(frame->txed!=0)return;
	rt_ringbuffer_putchar(frame->frame,FRAME_HEAD);
	rt_ringbuffer_putchar(frame->frame,FRAME_HEAD);
	rt_ringbuffer_putchar(frame->frame,RE);
	uint8_t tmp[2];
	uint16_t chn_tmp,val_tmp;
	/*Sub command*/
	rt_ringbuffer_putchar(frame->frame,frame->sub_cmd);
	frame->length=4;
	/*Data write*/
	#ifdef STM32F103xE
	/*
		If it's slave board and just return channel and corresponding value
		Exception: CENA retx
	*/
	switch(frame->sub_cmd)
	{
		case TAR:
		case THR:
		case SEN:
		{
			for(uint8_t i=0;i<frame->nbrofchn;i++)
			{
				rt_ringbuffer_getchar(modcb->channel,&chn_tmp);
				rt_ringbuffer_putchar(frame->frame,chn_tmp);
				rt_ringbuffer_getchar(modcb->value,&val_tmp);
				value_trans(val_tmp,tmp);
				rt_ringbuffer_putchar(frame->frame,tmp[0]);
				rt_ringbuffer_putchar(frame->frame,tmp[1]);
				frame->length+=3;
			}
			break;
		}
		case CENA:
		{
			value_trans(mucb2->use_of_muscle,tmp);
			rt_ringbuffer_putchar(frame->frame,tmp[0]);
			rt_ringbuffer_putchar(frame->frame,tmp[1]);
			frame->length+=2;
		}
		case RETX:
		{
			
		}
	}

	#endif
	#ifdef STM32F767xx
	/*
		If it's master board, it has two modes: send channel number only 
		or channel with corresponding value.
		It depends on SET or READ cmd
	*/
	
	#endif
	rt_ringbuffer_putchar(frame->frame,CH_END);
	rt_ringbuffer_putchar(frame->frame,0x0d);
	rt_ringbuffer_putchar(frame->frame,0x0a);
	frame->length+=3;
}


	#ifndef RT_USING_FINSH
		#if COM_USED==1
void USART1_IRQHandler(void)
{
    int ch = -1;
    rt_base_t level;
    /* enter interrupt */
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断

    if ((__HAL_UART_GET_FLAG(&(UART1_Handler), UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&(UART1_Handler), UART_IT_RXNE) != RESET))
    {
        while (1)
        {
            ch = -1;
            if (__HAL_UART_GET_FLAG(&(UART1_Handler), UART_FLAG_RXNE) != RESET)
            {
							#ifdef STM32F767xx
                ch =  USART1->RDR & 0xff;
							#endif
							#ifdef STM32F103xE
								ch =  USART1->DR & 0xff;
							#endif
            }
            if (ch == -1)
            {
                break;
            }  
            /* 读取到数据，将数据存入 ringbuffer */
            rt_ringbuffer_putchar(&mod_rx, ch);
        }
				if(ch==0x0a)		//表示到达帧尾
				{					
					rt_sem_release(&mod_sem);
				}
    }

    /* leave interrupt */
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}
		#endif
		#if COM_USED==2
void USART2_IRQHandler(void)
{
    int ch = -1;
    rt_base_t level;
		#ifdef USE_RTT
    /* enter interrupt */
    rt_interrupt_enter();
		#endif

    if ((__HAL_UART_GET_FLAG(&(UART1_Handler), UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&(UART1_Handler), UART_IT_RXNE) != RESET))
    {
        while (1)
        {
            ch = -1;
            if (__HAL_UART_GET_FLAG(&(UART1_Handler), UART_FLAG_RXNE) != RESET)
            {
							#ifdef STM32F767xx
                ch =  USART2->RDR & 0xff;
							#endif
							#ifdef STM32F103xE
								ch =  USART2->DR & 0xff;
							#endif
            }
            if (ch == -1)
            {
                break;
            }  
            /* 读取到数据，将数据存入 ringbuffer */
            rt_ringbuffer_putchar(&mod_rx, ch);
        }
				#ifdef USE_RTT
				if(ch==0x0a)		//表示到达帧尾
				{					
					rt_sem_release(&mod_sem);
				}
				#endif
    }
		#ifdef USE_RTT
    /* leave interrupt */
    rt_interrupt_leave();
		#endif
}
		#endif
	#endif
#endif
#ifdef __cplusplus
}
#endif