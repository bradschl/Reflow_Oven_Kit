/*
 * ReflowGUI.c
 *
 *  Created on: Aug 10, 2013
 *      Author: user
 */

#include <stdbool.h>
#include "GUI/GUIManager.h"
#include "GUI/ReflowGUI.h"
//#include "LCD/ColorLCD.h"

/******************************************************************************
 * Defines
******************************************************************************/
#define LARGE_BUTTON_X_SIZE 80
#define LARGE_BUTTON_Y_SIZE 20

/******************************************************************************
 * Static Variables
******************************************************************************/
static TextButton_S button_HomeLeaded = {EVENT_HomeButton_Leaded, 10, 60, LARGE_BUTTON_X_SIZE, LARGE_BUTTON_Y_SIZE, "Leaded"};

/******************************************************************************
 * Functions
******************************************************************************/
void setupReflowGUI(void)
{
	initILI9225B();
}


void stepReflowGUI(void)
{
    static bool redraw = true;
    if(redraw)
    {
        addGUIButtionElement(&button_HomeLeaded);
        redraw = false;
    }
}
