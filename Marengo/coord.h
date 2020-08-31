/*
 * coord.h
 *
 *  Created on: 25 lut 2020
 *      Author: grzeg
 */

#ifndef MARENGO_COORD_H_
#define MARENGO_COORD_H_

#include "float.h"

typedef struct {
  float_t x;
  float_t y;
  float_t z;
  /*
  float_t int a;
  float_t int b;
  float_t int c;
  float_t int u;
  float_t int v;
  float_t int w;
   */
  float_t e;
} stpCoordF_t;

typedef struct {
  int32_t x;
  int32_t y;
  int32_t z;
  int32_t e;
} stpCoord_t;

stpCoordF_t stpCoordF_Add(stpCoordF_t a, stpCoordF_t b);
stpCoordF_t stpCoordF_Sub(stpCoordF_t a, stpCoordF_t b);
uint32_t stpCoordF_Abs(stpCoordF_t a);
stpCoordF_t stpCoordF_Zero(void);

stpCoord_t stpCoord_Add(stpCoord_t a, stpCoord_t b);
stpCoord_t stpCoord_Sub(stpCoord_t a, stpCoord_t b);
uint32_t stpCoord_Abs(stpCoord_t a);
stpCoord_t stpCoord_Zero(void);

#endif /* MARENGO_COORD_H_ */
