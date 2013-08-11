/*
 * GUIManager.h
 *
 *  Created on: Aug 10, 2013
 *      Author: user
 */

#ifndef GUIMANAGER_H_
#define GUIMANAGER_H_

#include <stdint.h>
#include "System.h"
#include "GUIEvents.h"
#include "GUIElements.h"
#include "LCD/colorLCD.h"


/******************************************************************************
 * Defines
******************************************************************************/
/*
 * Since all active elements are stored in static buffers, we need limits on the
 * number of active elements in the GUI.
 */
#define GUI_MAX_TEXT_BUTTONS 4
#define GUI_MAX_CUSTOM_ELEMENTS 5

/**
 * We can only store a finite number of events at a time
 */
#define GUI_EVENT_QUEUE_SIZE 3

/**
 * Maximum number of GUI event listeners
 */
#define GUI_MAX_OBSERVERS 3


/******************************************************************************
 * Global Variables
******************************************************************************/


/******************************************************************************
 * Functions
******************************************************************************/
void setupGUIManager(void);
void stepGUIManagerTask(void);
void publishGUIEvents(void);

void clearAllGUIElements(void);

void addGUIButtionElement(TextButton_S* textButton);
void removeGUITextButton(TextButton_S* pTextButton);

void addGUICustomElement(CustomElement_S* pCustomElement);
void removeGUICustomElement(CustomElement_S* pCustonElement);



#endif /* GUIMANAGER_H_ */



