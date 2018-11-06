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
  gptStart(&GPTD4, &gpt4cfg);

  for (int i = 0; i < STP_AXES_NUM; i++) {
    consPrintf("%d  %d"CONSOLE_NEWLINE_STR, i, stpAxes[i].line_stp);
    palSetLineMode(stpAxes[i].line_dir, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(stpAxes[i].line_stp, PAL_MODE_OUTPUT_PUSHPULL);
  }
  palSetLineMode(LINE_YSTP, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_YDIR, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_XSTP, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_XDIR, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_ZSTP, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_ZDIR, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_ENABLE, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_SPINDLE_ENABLE, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_SPINDLE_DIRECTION, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(LINE_ENABLE, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLine(LINE_ENABLE);
}

/* Enable stepper motor drivers.*/
void stpEnable(void)
{
  palClearLine(LINE_ENABLE);
  stpEnabled = true;
}

/* Disable stepper motor dirver.*/
void stpDisable(void)
{
  palSetLine(LINE_ENABLE);
  stpEnabled = false;
}

/* Toggle stepper motor driver.*/
void stpToggle(void)
{
  palToggleLine(LINE_ENABLE);
  stpEnabled = !stpEnabled;
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
  for (int n = 0; n < stpN; n++) {
    palToggleLine(stpAxes[i].line_stp);
    gptPolledDelay(&GPTD4, delay);
    palToggleLine(stpAxes[i].line_stp);
    gptPolledDelay(&GPTD4, delay);
  }
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
  for (int n = 0; n < stpN; n++) {
    palToggleLine(stpAxes[i].line_stp);
    gptPolledDelay(&GPTD4, delay);
    palToggleLine(stpAxes[i].line_stp);
    gptPolledDelay(&GPTD4, delay);
  }
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
