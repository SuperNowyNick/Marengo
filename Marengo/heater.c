/*
 * heater.c
 *
 *  Created on: 7 sty 2019
 *      Author: grzeg
 */

#include "heater.h"
#include "console.h"

static const ADCConversionGroup adccg =
    {
       FALSE, // use circular buffer
       4, // Number of channels
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
static PWMConfig PWMConf2 = {10000, 1000, NULL, {
                                                  {PWM_OUTPUT_DISABLED, NULL},
                                                  {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                                                  {PWM_OUTPUT_DISABLED, NULL},
                                                  {PWM_OUTPUT_DISABLED, NULL}
                             }, 0, 0};
static PWMConfig PWMConf3 = {10000, 1000, NULL, {
                                                  {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                                                  {PWM_OUTPUT_DISABLED, NULL},
                                                  {PWM_OUTPUT_DISABLED, NULL},
                                                  {PWM_OUTPUT_DISABLED, NULL}
                                                  },0, 0};

heater_t* heaterCreate(heater_t *heater, char* name, int maxTemp, int minTemp, \
                      ioline_t ctrlLine, PWMDriver *pwmd, int pwmChanNum, \
                      PWMConfig *pwmConf, \
                      ioline_t adcLine, ADCDriver *adcd, int adcChanNum, \
                      ADCConversionGroup *adcg, \
                      int lookupTableSize, int* lookupTable)
{
  palSetLineMode(adcLine, PAL_MODE_INPUT_ANALOG);
  palSetLineMode(ctrlLine, PAL_MODE_ALTERNATE(2));
  adcStart(adcd, NULL);
  pwmStart(pwmd, pwmConf);
  heater->name = name;
  heater->maxTemp = maxTemp;
  heater->minTemp = minTemp;
  heater->ctrlLine = ctrlLine;
  heater->pwmd = pwmd;
  heater->pwmChanNum = pwmChanNum;
  heater->adcLine = adcLine;
  heater->adcd = adcd;
  heater->adcChanNum = adcChanNum;
  heater->adcg = adcg;
  heater->adcDataStart = 0;
  heater->adcDataEnd = 0;
  heater->adcDataSize = 0;
  heater->lookupTableSize = lookupTableSize;
  heater->lookupTable = lookupTable;
  heaterOff(heater);
  return heater;
}
int heaterIntegrateADC(heater_t *heater)
{
  int integral=0;
  for(int i=0; i<heater->adcDataSize; i++)
  {
    integral+=heater->adcDataBuf[(heater->adcDataStart+i)%HEATER_ADC_DATA_BUF_DEPTH];
  }
  return integral;
}
int heaterIntegrateTemp(heater_t *heater)
{
  int integral=0;
  for(int i=0; i<heater->adcDataSize; i++)
  {
    integral+=heaterADCToTemp(&Heater1, heater->adcDataBuf[(heater->adcDataStart+i)%HEATER_ADC_DATA_BUF_DEPTH]);
  }
  return integral;
}
int heaterDifferentiateADC(heater_t *heater)
{
  int diff;
  if(heater->adcDataSize>1)
    diff=heater->adcDataBuf[heater->adcDataEnd]-heater->adcDataBuf[(heater->adcDataEnd-1)%16];
  else
      diff=0;
  return diff;
}
int heaterPopADC(heater_t *heater)
{
  int pop=heater->adcDataBuf[(heater->adcDataEnd-1)%HEATER_ADC_DATA_BUF_DEPTH];
  heater->adcDataSize-=1;
  heater->adcDataEnd=(heater->adcDataStart+heater->adcDataSize)%HEATER_ADC_DATA_BUF_DEPTH;
  return pop;
}
void heaterPushADC(heater_t *heater, int adcData)
{
  heater->adcDataBuf[(heater->adcDataStart+heater->adcDataSize)%HEATER_ADC_DATA_BUF_DEPTH]=adcData;
  if(heater->adcDataSize==HEATER_ADC_DATA_BUF_DEPTH)
  {
    heater->adcDataStart=(heater->adcDataStart+1)%HEATER_ADC_DATA_BUF_DEPTH;
    heater->adcDataEnd=(heater->adcDataEnd+1)%HEATER_ADC_DATA_BUF_DEPTH;
  }
  else
  {
    heater->adcDataSize+=1;
    heater->adcDataEnd=(heater->adcDataSize+heater->adcDataStart-1)%HEATER_ADC_DATA_BUF_DEPTH;
  }
}
int heaterReadADC(heater_t *heater)
{
  adcConvert(heater->adcd, heater->adcg, heater->adcSampleBuf, HEATER_ADC_SAMPLE_BUF_DEPTH);
  heaterPushADC(heater, heater->adcSampleBuf[0]);
  return heater->adcSampleBuf[0];
}
int heaterGetADC(heater_t *heater)
{
  return heater->adcDataBuf[heater->adcDataEnd];
}
int heaterGetTemp(heater_t *heater)
{
  return heaterADCToTemp(heater, heaterGetADC(heater));
}
int heaterSetTemp(heater_t *heater, int temp)
{
  if(temp<heater->minTemp)
    temp=heater->minTemp;
  if(temp>heater->maxTemp)
    temp=heater->maxTemp;
  heater->temp = temp;
}
int heaterADCToTemp(heater_t *heater, int adc)
{
  int i=1;
  while(i<heater->lookupTableSize)
  {
    if(*(heater->lookupTable+2*i)>adc)
      break;
    i++;
  }
  int deltaVar = (*(heater->lookupTable+2*i+1)-*(heater->lookupTable+2*(i-1)+1));
  int deltaArg = (*(heater->lookupTable+2*i)-*(heater->lookupTable+2*(i-1)));
  int diff = (adc-*(heater->lookupTable+2*(i-1)));
  int var0 = *(heater->lookupTable+2*(i-1)+1);
  return var0+deltaVar*diff/deltaArg;
}

static THD_WORKING_AREA(waHeater1Thread, 512);
static THD_FUNCTION(Heater1Thread, arg) {
  (void)arg;
  chRegSetThreadName("Heater1PID");
  while(1){
    // Get current ADC value
    heaterReadADC(&Heater1);
    int error = Heater1.temp-heaterGetTemp(&Heater1);
    int interror = Heater1.temp-heaterIntegrateTemp(&Heater1)/Heater1.adcDataSize;
    int gain = 5;
    int intgain = 5;
    int pwm= error*gain+interror*intgain;
    if(pwm>100)
          pwm=100;
    if(pwm<0)
      pwm=0;
    //consPrintf("PID:Temp:%dC,Err:%dC,IntErr:%dC,setPWM:%d"CONSOLE_NEWLINE_STR, getHeaterLastTemp(&Heater1), difference, integral, pwm);
    heaterSetPWM(&Heater1, pwm);
    chThdSleepMilliseconds(1000);
  }
}

heater_t* heaterDestroy(heater_t *heater)
{
  heaterOff(heater);
  adcStopConversion(heater->adcd);
  adcStop(heater->adcd);
}
void heaterSetPWM(heater_t *heater, int pwm)
{
  pwmEnableChannel(heater->pwmd, heater->pwmChanNum, PWM_PERCENTAGE_TO_WIDTH(heater->pwmd, pwm*100));
}
void heaterOn(heater_t *heater)
{
  pwmEnableChannel(heater->pwmd, heater->pwmChanNum, PWM_PERCENTAGE_TO_WIDTH(heater->pwmd, 10000));
}
void heaterOff(heater_t *heater)
{
  pwmDisableChannel(heater->pwmd, heater->pwmChanNum);
}

void heaterInit(void)
{
  // setup input and output
  adcInit();
  pwmInit();
  heaterCreate(&Heater1, "Extruder", 285, 5, LINE_EXTRUDERCTRL, &PWMD3, 0, \
               &PWMConf3, LINE_THERM0,\
               &ADCD3, ADC_CHANNEL_IN4, &adccg, 22, &Heater1LookupTable);
  heaterOff(&Heater1);
  if(heatThread == NULL)
    heatThread = chThdCreateStatic(waHeater1Thread, sizeof(waHeater1Thread),
                      NORMALPRIO + 10, Heater1Thread, NULL);
}
void heaterClenup(void)
{
  heaterDestroy(&Heater1);
}





