/*
 * console.h
 *
 *  Created on: 12 cze 2018
 *      Author: grzeg
 */

#include "ch.h"
#include <chthreads.h>
#include "hal.h"
#include "gfx.h"
#include <stdarg.h>
#include "chprintf.h"

#ifndef MARENGO_CONSOLE_H_
#define MARENGO_CONSOLE_H_


#define consDebug(...) //


#define CONSOLE_MAX_LINE_LENGTH 64
#define CONSOLE_MAX_TOKENS 8
#define CONSOLE_TOKEN_DELIMITERS " \t\n\r\a"

#define CONSOLE_NEWLINE_STR "\r\n"
#define CONSOLE_PROMPT_STR ">"

#define CCMD_SUCCES (ccmd_t)0
#define CCMD_FAIL (ccmd_t)1

#define CONSOLE_HISTORY_SIZE 16

#define CONSOLE_STATUS_STOPPED
#define CONSOLE_STATUS_READY
#define CONSOLE_STATUS_WORKING

typedef int (*ccmd_t)(int argc, char *argv[]);

typedef struct {
  const char    *ccmd_name; // Command name
  ccmd_t        ccmd_function; // Command function
  const char    *ccmd_helpmsg;
  // TODO: Add help strings to each function
} ConsoleCmd;

struct{
  BaseAsynchronousChannel *Stream; // ChibiOS channel acting as input
  GHandle Win; // uGFX handle to window acting as console
  const ConsoleCmd *cmds;
} consConfig;

thread_t* consThread;

char *consHistory[CONSOLE_HISTORY_SIZE];
int consHistoryStart;
int consHistorySize;
char* consHistoryGetLast(void);
char* consHistoryGet(int n);
void consHistoryPut(char *line);


// Functions for console read and print handling
int consPrintf(const char *fmt, ...);
void consPutChar(char c);
bool consGetLine(char *line, unsigned size);

// Internal console functions
int consExec(ConsoleCmd *cmds, int argc, char **argv);
int consParseLine(char *line, char **tokens);
void consShowHelp(int argc, char **argv);

// Function for starting console
void consStart(void);

void consInit(void);
void consDeinit(void);


#endif /* MARENGO_CONSOLE_H_ */
