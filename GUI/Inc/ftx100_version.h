/**
 ******************************************************************************
 * @file        : ftx100_version.h
 * @author      : Ivan.Estudiez
 * @brief       : Version information screen
 * @date        : 2 jun 2026
 * @version     : 0.
 ******************************************************************************
 */
#ifndef INC_FTX100_VERSION_H_
#define INC_FTX100_VERSION_H_

#include "stdbool.h"
#include "stdint.h"

/**
 * ----------------------------------------------------------------------------
 * @brief  Version Screen private settings
 * ----------------------------------------------------------------------------
 */
typedef struct FTX100VersionPrivate_t
{
	uint8_t status;
	bool redraw;
	bool init;
} FTX100VersionPrivate_t;

typedef struct GUI_FTX100Version_t
{
	FTX100VersionPrivate_t priv;
	char *sCustomer;
	char *sFirmware;
	char *sSoftware;
	char *sHardware1;
	char *sHardware2;
	void *pRet;
} GUI_FTX100Version_t;

// Public functions
// ----------------------------------------------------------------------------
void GUI_FTX100VersionScreen(GUI_FTX100Version_t *info);

#endif /* INC_FTX100_VERSION_H_ */
