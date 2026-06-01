/**
 ******************************************************************************
 * @file        : app_main.h
 * @author      : Ivan.Estudiez
 * @brief       : 
 * @date        : 27 may 2026
 * @version     : 0.
 ******************************************************************************
 */

#ifndef INC_FTX100_MAIN_H_
#define INC_FTX100_MAIN_H_

#include "stdbool.h"
#include "stdint.h"

/**
 * ----------------------------------------------------------------------------
 * @brief  Application Screen private settings
 * ----------------------------------------------------------------------------
 */
typedef struct FTX100AppPrivate_t
{
	uint8_t status;
	bool redraw;
	bool init;
} FTX100AppPrivate_t;

/**
 * ----------------------------------------------------------------------------
 * @brief  Application Screen for Fertilizer Spreader
 * ----------------------------------------------------------------------------
 */
typedef struct GUI_FTX100App_t
{
	FTX100AppPrivate_t priv;
	uint16_t *pSpeed;
	uint16_t *pDose;
	uint16_t *pMotorRpm;
	uint16_t *pTurbineRpm;
	uint16_t *pSimuSpeed;
	uint16_t maxSpeed;
	uint8_t *pDosePreset;
	bool *pEnable;
	bool *pSimuMode;
	void *pRet;
} GUI_FTX100App_t;

void GUI_FTX100MainScreen(GUI_FTX100App_t *app);

#endif /* INC_FTX100_MAIN_H_ */
