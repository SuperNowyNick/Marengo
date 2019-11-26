/*
 * StepperManager.c
 *
 *  Created on: 1 lip 2019
 *      Author: grzeg
 */
#include "StepperManager.h"

void StepperManager_Init(StepperManager_t* const me)
{
  me->delay=0;
  StepperMove_Init(&me->move);
  for(int i=0; i<STEPPER_MANAGER_STEPPER_NUM;i++)
  {
    me->Endstop[i]=NULL;
    me->Stepper[i]=NULL;
  }
  me->queue = NULL;
  chVTObjectInit(&me->vt);
  StepperMove_Init(&me->move);
  me->tp = chThdCreateStatic(StepperManagerThreadWorkingArea,
                         sizeof(StepperManagerThreadWorkingArea), NORMALPRIO,
                         StepperManager_ThreadFunction, (void*)me);
}

void StepperManager_SetStepper(StepperManager_t* const me, StepperProxy_t* step,
                              int i)
{
  if(i>=0 && i<STEPPER_MANAGER_STEPPER_NUM)
    me->Stepper[i]=step;
}

void StepperManager_SetEndstop(StepperManager_t* const me, EndstopProxy_t* end,
                               int i)
{
  if(i>=0&& i<4)
    me->Endstop[i]=end;
}

void StepperManager_SetItsMovementQueue(StepperManager_t* const me,
                                        MovementQueue_t* queue)
{
  me->queue = queue;
}

void StepperManager_DisableAllSteppers(StepperManager_t* const me)
{
  for(int i=0; i<STEPPER_MANAGER_STEPPER_NUM; i++)
    StepperProxy_Disable(me->Stepper[i]);
}
void StepperManager_EnableAllStepper(StepperManager_t* const me)
{
  for(int i=0; i<STEPPER_MANAGER_STEPPER_NUM; i++)
    StepperProxy_Enable(me->Stepper[i]);
}

// TODO: Rename this function to something more clear
// Return the axis with maximum number of steps to be made in current move
StepperProxy_t* StepperManager_MoveToAxisProjection(StepperManager_t* const me)
{
  int max = 0;
  for(int i=1; i<STEPPER_MANAGER_STEPPER_NUM;i++)
    max = fpos(fsub(fabs(me->move.coord[max]),
                    fabs(me->move.coord[i]))) ? max : i;
  return me->Stepper[max];
}
int StepperManager_CalculateRampDelay_Linear(StepperManager_t* const me)
{
  // Check for maximal feedrate possible for every stepper
  // Find the axis with constant stepping
  StepperProxy_t* maxax = StepperManager_MoveToAxisProjection(me);
  uint32_t minfeed = StepperProxy_MM2Stps(maxax, itof(1))*STEPPER_MANAGER_MINFEED/60;
  if(me->move.step == me->move.dm){
    me->delay = STEPPER_MANAGER_CLOCK_FREQ/minfeed;
  }
  else
  {
    int32_t accel = StepperProxy_MM2Stps(maxax, itof(1))*STEPPER_MANAGER_STPACCEL;
    int64_t maxfeed = StepperProxy_MM2Stps(maxax, itof(1))*me->move.feedrate;
    if(maxax->Name!=STEPPER_AXIS_E)
    {
      maxfeed*=me->move.dm;
      maxfeed/=StepperMove_GetMovementLenghtInSteps(&me->move);
    }
    maxfeed/=60;
    int64_t ramplen = (maxfeed*maxfeed-minfeed*minfeed);
    ramplen/=2;
    ramplen/=accel;
    if(ramplen <= 0)
    {
      me->delay = STEPPER_MANAGER_CLOCK_FREQ/minfeed;
    }
    else
    {
      if(me->move.dm < 2*ramplen) // If movement takes less steps than two ramps
      {
        ramplen = me->move.dm/2;
      }
      if(me->move.dm-me->move.step < ramplen) // Is it the acceleration ramp?
      {
        me->delay = STEPPER_MANAGER_CLOCK_FREQ/isqrt32(2*accel*(me->move.dm-\
            me->move.step)+minfeed*minfeed);
      }
      else if(me->move.step<ramplen) // Is it the deceleration ramp
      {
        me->delay = STEPPER_MANAGER_CLOCK_FREQ/isqrt32(2*accel*(me->move.step)+minfeed*minfeed);
      }
      else
      {
        me->delay = STEPPER_MANAGER_CLOCK_FREQ/maxfeed;
      }
    }
  }
  if(me->delay<=0)
    while(1){}
  return me->delay;
}

int StepperManager_StartISR(StepperManager_t* const me)
{
  if(me->delay<=0)
    me->delay=1;
  chVTSet(&me->vt, me->delay, StepperManager_ISRCallback, me);
}
int StepperManager_StopISR(StepperManager_t* const me)
{
  chVTReset(&me->vt);
}
void StepperManager_ISRCallback(void *p)
{
  StepperManager_t* me = (StepperManager_t*)p;
  // Check if the current move hasn't finished
  if(!StepperMove_Step(&me->move, me->Stepper))
  {
    // Get the delay for next move
    me->delay= StepperManager_CalculateRampDelay_Linear(me);
    chSysLockFromISR();
    chVTSetI(&me->vt, me->delay, StepperManager_ISRCallback, me);
    chSysUnlockFromISR();
  }
  else
  {
    // If the move was finished
    // TODO: Maybe check for another move?
    StepperManager_DisableAllSteppers(me);
    chSysLockFromISR();
    chVTResetI(&me->vt);
    chSysUnlockFromISR();
  }
}

static THD_FUNCTION(StepperManager_ThreadFunction, arg)
{
  StepperManager_t* me = (StepperManager_t*)arg;
  while(true)
  {
    // TODO: Add thread termination
    // if(chThdShouldTerminate())
    // do stuff to stop stepper motors and so on
    chThdSleepMilliseconds(100);
    if(StepperMove_IsFinished(&me->move))
    {
      StepperMove_t* moveptr = MovementQueue_Pull(me->queue, &me->move);
      while(moveptr==NULL)
      {
        chThdSleepMilliseconds(100);
        moveptr = MovementQueue_Pull(me->queue, &me->move);
      }

      StepperMove_Prepare(&me->move, me->Stepper);
      StepperManager_StartISR(me);
    }
  }
}

StepperManager_t* StepperManager_Create(memory_heap_t* heap)
{
  StepperManager_t* me = (StepperManager_t*)chHeapAlloc(heap,
                                                        sizeof(StepperManager_t));
  if(me!=NULL)
  {
    StepperManager_Init(me);
  }
  return me;
}
void StepperManager_Destroy(StepperManager_t* const me)
{
  chThdTerminate(me->tp);
  chThdRelease(me->tp);
  chHeapFree(me);
}
