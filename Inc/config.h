/**
 ******************************************************************************
 * @file        : config.h
 * @author      : Ivan.Estudiez
 * @brief       : Configuration file
 * @date        : 21 ago 2026
 * @version     : 0.1
 ******************************************************************************
 */
#ifndef CONFIG_H_
#define CONFIG_H_

#include "app.h"

// ----------------------------------------------------------------------------
// Application parameters
// ----------------------------------------------------------------------------

// Dose
// ----------------------------------------------------------------------------
#define	CONFIG_SELECTED_DOSE					(DOSE_PRESET_1)
#define	CONFIG_DOSE_1							(50)
#define	CONFIG_DOSE_2							(100)
#define	CONFIG_DOSE_3							(150)
#define	CONFIG_DOSE_4							(200)
#define	CONFIG_DOSE_5							(250)

// Calibration
// ----------------------------------------------------------------------------
#define CONFIG_CALIB_REV						(10)
#define CONFIG_CALIB_PWM						(300)

// Machine parameters
// ----------------------------------------------------------------------------
#define CONFIG_NUM_NOZZLES						(9)
#define CONFIG_MOTOR_PULSES_REV					(24)
#define CONFIG_TURBINE_PULSES_REV				(1)
#define CONFIG_WINGSPAN							(120)

// PID parameters
// ----------------------------------------------------------------------------
#define CONFIG_PID_KP							(500)
#define CONFIG_PID_KI							(50)
#define CONFIG_PID_KD							(50)
#define CONFIG_PID_KP_DIV						(10000)
#define CONFIG_PID_KI_DIV						(10000)
#define CONFIG_PID_KD_DIV						(1000)
#define CONFIG_PID_ITERM_MAX					(650)
#define CONFIG_PID_OUT_MIN						(180)
#define CONFIG_PID_OUT_MAX						(180)
#define CONFIG_PID_ALLOWED_ERR_TIME				(200)

#endif /* CONFIG_H_ */
