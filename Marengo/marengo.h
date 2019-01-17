/*
 * marengo.h
 *
 *  Created on: 11 wrz 2018
 *      Author: grzeg
 */

#ifndef MARENGO_MARENGO_H_
#define MARENGO_MARENGO_H_

#include "Stepper.h"

typedef enum mar_state_t {
  MARENGO_IDLE,
  MARENGO_INITIALIZING,
  MARENGO_LOADING,
  MARENGO_PRINTING,
  MARENGO_FINISHED,
  MARENGO_ERROR
} mar_state_t;

typedef enum mar_units_t {
  MARENGO_UNITS_METRIC,
  MAENRGO_UNITS_IMPERIAL
} mar_units_t;



int mar_bCoolant;

// These should be set using homing procedure
int mar_xPos;
int mar_yPos;
float mar_feedrate;
#define MAR_MAX_FEEDRATE




void MarengoInit();

#endif /* MARENGO_MARENGO_H_ */
