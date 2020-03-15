#include "StepperProxy.h"
#include "pin_mapping.h"

void StepperProxy_Init(StepperProxy_t* const me){
  me->Axis = STEPPER_AXIS_NULL;
  me->StepsPerRev = 0;
  me->ThreadJumpUM = 0;
  me->Microsteps = 0;
  me->GearRatio = 0;
  me->StepsPerMM = 0;
  me->maxFeedrate = 0;
  me->position = 0;
  me->Direction = DIR_NONE;
  me->bLinear = FALSE;
  me->bNeedStall = FALSE;
  me->lineStp = (ioline_t)NULL;
  me->lineDir = (ioline_t)NULL;
  me->lineEn = (ioline_t)NULL;
  me->errorCode = STEPPER_NOT_CONFIGURED;
}

// Configures StepperProxy
// Set ports
// Calculate internal helper values (stepsPerMM)
// Returns 1 if problems found
int StepperProxy_Configure(StepperProxy_t* const me)
{

  if(!me->Name)
    return 1;
  if(!me->StepsPerRev || !me->ThreadJumpUM || !me->GearRatio)
    return 1;
  if(!me->Microsteps)
    me->Microsteps=1;
  if(me->Direction==DIR_NONE)
    me->Direction = DIR_FORWARD;
  if(me->bLinear)
    me->StepsPerMM=me->StepsPerRev*me->Microsteps*me->GearRatio*1000/
      me->ThreadJumpUM;
  else
    me->StepsPerMM=me->StepsPerRev*me->Microsteps*me->GearRatio*1000/
      me->ThreadJumpUM*100/314;
  if(!me>lineStp || !me->lineDir || !me->lineEn)
    return 1;
  palSetLineMode(lineStp, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(lineDir, PAL_MODE_OUTPUT_PUSHPULL);
  palSetLineMode(lineEn, PAL_MODE_OUTPUT_PUSHPULL);
  StepperProxy_Disable(me);
  StepperProxy_SetDirection(me, me->Direction);
  return 0;
}

void StepperProxy_SetDirection(StepperProxy_t* const me,
                               StepperDirection_t Direction)
{
  if(!me->lineDir)
    me->errorCode = STEPPER_NO_PARAMS_LINES;
  else
  {
    me->Direction = Direction;
    if(Direction == DIR_FORWARD)
      palSetLine(me->lineDir);
    if(Direction == DIR_BACKWARD)
      palClearLine(me->lineDir);
  }
}

StepperDirection_t StepperProxy_GetDirection(StepperProxy_t* const me){
  return me->Direction;
}

void StepperProxy_ToggleDirection(StepperProxy_t* const me){
  if(me->Direction != DIR_NONE) // CANNOT TOGGLE NO DIRECTION!
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

uint32_t StepperProxy_GetPositionInSteps(StepperProxy_t* const me)
{
  return me->position;
}
void StepperProxy_SetPositionInSteps(StepperProxy_t* const me, uint32_t position)
{
  me->position = position;
}

float_t StepperProxy_GetPositionInMM(StepperProxy_t* const me)
{
  return idiv(me->position, me->StepsPerMM);
}

void StepperProxy_SetPositionInMM(StepperProxy_t* const me, float_t position)
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
