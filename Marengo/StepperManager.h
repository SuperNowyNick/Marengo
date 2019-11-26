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
#include "ch.h"

#define STEPPER_MANAGER_STEPPER_NUM 4

#define STEPPER_MANAGER_CLOCK_FREQ CH_CFG_ST_FREQUENCY
#define STEPPER_MANAGER_MINFEED 10
#define STEPPER_MANAGER_MAXFEED 1000
#define STEPPER_MANAGER_STPACCEL 10

typedef struct {
  StepperProxy_t* Stepper[STEPPER_MANAGER_STEPPER_NUM];
  EndstopProxy_t* Endstop[STEPPER_MANAGER_STEPPER_NUM];
  StepperMove_t move;
  MovementQueue_t* queue;
  virtual_timer_t vt;
  thread_t* tp;
  uint32_t delay;
} StepperManager_t;

static THD_WORKING_AREA(StepperManagerThreadWorkingArea, 128);

void StepperManager_Init(StepperManager_t* const me);
int StepperManager_StartISR(StepperManager_t* const me);
int StepperManager_StopISR(StepperManager_t* const me);
void StepperManager_ISRCallback(void *p);
static THD_FUNCTION(StepperManager_ThreadFunction, arg);

void StepperManager_SetStepper(StepperManager_t* const me, StepperProxy_t* step,
                              int i);
void StepperManager_SetEndstop(StepperManager_t* const me, EndstopProxy_t* end,
                               int i);
void StepperManager_SetItsMovementQueue(StepperManager_t* const me,
                                        MovementQueue_t* queue);
MovementQueue_t* StepperManager_GetItsMovementQueue(StepperManager_t* const me);

StepperManager_t* StepperManager_Create(memory_heap_t* heap);
void StepperManager_Destroy(StepperManager_t* const me);
#endif /* MARENGO_STEPPERMANAGER_H_ */
