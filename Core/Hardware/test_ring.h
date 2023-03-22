#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "my_ringbuf.h"
extern struct rt_ringbuffer mod;
extern struct rt_semaphore mod_sem;
void mod_init();
#ifdef __cplusplus
}
#endif