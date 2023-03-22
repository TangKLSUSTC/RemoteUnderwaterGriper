/*
*************************************************************************
*												BCL
*							SUSTech Robotic Institute
*
*Filename		: watergriper.c
*Programmer	: Kailuan Tang
*Description: This file is the spurce file of water griper controller command
*							reveiver.
*
*Version		:	V2.0
*************************************************************************
*/
#include "watergriper.h"


/*Basic varibles declaration*/
 uint8_t frame_buffer[12]={0};
 uint8_t frame_tx[20]={0};

/*Internal function prototype declaration*/
static void clear_ringbuffer(struct rt_ringbuffer* rb);

/************************************************************************
*wg_init	: initialize water griper struct
*
*INPUT: wg_t* wg :	pointer to water griper struct
*
*OUTPUT: none
*
*WARNINGS: This function is called before controller thread start.
*
========================================================================*/
void wg_init(wg_t* wg)
{
	wg->control_flag=0;
    wg->wg_state=0;
	wg->loss_cnt=0;
	wg->cmd_ptr=frame_tx;
    #if defined(WG_SLAVE)
        wg->arm_backward=arm_backward;
        wg->arm_forward=arm_forward;
        wg->arm_left=arm_left;
        wg->arm_right=arm_right;
        wg->arm_long=arm_long;
        wg->arm_short=arm_short;
        wg->grp_open=grp_open;
        wg->grp_close=grp_close;
        wg->arm_keep=arm_keep;
        wg->grp_keep=grp_keep;
    #endif
}

/************************************************************************
*wg_frame_process	: process water griper control frame
*
*INPUT: wg_t* wg :	pointer to water griper struct
*				struct rt_ringbuffer* rb	:	ringbuffer struct
*
*OUTPUT: none
*
*WARNINGS: This function is called in each pass of datareq thread
*
========================================================================*/
void wg_frame_process(wg_t* wg,struct rt_ringbuffer* rb)
{
	uint8_t tmp=0;
	uint8_t i=0;
	uint8_t len=0;
	while(rt_ringbuffer_getchar(rb,&tmp))
	{
		frame_buffer[i]=tmp;
		i++;
		switch(tmp)
		{
			case frame_head:
			{
				if(i>2)
				{
					clear_ringbuffer(rb);
                    wg->loss_cnt++;
					break;
				}
				else break;
			}
        #if defined(WG_SLAVE)
			case GRP_OPEN:
			{
				write_bit((uint32_t*)&wg->control_flag,0,1);
                write_bit((uint32_t*)&wg->wg_state,5,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
			case GRP_CLOSE:
			{
				write_bit((uint32_t*)&wg->control_flag,1,1);
                write_bit((uint32_t*)&wg->wg_state,5,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
			case ARM_LONG:
			{
				write_bit((uint32_t*)&wg->control_flag,2,1);
                write_bit((uint32_t*)&wg->wg_state,5,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
			case ARM_SHORT:
			{
				write_bit((uint32_t*)&wg->control_flag,3,1);
                write_bit((uint32_t*)&wg->wg_state,5,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
			case ARM_LEFT:
			{
				write_bit((uint32_t*)&wg->control_flag,4,1);
                write_bit((uint32_t*)&wg->wg_state,5,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
			case ARM_RIGHT:
			{
				write_bit((uint32_t*)&wg->control_flag,5,1);
                write_bit((uint32_t*)&wg->wg_state,5,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
                wg->loss_cnt=0;
				break;
			}
			case ARM_FORWARD:
			{
				write_bit((uint32_t*)&wg->control_flag,6,1);
                write_bit((uint32_t*)&wg->wg_state,5,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
			case ARM_BACKWARD:
			{
				write_bit((uint32_t*)&wg->control_flag,7,1);
                write_bit((uint32_t*)&wg->wg_state,5,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
            case EJECT_AIR:
			{
				write_bit((uint32_t*)&wg->control_flag,8,1);
                write_bit((uint32_t*)&wg->wg_state,5,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
			case CMDACK:
			{
				write_bit((uint32_t*)&wg->control_flag,10,1);
                write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
        #endif
        #if defined(WG_MASTER)
			case CMDACK:
			{
				write_bit((uint32_t*)&wg->control_flag,9,1);
        write_bit((uint32_t*)&wg->wg_state,6,1);
				break;
			}
        #endif
			case CONNECT_ACK:
			{
				write_bit((uint32_t*)&wg->control_flag,5,1);
        write_bit((uint32_t*)&wg->wg_state,6,1);
				wg->loss_cnt=0;
				break;
			}
			case CONNECT_REQ:
			{
					write_bit((uint32_t*)&wg->control_flag,11,1);
					write_bit((uint32_t*)&wg->wg_state,6,1);
					wg->loss_cnt=0;
					break;
			}
			case 0x0a:
			{
				if(frame_buffer[i-1]!=0x0d)
				{
					clear_ringbuffer(rb);
					wg->loss_cnt=0;
					break;
				}
			}
		}
	}
}

/************************************************************************
*wg_frame_generate	: process water griper control frame
*
*INPUT: wg_t* wg :	pointer to water griper struct
*				wg_func_t:	function enum type of water griper
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to generate data frame.
*
========================================================================*/
void wg_cmd_generate(wg_t* wg,wg_func_t func)
{
	/*write frame head*/
	wg->cmd_ptr[0]=wg->cmd_ptr[1]=frame_head;
	wg->cmd_len=2;
	/*write insturction part*/
	switch((int)func)
	{
			case CMDACK:
			{
				wg->cmd_ptr[2]=CMDACK;
				wg->cmd_len++;
				break;
			}
			case CONNECT_REQ:
			{
				wg->cmd_ptr[2]=CONNECT_REQ;
				wg->cmd_len++;
				break;
			}
			case CONNECT_ACK:
			{
					wg->cmd_ptr[2]=CONNECT_ACK;
					wg->cmd_len++;
					break;
			}
	}
	/*write frame tail part*/
	wg->cmd_ptr[3]=frame_tail0;
	wg->cmd_ptr[4]=frame_tail1;
	wg->cmd_len+=2;
}



#if defined(WG_SLAVE)
/************************************************************************
*arm_forward	: turn arm forward
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to turn arm forward
*
========================================================================*/
void arm_forward()
{
	MUSCLE_1_EXPAND();
	MUSCLE_2_EXPAND();
	MUSCLE_0_SHRINK();
	MUSCLE_3_SHRINK();
}

/************************************************************************
*arm_backward	: turn arm backward
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to turn arm backward
*
========================================================================*/
void arm_backward()
{
	MUSCLE_1_SHRINK();
	MUSCLE_2_SHRINK();
	MUSCLE_0_EXPAND();
	MUSCLE_3_EXPAND();
}

/************************************************************************
*arm_left	: turn arm to left
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to turn arm left 
*
========================================================================*/
void arm_left()
{
	MUSCLE_0_EXPAND();
	MUSCLE_1_EXPAND();
	MUSCLE_2_SHRINK();
	MUSCLE_3_SHRINK();
}

/************************************************************************
*arm_right	: turn arm to right
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to turn arm right
*
========================================================================*/
void arm_right()
{
	MUSCLE_0_SHRINK();
	MUSCLE_1_SHRINK();
	MUSCLE_2_EXPAND();
	MUSCLE_3_EXPAND();
}

/************************************************************************
*arm_short	: make arm short
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to make arm short
*
========================================================================*/
void arm_short()
{
	MUSCLE_0_SHRINK();
	MUSCLE_1_SHRINK();
	MUSCLE_2_SHRINK();
	MUSCLE_3_SHRINK();
}

/************************************************************************
*arm_long	: make arm long
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to make arm short
*
========================================================================*/
void arm_long()
{
	MUSCLE_0_EXPAND();
	MUSCLE_1_EXPAND();
	MUSCLE_2_EXPAND();
	MUSCLE_3_EXPAND();
}

/************************************************************************
*arm_keep	: keep arm state
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to keep arm state.
*
========================================================================*/
void arm_keep()
{
		MUSCLE_0_KEEP();
    MUSCLE_1_KEEP();
    MUSCLE_2_KEEP();
    MUSCLE_3_KEEP();
}

/************************************************************************
*grp_open	: open griper
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to open griper.
*
========================================================================*/
void grp_open()
{
	MUSCLE_GRP_OPEN();
}

/************************************************************************
*grp_close	: close griper
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to close griper.
*
========================================================================*/
void grp_close()
{
	MUSCLE_GRP_CLOSE();
}

/************************************************************************
*grp_keep	: keep grp state
*
*INPUT: none
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to keep griper state.
*
========================================================================*/
void grp_keep()
{
	MUSCLE_GRP_KEEP();
}
#endif

/************************************************************************
*clear_ringbuffer	: clear data frame buffer
*
*INPUT: struct rt_ringbuffer* rb :	pointer to rx ring buffer
*
*OUTPUT: none
*
*WARNINGS: This function is called when need to clear buffer 
*					 (e.g. data error).
*
========================================================================*/
void clear_ringbuffer(struct rt_ringbuffer* rb)
{
	uint8_t tmp=0;
	while(rt_ringbuffer_getchar(rb,&tmp));
}