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

typedef struct {
  char* Name;
  unsigned int StepsPerRev;
  unsigned int ThreadJumpUM; // if not linear then this is diameter
  unsigned int Microsteps;
  unsigned int GearRatio;
  unsigned int StepsPerMM;
  signed int maxFeedrate; // in mm/min
  StepperDirection_t Direction;
  bool_t bLinear; // is any gear attached to stepper?
  bool_t bNeedStall; // turns off engines if not used (if not using ball screws)
  ioline_t lineStp; // step IO line
  ioline_t lineDir; // direction IO line
  ioline_t lineEn; // enable IO line
  StepperErrorCode_t errorCode;
} StepperProxy_t;

void StepperProxy_Init(StepperProxy_t* const me);
void StepperProxy_Configure(StepperProxy_t* const me, char* Name,
                            unsigned int StepsPerRev, unsigned int Microsteps,
                            unsigned int ThreadJumpUM, unsigned int GearRatio,
                            unsigned int maxFeedrate,
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
unsigned int StepperProxy_GetMaxFeedrate(StepperProxy_t* const me);
float_t StepperProxy_Stps2MM(StepperProxy_t* const me, int steps);
unsigned int StepperProxy_MM2Stps(StepperProxy_t* const me, float_t mm);
StepperErrorCode_t StepperProxy_GetStatus(StepperProxy_t* const me);

StepperProxy_t* Stepper_ProxyCreate(memory_heap_t* heap);
void StepperProxy_Destroy(StepperProxy_t* const me);
#endif /* MARENGO_STEPPERPROXY_H_ */
