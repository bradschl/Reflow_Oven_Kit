/*
 * oven_control.h
 *
 *  Created on: Apr 13, 2013
 *      Author: Nate
 */

#ifndef OVEN_CONTROL_H_
#define OVEN_CONTROL_H_

#include <stdint.h>
#include <stdbool.h>

#include "Profiles/TempProfiles.h"

typedef enum
{
    Oven_Idle,
    Oven_Reflowing,
    Oven_Alarm,
    Oven_Cooldown,
    Oven_Error
} OvenState_E;

uint8_t start_oven(TemperatureProfile_S tempProfile);
void stop_oven();

void stepOvenStateMachine();

void get_reflow_time_string(char * buffer);
uint8_t get_reflow_time_seconds();
uint8_t get_reflow_time_minutes();

OvenState_E OvenCntl_getOvenState(void);

#endif /* OVEN_CONTROL_H_ */
