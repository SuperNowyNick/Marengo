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

#define ADC_CHAN_NUM 1
#define ADC_BUF_DEPTH 1
int heaterPWMDuty;
int freq;

void InitHeater(void);
void HeaterPWMDuty(int duty);
int HeaterGetExtruderTemp(void);
int HeaterGetADCValue(void);
void CleanUpHeater(void);
void HeaterOn(void);
void HeaterOff(void);

#endif /* MARENGO_HEATER_H_ */
