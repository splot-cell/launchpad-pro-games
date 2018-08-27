/******************************************************************************
 
 Copyright (c) 2015, Focusrite Audio Engineering Ltd.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of Focusrite Audio Engineering Ltd., nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 *****************************************************************************/

//______________________________________________________________________________
//
// Headers
//______________________________________________________________________________

#include "app.h"

//______________________________________________________________________________
//
// This is where the fun is!  Add your code to the callbacks below to define how
// your app behaves.
//
// In this example, we either render the raw ADC data as LED rainbows or store
// and recall the pad state from flash.
//______________________________________________________________________________

// store ADC frame pointer
static const u16 *g_ADC = 0;

// buffer to store pad states for flash save
#define BUTTON_COUNT 100

u8 state1[BUTTON_COUNT] = {0};
u8 state2[BUTTON_COUNT] = {0};

u8 *currentState;
u8 *nextState;

u8 start = 0;

//______________________________________________________________________________

void app_surface_event(u8 type, u8 index, u8 value)
{
	if (type == TYPESETUP && value != 0)
	{
		start = 1;
	}
	if (type == TYPEPAD && value != 0 && !start)
	{
		currentState[index] = 1 * !currentState[index];
		hal_plot_led(TYPEPAD, index, 0, MAXLED * currentState[index], 0);	
	}
}

//______________________________________________________________________________

void app_midi_event(u8 port, u8 status, u8 d1, u8 d2)
{
	
}

//______________________________________________________________________________

void app_sysex_event(u8 port, u8 * data, u16 count)
{
    // example - respond to UDI messages?
}

//______________________________________________________________________________

void app_aftertouch_event(u8 index, u8 value)
{
	
}

//______________________________________________________________________________

void app_cable_event(u8 type, u8 value)
{
	
}

//______________________________________________________________________________

u8 countNeighbours(u8 index)
{
	s8 total = 0;
	
// 	total += currentState[index - 11];
// 	total += currentState[index - 10];
// 	total += currentState[index - 9];
// 
// 	total += currentState[index - 1];
// 	total += currentState[index + 1];
// 	
// 	total += currentState[index + 9];
// 	total += currentState[index + 10];
// 	total += currentState[index + 11];	

	for (s8 i = -10; i < 20; i += 10)
	{
		for (s8 j = -1; j < 2; ++j)
		{
			total += currentState[index + j + i];
		}
	}
	total -= currentState[index]; // remove count of own led
	return total;
}

//______________________________________________________________________________

void updateLEDS()
{
	for (u8 i = 0; i < BUTTON_COUNT; ++i)
    {
		hal_plot_led(TYPEPAD, i, 0, currentState[i] * MAXLED, 0);
	}
}

//______________________________________________________________________________

void app_timer_event()
{
    // example - send MIDI clock at 125bpm
#define TICK_MS 200
#define VIEW_REFRESH 100
    
    if (start)
    {
   	    static u8 ms = 0;
   	    
   	    if (ms < VIEW_REFRESH)
   	    	nextState[ms] = currentState[ms];
   	    
	    if (ms > 10 && ms < 89)
    	{
    		u8 count = countNeighbours(ms);
    		
	        if (count == 3)
    	    	nextState[ms] = 1;        			
	    	else if (count == 2 && currentState[ms] == 1)
        		nextState[ms] = 1;
	        else
    	    	nextState[ms] = 0;
	    }
    	else if (ms >= VIEW_REFRESH)
	    {
    		hal_plot_led(TYPEPAD, ms - BUTTON_COUNT, 0, nextState[ms - BUTTON_COUNT] * MAXLED, 0);
	    }
	    if (ms >= TICK_MS)
	    {
	    	u8 *temp;
	    	temp = currentState;
    	    currentState = nextState;
        	nextState = temp;
	    	ms = -1;
	    }
		ms++;
    }
}

//______________________________________________________________________________

void app_init(const u16 *adc_raw)
{
	currentState = state1;
	nextState = state2;
	// store off the raw ADC frame pointer for later use
	g_ADC = adc_raw;
}
