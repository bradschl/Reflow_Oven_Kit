/*
 * GUIManager.c
 *
 *  Created on: Aug 10, 2013
 *      Author: user
 */

#include <stdbool.h>
#include "System.h"
#include "GUI/GUIEvents.h"
#include "GUI/GUIManager.h"
#include "LCD/touch.h"

/******************************************************************************
 * Defines
******************************************************************************/

#define TOUCH_TOLERANCE     5

typedef struct
{
    TextButton_S* pTextButton;
    bool fRedraw;
    bool fTouchActive;
} TextButton_Element_S;

typedef struct
{
    CustomElement_S* pCustomElement;
    bool fRedraw;
    bool fTouchActive;
} CustomElement_Element_S;

typedef struct
{
    GUIEvent_E event;
    GUIAction_E action;
} EventElement_S;

/******************************************************************************
 * Static Variables
******************************************************************************/
static TextButton_Element_S scTextButtonElements[GUI_MAX_TEXT_BUTTONS];
static CustomElement_Element_S scCustomElements[GUI_MAX_CUSTOM_ELEMENTS];
static EventElement_S scEventQueue[GUI_EVENT_QUEUE_SIZE];
static pfnElementtHandler scpfnEventObservers[GUI_MAX_OBSERVERS];
static bool fClearScreen = true;

/******************************************************************************
 * Functions
******************************************************************************/
static bool isInRegion( touch_t xTouch, touch_t yTouch, pixel_t xStart,
                        pixel_t yStart, pixel_t xSize, pixel_t ySize);
static void queueEvent(GUIEvent_E event, GUIAction_E action);
static void redrawMarkedElements(void);
static void drawTextButton(TextButton_S* pTextButton);


void setupGUIManager(void)
{
    uint8_t i;

    clearAllGUIElements();

    for(i = 0; i < GUI_EVENT_QUEUE_SIZE; i++)
    {
        scEventQueue[i].event = EVENT_NoEvent;
    }

    for(i = 0; i < GUI_MAX_OBSERVERS; i++)
    {
        scpfnEventObservers[i] = NULL;
    }
}

void clearAllGUIElements(void)
{
    uint8_t i;

    for(i = 0; i < GUI_MAX_TEXT_BUTTONS; i++)
    {
        scTextButtonElements[i].pTextButton = NULL;
    }

    for(i = 0; i < GUI_MAX_CUSTOM_ELEMENTS; i++)
    {
        scCustomElements[i].pCustomElement = NULL;
    }
}

void stepGUIManagerTask(void)
{
    uint8_t i;
    uint16_t xTouch, yTouch;
    bool collision;
    TextButton_S* pTextButton;
    CustomElement_S* pCustomElement;

    // Get the current touch location value
    TOUCH_VAL(&xTouch, &yTouch);

    // Iterate through all known display elements and check for collision
    for(i = 0; i < GUI_MAX_TEXT_BUTTONS; i++)
    {
        pTextButton = scTextButtonElements[i].pTextButton;
        if(pTextButton != NULL)
        {
            collision = isInRegion(xTouch, yTouch, pTextButton->xStart,  pTextButton->yStart,
                                   pTextButton->xStart + pTextButton->xSize,
                                   pTextButton->yStart + pTextButton->ySize);

            if(collision && (!scTextButtonElements[i].fTouchActive))
            {
                // Pressed but not released
                scTextButtonElements[i].fTouchActive = true;
            }
            else if((!collision) && (scTextButtonElements[i].fTouchActive))
            {
                // Released, generate event
                queueEvent(pTextButton->event, ACTION_Clicked);
                scTextButtonElements[i].fTouchActive = false;
            }

            if(fClearScreen)
            {
                scTextButtonElements[i].fRedraw = true;
            }
        }
    }

    for(i = 0; i < GUI_MAX_CUSTOM_ELEMENTS; i++)
    {
        pCustomElement = scCustomElements[i].pCustomElement;
        if(pCustomElement != NULL)
        {
            collision = isInRegion(xTouch, yTouch, pCustomElement->xStart,  pCustomElement->yStart,
                                   pCustomElement->xStart + pCustomElement->xSize,
                                   pCustomElement->yStart + pCustomElement->ySize);
            if(collision && (!scCustomElements[i].fTouchActive))
            {
                // Pressed but not released
                scCustomElements[i].fTouchActive = true;
            }
            else if((!collision) && (scCustomElements[i].fTouchActive))
            {
                // Released, generate event
                queueEvent(pCustomElement->event, ACTION_Clicked);
                scCustomElements[i].fTouchActive = false;
            }

            if(fClearScreen)
            {
                scCustomElements[i].fRedraw = true;
            }
        }
    }

    // Handle all redraw requests
    redrawMarkedElements();
}

static bool isInRegion( touch_t xTouch, touch_t yTouch, pixel_t xStart,
                        pixel_t yStart, pixel_t xSize, pixel_t ySize)
{
    if((xTouch >= (xStart - TOUCH_TOLERANCE) && (xTouch <= (xStart + xSize + TOUCH_TOLERANCE))))
    {
        if((yTouch >= (yStart - TOUCH_TOLERANCE) && (yTouch <= (yStart + ySize + TOUCH_TOLERANCE))))
        {
            return true;
        }
    }

    return false;
}

void publishGUIEvents(void)
{
    uint8_t i,j;
    GUIAction_E action;
    GUIEvent_E event;

    for(i = 0; i < GUI_EVENT_QUEUE_SIZE; i++)
    {
        if(scEventQueue[i].event != EVENT_NoEvent)
        {
            event = scEventQueue[i].event;
            action = scEventQueue[i].action;
            for(j = 0; j < GUI_MAX_OBSERVERS; j++)
            {
                if(scpfnEventObservers[j] != NULL)
                {
                    (scpfnEventObservers[j])(event, action);
                }
            }

            // Clear the event
            scEventQueue[i].event = EVENT_NoEvent;
        }
    }

}

static void queueEvent(GUIEvent_E event, GUIAction_E action)
{
    uint8_t i = 0;

    while(i < GUI_EVENT_QUEUE_SIZE)
    {
        if(scEventQueue[i].event == EVENT_NoEvent)
        {
            // Free spot, use it;
            scEventQueue[i].event = event;
            scEventQueue[i].action = action;
            break;
        }

        i++;
    }

}

void addGUIButtionElement(TextButton_S* textButton)
{
    uint8_t i = 0;
    TextButton_S* pTextButton;

    while(i < GUI_MAX_TEXT_BUTTONS)
    {
        pTextButton = scTextButtonElements[i].pTextButton;

        if(pTextButton == NULL)
        {
            // free spot
            scTextButtonElements[i].pTextButton = textButton;
            scTextButtonElements[i].fRedraw = true;
            scTextButtonElements[i].fTouchActive = false;
            break;
        }

        i++;
    }
}

static void redrawMarkedElements(void)
{
    uint8_t i;
    TextButton_S* pTextButton;
    CustomElement_S* pCustomElement;

    if(fClearScreen)
    {
        clearScreen(1);
        fClearScreen = false;
    }

    i = 0;
    while(i < GUI_MAX_TEXT_BUTTONS)
    {
        if(scTextButtonElements[i].fRedraw)
        {
            pTextButton = scTextButtonElements[i].pTextButton;
            if(pTextButton != NULL)
            {
                drawTextButton(pTextButton);
            }

            scTextButtonElements[i].fRedraw = false;
        }

        i++;
    }

    i = 0;
    while(i < GUI_MAX_CUSTOM_ELEMENTS)
    {
        if(scCustomElements[i].fRedraw)
        {
            pCustomElement = scCustomElements[i].pCustomElement;
            if(pCustomElement != NULL)
            {
                // Issue redraw command to handler function
                (pCustomElement->pfnHandler)(pCustomElement->event, ACTION_Redraw);
            }

            scCustomElements[i].fRedraw = false;
        }

        i++;
    }

}

static void drawTextButton(TextButton_S* pTextButton)
{
    // TODO: stub function
    uint16_t bg_color = COLOR_16_RED;
    pixel_t xEnd = (pTextButton->xStart) + (pTextButton->xSize);
    pixel_t yEnd = (pTextButton->yStart) + (pTextButton->ySize);

    setColor16(bg_color);
    fillRect(pTextButton->xStart, pTextButton->yStart, xEnd, yEnd);

    setColor16(COLOR_16_WHITE);
    setBackgroundColor16(bg_color);
    setColor16(COLOR_16_WHITE);
    drawString8_12(pTextButton->xStart+5, pTextButton->yStart + 5, pTextButton->pText );
    //setBackgroundColor16(COLOR_16_BLACK);
}

