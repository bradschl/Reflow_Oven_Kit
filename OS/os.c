/*
 * os.c
 *
 *  Created on: Apr 12, 2013
 *      Author: Nate
 */


#include "os.h"
#include "DRIVE/rtc.h"
#include "DRIVE/timer.h"
#include "LCD/colorLCD.h"
#include "LCD/touch.h"
#include "DRIVE/GPIO.h"
#include "DRIVE/rtc.h"
#include "OVEN/oven.h"
#include "OVEN/oven_control.h"
#include "DRIVE/led.h"
#include "GUI/GUIManager.h"
#include "GUI/ReflowGUI.h"

#define TICK_BIT_16MS 4
#define TICK_BIT_32MS 5
#define TICK_BIT_64MS 6
#define TICK_BIT_256MS 8
#define TICK_BIT_512MS 9

uint16_t bit_modulous(uint16_t num, uint16_t bit);

void run_tasks(uint16_t tick_ms)
{
	if(bit_modulous(tick_ms, TICK_BIT_16MS) == 0)
	{
		stepLEDStateMachined();
		stepOvenStateMachine();
	}

	if(bit_modulous(tick_ms, TICK_BIT_32MS) == 1)
	{
		update_temp();
	}

	if(bit_modulous(tick_ms, TICK_BIT_64MS) == 2)
	{
		if(Get_LED_Timer_Status())
		{
			GLED_Sequence();
			RLED_Sequence();
			BLED_Sequence();
		}
	    updateTouchValue();
	}


	if(bit_modulous(tick_ms, TICK_BIT_256MS) == 3)
	{
	    stepGUIManagerTask();
	}

	if((bit_modulous(tick_ms, TICK_BIT_512MS) == 4))
	{
	    stepReflowGUI();

	    setBackgroundColor16(COLOR_16_BLACK);
		setColor16(COLOR_16_WHITE);
		get_system_time_string(text_buffer);
		drawString(10,160,text_buffer);
		//print_oven_data();

		// TODO: Fix graph to be period agnostic
		/*if(task_flags & graph_update)
		{
			update_graph();
			task_flags &= ~graph_update;
		}*/

		//RLED_OUT ^= RLED;
	}

}

void runScheduler_noRet()
{
	uint16_t currentTick_ms;
	uint16_t newTick_ms;

	currentTick_ms = getTaskTick_ms() - 1;
	while(1)
	{
		newTick_ms = getTaskTick_ms();
		if(newTick_ms != currentTick_ms)
		{
			currentTick_ms++;
			if(currentTick_ms >= MAX_CPU_TICK_TIME_MS)
			{
			    currentTick_ms -= MAX_CPU_TICK_TIME_MS;
			}

			run_tasks(currentTick_ms);
		}
		else
		{
		    // Background tasks
		    P2OUT ^= 1 << 7;
		    publishGUIEvents();
		}
	}
}


uint16_t bit_modulous(uint16_t num, uint16_t bit)
{
	return (~(0xFFFF << bit)) & num;
}
