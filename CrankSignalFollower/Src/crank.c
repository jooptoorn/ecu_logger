#include "crank.h"

void crank_init(machine_state_t *sm)
{
	/*
	
	State Guard Variables
	
	*/
	
	/*
	
	State Entry
	
	*/
	if(STATE_IS_ENTERED(sm, &crank_init) || STATE_IS_RE_ENTERED(sm))
	{
		sm->currentState = &crank_init;
        sm->reentry = 0;
	}

	/*
	
	State Activity
	
	*/

	/*
	
	State Transition
	
	*/
    GOTO_STATE(sm, crank_step);

    /*
    
    State Exit
    
    */
	if(STATE_IS_EXIT(sm, &crank_init))
	{
		
	}
}

void crank_step(machine_state_t *sm)
{
	/*
	
	State Guard Variables
	
	*/
	
	/*
	
	State Entry
	
	*/
	if(STATE_IS_ENTERED(sm, &crank_step) || STATE_IS_RE_ENTERED(sm))
	{
		sm->currentState = &crank_step;
        sm->reentry = 0;
	}

	/*
	
	State Activity
	
	*/

	/*
	
	State Transition
	
	*/
    RE_ENTER_STATE(sm);

    /*
    
    State Exit
    
    */
	if(STATE_IS_EXIT(sm, &crank_step))
	{
		
	}
}