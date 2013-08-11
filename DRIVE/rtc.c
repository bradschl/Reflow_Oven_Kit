/*
 * rtc.c
 *
 *  Created on: Apr 6, 2013
 *      Author: Nate
 */


#include "System.h"
#include "DRIVE/rtc.h"
#include "DRIVE/timer.h"

/******************************************************************************
 * Defines
******************************************************************************/
#ifndef SYSTEM_TIME_12H
	#define SYSTEM_TIME_24H
	#define HOUR_LIMIT 24
#else
	#define HOUR_LIMIT 12
#endif

#ifndef CPU_FREQUENCY_HZ
	#error "CPU frequency is not defined!"
#endif

#define WDT_TIMER_PERIOD_US 	(512*1000000)/CPU_FREQUENCY_HZ


/******************************************************************************
 * Static Variables
******************************************************************************/
static uint16_t cpuTick_ms = 0;
static time_stamp system_time={0,0,0};


/******************************************************************************
 * Functions
******************************************************************************/
static void incrementSystemTime_FromISR(uint16_t period_us);
static void incrementTaskTick_FromISR(uint16_t period_us);

void setupRTC()
{
	WDTCTL = WDT_MDLY_0_5;	// Interval timer, SMCLK / 512 --> 32us
	IE1 |= WDTIE;         	// Enable WDT interrupt
}


/**
 * Watchdog timer ISR
 */
#pragma vector=WDT_VECTOR
__interrupt void watchdog_timer(void)
{

	incrementSystemTime_FromISR(WDT_TIMER_PERIOD_US);
	incrementTaskTick_FromISR(WDT_TIMER_PERIOD_US);

}

/**
 * Increments the system time.
 *
 * @param period_us Time (in us) to increment system time
 */
static void incrementSystemTime_FromISR(uint16_t period_us)
{
	static uint32_t cpuTime_us = 0;

	cpuTime_us += period_us;

	if(cpuTime_us > 1000000)
	{
		cpuTime_us -= 1000000;
		system_time.secs++;
		if(system_time.secs > 59)
		{
			system_time.secs = 0;
			system_time.minutes++;
			if(system_time.minutes > 59)
			{
				system_time.minutes = 0;
				system_time.hours++;
				while(system_time.hours > HOUR_LIMIT)
				{
					system_time.hours -= HOUR_LIMIT;
				}
			}
		}
	}
}

/**
 * Increments the system task tick counter.
 */
static void incrementTaskTick_FromISR(uint16_t period_us)
{
	static uint16_t cpuTime_us = 0;

	cpuTime_us += period_us;

	if(cpuTime_us > 1000)
	{
		// 1ms has passed
		cpuTime_us -= 1000;
		cpuTick_ms++;
		if(cpuTick_ms >= MAX_CPU_TICK_TIME_MS)
		{
			cpuTick_ms -= MAX_CPU_TICK_TIME_MS;
		}
	}

}

/**
 * Gets the millisecond task tick. Note that this number rolls over at MAX_CPU_TICK_TIME_MS
 */
uint16_t getTaskTick_ms(void)
{
	return cpuTick_ms;
}

const time_stamp* getSystemTime(void)
{
	return &system_time;
}

uint8_t get_system_seconds()
{
	return system_time.secs;
}

uint8_t get_system_minutes()
{
	return system_time.minutes;
}

uint8_t get_system_hours()
{
	return system_time.hours;
}

void set_system_seconds(uint8_t secs)
{
	system_time.secs=secs;
}

void set_system_minutes(uint8_t minutes)
{
	system_time.minutes=minutes;
}

void set_system_hours(uint8_t hours)
{
	system_time.hours=hours;
}

void get_system_time_string(char * buffer)
{
	get_time_long(buffer,&system_time); // format hours:mins:secs
}



