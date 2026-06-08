/**
 ******************************************************************************
 * @file        : ftx100_info.c
 * @author      : Ivan.Estudiez
 * @brief       : Info Screen FTX100
 * @date        : 2 jun 2026
 * @version     : 0.1
 ******************************************************************************
 */

#include "stdio.h"
#include "ftx100_info.h"
#include "gui_utils.h"
#include "keyboard.h"
#include "glcd.h"
#include "fonts.h"
#include "app.h"
#include "ftx100_bitmaps.h"
#include "system_cx100.h"

// Private functions
// ----------------------------------------------------------------------------
void ftx100Info_Init(GUI_FTX100Info_t *info);
bool ftx100Info_KeyActions(GUI_FTX100Info_t *info);
void ftx100Info_Draw(GUI_FTX100Info_t *info);

/**
 * -----------------------------------------------------------------------------
 * @brief			FTX100 Fertilizer info screen
 *-----------------------------------------------------------------------------
 * @param info		Pointer to FTX100 info screen structure
 * -----------------------------------------------------------------------------
 */
void GUI_FTX100InfoScreen(GUI_FTX100Info_t *info)
{
	// Exit if state is inactive
	if (info->priv.status == GUI_STATUS_DISABLED)
		return;

	// If there was a jump from a previous menu,
	// perform the keyboard reading in the next cycle
	if (info->priv.status == GUI_STATUS_JUMP)
	{
		info->priv.status = GUI_STATUS_ENABLED;
		return;
	}

	// Initialization
	if (info->priv.init == false)
		ftx100Info_Init(info);

	// Process keyboard actions
	if (ftx100Info_KeyActions(info))
		return;

	// Draw the menu on screen
	if (info->priv.redraw)
		ftx100Info_Draw(info);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				FTX100 calibration screen initialization
 * ----------------------------------------------------------------------------
 * @param info			Pointer to FTX100Info type structure
 * ----------------------------------------------------------------------------
 */
void ftx100Info_Init(GUI_FTX100Info_t *info)
{
	info->priv.init = true;
	info->priv.redraw = true;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Perform keyboard actions
 * ----------------------------------------------------------------------------
 * @param info			Pointer to FTX100Info type structure
 * @return				False = Continue, True = Exit current control.
 * ----------------------------------------------------------------------------
 */
bool ftx100Info_KeyActions(GUI_FTX100Info_t *info)
{
	// Escape action
	if (KEYBOARD_Event.Esc == 1)
	{
		GUI_TransferControl(&info->priv.status, info->pRet);
		info->priv.redraw = true;
		return true;
	}

	info->priv.redraw = true;
	return false;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Draw FTX100 Info screen
 * ----------------------------------------------------------------------------
 * @param info			Pointer to FTX100Info type structure
 * ----------------------------------------------------------------------------
 */
void ftx100Info_Draw(GUI_FTX100Info_t *info)
{
	const uint8_t BIG_TEXT_YPOS = 9;
	const uint8_t SMALL_TEXT_YPOS = 37;

	const uint8_t BATT_XPOS = 60;
	const uint8_t TEMP_XPOS = 100;

	const uint8_t ERROR_XPOS = 60;
	const uint8_t CURRENT_XPOS = 98;

	char sTxtBox[10];
	uint8_t xPos;

	// Draw Info Screen
	GLCD_ScreenBitmap(FTX100Info_Screen_128x64);
	SYSTEM_DelayUs(5);

	// Text boxes section
	// ========================================================================

	// Update battery text box
	// ------------------------------------------------------------------------
	sprintf(sTxtBox, "%d", (int) (*info->pVoltage /100));
	GUI_DecimalPoint(sTxtBox, 1);
	xPos = BATT_XPOS - GLCD_TextWidth(sTxtBox, Font_NumArialBlack, 0);
	GLCD_DrawText(sTxtBox, xPos, BIG_TEXT_YPOS, Font_NumArialBlack, 0, 1);

	// Update temperature text box
	// ------------------------------------------------------------------------
	sprintf(sTxtBox, "%d", (int) *info->pTemp);
	GUI_DecimalPoint(sTxtBox, 1);
	xPos = TEMP_XPOS - GLCD_TextWidth(sTxtBox, Font_NumArialBlack, 0);
	GLCD_DrawText(sTxtBox, xPos, BIG_TEXT_YPOS, Font_NumArialBlack, 0, 1);

	// Update error code text box
	// ------------------------------------------------------------------------
	sprintf(sTxtBox, "%d", (int) *info->pErrorCode);
	xPos = ERROR_XPOS - GLCD_TextWidth(sTxtBox, Font_NumVerdanaBlack, 1);
	GLCD_DrawText(sTxtBox, xPos, SMALL_TEXT_YPOS, Font_NumVerdanaBlack, 1, 1);

	// Update device current text box
	// ------------------------------------------------------------------------
	sprintf(sTxtBox, "%d", (int) *info->pCurrent);
	xPos = CURRENT_XPOS - GLCD_TextWidth(sTxtBox, Font_NumVerdanaBlack, 1);
	GLCD_DrawText(sTxtBox, xPos, SMALL_TEXT_YPOS, Font_NumVerdanaBlack, 1, 1);

	// Update Check Boxes section
	// ========================================================================

	// CAN Bus box
	// ------------------------------------------------------------------------
	if (*info->pErrorCode & APP_ERROR_CAN)
		GLCD_PutBitmap(34, 53, FTX100IconCheckFail, 0);
	else
		GLCD_PutBitmap(34, 53, FTX100IconCheckOk, 0);

	// PID box
	// ------------------------------------------------------------------------
	if (*info->pErrorCode & APP_ERROR_PID)
		GLCD_PutBitmap(66, 53, FTX100IconCheckFail, 0);
	else
		GLCD_PutBitmap(66, 53, FTX100IconCheckOk, 0);

	// GPS box
	// ------------------------------------------------------------------------
	if (*info->pErrorCode & APP_ERROR_GPS)
		GLCD_PutBitmap(99, 53, FTX100IconCheckFail, 0);
	else
		GLCD_PutBitmap(99, 53, FTX100IconCheckOk, 0);
}
