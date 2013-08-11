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
    EVENT_HomeButton_Leaded
} GUIEvent_E;

typedef enum
{
    ACTION_NoAction,
    ACTION_Redraw,
    ACTION_Clicked
} GUIAction_E;

#endif /* GUIEVENTS_H_ */
