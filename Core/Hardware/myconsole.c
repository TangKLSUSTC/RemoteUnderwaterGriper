#include "myconsole.h"
#ifdef RT_USING_FINSH
#define UART_RX_BUF_LEN 100
rt_uint8_t uart_rx_buf[UART_RX_BUF_LEN] = {0};
struct rt_ringbuffer  uart_rxcb;         /* ����һ�� ringbuffer cb */
static struct rt_semaphore shell_rx_sem; /* ����һ����̬�ź��� */


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
/* ��ֲ FinSH��ʵ�������н���, ��Ҫ��� FinSH Դ�룬Ȼ���ٶԽ� rt_hw_console_getchar */
/* �жϷ�ʽ */
char rt_hw_console_getchar(void)
{
    char ch = 0;

    /* �� ringbuffer ���ó����� */
    while (rt_ringbuffer_getchar(&uart_rxcb, &ch) != 1)
    {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    } 
    return ch;   
}


void usart_init(uint32_t bound)
{	
	//UART ��ʼ������

	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //������
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
	
	HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)aRxBuffer, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
  
}







void USART1_IRQHandler(void)
{
    int ch = -1;
    rt_base_t level;
    /* enter interrupt */
    rt_interrupt_enter();          //���ж���һ��Ҫ������Ժ����������ж�

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
            /* ��ȡ�����ݣ������ݴ��� ringbuffer */
            rt_ringbuffer_putchar(&uart_rxcb, ch);
        }        
        rt_sem_release(&shell_rx_sem);
    }

    /* leave interrupt */
    rt_interrupt_leave();    //���ж���һ��Ҫ������Ժ������뿪�ж�
}
#endif