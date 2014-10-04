/*
 * TempProfiles.c
 *
 *  Created on: Oct 4, 2014
 *      Author: user
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "TempProfiles.h"

#include "Oven/profile_lead.h"
#include "Oven/profile_pb_free.h"

static const TemperatureProfile_S profiles[] =
{
        {PB_REFLOW_PROFILE,         lead_profile,       360},
        {PB_FREE_REFLOW_PROFILE,    lead_free_profile,  360},

        // This must be the last entry in the table
        {NULL_PROFILE,              NULL,               0}
};

TemperatureProfile_S TempProfile_getProfile(TempProfile_E profile)
{
    size_t i;
    for(i = 0; profiles[i].profile != NULL; i++)
    {
        if(profiles[i].profileName == profile)
        {
            return profiles[i];
        }
    }

    // Didn't find it
    TemperatureProfile_S null_profile = {NULL_PROFILE, NULL, 0};
    return null_profile;
}


