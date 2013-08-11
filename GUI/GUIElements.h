/*
 * GUIElements.h
 *
 *  Created on: Aug 10, 2013
 *      Author: user
 */

#ifndef GUIELEMENTS_H_
#define GUIELEMENTS_H_

#include <stdint.h>
#include "GUIEvents.h"

/******************************************************************************
 * Defines
******************************************************************************/

/**
 * Prototype function pointer for event handling functions
 */
typedef void (*pfnElementtHandler)(GUIEvent_E, GUIAction_E);

/**
 * Pixel type. Can be changed here for larger screens if needed
 */
typedef uint8_t pixel_t;
#define PIXEL_INVALID   UINT8_MAX

typedef struct TextButton_S
{
    GUIEvent_E event;
    pixel_t xStart;
    pixel_t yStart;
    pixel_t xSize;
    pixel_t ySize;
    const char* pText;
} TextButton_S;

/**
 * Custom element handler. Use this for generating things like text elements,
 * click-able regions, or custom graphic plots.
 */
typedef struct
{
    GUIEvent_E event;
    pixel_t xStart;
    pixel_t yStart;
    pixel_t ySize;
    pixel_t xSize;
    pfnElementtHandler pfnHandler;
} CustomElement_S;


/******************************************************************************
 * Global Variables
******************************************************************************/


/******************************************************************************
 * Functions
******************************************************************************/

#endif /* GUIELEMENTS_H_ */










