/*
 * stepperproxy.h
 *
 *  Created on: 27 cze 2019
 *      Author: grzeg
 */

#ifndef MARENGO_STEPPERPROXY_H_
#define MARENGO_STEPPERPROXY_H_

#include "ch.h"
#include "hal.h"
#include "float.h"
#include "EndstopProxy.h"

#define STEPPER_NUM 4 // Up to STEPPER_AXIS_E

typedef enum {
  NO_DIRECTION = 0,
  FORWARD = 1,
  BACKWARD = -1
} StepperDirection_t;

typedef enum {
  STEPPER_OK,
  STEPPER_NOT_INITIALIZED,
  STEPPER_NOT_CONFIGURED,
  STEPPER_NO_NAME,
  STEPPER_NO_PARAMS_DIMENSIONS,
  STEPPER_NO_PARAMS_LINES,
  STEPPER_NOT_HOMED,
} StepperErrorCode_t;

typedef enum {
  STEPPER_AXIS_NULL=0,
  STEPPER_AXIS_X=1,
  STEPPER_AXIS_Y,
  STEPPER_AXIS_Z,
  STEPPER_AXIS_E,
  STEPPER_AXIS_A,
  STEPPER_AXIS_B,
  STEPPER_AXIS_C
} StepperAxisType_t;

typedef struct {
  StepperAxisType_t Axis;
  int16_t StepsPerRev;
  int16_t ThreadJumpUM; // if not linear then this is diameter
  int16_t Microsteps;
  int16_t GearRatio;
  int16_t maxFeedrate; // in mm/min
  int32_t position; // in steps
  StepperDirection_t Direction;
  bool_t bLinear; // is any gear attached to stepper?
  bool_t bNeedStall; // turns off engines if not used (if not using ball screws)
  ioline_t lineStp; // step IO line
  ioline_t lineDir; // direction IO line
  ioline_t lineEn; // enable IO line
  int16_t StepsPerMM;
} StepperProxy_t;

void StepperProxy_Init(StepperProxy_t* const me);
void StepperProxy_Configure(StepperProxy_t* const me);

void StepperProxy_SetDirection(StepperProxy_t* const me,
                               StepperDirection_t Direction);
StepperDirection_t StepperProxy_GetDirection(StepperProxy_t* const me);
void StepperProxy_ToggleDirection(StepperProxy_t* const me);
void StepperProxy_Enable(StepperProxy_t* const me);
void StepperProxy_Disable(StepperProxy_t* const me);
void StepperProxy_Step(StepperProxy_t* const me);

bool_t StepperProxy_GetNeedStall(StepperProxy_t* const me);
uint32_t StepperProxy_GetMaxFeedrate(StepperProxy_t* const me);
float_t StepperProxy_Stps2MM(StepperProxy_t* const me, uint32_t steps);
uint32_t StepperProxy_MM2Stps(StepperProxy_t* const me, float_t mm);
uint32_t StepperProxy_GetPositionInSteps(StepperProxy_t* const me);
void StepperProxy_SetPositionInSteps(StepperProxy_t* const me, uint32_t position);
float_t StepperProxy_GetPositionInMM(StepperProxy_t* const me);
void StepperProxy_SetPositionInMM(StepperProxy_t* const me, float_t position);

StepperProxy_t* Stepper_ProxyCreate(memory_heap_t* heap);
void StepperProxy_Destroy(StepperProxy_t* const me);
#endif /* MARENGO_STEPPERPROXY_H_ */
