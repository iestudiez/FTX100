/**
 ******************************************************************************
 * @file        : ftx100_version.c
 * @author      : Ivan.Estudiez
 * @brief       : Version information screen
 * @date        : 2 jun 2026
 * @version     : 0.1
 ******************************************************************************
 */

#include "stdio.h"
#include "ftx100_version.h"
#include "gui_utils.h"
#include "keyboard.h"
#include "glcd.h"
#include "fonts.h"
#include "ftx100_bitmaps.h"
#include "system_cx100.h"

// Private functions
// ----------------------------------------------------------------------------
void ftx100Version_Init(GUI_FTX100Version_t *info);
bool ftx100Version_KeyActions(GUI_FTX100Version_t *info);
void ftx100Version_Draw(GUI_FTX100Version_t *info);

/**
 * -----------------------------------------------------------------------------
 * @brief			FTX100 Fertilizer info screen
 *-----------------------------------------------------------------------------
 * @param info		Pointer to FTX100 info screen structure
 * -----------------------------------------------------------------------------
 */
void GUI_FTX100VersionScreen(GUI_FTX100Version_t *info)
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
		ftx100Version_Init(info);

	// Process keyboard actions
	if (ftx100Version_KeyActions(info))
		return;

	// Draw the menu on screen
	if (info->priv.redraw)
		ftx100Version_Draw(info);
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				FTX100 version screen initialization
 * ----------------------------------------------------------------------------
 * @param info			Pointer to FTX100Version type structure
 * ----------------------------------------------------------------------------
 */
void ftx100Version_Init(GUI_FTX100Version_t *info)
{
	info->priv.init = true;
	info->priv.redraw = true;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Perform keyboard actions
 * ----------------------------------------------------------------------------
 * @param info			Pointer to FTX100Version type structure
 * @return				False = Continue, True = Exit current control.
 * ----------------------------------------------------------------------------
 */
bool ftx100Version_KeyActions(GUI_FTX100Version_t *info)
{
	// Escape action
	if (KEYBOARD_Event.Esc == 1)
	{
		GUI_TransferControl(&info->priv.status, info->pRet);
		info->priv.redraw = true;
		return true;
	}

	return false;
}

/**
 * ----------------------------------------------------------------------------
 * @brief 				Draw FTX100 Version screen
 * ----------------------------------------------------------------------------
 * @param info			Pointer to FTX100Version type structure
 * ----------------------------------------------------------------------------
 */
void ftx100Version_Draw(GUI_FTX100Version_t *info)
{
	const uint8_t TEXT_XPOS = 48;
	const uint8_t ROW1_YPOS = 11;
	const uint8_t ROW2_YPOS = 21;
	const uint8_t ROW3_YPOS = 31;
	const uint8_t ROW4_YPOS = 41;
	const uint8_t ROW5_YPOS = 51;

	// Draw Info Screen
	GLCD_ScreenBitmap(FTX100Version_Screen_128x64);
	SYSTEM_DelayUs(5);

	// Print customer name
	// ------------------------------------------------------------------------
	GLCD_DrawText(info->sCustomer, TEXT_XPOS, ROW1_YPOS, Font_Small, 0, 1);

	// Print firmware version
	// ------------------------------------------------------------------------
	GLCD_DrawText(info->sFirmware, TEXT_XPOS, ROW2_YPOS, Font_Small, 0, 1);

	// Print software version
	// ------------------------------------------------------------------------
	GLCD_DrawText(info->sSoftware, TEXT_XPOS, ROW3_YPOS, Font_Small, 0, 1);

	// Print hardware-1 version
	// ------------------------------------------------------------------------
	GLCD_DrawText(info->sHardware1, TEXT_XPOS, ROW4_YPOS, Font_Small, 0, 1);

	// Print hardware-2 version
	// ------------------------------------------------------------------------
	GLCD_DrawText(info->sHardware2, TEXT_XPOS, ROW5_YPOS, Font_Small, 0, 1);
}

