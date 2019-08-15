/*
 * StepperMove.c
 *
 *  Created on: 1 lip 2019
 *      Author: grzeg
 */

#include "StepperMove.h"
#include "StepperProxy.h"

int StepperMove_max(int argc, ...)
{
  int max,a;
  va_list ptr;
  va_start(ptr, argc);
  max = va_arg(ptr,int);
  for(int i=1; i< argc; i++)
  {
    a = va_arg(ptr,int);
    max = a>max ? a : max;
  }
  va_end(ptr);
  return max;
}

void StepperMove_Init(StepperMove_t* const me)
{
  for(int i=0;i<4;i++)
  {
    me->d[i]=0;
    me->direction[i]=0;
    me->steps[i]=0;
  }
  me->feedrate=0;
  me->step=0;
  me->dm = 0;
}

void StepperMove_Set(StepperMove_t* const me, float_t x, float_t y, float_t z,
                     float_t e, int f)
{
  me->coord[0]=x;
  me->coord[1]=y;
  me->coord[2]=z;
  me->coord[3]=e;
  me->feedrate = f;
}

void StepperMove_Prepare(StepperMove_t* const me, StepperProxy_t* steppers[4])
{
  // Enable needed stepper motors
  for(int i=0; i<4; i++)
  {
    if(fnonzero(me->coord[i])) StepperProxy_Enable(steppers[i]);
    me->direction[i] = fneg(me->coord[i]) ? -1 : 1;
    StepperProxy_SetDirection(steppers[i], me->direction[i]);
    me->d[i] = abs(StepperProxy_MM2Stps(steppers[i], me->coord[i]));
    me->steps[i] = me->d[i]/2;
  }
  me->dm = StepperMove_max(4, me->d[0], me->d[1], me->d[2], me->d[3]);
  me->step = me->dm;
}

int StepperMove_Step(StepperMove_t* const me, StepperProxy_t* steppers[4])
{
  if(--me->step<=0)
    return 1;
  for(int i=0;i<4;i++)
  {
    me->steps[i]-=me->d[i];
    if(me->steps[i]<0){
      me->steps[i]+=me->dm;
      StepperProxy_Step(steppers[i]);
      // Modify stepper position
    }
  }
  return 0;
}

int StepperMove_GetMovementLenghtInSteps(StepperMove_t* const me)
{
  int x=me->steps[0];
  int y=me->steps[1];
  int z=me->steps[2];
  if(x>0xFFFF || y>0xFFFF || z>0xFFFF){
    x/=0x100;
    y/=0x100;
    z/=0x100;
    return 0x100*sqrt(x*x+y*y+z*z);
  }
  return sqrt(x*x+y*y+z*z);
}

char StepperMove_IsFinished(StepperMove_t* const me)
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
