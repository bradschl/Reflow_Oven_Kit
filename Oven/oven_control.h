/*
 * oven_control.h
 *
 *  Created on: Apr 13, 2013
 *      Author: Nate
 */

#ifndef OVEN_CONTROL_H_
#define OVEN_CONTROL_H_

#include "stdint.h"

void get_reflow_time_string(char * buffer);
void stepOvenStateMachine();
void stop_oven();
uint8_t start_oven();

void set_profile_leaded();
void set_profile_pb_free();
uint8_t check_for_lead_profile();


uint8_t get_reflow_time_seconds();
uint8_t get_reflow_time_minutes();

typedef enum
{
	Oven_Off,
	Oven_Reflowing,
	Oven_Alarm,
	Oven_Cooldown,
	Oven_Idle,
	Oven_Error
} OvenState_E;

OvenState_E OvenCntl_getOvenState(void);

#endif /* OVEN_CONTROL_H_ */
