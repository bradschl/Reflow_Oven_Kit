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
// See C file for function descriptions

void setupGUIManager(void);
void stepGUIManagerTask(void);
void publishGUIEvents(void);

void clearAllGUIElements(void);

void addGUIButtionElement(const TextButton_S* textButton);
void removeGUITextButton(const TextButton_S* pTextButton);

void addGUICustomElement(const CustomElement_S* pCustomElement);
void removeGUICustomElement(const CustomElement_S* pCustonElement);

void addGUIObserver(pfnElementtHandler pfnObserver);
// TODO: Add in removeGUIObserver

void requestGUIRedrawCustom(const CustomElement_S* pCustomElement);
void requestGUIRedrawTextButton(const TextButton_S* pTextButton);

#endif /* GUIMANAGER_H_ */



