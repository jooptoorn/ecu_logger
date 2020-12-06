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
        sm->reentry = NO_RE_ENTRY_STATE;
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
        sm->reentry = NO_RE_ENTRY_STATE;
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

void crank_pulse_init(machine_state_t *sm)
{
	/*
	
	State Guard Variables
	
	*/
    static uint8_t regPulseCnt = 0;     //count number of normal pulses to predict when sync pulse occurs
    static float prevPulseDur = 0;
	
	/*
	
	State Entry
	
	*/
	if(STATE_IS_ENTERED(sm, &crank_pulse_init) || STATE_IS_RE_ENTERED(sm))
	{
		sm->currentState = &crank_pulse_init;
        sm->reentry = NO_RE_ENTRY_STATE;
        //reset guard var
        regPulseCnt = 0;
	}

	/*
	
	State Activity
	
	*/

	/*
	
	State Transition
	
	*/
    crank_pulse_data_t* data = (crank_pulse_data_t*) sm->data;      //recast generic pointer
    //check for new data through interrupt
    if(data->newTimerVal != data->prevTimerVal)
    {
        //process new pulse
        uint32_t newPulseDur;

        if(data->newTimerVal > data->prevTimerVal)
            newPulseDur = data->newTimerVal - data->prevTimerVal;
        else
            newPulseDur = (UINT32_MAX - data->prevTimerVal) + data->newTimerVal + 1;   //deal with uint32 overflow
        
        /* Convert to normal seconds */
        float newPulseDur_f = (float) newPulseDur * data->timCntToTime;

        if(regPulseCnt < NUM_PULSE_PER_REV - 1)
        {
            /* happens for regular pulse */
            /* Sanity check pulse length */
            float minExpPulseDur = prevPulseDur * REG_PULSE_MARGIN_INV;
            float maxExpPulseDur = prevPulseDur * REG_PULSE_MARGIN;

            if(minExpPulseDur < newPulseDur_f && minExpPulseDur < maxExpPulseDur)
            {
                /* Happens for pulse that had a valid/expected duration.
                Increase regular pulse count and reset state vars */
                regPulseCnt++;
                
            }
            else
            {
                /* happens for invalid/unexpected pulses 
                Act as if the new pulse was the first regular pulse of a series */
                regPulseCnt = 1;
            }

        }
        else
        {
            /* happens for sync pulse */
             /* Sanity check pulse length */
            float minExpPulseDur = prevPulseDur * SYNC_PULSE_DELAY_F * SYNC_PULSE_MARGIN_INV;
            float maxExpPulseDur = prevPulseDur * SYNC_PULSE_DELAY_F * SYNC_PULSE_MARGIN;

            if(minExpPulseDur < newPulseDur_f && minExpPulseDur < maxExpPulseDur)
            {
                /* Happens for pulse that had a valid/expected duration.
                We are in sync, goto next state! */
            	GOTO_STATE(sm, &crank_pulse_reg);
                
            }
            else
            {
                //something must have gone wrong. Re-enter state and try again
                RE_ENTER_STATE(sm);
            }
            
        }
        
        //update state vars
        prevPulseDur = newPulseDur_f;
        data->prevTimerVal = data->newTimerVal;
        
    }

    /*
    
    State Exit
    
    */
	if(STATE_IS_EXIT(sm, &crank_pulse_init))
	{
		
	}
}

void crank_pulse_reg(machine_state_t *sm)
{
	/*
	
	State Guard Variables
	
	*/
	static uint8_t regPulseCnt;
	
	/*
	
	State Entry
	
	*/
	if(STATE_IS_ENTERED(sm, &crank_pulse_reg) || STATE_IS_RE_ENTERED(sm))
	{
		sm->currentState = &crank_pulse_reg;
        sm->reentry = NO_RE_ENTRY_STATE;
		//reset state
		regPulseCnt = 0;
	}

	/*
	
	State Activity
	
	*/
    //check if new pulse has been detected
	crank_pulse_data_t* data = (crank_pulse_data_t*) sm->data;      //recast generic pointer
	if(data->newTimerVal != data->prevTimerVal)
    {
		//process new pulse.
		//in this state, assume the pulses are of valid length. This will be check later on in sync state
        uint32_t newPulseDur;

        if(data->newTimerVal > data->prevTimerVal)
            newPulseDur = data->newTimerVal - data->prevTimerVal;
        else
            newPulseDur = (UINT32_MAX - data->prevTimerVal) + data->newTimerVal + 1;   //deal with uint32 overflow
        
        /* Convert timer count to duration in seconds and store */
        float newPulseDur_f = (float) newPulseDur * data->timCntToTime;
		uint8_t idx = regPulseCnt % NUM_PULSE_PER_REV; //safe indexing
		data->pulseTimes[idx] = newPulseDur_f;

		//update state vars
        data->prevTimerVal = data->newTimerVal;
		regPulseCnt++;
	}

	/*
	
	State Transition
	
	*/
	//goto sync pulse state when received N-1 pulses out of a sequence with length N
	if(regPulseCnt == NUM_PULSE_PER_REV - 1)
		GOTO_STATE(sm, &crank_pulse_sync);

    /*
    
    State Exit
    
    */
	if(STATE_IS_EXIT(sm, &crank_pulse_reg))
	{
		
	}
}

void crank_pulse_sync(machine_state_t *sm)
{
	/*
	
	State Guard Variables
	
	*/
    uint32_t newPulseDur = 0;
	
	/*
	
	State Entry
	
	*/
	if(STATE_IS_ENTERED(sm, &crank_pulse_sync) || STATE_IS_RE_ENTERED(sm))
	{
		sm->currentState = &crank_pulse_sync;
        sm->reentry = NO_RE_ENTRY_STATE;
	}

	/*
	
	State Activity
	
	*/
	//check if new pulse has been detected
	crank_pulse_data_t* data = (crank_pulse_data_t*) sm->data;      //recast generic pointer
	if(data->newTimerVal != data->prevTimerVal)
    {
		//process new pulse.
		//in this state, assume the pulses are of valid length. This will be check later on in sync state

        if(data->newTimerVal > data->prevTimerVal)
            newPulseDur = data->newTimerVal - data->prevTimerVal;
        else
            newPulseDur = (UINT32_MAX - data->prevTimerVal) + data->newTimerVal + 1;   //deal with uint32 overflow
        
        /* Convert timer count to duration in seconds and store */
        float newPulseDur_f = (float) newPulseDur * data->timCntToTime;
		data->pulseTimes[NUM_PULSE_PER_REV - 1] = newPulseDur_f;

		//update state vars
        data->prevTimerVal = data->newTimerVal;
	}

	/*
	
	State Transition
	
	*/
	//check if new pulse was processed
	if(newPulseDur > 0)
	{
		//TODO SANITY CHECK result
		GOTO_STATE(sm, &crank_pulse_reg);
	}

    /*
    
    State Exit
    
    */
	if(STATE_IS_EXIT(sm, &crank_pulse_sync))
	{
		
	}
}