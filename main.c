/**
 * Marengo 3D Printer Firmware
 * Copyright (C) 2017 Grzegorz Sobczak
 *
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016, 2017 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "gfx.h"
#include "ch.h"
#include "hal.h"
#include "ch_test.h"

#include "chprintf.h"
#include "shell.h"

#include "usbcfg.h"

#include "stdlib.h"
#include <string.h>

// Include header file for LoadMouseCalibration function required for uGFX to work
#include "src/ginput/ginput_driver_mouse.h"


// Include Marengo configurations
#include "Marengo/pin_mapping.h"
#include "Marengo/console.h"
#include "Marengo/stepper.h"
#include "Marengo/gcode.h"

// uGFX GINPUT Mouse calibration data
float calibrationdata[]={
                         -0.06283,  //ax
                         0.00439,   //bx
                         239.20356, //cx
                         -0.00145,  //ay
                         0.08958,   //by
                         -29.68285 //cy
};

//  uGFX GINPUT Mouse calibration data loading function
bool_t LoadMouseCalibration(unsigned instance, void *data, size_t sz)
{
  (void)instance;
  if (sz != sizeof(calibrationdata) || instance !=0)
    return FALSE;
  memcpy(data, (void*)&calibrationdata, sz);
  return TRUE;
}

// Main console object
static GConsoleObject           gc;
static font_t                   font;
static coord_t                  bHeight;
static GHandle                  ghc;
static coord_t                  swidth, sheight;

// Test info cmd for Marengo console
ccmd_t CmdInfo(int argc, char **argv)
{
  consPrintf("System info:"CONSOLE_NEWLINE_STR);
  consPrintf("Kernel:      %s"CONSOLE_NEWLINE_STR, CH_KERNEL_VERSION);
#if defined(PORT_COMPILER_NAME)
  consPrintf("Compiler:    %s"CONSOLE_NEWLINE_STR, PORT_COMPILER_NAME);
#endif
  consPrintf("Architecture:%s"CONSOLE_NEWLINE_STR, PORT_ARCHITECTURE_NAME);
#if defined(PORT_CORE_VARIANT_NAME)
  consPrintf("Core variant:%s"CONSOLE_NEWLINE_STR, PORT_CORE_VARIANT_NAME);
#endif
#ifdef PORT_INFO
  consPrintf("Port info:   %s"CONSOLE_NEWLINE_STR, PORT_INFO);
#endif
#if defined(PLATFORM_NAME)
  consPrintf("Platform name:%s"CONSOLE_NEWLINE_STR, PLATFORM_NAME);
#endif
#if defined(BOARD_NAME)
  consPrintf("Board name:   %s"CONSOLE_NEWLINE_STR, BOARD_NAME);
#endif
#if defined(__DATE__)
#if defined(__TIME__)
  consPrintf("Build time:   %s%s%s"CONSOLE_NEWLINE_STR, __DATE__, "-", __TIME__);
#endif
#endif
}
ccmd_t CmdStpen(int argc, char **argv)
{
  stpEnable();
  consPrintf("Stepper motors enabled"CONSOLE_NEWLINE_STR);
}
ccmd_t CmdStpdis(int argc, char **argv)
{
  stpDisable();
  consPrintf("Stepper motors disabled"CONSOLE_NEWLINE_STR);
}
ccmd_t CmdStptg(int argc, char **argv)
{
  stpToggle();
  consPrintf("Stepper motors ");
  if (stpEnabled) consPrintf("enabled");
  else consPrintf("disabled");
  consPrintf(CONSOLE_NEWLINE_STR);
}
ccmd_t CmdStpmovsteps(int argc, char **argv)
{
  if (argc!=4){
    consPrintf("Not enough or too many parameters!"CONSOLE_NEWLINE_STR);
    return 1;
  }
  consPrintf(argv[1]);
  if (!strspn(argv[1], "XYZA")){
    consPrintf("No such axis!"CONSOLE_NEWLINE_STR);
    return 1;
  }
  char axis = *argv[1];
  int steps, delay;
  steps = atoi(argv[2]);
  delay = atoi(argv[3]);
  if (steps<=0){
    consPrintf("Steps number must be numerical and greater than zero!"CONSOLE_NEWLINE_STR);
    return 1;
  }
  if (delay<=0){
    consPrintf("Delay time must be numerical and greater than zero!"CONSOLE_NEWLINE_STR);
    return 1;
  }
  consPrintf("Moving axis %c with %d steps and %d x2 us delay"CONSOLE_NEWLINE_STR, axis, steps, delay);
  if (stpMoveAxisSteps(axis, steps, delay))
    consPrintf("Error!"CONSOLE_NEWLINE_STR);
  return 0;
}
ccmd_t CmdStpDir(int argc, char **argv)
{
  if (argc!=2){
    consPrintf("Not enough or too many parameters!"CONSOLE_NEWLINE_STR);
    return 1;
  }
  consPrintf(argv[1]);
  if (!strspn(argv[1], "XYZA")){
    consPrintf("No such axis!"CONSOLE_NEWLINE_STR);
    return 1;
  }
  char axis = *argv[1];
  if (stpDirToggle(axis))
    consPrintf("Error!"CONSOLE_NEWLINE_STR);
  consPrintf("Axis %c direction changed"CONSOLE_NEWLINE_STR, axis);
  return 0;
}
ccmd_t CmdGCode(int argc, char **argv)
{
  char line[256];
  consGetLine(line, 256);
  if (line[0]!='%'){
    consPrintf("Program must begin with % sign"CONSOLE_NEWLINE_STR);
    return 0;
  }
  for(;;)
  {
    consGetLine(line,256);
    if (line[0]==37 || !strcmp(line, "M30"))
      break;
    gcode_parseline(line);
  }
}
/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Initializes a serial-over-USB CDC driver.
   */
  sduObjectInit(&SDU1);
  sduStart(&SDU1, &serusbcfg);

  /*
   * Activates the USB driver and then the USB bus pull-up on D+.
   * Note, a delay is inserted in order to not have to disconnect the cable
   * after a reset.
   */
  usbDisconnectBus(serusbcfg.usbp);
  chThdSleepMilliseconds(1000);
  usbStart(serusbcfg.usbp, &usbcfg);
  usbConnectBus(serusbcfg.usbp);

  /* Init screen.*/
  gfxInit();

  gfxSleepMilliseconds (10);
  gdispClear (RGB2COLOR(255,255,255));/* glitches.. */
  gfxSleepMilliseconds (10);
  gdispClear (RGB2COLOR(255,255,255));  /* glitches.. */
  gfxSleepMilliseconds (10);
  gdispClear (RGB2COLOR(255,255,255));  /* glitches.. */

  /* Initialize uGFX console.*/
  swidth = gdispGetWidth();
  sheight = gdispGetHeight();
  font = gdispOpenFont("fixed_5x8");
  gwinSetDefaultFont(font);
  bHeight = gdispGetFontMetric(font, fontHeight)+4; // title bar height
  gdispFillStringBox(0, 0, swidth, bHeight, "Marengo", font, Red, White, justifyCenter); // Display title bar
  // Create our main display writing window
  {
      GWindowInit             wi;
      gwinClearInit(&wi);
      wi.show = TRUE; wi.x = 0; wi.y = bHeight; wi.width = swidth; wi.height = sheight-bHeight-40;
      ghc = gwinConsoleCreate(&gc, &wi);
  }

  /* Console commands.*/
  ConsoleCmd consoleCommands[]=
  {
   {"info", CmdInfo},
   {"stpen", CmdStpen},
   {"stpdis", CmdStpdis},
   {"stptg", CmdStptg},
   {"stpmovsteps", CmdStpmovsteps},
   {"stpdir", CmdStpDir},
   {"gcode", CmdGCode},
   {NULL, NULL}
  };

  MarengoConsoleConfig.Stream = &SDU1;
  MarengoConsoleConfig.Win = ghc;
  MarengoConsoleConfig.cmds = consoleCommands;
  MarengoStartConsole();

  /* Initialize stepper driver.*/
  stpInit();
  consPrintf(CONSOLE_NEWLINE_STR"Stepper motor driver initialized"CONSOLE_NEWLINE_STR);
  /*
   * Normal main() thread activity, spawning shells.
   */
  while (true) {

    //if (SDU1.config->usbp->state == USB_ACTIVE) {
    // thread_t *shelltp = chThdCreateFromHeap(NULL, SHELL_WA_SIZE,
    //                                          "shell", NORMALPRIO + 1,
    //                                          shellThread, (void *)&shell_cfg1);
    //  chThdWait(shelltp);               /* Waiting termination.             */
    //}

    chThdSleepMilliseconds(1000);
  }
}
/* ---------------------------------------------------------------------- */
