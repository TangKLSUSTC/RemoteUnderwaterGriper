#ifdef __cplusplus
extern "C" {
#endif
#include "test_ring.h"
#include "rtthread.h"

rt_uint8_t modbuf[20];
struct rt_ringbuffer mod;
struct rt_semaphore mod_sem;

void mod_init()
{
	rt_ringbuffer_init(&mod,modbuf,20); 
	rt_sem_init(&mod_sem,"modu",0,0);
}
MSH_CMD_EXPORT_ALIAS(mod_init,modtest,000);

void Enable_mod()
{
	mod_init();
}
void read_mod()
{
	rt_uint8_t ch;
	rt_err_t uwet=RT_EOK;
	do
	{
		uwet=rt_sem_take(&mod_sem,0);
		rt_ringbuffer_getchar(&mod,&ch);
		rt_kprintf("%c",ch);
	}
	while(ch!=0x0a);
}
MSH_CMD_EXPORT_ALIAS(read_mod,rm,000);
#ifdef __cplusplus
}
#endif