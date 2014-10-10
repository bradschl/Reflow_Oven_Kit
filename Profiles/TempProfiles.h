/*
 * TempCurve.h
 *
 *  Created on: Oct 4, 2014
 *      Author: user
 */

#ifndef TEMPCURVE_H_
#define TEMPCURVE_H_

/*************************************************************************************************
 * Include Files
 *************************************************************************************************/
#include <stdint.h>

#include "ScalableProfile/SProfile.h"

/*************************************************************************************************
 * Macros and Defines
 *************************************************************************************************/

/*************************************************************************************************
 * Typedefs, structures, and enumerations
 *************************************************************************************************/
typedef uint8_t Temperature_T;
typedef uint16_t Seconds_T;

typedef struct
{
    // String name of the profile
    const char* name;

    // Scalable profile data points
    const SProfile_DataPoint* profile;

    // Recommended duration of the profile
    Seconds_T standardProfileDuration;
} TemperatureProfile_S;


/*************************************************************************************************
 * Functions
 *************************************************************************************************/
const TemperatureProfile_S* TempProfile_getProfiles(void);

#endif /* TEMPCURVE_H_ */
