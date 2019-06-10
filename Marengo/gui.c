/*
 * gui.c
 *
 *  Created on: 26 mar 2019
 *      Author: grzeg
 */

#include "gui.h"
#include "heater.h"

void guiInit(void)
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

  gwinAttachMouse(0);
  guiCreateWidgets();
  guiConsoleConf_t guiConsoleConf = { "Marengo Console", NULL, NULL, NULL };
  guiConsoleCreate(guiConsoleConf);
  guiConsoleHide();

  gwinHide(guiMainMenuHandle);
  guiMainMenuShow();

  guibInitialised = TRUE;
}
void guiProgressBarIncrement(void)
{
	gwinProgressbarIncrement(guiStatusWindowProgressBar);
	// TODO: Change value on guiStatusWindowProgressVal
}
void guiCreateWidgets(void){
	guiMainMenuCreate();
  guiStatusWindowCreate();
  guiMotionMenuCreate();
  guiTemperatureMenuCreate();
  guiWifiMenuCreate();
  guiSettingsMenuCreate();
}
void guiConsoleCreate(guiConsoleConf_t config){
  // TODO: Use containers!
  if(config.height == NULL)
    config.height = guiWindowHeight;
  if(config.width == NULL)
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
  gwinWidgetClearInit(&wi);
  wi.g.show = TRUE;
  wi.g.x = 0;
  wi.g.y = bHeight;
  wi.g.width = guiWindowWidth;
  wi.g.height = guiWindowHeight-bHeight-40;
  wi.g.parent = guiConsoleContainerHandle;
  guiConsoleHandle = gwinConsoleCreate(NULL, &wi);
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
void guiConsoleShow(void)
{
  gwinShow(guiConsoleContainerHandle);
}
void guiConsoleHide(void)
{
	gwinHide(guiConsoleContainerHandle);
}
void guiStatusWindowShow(void)
{
	gwinShow(guiStatusWindow);
}
void guiStatusWindowHide(void)
{
	gwinHide(guiStatusWindow);
}
void guiMotionMenuShow(void) {
	gwinShow(guiMotionMenuHandle);
}
void guiMotionMenuHide(void)
{
	gwinHide(guiMotionMenuHandle);
}
void guiTemperatureMenuShow(void) {
	gwinShow(guiTemperatureMenuHandle);
}
void guiTemperatureMenuHide(void) {
	gwinHide(guiTemperatureMenuHandle);
}
void guiWiFiMenuShow(void) {
	gwinShow(guiWiFiMenuHandle);
}
void guiWiFiMenuHide(void) {
	gwinHide(guiWiFiMenuHandle);
}
void guiSettingsMenuShow(void) {
	gwinShow(guiSettingsMenuHandle);
}
void guiSettingsMenuHide(void) {
	gwinHide(guiSettingsMenuHandle);
}
void guiMainMenuShow(void)
{
	gwinShow(guiMainMenuHandle);
}
GHandle guiConsoleGetWinHandle(void)
{
  return guiConsoleHandle;
}

void guiMainMenuCreate(void)
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
void guiStatusWindowCreate(void)
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
void guiMotionMenuCreate(void)
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
void guiTemperatureMenuCreate(void)
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
	wi.g.height = 28;
	wi.g.y = 192;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "0 deg";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
	guiTemperatureMenuHeatbedCurrentTempValue = gwinLabelCreate(NULL, &wi);

	//Add set extruder temp value
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	wi.g.width = 64;
	wi.g.x = 128;
	wi.g.height = 28;
	wi.g.y = 192;
	wi.g.parent = guiTemperatureMenuHandle;
	wi.text = "0 deg";
	wi.customDraw = gwinLabelDrawJustifiedLeft;
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
void guiWifiMenuCreate(void)
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
void guiSettingsMenuCreate(void)
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
			break;
		default:
			break;
		}

        // Update labels
        // Heater temperatures
        gwinSetText(guiTemperatureMenuExtruderCurrentTempValue,
                    itoa(heaterGetTemp(&Heater1), buf, 10), TRUE);
        gwinSetText(guiTemperatureMenuExtruderSetTempValue,
                    itoa(heaterGetDesiredTemp(&Heater1), buf, 10), TRUE);
        gwinSetText(guiTemperatureMenuHeatbedCurrentTempValue,
                    itoa(heaterGetTemp(&Heater2), buf, 10), TRUE);
        gwinSetText(guiTemperatureMenuHeatbedSetTempValue,
                    itoa(heaterGetDesiredTemp(&Heater2), buf, 10), TRUE);
        // Check inc dec temperature buttons
        if(gwinButtonIsPressed(guiTemperatureMenuExtruderSetTempUpButton)==TRUE)
          heaterIncDesiredTemp(&Heater1);
        if(gwinButtonIsPressed(guiTemperatureMenuExtruderSetTempDownButton)==TRUE)
          heaterDecDesiredTemp(&Heater1);
        if(gwinButtonIsPressed(guiTemperatureMenuHeatbedSetTempUpButton)==TRUE)
          heaterIncDesiredTemp(&Heater2);
        if(gwinButtonIsPressed(guiTemperatureMenuHeatbedSetTempDownButton)==TRUE)
          heaterDecDesiredTemp(&Heater2);
        gfxSleepMilliseconds(100);
	}

	return (threadreturn_t)0;
}