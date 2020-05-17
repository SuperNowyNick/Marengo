/*
 * StepperMove.c
 *
 *  Created on: 1 lip 2019
 *      Author: grzeg
 */

#include "StepperMove.h"
#include "StepperProxy.h"

static uint32_t StepperMove_max(uint16_t argc, uint32_t* table)
{
  int max,a;
  max = table[0];
  for(int i=1; i< argc; i++)
  {
    a = table[i];
    max = a>max ? a : max;
  }
  return max;
}

void StepperMove_Init(StepperMove_t* const me)
{
  for(int i=0;i<4;i++)
  {
    me->d[i]=0;
    me->direction[i]=0;
    me->steps[i]=0;
    me->coord[i] = fzero();
  }
  me->feedrate=0;
  me->step=0;
  me->dm = 0;
  me->length = 0;
}

void StepperMove_Set(StepperMove_t* const me, float_t x, float_t y, float_t z,
                     float_t e, int16_t f)
{
  // TODO: Add checking for feedrate overrun
  me->coord[0]=x;
  me->coord[1]=y;
  me->coord[2]=z;
  me->coord[3]=e;
  me->feedrate = f;
}

void StepperMove_Prepare(StepperMove_t* const me, StepperProxy_t* steppers[4])
{

  for(int i=0; i<4; i++)
  {
    me->direction[i] = fneg(me->coord[i]) ? -1 : 1;
    me->d[i] = abs(StepperProxy_MM2Stps(steppers[i], me->coord[i]));
    me->steps[i] = me->d[i]/2;
  }
  me->dm = StepperMove_max(4, me->d);
  me->step = me->dm;
  me->length = StepperMove_GetMovementLenghtInSteps(me);
}
int32_t StepperMove_GetFeedrateToAxisProjection(StepperMove_t* const me,
                                                StepperAxisType_t axis)
{
  int32_t val = me->feedrate*me->d[axis];
  val /= me->length;
  return val;
}

uint16_t StepperMove_Step(StepperMove_t* const me, StepperProxy_t* steppers[4])
{
  if(--me->step<=0)
    return 1;
  for(int i=0;i<4;i++)
  {
    me->steps[i]-=me->d[i];
    if(me->steps[i]<0){
      me->steps[i]+=me->dm;
      StepperProxy_Step(steppers[i]);
    }
  }
  return 0;
}

uint32_t StepperMove_GetMovementLenghtInSteps(StepperMove_t* const me)
{
  int32_t x=me->d[0];
  int32_t y=me->d[1];
  int32_t z=me->d[2];
  uint64_t temp = x*x+y*y+z*z;
  uint32_t ret = isqrt64(temp);
  me->length = ret;
  return ret;
}

bool_t StepperMove_IsFinished(StepperMove_t* const me)
{
  return me->step<=0;
}

StepperMove_t* StepperMove_Create(memory_heap_t* heap)
{
  StepperMove_t* me = (StepperMove_t*)chHeapAlloc(heap, sizeof(StepperMove_t));
  if(NULL != me)
    StepperMove_Init(me);
  return me;
}
void StepperMove_Destroy(StepperMove_t* const me)
{
  chHeapFree(me);
}
