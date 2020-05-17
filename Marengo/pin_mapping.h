/*
 * pin_mapping.h
 *
 *  Created on: 4 maj 2018
 *      Author: grzeg
 */

#ifndef MARENGO_PIN_MAPPING_H_
#define MARENGO_PIN_MAPPING_H_

// RAMPS 1.4 PINS
// Stepper motors pins
#define LINE_XSTP                 PAL_LINE(GPIOC, 13U)
#define LINE_XDIR                 PAL_LINE(GPIOE, 6U)
#define LINE_XEN                  PAL_LINE(GPIOD, 2U)
#define LINE_YSTP                 PAL_LINE(GPIOE, 5U)
#define LINE_YDIR                 PAL_LINE(GPIOE, 3U)
#define LINE_YEN                  PAL_LINE(GPIOE, 4U)
#define LINE_ZSTP                 PAL_LINE(GPIOD, 5U)
#define LINE_ZDIR                 PAL_LINE(GPIOD, 7U)
#define LINE_ZEN                  PAL_LINE(GPIOE, 2U)
#define LINE_E1STP                PAL_LINE(GPIOD, 4U)
#define LINE_E1DIR                PAL_LINE(GPIOC, 11U)
#define LINE_E1EN                 PAL_LINE(GPIOC, 12U)
// Endstops
#define LINE_XMIN                 PAL_LINE(GPIOB, 7U)
#define LINE_XMAX                 0
#define LINE_YMIN                 PAL_LINE(GPIOG, 2U)
#define LINE_YMAX                 0
#define LINE_ZMIN                 PAL_LINE(GPIOG, 3U)
#define LINE_ZMAX                 0
// MOSFETs and thermoresistors
#define LINE_EXTRUDERCTRL           PAL_LINE(GPIOB, 4U) // TIM3_CH1
#define LINE_HEATBEDCTRL            PAL_LINE(GPIOB, 3U) // TIM2_CH2

#define LINE_THERM0                 PAL_LINE(GPIOF, 6U) // ADC3IN4
#define LINE_THERM1                 PAL_LINE(GPIOC, 3U) // ADC123IN13
#define LINE_THERM2                 PAL_LINE(GPIOA, 5U) // ADC12IN5

// Intrinsic STM32F429i board peripherals

#define LINE_GREENLED               PAL_LINE(GPIOG, 13U)
#define LINE_REDLED                 PAL_LINE(GPIOG, 14U)
#define LINE_USERBUTTON             PAL_LINE(GPIOA, 0U)

// ESP8266 pins (USART1)
#define ESP_TXD                     PAL_LINE(GPIOA, 9U)
#define ESP_RXD                     PAL_LINE(GPIOA, 10U)

#endif /* MARENGO_PIN_MAPPING_H_ */
