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
} TextButton_Element_S;

typedef struct
{
    CustomElement_S* pCustomElement;
    bool fRedraw;
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

// Clear the screen on next redraw event
static bool fClearScreen = true;

// Lockout for touch debouncing
static touch_t touchLockoutX = 0;
static touch_t touchLockoutY = 0;

/******************************************************************************
 * Functions
******************************************************************************/
static void createTouchEvents(uint16_t xTouch, uint16_t yTouch);
static bool isInRegion( touch_t xTouch, touch_t yTouch, pixel_t xStart,
                        pixel_t yStart, pixel_t xSize, pixel_t ySize);
static void queueEvent(GUIEvent_E event, GUIAction_E action);
static void redrawMarkedElements(void);
static void drawTextButton(TextButton_S* pTextButton);


/**
 * Sets up the GUI manager so that it can manage an event driven interface.
 */
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

void addGUIObserver(pfnElementtHandler pfnObserver)
{
    uint8_t i;

    if(pfnObserver == NULL)
    {
        return;
    }

    i = 0;
    while(i < GUI_MAX_OBSERVERS)
    {
        if(scpfnEventObservers[i] == NULL)
        {
            // Found an empty spot
            scpfnEventObservers[i] = pfnObserver;
            break;
        }

        i++;
    }
}


/**
 * Remove all active GUI elements and clear the screen.
 */
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

    fClearScreen = true;
}


/**
 * Step the GUI manager task state machine
 */
void stepGUIManagerTask(void)
{
    uint16_t xTouch, yTouch;

    // Get the current touch location value
    TOUCH_VAL(&xTouch, &yTouch);

    //if((xTouch < LCD_WIDTH) && (yTouch < LCD_HEIGHT))
    if(!isInRegion(xTouch, yTouch,touchLockoutX, touchLockoutY,0,0))
    {
        createTouchEvents(xTouch, yTouch);
        touchLockoutX = xTouch;
        touchLockoutY = yTouch;
    }

    // Handle all redraw requests
    redrawMarkedElements();
}


/**
 * Periodic GUI event publisher. Can be run in the background or on a tick
 */
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
            break;
        }

        i++;
    }
}


void removeGUITextButton(TextButton_S* pTextButton)
{
    uint8_t i;
    pixel_t xEnd;
    pixel_t yEnd;

    if(pTextButton == NULL)
    {
        return;
    }

    i = 0;
    while(i < GUI_MAX_TEXT_BUTTONS)
    {
        if(scTextButtonElements[i].pTextButton == pTextButton)
        {
            // Remove from list
            scTextButtonElements[i].pTextButton = NULL;

            // Overwrite on display
            xEnd = (pTextButton->xStart) + (pTextButton->xSize);
            yEnd = (pTextButton->yStart) + (pTextButton->ySize);
            setColor16(COLOR_16_BLACK);
            fillRect(pTextButton->xStart, pTextButton->yStart, xEnd, yEnd);

            break;
        }

        i++;
    }
}


void removeGUICustomElement(CustomElement_S* pCustonElement)
{
    uint8_t i;
    pixel_t xEnd;
    pixel_t yEnd;

    if(pCustonElement == NULL)
    {
        return;
    }

    i = 0;
    while(i < GUI_MAX_CUSTOM_ELEMENTS)
    {
        if(scCustomElements[i].pCustomElement == pCustonElement)
        {
            // Remove from list
            scCustomElements[i].pCustomElement = NULL;

            // Overwrite on display
            xEnd = pCustonElement->xStart + pCustonElement->xSize;
            yEnd = pCustonElement->yStart + pCustonElement->ySize;
            setColor16(COLOR_16_BLACK);
            fillRect(pCustonElement->xStart, pCustonElement->yStart, xEnd, yEnd);
        }

        i++;
    }
}


void addGUICustomElement(CustomElement_S* pCustomElement)
{
    uint8_t i;

    i = 0;
    while(i < GUI_MAX_CUSTOM_ELEMENTS)
    {
        if(scCustomElements[i].pCustomElement == NULL)
        {
            // Found an empty spot
            scCustomElements[i].pCustomElement = pCustomElement;
            scCustomElements[i].fRedraw = true;
            break;
        }

        i++;
    }
}

void requestGUIRedrawCustom(CustomElement_S* pCustomElement)
{
    uint8_t i;

    i = 0;
    while(i < GUI_MAX_CUSTOM_ELEMENTS)
    {
        if(scCustomElements[i].pCustomElement == pCustomElement)
        {
            scCustomElements[i].fRedraw = true;
            break;
        }

        i++;
    }
}


void requestGUIRedrawTextButton(TextButton_S* pTextButton)
{
    uint8_t i;

    i = 0;
    while(i < GUI_MAX_TEXT_BUTTONS)
    {
        if(scTextButtonElements[i].pTextButton == pTextButton)
        {
            scTextButtonElements[i].fRedraw = true;
            break;
        }

        i++;
    }
}

static void createTouchEvents(uint16_t xTouch, uint16_t yTouch)
{
    uint8_t i;
    bool collision;
    TextButton_S* pTextButton;
    CustomElement_S* pCustomElement;

    // Iterate through all known display elements and check for collision
    for(i = 0; i < GUI_MAX_TEXT_BUTTONS; i++)
    {
        pTextButton = scTextButtonElements[i].pTextButton;
        if(pTextButton != NULL)
        {
            collision = isInRegion(xTouch, yTouch, pTextButton->xStart,  pTextButton->yStart,
                                   pTextButton->xSize, pTextButton->ySize);
            if(collision)
            {
                // Generate event
                queueEvent(pTextButton->event, ACTION_Clicked);
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
                                   pCustomElement->xSize, pCustomElement->ySize);
            if(collision)
            {
                // Generate event
                queueEvent(pCustomElement->event, ACTION_Clicked);
            }

            if(fClearScreen)
            {
                scCustomElements[i].fRedraw = true;
            }
        }
    }
}

static bool isInRegion( touch_t xTouch, touch_t yTouch, pixel_t xStart,
                        pixel_t yStart, pixel_t xSize, pixel_t ySize)
{
    bool ret = false;
    if(((xTouch + TOUCH_TOLERANCE) > xStart) && (xTouch < (xStart + xSize + TOUCH_TOLERANCE)))
    {
        if(((yTouch + TOUCH_TOLERANCE) > yStart) && (yTouch < (yStart + ySize + TOUCH_TOLERANCE)))
        {
            ret = true;
        }
    }

    return ret;
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
    uint16_t bg_color = COLOR_16_RED;
    pixel_t xEnd = (pTextButton->xStart) + (pTextButton->xSize);
    pixel_t yEnd = (pTextButton->yStart) + (pTextButton->ySize);

    setColor16(bg_color);
    fillRect(pTextButton->xStart, pTextButton->yStart, xEnd, yEnd);

    setColor16(COLOR_16_WHITE);
    setBackgroundColor16(bg_color);
    setColor16(COLOR_16_WHITE);
    drawString8_12(pTextButton->xStart+5, pTextButton->yStart + 5, pTextButton->pText );
}






