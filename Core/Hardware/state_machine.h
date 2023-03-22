/*
*************************************************************************
*											BCL
*						SUSTech Robotic Institute
*
*Filename		: state_machine.h
*Programmer	: Kailuan Tang
*Description: This file is the head file of public state machine logic.
*
*Version		:	V1.0
*************************************************************************
*/
#pragma once
#include "rtthread.h"



/*One in one out state node*/
struct state_base
{
	/*
		Pointer to last state
	*/
	struct state_base* last_state;
	/*
		Pointer to next state
	*/
	struct state_base* next_state;
	/*
		Pointer to state node entry function
	*/
	void 			*entry;
	/*
		parameters
	*/
	void 			*parameter;
	/*
		Case, to switch to next state node
	*/
	void			*state;
	/*
		Public data block pointer to any type
		To store and share data among state nodes
	*/
	void			*usr_data;
};
typedef struct state_base* state_t;


struct state_branch
{
	union last_state
	{
		struct state_base*   last_state_b;
		struct state_branch* last_state_f;
	}last_state;
	
	union next_state
	{
		struct state_base*   next_state_b;
		struct state_branch* next_state_f;
	}next_state;
	/*
		Pointer to state node entry function
	*/
	void 			*entry;
	/*
		parameters
	*/
	void 			*parameter;
	
	/*
		Public data block pointer to any type
		To store and share data among state nodes
	*/
	void			*usr_data;
	
};

struct state_control_block_base
{
	struct state_base* current_node;
	void* current_state;
	
};
typedef struct state_control_block_base smcb_t;


