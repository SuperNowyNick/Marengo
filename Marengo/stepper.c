/*
 * stepper.c
 *
 *  Created on: 8 lip 2018
 *      Author: grzeg
 */

#include "stepper.h"
#include "console.h" // Temporarily for debug reasons
#include <stdarg.h>

// TODO: Refactor this code!!!

/* Initialize stepper motor driver.*/

void stpInit(void)
{
  consPrintf("Initializing steppers"CONSOLE_NEWLINE_STR);
  for (int i = 0; i < STP_AXES_NUM; i++) {
    consPrintf("Axis %c no: %d line: %d "CONSOLE_NEWLINE_STR, stpAxes[i].designation, i, stpAxes[i].line_stp);
    palSetLineMode(stpAxes[i].line_dir, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(stpAxes[i].line_stp, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(stpAxes[i].line_en, PAL_MODE_OUTPUT_PUSHPULL);
    palClearLine(stpAxes[i].line_stp);
    palClearLine(stpAxes[i].line_dir);
    palSetLine(stpAxes[i].line_en);
  }
  consPrintf("Initializing endstops"CONSOLE_NEWLINE_STR);
  for (int i = 0; i < STP_ENDSTOPS_NUM; i++) {
    consPrintf("Endstop on axis %c no: %d line: %d"CONSOLE_NEWLINE_STR, stpEndstops[i].designation, i, stpEndstops[i].line);
    palSetLineMode(stpEndstops[i].line, PAL_MODE_INPUT_PULLDOWN);
    palSetLineCallback(stpEndstops[i].line, stpEndstopCallback, (void*)&stpEndstops[i]);
    stpEndstops[i].active = palReadLine(stpEndstops[i].line);
  }

  // TODO: Perform homing operation with endstop check
  //set current coords and speed to 0,0,0,0
  stpSetHome();
  stpCurrentSpeed.x=0;
  stpCurrentSpeed.y=0;
  stpCurrentSpeed.z=0;
  stpCurrentSpeed.stpE=0;
  // set mode to absolute
  stpModeInc=1;
  stpFeedrate=100;
  stpAccel=100;
  stpMovementQueue= NULL;
  stpStatus = STP_STATE_WAITING;
}

void stpSetQueue(MovementQueue_t* queue)
{
  stpMovementQueue = queue;
}

void stpSetHome(void)
{
  stpCurrentAbsPos.x=0;
  stpCurrentAbsPos.y=0;
  stpCurrentAbsPos.z=0;
  stpCurrentAbsPos.stpE=0;
}

void stpStop(void)
{
  // TODO: Stop fetching new moves
  // Disable stepper interrupt routine
  chVTReset(&vt);
  stpStatus = STP_STATE_WAITING;
}

stpCoord_t stpGetCurrentPos(void)
{
  return stpCurrentAbsPos;
}

int stpGetFeedrate(void)
{
  return stpFeedrate;
}

int stpSetFeedRate(int feedrate)
{
  if(feedrate<MIN_FEEDRATE)
    feedrate=MIN_FEEDRATE;
  if(feedrate>MAX_FEEDRATE)
    feedrate=MAX_FEEDRATE;
  stpFeedrate = feedrate;
  return 0;
}

// Callback from endstop line, arg is
palcallback_t stpEndstopCallback(void *arg)
{
  stpEndstop_t* endstop = arg;
  endstop->active = palReadLine(endstop->line);
  palTogglePad(GPIOG, GPIOG_LED4_RED);
  consPrintf("Endstop %c at end %d is %d"CONSOLE_NEWLINE_STR, endstop->designation, endstop->side, endstop->active);
}

/* Toggle direction in which the carriage moves on a given axis.*/
int stpDirToggle(char dsgn) {
  int i = 0;
  while (stpAxes[i].designation != dsgn) {
    i++;
  }
  if (i > STP_AXES_NUM)
    return 1;
  palToggleLine(stpAxes[i].line_dir);
  return 0;
}

int max(int argc, ...)
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

stpCoord_t stpCoordAdd(stpCoord_t a, stpCoord_t b)
{
  stpCoord_t ret;
  ret.x=a.x+b.x;
  ret.y=a.y+b.y;
  ret.z=a.z+b.z;
  ret.stpE=a.stpE+b.stpE;
  return ret;
}
stpCoord_t stpCoordSub(stpCoord_t a, stpCoord_t b)
{
  stpCoord_t ret;
  ret.x=a.x-b.x;
  ret.y=a.y-b.y;
  ret.z=a.z-b.z;
  ret.stpE=a.stpE-b.stpE;
  return ret;
}

int stpCoordAbs(stpCoord_t coord)
{
  if(max(3, coord.x, coord.y, coord.z)>0xFFFF){
    coord.x/=0x100;
    coord.y/=0x100;
    coord.z/=0x100;
    return 0x100*sqrt(coord.x*coord.x+coord.y*coord.y+coord.z*coord.z);
  }
  return sqrt(coord.x*coord.x+coord.y*coord.y+coord.z*coord.z);
}

stpCoordF_t stpCoordFZero(void)
{
  stpCoordF_t a;
  a.x = fzero();
  a.y = fzero();
  a.z = fzero();
  a.stpE = fzero();
  return a;
}
stpCoordF_t stpCoordFAdd(stpCoordF_t a, stpCoordF_t b)
{
  stpCoordF_t ret;
  ret.x=fadd(a.x,b.x);
  ret.y=fadd(a.y,b.y);
  ret.z=fadd(a.z,b.z);
  ret.stpE=fadd(a.stpE,b.stpE);
  return ret;
}
stpCoordF_t stpCoordFSub(stpCoordF_t a, stpCoordF_t b)
{
  stpCoordF_t ret;
  ret.x=fsub(a.x,b.x);
  ret.y=fsub(a.y,b.y);
  ret.z=fsub(a.z,b.z);
  ret.stpE=fsub(a.stpE,b.stpE);
  return ret;
}

void stpSetPosition(stpCoordF_t pos)
{
  stpCoordConvMetricF2Steps(&pos, &stpCurrentAbsPos);
}

stpCoord_t* stpCoordConvMetric2Steps(stpCoord_t* coord)
{
  coord->x*=stpAxes[1].steps_per_mm;
  coord->y*=stpAxes[0].steps_per_mm;
  coord->z*=stpAxes[2].steps_per_mm;
  coord->stpE*=stpAxes[3].steps_per_mm;
  return coord;
}

stpCoord_t* stpCoordConvMetricF2Steps(stpCoordF_t* input, stpCoord_t* output)
{
  float_t temp;
  temp = fmulti(input->x, stpAxes[1].steps_per_mm);
  output->x=temp.signum*temp.character;
  temp = fmulti(input->y, stpAxes[0].steps_per_mm);
  output->y=temp.signum*temp.character;
  temp = fmulti(input->z, stpAxes[2].steps_per_mm);
  output->z=temp.signum*temp.character;
  temp = fmulti(input->stpE, stpAxes[3].steps_per_mm);
  output->stpE=temp.signum*temp.character;
  return output;
}

stpCoordF_t stpGetCoordF(void)
{
  stpCoordF_t coord;
  coord.x=idiv(stpCurrentAbsPos.x, stpAxes[1].steps_per_mm, 2);
  coord.y=idiv(stpCurrentAbsPos.y, stpAxes[0].steps_per_mm, 2);
  coord.z=idiv(stpCurrentAbsPos.z, stpAxes[2].steps_per_mm, 2);
  coord.stpE=idiv(stpCurrentAbsPos.stpE, stpAxes[3].steps_per_mm, 2);
  return coord;
}

int stpMoveLinearInit(stpCoordF_t end){
  if(stpMovementQueue!=NULL)
  {
    StepperMove_t step;
    StepperMove_Init(&step);
      StepperMove_Set(&step, end.x, end.y, end.z, end.stpE, stpFeedrate);
      while(MovementQueue_Push(stpMovementQueue, &step)!=-1)
        chThdSleepMilliseconds(1);
      return 0;
  }
  return 1;
}

//int stpMoveLinearInit(stpCoord_t end){
//  if(stpStatus != STP_STATE_WAITING){
//    consPrintf("ERROR! STEPPERMOTOR ALREADY MOVING!"CONSOLE_NEWLINE_STR);
//    return 1;
//  }
//  stpCoord_t mov;
//  //stpCoordConvMetric2Steps(&end);
//  if(!stpModeInc)
//    mov = stpCoordSub(end, stpCurrentAbsPos);
//  else mov=end;
//  // TODO: Fix if end = 0 0 0 0
//  if(!mov.x && !mov.y && !mov.z && !mov.stpE){
//    stpStatus = STP_STATE_WAITING;
//    return 1;
//  }
//  // set stepper drivers
//  if(mov.x!=0) palClearLine(stpAxes[1].line_en);
//  if(mov.y!=0) palClearLine(stpAxes[0].line_en);
//  if(mov.z!=0) palClearLine(stpAxes[2].line_en);
//  if(mov.stpE!=0) palClearLine(stpAxes[3].line_en);
//  // set directions
//  stpCurrentMove.xdir=1;
//  stpCurrentMove.ydir=1;
//  stpCurrentMove.zdir=1;
//  stpCurrentMove.edir=1;
//
//  if(mov.x<0) {
//    palClearLine(stpAxes[1].line_dir);
//    stpCurrentMove.xdir=-1;
//  }
//  else palSetLine(stpAxes[1].line_dir);
//  if(mov.y<0) {
//    palClearLine(stpAxes[0].line_dir);
//    stpCurrentMove.ydir=-1;
//  }
//  else palSetLine(stpAxes[0].line_dir);
//  if(mov.z<0) {
//    palClearLine(stpAxes[2].line_dir);
//    stpCurrentMove.zdir=-1;
//  }
//  else palSetLine(stpAxes[2].line_dir);
//  if(mov.stpE<0) {
//    palClearLine(stpAxes[3].line_dir);
//    stpCurrentMove.edir=-1;
//  }
//  else palSetLine(stpAxes[3].line_dir);
//  stpCurrentMove.dx = abs(mov.x);
//  stpCurrentMove.dy = abs(mov.y);
//  stpCurrentMove.dz = abs(mov.z);
//  stpCurrentMove.de = abs(mov.stpE);
//  // Calc maximal number of steps
//  stpCurrentMove.dm = max(4, stpCurrentMove.dx, stpCurrentMove.dy,
//                          stpCurrentMove.dz, stpCurrentMove.de);
//  stpCurrentMove.stpX=stpCurrentMove.dm/2;
//  stpCurrentMove.stpY=stpCurrentMove.dm/2;
//  stpCurrentMove.stpZ=stpCurrentMove.dm/2;
//  stpCurrentMove.stpE1=stpCurrentMove.dm/2;
//  stpCurrentMove.i = stpCurrentMove.dm;
//  chVTSet(&vt, 1, stpMoveLinear, NULL);
//  stpStatus = STP_STATE_RUNNING;
//  //consPrintf("Moving stepper motors by X%dY%dZ%dE%d steps"CONSOLE_NEWLINE_STR, stpCurrentMove.dx, stpCurrentMove.dy,
//  //  stpCurrentMove.dz, stpCurrentMove.de);
//  return 0;
//}
void stpMoveLinear(void *p)
{
  stpCoord_t totalsteps = (stpCoord_t){stpCurrentMove.dx, stpCurrentMove.dy,
    stpCurrentMove.dz, stpCurrentMove.de};
  int delay = stpAccelRampLinear(stpCurrentMove.i, totalsteps);
  if(--stpCurrentMove.i==0)
{
    palSetLine(stpAxes[0].line_en);
    palSetLine(stpAxes[1].line_en);
    palSetLine(stpAxes[2].line_en);
    palSetLine(stpAxes[3].line_en);
    chSysLockFromISR();
    chVTResetI(&vt);
    chSysUnlockFromISR();
    stpStatus = STP_STATE_WAITING;
}
  else
  {
  chSysLockFromISR();
  chVTSetI(&vt, delay, stpMoveLinear, NULL);
  chSysUnlockFromISR();
  }
    delay = stpAccelRampLinear(stpCurrentMove.i, totalsteps);
    stpCurrentMove.stpX-=stpCurrentMove.dx; if(stpCurrentMove.stpX<0) {
      stpCurrentMove.stpX+=stpCurrentMove.dm;
      palToggleLine(stpAxes[1].line_stp);
      stpCurrentAbsPos.x+=stpCurrentMove.xdir;
    }
    stpCurrentMove.stpY-=stpCurrentMove.dy; if(stpCurrentMove.stpY<0) {
      stpCurrentMove.stpY+=stpCurrentMove.dm;
      palToggleLine(stpAxes[0].line_stp);
      stpCurrentAbsPos.y+=stpCurrentMove.ydir;
    }
    stpCurrentMove.stpZ-=stpCurrentMove.dz; if(stpCurrentMove.stpZ<0) {
      stpCurrentMove.stpZ+=stpCurrentMove.dm;
      palToggleLine(stpAxes[2].line_stp);
      stpCurrentAbsPos.z+=stpCurrentMove.zdir;
    }
    stpCurrentMove.stpE1-=stpCurrentMove.de; if(stpCurrentMove.stpE1<0) {
      stpCurrentMove.stpE1+=stpCurrentMove.dm;
      palToggleLine(stpAxes[3].line_stp);
      stpCurrentAbsPos.stpE+=stpCurrentMove.edir;
    }
    // TODO: check for endstop hit!
}


int stpAccelRampLinear(int nremstep, stpCoord_t totalsteps)
{
  // TODO: Add entry speed and exit speed
  // find axis with maximum steps
  stpAxis_t* maxax;
  int maxsteps = max(4, totalsteps.x, totalsteps.y, totalsteps.z, totalsteps.stpE);
  if(totalsteps.x == maxsteps)
    maxax = &stpAxes[1];
  if(totalsteps.y == maxsteps)
    maxax = &stpAxes[0];
  if(totalsteps.z == maxsteps)
    maxax = &stpAxes[2];
  if(totalsteps.stpE == maxsteps)
    maxax = &stpAxes[3];
  // If it's the first step
  int minfeed = maxax->steps_per_mm*STEPPER_START_SPEED/60;
  if(nremstep==maxsteps)
    return CLOCK_FREQ/minfeed;
  // calculate number of ramping steps
  // find speed in the main axis
  int accel = maxax->steps_per_mm*stpAccel;
  unsigned int maxfeed = maxax->steps_per_mm;
  maxfeed*=stpFeedrate;
  if(maxax->designation!='E'){
    maxfeed*=maxsteps/stpCoordAbs(totalsteps);
  }
  maxfeed/=60;
  unsigned int ramplen = (maxfeed*maxfeed-minfeed*minfeed)/2/accel;
  // check if acceleration to desired feedrate possible?
  if(maxsteps<2*ramplen)
    ramplen = maxsteps/2; // change the ramp lenght to halfpoint of movement
  if(maxsteps-nremstep<ramplen)
    return CLOCK_FREQ/sqrt(2*accel*(maxsteps-nremstep)+minfeed*minfeed);
  if(nremstep<ramplen)
    return CLOCK_FREQ/sqrt(2*accel*(nremstep)+minfeed*minfeed);
   return CLOCK_FREQ/maxfeed;
}


