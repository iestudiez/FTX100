/**
 ******************************************************************************
 * @file        : ftx100_info.h
 * @author      : Ivan.Estudiez
 * @brief       : Info Screen FTX100
 * @date        : 2 jun 2026
 * @version     : 0.1
 ******************************************************************************
 */
#ifndef INC_FTX100_INFO_H_
#define INC_FTX100_INFO_H_

#include "stdbool.h"
#include "stdint.h"

/**
 * ----------------------------------------------------------------------------
 * @brief  Application Screen private settings
 * ----------------------------------------------------------------------------
 */
typedef struct FTX100InfoPrivate_t
{
	uint8_t status;
	bool redraw;
	bool init;
} FTX100InfoPrivate_t;

typedef struct GUI_FTX100Info_t
{
	FTX100InfoPrivate_t priv;
	uint16_t *pVoltage;
	uint16_t *pTemp;
	uint16_t *pCurrent;
	uint8_t *pErrorCode;
	void *pRet;
} GUI_FTX100Info_t;

// Public functions
// ----------------------------------------------------------------------------
void GUI_FTX100InfoScreen(GUI_FTX100Info_t *info);

#endif /* INC_FTX100_INFO_H_ */
