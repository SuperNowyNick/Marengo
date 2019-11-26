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
  StepperAxisType_t Name;
  uint16_t StepsPerRev;
  uint16_t ThreadJumpUM; // if not linear then this is diameter
  uint16_t Microsteps;
  uint16_t GearRatio;
  uint16_t StepsPerMM;
  int16_t maxFeedrate; // in mm/min
  uint32_t position; // in steps
  StepperDirection_t Direction;
  bool_t bLinear; // is any gear attached to stepper?
  bool_t bNeedStall; // turns off engines if not used (if not using ball screws)
  ioline_t lineStp; // step IO line
  ioline_t lineDir; // direction IO line
  ioline_t lineEn; // enable IO line
  StepperErrorCode_t errorCode;
} StepperProxy_t;

void StepperProxy_Init(StepperProxy_t* const me);
void StepperProxy_Configure(StepperProxy_t* const me, StepperAxisType_t Name,
                            uint16_t StepsPerRev, uint16_t Microsteps,
                            uint16_t ThreadJumpUM, uint16_t GearRatio,
                            uint16_t maxFeedrate,
                            StepperDirection_t Direction, bool_t bLinear,
                            bool_t bNeedStall, ioline_t lineStp,
                            ioline_t lineDir, ioline_t lineEn);

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
StepperErrorCode_t StepperProxy_GetStatus(StepperProxy_t* const me);
uint32_t StepperProxy_GetPosition(StepperProxy_t* const me);
void StepperProxy_SetPosition(StepperProxy_t* const me, uint32_t position);

StepperProxy_t* Stepper_ProxyCreate(memory_heap_t* heap);
void StepperProxy_Destroy(StepperProxy_t* const me);
#endif /* MARENGO_STEPPERPROXY_H_ */
