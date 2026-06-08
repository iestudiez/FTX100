/**
 ******************************************************************************
 * @file        : ftx100_calib.c
 * @author      : Ivan.Estudiez
 * @brief       : Calibration screen
 * @date        : 29 may 2026
 * @version     : 0.
 ******************************************************************************
 */

#include "stdio.h"
#include "keyboard.h"
#include "ftx100_calib.h"
#include "gui_utils.h"
#include "glcd.h"
#include "ftx100_bitmaps.h"
#include "stdbool.h"
#include "input_box.h"
#include "app.h"
#include "fonts.h"
#include "system_cx100.h"

// Defines
// ----------------------------------------------------------------------------
#define CALIB_PGR_BAR_X1			3
#define CALIB_PGR_BAR_Y1			15
#define CALIB_PGR_BAR_X2			124
#define CALIB_PGR_BAR_Y2			24
#define CALIB_TXT_YPOS 				32
#define CALIB_PWM_XPOS 				59
#define CALIB_REV_XPOS				102
#define CALIB_PWM_INC				25
#define CALIB_PWM_MIN				100
#define CALIB_PWM_MAX				500
#define CALIB_MAX_REVOLUTIONS		100

// Public variables
// ----------------------------------------------------------------------------
GUI_InputBox_t InputBox_CalibValue;

// Private functions
// ----------------------------------------------------------------------------
void ftx100Calib_Init(GUI_FTX100Calib_t *calibScreen);
bool ftx100Calib_Done(GUI_FTX100Calib_t *calibScreen);
bool ftx100Calib_KeyActions(GUI_FTX100Calib_t *calibScreen);
void ftx100Calib_Draw(GUI_FTX100Calib_t *calibScreen);
void ftx100_TxtBox(uint16_t value, uint8_t x, uint8_t decPos);

/**
 * -----------------------------------------------------------------------------
 * @brief				FTX100 Fertilizer calibration screen
 *-----------------------------------------------------------------------------
 * @param calibScreen	Pointer to FTX100 calibration structure
 * -----------------------------------------------------------------------------
 */
void GUI_FTX100CalibScreen(GUI_FTX100Calib_t *calibScreen)
{
	// Exit if state is inactive
	if (calibScreen->priv.status == GUI_STATUS_DISABLED)
		return;

	// If there was a jump from a previous menu,
	// perform the keyboard reading in the next cycle
	if (calibScreen->priv.status == GUI_STATUS_JUMP)
	{
		calibScreen->priv.status = GUI_STATUS_ENABLED;
		*calibScreen->pEnabled = true;
		return;
	}

	// Initialization
	if (calibScreen->priv.init == false)
		ftx100Calib_Init(calibScreen);

	// Check if the calibration process has been completed
	if (ftx100Calib_Done(calibScreen))
		return;

	// Process keyboard actions
	if (ftx100Calib_KeyActions(calibScreen))
		return;

	// Draw the menu on screen
	if (calibScreen->priv.redraw)
		ftx100Calib_Draw(calibScreen);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				FTX100 calibration screen initialization
 * ----------------------------------------------------------------------------
 * @param calibScreen	Pointer to FTX100Calib type structure
 * ----------------------------------------------------------------------------
 */
void ftx100Calib_Init(GUI_FTX100Calib_t *calibScreen)
{
	// Build calibration input box
	InputBox_CalibValue.pRet = &calibScreen->priv.status;
	InputBox_CalibValue.pInputVar = &APP_CalibValue;
	InputBox_CalibValue.dataType = GUI_UINT16;
	InputBox_CalibValue.decimalPos = 0;
	InputBox_CalibValue.numDigits = 4;
	InputBox_CalibValue.sUnit = "g";
	InputBox_CalibValue.maxValue = CALIBRATION_MAX_VALUE;
	InputBox_CalibValue.xPos = 26;
	InputBox_CalibValue.yPos = 11;

	calibScreen->priv.init = true;
	calibScreen->priv.redraw = true;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Perform keyboard actions
 * ----------------------------------------------------------------------------
 * @param calibScreen	Pointer to FTX100Calib type structure
 * @return				False = Continue, True = Exit current control.
 * ----------------------------------------------------------------------------
 */
bool ftx100Calib_KeyActions(GUI_FTX100Calib_t *calibScreen)
{
	// Exit the calibration screen
	if (KEYBOARD_Event.Esc == 1)
	{
		// Reset status variables
		calibScreen->priv.redraw = true;
		*calibScreen->pEnabled = false;
		*calibScreen->pStart = false;
		*calibScreen->pProgressBar = 0;

		// Give execution control to the return address
		GUI_TransferControl(&calibScreen->priv.status, calibScreen->pRet);
		return true;
	}

	// Start the calibration process
	if (KEYBOARD_Event.Pwr == 1)
		*calibScreen->pStart = true;

	// Key Up
	if ((KEYBOARD_Event.N2_Up == 1) && *calibScreen->pCalibPwm < CALIB_PWM_MAX)
		*calibScreen->pCalibPwm += CALIB_PWM_INC;

	// Key Down
	if ((KEYBOARD_Event.N5_Down == 1) && *calibScreen->pCalibPwm > CALIB_PWM_MIN)
		*calibScreen->pCalibPwm -= CALIB_PWM_INC;

	calibScreen->priv.redraw = true;
	return false;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Draw FTX100 calibration screen
 * ----------------------------------------------------------------------------
 * @param calibScreen	Pointer to FTX100Calib type structure
 * ----------------------------------------------------------------------------
 */
void ftx100Calib_Draw(GUI_FTX100Calib_t *calibScreen)
{
	uint16_t revolutions;

	// Reset redraw flag
	calibScreen->priv.redraw = false;

	// Clear LCD_Buffer and print calibration screen
	GLCD_ScreenBitmap(FTX100Calib_Screen_128x64);

	// Delay for the "ScreenBitmap" DMA transfer to finish drawing the screen
	SYSTEM_DelayUs(5);

	// Calculate revolutions
	if (*calibScreen->pStart)
		revolutions = *calibScreen->pProgressBar;
	else
		revolutions = CALIB_MAX_REVOLUTIONS;

	// Print current number of revolutions
	ftx100_TxtBox(revolutions, CALIB_REV_XPOS, 1);

	// Print PWM value
	ftx100_TxtBox(*calibScreen->pCalibPwm, CALIB_PWM_XPOS, 0);

	// Draw progress bar
	GLCD_ProgressBar(CALIB_PGR_BAR_X1, CALIB_PGR_BAR_Y1, CALIB_PGR_BAR_X2, CALIB_PGR_BAR_Y2, *calibScreen->pProgressBar);

	// Indicate if the calibration process has started.
	if (*calibScreen->pStart)
		GLCD_PutBitmap(45, 50, FTX100IconCalibStop, 1);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Check if the calibration process has been completed
 * ----------------------------------------------------------------------------
 * @param calibScreen	Pointer to FTX100Calib type structure
 * @return				False = Continue, True = Exit current control.
 * ----------------------------------------------------------------------------
 */
bool ftx100Calib_Done(GUI_FTX100Calib_t *calibScreen)
{
	static bool fillProgressBar = true;

	if (*calibScreen->pCalibDone == false)
		return false;

	// Fill the progress bar when calibration is complete.
	if (fillProgressBar)
	{
		fillProgressBar = false;
		GLCD_DrawBox(CALIB_PGR_BAR_X1, CALIB_PGR_BAR_Y1, CALIB_PGR_BAR_X2, CALIB_PGR_BAR_Y2, 1);

		// Print the target number of revolutions
		GLCD_DrawBox(69, 31, 100, 43, 0);
		ftx100_TxtBox(CALIB_MAX_REVOLUTIONS, CALIB_REV_XPOS, 1);

		return true;
	}

	// Reset status variables
	calibScreen->priv.redraw = true;
	*calibScreen->pEnabled = false;
	*calibScreen->pStart = false;
	*calibScreen->pCalibDone = false;
	*calibScreen->pProgressBar = 0;

	fillProgressBar = true;

	// Transfer execution control to the return graphical component.
	GUI_TransferControl(&calibScreen->priv.status, &InputBox_CalibValue.priv.status);
	GUI_InputBox_DefaultPointer = &InputBox_CalibValue;
	return true;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Print a text box
 * ----------------------------------------------------------------------------
 * @param value		Value to print
 * @param x			Horizontal position
 * @param decPos	Number of decimal places
 * ----------------------------------------------------------------------------
 */
void ftx100_TxtBox(uint16_t value, uint8_t x, uint8_t decPos)
{
	char sTxtBox[10];
	uint8_t xPos;

	sprintf(sTxtBox, "%d", (int) value);

	if (decPos)
		GUI_DecimalPoint(sTxtBox, decPos);

	xPos = x - GLCD_TextWidth(sTxtBox, Font_NumVerdanaBlack, 1);

	GLCD_DrawText(sTxtBox, xPos, CALIB_TXT_YPOS, Font_NumVerdanaBlack, 1, 1);
}
