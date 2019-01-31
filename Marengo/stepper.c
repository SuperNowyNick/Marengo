/*
 * stepper.c
 *
 *  Created on: 8 lip 2018
 *      Author: grzeg
 */

#include "stepper.h"
#include "console.h" // Temporarily for debug reasons

/* Initialize stepper motor driver.*/

void stpInit(void)
{
  //gptStart(&GPTD4, &gpt4cfg);
  consPrintf("Initializing steppers"CONSOLE_NEWLINE_STR);
  for (int i = 0; i < STP_AXES_NUM; i++) {
    consPrintf("Axis %c no: %d line: %d "CONSOLE_NEWLINE_STR, stpAxes[i].designation, i, stpAxes[i].line_stp);
    palSetLineMode(stpAxes[i].line_dir, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(stpAxes[i].line_stp, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(stpAxes[i].line_en, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(stpAxes[i].line_stp);
    palClearLine(stpAxes[i].line_dir);
    palSetLine(stpAxes[i].line_en);
  }
  consPrintf("Initializing endstops"CONSOLE_NEWLINE_STR);
  for (int i = 0; i < STP_ENDSTOPS_NUM; i++) {
    consPrintf("Endstop on axis %c no: %d line: %d"CONSOLE_NEWLINE_STR, stpEndstops[i].designation, i, stpEndstops[i].line);
    palSetLineMode(stpEndstops[i].line, PAL_MODE_INPUT_PULLDOWN);
    palSetLineCallback(stpEndstops[i].line, stpEndstopCallback, (void*)&stpEndstops[i]);
    stpEndstops[i].active = palReadLine(stpEndstops[i].line);
  }
}

// Callback from endstop line, arg is
palcallback_t stpEndstopCallback(void *arg)
{
  stpEndstop_t* endstop = arg;
  endstop->active = palReadLine(endstop->line);
  palTogglePad(GPIOG, GPIOG_LED4_RED);
  consPrintf("Endstop %c at end %d is %d"CONSOLE_NEWLINE_STR, endstop->designation, endstop->side, endstop->active);
}

/* Do stpN steps with delay in us between them on given axis.*/
int stpMoveAxisSteps(char axis, int stpN, int delay)
{
  int i = 0;
  while (stpAxes[i].designation != axis) {
    i++;
  }
  if (i > STP_AXES_NUM)
    return 1;
  palClearLine(stpAxes[i].line_en);
  for (int n = 0; n < stpN; n++) {
    palToggleLine(stpAxes[i].line_stp);
    //chThdSleepMilliseconds(delay);
    gptPolledDelay(&GPTD4, delay);
    palToggleLine(stpAxes[i].line_stp);
    //chThdSleepMilliseconds(delay);
    gptPolledDelay(&GPTD4, delay);
  }
  palSetLine(stpAxes[i].line_en);
  return 0;
}

/* Do distance movement in mm with delay between steps on given axis.*/
int stpMoveAxisUnits(char axis, float distance, int delay) {
  int i = 0;
  // Find desired axis
  while (stpAxes[i].designation != axis) {
    i++;
  }
  if (i > STP_AXES_NUM)
    return 1;
  // Axis found, calculate number of steps
  int stpN = (int)(distance / stpAxes[i].thread_jump * stpAxes[i].steps_per_rev);
  // Do the motion
  palClearLine(stpAxes[i].line_en);
  for (int n = 0; n < stpN; n++) {
    palToggleLine(stpAxes[i].line_stp);
    //chThdSleepMilliseconds(delay);
    gptPolledDelay(&GPTD4, delay);
    palToggleLine(stpAxes[i].line_stp);
    //chThdSleepMilliseconds(delay);
    gptPolledDelay(&GPTD4, delay);
  }
  palSetLine(stpAxes[i].line_en);
  return 0;
}

/* Toggle direction in which the carriage moves on a given axis.*/
int stpDirToggle(char dsgn) {
  int i = 0;
  while (stpAxes[i].designation != dsgn) {
    i++;
  }
  if (i > STP_AXES_NUM)
    return 1;
  palToggleLine(stpAxes[i].line_dir);
}
