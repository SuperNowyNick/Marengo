#pragma once

#include "gfx.h"

#define DialWidgetCreate(so, pI) DialWidgetGCreate(GDISP, so, pI)
#define DIAL_THICKNESS 5
#define DIAL_ROTATION_SPEED 5 // In deg per interval
#define DIAL_ROTATION_INTERVAL 100 // in ms
#define DIAL_FLAG_ROTATE 0

#if GDISP_NEED_ARC != TRUE
#error "GDISP_NEED_ARC needs to be set TRUE"
#endif

typedef struct DialWidget_t {
	GWidgetObject w;
	int32_t value;
	int32_t setvalue;
	int32_t max;
	int32_t min;
	int32_t time;
	GTimer gt;
	bool_t rotate;
	bool_t showset;
} DialWidget;

GHandle DialWidgetGCreate(GDisplay* g, DialWidget* wo, GWidgetInit* pInit);
void DialWidgetDraw(GWidgetObject* gw, void *param);
void DialWidgetMouseDown(GWidgetObject *gw, coord_t x, coord_t y);
void DialWidgetTimerCallback(void* arg);
void DialWidgetSetRotation(DialWidget* dw, bool_t rotation);
bool_t DialWidgetGetRotation(DialWidget* dw);
int32_t DialWidgetGetValue(DialWidget* dw);
void DialWidgetSetValue(DialWidget* dw, int32_t value);
void DialWidgetSetMaxValue(DialWidget* dw, int32_t maxval);
void DialWidgetSetMinValue(DialWidget* dw, int32_t minval);
void DialWidgetSetSetValue(DialWidget* dw, int32_t value);
