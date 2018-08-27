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

// Two arrays of LED status data
u8 state1[BUTTON_COUNT] = {0};
u8 state2[BUTTON_COUNT] = {0};

// Pointers for accessing the arrays
u8 *currentState;
u8 *nextState;

// Define different states for program
enum status_enum
{
	SET_RED = 0,
	SET_GREEN = 1,
	SET_BLUE = 2,
	RUN = 3
};

// Default state
u8 status = SET_RED;

// Function declarations
void setLED(u8 value, u8 index);
u8 colourToMask(u8 colour);

//______________________________________________________________________________

void app_surface_event(u8 type, u8 index, u8 value)
{
	if (type == TYPESETUP && value != 0)
	{
		status += 1;
		status = status % 4;
	}
	if (type == TYPEPAD && value != 0 && status <= SET_BLUE)
	{
		currentState[index] = currentState[index] ^ colourToMask(status); // Toggle bit using XOR
		setLED(currentState[index], index);
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

u8 colourToMask(u8 colour)
{
	return 1 << colour;
}

//______________________________________________________________________________

u8 countNeighbours(u8 col, u8 row, u8 colour)
{
	u8 total = 0;
	for (s8 i = -1; i < 2; ++i)
	{
		for (s8 j = -1; j < 2; ++j)
		{
			u8 columnIndex = (10 + col + j) % 10;
			u8 rowIndex = (10 + row + i) % 10;
			total += (currentState[columnIndex + (10 * rowIndex)] & colourToMask(colour)) >> colour; // count neighbours
		}
	}
	total -= (currentState[col + (row * 10)] & colourToMask(colour)) >> colour; // remove count of own led
	return total;
}

//______________________________________________________________________________

void setLED(u8 value, u8 index)
{
	u8 r = 0, g = 0, b = 0;
	if (value & colourToMask(SET_RED))
		r = MAXLED;
	if (value & colourToMask(SET_GREEN))
		g = MAXLED;
	if (value & colourToMask(SET_BLUE))
		b = MAXLED;
	
	hal_plot_led(TYPEPAD, index, r, g, b);
}

//______________________________________________________________________________

void updateLEDS()
{
	for (u8 i = 0; i < BUTTON_COUNT; ++i)
    {
    	setLED(currentState[i], i);
	}
}

//______________________________________________________________________________

void app_timer_event()
{
#define TICK_MS 100 // Refresh rate
	
    static u8 ms = TICK_MS;	
    
	if (++ms >= TICK_MS && status == RUN)
	{
		static u8 colour = SET_RED; // cycle through each colour
		ms = 0;
		
		for (u8 row = 0; row < 10; ++row)
		{
			for (u8 col = 0; col < 10; ++col)
			{
				u8 index = col + (10 * row);				
				u8 count = countNeighbours(col, row, colour);
    		
				if (count == 3)
					nextState[index] = currentState[index] | colourToMask(colour);        			
				else if (count == 2 && (currentState[index] & colourToMask(colour)))
					nextState[index] = currentState[index] | colourToMask(colour);
				else
					nextState[index] = currentState[index] & ~colourToMask(colour);
			}
		}
		
		colour = (colour + 1) % 3;
		
		// Swap over arrays and update LEDs
		u8 *temp;
		temp = currentState;
		currentState = nextState;
		nextState = temp;
		updateLEDS();
	}

}

//______________________________________________________________________________

void app_init(const u16 *adc_raw)
{
	// Initialise pointers
	currentState = state1;
	nextState = state2;
	// store off the raw ADC frame pointer for later use
	g_ADC = adc_raw;
}
