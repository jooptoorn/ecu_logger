#ifndef _CRANK_H_
#define _CRANK_H_

#include <stdint.h>

#define NO_RE_ENTRY_STATE 0
#define RE_ENTRY_STATE (!NO_RE_ENTRY_STATE)

#define STATE_IS_ENTERED(x,y) 	(x->currentState != y)
#define STATE_IS_RE_ENTERED(x) 	(x->reentry == RE_ENTRY_STATE)
#define STATE_IS_EXIT(x,y) 		(x->nextState != y)
#define RE_ENTER_STATE(x)		(x->reentry = RE_ENTRY_STATE)
#define GOTO_STATE(x,y)			(x->nextState = y)

typedef struct machine_state_s machine_state_t;

typedef void (state_func_t)(machine_state_t*);

typedef struct machine_state_s
{
	state_func_t *currentState;
	state_func_t *nextState;
	uint8_t reentry;
} machine_state_t;

void crank_init(machine_state_t *sm);
void crank_step(machine_state_t *sm);

#endif