#include "StepperProxy.h"
#include "pin_mapping.h"

void StepperProxy_Init(StepperProxy_t* const me){
  me->Name = STEPPER_AXIS_NULL;
  me->StepsPerRev = 0;
  me->ThreadJumpUM = 0;
  me->Microsteps = 0;
  me->GearRatio = 0;
  me->StepsPerMM = 0;
  me->maxFeedrate = 0;
  me->position = 0;
  me->Direction = NO_DIRECTION;
  me->bLinear = FALSE;
  me->bNeedStall = FALSE;
  me->lineStp = (ioline_t)NULL;
  me->lineDir = (ioline_t)NULL;
  me->lineEn = (ioline_t)NULL;
  me->errorCode = STEPPER_NOT_CONFIGURED;
}

void StepperProxy_Configure(StepperProxy_t* const me, StepperAxisType_t Name,
                            int16_t StepsPerRev, int16_t Microsteps,
                            int16_t ThreadJumpUM, int16_t GearRatio,
                            int16_t maxFeedrate,
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
  palSetLineMode(lineStp, PAL_MODE_OUTPUT_PUSHPULL);
  me->lineStp = lineStp;
  palSetLineMode(lineDir, PAL_MODE_OUTPUT_PUSHPULL);
  me->lineDir = lineDir;
  palSetLineMode(lineEn, PAL_MODE_OUTPUT_PUSHPULL);
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
  palToggleLine(LINE_REDLED);
  if(!me->lineEn)
    me->errorCode = STEPPER_NO_PARAMS_LINES;
  else
  {
    palClearLine(me->lineEn);
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
  {
    palToggleLine(me->lineStp);
    me->position+=me->Direction;
  }
}

bool_t StepperProxy_GetNeedStall(StepperProxy_t* const me)
{
  return me->bNeedStall;
}

uint32_t StepperProxy_GetMaxFeedrate(StepperProxy_t* const me){
  return me->maxFeedrate;
}

float_t StepperProxy_Stps2MM(StepperProxy_t* const me, uint32_t steps){
  return idiv(steps, me->StepsPerMM, FLOAT_PRECISION);
}

uint32_t StepperProxy_MM2Stps(StepperProxy_t* const me, float_t mm){
  return ffloor(fmulti(mm, me->StepsPerMM));
}

StepperErrorCode_t StepperProxy_GetStatus(StepperProxy_t* const me){
  return me->errorCode;
}

uint32_t StepperProxy_GetPositionInSteps(StepperProxy_t* const me)
{
  return me->position;
}
void StepperProxy_SetPositionInSteps(StepperProxy_t* const me, uint32_t position)
{
  me->position = position;
}

float_t StepperProxy_GetPosition(StepperProxy_t* const me)
{
  return idiv(me->position, me->StepsPerMM);
}

void StepperProxy_SetPosition(StepperProxy_t* const me, float_t position)
{
  me->position = StepperProxy_MM2Stps(me, position);
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
