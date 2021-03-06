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
#include <stddef.h>

#include "ScalableProfile/SProfile.h"
#include "TempProfiles.h"

/*************************************************************************************************
 * Macros and Defines
 *************************************************************************************************/

/*************************************************************************************************
 * Typedefs, structures, and enumerations
 *************************************************************************************************/

/*************************************************************************************************
 * Variables
 *************************************************************************************************/
static const SProfile_DataPoint leadProfile[] = {// Ramp time, target temp, next profile point
                                                        {70,     140,    leadProfile + 1}, // Pre-heat
                                                        {110,    160,    leadProfile + 2}, // Soak
                                                        {50,     220,    leadProfile + 3}, // Ramp
                                                        {30,     220,    leadProfile + 4}, // Reflow
                                                        {1,      0,      NULL}             // Cool down
                                                       };

static const SProfile_DataPoint pbFreeProfile[] = {// Ramp time, target temp, next profile point
                                                          {70,   140,    pbFreeProfile + 1}, // Pre-heat
                                                          {110,  180,    pbFreeProfile + 2}, // Soak
                                                          {80,   240,    pbFreeProfile + 3}, // Ramp
                                                          {10,   240,    pbFreeProfile + 4}, // Reflow
                                                          {1,    0,      NULL}               // Cool down
                                                         };

// Table of all profiles to hand to expose
static const TemperatureProfile_S profiles[] =
{
    {"Lead",        leadProfile,        261},
    {"PB Free",     pbFreeProfile,      271},

    // This must be the last entry in the table
    {NULL,          NULL,               0}
};

/*************************************************************************************************
 * Functions
 *************************************************************************************************/

const TemperatureProfile_S* TempProfile_getProfiles(void)
{
    return profiles;
}


