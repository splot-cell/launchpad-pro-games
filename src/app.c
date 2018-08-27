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

#define BUTTON_COUNT 100

#define NUM_ROWS 10
#define NUM_COLS 10

u8 snake[BUTTON_COUNT] = {0}; // arbitrary number outside range of pads
u8 snake_length;

enum u8
{
    UP, LEFT, DOWN, RIGHT
};

u8 dir = UP;

//______________________________________________________________________________

void app_surface_event(u8 type, u8 index, u8 value)
{
    if (type == TYPESETUP && value != 0)
    {
        
    }
    if (type == TYPEPAD && value != 0)
    {
        if (index == 91 && dir != DOWN)
            dir = UP;
        else if (index == 92 && dir != UP)
            dir = DOWN;
        else if (index == 93 && dir != RIGHT)
            dir = LEFT;
        else if (index == 94 && dir != LEFT)
            dir = RIGHT;
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

void updateLEDS()
{
    for (u8 i = 0; i < BUTTON_COUNT; ++i)
    {
        hal_plot_led(TYPEPAD, i, 0, 0, 0);
    }
    hal_plot_led(TYPEPAD, snake[0], 65, 65, 127); // Make head blue
    for (u8 j = 1; j < snake_length; ++j)
    {
        hal_plot_led(TYPEPAD, snake[j], 127, 127, 127);
    }
}

//______________________________________________________________________________

void moveSnake()
{
    s8 head_row = snake[0] / (u8) NUM_ROWS;
    s8 head_col = snake[0] % NUM_COLS;
    for (u8 count = BUTTON_COUNT - 1; count > 0; --count)
    {
        snake[count] = snake[count - 1];
    }
    
    if (dir == UP)
        head_row = (head_row + 1) % NUM_ROWS;
    else if (dir == LEFT)
        head_col = (NUM_COLS + head_col - 1) % NUM_COLS;
    else if (dir == RIGHT)
        head_col = (head_col + 1) % NUM_COLS;
    else if (dir == DOWN)
        head_row = (NUM_ROWS + head_row - 1) % NUM_ROWS;
    
    snake[0] = head_row * NUM_COLS + head_col;
}

//______________________________________________________________________________

void app_timer_event()
{
#define VIEW_REFRESH 250
    
    static u16 ms = 0;
    
    if (ms >= VIEW_REFRESH)
    {
        ms = 0;
        moveSnake();
        updateLEDS();
    }
    
    ++ms;
}

//______________________________________________________________________________

void app_init(const u16 *adc_raw)
{
    // store off the raw ADC frame pointer for later use
    g_ADC = adc_raw;
    
    snake_length = 5;
    
    snake[0] = 44;
    updateLEDS();
}
