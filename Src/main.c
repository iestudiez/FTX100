/**
 *
 ******************************************************************************
 * @file        : main.c
 * @author      : Estudiez, Ivan
 * @brief       : CX100 Main
 * @date		: 20/05/2026
 * @version		: 1.2 (New CX100LIB)
 ******************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stm32f4xx.h>
#include "system_cx100.h"
#include "indicators.h"
#include "keyboard.h"
#include "gnss.h"
#include "scheduler.h"
#include "lcd.h"
#include "pwrboard.h"
#include "gui.h"
#include "app.h"

// Definitions
// ----------------------------------------------------------------------------

// Global variables
// ----------------------------------------------------------------------------

// Private methods
// ----------------------------------------------------------------------------
void UI_Update(void);
void COMM_Update(void);

int main()
{
	// System Initialization
	// -------------------------------------------------------------------------
	SYSTEM_Init();
	PWRBOARD_Init();
	LCD_Init();
	LCD_Clear();
	GNSS_Init();
	APP_Init();
	GUI_Init();

	// Task Scheduler Section
	// -------------------------------------------------------------------------
	SCHEDULER_Init();
	SCHEDULER_AddTask(PWRBOARD_Update, 0, 10);
	SCHEDULER_AddTask(LCD_Update, 1, 10);
	SCHEDULER_AddTask(COMM_Update, 2, 10);
	SCHEDULER_AddTask(APP_User, 3, 10);
	SCHEDULER_AddTask(UI_Update, 4, 200);
	SCHEDULER_Start();

	while (1)
	{
		SCHEDULER_DispatchTasks();
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief	User Interface Task (200 milliseconds period)
 * -----------------------------------------------------------------------------
 */
void UI_Update(void)
{
	KEYBOARD_Update();
	INDICATORS_Update();
	GUI_Update();
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Serial communications task
 * -----------------------------------------------------------------------------
 */
void COMM_Update(void)
{
	// RS233 Communications
	GNSS_Update();

	// CAN Communications (RX)
	// CAN_ReadMsg(CAN1, &gCanMsgRx);
}

