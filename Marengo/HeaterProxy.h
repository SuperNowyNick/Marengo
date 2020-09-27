/*
 * HeaterProxy.h
 *
 *  Created on: 2 wrz 2020
 *      Author: grzeg
 */

#ifndef MARENGO_HEATERPROXY_H_
#define MARENGO_HEATERPROXY_H_

#include "ch.h"
#include "hal.h"
#include "chtypes.h"

#define HEATER_TEMP_MAX 300
#define HEATER_TEMP_MIN 25

#define HEATER_ADC_SAMPLE_CHAN_NUM 1
#define HEATER_ADC_SAMPLE_BUF_DEPTH 128

#define HEATER_TEMP_DATA_BUF_SIZE 32

typedef struct {
  int32_t propGain;
  int32_t intGain;
  int32_t difGain;
} PIDParams_t;

typedef struct {
  uint32_t tempDataBuf[HEATER_TEMP_DATA_BUF_SIZE];
  uint32_t dataStart;
  uint32_t dataEnd;
  uint32_t dataSize;
} TempBuffer_t;

typedef struct {
  uint32_t lastadc;
  ioline_t adcLine;
  ADCDriver* adcd;
  int adcChanNum;
  ADCConversionGroup adcg;
  adcsample_t adcSampleBuf[HEATER_ADC_SAMPLE_BUF_DEPTH * HEATER_ADC_SAMPLE_CHAN_NUM];
} ADCProxy_t;

typedef struct {
  int val;
  ioline_t line;
  int lineAltFncNum;
  PWMDriver* pwmd;
  PWMConfig pwmConf;
  int pwmChanNum;
} PWMProxy_t;

typedef struct {
  int32_t temp;
  int32_t maxtemp;
  int32_t mintemp;
  bool isOn;
  char* name;
  int lookupTableSize;
  int* lookupTable;
  time_msecs_t interval;
  time_measurement_t sysinterval; // holds time interval between last readings
  TempBuffer_t tempBuf;
  PIDParams_t pid;
  PWMProxy_t pwm;
  ADCProxy_t adc;
  thread_t *tp;
} HeaterProxy_t;

void PWMProxy_Init(PWMProxy_t* const me);
void PWMProxy_Configure(PWMProxy_t* const me);
void PWMProxy_SetPWM(PWMProxy_t* const me, uint32_t val);

void ADCProxy_Init(ADCProxy_t* const me);
void ADCProxy_Configure(ADCProxy_t* const me);
static uint32_t ADCProxy_ADCConvert(ADCProxy_t* const me);
static uint32_t ADCProxy_GetADC(ADCProxy_t* const me);

void HeaterProxy_Init(HeaterProxy_t* const me);
void HeaterProxy_Start(HeaterProxy_t* const me);
void HeaterProxy_Stop(HeaterProxy_t* const me);
uint32_t HeaterProxy_GetDesiredTemp(HeaterProxy_t* const me);
uint32_t HeaterProxy_GetRealTemp(HeaterProxy_t* const me);
void HeaterProxy_SetTemp(HeaterProxy_t* const me, int32_t temp);
void HeaterProxy_IncSetTemp(HeaterProxy_t* const me);
void HeaterProxy_DecSetTemp(HeaterProxy_t* const me);
void HeaterProxy_SetPID(HeaterProxy_t* const me, int32_t p, \
                         int32_t i, int32_t d);
static int32_t HeaterProxy_ADC2Temp(HeaterProxy_t* const me, uint32_t adc);
static void HeaterProxy_BufferPush(HeaterProxy_t* const me, uint32_t val);
static int32_t HeaterProxy_IntegrateError(HeaterProxy_t* const me);
static int32_t HeaterProxy_ErrorDiff(HeaterProxy_t* const me);
static void HeaterProxy_PIDRoutine(HeaterProxy_t* const me);
static THD_FUNCTION(HeaterProxyThread, arg);
void HeaterProxy_TurnOn(HeaterProxy_t* const me);
void HeaterProxy_TurnOff(HeaterProxy_t* const me);

HeaterProxy_t* HeaterProxy_Create(memory_heap_t* heap);
void HeaterProxy_Destroy(HeaterProxy_t* const me);

#endif /* MARENGO_HEATERPROXY_H_ */
