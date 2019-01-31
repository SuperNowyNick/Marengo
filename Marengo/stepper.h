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
  char designation; // axis designation
  int steps_per_rev; // steps per revolution
  int microstep_div; // microsteps per division
  int thread_jump; // in mm
  ioline_t line_stp; // step IO line
  ioline_t line_dir; // direction IO line
  ioline_t line_en; // enable IO line
} stpAxis_t;

typedef struct {
  char designation; // axis designation
  char side; // 0 for beggining, 1 for end
  char active; // state of given endstop
  ioline_t line; // Endstop ioline
} stpEndstop_t;

// Clock controlling steppers
/*static const GPTConfig gpt4cfg = {
  CLOCK_FREQ, //frequency of timer clock.
  NULL, // No callback
  0, 0
};*/

// Stepper motors
#define STP_AXES_NUM 4
static stpAxis_t stpAxes[STP_AXES_NUM] = {
{ 'Y', 200, 16, 2, LINE_YSTP, LINE_YDIR, LINE_YEN},
{ 'X', 400, 16, 2, LINE_XSTP, LINE_XDIR, LINE_XEN},
{ 'Z', 200 ,16, 2, LINE_ZSTP, LINE_ZDIR, LINE_ZEN},
{ 'E', 200, 16, 1, LINE_E1STP, LINE_E1DIR, LINE_E1EN}
};

#define STP_ENDSTOPS_NUM 3
static stpEndstop_t stpEndstops[STP_ENDSTOPS_NUM] = {
{'X', 0, 0, LINE_XMIN},
{'Y', 0, 0, LINE_YMIN},
{'Z', 0, 0, LINE_ZMIN}
};

palcallback_t stpEndstopCallback(void *arg);

void stpInit(void); // Init stepper motor driver

int stpDirToggle(char dsgn);
int stpMoveAxisSteps(char axis, int stpN, int delay);
int stpMoveAxisUnits(char axis, float distance, int delay);

#endif /* MARENGO_STEPPER_H_ */
