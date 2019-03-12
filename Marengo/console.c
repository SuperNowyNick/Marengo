/*
 * console.c
 *
 *  Created on: 12 cze 2018
 *      Author: grzeg
 */

#include "console.h"
#include <string.h>


int consPrintf(const char *fmt, ...){
  va_list ap;
  int formatted_bytes;

  va_start(ap, fmt);
  formatted_bytes = chvprintf(consConfig.Stream, fmt, ap);
  gwinvPrintf(consConfig.Win, fmt, ap); // This function has been added to gwin_console.c
  va_end(ap);

  return formatted_bytes;
}

void consPutChar(char c){
  streamPut(consConfig.Stream, c);
  gwinPutChar(consConfig.Win, c);
}

int consExec(ConsoleCmd *cmds, int argc, char **argv)
{
  if(!strcmp(argv[0], "help")){
    if(argc=1)
    {
      while(cmds->ccmd_name!=NULL){
        consPrintf("%s   ", cmds->ccmd_name);
        cmds++;
      }
      consPrintf(CONSOLE_NEWLINE_STR);
    }
    // else TODO: Add help strings for each function!
    return 0;
  }
  while(cmds->ccmd_name!=NULL)
  {
    if(!strcmp(argv[0], cmds->ccmd_name)){
      cmds->ccmd_function(argc, argv);
      return 0;
    }
    cmds++;
  }
  return 1;
}

bool consGetLine(char *line, unsigned size){
  char *p = line;
  while(true){
    char c;
    if (streamRead(consConfig.Stream, (uint8_t *)&c, 1) == 0)
      return true;
    if (c == 4) {
      consPrintf("^D");
      return true;
    }
    if((c=="[")) // escape characters
    {
      if (streamRead(consConfig.Stream, (uint8_t *)&c, 1) == 0)
        return true;
      if(c=="A") // arrow up
        // should move to beggining of the line
        consPrintf("Stara linia"CONSOLE_NEWLINE_STR);
      if(c=="B") // arrow down
        consPrintf("Nowa linia"CONSOLE_NEWLINE_STR);
        // should move to beggining of the line
    }
    if ((c == 8) || (c == 127)) { // backspace or delete?
      if (p != line) {
        consPutChar(0x08); // Added backspace support for gwinPutChar
        consPutChar(0x20);
        consPutChar(0x08);
        p--;
      }
      continue;
    }
    if (c == '\r') { // enter?
      consPrintf(CONSOLE_NEWLINE_STR);
      *p = 0;
      return false;
    }
    if (c < 0x20)
      continue;
    if (p < line + size - 1) {
      consPutChar(c);
      *p++ = (char)c;
    }
  }
}

// Line parsing functions
int consParseLine(char *line, char **tokens)
{
  if(!line||line==NULL)
    return 1;
  int position=0;
  char *token;
  token=strtok(line, CONSOLE_TOKEN_DELIMITERS);
  // TODO: Add tokens overflow prevention?!
  while(token!=NULL)
  {
    tokens[position]=token;
    position++;
    if(position>=CONSOLE_MAX_TOKENS)
    {
      consPrintf("Error. Too many arguments. Maximum number allowed is %i"CONSOLE_NEWLINE_STR, CONSOLE_MAX_TOKENS);
      return 1;
    }
    token = strtok(NULL, CONSOLE_TOKEN_DELIMITERS);
  }
  tokens[position]=NULL;
  return 0;
}

/* ---------------------------------------------------------------------- */
// CONSOLE PRINTING THREAD
static THD_WORKING_AREA(waConsoleThread, 2048);
static THD_FUNCTION(ConsoleThread, arg) {
  (void)arg;
  chRegSetThreadName("MarengoConsole");

  consPrintf("\r\nMarengo Console\r\n");

  char line[CONSOLE_MAX_LINE_LENGTH];

  while (1) {
    consPrintf(CONSOLE_PROMPT_STR);
    if(consGetLine(line, sizeof(line))){
      consPrintf(CONSOLE_NEWLINE_STR);
      consPrintf("Marengo console exit");
      break;
    }
    else if(line!=0)
    {
      // line processing goes here!
      char *tokens[CONSOLE_MAX_TOKENS+1];
      consParseLine(line, tokens);
      if(tokens[0]!=NULL){
        int argc=0;
        while(tokens[argc]!=NULL){
          argc++;
        }
        if(consExec(consConfig.cmds, argc, tokens))
          consPrintf("No such function %s"CONSOLE_NEWLINE_STR, tokens[0]);
      }
    }
    chThdSleepMilliseconds(100);
  }
  chThdExitS(MSG_OK);
}

void consStart(void)
{
  if(consThread == NULL)
    consThread = chThdCreateStatic(waConsoleThread, sizeof(waConsoleThread),
                    NORMALPRIO + 10, ConsoleThread, NULL);
}

void consDeinit(void)
{
  consPrintf("Exiting console"CONSOLE_NEWLINE_STR);
  chThdTerminate(consThread);
}

void consInit(void)
{
  //consHistory = calloc(CONSOLE_HISTORY_SIZE, sizeof(char)*CONSOLE_MAX_LINE_LENGTH);
  consThread = NULL;
}
