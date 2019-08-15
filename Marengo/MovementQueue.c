/*
 * MovementQueue.c
 *
 *  Created on: 24 lip 2019
 *      Author: grzeg
 */

#include "MovementQueue.h"

void MovementQueue_Init(MovementQueue_t* const me)
{
  me->head=0;
  me->tail=0;
  me->isEmpty=TRUE;
  me->isFull=FALSE;
  for(int i=0; i< MOVEMENT_QUEUE_SIZE; i++)
    me->moves[i]=0;
  chMtxObjectInit(&me->mutex);
}

StepperMove_t* MovementQueue_Pull(MovementQueue_t* const me)
{
  StepperMove_t* step;
  if(me->isEmpty || me==NULL)
    step = NULL;
  else
  {
  step = me->moves[me->tail];
  me->tail = (me->tail+1)%MOVEMENT_QUEUE_SIZE;
  me->isFull = FALSE;
  me->isEmpty = (me->tail == me->head);
  }
  return step;
}
int MovementQueue_Push(MovementQueue_t* me, StepperMove_t* move)
{
  int succes;
  chMtxLock(&me->mutex);
  me->isEmpty=FALSE;
  if(me->isFull)
  {
    succes=FALSE;
  }
  else
  {
  me->moves[me->head]=move;
  me->head = (me->head+1)%MOVEMENT_QUEUE_SIZE;
  me->isFull = (me->head == me->tail);
  succes=TRUE;
  }
  chMtxUnlock(&me->mutex);
  return succes;
}
bool_t MovementQueue_isFull(MovementQueue_t* const me)
{
  return me->isFull;
}
bool_t MovementQueue_isEmpty(MovementQueue_t* const me)
{
  return me->isEmpty;
}

MovementQueue_t* MovementQueue_Create(memory_heap_t* heap)
{
  MovementQueue_t* me = (MovementQueue_t*)chHeapAlloc(heap,
                                                        sizeof(MovementQueue_t));
  if(me!=NULL)
  {
    MovementQueue_Init(me);
  }
  return me;
}
void MovementQueue_Destroy(MovementQueue_t* const me)
{
  chHeapFree(me);
}
