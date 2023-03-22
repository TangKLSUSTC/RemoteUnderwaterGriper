#ifdef __cplusplus
extern "C" {
#endif
#pragma once
#include "sys.h"
#include "rtthread.h"
#include "IO_config.h"
#include <stdio.h>
#include <stdlib.h>
#include "data_req.h"
#include "bit_io.h"
#include "watergriper.h"
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
				/*Extern */
extern rt_sem_t adc_sig;
extern DMA_HandleTypeDef hdma_adc1;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
				/*Functions prototypes*/
void control_thread_init();
void controller_init();
void Muscles(void* param);

void Muscle_channel_init();
static uint8_t get_msta(uint8_t k);
static void write_msta(uint8_t k, uint8_t bit);
static char BBcontrol(int dest_val,int threshold,int val_now);
static char BBcontrol_ex(int dest_val,int threshold,int val_now);
static rt_bool_t chn_assert(uint8_t chn);
static uint8_t get_use(uint8_t k);
static void write_use(uint8_t k,uint8_t bit);



#ifdef STM32F767xx
static uint16_t pressure_read(uint8_t channel);
static void pressure_set(uint8_t channel,uint16_t value);
static void threshold_adj(uint8_t channel,uint16_t value);
static void prec_help();
#endif
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
				/*Struct*/
struct muscle_cb{
	uint32_t* air_pre;
	uint32_t* threshold;
	char* ctrl_cnt;
	uint16_t use_of_muscle;
	uint16_t muscle_sta;
};
struct pump_sta_t
{
	uint8_t pump_in:1;
	uint8_t pimp_out:1;
};

#ifdef __cplusplus
}
#endif