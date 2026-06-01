/**
 ******************************************************************************
 * @file        : app_main.c
 * @author      : Ivan.Estudiez
 * @brief       : 
 * @date        : 27 may 2026
 * @version     : 0.
 ******************************************************************************
 */

#include "stdio.h"
#include "ftx100_main.h"
#include "glcd.h"
#include "keyboard.h"
#include "stdbool.h"
#include "gui.h"
#include "gui_utils.h"
#include "fonts.h"
#include "ftx100_bitmaps.h"
#include "app.h"
#include "global.h"
#include "gnss.h"

// Private functions
// ----------------------------------------------------------------------------
void ftx100_Init(GUI_FTX100App_t *app);
bool ftx100_KeyActions(GUI_FTX100App_t *app);
void ftx100_Draw(GUI_FTX100App_t *app);

/**
 * -----------------------------------------------------------------------------
 * @brief			FTX100 Fertilizer main application screen
 *-----------------------------------------------------------------------------
 * @param app		Pointer to FTX100 application structure
 * -----------------------------------------------------------------------------
 */
void GUI_FTX100MainScreen(GUI_FTX100App_t *app)
{
	// Exit if state is inactive
	if (app->priv.status == GUI_STATUS_DISABLED)
		return;

	// If there was a jump from a previous menu,
	// perform the keyboard reading in the next cycle
	if (app->priv.status == GUI_STATUS_JUMP)
	{
		app->priv.status = GUI_STATUS_ENABLED;
		return;
	}

	// Initialization
	if (app->priv.init == false)
		ftx100_Init(app);

	// Process keyboard actions
	if (ftx100_KeyActions(app))
		return;

	// Draw the menu on screen
	if (app->priv.redraw)
		ftx100_Draw(app);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			FTX100 screen initialization
 * ----------------------------------------------------------------------------
 * @param app		Pointer to FTX100App type structure
 * ----------------------------------------------------------------------------
 */
void ftx100_Init(GUI_FTX100App_t *app)
{
	app->priv.init = true;
	app->priv.redraw = true;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Perform keyboard actions
 * ----------------------------------------------------------------------------
 * @param app		Pointer to FTX100App type structure
 * @return			False = Continue, True = Exit menu.
 * ----------------------------------------------------------------------------
 */
bool ftx100_KeyActions(GUI_FTX100App_t *app)
{
	// Escape action
	if (KEYBOARD_Event.Esc == 1)
	{
		GUI_TransferControl(&app->priv.status, app->pRet);
		app->priv.redraw = true;
		return true;
	}

	// Enable dosing system
	if (KEYBOARD_Event.Pwr == 1)
		*app->pEnable = !(*app->pEnable);

	// Dose preset N1
	if (KEYBOARD_Event.N1 == 1)
		*app->pDosePreset = DOSE_PRESET_1;

	// Dose preset N2
	if (KEYBOARD_Event.N2_Up == 1)
		*app->pDosePreset = DOSE_PRESET_2;

	// Dose preset N3
	if (KEYBOARD_Event.N3 == 1)
		*app->pDosePreset = DOSE_PRESET_3;

	// Dose preset N4
	if (KEYBOARD_Event.N4_Left == 1)
		*app->pDosePreset = DOSE_PRESET_4;

	// Dose preset N5
	if (KEYBOARD_Event.N5_Down == 1)
		*app->pDosePreset = DOSE_PRESET_5;

	// Enable simulation mode
	if (KEYBOARD_Event.Bckspc == 5)
		*app->pSimuMode = !(*app->pSimuMode);

	if (KEYBOARD_Event.N7 == 1 || KEYBOARD_Event.N7 > 3)
		if (*app->pSimuSpeed > 0)
			*app->pSimuSpeed -= 5;

	if (KEYBOARD_Event.N9 == 1 || KEYBOARD_Event.N9 > 3)
		if (*app->pSimuSpeed < app->maxSpeed)
			*app->pSimuSpeed += 5;

	app->priv.redraw = true;
	return false;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 			Draw FTX100 main screen
 * ----------------------------------------------------------------------------
 * @param app		Pointer to FTX100App type structure
 * ----------------------------------------------------------------------------
 */
void ftx100_Draw(GUI_FTX100App_t *app)
{
	const uint8_t BIG_TEXT_YPOS = 18;
	const uint8_t SPEED_XPOS = 48;
	const uint8_t DOSE_XPOS = 114;
	const uint8_t SMALL_TEXT_YPOS = 42;
	const uint8_t MOTOR_XPOS = 95;
	const uint8_t TURBINE_XPOS = 63;

	char txtBox[10];
	uint8_t xPos;
	uint8_t speedBar;

	// Draw screen
	GLCD_ScreenBitmap(FTX100App_Screen_128x64);
	app->priv.redraw = false;

	// Text boxes section
	// ========================================================================

	// Update speed text box
	// ------------------------------------------------------------------------
	sprintf(txtBox, "%d", (int) *app->pSpeed);
	GUI_DecimalPoint(txtBox, 1);
	xPos = SPEED_XPOS - GLCD_TextWidth(txtBox, Font_NumArialBlack, 1);
	GLCD_DrawText(txtBox, xPos, BIG_TEXT_YPOS, Font_NumArialBlack, 1, 1);

	// Update motor text box
	// ------------------------------------------------------------------------
	sprintf(txtBox, "%d", (int) *app->pMotorRpm);
	xPos = MOTOR_XPOS - GLCD_TextWidth(txtBox, Font_NumVerdanaBlack, 1);
	GLCD_DrawText(txtBox, xPos, SMALL_TEXT_YPOS, Font_NumVerdanaBlack, 1, 1);

	// Update turbine text box
	// ------------------------------------------------------------------------
	sprintf(txtBox, "%d", (int) *app->pTurbineRpm);
	xPos = TURBINE_XPOS - GLCD_TextWidth(txtBox, Font_NumVerdanaBlack, 1);
	GLCD_DrawText(txtBox, xPos, SMALL_TEXT_YPOS, Font_NumVerdanaBlack, 1, 1);

	// Update dose text box
	// ------------------------------------------------------------------------
	if (!((APP_ErrorCode & APP_ERROR_PID) && GLOBAL_Blink2))
	{
		sprintf(txtBox, "%d", (int) *app->pDose);
		GUI_DecimalPoint(txtBox, 1);
		xPos = DOSE_XPOS - GLCD_TextWidth(txtBox, Font_NumArialBlack, 1);
		GLCD_DrawText(txtBox, xPos, BIG_TEXT_YPOS, Font_NumArialBlack, 1, 1);
	}

	// Speed bar section
	// ========================================================================
	speedBar = (uint8_t) ((*app->pSpeed * 100) / app->maxSpeed);
	GLCD_ProgressBar(4, 56, 123, 60, speedBar);

	// Icons section
	// ========================================================================

	// Draw On/Off Icon
	// ------------------------------------------------------------------------
	if (*app->pEnable)
		GLCD_PutBitmap(43, 3, FTX100IconOn, 0);
	else
		GLCD_PutBitmap(43, 3, FTX100IconOff, 0);

	// Draw DOSE Icon
	// ------------------------------------------------------------------------
	switch (*app->pDosePreset)
	{
	case DOSE_PRESET_1:
		GLCD_PutBitmap(78, 2, FTX100IconDose1, 0);
		break;

	case DOSE_PRESET_2:
		GLCD_PutBitmap(78, 2, FTX100IconDose2, 0);
		break;

	case DOSE_PRESET_3:
		GLCD_PutBitmap(78, 2, FTX100IconDose3, 0);
		break;

	case DOSE_PRESET_4:
		GLCD_PutBitmap(78, 2, FTX100IconDose4, 0);
		break;

	case DOSE_PRESET_5:
		GLCD_PutBitmap(78, 2, FTX100IconDose5, 0);
		break;
	}

	// Draw GPS Icon
	// ------------------------------------------------------------------------
	if (*app->pSimuMode)
	{
		GLCD_PutBitmap(59, 2, FTX100IconGpsSimu, 0);
		return;
	}

	// Draw GPS Icon
	// ------------------------------------------------------------------------
	if (GNSS_Rmc.mode == GNSS_RMC_MODE_D || GNSS_Rmc.mode == GNSS_RMC_MODE_A || GNSS_Rmc.mode == GNSS_RMC_MODE_E)
		GLCD_PutBitmap(59, 2, FTX100IconGpsOk, 0);
	else
		GLCD_PutBitmap(59, 2, FTX100IconGpsError, 0);

}
