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
// speed in stp/s
// accel in stp/s^2
int stpMoveAxisSteps(char axis, int stpN, int speed, int accel)
{
  int i = 0;
  while (stpAxes[i].designation != axis) {
    i++;
  }
  if (i > STP_AXES_NUM)
    return 1;
  int accel_steps = speed*speed/accel/2;
  int delay = sqrt(2000000000000/accel);
  if( accel_steps*2>stpN){
    consPrintf("Error acceleration and deceleration longer than whole movement. Consider increasing acceleration"CONSOLE_NEWLINE_STR);
    return 1;
  }
  consPrintf("Acceleration steps: %d"CONSOLE_NEWLINE_STR, accel_steps);
  consPrintf("Initial delay: %d"CONSOLE_NEWLINE_STR, delay);
  palClearLine(stpAxes[i].line_en);
  for (int n = 0; n < accel_steps; n++)
  {
    palToggleLine(stpAxes[i].line_stp);
    gptPolledDelay(&GPTD4, delay);
    delay = sqrt(2*(n+1)*1000000000000/accel)-sqrt(2*n*1000000000000/accel);
  }
  consPrintf("Acceleration complete. Moving with delay: %d"CONSOLE_NEWLINE_STR, delay);
  for (int n = accel_steps; n < stpN-accel_steps; n++) {
    palToggleLine(stpAxes[i].line_stp);
    //chThdSleepMicroseconds(delay);
    gptPolledDelay(&GPTD4, delay);
  }
  consPrintf("Decelerating..."CONSOLE_NEWLINE_STR);
  for (int n = 0; n < accel_steps; n++)
  {
    palToggleLine(stpAxes[i].line_stp);
    gptPolledDelay(&GPTD4, delay);
    delay = sqrt(2*(accel_steps-n)*1000000000000/accel)-sqrt(2*(accel_steps-n-1)*1000000000000/accel);
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
  return 0;
}
