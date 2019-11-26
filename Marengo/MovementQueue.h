/*
 * MovementQueue.h
 *
 *  Created on: 24 lip 2019
 *      Author: grzeg
 */

#ifndef MARENGO_MOVEMENTQUEUE_H_
#define MARENGO_MOVEMENTQUEUE_H_

#define MOVEMENT_QUEUE_SIZE 16

#include "StepperMove.h"
#include "ch.h"

typedef struct {
  int head;
  int tail;
  int size;
  char isFull;
  char isEmpty;
  StepperMove_t moves[MOVEMENT_QUEUE_SIZE];
  mutex_t mutex;
} MovementQueue_t;

void MovementQueue_Init(MovementQueue_t* const me);
void MovementQueue_Cleanup(MovementQueue_t* const me);
StepperMove_t* MovementQueue_Pull(MovementQueue_t* const me,
                                  StepperMove_t* target);
int MovementQueue_Push(MovementQueue_t* me, StepperMove_t* move);
void MovementQueue_CleanMoves(MovementQueue_t* const me);

MovementQueue_t* MovementQueue_Create(memory_heap_t* heap);
void MovementQueue_Destroy(MovementQueue_t* const me);

#endif /* MARENGO_MOVEMENTQUEUE_H_ */
