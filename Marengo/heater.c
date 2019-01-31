/*
 * heater.c
 *
 *  Created on: 7 sty 2019
 *      Author: grzeg
 */

#include "heater.h"

static const ADCConversionGroup adccg =
    {
       FALSE, // use circular buffer
       ADC_CHAN_NUM, // Number of channels
       NULL, // callback func
       NULL, // callback err func
       0, // CR1 REG
       ADC_CR2_SWSTART, // CR2 REG
       0, // SMPR1 reg
       ADC_SMPR2_SMP_AN4(ADC_SAMPLE_480), // SMPR2 reg
       0,  // htr reg
       0,  // ltr reg
       0, // SQR1 REG
       0, // SQR2 REG
       ADC_SQR3_SQ1_N(ADC_CHANNEL_IN4), // SQR3 REG
    };
static adcsample_t ADCSampleBuf[ADC_BUF_DEPTH * ADC_CHAN_NUM];

void HeaterInit(void)
{
  // setup input and output
  palSetLineMode(LINE_THERM0, PAL_MODE_INPUT_ANALOG);
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
  adcConvert(&ADCD3, &adccg, ADCSampleBuf, ADC_BUF_DEPTH);
  return ADCSampleBuf[0];

}

float HeaterGetADCVoltage(void)
{
  adcConvert(&ADCD3, &adccg, ADCSampleBuf, ADC_BUF_DEPTH);
  return (3.3*ADCSampleBuf[0])/0xFFF;
}

void HeaterOn(void)
{
  palSetLine(LINE_EXTRUDERCTRL);
}
void HeaterOff(void)
{
  palClearLine(LINE_EXTRUDERCTRL);
}



