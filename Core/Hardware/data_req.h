#pragma once

#include "main.h"
#include "sys.h"
#include "rtthread.h"
#include "my_ringbuf_8bit.h"
#include "watergriper.h"
#define DATAPORT USART3


typedef enum req_mode
{
	passive=0xa1,
	active
}req_mode_t;

enum req_status
{
	FRAME_OK=0x01,
	FRAME_ERR
};
typedef enum req_status req_sta_t;

struct data_req
{
	#if defined(USE_NORMALIZED_DATA)
		float *data_ptr;
	#else
		int16_t *data_ptr;
	#endif
	uint32_t data_size;
};

typedef struct data_req data_req_t;
extern wg_t wg;

extern rt_sem_t req_sync;
extern rt_sem_t conchk_req_sem;
extern data_req_t data_cb;
extern rt_mq_t data_req_mq;
extern void data_requester_init();

extern void cmd_tx(uint8_t* str, uint8_t len);
extern req_sta_t frame_process(struct rt_ringbuffer* rb);
