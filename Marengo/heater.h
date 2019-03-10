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

#define HEAT_ADC_CHAN_NUM 1
#define HEAT_ADC_BUF_DEPTH 1
int heatFreq;
int heatPWMDuty;
int heatSetTemp;
int heatPrevADC;

typedef struct {
  char* name;
  int maxTemp;
  int minTemp;
  int temp;
  ioline_t ctrlLine;
  ioline_t adcLine;
  ADCDriver* adcd;
  int adcChanNum;
  int adcDataBuf[16];
  int adcDataStart;
  int adcDataEnd;
  int adcDataSize;
  int lookupTableSize;
  int* lookupTable;
} heater_t;

heater_t* createHeater(heater_t *heater, char* name, int maxTemp, int minTemp, \
                      ioline_t ctrlLine, ioline_t adcLine, ADCDriver *adcd, \
                      int adcChanNum, int lookupTableSize, int* lookupTable);

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

heater_t* createHeater(heater_t *heater, char* name, int maxTemp, int minTemp, \
                      ioline_t ctrlLine, ioline_t adcLine, ADCDriver *adcd, \
                      int adcChanNum, int lookupTableSize, int* lookupTable);
int getHeaterTemp(heater_t *heater, int adc);
int setHeaterTemp(heater_t *heater, int temp);
int getHeaterADC(heater_t *heater);
int readHeaterADC(heater_t *heater);
int getHeaterADCIntegral(heater_t *heater);
int getHeaterADCDerivative(heater_t *heater);
void HeaterOn(heater_t *heater);
void HeaterOff(heater_t *heater);
void HeaterSetPWM(int pwm);
int HeaterSetTemp(heater_t *heater, int temp);

thread_t* heatThread;

void HeaterInit(void);
int HeaterGetExtruderTemp(void);
int HeaterGetADCValue(void);
void CleanUpHeater(void);


#endif /* MARENGO_HEATER_H_ */
