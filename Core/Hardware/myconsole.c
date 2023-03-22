#include "myconsole.h"
#ifdef RT_USING_FINSH
#define UART_RX_BUF_LEN 100
rt_uint8_t uart_rx_buf[UART_RX_BUF_LEN] = {0};
struct rt_ringbuffer  uart_rxcb;         /* 定义一个 ringbuffer cb */
static struct rt_semaphore shell_rx_sem; /* 定义一个静态信号量 */


UART_HandleTypeDef UART1_Handler;
#define RXBUFFERSIZE   1
uint8_t aRxBuffer[RXBUFFERSIZE];


void my_console_init(void)
{
	rt_ringbuffer_init(&uart_rxcb, uart_rx_buf, UART_RX_BUF_LEN);
	rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);
	MX_USART1_UART_Init();
//	__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
	HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer, RXBUFFERSIZE);

}
INIT_BOARD_EXPORT(my_console_init);

void rt_hw_console_output(const char *str)
{
	rt_size_t i=0,size=0;
	__HAL_UNLOCK(&huart1);
	size=rt_strlen(str);
	for(i=0;i<size;i++)
	{
		#ifdef STM32F767xx
		while((USART1->ISR&0X40)==0);
		#endif
		#if defined(STM32F103xE)||defined(STM32F103xB)
		while((USART1->SR&0x40)==0);
		#endif
		if(*(str+i)=='\n')
		{
			#ifdef STM32F767xx
			USART1->TDR='\r';
			while((USART1->ISR&0X40)==0);
			#endif
			#if defined(STM32F103xE)||defined(STM32F103xB)
			USART1->DR='\r';
			while((USART1->SR&0X40)==0);
			#endif
		}
		#ifdef STM32F767xx
		USART1->TDR=*(str+i);
		#endif
		#if defined(STM32F103xE)||defined(STM32F103xB)
		USART1->DR=*(str+i);
		#endif
	}
}
/* 移植 FinSH，实现命令行交互, 需要添加 FinSH 源码，然后再对接 rt_hw_console_getchar */
/* 中断方式 */
char rt_hw_console_getchar(void)
{
    char ch = 0;

    /* 从 ringbuffer 中拿出数据 */
    while (rt_ringbuffer_getchar(&uart_rxcb, &ch) != 1)
    {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    } 
    return ch;   
}


void usart_init(uint32_t bound)
{	
	//UART 初始化设置

	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //波特率
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)aRxBuffer, RXBUFFERSIZE);//该函数会开启接收中断：标志位UART_IT_RXNE，并且设置接收缓冲以及接收缓冲接收最大数据量
  
}







void USART1_IRQHandler(void)
{
    int ch = -1;
    rt_base_t level;
    /* enter interrupt */
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断

    if ((__HAL_UART_GET_FLAG(&(huart1), UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&(huart1), UART_IT_RXNE) != RESET))
    {
        while (1)
        {
            ch = -1;
            if (__HAL_UART_GET_FLAG(&(huart1), UART_FLAG_RXNE) != RESET)
            {
							#ifdef STM32F767xx
                ch =  USART1->RDR & 0xff;
							#endif
							#if defined(STM32F103xE)||defined(STM32F103xB)
								ch =  USART1->DR & 0xff;
							#endif
            }
            if (ch == -1)
            {
                break;
            }  
            /* 读取到数据，将数据存入 ringbuffer */
            rt_ringbuffer_putchar(&uart_rxcb, ch);
        }        
        rt_sem_release(&shell_rx_sem);
    }

    /* leave interrupt */
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}
#endif