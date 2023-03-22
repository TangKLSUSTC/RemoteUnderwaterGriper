
#ifdef __cplusplus
extern "C" {
#endif
#include "controller.h"
#include "sys.h"
#include <main.h>
#include <stdlib.h>
//#include "IO_def.h"

#define ctrl_Priority 5
#define ctrl_Stack_Size 3000
#define ctrl_tick 30


/*
	control_flag:
	bit:	 	func:		
			0					pump_in
			1					pump_out
			
			2					muscle_in 0
			3					muscle_in 1
			4					muscle_in 2
			5					muscle_in 3
			
			6					muscle_out 0
			7					muscle_out 1
			8					muscle_out 2
			9					muscle_out 3
			
			10				griper close
			11				griper open
			
			his_sta:		
	bit:		func:
			0					arm flex long
			1					arm flex short
			2					griper open
			3					griper close
*/
#if defined(WG_SLAVE)
#define CLEAR_FLAG		{wg.control_flag=0;pump_flag=0;}
#endif

#if defined(WG_MASTER)
	#define ROCK_X 			 &adc_val[0]
	#define ROCK_Y 			 &adc_val[1]
	#define PRESS_CHN0	 &adc_val[2]
	#define PRESS_CHN1	 &adc_val[3]
	#define GRIPER_OPEN_SW	(uint8_t)io_scan(&griper_switch[1],read_bit(&his_sta,2))
	#define GRIPER_CLOSE_SW	(uint8_t)io_scan(&griper_switch[0],read_bit(&his_sta,3))
	#define ARM_LONG_SW			(uint8_t)io_scan(&arm_flex_pin[0],read_bit(&his_sta,0))
	#define ARM_SHORT_SW		(uint8_t)io_scan(&arm_flex_pin[1],read_bit(&his_sta,1))
	

	const uint32_t rock_x_mid_val = 1600;
	const uint32_t rock_x_upper_threshold = 3000;
	const uint32_t rock_x_lower_threshold = 200;

	const uint32_t rock_y_mid_val = 1600;
	const uint32_t rock_y_upper_threshold = 3000;
	const uint32_t rock_y_lower_threshold = 200;

	const uint32_t press_chn0_threshold = 2500;
	const uint32_t press_chn1_threshold = 2500;

	uint32_t adc_buf[4]={0,0,0,0};
	uint32_t adc_cal[4]={0,0,0,0};
	uint32_t adc_val[4]={0,0,0,0};
	uint8_t adc_cnt=0;
	uint8_t tmp1=0; 
	uint32_t his_sta=0;
#endif
#if defined(WG_SLAVE)
	uint8_t pump_flag=0;
#endif

static wg_t wg_tmp;

//rt_sem_t io_event = RT_NULL;

/*prototype of entry functions*/
void watcher_main(void* param);
void controller_main(void* param);
#if defined(WG_SLAVE)
	void watch_dog_main(void* param);
#endif
	/*Main control thread block*/
static rt_thread_t ctrl_tcb = RT_NULL;
	/*Watcher thread block*/
static rt_thread_t sensor_watcher = RT_NULL;

	/*Soft timer to protect muscles*/
static rt_timer_t watch_dog=RT_NULL;
rt_sem_t adc_sig=RT_NULL;


/************************************************************************
*control_thread_init	: controller thread initialization
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called in main thread after system start up.
*
========================================================================*/
void control_thread_init()
{
	ctrl_tcb=rt_thread_create("controller",controller_main,RT_NULL,ctrl_Stack_Size,ctrl_Priority,ctrl_tick);
//	sensor_watcher=rt_thread_create("watcher",watcher_main,RT_NULL,256,4,2);
	if(ctrl_tcb!=RT_NULL)rt_thread_startup(ctrl_tcb);
	//if(sensor_watcher!=RT_NULL)rt_thread_startup(sensor_watcher);
	#if defined(WG_MASTER)
	adc_sig=rt_sem_create("adc_syn",1,RT_IPC_FLAG_FIFO);
	#endif
	#if defined(WG_SLAVE)
	watch_dog=rt_timer_create("watchdog",watch_dog_main,RT_NULL,100,RT_TIMER_FLAG_SOFT_TIMER);
	rt_timer_control(watch_dog,RT_TIMER_CTRL_SET_ONESHOT,RT_NULL);
	/*test part*/
//	if(watch_dog!=RT_NULL)
//	{
//		rt_timer_start(watch_dog);
//	}
	#endif
}


void controller_init()
{
#if defined(WG_SLAVE)
	io_group_init(&mi);
	io_group_init(&mo);
	io_group_init(&pp);
	io_init(&griper_open);
	io_init(&griper_close);
#endif
	
#if defined(WG_MASTER)
	io_group_init(&gswitch);
	io_group_init(&arm_flex);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	TIM2->CCR2=500;
	HAL_ADC_Start_DMA(&hadc1,adc_buf,4);
	__HAL_DMA_ENABLE_IT(&hdma_adc1,DMA_IT_TC);
	__HAL_DMA_DISABLE_IT(&hdma_adc1,DMA_IT_HT);
	__HAL_DMA_DISABLE_IT(&hdma_adc1,DMA_IT_TE);
#endif
}
INIT_BOARD_EXPORT(controller_init);

/************************************************************************
*controller_main	: controller thread entry function
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: DO NOT return in while(1) infinite loop!
*
========================================================================*/
void controller_main(void* param)
{
//	wg_tmp=wg;
	#if defined(WG_MASTER)
		uint8_t valid_cmd=0;
		wg.cmd_ptr[0]=wg.cmd_ptr[1]=frame_head;
		wg.cmd_len=2;
	#endif
	rt_kprintf("controller running\n");
	//**************
	#if defined(WG_MASTER)
	//rt_sem_release(req_sync);
	#endif
	while(1)
	{
	#if defined(WG_SLAVE)
		rt_sem_take(req_sync,RT_WAITING_FOREVER);
		//rt_mq_recv(data_req_mq,(void*)&wg_tmp,sizeof(wg_tmp),RT_WAITING_FOREVER);
		rt_timer_stop(watch_dog);
		
		/*read griper control bits*/
		switch(wg.control_flag&0x0003)
		{
			case 1:
			{
				wg.grp_open();
				write_bit(U32P&pump_flag,1,1);
				break;
			}
			case 2:
			{
				wg.grp_close();
				write_bit(U32P&pump_flag,0,1);
				break;
			}
			case 0:
			case 3:
			default:
			{
				wg.grp_keep();
			}
		}
		
		/*read arm long and short control bits*/
		switch((wg.control_flag>>2)&0x0003)
		{
			case 1:
			{
				wg.arm_long();
				write_bit(U32P&pump_flag,0,1);
				break;
			}
			case 2:
			{
				wg.arm_short();
				write_bit(U32P&pump_flag,1,1);
				break;
			}
			case 3:
			{
				wg.arm_long();
				write_bit(U32P&pump_flag,0,1);
				wg.arm_short();
				write_bit(U32P&pump_flag,1,1);
				break;
			}
			case 0:
			
			default:
			{
				//wg.arm_keep();
				break;
			}
		}
		
		/*read arm left and right control bits*/
		switch((wg.control_flag>>4)&0x0003)
		{
			case 1:
			{
				wg.arm_left();
				pump_flag=3;
				break;
			}
			case 2:
			{
				wg.arm_right();
				pump_flag=3;
				break;
			}
			case 0:
			case 3:
			default:
			{
				//wg.arm_keep();
				break;
			}
		}
		
		/*read arm forward and backward control bits*/
		switch((wg.control_flag>>6)&0x0003)
		{
			case 1:
			{
				wg.arm_forward();
				pump_flag=3;
				break;
			}
			case 2:
			{
				wg.arm_backward();
				pump_flag=3;
				break;
			}
			case 0:
			case 3:
			default:
			{
				//wg.arm_keep();
				break;
			}
		}
		
		/*read eject air control bit*/
		if((wg.control_flag>>8)&0x0001)
		{
			MUSCLE_EJECT_AIR();
			pump_flag=3;
		}
		
		switch(pump_flag)
		{
			case 0:
			{
				PUMP_IN_OFF;
				PUMP_OUT_OFF;
				break;
			}
			case 1:
			{
				PUMP_IN_ON;
				PUMP_OUT_OFF;
				break;
			}
			case 2:
			{
				PUMP_IN_OFF;
				PUMP_OUT_ON;
				break;
			}
			case 3:
			{
				PUMP_IN_ON;
				PUMP_OUT_ON;
				break;
			}
			default:
			{
				PUMP_IN_OFF;
				PUMP_OUT_OFF;
				break;
			}
		}
		CLEAR_FLAG;
		rt_timer_start(watch_dog);
	#endif
		
	#if defined(WG_MASTER)
	
	
		/*
			Scan griper or arm switch and do some operation
		*/
		//**************
		//rt_sem_take(req_sync,RT_WAITING_NO);
		/*arm long switch*/
		switch(ARM_LONG_SW)
		{
			case io_rising_edge:
			{
				wg.cmd_ptr[wg.cmd_len]=ARM_LONG;
				wg.cmd_len++;
				valid_cmd=1;
				write_bit(&his_sta,0,1);
				break;
			}
			case io_falling_edge:
			{
				write_bit(&his_sta,0,0);
				break;
			}
			case io_nochange:
			{
				if(read_bit(&his_sta,0))
				{
					wg.cmd_ptr[wg.cmd_len]=ARM_LONG;
					wg.cmd_len++;
					valid_cmd=1;
				}
				break;
			}
		}
		/*arm short switch*/
		switch(ARM_SHORT_SW)
		{
			case io_rising_edge:
			{
				wg.cmd_ptr[wg.cmd_len]=ARM_SHORT;
				wg.cmd_len++;
				valid_cmd=1;
				write_bit(&his_sta,1,1);
				break;
			}
			case io_falling_edge:
			{
				write_bit(&his_sta,1,0);
				break;
			}
			case io_nochange:
			{
				if(read_bit(&his_sta,1))
				{
					wg.cmd_ptr[wg.cmd_len]=ARM_SHORT;
					wg.cmd_len++;
					valid_cmd=1;
				}
				break;
			}
		}
		/*griper open switch*/
		switch(GRIPER_OPEN_SW)
		{
			case io_rising_edge:
			{
				wg.cmd_ptr[wg.cmd_len]=GRP_OPEN;
				wg.cmd_len++;
				valid_cmd=1;
				write_bit(&his_sta,2,1);
				break;
			}
			case io_falling_edge:
			{
				write_bit(&his_sta,2,0);
				break;
			}
			case io_nochange:
			{
				if(read_bit(&his_sta,2))
				{
					wg.cmd_ptr[wg.cmd_len]=GRP_OPEN;
					wg.cmd_len++;
					valid_cmd=1;
				}
				break;
			}
		}
		
		/*griper close switch*/
		switch(GRIPER_CLOSE_SW)
		{
			case io_rising_edge:
			{
				wg.cmd_ptr[wg.cmd_len]=GRP_CLOSE;
				wg.cmd_len++;
				valid_cmd=1;
				write_bit(&his_sta,3,1);
				break;
			}
			case io_falling_edge:
			{
				write_bit(&his_sta,3,0);
				break;
			}
			case io_nochange:
			{
				if(read_bit(&his_sta,3))
				{
					wg.cmd_ptr[wg.cmd_len]=GRP_CLOSE;
					wg.cmd_len++;
					valid_cmd=1;
				}
				break;
			}
		}
		/*use adc value*/
		if(rt_sem_trytake(adc_sig)==-RT_ETIMEOUT)
		{
			if(valid_cmd==1)
			{
				wg.cmd_ptr[wg.cmd_len]=frame_tail0;
				wg.cmd_len++;
				wg.cmd_ptr[wg.cmd_len]=frame_tail1;
				wg.cmd_len++;
				if(rt_sem_take(conchk_req_sem,RT_WAITING_NO)==RT_EOK)
				{
					cmd_tx(wg.cmd_ptr,wg.cmd_len);
				}
				else
				{
					rt_thread_mdelay(5);
					cmd_tx(wg.cmd_ptr,wg.cmd_len);
				}
				
				//***************
				rt_sem_release(req_sync);
				wg.cmd_len=2;
				valid_cmd=0;
			}
			rt_thread_yield();
		}
		else		/*if semphore is valid*/
		{
			if(*ROCK_Y>rock_y_upper_threshold)	//Y+ U+
			{
				wg.cmd_ptr[wg.cmd_len]=ARM_LEFT;
				wg.cmd_len++;
				valid_cmd=1;
			}
			else if(*ROCK_Y<rock_y_lower_threshold)		//Y- U-
			{
				wg.cmd_ptr[wg.cmd_len]=ARM_RIGHT;
				wg.cmd_len++;
				valid_cmd=1;
			}
			else if(*ROCK_Y>=rock_y_lower_threshold&&*ROCK_Y<=rock_y_upper_threshold)
			{
				
			}
			if(*ROCK_X>rock_x_upper_threshold)		//X+ V+
			{
				wg.cmd_ptr[wg.cmd_len]=ARM_BACKWARD;
				wg.cmd_len++;
				valid_cmd=1;
			}
			else if(*ROCK_X<rock_x_lower_threshold)		//X- V-
			{
				wg.cmd_ptr[wg.cmd_len]=ARM_FORWARD;
				wg.cmd_len++;
				valid_cmd=1;
			}
			else if(*ROCK_X>=rock_x_lower_threshold&&*ROCK_X<=rock_x_upper_threshold)
			{

			}
			if(valid_cmd==1)
			{
				wg.cmd_ptr[wg.cmd_len]=frame_tail0;
				wg.cmd_len++;
				wg.cmd_ptr[wg.cmd_len]=frame_tail1;
				wg.cmd_len++;
				if(rt_sem_take(conchk_req_sem,RT_WAITING_NO)==RT_EOK)
				{
					cmd_tx(wg.cmd_ptr,wg.cmd_len);
				}
				else
				{
					rt_thread_mdelay(5);
					cmd_tx(wg.cmd_ptr,wg.cmd_len);
				}
				//*******************
				rt_sem_release(req_sync);
				wg.cmd_len=2;
				valid_cmd=0;
			}
		}
	#endif
	#if defined(WG_MASTER)	
		rt_thread_mdelay(25);
	#endif
	#if defined(WG_SLAVE)
		rt_thread_mdelay(20);
	#endif
	}
}


#if defined(WG_SLAVE)
/************************************************************************
*watch_dog_main	: soft watch dog thread entry function
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: DO NOT return in while(1) infinite loop!
*
========================================================================*/

/*
TODO: 整理线程间通信与指针传递
*/
void watch_dog_main(void* param)
{
	CLEAR_FLAG;
	PUMP_IN_OFF;
	PUMP_OUT_OFF;
	wg.arm_keep();
	wg.grp_keep();
//	wg_tmp.arm_keep();
//	wg_tmp.grp_keep();
	rt_kprintf("command timeout\n");
}
#endif


void watcher_main(void* param)
{
	while(1)
	{
		
	}
}

/*		BSP function api		*/
void __attribute__((weak)) soft_reset(void)
{
	#if defined(RT_USING_FINSH)
	rt_kprintf("BSP soft reset is undefined\n");
	#else
	return;
	#endif
}

/*-------------------------------------------------------------------*/
#if defined(WG_SLAVE)
const char NAME[]="Liquid Driven Muscles Slave Controller";
#endif

#if defined(WG_MASTER)
const char NAME[]="Liquid Driven Muscles Master Controller";
#endif
const char main_version=1;
const char sub_version=00;
const char branch[]="Debug";
void rt_show_version(void)
{
	#if defined(RT_USING_FINSH)
	rt_kprintf("%s\n",NAME);
	rt_kprintf("Version: %s %d.%d\n",branch,main_version,sub_version);
	rt_kprintf("Build: %s\n",__DATE__);
	#else
	return;
	#endif
}
#if defined(WG_MASTER)
void DMA1_Channel1_IRQHandler(void)
{
	rt_interrupt_enter();
	if(__HAL_DMA_GET_FLAG(&hdma_adc1,DMA_FLAG_TC1)!=RESET)
	__HAL_DMA_CLEAR_FLAG(&hdma_adc1,DMA_FLAG_TC1);
	/*
		Count number lower than 7
	*/
	if(adc_cnt<7)
	{
		for(uint8_t i=0;i<4;i++)
		{
			adc_cal[i]+=adc_buf[i];
		}
		adc_cnt++;
	}
	/*Count number equals to 7*/
	else if(adc_cnt==7)
	{
		for(uint8_t i=0;i<4;i++)
		{
			adc_val[i]=adc_cal[i]>>3;
			adc_cal[i]=0;
		}
		adc_cnt=0;
		rt_sem_release(adc_sig);
	}
	rt_interrupt_leave();
}
#endif
#ifdef __cplusplus
}
#endif

