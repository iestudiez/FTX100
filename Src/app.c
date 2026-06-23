/**
 ******************************************************************************
 * @file        : app.c
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

#include "app.h"
#include "eeprom.h"
#include "indicators.h"
#include "pwrboard.h"
#include "gnss.h"
#include "global.h"
#include "spi.h"

// Definitions
// ----------------------------------------------------------------------------
#define APP_EEPROM_CONFIG_PAGE			(8U)
#define APP_EEPROM_BUFF_SIZE			(32U)
#define APP_DISPLAY_REFRESH_RATE		(10U)
#define APP_MAX_TEMPERATURE				(60U)
#define APP_MAX_TURBINE_RPM				(6000U)
#define APP_PID_ALLOWED_ERR_TIME		(200U)

// Application public variables
// =----------------------------------------------------------------------------
bool APP_AutoMode = false;
bool APP_EnableMotor = false;
bool APP_SaveConfigRequest = false;
bool APP_CalibScreen = false;
bool APP_CalibStart = false;
bool APP_CalibDone = false;
bool APP_SimuMode = false;
// ---------------------------
uint8_t APP_ErrorCode = 0;
uint8_t APP_NumNozzles;
uint8_t APP_Eeprom[APP_EEPROM_BUFF_SIZE];
// ---------------------------
uint16_t APP_Speed;
uint16_t APP_MinSpeed;
uint32_t APP_RpmSetpoint = 0;
// ---------------------------
uint16_t APP_Dose;
uint16_t APP_DosePreset1;
uint16_t APP_DosePreset2;
uint16_t APP_DosePreset3;
uint16_t APP_DosePreset4;
uint16_t APP_DosePreset5;
uint8_t APP_DoseSelectedPreset = DOSE_PRESET_1;
// ---------------------------
// Calibration parameters
uint16_t APP_CalibValue;
uint16_t APP_CalibRevolutions = 10;
uint16_t APP_CalibPwm = 200;
uint16_t APP_CalibRevCounter;
uint8_t APP_CalibProgress;
// ---------------------------
uint16_t APP_Wingspan;
uint16_t APP_MotorPulses;
uint16_t APP_TurbinePulses;
// ---------------------------
uint16_t APP_SimuSpeed;
uint16_t APP_DisplaySpeed;
uint16_t APP_DisplayMotorRpm;
uint16_t APP_DisplayTurbineRpm;
// ---------------------------
PID_Vars_t APP_PidMotor;
uint16_t APP_PidMaxInt = 500;
uint16_t APP_PidOffset = 0;
uint32_t APP_PidKp;
uint32_t APP_PidKi;
uint32_t APP_PidKd;

// Private variables
// -----------------------------------------------------------------------------
uint16_t app_KQ;

// Function Prototypes
// -----------------------------------------------------------------------------
void app_PID(void);
void app_SaveConfiguration(void);
void app_LoadConfiguration(void);
void app_Calibration(void);
void app_UpdateParameters(void);
void app_IndicatorsUpdate(void);
void app_ErrorReport(void);
void app_DisplayValues(void);
void app_MachineSpeed(void);
void app_RpmSetpoint(void);
void app_SelectDose(void);

/**
 * -----------------------------------------------------------------------------
 * @brief 	User Application Initialization
 * -----------------------------------------------------------------------------
 */
void APP_Init(void)
{
	// Load EEPROM Saved Parameters
	// ----------------------------
	app_LoadConfiguration();
	PowerBoard.Freq[0].Div = APP_MotorPulses;
	PowerBoard.Freq[1].Div = APP_TurbinePulses;

	// PID Configuration
	APP_PidMotor.Kp = APP_PidKp;
	APP_PidMotor.Ki = APP_PidKi;
	APP_PidMotor.Kd = APP_PidKd;
	APP_PidMotor.maxIntegral = APP_PidMaxInt;
	APP_PidMotor.offset = APP_PidOffset;
	APP_PidMotor.allowedErr = 5;
	APP_PidMotor.pEnable = &APP_EnableMotor;
	APP_PidMotor.pOutput = &PowerBoard.Out[0].DutyCycle;
	APP_PidMotor.pFeedback = &PowerBoard.Freq[0].Rpm;
	APP_PidMotor.pSetpoint = &APP_RpmSetpoint;

	// Set the LEDs and the screen backlight.
	Indicator.Led.Power = LED_OFF;
	Indicator.Led.Message = LED_OFF;
	Indicator.Led.Error = LED_OFF;
	Indicator.Backlight.Green = LED_ON;
	Indicator.Backlight.Red = LED_OFF;
	Indicator.Backlight.Blue = LED_ON;

	// Sensors Power Supply
	PowerBoard.SensorSupply.External = PWR_ON;
	PowerBoard.SensorSupply.Internal = PWR_ON;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	User application task
 * -----------------------------------------------------------------------------
 */
void APP_User(void)
{
	// Update LEDs status
	app_IndicatorsUpdate();

	// Update application parameters
	app_UpdateParameters();

	// Calibration routine
	// -------------------
	if (APP_CalibScreen)
	{
		// Disable automatic mode
		APP_AutoMode = false;

		// Disable valve output
		if (!APP_CalibStart)
			PowerBoard.Out[0].DutyCycle = 0;

		// Change Freq[0] to counter mode
		if (PowerBoard.Freq[0].Mode != INPUT_COUNTER_MODE)
			PowerBoard.Freq[0].Mode = INPUT_COUNTER_MODE;

		app_Calibration();
		return;
	}

	// Check frequency input mode
	if (PowerBoard.Freq[0].Mode != INPUT_FREQUENCY_MODE)
		PowerBoard.Freq[0].Mode = INPUT_FREQUENCY_MODE;

	if (PowerBoard.Freq[1].Mode != INPUT_FREQUENCY_MODE)
		PowerBoard.Freq[1].Mode = INPUT_FREQUENCY_MODE;

	// Estimate machine speed
	app_MachineSpeed();

	// Compute setpoint
	app_RpmSetpoint();

	// Perform PID control
	app_PID();

	// Report error code
	app_ErrorReport();

	// Update values ​​for on-screen display
	app_DisplayValues();

	// Select the current dose
	app_SelectDose();

	// Save configuration
	if (APP_SaveConfigRequest)
		app_SaveConfiguration();
}

/**
 * -----------------------------------------------------------------------------
 * @brief	PID calculation task
 * -----------------------------------------------------------------------------
 */
void app_PID(void)
{
	if (APP_CalibScreen)
		return;

	// Enable the hydraulic motor only when the speed is greater than the minimum speed.
	if ((APP_Speed >= APP_MinSpeed) && APP_AutoMode)
		APP_EnableMotor = true;
	else
		APP_EnableMotor = false;

	// Perform PID calculations
	PID_Pwm1000(&APP_PidMotor);
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Perform calibration routine
 *-----------------------------------------------------------------------------
 */
void app_Calibration(void)
{
	uint16_t targetRevolutions;

	if (!APP_CalibStart)
		return;

	// Activate hydraulic motor
	PowerBoard.Out[0].DutyCycle = APP_CalibPwm;

	// Read frequency counter
	APP_CalibRevCounter = PowerBoard.Freq[0].Counter;

	// Calculate the target revolutions
	targetRevolutions = APP_CalibRevolutions * APP_MotorPulses;

	// Calibration progress
	APP_CalibProgress = (uint8_t) ((uint32_t) (APP_CalibRevCounter * 100) / targetRevolutions);

	if (APP_CalibRevCounter >= targetRevolutions)
	{
		// Stop hydraulic motor
		PowerBoard.Out[0].DutyCycle = 0;

		// End calibration routine
		APP_CalibStart = false;

		// Reset counter
		PowerBoard.Freq[0].Reset = true;

		// Calibration process done
		APP_CalibDone = true;
		APP_CalibProgress = 100;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Save parameters in EEPROM memory
 * -----------------------------------------------------------------------------
 */
void app_SaveConfiguration(void)
{
	APP_SaveConfigRequest = false;

	EEPROM_StoreWord(&APP_Eeprom[0], APP_DosePreset1);
	EEPROM_StoreWord(&APP_Eeprom[2], APP_DosePreset2);
	EEPROM_StoreWord(&APP_Eeprom[4], APP_DosePreset3);
	EEPROM_StoreWord(&APP_Eeprom[6], APP_DosePreset4);
	EEPROM_StoreWord(&APP_Eeprom[8], APP_DosePreset5);

	EEPROM_StoreWord(&APP_Eeprom[10], APP_PidMaxInt);
	EEPROM_StoreWord(&APP_Eeprom[12], APP_PidOffset);
	EEPROM_StoreWord(&APP_Eeprom[14], APP_PidKp);
	EEPROM_StoreWord(&APP_Eeprom[16], APP_PidKi);
	EEPROM_StoreWord(&APP_Eeprom[18], APP_PidKd);

	EEPROM_StoreWord(&APP_Eeprom[20], APP_Wingspan);
	EEPROM_StoreWord(&APP_Eeprom[22], APP_MotorPulses);
	EEPROM_StoreWord(&APP_Eeprom[24], APP_TurbinePulses);

	EEPROM_StoreWord(&APP_Eeprom[26], APP_CalibValue);
	APP_Eeprom[28] = APP_NumNozzles;
	APP_Eeprom[29] = APP_DoseSelectedPreset;
	APP_Eeprom[30] = (uint8_t) APP_MinSpeed;

	// Save data to EEPROM
	EEPROM_Write(EEPROM_GetPageAddress(APP_EEPROM_CONFIG_PAGE), APP_Eeprom, APP_EEPROM_BUFF_SIZE);

	// Update PID parameters
	app_UpdateParameters();
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Load parameters from EEPROM memory
 * -----------------------------------------------------------------------------
 */
void app_LoadConfiguration(void)
{
	// Load data from EEPROM
	EEPROM_Read(EEPROM_GetPageAddress(APP_EEPROM_CONFIG_PAGE), APP_Eeprom, APP_EEPROM_BUFF_SIZE);

	// Extract data
	APP_DosePreset1 = EEPROM_GetWord(&APP_Eeprom[0]);
	APP_DosePreset2 = EEPROM_GetWord(&APP_Eeprom[2]);
	APP_DosePreset3 = EEPROM_GetWord(&APP_Eeprom[4]);
	APP_DosePreset4 = EEPROM_GetWord(&APP_Eeprom[6]);
	APP_DosePreset5 = EEPROM_GetWord(&APP_Eeprom[8]);

	APP_PidMaxInt = EEPROM_GetWord(&APP_Eeprom[10]);
	APP_PidOffset = EEPROM_GetWord(&APP_Eeprom[12]);
	APP_PidKp = EEPROM_GetWord(&APP_Eeprom[14]);
	APP_PidKi = EEPROM_GetWord(&APP_Eeprom[16]);
	APP_PidKd = EEPROM_GetWord(&APP_Eeprom[18]);

	APP_Wingspan = EEPROM_GetWord(&APP_Eeprom[20]);
	APP_MotorPulses = EEPROM_GetWord(&APP_Eeprom[22]);
	APP_TurbinePulses = EEPROM_GetWord(&APP_Eeprom[24]);

	APP_CalibValue = EEPROM_GetWord(&APP_Eeprom[26]);
	APP_NumNozzles = APP_Eeprom[28];
	APP_DoseSelectedPreset = APP_Eeprom[29];
	APP_MinSpeed = (uint16_t) APP_Eeprom[30];
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Update application parameters
 * -----------------------------------------------------------------------------
 */
void app_UpdateParameters(void)
{
	PowerBoard.Freq[0].Div = APP_MotorPulses;
	PowerBoard.Freq[1].Div = APP_TurbinePulses;
	APP_PidMotor.Kp = APP_PidKp;
	APP_PidMotor.Ki = APP_PidKi;
	APP_PidMotor.Kd = APP_PidKd;
	APP_PidMotor.maxIntegral = APP_PidMaxInt;
	APP_PidMotor.offset = APP_PidOffset;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Update LEDs status
 * -----------------------------------------------------------------------------
 */
void app_IndicatorsUpdate(void)
{
	static bool pidError = false;

	// POWER LED
	// ---------------------------------------------

	// Automatic/ calibration mode
	if (APP_AutoMode)
	{
		if (APP_Speed >= APP_MinSpeed)
			Indicator.Led.Power = LED_BLINK;
		else
			Indicator.Led.Power = LED_ON;
	}
	else if (APP_CalibStart)
		Indicator.Led.Power = LED_BLINK;
	else
		Indicator.Led.Power = LED_OFF;

	// MESSAGE LED
	// ---------------------------------------------

	// Simulation mode
	if (APP_SimuMode)
		Indicator.Led.Message = LED_BLINK;
	else
		Indicator.Led.Message = LED_OFF;

	// ERROR LED
	// ---------------------------------------------

	// Error messages
	if (APP_ErrorCode)
		Indicator.Led.Error = LED_BLINK;
	else
		Indicator.Led.Error = LED_OFF;

	// BUZZER
	// ---------------------------------------------

	// Activate alarm
	if (APP_ErrorCode & APP_ERROR_PID)
	{
		Indicator.Buzzer = BUZZER_ALARM;
		pidError = true;
		return;
	}

	// Turn off Alarm
	if (pidError)
	{
		pidError = false;
		Indicator.Buzzer = BUZZER_OFF;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Set the error code variable
 * -----------------------------------------------------------------------------
 */
void app_ErrorReport(void)
{
	// Check PID error
	if (APP_PidMotor.err >= APP_PID_ALLOWED_ERR_TIME)
		APP_ErrorCode |= APP_ERROR_PID;
	else
		APP_ErrorCode &= ~APP_ERROR_PID;

	// Check GNSS module timeout
	if (GNSS_Status.rxSerialOk)
		APP_ErrorCode &= ~APP_ERROR_GPS;
	else
		APP_ErrorCode |= APP_ERROR_GPS;

	// Check temperature
	if (PowerBoard.Status.Temp >= (APP_MAX_TEMPERATURE * 10))
		APP_ErrorCode |= APP_ERROR_TEMP;
	else
		APP_ErrorCode &= ~APP_ERROR_TEMP;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Display the values ​​at a specific refresh rate
 * -----------------------------------------------------------------------------
 */
void app_DisplayValues(void)
{
	static uint8_t refreshRateCnt = 0;

	if (refreshRateCnt == APP_DISPLAY_REFRESH_RATE)
	{
		APP_DisplaySpeed = APP_Speed;
		APP_DisplayMotorRpm = PowerBoard.Freq[0].Rpm / 10;

		if (PowerBoard.Freq[1].Rpm < APP_MAX_TURBINE_RPM)
			APP_DisplayTurbineRpm = PowerBoard.Freq[1].Rpm / 10;
		else
			APP_DisplayTurbineRpm = APP_MAX_TURBINE_RPM;

		refreshRateCnt = 0;
	}
	else
		refreshRateCnt++;
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Select the dose to apply
 * -----------------------------------------------------------------------------
 */
void app_SelectDose(void)
{
	switch (APP_DoseSelectedPreset)
	{
	case DOSE_PRESET_1:
		APP_Dose = APP_DosePreset1;
		break;

	case DOSE_PRESET_2:
		APP_Dose = APP_DosePreset2;
		break;

	case DOSE_PRESET_3:
		APP_Dose = APP_DosePreset3;
		break;

	case DOSE_PRESET_4:
		APP_Dose = APP_DosePreset4;
		break;

	case DOSE_PRESET_5:
		APP_Dose = APP_DosePreset5;
		break;
	}
}

/**
 * -----------------------------------------------------------------------------
 * @brief	Set the machine speed
 * -----------------------------------------------------------------------------
 */
void app_MachineSpeed(void)
{
	// Simulation Mode
	if (APP_SimuMode)
	{
		APP_Speed = APP_SimuSpeed;
		return;
	}

	// GPS Mode
	if ((GNSS_Rmc.mode == GNSS_RMC_MODE_A) | (GNSS_Rmc.mode == GNSS_RMC_MODE_D) | (GNSS_Rmc.mode == GNSS_RMC_MODE_E))
		APP_Speed = (GNSS_Rmc.speed) / 10;
	else
		APP_Speed = 0;
}

/**
 * -----------------------------------------------------------------------------
 * @brief 	Perform RPM calculation based on the prescribed dose
 * -----------------------------------------------------------------------------
 */
void app_RpmSetpoint(void)
{
	app_KQ = APP_CalibValue * APP_NumNozzles;

	if (app_KQ > 0)
		APP_RpmSetpoint = (APP_Speed * APP_Dose * APP_Wingspan) / (app_KQ * 6);
	else
		APP_RpmSetpoint = 0;
}
