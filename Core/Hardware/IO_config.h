#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#include "IO_def.h"
#define Griper_Close_Ena io_control(&gswitch.iob[0],1)

extern io_group mi;
extern io_group mo;
extern io_group pp;

extern io_base muscle_in[4];
extern io_base muscle_out[4];

extern io_base pump[2];
extern io_base griper_open;
extern io_base griper_close;

extern io_base griper_switch[2];
extern io_base arm_flex_pin[2];
extern io_group gswitch;
extern io_group arm_flex;
#ifdef STM32F767xx

extern const io_group m_in;
extern const io_group m_out;

extern const io_group csgroup;

#endif
#ifdef STM32F103xE

extern const io_group m_in;
extern const io_group m_out;

extern const io_group csgroup;

#endif
#ifdef __cplusplus
}
#endif