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

#include "MovementQueue.h"
#include "StepperMove.h"
#include "StepperManager.h"

// Timer and speed settings
#define CLOCK_FREQ CH_CFG_ST_FREQUENCY
#define MAX_FEEDRATE 4000 // in mm per min
#define MIN_FEEDRATE 1
#define STEPPER_START_SPEED 10 //in mm per min
//#define STEPPER_START_DELAY 12000 // ticks of clock (80000*60 1/min / (1mm/min * 400stp/mm))
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
  signed int a;
  signed int b;
  signed int c;
  signed int u;
  signed int v;
  signed int w;
   */
  int stpE;
} stpCoord_t;

typedef struct {
  int stpX;
  int stpY;
  int stpZ;
  int stpE1;
  int dx;
  int dy;
  int dz;
  int de;
  int dm;
  int i; // number of steps to end
  signed char xdir;
  signed char ydir;
  signed char zdir;
  signed char edir;
} stpLinearMove_t;

typedef struct {
  char type;
  stpCoord_t startCoord;
  stpCoord_t endCoord;
  int feedrate;
} stpCmd_t;

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

void stpSetQueue(MovementQueue_t* queue);
void stpSetManager(StepperManager_t* manager);

void stpInit(void); // Init stepper motor driver

stpCoord_t stpCoordAdd(stpCoord_t a, stpCoord_t b);
stpCoord_t stpCoordSub(stpCoord_t a, stpCoord_t b);
int stpCoordAbs(stpCoord_t coord);

stpCoordF_t stpCoordFZero(void);
stpCoordF_t stpCoordFAdd(stpCoordF_t a, stpCoordF_t b);
stpCoordF_t stpCoordFSub(stpCoordF_t a, stpCoordF_t b);

stpCoord_t* stpCoordConvMetric2Steps(stpCoord_t* coord);
stpCoord_t* stpCoordConvMetricF2Steps(stpCoordF_t* input, stpCoord_t* output);

int stpDirToggle(char dsgn);
int stpMoveAxisSteps(char axis, int stpN, int speed, int accel);
int stpMoveAxisUnits(char axis, float distance, int delay);

int stpMMtoSTPS(stpAxis_t *axis, int mm);
int stpSTPStoMM(stpAxis_t *axis, int mm);

int stpAccelRampLinear(int nremstep, stpCoord_t totalsteps);
int stpMoveLinearInit(stpCoordF_t end);
void stpMoveLinear(void *p);
int stpMoveLine(int stpX, int stpY, int stpZ, int stpE, int delay);
int stpMoveArc(stpCoord_t start, stpCoord_t end, stpCoord_t center, int feedrate);

void stpSetHome(void);
void stpSetPosition(stpCoordF_t pos);
void stpStop(void);
stpCoord_t stpGetCurrentPos(void);
stpCoordF_t stpGetCoordF(void);
int stpGetFeedrate(void);
int stpSetFeedRate(int feedrate);

stpCoord_t stpCurrentAbsPos; // current position in steps
stpCoord_t stpCurrentSpeed;

stpLinearMove_t stpCurrentMove;

int stpFeedrate;
int stpAccel;
int stpModeInc;

MovementQueue_t* stpMovementQueue;
StepperManager_t* stpManager;

virtual_timer_t vt;
char stpStatus;



#endif /* MARENGO_STEPPER_H_ */
