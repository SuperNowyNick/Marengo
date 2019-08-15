/*
 * EndstopProxy.h
 *
 *  Created on: 2 lip 2019
 *      Author: grzeg
 */

#ifndef MARENGO_ENDSTOPPROXY_H_
#define MARENGO_ENDSTOPPROXY_H_

#include "ch.h"
#include "hal.h"

typedef struct {
  ioline_t line; // Endstop ioline
  bool isHit; // Endstop state
} EndstopProxy_t;

void EndstopProxy_Init(EndstopProxy_t* const me);
int EndstopProxy_Configure(EndstopProxy_t* const me, ioline_t line);
int EndstopProxy_Activate(EndstopProxy_t* const me);
void EndstopProxy_Deactivate(EndstopProxy_t* const me);
void EndstopProxy_Callback(void *arg);

EndstopProxy_t* EndstopProxy_Create(memory_heap_t* heap);
void EndstopProxy_Destroy(EndstopProxy_t* const me);

#endif /* MARENGO_ENDSTOPPROXY_H_ */
