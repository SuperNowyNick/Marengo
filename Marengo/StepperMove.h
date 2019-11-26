/*
 * StepperMove.h
 *
 *  Created on: 1 lip 2019
 *      Author: grzeg
 */
#ifndef MARENGO_STEPPERMOVE_H_
#define MARENGO_STEPPERMOVE_H_

#include "ch.h"
#include "StepperProxy.h"

typedef struct {
  float_t coord[4];
  int32_t steps[4];
  int8_t direction[4];
  int32_t d[4];
  int16_t feedrate;
  int32_t step;
  int32_t dm;
} StepperMove_t;

void StepperMove_Init(StepperMove_t* const me);

void StepperMove_Set(StepperMove_t* const me, float_t x, float_t y, float_t z,
                     float_t e, int16_t f); // TODO: Add function documentation
void StepperMove_Prepare(StepperMove_t* const me, StepperProxy_t* steppers[4]);
uint16_t StepperMove_Step(StepperMove_t* const me, StepperProxy_t* steppers[4]);
uint32_t StepperMove_GetMovementLenghtInSteps(StepperMove_t* const me);

bool_t StepperMove_IsFinished(StepperMove_t* const me);

StepperMove_t* StepperMove_Create(memory_heap_t* heap);
void StepperMove_Destroy(StepperMove_t* const me);

#endif /* MARENGO_STEPPERMOVE_H_ */
