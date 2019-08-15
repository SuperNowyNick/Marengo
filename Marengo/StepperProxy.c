#include "StepperProxy.h"

void StepperProxy_Init(StepperProxy_t* const me){
  me->Name = NULL;
  me->StepsPerRev = 0;
  me->ThreadJumpUM = 0;
  me->Microsteps = 0;
  me->GearRatio = 0;
  me->StepsPerMM = 0;
  me->maxFeedrate = 0;
  me->Direction = NO_DIRECTION;
  me->bLinear = FALSE;
  me->bNeedStall = FALSE;
  me->lineStp = (ioline_t)NULL;
  me->lineDir = (ioline_t)NULL;
  me->lineEn = (ioline_t)NULL;
  me->errorCode = STEPPER_NOT_CONFIGURED;
}

void StepperProxy_Configure(StepperProxy_t* const me, char* Name,
                            unsigned int StepsPerRev, unsigned int Microsteps,
                            unsigned int ThreadJumpUM, unsigned int GearRatio,
                            unsigned int maxFeedrate,
                            StepperDirection_t Direction, bool_t bLinear,
                            bool_t bNeedStall, ioline_t lineStp,
                            ioline_t lineDir, ioline_t lineEn)
{
  if(!Name)
    me->errorCode = STEPPER_NO_NAME;
  me->Name = Name;
  if(!StepsPerRev || !ThreadJumpUM || !GearRatio)
    me->errorCode = STEPPER_NO_PARAMS_DIMENSIONS;
  me->StepsPerRev = StepsPerRev;
  if(!Microsteps)
    Microsteps=1;
  me->Microsteps = Microsteps;
  me->ThreadJumpUM = ThreadJumpUM;
  me->GearRatio = GearRatio;
  me->maxFeedrate = maxFeedrate;
  if(!Direction)
    Direction = FORWARD;
  me->Direction = Direction;
  me->bLinear = bLinear;
  if(bLinear)
    me->StepsPerMM=StepsPerRev*Microsteps*GearRatio*1000/ThreadJumpUM;
  else
    me->StepsPerMM=StepsPerRev*Microsteps*GearRatio*1000/ThreadJumpUM*100/314;
  me->bNeedStall = bNeedStall;
  if(!lineStp || !lineDir || !lineEn)
    me->errorCode = STEPPER_NO_PARAMS_LINES;
  me->lineStp = lineStp;
  me->lineDir = lineDir;
  me->lineEn = lineEn;
  StepperProxy_Disable(me);
  StepperProxy_SetDirection(me, Direction); // TODO: Think if it is needed here
  me->errorCode = STEPPER_NOT_HOMED;
}

void StepperProxy_SetDirection(StepperProxy_t* const me,
                               StepperDirection_t Direction)
{
  if(!me->lineDir)
    me->errorCode = STEPPER_NO_PARAMS_LINES;
  else
  {
    me->Direction = Direction;
    if(Direction == FORWARD)
      palSetLine(me->lineDir);
    if(Direction == BACKWARD)
      palClearLine(me->lineDir);
  }
}

StepperDirection_t StepperProxy_GetDirection(StepperProxy_t* const me){
  return me->Direction;
}

void StepperProxy_ToggleDirection(StepperProxy_t* const me){
  if(me->Direction != NO_DIRECTION) // CANNOT TOGGLE NO DIRECTION!
  {
    me->Direction = -me->Direction;
    palToggleLine(me->lineDir);
  }
}

void StepperProxy_Enable(StepperProxy_t* const me)
{
  if(!me->lineEn)
    me->errorCode = STEPPER_NO_PARAMS_LINES;
  else
  {
    palClearLine(me->lineEn);
    //palClearLine(me->lineEn);
  }
}

void StepperProxy_Disable(StepperProxy_t* const me)
{
  if(!me->lineEn)
    me->errorCode = STEPPER_NO_PARAMS_LINES;
  else
    palSetLine(me->lineEn);
}

void StepperProxy_Step(StepperProxy_t* const me)
{
  if(!me->lineStp)
    me->errorCode = STEPPER_NO_PARAMS_LINES;
  else
    palToggleLine(me->lineStp);
}

bool_t StepperProxy_GetNeedStall(StepperProxy_t* const me)
{
  return me->bNeedStall;
}

unsigned int StepperProxy_GetMaxFeedrate(StepperProxy_t* const me){
  return me->maxFeedrate;
}

float_t StepperProxy_Stps2MM(StepperProxy_t* const me, int steps){
  return idiv(steps, me->StepsPerMM, 3);
}

unsigned int StepperProxy_MM2Stps(StepperProxy_t* const me, float_t mm){
  return ffloor(fmulti(mm, me->StepsPerMM));
}

StepperErrorCode_t StepperProxy_GetStatus(StepperProxy_t* const me){
  return me->errorCode;
}

StepperProxy_t* Stepper_ProxyCreate(memory_heap_t* heap){
  StepperProxy_t* me = (StepperProxy_t*)chHeapAlloc(heap , \
                                                    sizeof(StepperProxy_t));
  if(me!=NULL)
  {
    StepperProxy_Init(me);
  }
  return me;
}

void StepperProxy_Destroy(StepperProxy_t* const me){
  chHeapFree(me);
}
