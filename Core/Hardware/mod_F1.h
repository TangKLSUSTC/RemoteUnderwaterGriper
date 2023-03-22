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
#define SET 		0xa1						//写入
#define READ 		0xa2						//读取
		//子指令
	#define TAR	0xb1						//气压目标值，权限：R/W
	#define CHN	0xb2						//目标通道
	#define THR 0xb3						//门限值
	#define SEN 0xb4						//传感器值
	#define ADJ 0xb5
	#define CENA 0xb6
	#define SNN 0xc1
	//通道数
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
	//通道状态	
		#define CH_ENA `0x1a
		#define CH_DIS 0x1b
		
/*+++++++++++++++++++++++++++++++++++++++++*/
									/*返回值*/
	#define SYS_ERROR 0xee					//系统错误
	#define SET_OK 		0xaa					//设置成功
	
	#define RE				0xa3					//返回帧主指令头
	#define RETX			0xa4					//re-transmission
//	#define 
	
	#define COM_PORT	USART2
	#define COM_USED	2
struct mod_cb
{
	uint8_t 	main_cmd;							//主指令
	uint8_t 	sub_cmd;							//子指令
	uint8_t 	option;								//选项
	uint8_t 	length;								//数组长度
	struct rt_ringbuffer* channel;		//通道编号数组buffer
	struct rt_ringbuffer* value;			//通道对应数值buffer
	struct rt_ringbuffer* rx;					//接收帧缓存
//	uint16_t* 	channel;						//通道编号数组
//	uint16_t* 	value;							
	uint16_t	chnena;								//通道使能位
	uint8_t   nbrofchn :7;							//通道个数
	uint8_t 	use	:1;									//是否使用过, 0：未使用
	uint32_t 	CRC_val;							//CRC校验值
};

struct frame_cb
{
	uint32_t 	CRC_val;						//CRC校验值
	struct rt_ringbuffer* channel;		//通道编号数组buffer
	struct rt_ringbuffer* value;			//通道对应数值buffer
	struct rt_ringbuffer* frame;			//发送帧缓存
//	uint8_t* channel;							//通道数组
//	uint8_t* value;								//值
	uint8_t length;								//帧长度
	uint8_t sub_cmd;							//子指令
	uint8_t nbrofchn:7;							//通道个数
	uint8_t txed:1;									//已传输标志位，0：未传输
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

