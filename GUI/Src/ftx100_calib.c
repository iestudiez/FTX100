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
#define CALIB_PGR_BAR_X1		3
#define CALIB_PGR_BAR_Y1		19
#define CALIB_PGR_BAR_X2		124
#define CALIB_PGR_BAR_Y2		28
#define CALIB_TXT_YPOS 			37
#define CALIB_PWM_XPOS 			59
#define CALIB_REV_XPOS			100

// Public variables
// ----------------------------------------------------------------------------
GUI_InputBox_t InputBox_CalibValue;

// Private functions
// ----------------------------------------------------------------------------
void ftx100Calib_Init(GUI_FTX100Calib_t *calibScreen);
bool ftx100Calib_Done(GUI_FTX100Calib_t *calibScreen);
bool ftx100Calib_KeyActions(GUI_FTX100Calib_t *calibScreen);
void ftx100Calib_Draw(GUI_FTX100Calib_t *calibScreen);

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
	InputBox_CalibValue.sUnit = "gramos";
	InputBox_CalibValue.maxValue = CALIBRATION_MAX_VALUE;
	InputBox_CalibValue.xPos = 26;
	InputBox_CalibValue.yPos = 15;

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
	char sTxtBox[10];
	uint8_t xPos;
	uint8_t revolutions;

	// Reset redraw flag
	calibScreen->priv.redraw = false;

	// Clear LCD_Buffer and print calibration screen
	GLCD_ScreenBitmap(FTX100Calib_Screen_128x64);

	// Delay for the "ScreenBitmap" DMA transfer to finish drawing the screen
	SYSTEM_Delay(5);

	// Calculate revolutions
	if (*calibScreen->pStart)
		revolutions = *calibScreen->pProgressBar;
	else
		revolutions = 100;

	// Current number of revolutions
	sprintf(sTxtBox, "%d", (int) revolutions);
	GUI_DecimalPoint(sTxtBox, 1);
	xPos = CALIB_REV_XPOS - GLCD_TextWidth(sTxtBox, Font_NumVerdanaBlack, 1);
	GLCD_DrawText(sTxtBox, xPos, CALIB_TXT_YPOS, Font_NumVerdanaBlack, 1, 1);

	// Print PWM value
	sprintf(sTxtBox, "%d", (int) *calibScreen->pCalibPwm);
	xPos = CALIB_PWM_XPOS - GLCD_TextWidth(sTxtBox, Font_NumVerdanaBlack, 1);
	GLCD_DrawText(sTxtBox, xPos, CALIB_TXT_YPOS, Font_NumVerdanaBlack, 1, 1);

	// Draw progress bar
	GLCD_ProgressBar(CALIB_PGR_BAR_X1, CALIB_PGR_BAR_Y1, CALIB_PGR_BAR_X2, CALIB_PGR_BAR_Y2, *calibScreen->pProgressBar);

	// Indicate if the calibration process has started.
	if (*calibScreen->pStart)
		GLCD_PutBitmap(45, 51, FTX100IconCalibStop, 1);
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
