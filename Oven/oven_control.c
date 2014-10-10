/*
 * oven_control.c
 *
 *  Created on: Apr 13, 2013
 *      Author: Nate
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// TODO: Remove this include
#include <msp430g2553.h>

#include "Profiles/TempProfiles.h"
#include "ScalableProfile/SProfile.h"

#include "oven_control.h"
#include "oven.h"
#include "OS/time.h"

static uint16_t secondsIntoProfile      = 0;
static uint16_t secondsLeftInProfile    = 0;

static time_stamp reflow_timer={0,6,0};

const uint8_t total_counts = 64; // period
static OvenState_E oven_status = Oven_Idle;
static SProfile_Profile temperatureProfile;

OvenState_E OvenCntl_getOvenState(void)
{
    return oven_status;
}


void compensator(int setpoint_temp, int current_temp)
{
	static uint8_t current_counts = 0;
	static uint8_t time_on_counts = 10; //time on out of period, initial value set
	uint8_t local_setpoint = setpoint_temp;
	uint8_t local_ctemp = current_temp;
	int set_on_counts = 0;
	uint8_t drive_strength = 1;

	drive_strength = (local_setpoint - local_ctemp);

	if (drive_strength < 3) {
		drive_strength = 3;
	} else if (drive_strength > total_counts) {
		drive_strength = total_counts;
	}

	set_on_counts = drive_strength;

	if (current_counts < 1) {
		current_counts = total_counts;
		time_on_counts = set_on_counts;
	}

	if (time_on_counts) // If time_on_counts = non zero, decrement and turn on output
	{
		OVEN_ON;
		time_on_counts--;
	} else {
		OVEN_OFF;
	}

	current_counts--;

}

#define min_count		0
#define max_count		64
#define gain			4

void new_compensator(uint8_t current_temp, uint8_t set_point)
{
	int16_t error =gain*(set_point-current_temp);
	static unsigned int cycle_counts=max_count;
	static unsigned int time_on=max_count;

	if(error<min_count)
	{
		error=min_count;	// Is error smaller than min counts?
	}
	if(error>max_count)
	{
		error=max_count; 	// Is error larger than max counts?
	}
	//if(error>error_previous)
	//{
	//	time_on+= error - error_previous;	// If error has changed from previous call of function
	//}

	if(time_on>1)
	{
		OVEN_ON;
		time_on--;
		if(time_on<1)
		time_on=1;
	}
	else
		OVEN_OFF;

	if(cycle_counts<1)				// if end of cycle, refresh duty cycle
	{
		cycle_counts+=max_count;
		time_on=error;
	}


	cycle_counts--; // End of cycle

}

// TODO: Make this return a SUCCESS / FAILURE enum
uint8_t start_oven(TemperatureProfile_S tempProfile)
{
	if((oven_status == Oven_Idle)
	    && (tempProfile.profile != NULL))
	{
		reflow_timer.minutes=6;
		reflow_timer.secs=0;
		oven_status=Oven_Reflowing;
		secondsIntoProfile=0;

		temperatureProfile = SProfile_createProfile( tempProfile.profile, tempProfile.standardProfileDuration, get_temp());
		secondsLeftInProfile = SProfile_getProfileDuration(&temperatureProfile);
		return 1;
	}
	else
	{
		return 0;
	}
}

void stop_oven()
{
	oven_status=Oven_Cooldown;
	OVEN_OFF;
}

void stepOvenStateMachine()
{
	uint16_t oven_temp = get_temp();;

	if((oven_temp>50) & (oven_status==Oven_Idle))
	{
		oven_status = Oven_Cooldown;
	}

	if(oven_temp > 300)
	{
		oven_status = Oven_Error;
		OVEN_OFF;
	}
	else if((oven_status==Oven_Alarm) || (oven_status==Oven_Reflowing))
	{
		if(count_down_time(&reflow_timer))
		{
			secondsIntoProfile++;

			if(secondsLeftInProfile > 0)
			{
			    secondsLeftInProfile--;
			}
		}

		uint8_t targetTemp = SProfile_getTemperatureAtTime(&temperatureProfile, secondsIntoProfile);
        new_compensator( oven_temp, targetTemp );

        if(secondsLeftInProfile <= 2)
        {
            oven_status = Oven_Alarm;
        }
        else if(secondsLeftInProfile <= 0)
		{
			oven_status = Oven_Cooldown;
			OVEN_OFF;
			reflow_timer.secs = 0;
		}
	}
	else if((oven_status==Oven_Cooldown) && (oven_temp<50))
	{
		oven_status = Oven_Idle;
	}

}

void get_reflow_time_string(char * buffer)
{
	get_time_short(buffer,&reflow_timer); // format hours:mins:secs
}

uint8_t get_reflow_time_minutes()
{
	return reflow_timer.minutes;
}

uint8_t get_reflow_time_seconds()
{
	return reflow_timer.secs;
}

