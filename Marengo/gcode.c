#include "gcode.h"
#include <ctype.h>
#include <string.h>
#include "console.h"

void gcode_init()
{
  g_bSeenStart=0;
  g_linenumber=0;
}


char *gcode_stripwhitespace(char* line)
{
  int i=0;
  if(line==NULL)
    return NULL;
  while(line[i]!=10 && line[i]!=13)
  {
    if(isspace(line[i]))
      for(int j=i; j<GCODE_MAXLINELENGTH;j++)
        line[j]=line[j+1];
    i++;
  }
  return line;
}
int gcode_stripcomments(char* line, char* comments)
{
  // TODO: Add nonstandard semicolon comments style support
  int beg=0;
  while(line[beg]!='(' && line[beg]!=';')
  {
    if(beg>GCODE_MAXLINELENGTH || line[beg]==10 || line[beg]==13)
      return 0; // No comments found! OK
    beg++;
  }
  int end;
  if(line[beg]==';')
    end=strlen(line);
  else
  {
  end=beg+1;
  while(line[end]!=')')
    {
      if(end>GCODE_MAXLINELENGTH || line[end]==10 || line[end]==13)
        return 1; // ERROR! Comment without ending!
      end++;
    }
  }
  int length = end-beg-1;
  memcpy(comments, line+beg+1, length); // copy the comment
  memset(comments+length+1, "\0", 1);
  for(int i=0; i+end<GCODE_MAXLINELENGTH;i++)
    line[beg+i]=line[end+1+i];
  return gcode_stripcomments(line, comments+length);
}
int gcode_parsecomments(char* comments)
{
  char key[]="MSG";
  if(!memcmp(comments, key, 3)){
    consPrintf(comments+3);
    consPrintf(CONSOLE_NEWLINE_STR);
    // TODO: Add a windows or smth better
  }
  return 0;
}

int gcode_parseline(char* line)
{
  if(line[0]=='%'){ //check for "%" file start character (add a condition to check for the '%' at the end)
    if(!g_bSeenStart)
      g_bSeenStart=1;
    else {
      // TODO: If it's the second '%' end program!
    }
    return 0;
  }
  char comments[256]; // split line of comments and commands
  if(gcode_stripcomments(line, comments)){
    consPrintf("Error. Bad comments structure!"CONSOLE_NEWLINE_STR);
    // TODO: Return something more adequate (in G code) and set interpreter to error state
  }
  gcode_stripwhitespace(line); // strip line of whitespaces
  //check for "/" blockend character
  for(int i=0; i<GCODE_MAXLINELENGTH; i++)
  {
    if(line[i]='/'){
      line[i]='\0';
      break;
    }
  }
  //check for line number
  int line_num;
  int bFoundN=0;
  for(int i=0; i<GCODE_MAXLINELENGTH; i++)
  {
    if(line[i]='N'){
      if(!bFoundN){
        bFoundN=1;
        int j=i+1;
        while(isdigit(line[i])) j++;
        if(j-i>5) consPrintf("Error line number over 5 digits!"CONSOLE_NEWLINE_STR); // Add smth more?
        char number[6];
        memcpy(number, line+i+1, j-i);
        line_num = atoi(number);
      }
      else {
        consPrintf("Error! Line number placed twice!"CONSOLE_NEWLINE_STR);
      }
    }
  }
  //check for any other words parameter setting or comments <-Not necessary?
  //    if there is a word check if first letter is valid
  for(int i=0; i<GCODE_MAXLINELENGTH && line[i]!=0; i++)
  {
    if(isalpha(line[i])&&strchr(GCODE_POSSIBLEWORDS, line[i])==NULL){
      consPrintf("Error! Invalid word found in the line!"CONSOLE_NEWLINE_STR);
    }
  }
  //    count commands in each modal group (do not allow two in one!)

  //        also check for commands using axis words

  //        suspend previous axis commands (?)

  //    check for '#' which gives parameter values

  //    check for expressions marked by '[]' brackets and perform them

  //    check for last comment - if it's "MSG..." display message

  //check for line end char (carriage return, line feed or both)
  // perform commands
  gcode_parsecomments(comments);
  // set parameters
}


float parse_expression(char* expr)
{
  //figure out calculating expression with recursive calling of this function
}

float atof(char* string)
{
  //write an atof with '.'
}
