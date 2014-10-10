/*
 * ReflowGUI.c
 *
 *  Created on: Aug 10, 2013
 *      Author: user
 */

#include <stdio.h>
#include <stdbool.h>
#include "GUI/GUIManager.h"
#include "GUI/ReflowGUI.h"
#include "Oven/oven.h"
#include "Oven/calibration.h"
#include "Oven/oven_control.h"
#include "Profiles/TempProfiles.h"

/******************************************************************************
 * Defines
******************************************************************************/
#define LARGE_BUTTON_X_SIZE 70
#define LARGE_BUTTON_Y_SIZE 20

typedef enum
{
    PAGE_Home,
    PAGE_Reflow,
    PAGE_Settings,
    PAGE_Time,
    PAGE_Calibration
} ReflowPages_E;

typedef enum
{
    REFLOW_Init,
    REFLOW_Idle,
    REFLOW_StartReflow,
    REFLOW_Reflowing,
    REFLOW_Cooldown,
    REFLOW_GoIdle
} ReflowPageState_E;

/******************************************************************************
 * Static Variables
******************************************************************************/

// Multi-page
static void multi_PageTitleHandler(GUIEvent_E event, GUIAction_E action);
static const CustomElement_S multi_PageTitle = {EVENT_MultiPageTitle, 0, 0, 219, 30, multi_PageTitleHandler};
static const TextButton_S multi_BackButton = {EVENT_MultiBackButton, 70, 150, 42, 20, "Back"};

// Home Page
static void home_CustomElementsHandler(GUIEvent_E event, GUIAction_E action);
static const CustomElement_S home_CustomElements = {EVENT_HomeCustomElements, 0, 0, 0, 0, home_CustomElementsHandler};
static       TextButton_S home_profileButton = {EVENT_HomeProfileButton, 10, 60,
                                           LARGE_BUTTON_X_SIZE, LARGE_BUTTON_Y_SIZE, ""}; // Text gets set later
static const TextButton_S home_nextProfileButton = {EVENT_HomeNextProfileButton, 110, 60,
                                           LARGE_BUTTON_X_SIZE, LARGE_BUTTON_Y_SIZE, "Next -->"};
static const TextButton_S home_TempCalButton = {EVENT_HomeTempCalButton, 140, 150,
                                           LARGE_BUTTON_X_SIZE, LARGE_BUTTON_Y_SIZE, "Temp Cal"};

static const TemperatureProfile_S* tempProfiles;
static size_t currentProfileIndex = 0;

// Reflow Page
static void reflow_CustomElementsHandler(GUIEvent_E event, GUIAction_E action);
static void reflow_GraphElementHandler(GUIEvent_E event, GUIAction_E action);
static void reflow_OvenStatusHandler(GUIEvent_E event, GUIAction_E action);
static const CustomElement_S reflow_CustomElements = {EVENT_ReflowCustomElements, 0, 0, 0, 0, reflow_CustomElementsHandler};
static const CustomElement_S reflow_GraphElement = {EVENT_ReflowGraphElement, 0, 0, 0, 0, reflow_GraphElementHandler};
static const CustomElement_S reflow_OvenStatus = {EVENT_ReflowOvenStatusElement, 0, 0, 0, 0, reflow_OvenStatusHandler};
static const char* reflow_StartText = "Start";
static const char* reflow_StopText = "EStop";
static TextButton_S reflow_StartStopButton = {EVENT_ReflowStartStopButton, 160, 150, 50, 20, ""};


// Calibration Page
static void tempCal_CustomElementsHandler(GUIEvent_E event, GUIAction_E action);
static void tempCal_TempDisplayHandler(GUIEvent_E event, GUIAction_E action);
static const CustomElement_S tempCal_CustomElements = {EVENT_TempCalCustomElements, 0, 0, 0, 0, tempCal_CustomElementsHandler};
static const CustomElement_S tempCal_TempDisplay = {EVENT_TempCalTempDisplay, 0, 0, 0, 0, tempCal_TempDisplayHandler};
static const TextButton_S tempCal_PlusButton = {EVENT_TempCalPlusButton, 10, 90, 30, 20, " + "};
static const TextButton_S tempCal_MinusButton = {EVENT_TempCalMinusButton, 10, 120, 30, 20," - "};
static const TextButton_S tempCal_SetButton = {EVENT_TempCalSetButton, 70, 90,
                                         80, LARGE_BUTTON_Y_SIZE, "Calibrate"};
static uint16_t set_temp_val = 0; // TODO: Rename


// Page flippin'
typedef void (*pfnPageHandler)(bool, GUIEvent_E, GUIAction_E);
static pfnPageHandler currentPage = NULL;
static pfnPageHandler lastCalledPage = NULL;


/******************************************************************************
 * Functions
******************************************************************************/
static void homePageHandler(bool isFirstCall, GUIEvent_E event, GUIAction_E action);
static void tempCalPageHandler(bool isFirstCall, GUIEvent_E event, GUIAction_E action);
static void ReflowPageHandler(bool isFirstCall, GUIEvent_E event, GUIAction_E action);

static void draw_status_message(); // TODO: Refactor

static void reflowEventObserver(GUIEvent_E event, GUIAction_E action);

void setupReflowGUI(void)
{
    // Setup the LCD driver
	initILI9225B();

	tempProfiles = TempProfile_getProfiles();
	currentProfileIndex = 0;

	// Start the GUI manager
    setupGUIManager();
	addGUIObserver(reflowEventObserver);
}


/**
 * Reflow oven GUI periodic task
 */
void stepReflowGUI(void)
{
    reflowEventObserver(EVENT_NoEvent, ACTION_NoAction);
}


/**
 * Reflow GUI event observer. This function either gets called async by the
 * GUI manager when an event happens, or by the Reflow GUI periodic task.
 */
static void reflowEventObserver(GUIEvent_E event, GUIAction_E action)
{
    bool isFirstCall = false;

    if(currentPage == NULL)
    {
        currentPage = homePageHandler;
    }

    if(currentPage != lastCalledPage)
    {
        isFirstCall = true;
    }

    lastCalledPage = currentPage;
    (currentPage)(isFirstCall, event, action);
}


static void homePageHandler(bool isFirstCall, GUIEvent_E event, GUIAction_E action)
{
    if(isFirstCall)
    {
        // First time this page has been called, set up GUI elements
        home_profileButton.pText = tempProfiles[currentProfileIndex].name;

        addGUICustomElement(&multi_PageTitle);
        addGUICustomElement(&home_CustomElements);
        addGUIButtionElement(&home_profileButton);
        addGUIButtionElement(&home_nextProfileButton);
        addGUIButtionElement(&home_TempCalButton);
    }

    if(action == ACTION_Clicked)
    {
        if(event == EVENT_HomeProfileButton)
        {
            currentPage = ReflowPageHandler;
        }
        else if(event == EVENT_HomeNextProfileButton)
        {
            currentProfileIndex++;

            if(tempProfiles[currentProfileIndex].profile == NULL)
            {
                currentProfileIndex = 0;
            }

            home_profileButton.pText = tempProfiles[currentProfileIndex].name;
            requestGUIRedrawTextButton(&home_profileButton);
        }
        else if(event == EVENT_HomeTempCalButton)
        {
            // Jump to temperature calibration page
            currentPage = tempCalPageHandler;
        }
    }

    if(currentPage != homePageHandler)
    {
        // We are no longer the handler, remove our elements
        clearAllGUIElements();
    }
}


static void tempCalPageHandler(bool isFirstCall, GUIEvent_E event, GUIAction_E action)
{
    if(isFirstCall)
    {
        // First time this page has been called, set up GUI elements
        addGUICustomElement(&multi_PageTitle);
        addGUICustomElement(&tempCal_CustomElements);
        addGUICustomElement(&tempCal_TempDisplay);
        addGUIButtionElement(&multi_BackButton);
        addGUIButtionElement(&tempCal_PlusButton);
        addGUIButtionElement(&tempCal_MinusButton);
        addGUIButtionElement(&tempCal_SetButton);

        set_temp_val = get_temp();
    }


    if(action == ACTION_Clicked)
    {
        if(event == EVENT_TempCalPlusButton)
        {
            set_temp_val++;
            requestGUIRedrawCustom(&tempCal_TempDisplay);
        }
        else if(event == EVENT_TempCalMinusButton)
        {
            set_temp_val--;
            requestGUIRedrawCustom(&tempCal_TempDisplay);
        }
        else if(event == EVENT_TempCalSetButton)
        {
            set_cal(set_temp_val);
            requestGUIRedrawCustom(&tempCal_TempDisplay);
        }
        else if(event == EVENT_MultiBackButton)
        {
            // Go back to the home page
            currentPage = homePageHandler;
        }
    }
    else if(action == ACTION_NoAction)
    {
        // Just a periodic call, update the temp display
        requestGUIRedrawCustom(&tempCal_TempDisplay);
    }

    if(currentPage != tempCalPageHandler)
    {
        // We are no longer the handler, remove our elements
        clearAllGUIElements();
    }
}


static void ReflowPageHandler(bool isFirstCall, GUIEvent_E event, GUIAction_E action)
{
    static ReflowPageState_E eState = REFLOW_Init;
    bool fBackRequested = false;
    bool fStartStopRequested = false;

    OvenState_E oven_status = OvenCntl_getOvenState();

    if(isFirstCall)
    {
        // Entering the page
        eState = REFLOW_Init;
    }

    if(action == ACTION_Clicked)
    {
        if(event == EVENT_MultiBackButton)
        {
            fBackRequested = true;
        }
        else if(event == EVENT_ReflowStartStopButton)
        {
            fStartStopRequested = true;
        }
    }
    else if(action == ACTION_NoAction)
    {
        // Ended up here from the periodic task
        requestGUIRedrawCustom(&reflow_OvenStatus);
    }

    switch(eState)
    {
    case REFLOW_Init:
        addGUICustomElement(&reflow_CustomElements);
        addGUICustomElement(&reflow_GraphElement);
        addGUIButtionElement(&multi_BackButton);
        addGUICustomElement(&reflow_OvenStatus);
         eState = REFLOW_GoIdle;
        break;

    case REFLOW_GoIdle:
        if((oven_status==Oven_Idle))
        {
            reflow_StartStopButton.pText = reflow_StartText;
            addGUIButtionElement(&reflow_StartStopButton);
            eState = REFLOW_Idle;
        }
        break;

    case REFLOW_Idle:
        if(fBackRequested)
        {
            currentPage = homePageHandler;
        }
        else if(fStartStopRequested)
        {
            if(start_oven(tempProfiles[currentProfileIndex]))
            {
                reflow_StartStopButton.pText = reflow_StopText;
                requestGUIRedrawTextButton(&reflow_StartStopButton);
                removeGUITextButton(&multi_BackButton);
                eState = REFLOW_Reflowing;
            }
        }
        break;

    case REFLOW_Reflowing:
        if(fStartStopRequested)
        {
            // EStop
            stop_oven();
            eState = REFLOW_Cooldown;
        }
        else if(oven_status != Oven_Reflowing)
        {
            eState = REFLOW_Cooldown;
        }
        break;

    case REFLOW_Cooldown:
        removeGUITextButton(&reflow_StartStopButton);
        addGUIButtionElement(&multi_BackButton);
        eState = REFLOW_GoIdle;
        break;

    }

    if(currentPage != ReflowPageHandler)
    {
        // We are no longer the handler, remove our elements
        clearAllGUIElements();
    }
}


static void multi_PageTitleHandler(GUIEvent_E event, GUIAction_E action)
{
    if(event == EVENT_MultiPageTitle)
    {
        if(action == ACTION_Redraw)
        {
            setColor16(COLOR_16_BLUE);
            fillRect(0, 0, 220, 30);
            setColor16(COLOR_16_WHITE);
            setBackgroundColor16(COLOR_16_BLUE);
            drawString11_16(10, 10, "Reflow Oven V3");
        }
    }
}

static void home_CustomElementsHandler(GUIEvent_E event, GUIAction_E action)
{
    if(event == EVENT_HomeCustomElements)
    {
        if(action == ACTION_Redraw)
        {
            setBackgroundColor16(COLOR_16_BLACK);
            setColor16(COLOR_16_WHITE);
            drawString(10, 105, "Ensure Thermocouple Touches PCB");
            drawString(10, 150, "Time:");
            drawString(10, 120, "Hardware/Software:");
            drawString(10, 130, "Nathan Zimmerman");
        }
    }
}


static void tempCal_CustomElementsHandler(GUIEvent_E event, GUIAction_E action)
{
    if(event == EVENT_TempCalCustomElements)
    {
        if(action == ACTION_Redraw)
        {
            setBackgroundColor16(COLOR_16_BLACK);
            setColor16(COLOR_16_WHITE);
            drawString8_12(10,40, "Calibrate Temperature");
            drawString(10,55,"Ensure Thermcouple is plugged in.");
            drawString(10,63,"Thermcouple must be at room temp.");
            drawString(10,75,"Current Temp:");
            drawString(120,75,"Set Temp:");
        }
    }
}


static void tempCal_TempDisplayHandler(GUIEvent_E event, GUIAction_E action)
{
    if(event == EVENT_TempCalTempDisplay)
    {
        if(action == ACTION_Redraw)
        {
            char text_buffer[8];
            uint16_t oven_temp;

            //TODO: Refactor out this sprintf, save some space
            sprintf(text_buffer,"%dC",set_temp_val);
            drawString(180,75,text_buffer);

            oven_temp = get_temp();
            get_temp_str(oven_temp,text_buffer);
            setColor16(COLOR_16_CYAN);
            drawString(90, 75, text_buffer);
        }
    }

}



static void reflow_GraphElementHandler(GUIEvent_E event, GUIAction_E action)
{
    if(event == EVENT_ReflowGraphElement)
    {
        if(action == ACTION_Redraw)
        {
            setColor16(COLOR_16_GRAY);
            fillRect(30, 55, 210, 55);
            fillRect(30, 65, 210, 65);
            fillRect(30, 75, 210, 75);
            fillRect(30, 85, 210, 85);
            fillRect(30, 95, 210, 95);
            fillRect(30, 105, 210, 105);
            fillRect(30, 115, 210, 115);
            fillRect(30, 125, 210, 125);
            setColor16(COLOR_16_WHITE);
            fillRect(30, 50, 31, 135);
            fillRect(30, 135, 210, 136);
            fillRect(28, 55, 33, 55); //240C
            drawString(5, 52, "240");
            fillRect(28, 65, 33, 65); //210C
            drawString(5, 62, "210");
            fillRect(28, 75, 33, 75); //180C
            drawString(5, 72, "180");
            fillRect(28, 85, 33, 85); //150C
            drawString(5, 82, "150");
            fillRect(28, 95, 33, 95); //120C
            drawString(5, 92, "120");
            fillRect(28, 105, 33, 105); //90C
            drawString(5, 102, "90");
            fillRect(28, 115, 33, 115); //50C
            drawString(5, 112, "60");
            fillRect(28, 125, 33, 125); //20C
            drawString(5, 122, "30");
            fillRect(61, 133, 61, 138);
            fillRect(91, 133, 91, 138);
            fillRect(121, 133, 121, 138);
            fillRect(151, 133, 151, 138);
            fillRect(181, 133, 181, 138);
            fillRect(211, 133, 211, 138);
        }
    }

}


static void reflow_CustomElementsHandler(GUIEvent_E event, GUIAction_E action)
{
    if(event == EVENT_ReflowCustomElements)
    {
        if(action == ACTION_Redraw)
        {
            setBackgroundColor16(COLOR_16_BLACK);
            setColor16(COLOR_16_WHITE);
            drawString(10, 150, "Time:");


            setColor16(COLOR_16_CYAN);
            drawString(10, 25, "Oven Temp:");
            setColor16(COLOR_16_ORANGE);
            drawString(120, 25, "Peak Temp:");
            setColor16(COLOR_16_GREEN);
            drawString(120, 40, "Duration:");

            setColor16(COLOR_16_WHITE);

            setColor16(COLOR_16_GREEN);
            drawString8_12(10,5, tempProfiles[currentProfileIndex].name );
        }
    }
}



static void draw_status_message();
static void reflow_OvenStatusHandler(GUIEvent_E event, GUIAction_E action)
{
    char text_buffer[20];
    uint16_t oven_temp =0;
    static uint8_t oven_temp_max=0;

    if(event == EVENT_ReflowOvenStatusElement)
    {
        if(action == ACTION_Redraw)
        {
            setBackgroundColor16(COLOR_16_BLACK);
            draw_status_message();
            oven_temp = get_temp();
            get_temp_str(oven_temp, text_buffer);
            setColor16(COLOR_16_CYAN);
            drawString(75, 25, text_buffer);

            OvenState_E oven_status = OvenCntl_getOvenState();
            if((oven_status==Oven_Reflowing) || (oven_status==Oven_Alarm) || (oven_status==Oven_Cooldown))
            {
                if(oven_temp_max<oven_temp)
                {
                    oven_temp_max = oven_temp;
                    setColor16(COLOR_16_ORANGE);
                    drawString(185, 25, text_buffer);
                }
                setColor16(COLOR_16_GREEN);
                get_reflow_time_string(text_buffer);
                drawString(177, 40, text_buffer);
            }
        }
    }
}


static void draw_status_message()
{
    OvenState_E oven_status = OvenCntl_getOvenState();
    switch(oven_status)
    {
        case Oven_Error:
            setColor16(COLOR_16_RED);
            drawString(10, 40,"ThermocoupleError");
            break;
        case Oven_Alarm:
            setColor16(COLOR_16_YELLOW);
            drawString(10, 40,"Slightly OpenDoor");
            break;
        case Oven_Cooldown:
            setColor16(COLOR_16_RED);
            drawString(10, 40,"Too Hot to Start ");
            break;
        case Oven_Reflowing:
            setColor16(COLOR_16_GREEN);
            drawString(10, 40,"Oven Reflowing    ");
            break;
        case Oven_Idle:
            setColor16(COLOR_16_GREEN);
            drawString(10, 40,"Oven Ready       ");
            break;

        default:
            break;
    }
}







