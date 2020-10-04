/*
 * HeaterProxy.c
 *
 *  Created on: 2 wrz 2020
 *      Author: grzeg
 */

#include "HeaterProxy.h"


void PWMProxy_Init(PWMProxy_t* const me)
{
  me->val = 0;
  me->line = 0;
  me->lineAltFncNum = 0; // In STM32 MCU's AF0 is system, pwm should be other
  me->pwmChanNum = 0;
  me->pwmd = NULL;
  me->pwmConf.frequency = 0;
  me->pwmConf.period = 0;
  me->pwmConf.callback = NULL;
  for(int i=0; i<PWM_CHANNELS; i++)
  {
    me->pwmConf.channels[i].mode = PWM_OUTPUT_DISABLED;
    me->pwmConf.channels[i].callback = NULL;
  }
}

void PWMProxy_Configure(PWMProxy_t* const me)
{
  if((me->line !=0) && (me->lineAltFncNum != 0) \
      && (me->pwmd != NULL))
  {
    palSetLineMode(me->line, PAL_MODE_ALTERNATE(me->lineAltFncNum));
    pwmStart(me->pwmd, &(me->pwmConf));
  }
}

void PWMProxy_SetPWM(PWMProxy_t* const me, uint32_t val)
{
  me->val = val;
  if(val>0)
    pwmEnableChannel(me->pwmd, me->pwmChanNum,
                   PWM_PERCENTAGE_TO_WIDTH(me->pwmd, me->val*100));
  else
    pwmDisableChannel(me->pwmd, me->pwmChanNum);
}

void ADCProxy_Init(ADCProxy_t* const me)
{
  me->lastadc = 0;
  me->adcChanNum = 0;
  me->adcLine = 0;
  me->adcd = NULL;
  memset(&me->adcSampleBuf, 0, sizeof(adcsample_t)*
         HEATER_ADC_SAMPLE_BUF_DEPTH*HEATER_ADC_SAMPLE_CHAN_NUM);
  memset(&me->adcg, 0, sizeof(ADCConversionGroup));
}

void ADCProxy_Configure(ADCProxy_t* const me)
{
  if((me->adcLine != 0) && (me->adcd != NULL))
  {
    palSetLineMode(me->adcLine, PAL_MODE_INPUT_ANALOG);

    adcAcquireBus(me->adcd);
    adcStart(me->adcd, NULL);
    adcReleaseBus(me->adcd);
  }
}

static uint32_t ADCProxy_ADCConvert(ADCProxy_t* const me)
{
  adcAcquireBus(me->adcd);
  while(MSG_OK != adcConvert(me->adcd, &me->adcg, me->adcSampleBuf,
                             HEATER_ADC_SAMPLE_BUF_DEPTH))
  {}
  adcReleaseBus(me->adcd);
  // Take the mean value from all the samples to get rid of noise
  uint64_t mean=0;
  for(int i=0;i<HEATER_ADC_SAMPLE_BUF_DEPTH; i++)
    mean+=me->adcSampleBuf[i];
  mean/=HEATER_ADC_SAMPLE_BUF_DEPTH;
  me->lastadc = (uint32_t)mean;
  return (uint32_t)mean;
}

static uint32_t ADCProxy_GetADC(ADCProxy_t* const me)
{
  return me->lastadc;
}

void HeaterProxy_Init(HeaterProxy_t* const me)
{
  me->isOn = FALSE;
  me->temp = HEATER_TEMP_MIN;
  me->mintemp = HEATER_TEMP_MIN;
  me->maxtemp = HEATER_TEMP_MAX;
  me->name = NULL;
  me->lookupTableSize = 0;
  me->lookupTable = NULL;
  memset(&me->tempBuf, 0 ,sizeof(TempBuffer_t));
  me->tempBuf.dataStart=0;
  me->tempBuf.dataEnd=0;
  me->tempBuf.dataSize=0;
  me->interval = 0;
  chTMObjectInit(&me->sysinterval);
  me->pid.difGain=0;
  me->pid.intGain=0;
  me->pid.propGain=1;
  PWMProxy_Init(&me->pwm);
  ADCProxy_Init(&me->adc);
}

void HeaterProxy_Start(HeaterProxy_t* const me)
{
  // TODO: Check if ADC and PWM are correctly configured
  ADCProxy_Configure(&me->adc);
  PWMProxy_Configure(&me->pwm);
  chTMStartMeasurementX(&me->sysinterval);
  me->tp = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(128),
                               "HeaterProxyThread", NORMALPRIO+1,
                               HeaterProxyThread, me);
  if(me->tp==NULL)
    chSysHalt("Out of memory :(");
}

void HeaterProxy_Stop(HeaterProxy_t* const me)
{
  chTMStopMeasurementX(&me->sysinterval);
  // TODO: Flush the temp buffer
  chThdTerminate(me->tp);
  // stop the timer
}

uint32_t HeaterProxy_GetDesiredTemp(HeaterProxy_t* const me)
{
  return me->temp;
}

uint32_t HeaterProxy_GetRealTemp(HeaterProxy_t* const me)
{
  return HeaterProxy_ADC2Temp(me, ADCProxy_GetADC(&me->adc));
}

void HeaterProxy_SetTemp(HeaterProxy_t* const me, int32_t temp)
{
  if(!me->isOn)
    HeaterProxy_TurnOn(me);
  if(temp<me->mintemp)
  {
    temp = me->mintemp;
    HeaterProxy_TurnOff(me);
  }
  if(temp>me->maxtemp)
    temp = me->maxtemp;
  me->temp = temp;
}

void HeaterProxy_IncSetTemp(HeaterProxy_t* const me)
{
  if(me->temp<me->maxtemp)
    me->temp++;
  if(!me->isOn)
    HeaterProxy_TurnOn(me);
}
void HeaterProxy_DecSetTemp(HeaterProxy_t* const me)
{
  if(me->temp>me->mintemp)
    me->temp--;
  if(!me->isOn)
    HeaterProxy_TurnOn(me);
}

void HeaterProxy_SetPID(HeaterProxy_t* const me, int32_t p, \
                         int32_t i, int32_t d)
{
  me->pid.propGain = p;
  me->pid.intGain = i;
  me->pid.difGain = d;
}
static int32_t HeaterProxy_ADC2Temp(HeaterProxy_t* const me, uint32_t adc)
{
  int32_t i=1;
  // Find the closest point
  while(i<me->lookupTableSize-1)
  {
    if(*(me->lookupTable+2*i)>(int32_t)adc)
      break;
    i++;
  }
  // Get linear approximation between the consecutive points
  int32_t deltaVar = (*(me->lookupTable+2*i+1)-*(me->lookupTable+2*(i-1)+1));
  int32_t deltaArg = (*(me->lookupTable+2*i)-*(me->lookupTable+2*(i-1)));
  int32_t diff = (adc-*(me->lookupTable+2*(i-1)));
  int32_t var0 = *(me->lookupTable+2*(i-1)+1);
  return var0+deltaVar*diff/deltaArg;
}

static void HeaterProxy_BufferPush(HeaterProxy_t* const me, uint32_t val)
{

  me->tempBuf.tempDataBuf[(me->tempBuf.dataStart+
                              me->tempBuf.dataSize)%
                          HEATER_TEMP_DATA_BUF_SIZE]=val;
  if(me->tempBuf.dataSize==HEATER_TEMP_DATA_BUF_SIZE)
  {
    me->tempBuf.dataStart=(me->tempBuf.dataStart+1)%
        HEATER_TEMP_DATA_BUF_SIZE;
    me->tempBuf.dataEnd=(me->tempBuf.dataEnd+1)%
        HEATER_TEMP_DATA_BUF_SIZE;
  }
  else
  {
    me->tempBuf.dataSize+=1;
    me->tempBuf.dataEnd=(me->tempBuf.dataSize+
                                me->tempBuf.dataStart-1)%
                                    HEATER_TEMP_DATA_BUF_SIZE;
  }
}

static int32_t HeaterProxy_IntegrateError(HeaterProxy_t* const me)
{
  int integral=0;
  for(uint32_t i=0; i<me->tempBuf.dataSize; i++)
  {
    integral+=(me->temp - me->tempBuf.tempDataBuf[(me->tempBuf.dataStart+i)%
                                      HEATER_TEMP_DATA_BUF_SIZE]);
  }
  return integral;
}

// Return the difference between last consecutive measurements
static int32_t HeaterProxy_ErrorDiff(HeaterProxy_t* const me)
{
  if(me->tempBuf.dataSize>1)
    return (me->temp-me->tempBuf.tempDataBuf[me->tempBuf.dataEnd])-
        (me->temp-me->tempBuf.tempDataBuf[(me->tempBuf.dataEnd-1)
                                %HEATER_TEMP_DATA_BUF_SIZE]);
  return 0;
}

static void HeaterProxy_PIDRoutine(HeaterProxy_t* const me)
{
  ADCProxy_ADCConvert(&me->adc);
  int32_t temp = HeaterProxy_GetRealTemp(me);
  chTMStopMeasurementX(&me->sysinterval);
  me->interval = TIME_I2MS(me->sysinterval.last);
  chTMStartMeasurementX(&me->sysinterval);
  HeaterProxy_BufferPush(me, temp);
  int32_t pwm = me->pid.propGain * (me->temp-temp);
  pwm += (me->pid.intGain * HeaterProxy_IntegrateError(me))/((int32_t)me->tempBuf.dataSize);
  pwm +=(me->pid.difGain * HeaterProxy_ErrorDiff(me))/((int32_t)me->interval);
  if(pwm>100)
    pwm=100;
  if(pwm<0)
    pwm=0;
  if(me->isOn)
    PWMProxy_SetPWM(&me->pwm, pwm);
}

static THD_FUNCTION(HeaterProxyThread, arg) {
  HeaterProxy_t* me = (HeaterProxy_t*)arg;
  chRegSetThreadName("HeaterProxy");
  while(1){
    HeaterProxy_PIDRoutine(me);
    chThdSleepMilliseconds(100);
  }
  chthdExit((msg_t)0);
}

void HeaterProxy_TurnOn(HeaterProxy_t* const me)
{
  me->isOn = TRUE;
}

void HeaterProxy_TurnOff(HeaterProxy_t* const me)
{
  me->isOn = FALSE;
  PWMProxy_SetPWM(&me->pwm, 0);
}

HeaterProxy_t* HeaterProxy_Create(memory_heap_t* heap)
{
  HeaterProxy_t* me = \
      (HeaterProxy_t*)chHeapAlloc(heap, sizeof(HeaterProxy_t));
  if(NULL!=me)
    HeaterProxy_Init(me);
  return me;
}

void HeaterProxy_Destroy(HeaterProxy_t* const me)
{
  // TODO: Stop the thread if exists
  if(me->tp!=NULL)
    chThdTerminate(me->tp);
  chHeapFree(me);
}
