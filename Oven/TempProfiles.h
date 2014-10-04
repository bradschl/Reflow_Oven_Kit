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

/*************************************************************************************************
 * Macros and Defines
 *************************************************************************************************/

/*************************************************************************************************
 * Typedefs, structures, and enumerations
 *************************************************************************************************/
typedef uint8_t Temperature_T;
typedef uint16_t Seconds_T;

typedef enum
{
    PB_REFLOW_PROFILE,
    PB_FREE_REFLOW_PROFILE,
    NULL_PROFILE
} TempProfile_E;

typedef struct
{
    // Enumeration handle
    TempProfile_E profileName;

    // Profile data points
    const Temperature_T* profile;

    // Length of profile
    Seconds_T profileLength;
} TemperatureProfile_S;


/*************************************************************************************************
 * Functions
 *************************************************************************************************/
TemperatureProfile_S TempProfile_getProfile(TempProfile_E profile);

#endif /* TEMPCURVE_H_ */
