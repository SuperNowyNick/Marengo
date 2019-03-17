/*
 * console.c
 *
 *  Created on: 12 cze 2018
 *      Author: grzeg
 */

#include "console.h"
#include <string.h>

// TODO: Add console status
// ex console awaits command
// ex command execution
// for purpose of proper printf handling from other threads

// TODO: Add console history and scrolling feature?

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
    if(argc==1)
    {
      while(cmds->ccmd_name!=NULL){
        consPrintf("%s   ", cmds->ccmd_name);
        cmds++;
      }
      consPrintf(CONSOLE_NEWLINE_STR);
    }
    else if(argc>1)
    {
      while(cmds->ccmd_name!=NULL){
        if(!strcmp(cmds->ccmd_name, argv[1]))
        {
          consPrintf(cmds->ccmd_helpmsg);
          consPrintf(CONSOLE_NEWLINE_STR);
        }
        cmds++;
      }
    }
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
  int esc_value =0;
  char *oldline;
  int histn=0;
  while(true){
    char c;
    if (streamRead(consConfig.Stream, (uint8_t *)&c, 1) == 0)
      return true;
    if (c == 4) {
      consPrintf("^D");
      return true;
    }
    if((c==27)) // escape characters
    {
      if (streamRead(consConfig.Stream, (uint8_t *)&c, 1) == 0)
        return true;
      if(c=='[')
        if (streamRead(consConfig.Stream, (uint8_t *)&c, 1) == 0)
          return true;
      if(isdigit(c)){
        char val_buf[16];
        int i=0;
        do{
          val_buf[i] = c;
          i++;
        }
        while(streamRead(consConfig.Stream, (uint8_t *)&c, 1) != 0 && isdigit(c));
        esc_value = atoi(val_buf, 10);
      }
      // clear line
      for(;p>line; p--){
        consPutChar(0x08); // Added backspace support for gwinPutChar
        consPutChar(0x20);
        consPutChar(0x08);
      }
      switch(c)
      {
      case 'A': // arrow up
        histn++;
        if(histn>consHistorySize)
          histn=consHistorySize;
        oldline=consHistoryGet(histn);
        if(oldline==NULL)
          break;
        memcpy(line, oldline, strlen(oldline)+1);
        consPrintf(oldline);
        p+=strlen(oldline);
        break;
      case 'B': // arrow down
        histn--;
        if(histn<0)
          histn=0;
        oldline=consHistoryGet(histn);
        if(oldline==NULL)
          break;
        memcpy(line, oldline, strlen(oldline)+1);
        consPrintf(oldline);
        p+=strlen(oldline);
        break;
      default:
        consPrintf("Unknown escape sequence!");
        p+=strlen("Unknown escape sequence!");
        break;
      }
      continue;
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

char* consHistoryGetLast()
{
  if(consHistorySize>0)
    return consHistory[(consHistoryStart+consHistorySize)%CONSOLE_HISTORY_SIZE];
  else
    return NULL;
}

char* consHistoryGet(int n)
{
  if(n>consHistorySize || consHistorySize==0)
    return NULL;
  else
    return consHistory[(consHistoryStart+consHistorySize-n)%CONSOLE_HISTORY_SIZE];
}

void consHistoryPut(char *line){
  if(consHistorySize == CONSOLE_HISTORY_SIZE)
  {
    memcpy(consHistory[consHistoryStart], line, strlen(line)+1);
    consHistoryStart=(consHistoryStart+1)%CONSOLE_HISTORY_SIZE;
  }
  else
  {
    memcpy(consHistory[(consHistoryStart+consHistorySize)%CONSOLE_HISTORY_SIZE], line, strlen(line)+1);
    consHistorySize++;
  }
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
      // Add line to history
      consHistoryPut(line);
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
  for(int i=0; i<CONSOLE_HISTORY_SIZE; i++)
    free(consHistory[i]);
}

void consInit(void)
{
  consHistoryStart =0;
  consHistorySize =0 ;
  consThread = NULL;
  for(int i=0; i<CONSOLE_HISTORY_SIZE; i++)
    consHistory[i]=malloc(sizeof(char)*CONSOLE_MAX_LINE_LENGTH);
}
