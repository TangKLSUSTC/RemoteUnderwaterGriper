#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "my_ringbuf.h"
#include "rtthread.h"
#include <stdint.h>
#include "controller.h"

extern struct muscle_cb mucb;
#define FRAME_HEAD 0xff
/*+++++++++++++++++++++++++++++++++++++++++*/
						/*Function Number*/
#define SET 		0xa1						//д��
#define READ 		0xa2						//��ȡ
		//��ָ��
	#define TAR	0xb1						//��ѹĿ��ֵ��Ȩ�ޣ�R/W
	#define CHN	0xb2						//Ŀ��ͨ��
	#define THR 0xb3						//����ֵ
	#define SEN 0xb4						//������ֵ
	#define ADJ 0xb5
	#define CENA 0xb6
	#define SNN 0xc1
	//ͨ����
		#define CH0 0xe0
		#define CH1 0xe1
		#define CH2 0xe2
		#define CH3 0xe3
		#define CH4 0xe4
		#define CH5 0xe5
		#define CH6 0xe6
		#define CH7 0xe7
		#define CH8 0xe8
		#define CH9 0xe9
		#define CH10 0xea
		#define CH11 0xeb
		#define CH_ALL 0xec
		#define CH_END 0xed
	//ͨ��״̬	
		#define CH_ENA `0x1a
		#define CH_DIS 0x1b
		
/*+++++++++++++++++++++++++++++++++++++++++*/
									/*����ֵ*/
	#define SYS_ERROR 0xee					//ϵͳ����
	#define SET_OK 		0xaa					//���óɹ�
	
	#define RE				0xa3					//����֡��ָ��ͷ
	#define RETX			0xa4					//re-transmission
//	#define 
	
	#define COM_PORT	USART2
	#define COM_USED	2
struct mod_cb
{
	uint8_t 	main_cmd;							//��ָ��
	uint8_t 	sub_cmd;							//��ָ��
	uint8_t 	option;								//ѡ��
	uint8_t 	length;								//���鳤��
	struct rt_ringbuffer* channel;		//ͨ���������buffer
	struct rt_ringbuffer* value;			//ͨ����Ӧ��ֵbuffer
	struct rt_ringbuffer* rx;					//����֡����
//	uint16_t* 	channel;						//ͨ���������
//	uint16_t* 	value;							
	uint16_t	chnena;								//ͨ��ʹ��λ
	uint8_t   nbrofchn :7;							//ͨ������
	uint8_t 	use	:1;									//�Ƿ�ʹ�ù�, 0��δʹ��
	uint32_t 	CRC_val;							//CRCУ��ֵ
};

struct frame_cb
{
	uint32_t 	CRC_val;						//CRCУ��ֵ
	struct rt_ringbuffer* channel;		//ͨ���������buffer
	struct rt_ringbuffer* value;			//ͨ����Ӧ��ֵbuffer
	struct rt_ringbuffer* frame;			//����֡����
//	uint8_t* channel;							//ͨ������
//	uint8_t* value;								//ֵ
	uint8_t length;								//֡����
	uint8_t sub_cmd;							//��ָ��
	uint8_t nbrofchn:7;							//ͨ������
	uint8_t txed:1;									//�Ѵ����־λ��0��δ����
};

struct integrity_check
{
	char frame_head		:1;
	char main_cmd			:1;
	char sub_cmd			:1;
	char chn_and_val	:1;
	char chn_end			:1;
	char CRC_chk			:1;
	char frame_tail		:1;
	char rx_fail			:1;
}int_chk;


#ifdef USE_RTT
	void mod_conmmu_func(void* param);
#endif
void frame_process(struct mod_cb* modcb);
static uint16_t value_cal(uint8_t high, uint8_t low);
static void value_trans(uint16_t value, uint8_t* p);
void chk_init(struct integrity_check* chk);

static void set_used(struct mod_cb* modcb);
static void clr_used(struct mod_cb* modcb);
void set_func(struct mod_cb* modcb,struct muscle_cb* mucb2);

static uint8_t assert_frame(uint8_t ch, uint8_t word);
static void data_tx(const struct frame_cb* frame);
void hw_tx(const uint8_t* buf,uint16_t size);

static void exchange_channel_list(struct mod_cb* modcb,struct frame_cb* frame);
static void return_preprocess(struct frame_cb* frame,struct muscle_cb* mucb2);
static void frame_format(struct frame_cb* frame,struct mod_cb* modcb,struct muscle_cb* mucb2);
#ifdef __cplusplus
}
#endif

