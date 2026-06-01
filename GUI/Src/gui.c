/**
 ******************************************************************************
 * @file        : gui.c
 * @author      : Estudiez, Ivan
 * @brief       : Graphic User Interface
 * @date		: 07/05/2026
 * @version		: 1.0
 ******************************************************************************
 *
 *
 *
 *
 ******************************************************************************
 */

#include "stddef.h"
#include "gui_utils.h"
#include "menu_list.h"
#include "input_box.h"
#include "list_box.h"
#include "msgbox.h"
#include "app.h"
#include "ftx100_main.h"
#include "ftx100_calib.h"

// ----------------------------------------------------------------------------
// Components of the graphical user interface
// ----------------------------------------------------------------------------
GUI_MenuList_t MenuList_MainMenu;
GUI_ListBox_t ListBox_Config;
GUI_ListBox_t ListBox_Doses;
GUI_ListBox_t ListBox_Pid;
GUI_MsgBox_t MsgBox_SaveConf;
GUI_FTX100App_t FTX100_MainScreen;
GUI_FTX100Calib_t FTX100_CalibScreen;

// ----------------------------------------------------------------------------

/**
 * ----------------------------------------------------------------------------
 * @brief		Initialization of the graphical components
 * ----------------------------------------------------------------------------
 */
void GUI_Init(void)
{
	// ------------------------------------------------------------------------
	// FTX100 APPLICATION SCREEN
	// ------------------------------------------------------------------------
	FTX100_MainScreen.priv.status = GUI_STATUS_ENABLED;
	FTX100_MainScreen.pRet = &MenuList_MainMenu;
	FTX100_MainScreen.maxSpeed = 180;
	FTX100_MainScreen.pMotorRpm = &APP_DisplayMotorRpm;
	FTX100_MainScreen.pTurbineRpm = &APP_DisplayTurbineRpm;
	FTX100_MainScreen.pDose = &APP_Dose;
	FTX100_MainScreen.pDosePreset = &APP_DoseSelectedPreset;
	FTX100_MainScreen.pEnable = &APP_AutoMode;
	FTX100_MainScreen.pSimuMode = &APP_SimuMode;
	FTX100_MainScreen.pSimuSpeed = &APP_SimuSpeed;
	FTX100_MainScreen.pSpeed = &APP_DisplaySpeed;

	// ------------------------------------------------------------------------
	// FTX100 CALIBRATION SCREEN
	// ------------------------------------------------------------------------
	FTX100_CalibScreen.pRet = &MenuList_MainMenu;
	FTX100_CalibScreen.pCalibDone = &APP_CalibDone;
	FTX100_CalibScreen.pProgressBar = &APP_CalibProgress;
	FTX100_CalibScreen.pEnabled = &APP_CalibScreen;
	FTX100_CalibScreen.pStart = &APP_CalibStart;

	// ------------------------------------------------------------------------
	// MAIN MENU
	// ------------------------------------------------------------------------
	MenuList_MainMenu.sTitle = "MENU PRINCIPAL";
	MenuList_MainMenu.pRet = &FTX100_MainScreen;
	MenuList_MainMenu.item[0].label = "PANTALLA APLICACION";
	MenuList_MainMenu.item[0].run = &FTX100_MainScreen;
	MenuList_MainMenu.item[1].label = "DOSIS GUARDADAS";
	MenuList_MainMenu.item[1].run = &ListBox_Doses;
	MenuList_MainMenu.item[2].label = "CALIBRACION";
	MenuList_MainMenu.item[2].run = &FTX100_CalibScreen;
	MenuList_MainMenu.item[3].label = "INFO SISTEMA";
	MenuList_MainMenu.item[3].run = NULL;
	MenuList_MainMenu.item[4].label = "CONFIGURACION GENERAL";
	MenuList_MainMenu.item[4].run = &ListBox_Config;
	MenuList_MainMenu.item[5].label = "CONFIGURACION PID";
	MenuList_MainMenu.item[5].run = &ListBox_Pid;
	MenuList_MainMenu.item[6].label = "GUARDAR CONFIGURACION";
	MenuList_MainMenu.item[6].run = &MsgBox_SaveConf;

	// ------------------------------------------------------------------------
	// LISTBOX DOSES
	// ------------------------------------------------------------------------
	ListBox_Doses.sTitle = "DOSIS GUARDADAS";
	ListBox_Doses.pRet = &MenuList_MainMenu;

	//Dose-1
	ListBox_Doses.item[0].sTile = "DOSIS-1";
	ListBox_Doses.item[0].pInputVar = &APP_DosePreset1;
	ListBox_Doses.item[0].dataType = GUI_UINT16;
	ListBox_Doses.item[0].maxValue = 5000;
	ListBox_Doses.item[0].decimalPos = 1;
	ListBox_Doses.item[0].numDigits = 4;
	ListBox_Doses.item[0].sUnit = "Kg/Ha";

	//Dose-2
	ListBox_Doses.item[1].sTile = "DOSIS-2";
	ListBox_Doses.item[1].pInputVar = &APP_DosePreset2;
	ListBox_Doses.item[1].dataType = GUI_UINT16;
	ListBox_Doses.item[1].maxValue = 5000;
	ListBox_Doses.item[1].decimalPos = 1;
	ListBox_Doses.item[1].numDigits = 4;
	ListBox_Doses.item[1].sUnit = "Kg/Ha";

	//Dose-3
	ListBox_Doses.item[2].sTile = "DOSIS-3";
	ListBox_Doses.item[2].pInputVar = &APP_DosePreset3;
	ListBox_Doses.item[2].dataType = GUI_UINT16;
	ListBox_Doses.item[2].maxValue = 5000;
	ListBox_Doses.item[2].decimalPos = 1;
	ListBox_Doses.item[2].numDigits = 4;
	ListBox_Doses.item[2].sUnit = "Kg/Ha";

	//Dose-4
	ListBox_Doses.item[3].sTile = "DOSIS-4";
	ListBox_Doses.item[3].pInputVar = &APP_DosePreset4;
	ListBox_Doses.item[3].dataType = GUI_UINT16;
	ListBox_Doses.item[3].maxValue = 5000;
	ListBox_Doses.item[3].decimalPos = 1;
	ListBox_Doses.item[3].numDigits = 4;
	ListBox_Doses.item[3].sUnit = "Kg/Ha";

	//Dose-5
	ListBox_Doses.item[4].sTile = "DOSIS-5";
	ListBox_Doses.item[4].pInputVar = &APP_DosePreset5;
	ListBox_Doses.item[4].dataType = GUI_UINT16;
	ListBox_Doses.item[4].maxValue = 5000;
	ListBox_Doses.item[4].decimalPos = 1;
	ListBox_Doses.item[4].numDigits = 4;
	ListBox_Doses.item[4].sUnit = "Kg/Ha";

	// ------------------------------------------------------------------------
	// LISTBOX CONFIGURATION
	// ------------------------------------------------------------------------
	ListBox_Config.sTitle = "CONFIGURACION GENERAL";
	ListBox_Config.pRet = &MenuList_MainMenu;

	// Labor width
	ListBox_Config.item[0].sTile = "ANCHO LABOR";
	ListBox_Config.item[0].pInputVar = &APP_Wingspan;
	ListBox_Config.item[0].dataType = GUI_UINT16;
	ListBox_Config.item[0].maxValue = 500;
	ListBox_Config.item[0].decimalPos = 1;
	ListBox_Config.item[0].numDigits = 3;
	ListBox_Config.item[0].sUnit = "m";

	// Number of nozzles
	ListBox_Config.item[1].sTile = "NUMERO DE BOQUILLAS";
	ListBox_Config.item[1].pInputVar = &APP_NumNozzles;
	ListBox_Config.item[1].dataType = GUI_UINT16;
	ListBox_Config.item[1].maxValue = 500;
	ListBox_Config.item[1].decimalPos = 0;
	ListBox_Config.item[1].numDigits = 3;
	ListBox_Config.item[1].sUnit = " ";

	// Turbine, pulses per revolution
	ListBox_Config.item[2].sTile = "PULSOS TURBINA";
	ListBox_Config.item[2].pInputVar = &APP_TurbinePulses;
	ListBox_Config.item[2].dataType = GUI_UINT16;
	ListBox_Config.item[2].maxValue = 900;
	ListBox_Config.item[2].decimalPos = 0;
	ListBox_Config.item[2].numDigits = 3;
	ListBox_Config.item[2].sUnit = "P/Rev";

	// Motor, pulses per revolution
	ListBox_Config.item[3].sTile = "PULSOS TURBINA";
	ListBox_Config.item[3].pInputVar = &APP_MotorPulses;
	ListBox_Config.item[3].dataType = GUI_UINT16;
	ListBox_Config.item[3].maxValue = 900;
	ListBox_Config.item[3].decimalPos = 0;
	ListBox_Config.item[3].numDigits = 3;
	ListBox_Config.item[3].sUnit = "P/Rev";

	// Minimum speed
	ListBox_Config.item[4].sTile = "VELOCIDAD MINIMA";
	ListBox_Config.item[4].pInputVar = &APP_MinSpeed;
	ListBox_Config.item[4].dataType = GUI_UINT16;
	ListBox_Config.item[4].maxValue = 100;
	ListBox_Config.item[4].decimalPos = 1;
	ListBox_Config.item[4].numDigits = 3;
	ListBox_Config.item[4].sUnit = "Km/h";

	// ------------------------------------------------------------------------
	// LISTBOX PID CONFIGURATION
	// ------------------------------------------------------------------------
	ListBox_Pid.sTitle = "CONFIGURACION PID";
	ListBox_Pid.pRet = &MenuList_MainMenu;

	// Maximum value of the integral term
	ListBox_Pid.item[0].sTile = "MAX. INTEGRAL";
	ListBox_Pid.item[0].pInputVar = &APP_PidMaxInt;
	ListBox_Pid.item[0].dataType = GUI_UINT16;
	ListBox_Pid.item[0].maxValue = 1000;
	ListBox_Pid.item[0].decimalPos = 0;
	ListBox_Pid.item[0].numDigits = 4;

	// KP (Proportional term gain)
	ListBox_Pid.item[1].sTile = "KP (PROPORCIONAL)";
	ListBox_Pid.item[1].pInputVar = &APP_PidKp;
	ListBox_Pid.item[1].dataType = GUI_INT32;
	ListBox_Pid.item[1].maxValue = 10000;
	ListBox_Pid.item[1].decimalPos = 0;
	ListBox_Pid.item[1].numDigits = 5;

	// KI (Integral term gain)
	ListBox_Pid.item[2].sTile = "KI (INTEGRAL)";
	ListBox_Pid.item[2].pInputVar = &APP_PidKi;
	ListBox_Pid.item[2].dataType = GUI_INT32;
	ListBox_Pid.item[2].maxValue = 10000;
	ListBox_Pid.item[2].decimalPos = 0;
	ListBox_Pid.item[2].numDigits = 5;

	// KD (Derivative term gain)
	ListBox_Pid.item[3].sTile = "KD (DERIVATIVO)";
	ListBox_Pid.item[3].pInputVar = &APP_PidKd;
	ListBox_Pid.item[3].dataType = GUI_INT32;
	ListBox_Pid.item[3].maxValue = 10000;
	ListBox_Pid.item[3].decimalPos = 0;
	ListBox_Pid.item[3].numDigits = 5;

	// Offset
	ListBox_Pid.item[4].sTile = "OFFSET";
	ListBox_Pid.item[4].pInputVar = &APP_PidOffset;
	ListBox_Pid.item[4].dataType = GUI_INT32;
	ListBox_Pid.item[4].maxValue = 1000;
	ListBox_Pid.item[4].decimalPos = 0;
	ListBox_Pid.item[4].numDigits = 4;

	// ------------------------------------------------------------------------
	// MSGBOX SAVE CONFIGURATION
	// ------------------------------------------------------------------------
	MsgBox_SaveConf.pRet = &MenuList_MainMenu;
	MsgBox_SaveConf.type = MSGBOX_WARNING;
	MsgBox_SaveConf.sMsgLine1 = "Guardar";
	MsgBox_SaveConf.sMsgLine2 = "configuracion?";
	MsgBox_SaveConf.pValue = &APP_SaveConfigRequest;
}

/**
 * ----------------------------------------------------------------------------
 * @brief		Run the graphics components
 * ----------------------------------------------------------------------------
 */
void GUI_Update(void)
{
	GUI_FTX100MainScreen(&FTX100_MainScreen);
	GUI_FTX100CalibScreen(&FTX100_CalibScreen);
	GUI_MenuList(&MenuList_MainMenu);
	GUI_ListBox(&ListBox_Doses);
	GUI_ListBox(&ListBox_Config);
	GUI_ListBox(&ListBox_Pid);
	GUI_MsgBox(&MsgBox_SaveConf);

	// Run all input boxes
	GUI_InputBox(GUI_InputBox_DefaultPointer);
}
