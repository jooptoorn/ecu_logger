#ifndef _CRANK_H_
#define _CRANK_H_

#include <stdbool.h>
#include "main.h"

#define NUM_PULSE_PER_REV 6	//Number of crank sensor pulses per one revolution of crankshaft

//margin is factor that a pulse may deviate from previous pulse to be considered as 'regular'.
//max expected new_pulse_t = regular_pulse_margin * previous_pulse_t
//min expected new_pulse_t = (1/regular_pulse_margin) * previous_pulse_t
#define REG_PULSE_MARGIN 1.25f
#define REG_PULSE_MARGIN_INV (1.0f/REG_PULSE_MARGIN)
//duration between previous and sync pulse rising flank must be SYNC_PULSE_DELAY_F longer than regular pulse duration. I.e., it is significantly later than a regular pulse would be.
//note that the next pulse will happen faster, at factor (2 - SYNC_PULSE_DELAY_F) when compared to regular interval
#define SYNC_PULSE_DELAY_F 1.6f
#define SYNC_PULSE_MARGIN 1.25f
#define SYNC_PULSE_MARGIN_INV (1.0f/SYNC_PULSE_MARGIN)

//timeouts for synced operation
#define PULSE_TIMEOUT_S 1u	//uint of number of seconds that statemachine will wait to receive next crank signal pulse

#define NO_RE_ENTRY_STATE 0
#define RE_ENTRY_STATE (!NO_RE_ENTRY_STATE)

#define STATE_IS_ENTERED(x,y) 	(x->currentState != y)
#define STATE_IS_RE_ENTERED(x) 	(x->reentry == RE_ENTRY_STATE)
#define STATE_IS_EXIT(x,y) 		(x->nextState != y)
#define RE_ENTER_STATE(x)		(x->reentry = RE_ENTRY_STATE)
#define GOTO_STATE(x,y)			(x->nextState = y)

/**
*
*	Utilities
*
*/


/**
 * 
 *	State machine defs  
 * 
 */

typedef struct machine_state_s machine_state_t;

typedef void (state_func_t)(machine_state_t*);

typedef struct machine_state_s
{
	state_func_t *currentState;	//current (or prev) state
	state_func_t *nextState;	//next state
	uint8_t reentry;			//flag for state re-entry
	void *data;					//pointer to any data structure that may be used inside statemachine
} machine_state_t;

// data belonging to pulse statemachine
typedef struct crank_pulse_data_s 
{
	uint32_t prevTimerVal;
	uint32_t newTimerVal;
	float engineRpm;
	float engineRps;					//rev per second
	float timCntToTime;					//factor to convert timer count (from sys clk) to actual time in seconds
	float pulseTimes[NUM_PULSE_PER_REV];//store intervals
} crank_pulse_data_t;

void crank_pulse_init(machine_state_t *sm);
void crank_pulse_reg(machine_state_t *sm);
void crank_pulse_sync(machine_state_t *sm);

#endif