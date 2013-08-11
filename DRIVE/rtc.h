/*
 * rtc.h
 *
 *  Created on: Apr 6, 2013
 *      Author: Nate
 */

#ifndef RTC_H_
#define RTC_H_

#include "stdint.h"

#include "Oven/oven_control.h"
#include "Oven/oven.h"
#include "Oven/profile_lead.h"
#include "Oven/profile_pb_free.h"
#include "OS/time.h"

/******************************************************************************
 * Defines
******************************************************************************/
#define SYSTEM_TIME_12H
#define MAX_CPU_TICK_TIME_MS 	1000

/******************************************************************************
 * Global Variables
******************************************************************************/


/******************************************************************************
 * Functions
******************************************************************************/
uint16_t getTaskTick_ms(void);

const time_stamp* getSystemTime(void);
uint8_t get_system_seconds();
uint8_t get_system_minutes();
uint8_t get_system_hours();

void set_system_seconds(uint8_t secs);
void set_system_minutes(uint8_t minutes);
void set_system_hours(uint8_t hours);

void get_system_time_string(char * buffer);
void setupRTC();


#endif /* RTC_H_ */
