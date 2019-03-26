/*
 * heater.h
 *
 *  Created on: 7 sty 2019
 *      Author: grzeg
 */


#ifndef MARENGO_HEATER_H_
#define MARENGO_HEATER_H_

#include "ch.h"
#include "hal.h"
#include "pin_mapping.h"

#define HEATER_ADC_SAMPLE_CHAN_NUM 1
#define HEATER_ADC_SAMPLE_BUF_DEPTH 1
#define HEATER_ADC_DATA_BUF_DEPTH 16


typedef struct {
  char* name;
  int maxTemp;
  int minTemp;
  int temp;
  ioline_t ctrlLine;
  int ctrLineAltFncNum;
  PWMDriver* pwmd;
  int pwmChanNum;
  ioline_t adcLine;
  ADCDriver* adcd;
  int adcChanNum;
  ADCConversionGroup *adcg;
  adcsample_t adcSampleBuf[HEATER_ADC_SAMPLE_BUF_DEPTH * HEATER_ADC_SAMPLE_CHAN_NUM];
  int pidPropGain;
  int pidIntGain;
  int adcDataBuf[HEATER_ADC_DATA_BUF_DEPTH];
  int adcDataStart;
  int adcDataEnd;
  int adcDataSize;
  int lookupTableSize;
  int* lookupTable;
} heater_t;

heater_t* heaterCreate(heater_t *heater, char* name, int maxTemp, int minTemp, \
                      ioline_t ctrlLine, int ctrlLineAltFncNum, \
                      PWMDriver *pwmd, int pwmChanNum, PWMConfig *pwmConf, \
                      ioline_t adcLine, ADCDriver *adcd, int adcChanNum, \
                      ADCConversionGroup *adcg, int pidPropGain, int pidIntGain,\
                      int lookupTableSize, int* lookupTable);
heater_t* heaterDestroy(heater_t *heater);
int heaterADCToTemp(heater_t *heater, int adc);
int heaterSetTemp(heater_t *heater, int temp);
int heaterGetADC(heater_t *heater);
int heaterReadADC(heater_t *heater);
int heaterPopADC(heater_t *heater);
void heaterPushADC(heater_t *heater, int adcData);
int heaterIntegrateADC(heater_t *heater);
int heaterDifferentiateADC(heater_t *heater);
void heaterOn(heater_t *heater);
void heaterOff(heater_t *heater);
void heaterSetPWM(heater_t *heater, int pwm);
int heaterSetTemp(heater_t *heater, int temp);
int heaterGetTemp(heater_t *heater);
int heaterIntegrateTemp(heater_t *heater);

void heaterInit(void);
void heaterClenup(void);

thread_t* heatThread;


static Heater1LookupTable[44] = {70,300,
                                 94,280,
                                 127,260,
                                 175,240,
                                 246,220,
                                 354,200,
                                 517,180,
                                 761,160,
                                 1120,140,
                                 1614,120,
                                 2220,100,
                                 2699,85,
                                 2853,80,
                                 3386,60,
                                 3590,50,
                                 3748,40,
                                 3911,25,
                                 4004,10,
                                 4041,0,
                                 4046,-10,
                                 4086,-30,
                                 4092,-50};

heater_t Heater1;
heater_t Heater2;


#endif /* MARENGO_HEATER_H_ */
