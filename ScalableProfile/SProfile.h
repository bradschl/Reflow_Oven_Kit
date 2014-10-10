/*
 * Copyright (c) 2014, Bradley Schleusner
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SPROFILE_H_
#define SPROFILE_H_

/*************************************************************************************************
 * Include Files
 *************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*************************************************************************************************
 * Macros and Defines
 *************************************************************************************************/

/*************************************************************************************************
 * Typedefs, structures, and enumerations
 *************************************************************************************************/
// Temperature storage type
typedef uint8_t     STemperature_T;

// Vectored time storage type
typedef uint8_t     SRelativeTime_T;

// Time storage type for time after devectoring
typedef uint16_t    SRealTime_T;


typedef struct
{
    // Arbitrary ramp time to reach the target temp
    SRelativeTime_T rampTime;
    // Temperature to reach at the end of the ramp time
    STemperature_T  targetTemp;
    // Pointer to the next VProfile_Point in the profile
    const void*     nextProfilePoint;
} SProfile_DataPoint;


typedef struct
{
    // Pointer to the root of the profile
    const SProfile_DataPoint* profileRoot;

    // Duration of the profile (real time after scaling)
    SRealTime_T profileDuration;

    // Duration of the profile (vector time)
    SRealTime_T profileVectorDuration;

    // Temperature to start the profile at
    STemperature_T startingTemp;
} SProfile_Profile;

/*************************************************************************************************
 * Variables
 *************************************************************************************************/

/*************************************************************************************************
 * Functions
 *************************************************************************************************/
/**
 * Creates scaled real time temperature profile out of a relative time profile
 *
 * @param profileRoot Pointer to the root node of the scalable profile points. Last profile must have nextProfilePoint=NULL
 * @param profileDuration Duration to scale the vector profile to
 * @param startingTemp Temperature to start the profile at (ambient starting)
 * @return Control structure for using the vector profile
 */
SProfile_Profile SProfile_createProfile(const SProfile_DataPoint* profileRoot, SRealTime_T profileDuration, STemperature_T startingTemp);


/**
 * Gets the temperature for a given point in the profile
 *
 * @param profile Profile to get the temperature from
 * @param t Time to find the temperature at
 * @return Temperature for the given time into the profile. Returns the last temperature in the profile if
 *         t is beyond the profile duration.
 */
STemperature_T SProfile_getTemperatureAtTime(const SProfile_Profile* profile, SRealTime_T t);


/**
 * Gets the duration of the profile (real time after scaling)
 *
 * @param profile Profile to get the information about
 * @return Real time duration of the profile (not vector time)
 */
bool SProfile_getProfileDuration(const SProfile_Profile* profile);


#endif /* SPROFILE_H_ */
