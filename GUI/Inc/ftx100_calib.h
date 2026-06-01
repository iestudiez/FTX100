/**
 ******************************************************************************
 * @file        : ftx100_calib.h
 * @author      : Ivan.Estudiez
 * @brief       : Calibration screen
 * @date        : 29 may 2026
 * @version     : 0.
 ******************************************************************************
 */

#ifndef INC_FTX100_CALIB_H_
#define INC_FTX100_CALIB_H_

#include "stdbool.h"
#include "stdint.h"

// Public definitions
// ----------------------------------------------------------------------------
#define CALIBRATION_MAX_VALUE			(5000U)

/**
 * ----------------------------------------------------------------------------
 * @brief  Calibration Screen private settings
 * ----------------------------------------------------------------------------
 */
typedef struct FTX100CalibPrivate_t
{
	uint8_t status;
	bool redraw;
	bool init;
} FTX100CalibPrivate_t;

/**
 * ----------------------------------------------------------------------------
 * @brief  Calibration Screen for Fertilizer Spreader
 * ----------------------------------------------------------------------------
 */
typedef struct GUI_FTX100Calib_t
{
	FTX100CalibPrivate_t priv;
	uint8_t *pProgressBar;
	uint16_t *pCalibPwm;
	bool *pEnabled;
	bool *pStart;
	bool *pCalibDone;
	void *pRet;
} GUI_FTX100Calib_t;

// Public functions
// ----------------------------------------------------------------------------
void GUI_FTX100CalibScreen(GUI_FTX100Calib_t *calibScreen);

#endif /* INC_FTX100_CALIB_H_ */
