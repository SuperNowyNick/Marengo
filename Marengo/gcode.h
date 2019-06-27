/*
 * gcode.h
 *
 *  Created on: 29 lip 2018
 *      Author: grzeg
 */

#ifndef MARENGO_GCODE_H_
#define MARENGO_GCODE_H_

#include "float.h"

#define GCODE_STPSTATUS_DELAY_TIME 1
 // Arbitrary rules due to RS274/NGC language compliance
#define GCODE_MAXLINELENGTH 256 // Maximal line length
#define GCODE_WORDSTARTINGLETTERS "ABCDEFGHIJKLMNPQRSTUVWXYZ" // Allowed first letters of words
#define GCODE_POSSIBLEWORDS "ABCDEFGHIJKLMNPQRSTUVWXYZ*"
#define GCODE_SIGNIFIG 17 // Significant figures in decimal numbers
#define GCODE_INTDECCRIT 0.0001 // Range (as part of integer) in which a decimal should be to be considered close to given decimal
//#define GCODE_PARAMPREFIX '#'
//#define GCODE_EXPRBRACKETS "[]"


#define PARAM_AXIS_X (1<<0)
#define PARAM_AXIS_Y (1<<1)
#define PARAM_AXIS_Z (1<<2)
#define PARAM_AXIS_A (1<<3)
#define PARAM_AXIS_B (1<<4)
#define PARAM_AXIS_C (1<<5)
#define PARAM_AXIS_I (1<<6)
#define PARAM_AXIS_J (1<<7)
#define PARAM_AXIS_K (1<<8)
#define PARAM_AXIS_U (1<<9)
#define PARAM_AXIS_V (1<<10)
#define PARAM_AXIS_W (1<<11)
#define PARAM_MISC_F (1<<0)
#define PARAM_MISC_E (1<<1)
#define PARAM_MISC_H (1<<2)
#define PARAM_MISC_L (1<<3)
#define PARAM_MISC_N (1<<4)
#define PARAM_MISC_P (1<<5)
#define PARAM_MISC_Q (1<<6)
#define PARAM_MISC_R (1<<7)
#define PARAM_MISC_S (1<<8)
#define PARAM_MISC_T (1<<9)
#define PARAM_MISC_D (1<<10)

#define GCODE_GMODALGROUP_1 1 // G0, G1, G2, G3, G38.2, G80, G81, G82, G83, G84, G85, G86, G87, G88, G89 motion
#define GCODE_GMODALGROUP_2 2 // G17, G18, G19 plane selection
#define GCODE_GMODALGROUP_3 3 // G90, G91 distance mode
#define GCODE_GMODALGROUP_5 4 // G93, G94 feed rate mode
#define GCODE_GMODALGROUP_6 5 // G20, G21 units
#define GCODE_GMODALGROUP_7 6 // G40, G41, G42 cutter radius compensation
#define GCODE_GMODALGROUP_8 7 // G43, G49 tool length offset
#define GCODE_GMODALGROUP_10 8 // G98, G99 return mode in canned cycles
#define GCODE_GMODALGROUP_12 9 // G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3 coordinate system selection
#define GCODE_GMODALGROUP_13 10 // G61, G61.1, G64 path control mode

#define GCODE_MMODALGROUP_4 11 // M0, M1, M2, M30, M60 stopping
#define GCODE_MMODALGROUP_6 12 // M6 tool change
#define GCODE_MMODALGROUP_7 13 // M3, M4, M5 spindle turning
#define GCODE_MMODALGROUP_8 14 // M7, M8, M9 coolant (special case M7 and M8 may be active at the same time)
#define GCODE_MMODALGROUP_9 15 // M48, M49 enable/disable feed and speed override switches

#define GCODE_NONMODALGROUP 16 // G4, G10, G28, G30, G53, G92, G92.1, G92.2, G92.3

#define GCODE_MODALGROUP_CODES(i) GCODE_MODALGROUP_CODES_ ## i

#define GCODE_MODALGROUP_CODES_1 "G0G1G2G3G38.2G80G81G82G83G84G85G86G87G88G89" // Motion
#define GCODE_MODALGROUP_CODES_2 "G17G18G19" // Plane selection
#define GCODE_MODALGROUP_CODES_3 "G90G91" // Distance mode
#define GCODE_MODALGROUP_CODES_4 "G93G94" // Feed rate mode
#define GCODE_MODALGROUP_CODES_5 "G20G21" // Units
#define GCODE_MODALGROUP_CODES_6 "G40G41G42" // Cutter radius compensation
#define GCODE_MODALGROUP_CODES_7 "G43G49" // Tool length offset
#define GCODE_MODALGROUP_CODES_8 "G98G99" // Return mode in canned cycles
#define GCODE_MODALGROUP_CODES_9 "G54G55G56G57G58G59G59.1G59.2G59.3" // Coordinate system
#define GCODE_MODALGROUP_CODES_10 "G61G61.1G64" // Path control mode
#define GCODE_MODALGROUP_CODES_11 "M0M1M2M30M60" // Stopping
#define GCODE_MODALGROUP_CODES_12 "M6" // Tool change
#define GCODE_MODALGROUP_CODES_13 "M3M4M5" // Spindle turning
#define GCODE_MODALGROUP_CODES_14 "M7M8M9" // Coolant (special case M7 and M8 may be active at the same time)
#define GCODE_MODALGROUP_CODES_15 "M48M49" // Enable disable feed and speed ovverride switches
#define GCODE_MODALGROUP_CODES_16 "G4G10G28G30G53G92G92.1G92.2G92.3" // Non modal group
// TODO: Implement error codes in parser

#define GCODE_ERROR_SAMEMODALGROUP 0 // Line contains two words from same modal group
#define GCODE_ERROR_UNKNOWNWORD 1 // Lie contains unknown word
#define GCODE_ERROR_GROUP01TGHT 2 // Line contains words from 1 and 0 groups

#define GCODE_WARNING_PARAMSREORDER // Same parameter is set twice in line


// MOVE MODES
#define MOVE_MODE_FAST 0
#define MOVE_MODE_LINE 1
#define MOVE_MODE_CW_ARC 2
#define MOVE_MODE_CCW_ARC 3
#define MOVE_MODE_PROBE_TOWARD 4
#define MOVE_MODE_PROBE_TOWARD_NO_ERROR 5
#define MOVE_MODE_PROBE_AWAY 6
#define MOVE_MODE_PROBE_AWAY_NO_ERROR 7
#define MOVE_MODE_NONE 8
#define MOVE_MODE_STEPPER 9
#define MOVE_MODE_CANNED 10
#define MOVE_MODE_CANNED_DRILL 11
#define MOVE_MODE_CANNED_DRILLDWELL 12
#define MOVE_MODE_CANNED_PECKDRILL 13
#define MOVE_MODE_CANNED_RHTAP 14
#define MOVE_MODE_CANNED_BORE 15
#define MOVE_MODE_CANNEDP 16
#define MOVE_MODE_CANNED_BACKBORE 17
#define MOVE_MODE_CANNED_BOREPRET 18
#define MOVE_MODE_CANNED_BOREPSTOP 19
#define MOVE_MODE_SET_HOME 20

// NON MODAL CODES
#define NON_MODAL_NONE
#define NON_MODAL_DWELL 1
#define NON_MODAL_SET_COORD_DATA 2
#define NON_MODAL_GOHOME 3
#define NON_MODAL_SETHOME 4
#define NON_MODAL_RETRACT 5
#define NON_MODAL_UNRETRACT 6
#define NON_MODAL_CLEAN_TOOL 7
#define NON_MODAL_AUTOBEDLEVELING 8
#define NON_MODAL_SINGLE_PROBE_Z 9
#define NON_MODAL_CURRENT_PROBESTAT 10
#define NON_MODAL_DOCK_ZPROBE_SLED 11
#define NON_MODAL_PROBEZ_CALC_ZPLANE 12
#define NON_MODAL_ABSOLUTE_OVERRIDE 13
#define NON_MODAL_SAVECOORD_TO_SLOT 14
#define NON_MODAL_TOOLCHANGE 15
#define NON_MODAL_MISTCOOLANT_ON 16
#define NON_MODAL_FLOODCOOLANT_ON 17
#define NON_MODAL_COOLANT_OFF 18
#define NON_MODAL_VACUUM_ON 19
#define NON_MODAL_VACUUM_OFF 20
#define NON_MODAL_ENABLE_STEPPERS 21
#define NON_MODAL_DISABLE_STEPPERS 22
#define NON_MODAL_EJECT 23
#define NON_MODAL_STOP_IDLEHOLD 24
#define NON_MODAL_SET_INACT_SHUTDOWN_TIM 25
#define NON_MODAL_SET_AXIS_STEPS_PER_UNIT 26
#define NON_MODAL_FILAMENT_RETR_UNDO 27
#define NON_MODAL_FILAMENT_RETR 28
#define NON_MODAL_SET_EXTR_TEMP 29
#define NON_MODAL_GET_EXTR_TEMP 30
#define NON_MODAL_FAN_ON 31
#define NON_MODAL_FAN_OFF 32
#define NON_MODAL_HEATING_CANC 33
#define NON_MODAL_SET_EXTR_TEMP_WAIT 34
#define NON_MODAL_SET_CURRENT_LINE_NUM 35
#define NON_MODAL_SET_DBG_LVL 36
#define NON_MODAL_EMERGENCY_STOP 37
#define NON_MODAL_GET_CURRENT_POS 38
#define NON_MODAL_GET_FIRMWARE 39
#define NON_MODAL_WAIT 40
#define NON_MODAL_GET_ENDSTOPS_STATUS 41
#define NON_MODAL_ENABLE_ENDSTOPS_DETECTION 42
#define NON_MODAL_DISABLE_ENDSTOP_DETECTION 43
#define NON_MODAL_DIAGNOSE 44
#define NON_MODAL_TACHO_VAL 45
#define NON_MODAL_MOTOR_STOP 46
#define NON_MODAL_VALVE_OPEN 47
#define NON_MODAL_VALVE_CLOSE 48
#define NON_MODAL_EXTRUDER_PRESS_PWM 49
#define NON_MODAL_PID_SET_P 50
#define NON_MODAL_PID_SET_I 51
#define NON_MODAL_PID_SET_D 52
#define NON_MODAL_PID_SET_I_MAX 53
#define NON_MODAL_PID_WRITE 54
#define NON_MODAL_PID_SET_SAMPLE_INTV 55
#define NON_MODAL_PID_PRINT 56
#define NON_MODAL_SET_BED_TEMP_FAST 57
#define NON_MODAL_SET_CHAM_TEMP_FAST 58
#define NON_MODAL_BED_SET_PRESS 59
#define NON_MODAL_SET_MAX_HEATER_TEMP 60
#define NON_MODAL_BED_STDBY 61
#define NON_MODAL_CHAM_SET_HUM 62
#define NON_MODAL_SET_TEMP_UNITS 63
#define NON_MODAL_SET_DISP_COLOR 64

#define NON_MODAL_WAIT_BED_TARGET_TEMP 65
#define NON_MODAL_WAIT_CHAM_TARGET_TEMP 66
#define NON_MODAL_SET_FILAMENT_DIAMETER 67
#define NON_MODAL_SET_MAX_PRINT_ACCEL 68
#define NON_MODAL_SET_MAX_TRAVEL_ACCEL 69
#define NON_MODAL_SET_MAX_FEEDRATE 70
#define NON_MODAL_SET_DEFAULT_ACCEL 71

// DISTANCE MODE
#define DISTANCE_MODE_ABSOLUTE 0
#define DISTANCE_MODE_INCREMENTAL 1
// FEEDRATE MODE
#define FEEDRATE_MODE_INVERSE_TIME 0
#define FEEDRATE_MODE_MM_PER_MIN 1
// UNITS MODE
#define UNITS_MODE_MM 0
#define UNITS_MODE_IN 1
// CUTTER COMPENSATION
#define CUTTERCOMP_MODE_DISABLE 0;
#define CUTTERCOMP_MODE_ENABLE 1;
// PROGRAM FLOW
#define PROGRAM_FLOW_RUNNING 0
#define PROGRAM_FLOW_PAUSED 1
#define PROGRAM_FLOW_FINISHED 2
#define PROGRAM_FLOW_STOPPED 3
// PATH CONTROL
#define PATH_CTRL_EXACT 0
#define PATH_CTRL_EXACT_STOP 1
#define PATH_CTRL_CNTS 2
// SPINDLE CONTROL
#define SPINDLE_ON_CW 0
#define SPINDLE_ON_CCW 1
#define SPINDLE_OFF 2
// EXTRUDER MODE
#define EXTRUDER_MODE_INCREMENT 0
#define EXTRUDER_MODE_ABSOLUTE 1

// TODO: Add table of parameters

// gcode parser state
typedef enum { GCODE_PARSER_READY, };

// gcode name and function struct
typedef int(*g_func_t)(int argc, char *argv[]);
typedef struct {
	const char *name;
	g_func_t function;
} g_gcode;

typedef enum { XY, ZX, YZ} gplane_t;

typedef struct {
	char movemode; // Move mode
	char nonmodalcmd; // Misc command
	int number;
	int params_present_axis; // Axes present in command
	int params_present_misc; // Misc parameters present in command
	float_t xyz[3]; // XYZ coord
	float_t ijk[3]; // IJK coord
	float_t abc[3]; // ABC coord
	float_t uvw[3]; // UVW coord
	float_t r; // Arc radius
	float_t e; // Extrusion length
	float_t f; // Feedrate
	float_t s; // Spindle speed
	int h; // Tool length offset index
	int d; // Tool radius compensation number
	int n; // Line number
	int t; // Tool selected
	int l; // G10 or canned cycles parameter
	float_t p; // G10 or dwell parameter
	float_t q; // feed increment in G73, G83 canned cycles
	gplane_t plane;
	char feedratemode;
	char distancemode;
	char units;
	char cuttercompmode;
	char coordindex;
	char pathctrlmode;
	char coolant;
	char programflow;
	char spindle;
	char extrudermode;
} gcommand_t;

gcommand_t com;

// All the values begin with g?
int g_bSeenStart;
int g_linenumber;


int* g_modalgroupCount; // Table for counting modal groups
g_gcode* g_gcodestable;

void gcode_init();
gcommand_t gcode_parseline(char* line);
char *gcode_stripwhitespace(char* line);
int gcode_stripcomments(char* line, char* comments);
int gcode_parsecomments(char* comments);
float parse_expression(char* expr);
int gcodeParseCommand(gcommand_t cmd);

#endif /* MARENGO_GCODE_H_ */
