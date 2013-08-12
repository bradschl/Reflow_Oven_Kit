//Nathan Zimmerman

#include "msp430g2553.h"
#include <stdint.h>
#include <stdio.h>
#include "LCD\colorLCD.h"
#include "LCD\touch.h"
#include "DRIVE\GPIO.h"
#include "DRIVE\rtc.h"
#include "OS\os.h"
#include "Oven\oven.h"
#include "DRIVE/timer.h"
#include "DRIVE/led.h"
#include "Oven\calibration.h"

#include "GUI/ReflowGUI.h"
#include "GUI/GUIManager.h"

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // disable WDT
	BCSCTL1 = CALBC1_16MHZ; // 16MHz clock
	DCOCTL = CALDCO_16MHZ;

	init_cal();
	Setup_PWM_Timers();
	setupRTC();

	// GUI Elements
	TOUCH_SETUP();
	setupReflowGUI();

	// Oven Control
	reset_idle_timer();
	OVEN_SETUP;
	OVEN_OFF;

	// Enable interrupts and start the scheduler
	__enable_interrupt();
	runScheduler_noRet();

}







