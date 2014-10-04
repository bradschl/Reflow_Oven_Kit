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

typedef struct
{
    // String name of the profile
    const char* profileName;

    // Profile data temperature points
    const Temperature_T* points;

    // Length of profile
    Seconds_T profileLength;

    // Time to issue the alarm at
    Seconds_T alarmPoint;
} TemperatureProfile_S;


/*************************************************************************************************
 * Functions
 *************************************************************************************************/
const TemperatureProfile_S* TempProfile_getProfiles(void);

#endif /* TEMPCURVE_H_ */
