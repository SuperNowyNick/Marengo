#pragma once

#include "dialwidget.h"
#include "src/gwin/gwin_class.h"

static const gwidgetVMT DialWidgetVMT = {
	{
		"DialWidget",
		sizeof(DialWidget),
		_gwidgetDestroy,
		_gwidgetRedraw,
		0,
	},
	DialWidgetDraw,
#if GINPUT_NEED_MOUSE
	DialWidgetMouseDown,// DialWidgetMouseDown,
	0,// DialWidgetMouseUp,
	0,// DialWidgetMouseMove
#endif
#if GINPUT_NEED_KEYBOARD || GWIN_NEED_KEYBOARD
	0,
#endif
#if GINPUT_NEED_TOGGLE
	0,
	0,
	0,
	0,
	0,
#endif	
#if GINPUT_NEED_DIAL
	0,
	0,
	0,
	0,
#endif
};

void DialWidgetMouseDown(GWidgetObject *gw, coord_t x, coord_t y)
{
	DialWidget* dw = gw;
	dw->showset = !dw->showset;
}

void DialWidgetDraw(GWidgetObject* gw, void *param)
{
	const GColorSet* colors;
	DialWidget* dw = gw;
	(void)param;

	// Make sure this is a dial
	if (gw->g.vmt != (gwinVMT*)&DialWidgetVMT)
		return;

	if (!(gw->g.flags & GWIN_FLG_SYSENABLED))
		colors = &gw->pstyle->disabled;
	else
		colors = &gw->pstyle->enabled;

	coord_t xcen = gw->g.x + gw->g.width / 2;
	coord_t ycen = gw->g.y + gw->g.height / 2;
	coord_t rad = gw->g.width > gw->g.height ? gw->g.width / 2 : gw->g.height / 2;
	coord_t ang = (360 * (dw->value - dw->min)) / dw->max;

	gdispGFillCircle(gw->g.display, xcen, ycen, rad, gw->pstyle->background);
	(void)colors;
	if (dw->value > dw->min) {
		// Draw 1px smaller circle, so all pixel get cleared when redrawing
		gdispGDrawThickArc(gw->g.display, xcen, ycen, rad - DIAL_THICKNESS - 1, rad - 1, dw->time, dw->time + ang, colors->edge);
	}

	char buf[32];
	if (!dw->showset) {
		itoa(dw->value, buf, 10);
		coord_t textx = gdispGetStringWidth(buf, gw->g.font);
		coord_t texty = gdispGetFontMetric(gw->g.font, fontHeight);
		gdispGDrawString(gw->g.display, xcen - textx / 2, ycen - texty / 2, buf, gw->g.font, colors->text);
	}
	else {
		itoa(dw->setvalue, buf, 10);
		coord_t textx = gdispGetStringWidth(buf, gw->g.font);
		coord_t texty = gdispGetFontMetric(gw->g.font, fontHeight);
		gdispGDrawString(gw->g.display, xcen - gdispGetStringWidth("Set:", gw->g.font) / 2, ycen - texty, "Set:", gw->g.font, colors->text);
		gdispGDrawString(gw->g.display, xcen - textx / 2, ycen , buf, gw->g.font, colors->text);
	}

	if (dw->time > 360)
		dw->time -= 360;
	if (dw->rotate == TRUE) {
		dw->time += DIAL_ROTATION_SPEED;
		gtimerStart(&dw->gt, DialWidgetTimerCallback, dw, FALSE, DIAL_ROTATION_INTERVAL);
	}
}

void DialWidgetTimerCallback(void* arg)
{
	GWidgetObject* wo = arg;
	_gwidgetRedraw(wo);
}

void DialWidgetSetRotation(DialWidget * dw, bool_t rotation)
{
	dw->rotate = rotation;
	_gwidgetRedraw(dw);
}

bool_t DialWidgetGetRotation(DialWidget * dw)
{
	return dw->rotate;
}

int32_t DialWidgetGetValue(DialWidget * dw)
{
	return dw->value;
}

void DialWidgetSetValue(DialWidget * dw, int32_t value)
{
	dw->value = value;
}

void DialWidgetSetSetValue(DialWidget* dw, int32_t value)
{
	if (value > dw->max)
		value = dw->max;
	else if (value < dw->min)
		value = dw->min;
	dw->setvalue = value;
}

void DialWidgetSetMaxValue(DialWidget * dw, int32_t maxval)
{
	dw->max = maxval;
}

void DialWidgetSetMinValue(DialWidget * dw, int32_t minval)
{
	dw->min = minval;
}


GHandle DialWidgetGCreate(GDisplay* g, DialWidget* dw, GWidgetInit* pInit)
{
	if (!(dw = (DialWidget*)_gwidgetCreate(g, dw, pInit, &DialWidgetVMT)))
		return 0;

	dw->max = 100;
	dw->min = 0;
	dw->value = 50;
	dw->setvalue = 100;
	dw->time = 0;
	dw->rotate = TRUE;
	dw->showset = FALSE;

	gwinSetVisible((GHandle)dw, pInit->g.show);

	return (GHandle)dw;
}