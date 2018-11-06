/*
 * stepper.h
 *
 *  Created on: 8 lip 2018
 *      Author: grzeg
 */

#ifndef MARENGO_STEPPER_H_
#define MARENGO_STEPPER_H_

#include "ch.h"
#include "hal.h"
#include "pin_mapping.h"

// Timer settings
#define CLOCK_FREQ 10240000 // No less than MAX_SPEED*STEPS_PER_MM
#define MAX_SPEED 200 // in mm per s

typedef struct {
  char designation;
  int steps_per_rev;
  int microstep_div;
  int thread_jump; // in mm
  ioline_t line_stp;
  ioline_t line_dir;
} stpAxis_t;

// Clock controlling steppers
static const GPTConfig gpt4cfg = {
  CLOCK_FREQ, //frequency of timer clock.
  NULL, // No callback
  0, 0
};

// Axes
#define STP_AXES_NUM 4
static stpAxis_t stpAxes[STP_AXES_NUM] = {
{ 'Y', 200, 16, 2, LINE_YSTP, LINE_YDIR},
{ 'X', 400, 16, 2, LINE_XSTP, LINE_XDIR},
{ 'Z', 200 ,16, 2, LINE_ZSTP, LINE_ZDIR},
{ 'A', 200, 16, 1, LINE_SPINDLE_ENABLE, LINE_SPINDLE_DIRECTION} // TODO: Resolve how to cope with spindle 'axis'
};

static bool stpEnabled = false; //

void stpInit(void); // Init stepper motor driver
void stpEnable(void); // Enable stepper motors
void stpDisable(void); // Disable stepper motors
void stpToggle(void); // Toggle stepper motors

int stpDirToggle(char dsgn);
int stpMoveAxisSteps(char axis, int stpN, int delay);
int stpMoveAxisUnits(char axis, float distance, int delay);

#endif /* MARENGO_STEPPER_H_ */
