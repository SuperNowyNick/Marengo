/*
 * gui.c
 *
 *  Created on: 26 mar 2019
 *      Author: grzeg
 */

#include "gui.h"
#include "coord.h"

void guiInit()
{
  /* Init screen.*/
  gfxSleepMilliseconds (100);
  gdispClear (COLOR1);
  gfxSleepMilliseconds (100);
  gdispClear (COLOR2);
  gfxSleepMilliseconds (100);
  gdispClear (COLOR3);
  gfxSleepMilliseconds (100);
  gdispClear (COLOR4);
  gfxSleepMilliseconds(100);
  gdispClear(COLOR1);

  //guiWindowWidth = gdispGetWidth();
  //guiWindowHeight = gdispGetHeight();
  guiWindowWidth = 240;
  guiWindowHeight = 320;

  gwinSetDefaultFont(gdispOpenFont("fixed_10x20"));
  gwinSetDefaultStyle(&guiWidgetStyle, FALSE);

  guiCreateWidgets();
  guiConsoleConf_t guiConsoleConf = { "Marengo Console", 0, 0, NULL };
  guiConsoleCreate(guiConsoleConf);
  guiConsoleHide();

  gwinHide(guiMainMenuHandle);
  guiMainMenuShow();

  guibInitialised = TRUE;
  stpManager = NULL;
  htrExtruder = NULL;
  htrBed = NULL;
}

void guiConfigure(StepperManager_t* stpman, HeaterProxy_t* extruder,
                  HeaterProxy_t* bed)
{
  stpManager = stpman;
  htrExtruder = extruder;
  htrBed = bed;
}

void guiProgressBarIncrement(void)
{
	gwinProgressbarIncrement(guiStatusWindowProgressBar);
	// TODO: Change value on guiStatusWindowProgressVal
}
static void guiCreateWidgets(void){
	guiMainMenuCreate();
  guiStatusWindowCreate();
  guiMotionMenuCreate();
  guiTemperatureMenuCreate();
  guiWifiMenuCreate();
  guiSettingsMenuCreate();
}
static void guiConsoleCreate(guiConsoleConf_t config){
  // TODO: Use containers!
  if(config.height == 0)
    config.height = guiWindowHeight;
  if(config.width == 0)
    config.width = guiWindowWidth;
  if(config.font == NULL)
    config.font = gdispOpenFont("fixed_5x8");
  guiConsoleConf = config;
  // Create console container
  GWidgetInit             wi;
  gwinWidgetClearInit(&wi);
  wi.g.show = FALSE;
  wi.g.width = config.width;
  wi.g.height = config.height;
  wi.g.x = 0;
  wi.g.y= 0;
  wi.text = config.name;
  guiConsoleContainerHandle = gwinContainerCreate(NULL, &wi, GWIN_CONTAINER_BORDER);
  // Get font height and add 4 to get title bar height
  coord_t bHeight = gdispGetFontMetric(guiConsoleConf.font, fontHeight)+4;

  gwinSetFont(guiConsoleContainerHandle, guiConsoleConf.font);
  gwinSetDefaultColor(Red);
  gwinSetDefaultBgColor(White);
  gwinSetDefaultColor(White);
  gwinSetDefaultBgColor(Black);

  // Create title bar
  gwinWidgetClearInit(&wi);
  wi.g.show = TRUE;
  wi.g.x = 0;
  wi.g.y = 0;
  wi.g.width = guiWindowWidth;
  wi.g.height = bHeight;
  wi.text = guiConsoleConf.name;
  wi.g.parent = guiConsoleContainerHandle;
  wi.customDraw = gwinLabelDrawJustifiedCenter;
  guiConsoleTitleHandle = gwinLabelCreate(NULL, &wi);
  // Create our console window
  GWindowInit wini;
  gwinClearInit(&wini);
  wini.show = TRUE;
  wini.x = 0;
  wini.y = bHeight;
  wini.width = guiWindowWidth;
  wini.height = guiWindowHeight-bHeight-40;
  wini.parent = guiConsoleContainerHandle;
  guiConsoleHandle = gwinConsoleCreate(NULL, &wini);
  gwinSetFont(guiConsoleHandle, guiConsoleConf.font);
  // Add return button
  gwinSetDefaultFont(gdispOpenFont("UI2"));
  gwinSetDefaultStyle(&guiWidgetStyle, FALSE);
  gwinWidgetClearInit(&wi);
  wi.text = "Return";
  wi.g.show = TRUE;
  wi.g.height = 40;
  wi.g.width = guiWindowWidth;
  wi.g.x = 0;
  wi.g.y = guiWindowHeight - 40;
  wi.g.parent = guiConsoleContainerHandle;
  wi.customDraw = gwinButtonDraw_Rounded;
  guiConsoleButtonHandle = gwinButtonCreate(NULL, &wi);
}
static void guiConsoleShow(void)
{
  gwinShow(guiConsoleContainerHandle);
}
static void guiConsoleHide(void)
{
	gwinHide(guiConsoleContainerHandle);
}
static void guiStatusWindowShow(void)
{
	gwinShow(guiStatusWindow);
}
static void guiStatusWindowHide(void)
{
	gwinHide(guiStatusWindow);
}
static void guiMotionMenuShow(void)
{
	gwinShow(guiMotionMenuHandle);
}
static void guiMotionMenuHide(void)
{
	gwinHide(guiMotionMenuHandle);
}
static void guiTemperatureMenuShow(void)
{
	gwinShow(guiTemperatureMenuHandle);
}
static void guiTemperatureMenuHide(void)
{
	gwinHide(guiTemperatureMenuHandle);
}
static void guiWiFiMenuShow(void)
{
	gwinShow(guiWiFiMenuHandle);
}
static void guiWiFiMenuHide(void)
{
	gwinHide(guiWiFiMenuHandle);
}
static void guiSettingsMenuShow(void)
{
	gwinShow(guiSettingsMenuHandle);
}
static void guiSettingsMenuHide(void)
{
	gwinHide(guiSettingsMenuHandle);
}
static void guiMainMenuShow(void)
{
	gwinShow(guiMainMenuHandle);
}
GHandle guiConsoleGetWinHandle(void)
{
  return guiConsoleHandle;
}

static void guiMainMenuCreate(void)
{
  // Create container
  GWidgetInit             wi;
  gwinWidgetClearInit(&wi);
  wi.g.show = TRUE;
  wi.g.width = guiWindowWidth;
  wi.g.height = guiWindowHeight;
  wi.g.x = 0;
  wi.g.y= 0;
  wi.text = "MainMenu";
  guiMainMenuHandle = gwinContainerCreate(NULL, &wi, GWIN_CONTAINER_BORDER);
  // Create buttons
  gwinWidgetClearInit(&wi);
  wi.g.show = TRUE;
  wi.g.width = guiWindowWidth/2-6;
  wi.g.x = 4;
  wi.g.height = guiWindowHeight/3-4;
  wi.g.y = 3;
  wi.g.parent = guiMainMenuHandle;
  wi.text = "Status";
  wi.customDraw = gwinButtonDraw_Rounded;
  guiMainMenuStatusButtonHandle = gwinButtonCreate(NULL, &wi);
  gwinWidgetClearInit(&wi);
  wi.g.show = TRUE;
  wi.g.width = guiWindowWidth/2-6;
  wi.g.x = guiWindowWidth/2+2;
  wi.g.height = guiWindowHeight/3-4;
  wi.g.y = 3;
  wi.g.parent = guiMainMenuHandle;
  wi.text = "Motion";
  wi.customDraw = gwinButtonDraw_Rounded;
  guiMainMenuMotionButtonHandle = gwinButtonCreate(NULL, &wi);
  gwinWidgetClearInit(&wi);
  wi.g.show = TRUE;
  wi.g.width = guiWindowWidth/2-6;
  wi.g.x = 4;
  wi.g.height = guiWindowHeight/3-4;
  wi.g.y = guiWindowHeight/3+2;
  wi.g.parent = guiMainMenuHandle;
  wi.text = "Temperature";
  wi.customDraw = gwinButtonDraw_Rounded;
  guiMainMenuTemperatureButtonHandle = gwinButtonCreate(NULL, &wi);
  gwinWidgetClearInit(&wi);
  wi.g.show = TRUE;
  wi.g.width = guiWindowWidth/2-6;
  wi.g.x = guiWindowWidth/2+2;
  wi.g.height = guiWindowHeight/3-4;
  wi.g.y = guiWindowHeight/3+2;
  wi.g.parent = guiMainMenuHandle;
  wi.text = "WiFi";
  wi.customDraw = gwinButtonDraw_Rounded;
  guiMainMenuWifiButtonHandle = gwinButtonCreate(NULL, &wi);
  gwinWidgetClearInit(&wi);
  wi.g.show = TRUE;
  wi.g.width = guiWindowWidth/2-6;
  wi.g.x = 4;
  wi.g.height = guiWindowHeight/3-4;
  wi.g.y = guiWindowHeight*2/3+1;
  wi.g.parent = guiMainMenuHandle;
  wi.text = "Console";
  wi.customDraw = gwinButtonDraw_Rounded;
  guiMainMenuConsoleButtonHandle = gwinButtonCreate(NULL, &wi);
  gwinWidgetClearInit(&wi);
  wi.g.show = TRUE;
  wi.g.width = guiWindowWidth/2-6;
  wi.g.x = guiWindowWidth/2+2;
  wi.g.height = guiWindowHeight/3-4;
  wi.g.y = guiWindowHeight*2/3+1;
  wi.g.parent = guiMainMenuHandle;
  wi.text = "Settings";
  wi.customDraw = gwinButtonDraw_Rounded;
  guiMainMenuSettingsButtonHandle = gwinButtonCreate(NULL, &wi);
}
static void guiStatusWindowCreate(void)
{
	// Create container
	GWidgetInit             wi;
	gwinWidgetClearInit(&wi);
	wi.g.show = FALSE;
	wi.g.width = guiWindowWidth;
	wi.g.height = guiWindowHeight;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.text = "StatusWindow";
	guiStatusWindow = gwinContainerCreate(NULL, &wi, GWIN_CONTAINER_BORDER);
	// Create status label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth*2/3-8;
	wi.g.x = 2;
	wi.g.height = 92;
	wi.g.y = 2;
	wi.g.parent = guiStatusWindow;
	wi.text = "Idle";
	wi.customDraw = gwinLabelDrawJustifiedCenter;
	guiStatusWindowState = gwinLabelCreate(NULL, &wi);
	// TODO: Add animated icon to this label

	// Add stop button
	gwinWidgetClearInit(&wi);
	wi.text = "STOP";
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth * 2 / 3 - 8;
	wi.g.x = 4;
	wi.g.height = 32;
	wi.g.y = 96;
	wi.g.parent = guiStatusWindow;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiStatusWindowStopButton = gwinButtonCreate(NULL, &wi);

	// Create X label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 20;
	wi.g.x = guiWindowWidth*2 / 3;
	wi.g.height = 28;
	wi.g.y = 2;
	wi.g.parent = guiStatusWindow;
	wi.text = "X:";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowLabelX = gwinLabelCreate(NULL, &wi);

	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth*2/3 - 3 -20;
	wi.g.x = guiWindowWidth*2/3+20;
	wi.g.height = 28;
	wi.g.y = 2;
	wi.g.parent = guiStatusWindow;
	wi.text = "0.000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowCoordX = gwinLabelCreate(NULL, &wi);
	// Create Y label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 20;
	wi.g.x = guiWindowWidth*2/3;
	wi.g.height = 28;
	wi.g.y = 34;
	wi.g.parent = guiStatusWindow;
	wi.text = "Y:";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowLabelY = gwinLabelCreate(NULL, &wi);

	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth*2/3 - 3 - 20;
	wi.g.x = guiWindowWidth*2/3 + 20;
	wi.g.height = 28;
	wi.g.y = 34;
	wi.g.parent = guiStatusWindow;
	wi.text = "0.000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowCoordY = gwinLabelCreate(NULL, &wi);

	// Create Z label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 20;
	wi.g.x = guiWindowWidth*2/3;
	wi.g.height = 28;
	wi.g.y = 66;
	wi.g.parent = guiStatusWindow;
	wi.text = "Z:";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowLabelZ = gwinLabelCreate(NULL, &wi);

	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth*2/3 - 3 - 20;
	wi.g.x = guiWindowWidth*2/3 + 20;
	wi.g.height = 28;
	wi.g.y = 66;
	wi.g.parent = guiStatusWindow;
	wi.text = "0.000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowCoordZ = gwinLabelCreate(NULL, &wi);

	// Create F label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 20;
	wi.g.x = guiWindowWidth*2/3;
	wi.g.height = 28;
	wi.g.y = 98;
	wi.g.parent = guiStatusWindow;
	wi.text = "F:";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowLabelF = gwinLabelCreate(NULL, &wi);

	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth*2/3 - 3 - 20;
	wi.g.x = guiWindowWidth*2/3 + 20;
	wi.g.height = 28;
	wi.g.y = 98;
	wi.g.parent = guiStatusWindow;
	wi.text = "0.000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowFeedrate = gwinLabelCreate(NULL, &wi);

	// Create elasped time label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.x = 4;
	wi.g.width = 116; // TODO: Adjust it!
	wi.g.y = 130;
	wi.g.height = 28;
	wi.g.parent = guiStatusWindow;
	wi.text = "Elasped time:";
	wi.customDraw = gwinLabelDrawJustifiedRight;
	guiStatusWindowElaspedTimeTextLabel = gwinLabelCreate(NULL, &wi);

	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.x = 136;
	wi.g.width = 116; // TODO: Adjust it!
	wi.g.y = 130;
	wi.g.height = 28;
	wi.g.parent = guiStatusWindow;
	wi.text = "0:00:00";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowElaspedTimeFieldLabel = gwinLabelCreate(NULL, &wi);

	// Create elasped time label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.x = 4;
	wi.g.width = 116; // TODO: Adjust it!
	wi.g.y = 162;
	wi.g.height = 28;
	wi.g.parent = guiStatusWindow;
	wi.text = "Remaining time:";
	wi.customDraw = gwinLabelDrawJustifiedRight;
	guiStatusWindowRemainingTimeTextLabel = gwinLabelCreate(NULL, &wi);

	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.x = 136;
	wi.g.width = 116; // TODO: Adjust it!
	wi.g.y = 162;
	wi.g.height = 28;
	wi.g.parent = guiStatusWindow;
	wi.text = "0:00:00";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowRemainingTimeFieldLabel = gwinLabelCreate(NULL, &wi);

	// Create progress label and bar
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 72;
	wi.g.x = 4;
	wi.g.height = 24;
	wi.g.y = 194;
	wi.g.parent = guiStatusWindow;
	wi.text = "Progress:";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowProgressLabel = gwinLabelCreate(NULL, &wi);

	//gwinWidgetClearInit(&wi);
	//wi.g.show = TRUE;
	//wi.g.width = 156;
	//wi.g.x = 80;
	//wi.g.height = 24;
	//wi.g.y = 194;
	//wi.g.parent = guiStatusWindow;
	//wi.text = "0%";
	//wi.customDraw = gwinLabelDrawJustifiedLeft;
	//guiStatusWindowProgressVal = gwinLabelCreate(NULL, &wi);

	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 156;
	wi.g.x = 80;
	wi.g.height = 24;
	wi.g.y = 194;
	wi.g.parent = guiStatusWindow;
	guiStatusWindowProgressBar = gwinGProgressbarCreate(gdispGetDisplay(NULL), guiStatusWindowProgressBarObj, &wi);

	// Create active command label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth;
	wi.g.x = 4;
	wi.g.height = 24;
	wi.g.y = guiWindowHeight/10*7+2;
	wi.g.parent = guiStatusWindow;
	wi.text = "Active command";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowActiveCommand = gwinLabelCreate(NULL, &wi);

	// Create active command label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth;
	wi.g.x = 0;
	wi.g.height = 24;
	wi.g.y = guiWindowHeight/10*8+2;
	wi.g.parent = guiStatusWindow;
	wi.text = "Commands to go...";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiStatusWindowCmds2Go = gwinLabelCreate(NULL, &wi);

	// Add return button
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&guiWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);
	wi.text = "Return";
	wi.g.show = TRUE;
	wi.g.height = guiWindowHeight / 10 * 2 - 26;
	wi.g.width = guiWindowWidth;
	wi.g.x = 0;
	wi.g.y = guiWindowHeight / 10 * 8 + 26;
	wi.g.parent = guiStatusWindow;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiStatusWindowReturnButton = gwinButtonCreate(NULL, &wi);

}
static void guiMotionMenuCreate(void)
{
	// Create container
	GWidgetInit             wi;
	gwinWidgetClearInit(&wi);
	wi.g.show = FALSE;
	wi.g.width = guiWindowWidth;
	wi.g.height = guiWindowHeight;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.text = "MotionWindow";
	guiMotionMenuHandle = gwinContainerCreate(NULL, &wi, GWIN_CONTAINER_BORDER);

	// Add +X button
	gwinWidgetClearInit(&wi);
	wi.text = "+X";
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = guiWindowWidth/2-32;
	wi.g.height = 64;
	wi.g.y = 4;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonPlusX = gwinButtonCreate(NULL, &wi);

	// Add -X button
	gwinWidgetClearInit(&wi);
	wi.text = "-X";
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = guiWindowWidth/2-32;
	wi.g.height = 64;
	wi.g.y = 4+128;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonMinusX = gwinButtonCreate(NULL, &wi);

	// Add +Y button
	gwinWidgetClearInit(&wi);
	wi.text = "+Y";
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = guiWindowWidth / 2 +32;
	wi.g.height = 64;
	wi.g.y = 4+64;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonPlusY = gwinButtonCreate(NULL, &wi);

	// Add -Y button
	gwinWidgetClearInit(&wi);
	wi.text = "-Y";
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = guiWindowWidth / 2 - 96;
	wi.g.height = 64;
	wi.g.y = 4 + 64;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonMinusY = gwinButtonCreate(NULL, &wi);

	// Add +Z button
	gwinWidgetClearInit(&wi);
	wi.text = "+Z";
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = guiWindowWidth - 70;
	wi.g.height = 64;
	wi.g.y = 4;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonPlusZ = gwinButtonCreate(NULL, &wi);

	// Add -Z button
	gwinWidgetClearInit(&wi);
	wi.text = "-Z";
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = guiWindowWidth - 70;
	wi.g.height = 64;
	wi.g.y = 4+64+64;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonMinusZ = gwinButtonCreate(NULL, &wi);

	// Add fine button
	gwinWidgetClearInit(&wi);
	wi.text = "Fine";
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = guiWindowWidth/2-32;
	wi.g.height = 64;
	wi.g.y = 4 + 64;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonFine = gwinButtonCreate(NULL, &wi);

	// Add home button
	gwinWidgetClearInit(&wi);
	wi.text = "Home";
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 4;
	wi.g.height = 64;
	wi.g.y = 4;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonHome = gwinButtonCreate(NULL, &wi);

	// Add Set Home button
	gwinWidgetClearInit(&wi);
	wi.text = "Set Home";
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 4;
	wi.g.height = 64;
	wi.g.y = 4 + 64 + 64;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonSetHome = gwinButtonCreate(NULL, &wi);

	//Add X label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 20;
	wi.g.x = 4;
	wi.g.height = 22;
	wi.g.y = 196+22;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "X:";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuXLabel = gwinLabelCreate(NULL, &wi);

	//Add Y label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 20;
	wi.g.x = 4;
	wi.g.height = 22;
	wi.g.y = 196+2*22;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "Y:";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuYLabel = gwinLabelCreate(NULL, &wi);

	//Add Z label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 20;
	wi.g.x = 4;
	wi.g.height = 22;
	wi.g.y = 196+3*22;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "Z:";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuZLabel = gwinLabelCreate(NULL, &wi);

	//Add relative label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 106;
	wi.g.x = 4+20;
	wi.g.height = 22;
	wi.g.y = 196;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "Relative";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuRelativeLabel = gwinLabelCreate(NULL, &wi);
	//Add X relative coord
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 106;
	wi.g.x = 4 + 20;
	wi.g.height = 22;
	wi.g.y = 196+22;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "0.00000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuXCoordRel = gwinLabelCreate(NULL, &wi);
	//Add Y relative coord
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 106;
	wi.g.x = 4 + 20;
	wi.g.height = 22;
	wi.g.y = 196 + 22*2;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "0.00000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuYCoordRel = gwinLabelCreate(NULL, &wi);
	//Add Z relative coord
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 106;
	wi.g.x = 4 + 20;
	wi.g.height = 22;
	wi.g.y = 196 + 22 * 3;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "0.00000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuZCoordRel = gwinLabelCreate(NULL, &wi);
	//Add absolute label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 106;
	wi.g.x = 4 + 20+110;
	wi.g.height = 22;
	wi.g.y = 196;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "Absolute";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuAbsoluteLabel = gwinLabelCreate(NULL, &wi);
	//Add X absolute coord
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 106;
	wi.g.x = 4 + 20 + 110;
	wi.g.height = 22;
	wi.g.y = 196+22;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "0.00000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuXCoordAbs = gwinLabelCreate(NULL, &wi);
	//Add Y absolute coord
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 106;
	wi.g.x = 4 + 20 + 110;
	wi.g.height = 22;
	wi.g.y = 196 + 22*2;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "0.00000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuYCoordAbs = gwinLabelCreate(NULL, &wi);
	//Add Z absolute coord
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 106;
	wi.g.x = 4 + 20 + 110;
	wi.g.height = 22;
	wi.g.y = 196 + 22*3;
	wi.g.parent = guiMotionMenuHandle;
	wi.text = "0.00000";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiMotionMenuZCoordAbs = gwinLabelCreate(NULL, &wi);
	// Add return button
	gwinWidgetClearInit(&wi);
	wi.text = "Return";
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth;
	wi.g.x = 0;
	wi.g.height = 40;
	wi.g.y = guiWindowHeight-40;
	wi.g.parent = guiMotionMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiMotionMenuButtonReturn = gwinButtonCreate(NULL, &wi);
}
static void guiTemperatureMenuCreate(void)
{
	// Create container
	GWidgetInit             wi;
	gwinWidgetClearInit(&wi);
	wi.g.show = FALSE;
	wi.g.width = guiWindowWidth;
	wi.g.height = guiWindowHeight;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.text = "TemperatureWindow";
	guiTemperatureMenuHandle = gwinContainerCreate(NULL, &wi, GWIN_CONTAINER_BORDER);

	//Add extruder label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 128;
	wi.g.x = 4;
	wi.g.height = 22;
	wi.g.y = 4;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "Extruder temperature";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiTemperatureMenuExtruderLabel = gwinLabelCreate(NULL, &wi);

	//Add current extruder temp label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 4;
	wi.g.height = 28;
	wi.g.y = 32;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "Current";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiTemperatureMenuExtruderCurrentTempLabel = gwinLabelCreate(NULL, &wi);

	//Add set extruder temp label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 124;
	wi.g.height = 28;
	wi.g.y = 32;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "Set";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiTemperatureMenuExtruderSetTempLabel = gwinLabelCreate(NULL, &wi);

	//Add current extruder temp value
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 4;
	wi.g.height = 28;
	wi.g.y = 64;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "0 deg";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiTemperatureMenuExtruderCurrentTempValue = gwinLabelCreate(NULL, &wi);

	//Add set extruder temp value
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 128;
	wi.g.height = 28;
	wi.g.y = 64;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "0 deg";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiTemperatureMenuExtruderSetTempValue = gwinLabelCreate(NULL, &wi);

	// Add + extruder temperature button
	gwinWidgetClearInit(&wi);
	wi.text = "+";
	wi.g.show = TRUE;
	wi.g.width = 32;
	wi.g.x = 192;
	wi.g.height = 32;
	wi.g.y = 32;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiTemperatureMenuExtruderSetTempUpButton = gwinButtonCreate(NULL, &wi);

	// Add - extruder temperature button
	gwinWidgetClearInit(&wi);
	wi.text = "-";
	wi.g.show = TRUE;
	wi.g.width = 32;
	wi.g.x = 192;
	wi.g.height = 32;
	wi.g.y = 64;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiTemperatureMenuExtruderSetTempDownButton = gwinButtonCreate(NULL, &wi);

	//Add extruder label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 128;
	wi.g.x = 4;
	wi.g.height = 22;
	wi.g.y = 128;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "Heatbed temperature";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiTemperatureMenuHeatbedLabel = gwinLabelCreate(NULL, &wi);

	//Add current extruder temp label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 4;
	wi.g.height = 28;
	wi.g.y = 160;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "Current";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiTemperatureMenuHeatbedCurrentTempLabel = gwinLabelCreate(NULL, &wi);

	//Add set extruder temp label
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 124;
	wi.g.height = 28;
	wi.g.y = 160;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "Set";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiTemperatureMenuHeatbedSetTempLabel = gwinLabelCreate(NULL, &wi);

	//Add current extruder temp value
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 4;
	wi.g.height = 64;
	wi.g.y = 192;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "0 deg";
	guiTemperatureMenuHeatbedCurrentTempValue = gwinLabelCreate(NULL, &wi);

	//Add set extruder temp value
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 128;
	wi.g.height = 64;
	wi.g.y = 192;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "0 deg";
	guiTemperatureMenuHeatbedSetTempValue = gwinLabelCreate(NULL, &wi);

	// Add + heatbed temperature button
	gwinWidgetClearInit(&wi);
	wi.text = "+";
	wi.g.show = TRUE;
	wi.g.width = 32;
	wi.g.x = 192;
	wi.g.height = 32;
	wi.g.y = 160;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiTemperatureMenuHeatbedSetTempUpButton = gwinButtonCreate(NULL, &wi);

	// Add - heatbed temperature button
	gwinWidgetClearInit(&wi);
	wi.text = "-";
	wi.g.show = TRUE;
	wi.g.width = 32;
	wi.g.x = 192;
	wi.g.height = 32;
	wi.g.y = 192;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiTemperatureMenuHeatbedSetTempDownButton = gwinButtonCreate(NULL, &wi);


	// Add return button
	gwinWidgetClearInit(&wi);
	wi.text = "Return";
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth;
	wi.g.x = 0;
	wi.g.height = 40;
	wi.g.y = guiWindowHeight - 40;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiTemperatureMenuReturnButtonHandle = gwinButtonCreate(NULL, &wi);
}
static void guiWifiMenuCreate(void)
{
	// Create container
	GWidgetInit             wi;
	gwinWidgetClearInit(&wi);
	wi.g.show = FALSE;
	wi.g.width = guiWindowWidth;
	wi.g.height = guiWindowHeight;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.text = "WiFiMenu";
	guiWiFiMenuHandle = gwinContainerCreate(NULL, &wi, GWIN_CONTAINER_BORDER);

	// Add return button
	gwinWidgetClearInit(&wi);
	wi.text = "Return";
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth;
	wi.g.x = 0;
	wi.g.height = 40;
	wi.g.y = guiWindowHeight - 40;
	wi.g.parent = guiWiFiMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiWiFiMenuReturnButtonHandle = gwinButtonCreate(NULL, &wi);
	// TODO: Add code here to initialize wifi control menu
}
static void guiSettingsMenuCreate(void)
{
	// Create container
	GWidgetInit             wi;
	gwinWidgetClearInit(&wi);
	wi.g.show = FALSE;
	wi.g.width = guiWindowWidth;
	wi.g.height = guiWindowHeight;
	wi.g.x = 0;
	wi.g.y = 0;
	wi.text = "SettingsMenu";
	guiSettingsMenuHandle = gwinContainerCreate(NULL, &wi, GWIN_CONTAINER_BORDER);

	// Add return button
	gwinWidgetClearInit(&wi);
	wi.text = "Return";
	wi.g.show = TRUE;
	wi.g.width = guiWindowWidth;
	wi.g.x = 0;
	wi.g.height = 40;
	wi.g.y = guiWindowHeight - 40;
	wi.g.parent = guiSettingsMenuHandle;
	wi.customDraw = gwinButtonDraw_Rounded;
	guiSettingsMenuReturnButtonHandle = gwinButtonCreate(NULL, &wi);
	/// TODO: Add code here to initialize settings menu
}
void guiStart(void)
{
	gfxThreadCreate(NULL, 2048, NORMAL_PRIORITY, guiThread, 0);
}

threadreturn_t guiThread(void* param)
{
	(void)param;

	GEvent *pe;
	GHandle gwin;
	// This is used to rotate display
	//gdispGSetOrientation(gdispGetDisplay(NULL), GDISP_ROTATE_90);

	geventListenerInit(&guiMainListener);
	gwinAttachListener(&guiMainListener);

    char buf[32];

    gfxSleepMilliseconds(100);

	while (1) {
		pe = geventEventWait(&guiMainListener, 100);
		switch (pe->type) {
		case GEVENT_GWIN_BUTTON:
			gwin = ((GEventGWinButton*)pe)->gwin;
			// Menu buttons
			// TODO: Change this to switch if possible
			if (gwin == guiMainMenuStatusButtonHandle) { guiStatusWindowShow(); };
			if (gwin == guiMainMenuMotionButtonHandle) { guiMotionMenuShow(); }
			if (gwin == guiMainMenuTemperatureButtonHandle) { guiTemperatureMenuShow(); }
			if (gwin == guiMainMenuWifiButtonHandle) { guiWiFiMenuShow(); }
			if (gwin == guiMainMenuConsoleButtonHandle) {guiConsoleShow();}
			if (gwin == guiMainMenuSettingsButtonHandle) { guiSettingsMenuShow(); };
			if (gwin == guiConsoleButtonHandle) { guiConsoleHide(); }
			if (gwin == guiStatusWindowReturnButton) { guiStatusWindowHide(); }
			if (gwin == guiMotionMenuButtonReturn) { guiMotionMenuHide(); }
			if (gwin == guiTemperatureMenuReturnButtonHandle) { guiTemperatureMenuHide(); }
			if (gwin == guiWiFiMenuReturnButtonHandle) { guiWiFiMenuHide(); }
			if (gwin == guiSettingsMenuReturnButtonHandle) { guiSettingsMenuHide(); }
			// Status window buttons
			if (gwin == guiStatusWindowStopButton) {
              if(stpManager!=NULL)
                StepperManager_StopISR(stpManager);
			}
			// Motion menu window buttons
			if (gwin == guiMotionMenuButtonSetHome) {
			  if(stpManager!=NULL)
			    StepperManager_SetPosition(stpManager, stpCoordF_Zero());
			  // TODO: This should also change state of gcode interpreter!
			}
			break;
		default:
			break;
		}

        // Update labels
		// Status menu labels
		// TODO: Update status of stepper motors
		stpCoordF_t coord;
		uint32_t feedrate;
		if(stpManager!=NULL){
		  coord = StepperManager_GetPosition(stpManager);
		  feedrate = StepperManager_GetCurrentFeedrate(stpManager);
		}
		else{
		  coord = stpCoordF_Zero();
		  feedrate = 0;
		}
		gwinSetText(guiStatusWindowCoordX, myftoa(coord.x, buf), TRUE);
		gwinSetText(guiStatusWindowCoordY, myftoa(coord.y, buf), TRUE);
		gwinSetText(guiStatusWindowCoordZ, myftoa(coord.z, buf), TRUE);
		gwinSetText(guiStatusWindowFeedrate, (const char*)itoa(feedrate,
		                                          buf, 10), TRUE);
		// TODO: Calculate elasped time
		// TODO: Calculate remaining time
		// TODO: Calculate progress
		// TODO: Show current command
		// TODO: Show queued commands

		// Motion menu labels
        gwinSetText(guiMotionMenuXCoordAbs, myftoa(coord.x, buf), TRUE);
        gwinSetText(guiMotionMenuYCoordAbs, myftoa(coord.y, buf), TRUE);
        gwinSetText(guiMotionMenuZCoordAbs, myftoa(coord.z, buf), TRUE);


        // Heater temperatures
        gwinSetText(guiTemperatureMenuExtruderCurrentTempValue,
                    itoa(HeaterProxy_GetRealTemp(htrExtruder), buf, 10), TRUE);
        gwinSetText(guiTemperatureMenuExtruderSetTempValue,
                    itoa(HeaterProxy_GetDesiredTemp(htrExtruder), buf, 10), TRUE);
        gwinSetText(guiTemperatureMenuHeatbedCurrentTempValue,
                    itoa(HeaterProxy_GetRealTemp(htrBed), buf, 10), TRUE);
        gwinSetText(guiTemperatureMenuHeatbedSetTempValue,
                    itoa(HeaterProxy_GetDesiredTemp(htrBed), buf, 10), TRUE);
        // Check inc dec temperature buttons
        if(gwinButtonIsPressed(guiTemperatureMenuExtruderSetTempUpButton)==TRUE)
          HeaterProxy_IncSetTemp(htrExtruder);
        if(gwinButtonIsPressed(guiTemperatureMenuExtruderSetTempDownButton)==TRUE)
          HeaterProxy_DecSetTemp(htrExtruder);
        if(gwinButtonIsPressed(guiTemperatureMenuHeatbedSetTempUpButton)==TRUE)
          HeaterProxy_IncSetTemp(htrBed);
        if(gwinButtonIsPressed(guiTemperatureMenuHeatbedSetTempDownButton)==TRUE)
          HeaterProxy_DecSetTemp(htrBed);
        gfxSleepMilliseconds(100);
	}

	return (threadreturn_t)0;
}
