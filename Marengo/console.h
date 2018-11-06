/*
 * console.h
 *
 *  Created on: 12 cze 2018
 *      Author: grzeg
 */

#include "ch.h"
#include "gfx.h"
#include <stdarg.h>
#include "chprintf.h"

#ifndef MARENGO_CONSOLE_H_
#define MARENGO_CONSOLE_H_

#define CONSOLE_MAX_LINE_LENGTH 64
#define CONSOLE_MAX_TOKENS 8
#define CONSOLE_TOKEN_DELIMITERS " \t\n\r\a"

#define CONSOLE_NEWLINE_STR "\r\n"
#define CONSOLE_PROMPT_STR ">"

typedef int (*ccmd_t)(int argc, char *argv[]);

typedef struct {
  const char    *ccmd_name; // Command name
  ccmd_t        ccmd_function; // Command function
  // TODO: Add help strings to each function
} ConsoleCmd;

struct{
  BaseSequentialStream *Stream; // ChibiOS stream acting as input
  GHandle Win; // uGFX handle to window acting as console
  const ConsoleCmd *cmds;
} MarengoConsoleConfig;

// Functions for console read and print handling
int consPrintf(const char *fmt, ...);
void consPutChar(char c);
bool consGetLine(char *line, unsigned size);

// Internal console functions
// int exec(ConsoleCmd *cmds, char *cmd, int argc, char **argv)
// int parse_line(char *line, char **tokens)

// Function for starting console
void MarengoStartConsole(void);


#endif /* MARENGO_CONSOLE_H_ */
