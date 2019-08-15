/*
 * EndstopProxy.c
 *
 *  Created on: 2 lip 2019
 *      Author: grzeg
 */

#include "EndstopProxy.h"

void EndstopProxy_Init(EndstopProxy_t* const me)
{
  me->line=0;
  me->isHit=0;
}

int EndstopProxy_Configure(EndstopProxy_t* const me, ioline_t line)
{
  if(!line)
    return 1;
  me->line = line;
  palSetLineMode(me->line, PAL_MODE_INPUT_PULLDOWN);
  me->isHit = palReadLine(me->line);
  return 0;
}

int EndstopProxy_Activate(EndstopProxy_t* const me)
{
  if(!me || !me->line)
    return 1;
  palSetLineCallback(me->line, (palcallback_t)EndstopProxy_Callback, (void*)me);
  return 0;
}

void EndstopProxy_Deactivate(EndstopProxy_t* const me)
{
  if(me->line)
    palDisableLineEvent(me->line);
}


void EndstopProxy_Callback(void *arg)
{
  EndstopProxy_t* me = arg;
  if(me)
  {
    me->isHit = palReadLine(me->line);
  }
}

EndstopProxy_t* EndstopProxy_Create(memory_heap_t* heap)
{
  EndstopProxy_t* me = \
      (EndstopProxy_t*)chHeapAlloc(heap, sizeof(EndstopProxy_t));
  if(me!=NULL)
  {
    EndstopProxy_Init(me);
  }
  return me;
}


void EndstopProxy_Destroy(EndstopProxy_t* const me)
{
  chHeapFree(me);
}
