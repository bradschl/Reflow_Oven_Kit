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

#include "profile_lead.h"
#include "profile_pb_free.h"

static const TemperatureProfile_S profiles[] =
{
    {"Lead",        lead_profile,       LEAD_PROFILE_LENGTH,        260},
    {"PB Free",     lead_free_profile,  LEAD_FREE_PROFILE_LENGTH,   270},

    // This must be the last entry in the table
    {NULL,          NULL,               0,                          0}
};

const TemperatureProfile_S* TempProfile_getProfiles(void)
{
    return profiles;
}


