/*
 * gcode.h
 *
 *  Created on: 29 lip 2018
 *      Author: grzeg
 */

#ifndef MARENGO_GCODE_H_
#define MARENGO_GCODE_H_



// Arbitrary rules due to RS274/NGC language compliance
#define GCODE_MAXLINELENGTH 256 // Maximal line length
#define GCODE_WORDSTARTINGLETTERS "ABCDFGHIJKLMNPQRSTXYZ" // Allowed first letters of words
#define GCODE_POSSIBLEWORDS "ABCDFGHIJKLMNPQRSTXYZ"
#define GCODE_SIGNIFIG 17 // Significant figures in decimal numbers
#define GCODE_INTDECCRIT 0.0001 // Range (as part of integer) in which a decimal should be to be considered close to given decimal
//#define GCODE_PARAMPREFIX '#'
//#define GCODE_EXPRBRACKETS "[]"


#define GCODE_GMODALGROUP_1 1 // G0, G1, G2, G3, G38.2, G80, G81, G82, G83, G84, G85, G86, G87, G88, G89 motion
#define GCODE_GMODALGROUP_2 2 // G17, G18, G19 plane selection
#define GCODE_GMODALGROUP_3 3 // G90, G91 distance mode
#define GCODE_GMODALGROUP_5 5 // G93, G94 feed rate mode
#define GCODE_GMODALGROUP_6 6 // G20, G21 units
#define GCODE_GMODALGROUP_7 7 // G40, G41, G42 cutter radius compensation
#define GCODE_GMODALGROUP_8 8 // G43, G49 tool length offset
#define GCODE_GMODALGROUP_10 10 // G98, G99 return mode in canned cycles
#define GCODE_GMODALGROUP_12 12 // G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3 coordinate system selection
#define GCODE_GMODALGROUP_13 13 // G61, G61.1, G64 path control mode

#define GCODE_MMODALGROUP_4 4 // M0, M1, M2, M30, M60 stopping
#define GCODE_MMODALGROUP_6 6 // M6 tool change
#define GCODE_MMODALGROUP_7 7 // M3, M4, M5 spindle turning
#define GCODE_MMODALGROUP_8 8 // M7, M8, M9 coolant (special case M7 and M8 may be active at the same time)
#define GCODE_MMODALGROUP_9 9 // M48, M49 enable/disable feed and speed override switches

#define GCODE_NONMODALGROUP 0 // G4, G10, G28, G30, G53, G92, G92.1, G92.2, G92.3

// TODO: Implement error codes in parser

#define GCODE_ERROR_SAMEMODALGROUP 0 // Line contains two words from same modal group
#define GCODE_ERROR_UNKNOWNWORD 1 // Lie contains unknown word
#define GCODE_ERROR_GROUP01TGHT 2 // Line contains words from 1 and 0 groups


#define GCODE_WARNING_PARAMSREORDER // Same parameter is set twice in line


// All the values begin with g?

int g_bSeenStart;
int g_linenumber;


// Add table of parameters

void gcode_init(void);
int gcode_parseline(char* line);
char *gcode_stripwhitespace(char* line);
int gcode_stripcomments(char* line, char* comments);
int gcode_parsecomments(char* comments);
float parse_expression(char* expr);

#endif /* MARENGO_GCODE_H_ */
