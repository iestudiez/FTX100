/**
 ******************************************************************************
 * @file        : app.h
 * @author      : Estudiez, Ivan
 * @brief       : User Application
 * @date        : 04/11/2024
 * @version     : 1.0
 ******************************************************************************
 *
 *
 *
 *
 *
 ******************************************************************************
 */

#ifndef APP_H_
#define APP_H_

#include <stdint.h>
#include <stdbool.h>
#include "pid.h"

// Definitions
// ----------------------------------------------------------------------------
#define APP_GNSS_TIMEOUT				(500U)
#define APP_ERROR_PID					(0x01)
#define APP_ERROR_GPS					(0x02)
#define APP_ERROR_CAN					(0x04)
#define APP_ERROR_TEMP					(0x08)
#define APP_MAX_MINIMUM_SPEED			(40U)

typedef enum DoseSelection_e
{
	DOSE_PRESET_1 = 1,
	DOSE_PRESET_2 = 2,
	DOSE_PRESET_3 = 3,
	DOSE_PRESET_4 = 4,
	DOSE_PRESET_5 = 5
} DoseSelection_e;

// ----------------------------------------------------------------------------
extern bool APP_AutoMode;
extern bool APP_EnableMotor;
extern bool APP_SaveConfigRequest;
extern bool APP_CalibScreen;
extern bool APP_CalibStart;
extern bool APP_CalibDone;
extern bool APP_SimuMode;
// ----------------------------------------------------------------------------
extern uint8_t APP_NumNozzles;
extern uint8_t APP_CalibSelectedPreset;
extern uint8_t APP_DoseSelectedPreset;
extern uint8_t APP_ErrorCode;
// ----------------------------------------------------------------------------
extern uint16_t APP_Speed;
extern uint16_t APP_MinSpeed;
extern uint32_t APP_RpmSetpoint;
// ----------------------------------------------------------------------------
extern uint16_t APP_Dose;
extern uint16_t APP_DosePreset1;
extern uint16_t APP_DosePreset2;
extern uint16_t APP_DosePreset3;
extern uint16_t APP_DosePreset4;
extern uint16_t APP_DosePreset5;
// ----------------------------------------------------------------------------
extern uint16_t APP_CalibValue;
extern uint16_t APP_CalibPwm;
// ----------------------------------------------------------------------------
extern uint16_t APP_SimuSpeed;
extern uint16_t APP_DisplaySpeed;
extern uint16_t APP_DisplayMotorRpm;
extern uint16_t APP_DisplayTurbineRpm;
// ----------------------------------------------------------------------------
extern uint16_t APP_Wingspan;
extern uint16_t APP_MotorPulses;
extern uint16_t APP_TurbinePulses;
extern uint16_t APP_CalibRevolutions;
extern uint8_t APP_CalibProgress;
// ----------------------------------------------------------------------------
extern PID_Vars_t APP_PidMotor;
extern uint16_t APP_PidMaxInt;
extern uint16_t APP_PidOffset;
extern uint32_t APP_PidKp;
extern uint32_t APP_PidKi;
extern uint32_t APP_PidKd;

void APP_Init(void);
void APP_User(void);

#endif /* APP_H_ */
