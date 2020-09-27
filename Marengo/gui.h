/*
 * gui.h
 *
 *  Created on: 26 mar 2019
 *      Author: grzeg
 */

#ifndef MARENGO_GUI_H_
#define MARENGO_GUI_H_

#include "gfx.h"
#include "gui/dialwidget.h"
#include "StepperManager.h"
#include "HeaterProxy.h"

typedef struct {
	char* name;
	coord_t width;
	coord_t height;
	font_t font;
} guiConsoleConf_t;

bool_t guibInitialised;

// Color schemes
// Standard/enabled colors
#define COLOR1					HTML2COLOR(0x595959) // Background
#define COLOR2					HTML2COLOR(0x737373) // Edge
#define COLOR3					HTML2COLOR(0xebf5ee) // Anti-flash white Text
#define COLOR4					HTML2COLOR(0x40403F) // Fill
#define COLOR5					HTML2COLOR(0xA4A5A6) // Focus
// Dimmed colors for disabled colors
#define COLOR6					HTML2COLOR(0x111218) // Licorice Background
#define COLOR7					HTML2COLOR(0x4b120b) // French Puce Edge
#define COLOR8					HTML2COLOR(0x565a57) // Davy's grey Text
#define COLOR9					HTML2COLOR(0x464c54) // Outer space Foreground
#define COLOR10					HTML2COLOR(0x1f0d19) // Licorice

static GWidgetStyle guiWidgetStyle = {
	COLOR1, // Background
	COLOR5, // Focus
	{
	COLOR3, // Text
	COLOR5, // Edge
	COLOR4, // Fill
	COLOR2, // Progress
}, // Color set when enabled
	{
	COLOR8, // Text
	COLOR7, // Edge
	COLOR9, // Fill
	COLOR10, // Progress
}, // Color set when disabled
	{
	COLOR3, // text
	COLOR5, // edge
	COLOR5, // fill
	COLOR2, // progress
}, // Color set when pressed
};


// Window parameters
static coord_t                  guiWindowHeight;
static coord_t                  guiWindowWidth;

// Main menu
static GHandle                  guiMainMenuHandle;
static GHandle                  guiMainMenuStatusButtonHandle;
static GHandle                  guiMainMenuMotionButtonHandle;
static GHandle                  guiMainMenuTemperatureButtonHandle;
static GHandle                  guiMainMenuWifiButtonHandle;
static GHandle                  guiMainMenuConsoleButtonHandle;
static GHandle                  guiMainMenuSettingsButtonHandle;
static void guiMainMenuShow(void);

// Status window
static GHandle				    guiStatusWindow;
static GHandle					guiStatusWindowState;
static GHandle					guiStatusWindowStopButton;
static GHandle                  guiStatusWindowCoordX;
static GHandle                  guiStatusWindowCoordY;
static GHandle                  guiStatusWindowCoordZ;
static GHandle                  guiStatusWindowFeedrate;
static GHandle                  guiStatusWindowLabelX;
static GHandle                  guiStatusWindowLabelY;
static GHandle                  guiStatusWindowLabelZ;
static GHandle                  guiStatusWindowLabelF;
static GHandle                  guiStatusWindowElaspedTimeTextLabel;
static GHandle                  guiStatusWindowElaspedTimeFieldLabel;
static GHandle                  guiStatusWindowRemainingTimeTextLabel;
static GHandle                  guiStatusWindowRemainingTimeFieldLabel;
static GHandle                  guiStatusWindowProgressLabel;
static GHandle                  guiStatusWindowProgressVal;
static GHandle                  guiStatusWindowProgressBar;
static GProgressbarObject *		guiStatusWindowProgressBarObj;
static GHandle                  guiStatusWindowActiveCommand;
static GHandle                  guiStatusWindowCmds2Go;
static GHandle                  guiStatusWindowReturnButton;
static void guiStatusWindowCreate(void);
static void guiStatusWindowShow(void);
static void guiStatusWindowHide(void);

void guiProgressBarIncrement(void);

// Motion menu
static GHandle					guiMotionMenuHandle;
static GHandle					guiMotionMenuButtonPlusX;
static GHandle					guiMotionMenuButtonMinusX;
static GHandle					guiMotionMenuButtonPlusY;
static GHandle					guiMotionMenuButtonMinusY;
static GHandle					guiMotionMenuButtonPlusZ;
static GHandle					guiMotionMenuButtonMinusZ;
static GHandle					guiMotionMenuButtonHome;
static GHandle					guiMotionMenuButtonFine;
static GHandle					guiMotionMenuButtonSetHome;
static GHandle					guiMotionMenuButtonReturn;
static GHandle					guiMotionMenuXLabel;
static GHandle					guiMotionMenuYLabel;
static GHandle					guiMotionMenuZLabel;
static GHandle					guiMotionMenuXCoordAbs;
static GHandle					guiMotionMenuYCoordAbs;
static GHandle					guiMotionMenuZCoordAbs;
static GHandle					guiMotionMenuXCoordRel;
static GHandle					guiMotionMenuYCoordRel;
static GHandle					guiMotionMenuZCoordRel;
static GHandle					guiMotionMenuAbsoluteLabel;
static GHandle					guiMotionMenuRelativeLabel;
static void guiMotionMenuCreate(void);
static void guiMotionMenuShow(void);
static void guiMotionMenuHide(void);

// Temperature menu
static GHandle					guiTemperatureMenuHandle;
static GHandle					guiTemperatureMenuExtruderLabel;
static GHandle					guiTemperatureMenuExtruderCurrentTempLabel;
static GHandle					guiTemperatureMenuExtruderSetTempLabel;
static GHandle					guiTemperatureMenuExtruderCurrentTempValue;
static GHandle					guiTemperatureMenuExtruderSetTempValue;
static GHandle					guiTemperatureMenuExtruderSetTempUpButton;
static GHandle					guiTemperatureMenuExtruderSetTempDownButton;
static GHandle					guiTemperatureMenuHeatbedLabel;
static GHandle					guiTemperatureMenuHeatbedCurrentTempLabel;
static GHandle					guiTemperatureMenuHeatbedSetTempLabel;
static GHandle					guiTemperatureMenuHeatbedCurrentTempValue;
static GHandle					guiTemperatureMenuHeatbedSetTempValue;
static GHandle					guiTemperatureMenuHeatbedSetTempUpButton;
static GHandle					guiTemperatureMenuHeatbedSetTempDownButton;
static GHandle					guiTemperatureMenuReturnButtonHandle;
static void guiTemperatureMenuCreate(void);
static void guiTemperatureMenuShow(void);
static void guiTemperatureMenuHide(void);

// WiFi menu
static GHandle					guiWiFiMenuHandle;
static GHandle					guiWiFiMenuReturnButtonHandle;
static void guiWifiMenuCreate(void);
static void guiWiFiMenuShow(void);
static void guiWiFiMenuHide(void);
// TODO: Design menu wifi

// Settings menu
static GHandle					guiSettingsMenuHandle;
static GHandle					guiSettingsMenuReturnButtonHandle;
static void guiSettingsMenuCreate(void);
static void guiSettingsMenuShow(void);
static void guiSettingsMenuHide(void);
// TODO: Design settings menu

// Main console object
static GConsoleObject           guiConsole;
static GHandle                  guiConsoleContainerHandle;
static GHandle                  guiConsoleButtonHandle;
static GHandle                  guiConsoleTitleHandle;
static GHandle                  guiConsoleHandle;
static guiConsoleConf_t         guiConsoleConf;
static void guiConsoleCreate(guiConsoleConf_t config);
static void guiConsoleHide(void);
static void guiConsoleShow(void);

static GListener				guiMainListener;



void guiInit(void);
void guiConfigure(StepperManager_t* stpman, HeaterProxy_t* extruder,
                                     HeaterProxy_t* bed);
void guiStart(void);

static void guiCreateWidgets(void);

static void guiMainMenuCreate(void);

static void guiConsoleCreate(guiConsoleConf_t config);
GHandle guiConsoleGetWinHandle(void);
static void guiConsoleShow(void);

static StepperManager_t* stpManager;
static HeaterProxy_t* htrExtruder;
static HeaterProxy_t* htrBed;

threadreturn_t guiThread(void* param);

#endif /* MARENGO_GUI_H_ */
