#include "data_req.h"

/*
	Data receive basic values
*/
#define DATA_FRAME_BUF_LEN 100
#define RXBUFFERSIZE   1
	/*data frame buffer array*/
rt_uint8_t data_frame_buf[DATA_FRAME_BUF_LEN]={0};
	/*receive ringbuffer control block*/
struct rt_ringbuffer  data_rxcb;
	/*data receive semphore block*/
static rt_sem_t data_rx_sem;
	/*connection check soft timer*/
static rt_timer_t conchk=RT_NULL;
	/*connection check timeout timer*/
static rt_timer_t conchk_timeout=RT_NULL;
	/*inter thread synchronize semphore*/
rt_sem_t req_sync;
	/*semphore of start timeout timer and send connection request*/
rt_sem_t conchk_req_sem=RT_NULL;
static uint8_t aRxBuffer1[RXBUFFERSIZE];



/*thread basic variables*/
static rt_thread_t data_requester=RT_NULL;
rt_mq_t data_req_mq=RT_NULL;
rt_sem_t lose_connection=RT_NULL;
//data_req_t data_cb;

/*
	To the sensor or devices
*/
wg_t wg;

void data_requester_main(void* param);
uint16_t dreq_hw_getdata(void);
static void connection_chk_timer(void* param);
extern void connection_chk_timeout(void* param);

void data_requester_init()
{
	/*initialize basic RTOS components of the thread*/
	
	/*initialize rx ring buffer*/
	rt_ringbuffer_init(&data_rxcb,data_frame_buf,DATA_FRAME_BUF_LEN);
	
	/*create data receive semphore*/
	data_rx_sem=rt_sem_create("data_rx",1,RT_IPC_FLAG_FIFO);
	
	/*create inter thread synchronization semphore*/
	req_sync=rt_sem_create("req_sync",1,RT_IPC_FLAG_FIFO);
	
	/*create lose connection semphore*/
	
	//lose_connection=rt_sem_create("lose",1,RT_IPC_FLAG_FIFO);
	
	/*create connection check triger semphore*/
	conchk_req_sem=rt_sem_create("conchk_req",1,RT_IPC_FLAG_FIFO);
	
	/*create message queue to send data to other threads*/
	data_req_mq=rt_mq_create("data_req",sizeof(wg),10,RT_IPC_FLAG_FIFO);
	
	/*create data requester thread*/
	data_requester=rt_thread_create("data_req",data_requester_main,RT_NULL,1024,3,20);
	
	/*create connection check timmer, this timmer is periodically trigered*/
	//conchk=rt_timer_create("conchk",connection_chk_timer,(void*)&wg,2000,RT_TIMER_CTRL_SET_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
	
	/*create timeout timer to shutdown all control bit, this timer is trigered only once*/
	//conchk_timeout=rt_timer_create("conchk_timeout",connection_chk_timeout,(void*)&wg,200,RT_TIMER_CTRL_SET_ONESHOT|RT_TIMER_FLAG_SOFT_TIMER);
	
	/*startup data requester thread*/
	if(data_requester!=RT_NULL)rt_thread_startup(data_requester);
	
	/*start USART receive through interrupt*/
	HAL_UART_Receive_IT(&huart3,(uint8_t*)aRxBuffer1,RXBUFFERSIZE);
	
	/*Initialize device interface*/
	wg_init(&wg);
}

void data_requester_main(void* param)
{
//	rt_err_t uwet;
//	if(conchk!=RT_NULL)
//	{
//		rt_timer_start(conchk);
//	}
	rt_kprintf("data req start\n");
	while(1)
	{
	#if defined(WG_MASTER)
		rt_sem_take(req_sync,RT_WAITING_FOREVER);
		
	#endif
		
		
		/*data requester is the master thread in slave device*/
		rt_sem_take(data_rx_sem,RT_WAITING_FOREVER);
		//wg_frame_process(&wg,&data_rxcb);
		#if defined(WG_SLAVE)
		/*If receive right command*/
		if(read_bit((uint32_t*)&wg.wg_state,5))
		{
			wg_cmd_generate(&wg,CMDACK);
			cmd_tx(wg.cmd_ptr,wg.cmd_len);
			rt_sem_release(req_sync);
			//rt_mq_send(data_req_mq,&wg,sizeof(wg));
			write_bit(U32P&wg.wg_state,5,0);
		}
		else
		{
			wg.loss_cnt++;
		}
		/*If receive a connection request (aka handshake)*/
		if(read_bit(U32P&wg.control_flag,11))
		{
			wg_cmd_generate(&wg,CONNECT_ACK);
			cmd_tx(wg.cmd_ptr,wg.cmd_len);
			write_bit(U32P&wg.control_flag,11,0);
		}
		#endif
	#if defined(WG_MASTER)
		
		/*
		TODO:
			完善通信协议的异常处理部分
		*/
		
		/*If receive CMD_RX confirm*/
		if(read_bit(U32P&wg.control_flag,9))
		{
			//**********
			//rt_sem_release(req_sync);
			rt_sem_release(conchk_req_sem);
		}
	
	#endif
		
		
		rt_thread_mdelay(5);
	}
}



__WEAK void cmd_tx(uint8_t* str, uint8_t len)
{
	rt_size_t i = 0;
	uint8_t *ptr=str;
	//__HAL_UNLOCK(&huart1);
	for(i=0;i<len;i++)
	{
		while((DATAPORT->SR&0x40)==0);
		DATAPORT->DR=*(ptr+i);
	}
}

/************************************************************************
*connection_chk_timer	: entry function of connection check timer
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called periodically by connection check timer.
*					 Quick in quick out.
*
========================================================================*/
static void connection_chk_timer(void* param)
{
//	wg_t* wg=(wg_t*)param;
//	/*clear loss flag*/
//	wg->loss_flag=0;
//	/*generate connection request frame*/
//	wg_frame_generate(wg,CONNECT_REQ);
//	/*release cnnection chenk semphore*/
//	rt_sem_release(conchk_req_sem);
}




/*
	data access function.
	For semaphore reslease in each pass of USART IRQ
*/
uint16_t dreq_hw_getdata(void)
{
	uint8_t data=0;
	while(rt_ringbuffer_getchar(&data_rxcb,&data)!=1)
	{
		rt_sem_take(data_rx_sem,RT_WAITING_FOREVER);
	}
	return data;
}


#if !defined(USE_NORMALIZED_DATA)
/************************************************************************
*normalized_data	: calculate thruster control value from normalized
*											3DX sensor data
*
*INPUT: raw_data	  :	raw sensor data array
*				dest_array	:	destination array
*				max_range		:	maximum range of sensor data
*				data_len		:	data array length
*
*OUTPUT: none
*
*WARNINGS: This function is called after get raw sensor data.
*
========================================================================*/
void normalized_data(const int16_t* raw_data,float* dest_array,uint16_t max_range,uint8_t data_len)
{
	for(uint8_t i=0;i<data_len;i++)
	{
		dest_array[i]=raw_data[i]/max_range;
	}
}
#endif


void USART3_IRQHandler(void)
{
    int ch = -1;
    rt_base_t level;
    /* enter interrupt */
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断

    if ((__HAL_UART_GET_FLAG(&(huart3), UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&(huart3), UART_IT_RXNE) != RESET))
    {
        while (1)
        {
            ch = -1;
            if (__HAL_UART_GET_FLAG(&(huart3), UART_FLAG_RXNE) != RESET)
            {
							#ifdef STM32F767xx
                ch =  DATAPORT->RDR & 0xff;
							#endif
							#if defined(STM32F103xE)||defined(STM32F103xB)
								ch =  DATAPORT->DR & 0xff;
							#endif
            }
            if (ch == -1)
            {
                break;
            }  
            /* 读取到数据，将数据存入 ringbuffer */
            rt_ringbuffer_putchar(&data_rxcb, ch);
						//rt_kprintf("0x%x\n",ch);
						/*接收到断帧符，释放信号量*/
						if(ch==0x0a)
						{
							rt_sem_release(data_rx_sem);
						}
        }        
        //rt_sem_release(data_rx_sem);
    }

    /* leave interrupt */
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}


