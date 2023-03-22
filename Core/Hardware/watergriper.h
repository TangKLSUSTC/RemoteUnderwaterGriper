/*
*************************************************************************
*												BCL
*							SUSTech Robotic Institute
*
*Filename		: watergriper.h
*Programmer	: Kailuan Tang
*Description: This file is the head file of water griper controller command
*							reveiver.
*
*Version		:	V2.0
*************************************************************************
*/
#pragma once

/*Include files*/
#include "bit_io.h"
#include "my_ringbuf_8bit.h"
#include "IO_config.h"


#define U32P (uint32_t*)

#define CTRL_MSK 0x01FF

/*io operate definations*/
#if defined(WG_SLAVE)
    #define PUMP_IN_ON   	io_control(&pump[0],1)
    #define PUMP_IN_OFF  	io_control(&pump[0],0)
    #define PUMP_OUT_ON  	io_control(&pump[1],1)
    #define PUMP_OUT_OFF 	io_control(&pump[1],0)

    #define MIN_0_ON			io_control(&muscle_in[0],1)
    #define MIN_1_ON			io_control(&muscle_in[1],1)
    #define MIN_2_ON			io_control(&muscle_in[2],1)
    #define MIN_3_ON			io_control(&muscle_in[3],1)

    #define MIN_0_OFF			io_control(&muscle_in[0],0)
    #define MIN_1_OFF			io_control(&muscle_in[1],0)
    #define MIN_2_OFF			io_control(&muscle_in[2],0)
    #define MIN_3_OFF			io_control(&muscle_in[3],0)

    #define MOUT_0_ON			io_control(&muscle_out[0],1)
    #define MOUT_1_ON			io_control(&muscle_out[1],1)
    #define MOUT_2_ON			io_control(&muscle_out[2],1)
    #define MOUT_3_ON			io_control(&muscle_out[3],1)

    #define MOUT_0_OFF		io_control(&muscle_out[0],0)
    #define MOUT_1_OFF		io_control(&muscle_out[1],0)
    #define MOUT_2_OFF		io_control(&muscle_out[2],0)
    #define MOUT_3_OFF		io_control(&muscle_out[3],0)

    //#define RELEASE_VALVE_ON   io_control(&release_valve,1)
    //#define RELEASE_VALVE_OFF  io_control(&release_valve,0)

    #define GRP_IN_ON			io_control(&griper_close,1)
    #define GRP_IN_OFF		io_control(&griper_close,0)
    #define GRP_OUT_ON 		io_control(&griper_open,1)
    #define GRP_OUT_OFF 	io_control(&griper_open,0)

    #define PUMP_IN_ON   	io_control(&pump[0],1)
    #define PUMP_IN_OFF 	io_control(&pump[0],0)
    #define PUMP_OUT_ON 	io_control(&pump[1],1)
    #define PUMP_OUT_OFF 	io_control(&pump[1],0)
#endif


enum wg_frame
{
	frame_head=0xff,
	frame_tail0=0x0d,
	frame_tail1=0x0a
};

typedef enum wg_function
{
	GRP_OPEN=0x01,
	GRP_CLOSE,
	ARM_SHORT,
	ARM_LONG,
	ARM_LEFT,
	ARM_RIGHT,
	ARM_FORWARD,
	ARM_BACKWARD,
	CMDACK,
	CONNECT_ACK=0x1a,
	CONNECT_REQ,
  CMDRX_Confirm,
	EJECT_AIR
}wg_func_t;


typedef struct wg_struct
{
    uint16_t control_flag;
    uint16_t wg_state;
    uint8_t loss_cnt;
    uint8_t cmd_len;
    uint16_t placeholder1;
    uint8_t *cmd_ptr;
    #if defined(WG_SLAVE)
        void (*arm_forward) (void);
        void (*arm_backward) (void);
        void (*arm_left) (void);
        void (*arm_right) (void);
        void (*arm_long) (void);
        void (*arm_short) (void);
        void (*grp_open) (void);
        void (*grp_close) (void);
        void (*arm_keep) (void);
        void (*grp_keep) (void);
    #endif
}wg_t;

/*
    Extern functions
*/
extern void wg_init(wg_t *wg);
extern void wg_frame_process(wg_t* wg,struct rt_ringbuffer* rb);
extern void wg_cmd_generate(wg_t* wg,wg_func_t func);

#if defined(WG_SLAVE)
    extern void arm_forward();
    extern void arm_backward();
    extern void arm_left();
    extern void arm_right();
    extern void arm_long();
    extern void arm_short();
    extern void arm_keep();
    extern void grp_open();
    extern void grp_close();
    extern void grp_keep();
#endif

/************************************************************************
*		Inline Functions
*
*		INPUT: none
*
*		OUTPUT: none
*
*		WARNINGS: These functions only control valves, when device is in 
*                 slave mode.
*
========================================================================*/
#if defined(WG_SLAVE)
    __STATIC_INLINE void MUSCLE_0_EXPAND()
    {
        MIN_0_ON;
        MOUT_0_OFF;
    }

    __STATIC_INLINE void MUSCLE_1_EXPAND()
    {
        MIN_1_ON;
        MOUT_1_OFF;
    }

    __STATIC_INLINE void MUSCLE_2_EXPAND()
    {
        MIN_2_ON;
        MOUT_2_OFF;
    }

    __STATIC_INLINE void MUSCLE_3_EXPAND()
    {
        MIN_3_ON;
        MOUT_3_OFF;
    }

    __STATIC_INLINE void MUSCLE_0_SHRINK()
    {
        MIN_0_OFF;
        MOUT_0_ON;
    }

    __STATIC_INLINE void MUSCLE_1_SHRINK()
    {
        MIN_1_OFF;
        MOUT_1_ON;
    }

    __STATIC_INLINE void MUSCLE_2_SHRINK()
    {
        MIN_2_OFF;
        MOUT_2_ON;
    }

    __STATIC_INLINE void MUSCLE_3_SHRINK()
    {
        MIN_3_OFF;
        MOUT_3_ON;
    }

    __STATIC_INLINE void MUSCLE_0_KEEP()
    {
        MIN_0_OFF;
        MOUT_0_OFF;
    }

    __STATIC_INLINE void MUSCLE_1_KEEP()
    {
        MIN_1_OFF;
        MOUT_1_OFF;
    }

    __STATIC_INLINE void MUSCLE_2_KEEP()
    {
        MIN_2_OFF;
        MOUT_2_OFF;
    }

    __STATIC_INLINE void MUSCLE_3_KEEP()
    {
        MIN_3_OFF;
        MOUT_3_OFF;
    }
	#if 0
    __STATIC_INLINE void MUSCLE_0_RELEASE()
    {
        MOUT_0_ON;
        RELEASE_VALVE_ON;
    }

    __STATIC_INLINE void MUSCLE_1_RELEASE()
    {
        MOUT_0_ON;
        RELEASE_VALVE_ON;
    }

    __STATIC_INLINE void MUSCLE_2_RELEASE()
    {
        MOUT_0_ON;
        RELEASE_VALVE_ON;
    }

    __STATIC_INLINE void MUSCLE_3_RELEASE()
    {
        MOUT_0_ON;
        RELEASE_VALVE_ON;
    }
	#endif
    __STATIC_INLINE void MUSCLE_GRP_OPEN()
    {
        GRP_OUT_ON;
        GRP_IN_OFF;
    }

    __STATIC_INLINE void MUSCLE_GRP_CLOSE()
    {
        GRP_IN_ON;
        GRP_OUT_OFF;
    }

    __STATIC_INLINE void MUSCLE_GRP_KEEP()
    {
        GRP_IN_OFF;
        GRP_OUT_OFF;
    }
	#if 0
    __STATIC_INLINE void MUSCLE_grp_RELEASE()
    {
        MOUT_0_ON;
        RELEASE_VALVE_ON;
    }
	#endif
    __STATIC_INLINE void MUSCLE_EJECT_AIR()
    {
        MIN_0_ON;
        MOUT_0_ON;
        MIN_1_ON;
        MOUT_1_ON;
        MIN_2_ON;
        MOUT_2_ON;
        MIN_3_ON;
        MOUT_3_ON;
        GRP_IN_ON;
        GRP_OUT_ON;
    }
#endif
