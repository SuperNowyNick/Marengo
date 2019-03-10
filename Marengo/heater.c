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
       HEAT_ADC_CHAN_NUM, // Number of channels
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

static adcsample_t heatADCSampleBuf[HEAT_ADC_BUF_DEPTH * HEAT_ADC_CHAN_NUM];

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

heater_t* createHeater(heater_t *heater, char* name, int maxTemp, int minTemp, \
                      ioline_t ctrlLine, ioline_t adcLine, ADCDriver *adcd, \
                      int adcChanNum, int lookupTableSize, int* lookupTable)
{
  heater->name = name;
  heater->maxTemp = maxTemp;
  heater->minTemp = minTemp;
  heater->adcLine = adcLine;
  heater->ctrlLine = ctrlLine;
  heater->adcd = adcd;
  heater->adcChanNum = adcChanNum;
  heater->adcDataStart = 0;
  heater->adcDataEnd = 0;
  heater->adcDataSize = 0;
  heater->lookupTableSize = lookupTableSize;
  heater->lookupTable = lookupTable;
  return heater;
}
int intdataHeaterADC(heater_t *heater)
{
  int integral=0;
  for(int i=0; i<heater->adcDataSize; i++)
  {
    integral+=heater->adcDataBuf[(heater->adcDataStart+i)%16];
  }
  return integral;
}
int intdataHeaterTemp(heater_t *heater)
{
  int integral=0;
  for(int i=0; i<heater->adcDataSize; i++)
  {
    integral+=getHeaterTemp(&Heater1, heater->adcDataBuf[(heater->adcDataStart+i)%16]);
  }
  return integral;
}
int diffdataHeaterADC(heater_t *heater)
{
  int diff;
  if(heater->adcDataSize>1)
    diff=heater->adcDataBuf[heater->adcDataEnd]-heater->adcDataBuf[(heater->adcDataEnd-1)%16];
  else
      diff=0;
  return diff;
}
int popdataHeaterADC(heater_t *heater)
{
  int pop=heater->adcDataBuf[(heater->adcDataEnd-1)%16];
  heater->adcDataSize-=1;
  heater->adcDataEnd=(heater->adcDataStart+heater->adcDataSize)%16;
  return pop;
}
void pushdataHeaterADC(heater_t *heater, int adcData)
{
  heater->adcDataBuf[(heater->adcDataStart+heater->adcDataSize)%16]=adcData;
  if(heater->adcDataSize==16)
  {
    heater->adcDataStart=(heater->adcDataStart+1)%16;
    heater->adcDataEnd=(heater->adcDataEnd+1)%16;
  }
  else
  {
    heater->adcDataSize+=1;
    heater->adcDataEnd=(heater->adcDataSize+heater->adcDataStart-1)%16;
  }
}
int readHeaterADC(heater_t *heater)
{
  adcConvert(&ADCD3, &adccg, heatADCSampleBuf, HEAT_ADC_BUF_DEPTH);

}
int getHeaterAdc(heater_t *heater)
{
  return heater->adcDataBuf[heater->adcDataEnd];
}
int getHeaterLastTemp(heater_t *heater)
{
  return getHeaterTemp(heater, getHeaterAdc(heater));
}

int getHeaterTemp(heater_t *heater, int adc)
{
  int i=1;
  //int adc=getHeaterAdc(heater);
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
    adcConvert(&ADCD3, &adccg, heatADCSampleBuf, HEAT_ADC_BUF_DEPTH);
    pushdataHeaterADC(&Heater1, heatADCSampleBuf[0]);
    int difference = Heater1.temp-getHeaterLastTemp(&Heater1);
    int integral = Heater1.temp-intdataHeaterTemp(&Heater1)/16;
    int gain = 5;
    int intgain = 5;
    int pwm= difference*gain+integral*intgain;
    if(pwm>100)
          pwm=100;
    if(pwm<0)
      pwm=0;
    //consPrintf("PID:Temp:%dC,Err:%dC,IntErr:%dC,setPWM:%d"CONSOLE_NEWLINE_STR, getHeaterLastTemp(&Heater1), difference, integral, pwm);
    HeaterSetPWM(pwm);
    chThdSleepMilliseconds(1000);
  }
}

void HeaterInit(void)
{
  // setup input and output
  adcInit();
  pwmInit();
  palSetLineMode(LINE_THERM0, PAL_MODE_INPUT_ANALOG);
  palSetLineMode(LINE_EXTRUDERCTRL, PAL_MODE_ALTERNATE(2));
  adcStart(&ADCD3, NULL);
  pwmStart(&PWMD2, &PWMConf2);
  pwmStart(&PWMD3, &PWMConf3);
  createHeater(&Heater1, "Extruder", 285, 5, LINE_THERM0, LINE_EXTRUDERCTRL,\
               &ADCD3, ADC_CHANNEL_IN4, 22, &Heater1LookupTable);
  HeaterOff(&Heater1);
  if(heatThread == NULL)
    heatThread = chThdCreateStatic(waHeater1Thread, sizeof(waHeater1Thread),
                      NORMALPRIO + 10, Heater1Thread, NULL);
}

void HeaterCleanUp(void)
{
  HeaterOff(&Heater1);
  adcStopConversion(&ADCD3);
  adcStop(&ADCD3);
}


int HeaterGetADCValue(void)
{
  adcConvert(&ADCD3, &adccg, heatADCSampleBuf, HEAT_ADC_BUF_DEPTH);
  return heatADCSampleBuf[0];

}

float HeaterGetADCVoltage(void)
{
  adcConvert(&ADCD3, &adccg, heatADCSampleBuf, HEAT_ADC_BUF_DEPTH);
  return (3.3*heatADCSampleBuf[0])/0xFFF;
}
void HeaterSetPWM(int pwm)
{
  pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, pwm*100));
}
void HeaterOn(heater_t *heater)
{
  pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 10000));
}
void HeaterOff(heater_t *heater)
{
  pwmDisableChannel(&PWMD3, 0);
}
int HeaterSetTemp(heater_t *heater, int temp)
{
  if(temp<heater->minTemp)
    temp=heater->minTemp;
  if(temp>heater->maxTemp)
    temp=heater->maxTemp;
  heater->temp = temp;
}




