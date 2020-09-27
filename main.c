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
#include "Marengo/gcode.h"
#include "Marengo/heater.h"
#include "Marengo/gui.h"

#include "Marengo/StepperProxy.h"
#include "Marengo/StepperManager.h"
static   CH_HEAP_AREA(testheap, 256);

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
  return CCMD_SUCCES;
}
ccmd_t CmdThreads(int argc, char **argv)
{
  static const char *states[] = {CH_STATE_NAMES};
  thread_t *tp;

  (void)argv;
  consPrintf("stklimit    stack     addr refs prio     state         name\r\n" CONSOLE_NEWLINE_STR);
  tp = chRegFirstThread();
  do {
#if (CH_DBG_ENABLE_STACK_CHECK == TRUE) || (CH_CFG_USE_DYNAMIC == TRUE)
    uint32_t stklimit = (uint32_t)tp->wabase;
#else
    uint32_t stklimit = 0U;
#endif
    consPrintf("%08lx %08lx %08lx %4lu %4lu %9s %12s"CONSOLE_NEWLINE_STR,
             stklimit, (uint32_t)tp->ctx.sp, (uint32_t)tp,
             (uint32_t)tp->refs - 1, (uint32_t)tp->prio, states[tp->state],
             tp->name == NULL ? "" : tp->name);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
  return CCMD_SUCCES;
}
ccmd_t CmdStpDir(int argc, char **argv)
{
  consPrintf("Currently not supported!"CONSOLE_NEWLINE_STR);
  /*
  if (argc!=2){
    consPrintf("Not enough or too many parameters!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  consPrintf(argv[1]);
  if (!strspn(argv[1], "XYZE")){
    consPrintf("No such axis!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  char axis = *argv[1];
  if (stpDirToggle(axis)){
    consPrintf("Error!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  else{
  consPrintf("Axis %c direction changed"CONSOLE_NEWLINE_STR, axis);
  return CCMD_SUCCES;
  }
  */
  return CCMD_SUCCES;
}
ccmd_t CmdGCode(int argc, char **argv)
{
  char line[GCODE_MAXLINELENGTH];
  gcommand_t cmd;
  cmd.programflow = PROGRAM_FLOW_RUNNING;
  consPrintf("GCode Interpreter"CONSOLE_NEWLINE_STR);
  for(;;)
  {
    consGetLine(line,GCODE_MAXLINELENGTH);
    if (line[0]==37 || !strcmp(line, "M30"))
      break;
    chThdSleepMilliseconds(100);
    cmd = gcode_parseline(line);
    if (cmd.programflow == PROGRAM_FLOW_STOPPED){
        consPrintf("GCode Interpreter stopped"CONSOLE_NEWLINE_STR);
        break;
    }
    gcodeParseCommand(cmd);
  }
  return CCMD_SUCCES;
}
ccmd_t CmdHeatTemp(int argc, char **argv)
{
  if (argc!=2){
  consPrintf("Not enough or too many parameters!"CONSOLE_NEWLINE_STR);
  return CCMD_FAIL;
  }
  if(atoi(argv[1])==1)
    consPrintf("Extruder thermistor temp: %d"CONSOLE_NEWLINE_STR, heaterGetTemp(&Heater1));
  else if(atoi(argv[1])==2)
    consPrintf("Extruder thermistor temp: %d"CONSOLE_NEWLINE_STR, heaterGetTemp(&Heater2));
  else {
    consPrintf("No such heater!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  return CCMD_SUCCES;
}
ccmd_t CmdHeatSet(int argc, char **argv)
{
  if (argc!=3){
  consPrintf("Not enough or too many parameters!"CONSOLE_NEWLINE_STR);
  return CCMD_FAIL;
  }
  heater_t *heat;
  if(atoi(argv[1])==1){
    heat=&Heater1;
    consPrintf("Extruder ");
  }
  else if(atoi(argv[1])==2){
    heat=&Heater2;
    consPrintf("Heatbed ");
  }
  else {
    consPrintf("No such heater!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  heaterSetTemp(heat, atoi(argv[2]));
  consPrintf("heater temp set to %d degrees"CONSOLE_NEWLINE_STR, atoi(argv[2]));
  return CCMD_SUCCES;
}
ccmd_t CmdEndstops(int argc, char **argv)
{
  consPrintf("Endstop 1: %d"CONSOLE_NEWLINE_STR, palReadLine(LINE_XMIN));
  consPrintf("Endstop 2: %d"CONSOLE_NEWLINE_STR, palReadLine(LINE_YMIN));
  consPrintf("Endstop 3: %d"CONSOLE_NEWLINE_STR, palReadLine(LINE_ZMIN));
  return CCMD_SUCCES;
}
ccmd_t CmdMoveLine(int argc, char **argv)
{
  if(argc<5){
    consPrintf("Not enough or too many parameters!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
    }
  StepperMove_t move;
  StepperMove_Init(&move);
  move.coord[1] = myatof(argv[1]);
  move.coord[1] = myatof(argv[2]);
  move.coord[2] = myatof(argv[3]);
  move.coord[3] = myatof(argv[4]);
  if(argc>5)
    move.feedrate = atoi(argv[5]);
  consPrintf("Moving in line to x:");
  printFloat(move.coord[0]);
  consPrintf("y:");
  printFloat(move.coord[1]);
  consPrintf("z:");
  printFloat(move.coord[2]);
  consPrintf("e:");
  printFloat(move.coord[3]);
  consPrintf(CONSOLE_NEWLINE_STR);
  // TODO: Add movement to queue MovementQueue_Push(queue ,move);
  // But where to get which queue?
  return CCMD_SUCCES;
}
ccmd_t CmdStpStop(int argc, char **argv)
{
  consPrintf("Currently not supported!"CONSOLE_NEWLINE_STR);
  /*
  stpStop();
  consPrintf("Stepper motors stopped!"CONSOLE_NEWLINE_STR);
  */
  return CCMD_SUCCES;
}

int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  gfxInit();
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

  guiInit();

  // Configure console
  ConsoleCmd consoleCommands[]=
  {
   {"info", (ccmd_t)CmdInfo, "Display system information"},
   {"threads", (ccmd_t)CmdThreads, "Displays threads informationn"},
   {"stpdir", (ccmd_t)CmdStpDir, "Changes direction of movement for stepper motor. Args: axis"},
   {"gcode", (ccmd_t)CmdGCode, "Opens up gcode interpreter"},
   {"endstops", (ccmd_t)CmdEndstops, "Displays endstops status"},
   {"heattemp", (ccmd_t)CmdHeatTemp, "Displays heater temperature. Args heater num"},
   {"heatsettemp", (ccmd_t)CmdHeatSet, "Set heater temperature. Args: heater num, temp in celsius"},
   {"stpmovline", (ccmd_t)CmdMoveLine, "Move to coords x,y,z,e, with delay e"},
   {"stpstop", (ccmd_t)CmdStpStop, "Immidiately stop stepper motors"},
   {NULL, NULL, NULL}
  };

  consInit();
  consConfig.Stream = (BaseAsynchronousChannel*)&SDU1;
  consConfig.Win = guiConsoleGetWinHandle();
  consConfig.cmds = consoleCommands;

  // Configure heater
  heaterInit();

  // Configure stepper motor proxies
  StepperProxy_t* stepX = Stepper_ProxyCreate(NULL);
  StepperProxy_t* stepY = Stepper_ProxyCreate(NULL);
  StepperProxy_t* stepZ = Stepper_ProxyCreate(NULL);
  StepperProxy_t* stepE = Stepper_ProxyCreate(NULL);
  stepX->Axis = STEPPER_AXIS_X;
  stepX->StepsPerRev=800;
  stepX->Microsteps=1;
  stepX->GearRatio=1;
  stepX->ThreadJumpUM=2000;
  stepX->maxFeedrate=800;
  stepX->Direction=DIR_FORWARD;
  stepX->bNeedStall = FALSE;
  stepX->bLinear = TRUE;
  stepX->lineStp = LINE_XSTP;
  stepX->lineEn = LINE_XEN;
  stepX->lineDir = LINE_XDIR;
  StepperProxy_Configure(stepX);
  stepY->Axis = STEPPER_AXIS_Y;
  stepY->StepsPerRev = 400;
  stepY->Microsteps = 1;
  stepY->GearRatio = 1;
  stepY->ThreadJumpUM = 2000;
  stepY->maxFeedrate = 900;
  stepY->Direction = DIR_FORWARD;
  stepY->bNeedStall = FALSE;
  stepY->bLinear = TRUE;
  stepY->lineStp = LINE_YSTP;
  stepY->lineEn = LINE_YEN;
  stepY->lineDir = LINE_YDIR;
  StepperProxy_Configure(stepY);
  stepZ->Axis = STEPPER_AXIS_Z;
  stepZ->StepsPerRev = 400;
  stepZ->Microsteps = 1;
  stepZ->GearRatio = 1;
  stepZ->ThreadJumpUM = 2000;
  stepZ->maxFeedrate = 60;
  stepZ->Direction = DIR_FORWARD;
  stepZ->bNeedStall = FALSE;
  stepZ->bLinear = TRUE;
  stepZ->lineStp = LINE_ZSTP;
  stepZ->lineEn = LINE_ZEN;
  stepZ->lineDir = LINE_ZDIR;
  StepperProxy_Configure(stepZ);
  stepE->Axis = STEPPER_AXIS_E;
  stepE->StepsPerRev = 800;
  stepE->Microsteps = 1;
  stepE->GearRatio = 3;
  stepE->ThreadJumpUM = 7300;
  stepE->maxFeedrate = 800;
  stepE->Direction = DIR_FORWARD;
  stepE->bNeedStall = FALSE;
  stepE->bLinear = FALSE;
  stepE->lineStp = LINE_E1STP;
  stepE->lineEn = LINE_E1EN;
  stepE->lineDir = LINE_E1DIR;
  StepperProxy_Configure(stepE);

  // Configure endstop proxies
  EndstopProxy_t* endstopX = EndstopProxy_Create(NULL);
  EndstopProxy_t* endstopY = EndstopProxy_Create(NULL);
  EndstopProxy_t* endstopZ = EndstopProxy_Create(NULL);
  EndstopProxy_Configure(endstopX, LINE_XMIN);
  EndstopProxy_Configure(endstopY, LINE_YMIN);
  EndstopProxy_Configure(endstopZ, LINE_ZMIN);

  // Configure stepper manager
  StepperManager_t* stepmanager = StepperManager_Create(NULL);
  StepperManager_SetStepper(stepmanager, stepX, 0);
  StepperManager_SetStepper(stepmanager, stepY, 1);
  StepperManager_SetStepper(stepmanager, stepZ, 2);
  StepperManager_SetStepper(stepmanager, stepE, 3);

  // Configure movement queue
  MovementQueue_t* queue = MovementQueue_Create(NULL);
  StepperManager_SetItsMovementQueue(stepmanager, queue);


  //guiSetStepperManager(stepmanager);
  guiStart();
  gcode_init(stepmanager, queue);

  // Wait for SDU1 state change to USB_ACTIVE
  while(SDU1.config->usbp->state != USB_ACTIVE)
  {
    chThdSleepMilliseconds(1000);
  }
  consStart(); // Start the console

  /*
   * Normal main() thread activity
   */

  while (true) {
    chThdSleepMilliseconds(1000);
  }
  return 0;
}
/* ---------------------------------------------------------------------- */
