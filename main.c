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
#include "Marengo/heater.h"

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
ccmd_t CmdStpmovsteps(int argc, char **argv)
{
  if (argc!=5){
    consPrintf("Not enough or too many parameters!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  consPrintf(argv[1]);
  if (!strspn(argv[1], "XYZE")){
    consPrintf("No such axis!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  char axis = *argv[1];
  int steps, speed, accel;
  steps = atoi(argv[2]);
  speed = atoi(argv[3]);
  accel = atoi(argv[4]);
  if (steps<=0){
    consPrintf("Steps number must be numerical and greater than zero!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  if (speed<=0){
    consPrintf("Speed must be numerical and greater than zero!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  if (accel<=0){
    consPrintf("Acceleration must be numerical and greater than zero!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  consPrintf("Moving axis %c with %d steps at %d mm/s speed with acceleration %d mm/s^2"CONSOLE_NEWLINE_STR, axis, steps, speed, accel);
  if (stpMoveAxisSteps(axis, steps, speed, accel)){
    consPrintf("Error!"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  else
    return CCMD_SUCCES;
}
ccmd_t CmdStpDir(int argc, char **argv)
{
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
}
ccmd_t CmdGCode(int argc, char **argv)
{
  char line[256];
  consGetLine(line, 256);
  if (line[0]!='%'){
    consPrintf("Program must begin with % sign"CONSOLE_NEWLINE_STR);
    return CCMD_FAIL;
  }
  for(;;)
  {
    consGetLine(line,256);
    if (line[0]==37 || !strcmp(line, "M30"))
      break;
    gcode_parseline(line);
  }
  return CCMD_SUCCES;
}
ccmd_t CmdHeatADC(int argc, char **argv)
{
  consPrintf("Extruder thermistor adc val: %d"CONSOLE_NEWLINE_STR, HeaterGetADCValue());
  return CCMD_SUCCES;
}
ccmd_t CmdHeatTemp(int argc, char **argv)
{
  consPrintf("Extruder thermistor temp: %d"CONSOLE_NEWLINE_STR, getHeaterTemp(&Heater1, HeaterGetADCValue()));
  return CCMD_SUCCES;
}
ccmd_t CmdHeatPWM(int argc, char **argv)
{
  if (argc!=2){
  consPrintf("Not enough or too many parameters!"CONSOLE_NEWLINE_STR);
  return CCMD_FAIL;
}
  HeaterSetPWM(atoi(argv[1]));
  consPrintf("Extruder heater PWM set to %d percent"CONSOLE_NEWLINE_STR, atoi(argv[1]));
  return CCMD_SUCCES;
}
ccmd_t CmdHeatVolt(int argc, char **argv)
{
  int adc_val = HeaterGetADCValue();
  int voltageint = adc_val*330/0xFFF/100;
  int voltagefrac = adc_val*330/0xFFF%100;
    consPrintf("Extruder thermistor adc val: %d"CONSOLE_NEWLINE_STR, adc_val);

  consPrintf("Extruder thermistor volt: %d.", voltageint);
  if(voltagefrac<10)
    consPrintf("0");
  consPrintf("%d"CONSOLE_NEWLINE_STR, voltagefrac);
  return CCMD_SUCCES;
}
ccmd_t CmdHeatOn(int argc, char **argv)
{
  HeaterOn(&Heater1);
  consPrintf("Extruder heater turned on"CONSOLE_NEWLINE_STR);
  return CCMD_SUCCES;
}
ccmd_t CmdHeatOff(int argc, char **argv)
{
  HeaterOff(&Heater1);
  consPrintf("Extruder heater turned off"CONSOLE_NEWLINE_STR);
  return CCMD_SUCCES;
}
ccmd_t CmdHeatIntegral(int arg, char **argv)
{
  consPrintf("Total ADC integral: %d"CONSOLE_NEWLINE_STR, intdataHeaterADC(&Heater1));
  return CCMD_SUCCES;
}
ccmd_t CmdHeatDifferentiate(int arg, char **argv)
{
  consPrintf("Total ADC differential: %d"CONSOLE_NEWLINE_STR, diffdataHeaterADC(&Heater1));
  return CCMD_SUCCES;
}
ccmd_t CmdHeatSet(int argc, char **argv)
{
  if (argc!=2){
  consPrintf("Not enough or too many parameters!"CONSOLE_NEWLINE_STR);
  return CCMD_FAIL;
  }
  HeaterSetTemp(&Heater1, atoi(argv[1]));
  consPrintf("Extruder heater temp set to %d degrees"CONSOLE_NEWLINE_STR, atoi(argv[1]));
  return CCMD_SUCCES;
}
ccmd_t CmdEndstops(int argc, char **argv)
{
  consPrintf("Endstop 1: %d"CONSOLE_NEWLINE_STR, palReadLine(LINE_XMIN));
  consPrintf("Endstop 2: %d"CONSOLE_NEWLINE_STR, palReadLine(LINE_YMIN));
  consPrintf("Endstop 3: %d"CONSOLE_NEWLINE_STR, palReadLine(LINE_ZMIN));
  return CCMD_SUCCES;
}
/*===========================================================================*/
/* Initialization and main thread.                                           */
/*===========================================================================*/

/*
 * Application entry point.
 */
/*
#define SPI_PORT    &SPID5
#define DC_PORT     GPIOD
#define DC_PIN      GPIOD_LCD_WRX

static const SPIConfig spi_cfg = {
    NULL,
    GPIOC,
    GPIOC_SPI5_LCD_CS,
    ((1 << 3) & SPI_CR1_BR) | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR
};
*/
/*
 * Red LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker1");
  while (true) {
    palClearPad(GPIOG, GPIOG_LED4_RED);
    chThdSleepMilliseconds(500);
    palSetPad(GPIOG, GPIOG_LED4_RED);
    chThdSleepMilliseconds(500);
  }
}

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread2, 128);
static THD_FUNCTION(Thread2, arg) {

  (void)arg;
  chRegSetThreadName("blinker2");
  while (true) {
    palClearPad(GPIOG, GPIOG_LED3_GREEN);
    chThdSleepMilliseconds(250);
    palSetPad(GPIOG, GPIOG_LED3_GREEN);
    chThdSleepMilliseconds(250);
  }
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
  //halInit();
  //chSysInit();
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

  gfxSleepMilliseconds (10);
  gdispClear (RGB2COLOR(255,0,0));
  gfxSleepMilliseconds (10);
  gdispClear (RGB2COLOR(0,255,0));
  gfxSleepMilliseconds (10);
  gdispClear (RGB2COLOR(0,0,255));
  gfxSleepMilliseconds (10);
  gdispClear (RGB2COLOR(0,0,0));
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
  ConsoleCmd consoleCommands[]=
  {
   {(const char*)"info", (ccmd_t)CmdInfo},
   {"threads", CmdThreads},
   {"stpmovsteps", CmdStpmovsteps},
   {"stpdir", CmdStpDir},
   {"gcode", CmdGCode},
   {"heatadc", CmdHeatADC},
   {"heatvolt", CmdHeatVolt},
   {"heaton", CmdHeatOn},
   {"heatoff", CmdHeatOff},
   {"endstops", CmdEndstops},
   {"heattemp", CmdHeatTemp},
   {"heatpwm", CmdHeatPWM},
   {"heatint", CmdHeatIntegral},
   {"heatdiff", CmdHeatDifferentiate},
   {"heatsettemp", CmdHeatSet},
   {NULL, NULL}
  };

  consInit();
  consConfig.Stream = (BaseSequentialStream*)&SDU1;
  consConfig.Win = ghc;
  consConfig.cmds = consoleCommands;

  stpInit();
  consPrintf(CONSOLE_NEWLINE_STR"Stepper motor driver initialized"CONSOLE_NEWLINE_STR);
  HeaterInit();
  consPrintf(CONSOLE_NEWLINE_STR"Heater driver initialized"CONSOLE_NEWLINE_STR);

  // Wait for SDU1 state change to USB_ACTIVE
  while(SDU1.config->usbp->state != USB_ACTIVE)
  {
    chThdSleepMilliseconds(1000);
  }
  consStart(); // Start the console


  /*
   * Normal main() thread activity, spawning shells.
   */
  while (true) {
    chThdSleepMilliseconds(1000);
  }
  return 0;
}
/* ---------------------------------------------------------------------- */
