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
#include <string.h>
#include <stdio.h>
#include "system_cx100.h"
#include "indicators.h"
#include "keyboard.h"
#include "gnss.h"
#include "scheduler.h"
#include "lcd.h"
#include "pwrboard.h"
#include "gui.h"
#include "app.h"
#include "uart.h"

// ----------------------------------------------------------------------------
// TODO:
// 		1.- Welcome Screen (LEDs and Logo, during GPS setup time)
// 		2.- Password PID parameters
//		3.-
// ----------------------------------------------------------------------------

// Definitions
// ----------------------------------------------------------------------------
#define DEBUG_PID_RS232

// Global variables
// ----------------------------------------------------------------------------

// Private methods
// ----------------------------------------------------------------------------
void UI_Update(void);
void COMM_Update(void);
void UartPlotterMode(void);
void SERIAL_Init(void);

int main()
{
	// System Initialization
	// -------------------------------------------------------------------------
	SYSTEM_Init();
	PWRBOARD_Init();
	LCD_Init();
	LCD_Clear();
	SERIAL_Init();
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
	// -------------------------------------------------------------------------
#ifdef DEBUG_PID_RS232
	// Send plotter data
	sprintf(UART_TxBuffer, " %d %d %d %d %d\n", (int) APP_RpmSetpoint, (int) APP_MotorRpm, (int) APP_PidMotor.priv.propTerm, (int) APP_PidMotor.priv.intgTerm, (int) APP_PidMotor.priv.dervTerm);

	// Clear transfer complete flag
	DMA1->HIFCR |= DMA_HIFCR_CTCIF6;

	// Start DMA transfer
	DMA1_Stream6->NDTR = strlen(UART_TxBuffer);
	DMA_Stream_Enable(DMA1_Stream6);
#else
	// Process GPS data
	GNSS_Update();
#endif
}
/**
 * -----------------------------------------------------------------------------
 * @brief 		Initialize serial communication
 * -----------------------------------------------------------------------------
 */
void SERIAL_Init(void)
{
#ifdef DEBUG_PID_RS232
	UartPlotterMode();
	UART_Enable(USART2);
#else
	GNSS_Init();
#endif
}

/**
 * -----------------------------------------------------------------------------
 * @brief 		Configure UART to send plotter data
 * -----------------------------------------------------------------------------
 */
void UartPlotterMode(void)
{
	UART_Config_t serialPort;

	// UART Configuration
	// ------------------
	// UART2 Module
	UART_Config_Reset(&serialPort);
	serialPort.module = USART2;
	serialPort.periph_clk = (uint32_t) RCC_APB1_CLK;
	serialPort.baudrate = BR_115200;
	serialPort.dir_tx = true;
	serialPort.dir_rx = true;
	serialPort.dma_tx = true;
	serialPort.dma_rx = true;
	serialPort.rxneie = false;
	serialPort.tcie = false;
	serialPort.idleie = false;
	UART_Init(serialPort);
}

