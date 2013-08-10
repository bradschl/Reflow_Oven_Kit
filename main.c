//Nathan Zimmerman

#include "msp430g2553.h"
#include <stdint.h>
#include <stdio.h>
#include "LCD\colorLCD.h"
#include "LCD\touch.h"
#include "LCD\gui.h"
#include "DRIVE\GPIO.h"
#include "DRIVE\rtc.h"
#include "OS\os.h"
#include "Oven\oven.h"
#include "DRIVE/timer.h"
#include "DRIVE/led.h"
#include "Oven\calibration.h"

int main(void)
{
	WDTCTL = WDTPW + WDTHOLD; // disable WDT
	BCSCTL1 = CALBC1_16MHZ; // 16MHz clock
	DCOCTL = CALDCO_16MHZ;

	init_cal();
	Setup_PWM_Timers();
	TOUCH_SETUP();
	setupRTC();
	Setup_LCD();
	reset_idle_timer();
	OVEN_SETUP;
	OVEN_OFF;

	__enable_interrupt();

	runScheduler_noRet();

//	while(1);






}

