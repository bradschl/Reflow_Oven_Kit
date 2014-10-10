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


/*************************************************************************************************
 * Include Files
 *************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "SProfile.h"

/*************************************************************************************************
 * Macros and Defines
 *************************************************************************************************/

/*************************************************************************************************
 * Typedefs, structures, and enumerations
 *************************************************************************************************/
// Type to use for fractional or large math operations. This can be an integer type, but never unsigned
typedef float Fractional_T;

/*************************************************************************************************
 * Variables
 *************************************************************************************************/

/*************************************************************************************************
 * Functions
 *************************************************************************************************/

SProfile_Profile SProfile_createProfile(const SProfile_DataPoint* profileRoot, SRealTime_T profileDuration, STemperature_T startingTemp)
{
    SProfile_Profile profile;
    profile.profileDuration         = 0;
    profile.profileRoot             = profileRoot;
    profile.profileVectorDuration   = 0;
    profile.startingTemp = startingTemp;

    if(profileRoot == NULL)
    {
        return profile;
    }

    // Find vector duration
    const SProfile_DataPoint* vPoint = profileRoot;
    for( ; vPoint != NULL; vPoint = (SProfile_DataPoint*)vPoint->nextProfilePoint)
    {
        profile.profileVectorDuration += vPoint->rampTime;
    }

    // If the vector profile is of length 0, then there is nothing we can scale to
    if(profile.profileVectorDuration > 0)
    {
        profile.profileDuration = profileDuration;
    }

    return profile;
}


bool SProfile_getProfileDuration(const SProfile_Profile* profile)
{
    if(profile != NULL)
    {
        return profile->profileDuration;
    }
    else
    {
        return 0;
    }
}


STemperature_T SProfile_getTemperatureAtTime(const SProfile_Profile* profile, SRealTime_T t)
{
    if(profile == NULL)
    {
        return 0;
    }

    STemperature_T Tstart = profile->startingTemp;
    STemperature_T Tstop = profile->startingTemp;;

    const SProfile_DataPoint* point = profile->profileRoot;
    for( ; point != NULL; point = (SProfile_DataPoint*)point->nextProfilePoint)
    {
        Tstop = point->targetTemp;

        if(t >= point->rampTime)
        {
            // Target time is not in this range, move on to the next vector
            t -= point->rampTime;
            Tstart = point->targetTemp;
        }
        else
        {
            // Target time is in this time range
            if(point->rampTime > 0)
            {
                return (STemperature_T) Tstart + (((Fractional_T) Tstop) - Tstart) * (((Fractional_T) t) / point->rampTime);
            }
        }
    }

    // Past profile length, return last good temp
    return Tstop;
}












