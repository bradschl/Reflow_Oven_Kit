/*
 * os.c
 *
 *  Created on: Apr 12, 2013
 *      Author: Nate
 */


#include "os.h"
#include "../DRIVE/rtc.h"
#include "../DRIVE/timer.h"
#include "../LCD/colorLCD.h"
#include "../LCD/touch.h"
#include "../LCD/gui.h"
#include "../DRIVE/GPIO.h"
#include "../DRIVE/rtc.h"
#include "../OVEN/oven.h"
#include "../OVEN/oven_control.h"
#include "../DRIVE/led.h"

#define TICK_BIT_16MS 4
#define TICK_BIT_32MS 5
#define TICK_BIT_64MS 6
#define TICK_BIT_256MS 8
#define TICK_BIT_512MS 9

uint16_t bit_modulous(uint16_t num, uint16_t bit);

void run_tasks(uint16_t tick_ms)
{
	if((bit_modulous(tick_ms, TICK_BIT_16MS) == 0) && (tick_ms >=16))
	{
		stepPageStateMachine();
		stepLEDStateMachined();
		stepOvenStateMachine();
	}

	if((bit_modulous(tick_ms, TICK_BIT_32MS) == 0) && (tick_ms >=32))
	{
		update_temp();
	}

	if((bit_modulous(tick_ms, TICK_BIT_64MS) == 0) && (tick_ms >=64))
	{
		if(Get_LED_Timer_Status())
		{
			GLED_Sequence();
			RLED_Sequence();
			BLED_Sequence();
		}

	}


	if((bit_modulous(tick_ms, TICK_BIT_256MS) == 0) && (tick_ms >=256))
	{

	}

	if((bit_modulous(tick_ms, TICK_BIT_512MS) == 0) && (tick_ms >=512))
	{

		setColor16(COLOR_16_WHITE);
		get_system_time_string(text_buffer);
		drawString(10,160,text_buffer);
		print_oven_data();

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
	uint16_t lastTick_ms = 0;
	uint16_t newTick_ms = 0;

	while(1)
	{
		newTick_ms = getTaskTick_ms();
		if(newTick_ms != lastTick_ms)
		{
			lastTick_ms = newTick_ms;
			run_tasks(newTick_ms);
		}
	}
}


uint16_t bit_modulous(uint16_t num, uint16_t bit)
{
	return (~(0xFFFF << bit)) & num;
}
