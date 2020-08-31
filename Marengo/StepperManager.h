/*
 * StepperManager.h
 *
 *  Created on: 1 lip 2019
 *      Author: grzeg
 */

#ifndef MARENGO_STEPPERMANAGER_H_
#define MARENGO_STEPPERMANAGER_H_

#include "StepperProxy.h"
#include "EndstopProxy.h"
#include "StepperMove.h"
#include "MovementQueue.h"
#include "coord.h"
#include "ch.h"

#define STEPPER_MANAGER_CLOCK_FREQ CH_CFG_ST_FREQUENCY
#define STEPPER_MANAGER_MINFEED 100
#define STEPPER_MANAGER_MAXFEED 1000
#define STEPPER_MANAGER_STPACCEL 10

// TODO: Add enum for stepper motors?
// TODO: Shouldn't STEPPER_NUM be defined here?

typedef struct {
  StepperProxy_t* Stepper[STEPPER_NUM];
  EndstopProxy_t* Endstop[STEPPER_NUM];
  bool b_NextMoveReady;
  bool b_InMove;
  StepperMove_t move;
  StepperMove_t buffered_move;
  MovementQueue_t* queue;
  virtual_timer_t vt;
  thread_t* tp;
  uint32_t delay;
} StepperManager_t;

THD_WORKING_AREA(StepperManagerThreadWorkingArea, 1024);

void StepperManager_Init(StepperManager_t* const me);
void StepperManager_StartISR(StepperManager_t* const me);
void StepperManager_StopISR(StepperManager_t* const me);
void StepperManager_ISRCallback(void *p);
static THD_FUNCTION(StepperManager_ThreadFunction, arg);

void StepperManager_SetStepper(StepperManager_t* const me, StepperProxy_t* step,
                              int i);
void StepperManager_SetEndstop(StepperManager_t* const me, EndstopProxy_t* end,
                               int i);
void StepperManager_SetItsMovementQueue(StepperManager_t* const me,
                                        MovementQueue_t* queue);
MovementQueue_t* StepperManager_GetItsMovementQueue(StepperManager_t* const me);
stpCoordF_t StepperManager_GetFinalPosition(StepperManager_t* const me);
stpCoord_t StepperManager_ConvertPositionToStps(StepperManager_t* const me,
                                                stpCoordF_t coord);

stpCoordF_t StepperManager_GetPosition(StepperManager_t* const me);
uint32_t StepperManager_GetCurrentFeedrate(StepperManager_t* const me);
void StepperManager_SetPosition(StepperManager_t* const me,
                                       stpCoordF_t position);
bool_t StepperManager_IsMoving(StepperManager_t* const me);
StepperManager_t* StepperManager_Create(memory_heap_t* heap);
void StepperManager_Destroy(StepperManager_t* const me);
#endif /* MARENGO_STEPPERMANAGER_H_ */
