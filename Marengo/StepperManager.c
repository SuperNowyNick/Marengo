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
  me->b_NextMoveReady = FALSE;
  me->b_InMove = FALSE;
  StepperMove_Init(&me->move);
  for(int i=0; i<STEPPER_NUM;i++)
  {
    me->Endstop[i]=NULL;
    me->Stepper[i]=NULL;
  }
  me->queue = NULL;
  chVTObjectInit(&me->vt);
  StepperMove_Init(&me->move);
  StepperMove_Init(&me->buffered_move);
  me->tp = chThdCreateStatic(StepperManagerThreadWorkingArea,
                         sizeof(StepperManagerThreadWorkingArea), NORMALPRIO,
                         StepperManager_ThreadFunction, (void*)me);
}

void StepperManager_SetStepper(StepperManager_t* const me, StepperProxy_t* step,
                              int i)
{
  if(i>=0 && i<STEPPER_NUM)
    me->Stepper[i]=step;
}


// Sets stepper direction for given movement
void StepperManager_SetSteppersDirections(StepperManager_t* const me, StepperMove_t* move)
{
  for(int i=0; i<STEPPER_NUM; i++)
  {
    if(move->d[i]>0) StepperProxy_Enable(me->Stepper[i]);
    else StepperProxy_Disable(me->Stepper[i]);
    StepperProxy_SetDirection(me->Stepper[i], move->direction[i]);
  }
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

// Returns coords in
stpCoordF_t StepperManager_GetPosition(StepperManager_t* const me)
{
  stpCoordF_t temp;
  temp.x = StepperProxy_GetPositionInMM(me->Stepper[0]);
  temp.y = StepperProxy_GetPositionInMM(me->Stepper[1]);
  temp.z = StepperProxy_GetPositionInMM(me->Stepper[2]);
  temp.e = StepperProxy_GetPositionInMM(me->Stepper[3]);
  return temp;
}

stpCoord_t StepperManager_GetNextPositionInStps(StepperManager_t* const me)
{
  if(me->b_InMove==FALSE)
    return StepperManager_ConvertPositionToStps(me,
      StepperManager_GetPosition(me));
  return StepperManager_ConvertPositionToStps(me,
      StepperMove_GetDestination(&me->move));
}

stpCoord_t StepperManager_ConvertPositionToStps(StepperManager_t* const me,
                                                stpCoordF_t coord)
{
  stpCoord_t temp;
  temp.x = StepperProxy_MM2Stps(me->Stepper[0], coord.x);
  temp.y = StepperProxy_MM2Stps(me->Stepper[1], coord.y);
  temp.z = StepperProxy_MM2Stps(me->Stepper[2], coord.z);
  temp.e = StepperProxy_MM2Stps(me->Stepper[3], coord.e);
  return temp;
}

uint32_t StepperManager_GetCurrentFeedrate(StepperManager_t* const me)
{
  // TODO: Should we store the previous move feedrate \
  and return it if there is no movement?
  if(me->b_InMove)
    return StepperMove_GetFeedrate(&me->move);
  else
    return 0;
}

void StepperManager_SetPosition(StepperManager_t* const me,
                                       stpCoordF_t position)
{
  StepperProxy_SetPositionInMM(me->Stepper[0], position.x);
  StepperProxy_SetPositionInMM(me->Stepper[1], position.y);
  StepperProxy_SetPositionInMM(me->Stepper[2], position.z);
  StepperProxy_SetPositionInMM(me->Stepper[3], position.e);
  // Set it also as a last position
}

bool_t StepperManager_IsMoving(StepperManager_t* const me)
{
  return me->b_InMove;
}

void StepperManager_DisableAllSteppers(StepperManager_t* const me)
{
  for(int i=0; i<STEPPER_NUM; i++)
    StepperProxy_Disable(me->Stepper[i]);
}
void StepperManager_EnableAllStepper(StepperManager_t* const me)
{
  for(int i=0; i<STEPPER_NUM; i++)
    StepperProxy_Enable(me->Stepper[i]);
}

// Return the axis with maximum number of steps to be made in current move
StepperProxy_t* StepperManager_GetMainMovementAxis(StepperManager_t* const me)
{
  int max = 0;
  for(int i=1; i<STEPPER_NUM;i++)
    max = me->move.d[max]>me->move.d[i] ? max : i;
  return me->Stepper[max];
}
void StepperManager_CheckMovementFeedrate(StepperManager_t* const me,
                                          StepperMove_t* const move)
{
  for(int i=0; i<STEPPER_NUM; i++)
  {
    int32_t feedproject = StepperMove_GetFeedrateToAxisProjection(move,i);
    if(feedproject > me->Stepper[i]->maxFeedrate)
    {
      // Reduce movement feedrate
      move->feedrate *= me->Stepper[i]->maxFeedrate;
      move->feedrate /= feedproject;
    }
  }
}
int StepperManager_CalculateRampDelay_Linear(StepperManager_t* const me)
{
  // Check for maximal feedrate possible for every stepper
  // Find the axis with constant stepping
  StepperProxy_t* maxax = StepperManager_GetMainMovementAxis(me);
  uint32_t minfeed = StepperProxy_MM2Stps(maxax, itof(1))*STEPPER_MANAGER_MINFEED/60;
  if(me->move.step == me->move.dm){
    me->delay = STEPPER_MANAGER_CLOCK_FREQ/minfeed;
  }
  else
  {
    int32_t accel = StepperProxy_MM2Stps(maxax, itof(1))*STEPPER_MANAGER_STPACCEL;
    int64_t maxfeed = StepperProxy_MM2Stps(maxax, itof(1))*me->move.feedrate;
    if(maxax->Axis!=STEPPER_AXIS_E)
    {
      maxfeed*=me->move.dm;
      maxfeed/=me->move.length;
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

void StepperManager_StartISR(StepperManager_t* const me)
{
  if(me->delay<=0)
    me->delay=1;
  me->b_InMove = TRUE;
  chVTSet(&me->vt, me->delay, StepperManager_ISRCallback, me);
}
void StepperManager_StopISR(StepperManager_t* const me)
{
  me->b_InMove = FALSE;
  chVTReset(&me->vt);
}
void StepperManager_ISRCallback(void *p)
{
  chSysLockFromISR();
  StepperManager_t* me = (StepperManager_t*)p;
  // Check if the current move hasn't finished
  if(!StepperMove_Step(&me->move, me->Stepper))
  {
    // Get the delay for next move
    me->delay= StepperManager_CalculateRampDelay_Linear(me);
    chVTSetI(&me->vt, me->delay, StepperManager_ISRCallback, me);
  }
  else // If the move was finished
  {
    if(me->b_NextMoveReady)
    {
      me->move = me->buffered_move;
      me->b_NextMoveReady = FALSE;
      me->delay=1;
      StepperManager_SetSteppersDirections(me, &me->move);
      chVTSetI(&me->vt, me->delay, StepperManager_ISRCallback, me);
    }
    else
    {
    me->b_InMove = FALSE;
    StepperManager_DisableAllSteppers(me);
    chVTResetI(&me->vt);
    }
  }
  chSysUnlockFromISR();
}

THD_FUNCTION(StepperManager_ThreadFunction, arg)
{
  StepperManager_t* me = (StepperManager_t*)arg;
  while(true)
  {
    StepperMove_t* moveptr;
    stpCoord_t start, dest, mov;
    // TODO: Add thread termination
    // if(chThdShouldTerminate())
    // do stuff to stop stepper motors and so on
    chThdSleepMilliseconds(100);
    if(StepperMove_IsFinished(&me->move))
    {
      moveptr = MovementQueue_Pull(me->queue, &me->move);
      while(moveptr==NULL)
      {
        chThdSleepMilliseconds(100);
        moveptr = MovementQueue_Pull(me->queue, &me->move);
      }
      start = StepperManager_GetNextPositionInStps(me);
      dest = StepperManager_ConvertPositionToStps(me,
                                      StepperMove_GetDestination(&me->move));
      mov = stpCoord_Sub(dest,start);
      StepperMove_Prepare(&me->move, mov);
      StepperManager_CheckMovementFeedrate(me, &me->move);
      StepperManager_SetSteppersDirections(me, &me->move);
      StepperManager_StartISR(me);
    }
    if(!me->b_NextMoveReady)
    {
      moveptr = MovementQueue_Pull(me->queue, &me->buffered_move);
      if(moveptr!=NULL)
      {
        start = StepperManager_GetNextPositionInStps(me);
        dest = StepperManager_ConvertPositionToStps(me,
                               StepperMove_GetDestination(&me->buffered_move));
        mov = stpCoord_Sub(dest,start);
        StepperMove_Prepare(&me->buffered_move, mov);
        StepperManager_CheckMovementFeedrate(me, &me->buffered_move);
        me->b_NextMoveReady = TRUE;
      }
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
