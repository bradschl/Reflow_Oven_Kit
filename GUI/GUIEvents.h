/*
 * GUIEvents.h
 *
 *  Created on: Aug 10, 2013
 *      Author: user
 */

#ifndef GUIEVENTS_H_
#define GUIEVENTS_H_

typedef enum
{
    EVENT_NoEvent,
    EVENT_MultiPageTitle,
    EVENT_MultiBackButton,

    EVENT_HomeCustomElements,
    EVENT_HomeLeadedButton,
    EVENT_HomeLeadFreeButton,
    EVENT_HomeTempCalButton,

    EVENT_TempCalPlusButton,
    EVENT_TempCalMinusButton,
    EVENT_TempCalSetButton,
    EVENT_TempCalTempDisplay,
    EVENT_TempCalCustomElements,

    EVENT_ReflowCustomElements,
    EVENT_ReflowGraphElement,
    EVENT_ReflowOvenStatusElement,
    EVENT_ReflowStartStopButton

} GUIEvent_E;

typedef enum
{
    ACTION_NoAction,
    ACTION_Redraw,
    ACTION_Clicked
} GUIAction_E;

#endif /* GUIEVENTS_H_ */
