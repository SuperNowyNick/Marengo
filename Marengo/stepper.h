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
#include "float.h"

// Timer and speed settings
#define CLOCK_FREQ 80000 // 400stp/mm * 200mm/min gives 80000 stp/min
                         // we will
#define MAX_FEEDRATE 200 // in mm per min
#define STEPPER_START_SPEED 1 //in mm per min
#define STEPPER_START_DELAY 12000 // ticks of clock (80000*60 1/min / (1mm/min * 400stp/mm))
#define STEPPER_ACCEL 10 // in mm per s

#define STP_CMD_MODE_LINE 0
#define STP_CMD_MODE_ARC 1

#define STP_STATE_STOPPED 0
#define STP_STATE_RUNNING 1
#define STP_STATE_WAITING 2

typedef struct {
  char designation; // axis designation
  int steps_per_rev; // steps per revolution
  int microstep_div; // microsteps per division
  int thread_jump; // in mm
  int gear_ratio;
  int steps_per_mm;
  bool is_linear;
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

typedef struct {
  int x;
  int y;
  int z;
  /*
  int a;
  int b;
  int c;
  int u;
  int v;
  int w;
   */
  int stpE;
} stpCoord_t;



typedef struct {
  char type;
  stpCoord_t startCoord;
  stpCoord_t endCoord;
  int feedrate;
} stpCmd_t;


// Clock controlling steppers
static const GPTConfig gpt4cfg = {
  CLOCK_FREQ, //frequency of timer clock.
  NULL, // No callback
  0, 0
};

// Stepper motors
#define STP_AXES_NUM 4
static stpAxis_t stpAxes[STP_AXES_NUM] = {
{ 'Y', 400, 1, 2, 1, 200, TRUE, LINE_YSTP, LINE_YDIR, LINE_YEN},
{ 'X', 800, 1, 2, 1, 400, TRUE, LINE_XSTP, LINE_XDIR, LINE_XEN},
{ 'Z', 400 ,1, 2, 1, 200, TRUE, LINE_ZSTP, LINE_ZDIR, LINE_ZEN},
{ 'E', 800, 1, 1, 1, 105, FALSE, LINE_E1STP, LINE_E1DIR, LINE_E1EN}
};

#define STP_ENDSTOPS_NUM 3
static stpEndstop_t stpEndstops[STP_ENDSTOPS_NUM] = {
{'X', 0, 0, LINE_XMIN},
{'Y', 0, 0, LINE_YMIN},
{'Z', 0, 0, LINE_ZMIN}
};

palcallback_t stpEndstopCallback(void *arg);



void stpInit(void); // Init stepper motor driver

stpCoord_t stpCoordAdd(stpCoord_t a, stpCoord_t b);
stpCoord_t stpCoordSub(stpCoord_t a, stpCoord_t b);

int stpDirToggle(char dsgn);
int stpMoveAxisSteps(char axis, int stpN, int speed, int accel);
int stpMoveAxisUnits(char axis, float distance, int delay);

int stpMMtoSTPS(stpAxis_t *axis, int mm);
int stpSTPStoMM(stpAxis_t *axis, int mm);

int stpAccelRampLinear(int nremstep, stpCoord_t totalsteps);
int stpMoveLinear(stpCoord_t end);
int stpMoveLine(int stpX, int stpY, int stpZ, int stpE, int delay);
int stpMoveArc(stpCoord_t start, stpCoord_t end, stpCoord_t center, int feedrate);

void stpSetHome(void);
stpCoord_t stpGetCurrentPos(void);
int stpCoordAbs(stpCoord_t coord);

stpCoord_t stpCurrentAbsPos;
stpCoord_t stpCurrentSpeed;
int stpFeedrate;
int stpAccel;
int stpModeInc;



#endif /* MARENGO_STEPPER_H_ */
