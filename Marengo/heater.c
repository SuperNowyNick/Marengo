/*
 * heater.c
 *
 *  Created on: 7 sty 2019
 *      Author: grzeg
 */

#include "heater.h"

static const ADCConversionGroup adccg =
    {
       TRUE, // use circular buffer
       (uint16_t)(ADC_CHAN_NUM), // Number of channels
       NULL, // callback func
       NULL, // callback err func
       0, // CR1 REG
       ADC_CR2_SWSTART, // CR2 REG
       0, // SMPR1 reg
       0, // SMPR2 reg
       ((ADC_CHAN_NUM -1)<<20), // SQR1 REG
       0, // SQR2 REG
       ADC_SQR3_SQ1_N(ADC_CHANNEL_IN4), // SQR3 REG
    };
static adcsample_t ADCSampleBuf[ADC_BUF_DEPTH * ADC_CHAN_NUM];

void HeaterInit(void)
{
  // setup input and output
  palSetLineMode(LINE_EXTRUDERTEMP, PAL_MODE_INPUT_ANALOG);
  palSetLineMode(LINE_EXTRUDERCTRL, PAL_MODE_OUTPUT_PUSHPULL);
  palClearLine(LINE_EXTRUDERCTRL);

  // config adc
  adcInit();
  adcStart(&ADCD3, NULL);
}

adccallback_t HeaterADCFinished()
{

}

void HeaterCleanUp(void)
{
  HeaterOff();
  adcStopConversion(&ADCD3);
  adcStop(&ADCD3);
}


int HeaterGetADCValue(void)
{
  adcStartConversion(&ADCD3, &adccg, ADCSampleBuf, ADC_BUF_DEPTH);
  adcStopConversion(&ADCD3);
  return ADCSampleBuf[0];

}

float HeaterGetADCVoltage(void)
{
    adcStartConversion(&ADCD3, &adccg, ADCSampleBuf, ADC_BUF_DEPTH);
    adcStopConversion(&ADCD3);
  return (3.3f*ADCSampleBuf[0])/0xFFF;
}

void HeaterOn(void)
{
  palSetLine(LINE_EXTRUDERCTRL);
}
void HeaterOff(void)
{
  palClearLine(LINE_EXTRUDERCTRL);
}

