#include "gcode.h"
#include <ctype.h>
#include <string.h>
#include "console.h"
#include "stdlib.h"
#include "heater.h"
#include "stepper.h"

void gcode_init()
{
  g_bSeenStart = 0;
  g_linenumber = 0;
  g_modalgroupCount = (int*)calloc(15, sizeof(int));
  if (g_modalgroupCount == NULL) consPrintf("Cannot allocate g_modalgroupCount");
  for(int i=0; i<3;i++)
    com.abc[i] = (float_t){0,0,0,0};
  com.coolant = FALSE;
  com.coordindex = 0;
  com.cuttercompmode = CUTTERCOMP_MODE_DISABLE;
  com.d=0;
  com.distancemode=DISTANCE_MODE_ABSOLUTE; // TODO: Add function stpGetDistanceMode()
  com.e = (float_t){0,0,0,0};
  com.extrudermode =EXTRUDER_MODE_ABSOLUTE;
  com.f = (float_t){0,0,0,0};
  com.feedratemode=FEEDRATE_MODE_MM_PER_MIN;
  com.h =0;
  for(int i=0; i<3;i++)
    com.ijk[i] = (float_t){0,0,0,0};
  com.l=0;
  com.movemode = MOVE_MODE_NONE;
  com.n =0;
  com.nonmodalcmd = 0;
  com.number =0;
  com.p = (float_t){0,0,0,0};
  com.params_present_axis = 0;
  com.params_present_misc = 0;
  com.pathctrlmode = PATH_CTRL_EXACT;
  com.plane = XY;
  com.programflow = PROGRAM_FLOW_RUNNING;
  com.q = (float_t){0,0,0,0};
  com.r = (float_t){0,0,0,0};
  com.s = (float_t){0,0,0,0};
  com.spindle=SPINDLE_OFF;
  com.t = 0;
  com.units = UNITS_MODE_MM;
  for(int i=0; i<3;i++)
    com.uvw[i] = (float_t){0,0,0,0};
  for(int i=0; i<3;i++)
    com.xyz[i] = (float_t){0,0,0,0};
}

// Lexing functions
// Remove all whitespaces from line
char *gcode_stripwhitespace(char* line)
{
  if (line == NULL)
    return NULL;
  int i = 0;
  while (line[i] != 10 && line[i] != 13 && line[i] != 0)
  {
    if (isspace(line[i]))
      for (int j = i; j < GCODE_MAXLINELENGTH; j++)
        line[j] = line[j + 1];
    i++;
  }
  return line;
}
// Strip comments from line in () brackets and after ; and write them to given address
int gcode_stripcomments(char* line, char* comments)
{
  // TODO: Add nonstandard semicolon comments style support
  int beg = 0;
  while (line[beg] != '(' && line[beg] != ';')
  {
    if (beg > GCODE_MAXLINELENGTH || line[beg] == 10 || line[beg] == 13 || line[beg] == 0)
      return 0; // No comments found! OK
    beg++;
  }
  int end;
  if (line[beg] == ';')
    end = strlen(line);
  else
  {
    end = beg + 1;
    while (line[end] != ')')
    {
      if (end > GCODE_MAXLINELENGTH || line[end] == 10 || line[end] == 13)
        return 1; // ERROR! Comment without ending!
      end++;
    }
  }
  int length = end - beg - 1;
  memcpy(comments, line + beg + 1, length); // copy the comment
  memset(comments + length, 0, 1);
  if (line[beg] == ';') {
    memset(line + beg, 0, 1);
    return 0;
  }
  for (int i = 0; i + end < GCODE_MAXLINELENGTH; i++)
    line[beg + i] = line[end + 1 + i];
  return gcode_stripcomments(line, comments + length);
}
// Parse the comments for MSG command
int gcode_parsecomments(char* comments)
{
  char key[] = "MSG";
  if (!memcmp(comments, key, 3)) {
    consPrintf(comments + 3);
    consPrintf(CONSOLE_NEWLINE_STR);
    // TODO: Add a window or smth better
  }
  return 0;
}

// Parse the line
gcommand_t gcode_parseline(char* line)
{
  com.nonmodalcmd = 0; // Clear non modal code
  com.params_present_axis = 0; // Clear param presence flags
  com.params_present_misc = 0;
  if(com.distancemode == DISTANCE_MODE_INCREMENTAL){
    com.xyz[0] = (float_t){0,0,0,0};
    com.xyz[1] = (float_t){0,0,0,0};
    com.xyz[2] = (float_t){0,0,0,0};
    com.e= (float_t){0,0,0,0};
  }
  char newline[GCODE_MAXLINELENGTH];
  memcpy(newline, line, GCODE_MAXLINELENGTH);
  if (line[0] == '%') { //check for "%" file start character (add a condition to check for the '%' at the end)
    if (!g_bSeenStart)
      g_bSeenStart = 1;
    else {
      // TODO: If it's the second '%' end program!
      consPrintf("End of file\n");
      com.programflow = PROGRAM_FLOW_STOPPED;
      return com;
    }
    return com;
  }
  char comments[256]; // split line of comments and commands
  if (gcode_stripcomments(line, comments)) {
    consPrintf("Error. Bad comments structure!"CONSOLE_NEWLINE_STR);
    // TODO: Return something more adequate (in G code) and set interpreter to error state
    com.programflow = PROGRAM_FLOW_STOPPED;
    return com;
  }
  gcode_stripwhitespace(line); // strip line of whitespaces

  // check for "/" blockend character
  // Exchange the '/' char marking the beggining of comments to end of line
  for (int i = 0; i < GCODE_MAXLINELENGTH; i++)
  {
    if (line[i] == '/') {
      line[i] = '\0';
      break;
    }
  }
  int line_len = strlen(line);
  if (line_len > GCODE_MAXLINELENGTH) {
    consPrintf("Error! Line length over 256!"CONSOLE_NEWLINE_STR);
    // TODO: Set interpreter to error state
    com.programflow = PROGRAM_FLOW_STOPPED;
    return com;
  }

  int line_num = 0;
  int bFoundN = 0;
  int modal_reg = 0;
  int checksum = 0;
  // Scan the line
  for (int i = 0; i < line_len || line[i] != 0; i++)
  {
    // Check if first letter is a leglible word
    if (strchr(GCODE_POSSIBLEWORDS, line[i]) == NULL) {
      consPrintf("Error! Invalid word found in the line: %s"CONSOLE_NEWLINE_STR, line);
      // TODO: Change interpreter to error state
      com.programflow = PROGRAM_FLOW_STOPPED;
      return com;
    }
    else {
      // If it is legible then scan for number after it
      int j = i + 1;
      char* num;
      num = (char*)calloc(256, sizeof(char)); // TODO: Calloc won't work in ChibiOS
      j = i + 1;
      while (isdigit(line[j]) || line[j] == '.' || line[j] == '-' || line[j] == '+') j++;
      memcpy(num, line + i + 1, j - i - 1);
      // TODO: Should we add 0 at the end of buffer?
      // TODO: Add support for paramters and expressions
      float_t num_f = myatof(num);
      switch (line[i]) {
      case 'N': //check for line number
        if (!bFoundN) {
          bFoundN = 1;
          if (strlen(num) > 5) consPrintf("Error line number over 5 digits!"CONSOLE_NEWLINE_STR); // Add smth more?
          com.l = num_f.character; // line number
          consDebug("Line number: %i\n", com.l);
        }
        else {
          consPrintf("Error! Line number placed twice!"CONSOLE_NEWLINE_STR);
        }
        break;
      case 'G': case 'M':
        // G-code with max of 3 digits
        consDebug("Executing: %c%s ", line[i], num);
        // check modal groups
        static const char *modalgroup_codes[16] = {
                                                   "G0G1G2G3G6G38.2G80G81G82G83G84G85G86G87G88G89", // G Modal group 1
                                                   "G17G18G19", // G Modal group 2
                                                   "G90G91", // G Modal group 3
                                                   "G93G94", // G Modal group 5
                                                   "G20G21", // G Modal group 6
                                                   "G40G41G42", // G Modal group 7
                                                   "G43G49", // G Modal group 8
                                                   "G98G99", // G Modal group 10
                                                   "G54G55G56G57G58G59G59.1G59.2G59.3", // G Modal group 12
                                                   "G61G61.1G64", // G Modal group 13
                                                   "M0M1M2M30M60", // M Modal group 4
                                                   "M6", // M Modal group 6
                                                   "M3M4M5", // M Modal group 7
                                                   "M7M8M9", // M Modal group 8
                                                   "M48M49", // M Modal group 9
                                                   "G4G10G28G30G53G92G92.1G92.2G92.3", // Non-modal group
        };
        char code[6];
        memcpy(code, line + i, 1);
        memcpy(code + 1, num, strlen(num));
        code[strlen(num) + 1] = 0;
        for (int m = 0; m < 16; m++) {
          char *modalgroup = modalgroup_codes[m];
          do {
            modalgroup = strstr(modalgroup, code);
            if (modalgroup != NULL) modalgroup += strlen(code);
          } while (modalgroup!=NULL && (isdigit(modalgroup[0]) || modalgroup[0]=='.'));
          if (modalgroup != NULL) {
            if (modal_reg & (1 << m)) {
              consPrintf("Error! Modal group violation!\n");
              // TODO: Set interpreter to error state
              //return 1;
            }
            else
              modal_reg |= (1 << m);
          }
        }
        if (line[i] == 'G') {

          switch (num_f.character)
          {
          case 0: com.movemode = MOVE_MODE_FAST; break;
          case 1: com.movemode = MOVE_MODE_LINE; break;
          case 2: com.movemode = MOVE_MODE_CW_ARC; break;
          case 3: com.movemode = MOVE_MODE_CCW_ARC; break;
          case 4: com.nonmodalcmd = NON_MODAL_DWELL; break; // Dwell
          case 6: com.movemode = MOVE_MODE_STEPPER; break;
          case 10: com.nonmodalcmd = NON_MODAL_SET_COORD_DATA; break; // coordinate system origin setting or retraction of filament
          case 11: com.nonmodalcmd = NON_MODAL_UNRETRACT; break;
          case 12: com.nonmodalcmd = NON_MODAL_CLEAN_TOOL; break;
          case 17: com.plane = XY; break;
          case 18: com.plane = ZX; break;
          case 19: com.plane = YZ; break;
          case 20: com.units = UNITS_MODE_IN; break;
          case 21: com.units = UNITS_MODE_MM; break;
          //case 22: break; // Firmware controlled retract
          //case 23: break; // Firmware controlled unretract/precharge
          //case 26: break; // Mesh validation pattern
          case 28: com.nonmodalcmd = NON_MODAL_GOHOME; break; // Move to origin
          case 29: com.nonmodalcmd = NON_MODAL_AUTOBEDLEVELING; break; // TODO: Add 29.1 and 29.2 support
          case 30: com.nonmodalcmd = NON_MODAL_SINGLE_PROBE_Z; break;
          case 31: com.nonmodalcmd = NON_MODAL_CURRENT_PROBESTAT; break;
          //case 32: com.nonmodalcmd = NON_MODAL_DOCK_ZPROBE_SLED; break; // Dock Z probe sled
          //case 32: com.nonmodalcmd = NON_MODAL_PROBEZ_CALC_ZPLANE; break;
          //case 33: break; // firmware dependent
          //case 34: break; // Set Delta Height calculated from toolhead position (only DELTA)
          case 38:
            switch (num_f.mantisa)
            {
            case 2: com.movemode = MOVE_MODE_PROBE_TOWARD; break;
            case 3: com.movemode = MOVE_MODE_PROBE_TOWARD_NO_ERROR; break;
            case 4: com.movemode = MOVE_MODE_PROBE_AWAY; break;
            case 5: com.movemode = MOVE_MODE_PROBE_AWAY_NO_ERROR; break;
            default:
              consPrintf("Error. Unknown code G%s\n", num);
              break;
            }
            break;
            case 40: com.cuttercompmode = CUTTERCOMP_MODE_DISABLE; break; // Cutter compensation mode
            //case 42: break; // Move to grid point (Bed leveling cmd)
            case 53: com.nonmodalcmd = NON_MODAL_ABSOLUTE_OVERRIDE; break;
            case 54: case 55: case 56: case 57: case 58:
              com.coordindex = num_f.mantisa; break;
            case 60: com.nonmodalcmd = NON_MODAL_SAVECOORD_TO_SLOT; break;
            case 61: // In 3D Printer mode it should be apply/restore saved coords to active extruder
              switch (num_f.mantisa)
              {
              case 0: com.pathctrlmode = PATH_CTRL_EXACT; break;// Set path control mode exact path
              case 1: com.pathctrlmode = PATH_CTRL_EXACT_STOP; break;
              default:
                consPrintf("Error. Unknown code G%s\n", num);
                break;
              }
              break;
              case 64: com.pathctrlmode = PATH_CTRL_CNTS; break;// Set path control mode
              // TODO: Add some detail work to canned cycles listed below
              case 80: com.movemode = MOVE_MODE_CANNED;  break; // Canned cycle
              case 81: com.movemode = MOVE_MODE_CANNED_DRILL;  break; // Canned cycle drilling
              case 82: com.movemode = MOVE_MODE_CANNED_DRILLDWELL; break; // Canned cycle drilling with dwell
              case 83: com.movemode = MOVE_MODE_CANNED_PECKDRILL; break; // Canned cycle peck drilling
              case 84: com.movemode = MOVE_MODE_CANNED_RHTAP; break; // Canned cycle right hand tapping
              case 85: com.movemode = MOVE_MODE_CANNED_BORE; break; // Canned cycle boring, no dwell, feed out
              case 86: com.movemode = MOVE_MODE_CANNEDP; break; // Canned cycle with P param
              case 87: com.movemode = MOVE_MODE_CANNED_BACKBORE; break; // Canned cycle back boring
              case 88: com.movemode = MOVE_MODE_CANNED_BOREPRET; break; // Canned boring with P param and return
              case 89: com.movemode = MOVE_MODE_CANNED_BOREPSTOP; break; // Canned boring with P param and spindle stop
              case 90: com.distancemode = DISTANCE_MODE_ABSOLUTE;
                       com.movemode = MOVE_MODE_NONE; break;
              case 91: com.distancemode = DISTANCE_MODE_INCREMENTAL;
                       com.movemode = MOVE_MODE_NONE; break;
              case 92: com.movemode = MOVE_MODE_SET_HOME; break;
              case 93: com.feedratemode = FEEDRATE_MODE_INVERSE_TIME; break;
              case 94: com.feedratemode = FEEDRATE_MODE_MM_PER_MIN; break;
              default:
                consPrintf("Error. Unknown G code G%s\n", num);
                com.programflow = PROGRAM_FLOW_STOPPED;
                return com;
                break;
          }
        }
        if (line[i] == 'M') {
          switch (num_f.character)
          {
          case 0: com.programflow = PROGRAM_FLOW_STOPPED; break; // TODO: Chenge it in standalone interpreter?
          case 1: com.programflow = PROGRAM_FLOW_PAUSED; break;
          case 2: case 30: case 60: com.programflow = PROGRAM_FLOW_FINISHED; break;
          case 3: com.spindle = SPINDLE_ON_CW; break;
          case 4: com.spindle = SPINDLE_ON_CCW; break;
          case 5: com.spindle = SPINDLE_OFF; break;
          case 6: com.nonmodalcmd = NON_MODAL_TOOLCHANGE; break;
          case 7: com.nonmodalcmd = NON_MODAL_MISTCOOLANT_ON; break;
          case 8: com.nonmodalcmd = NON_MODAL_FLOODCOOLANT_ON; break;
          case 9: com.nonmodalcmd = NON_MODAL_COOLANT_OFF; break;
          case 10: com.nonmodalcmd = NON_MODAL_VACUUM_ON; break;
          case 11: com.nonmodalcmd = NON_MODAL_VACUUM_OFF; break;
          case 17: com.nonmodalcmd = NON_MODAL_ENABLE_STEPPERS; break;
          case 18: com.nonmodalcmd = NON_MODAL_DISABLE_STEPPERS; break;
          // M-Codes below are not supported due to no SD card reader
          // M20 list SD cards
          // M21 initialize SD card
          // M22 release SD card
          // M23 select SD file
          // M24 start/resume SD print
          // M25 pause SD print
          // M26 set SD position
          // M27 report SD print status
          // M28 begin write to SD card
          // M29 stop writing to SD card
          // M30 delete a file on SD card. In normal CNC mode it should be program end
          // M31 output time since last M109 or SD card start to serial
          // M32 select file and start SD print
          // M33 get long name for an SD card file or folder
          // M34 Set SD file sorting options
          // M36 Return file information
          // M37 Simulation mode
          // M38 Compute SHA1 hash of target file
          // M39 Report SD card information
          case 40: com.nonmodalcmd = NON_MODAL_EJECT; break;
          // M42 Switch I/O Pin - not applicable to our board
          // M43 Stand by on material exhausted / Pin report and debug
          // M48 Measure Z-Probe repeatability
          // M80 Turn power supply on
          // M81 Turn power supply off
          case 82: com.extrudermode = EXTRUDER_MODE_ABSOLUTE; break;
          case 83: com.extrudermode = EXTRUDER_MODE_INCREMENT; break;
          case 84: com.nonmodalcmd = NON_MODAL_STOP_IDLEHOLD; break;
          case 85: com.nonmodalcmd = NON_MODAL_SET_INACT_SHUTDOWN_TIM; break;
          case 92: case 93: com.nonmodalcmd = NON_MODAL_SET_AXIS_STEPS_PER_UNIT; break;
          case 101: com.nonmodalcmd = NON_MODAL_FILAMENT_RETR_UNDO; break;
          case 103: com.nonmodalcmd = NON_MODAL_FILAMENT_RETR; break;
          case 104: com.nonmodalcmd = NON_MODAL_SET_EXTR_TEMP; break;
          case 105: com.nonmodalcmd = NON_MODAL_GET_EXTR_TEMP; break;
          case 106: com.nonmodalcmd = NON_MODAL_FAN_ON; break;
          case 107: com.nonmodalcmd = NON_MODAL_FAN_OFF; break;
          case 108: com.nonmodalcmd = NON_MODAL_HEATING_CANC; break;
          case 109: com.nonmodalcmd = NON_MODAL_SET_EXTR_TEMP_WAIT; break;
          case 110: com.nonmodalcmd = NON_MODAL_SET_CURRENT_LINE_NUM; break;
          case 111: com.nonmodalcmd = NON_MODAL_SET_DBG_LVL; break;
          case 112: com.nonmodalcmd = NON_MODAL_EMERGENCY_STOP; break;
          case 114: com.nonmodalcmd = NON_MODAL_GET_CURRENT_POS; break;
          case 115: com.nonmodalcmd = NON_MODAL_GET_FIRMWARE; break;
          case 116: com.nonmodalcmd = NON_MODAL_WAIT; break;
          // M117 Show msg on lcd screen
          // M118 Show msg on host
          case 119: com.nonmodalcmd = NON_MODAL_GET_ENDSTOPS_STATUS; break;
          case 120: com.nonmodalcmd = NON_MODAL_ENABLE_ENDSTOPS_DETECTION; break;
          case 121: com.nonmodalcmd = NON_MODAL_DISABLE_ENDSTOP_DETECTION; break;
          //case 122: com.nonmodalcmd = NON_MODAL_DIAGNOSE; break;
          //case 123: com.nonmodalcmd = NON_MODAL_TACHO_VAL; break;
          case 124: com.nonmodalcmd = NON_MODAL_MOTOR_STOP; break;
          //case 126: com.nonmodalcmd = NON_MODAL_VALVE_OPEN; break;
          //case 127: com.nonmodalcmd = NON_MODAL_VALVE_CLOSE; break;
          //case 128: com.nonmodalcmd = NON_MODAL_EXTRUDER_PRESS_PWM; break;
          //case 129: com.nonmodalcmd = NON_MODAL_EXTRUDER_PRESS_OFF; break;
          case 130: com.nonmodalcmd = NON_MODAL_PID_SET_P; break;
          case 131: com.nonmodalcmd = NON_MODAL_PID_SET_I; break;
          case 132: com.nonmodalcmd = NON_MODAL_PID_SET_D; break;
          case 133: com.nonmodalcmd = NON_MODAL_PID_SET_I_MAX; break;
          case 134: com.nonmodalcmd = NON_MODAL_PID_WRITE; break;
          case 135: com.nonmodalcmd = NON_MODAL_PID_SET_SAMPLE_INTV; break;
          case 136: com.nonmodalcmd = NON_MODAL_PID_PRINT; break;
          case 140: com.nonmodalcmd = NON_MODAL_SET_BED_TEMP_FAST; break;
          //case 141: com.nonmodalcmd = NON_MODAL_SET_CHAM_TEMP_FAST; break;
          case 142: com.nonmodalcmd = NON_MODAL_BED_SET_PRESS; break;
          case 143: com.nonmodalcmd = NON_MODAL_SET_MAX_HEATER_TEMP; break;
          case 144: com.nonmodalcmd = NON_MODAL_BED_STDBY; break;
          case 146: com.nonmodalcmd = NON_MODAL_CHAM_SET_HUM; break;
          case 149: com.nonmodalcmd = NON_MODAL_SET_TEMP_UNITS; break;
          case 150: com.nonmodalcmd = NON_MODAL_SET_DISP_COLOR; break;
          case 155:
          case 160:
          case 163:
          case 164:
          case 165:
          case 170:
          case 190: com.nonmodalcmd = NON_MODAL_WAIT_BED_TARGET_TEMP; break;
          //case 191: com.nonmodalcmd = NON_MODAL_WAIT_CHAM_TARGET_TEMP; break;
          case 200: com.nonmodalcmd = NON_MODAL_SET_FILAMENT_DIAMETER; break;
          case 201: com.nonmodalcmd = NON_MODAL_SET_MAX_PRINT_ACCEL; break;
          case 202: com.nonmodalcmd = NON_MODAL_SET_MAX_TRAVEL_ACCEL; break;
          case 203: com.nonmodalcmd = NON_MODAL_SET_MAX_FEEDRATE; break;
          case 204: com.nonmodalcmd = NON_MODAL_SET_DEFAULT_ACCEL; break;
          // TODO: Add the rest of codes from https://reprap.org/wiki/G-code
          default:
            consPrintf("Error. Unknown M code G%s\n", num);
            break;
          }
        }
        break;
      case 'X': com.params_present_axis |= PARAM_AXIS_X; com.xyz[0] = num_f; break;
      case 'Y': com.params_present_axis |= PARAM_AXIS_Y; com.xyz[1] = num_f; break;
      case 'Z': com.params_present_axis |= PARAM_AXIS_Z; com.xyz[2] = num_f; break;
      case 'A': com.params_present_axis |= PARAM_AXIS_A; com.abc[0] = num_f; break;
      case 'B': com.params_present_axis |= PARAM_AXIS_B; com.abc[1] = num_f; break;
      case 'C': com.params_present_axis |= PARAM_AXIS_C; com.abc[2] = num_f; break;
      case 'E': com.params_present_misc |= PARAM_MISC_E; com.e = num_f; break;
      case 'F': com.params_present_misc |= PARAM_MISC_F; com.f = num_f; break;
      case 'I': com.params_present_axis |= PARAM_AXIS_I; com.ijk[0] = num_f; break;
      case 'J': com.params_present_axis |= PARAM_AXIS_J; com.ijk[1] = num_f; break;
      case 'K': com.params_present_axis |= PARAM_AXIS_K; com.ijk[2] = num_f; break;
      case 'R': com.params_present_misc |= PARAM_MISC_R; com.r = num_f; break;
      case 'U': com.params_present_axis |= PARAM_AXIS_U; com.uvw[0] = num_f; break;
      case 'V': com.params_present_axis |= PARAM_AXIS_V; com.uvw[1] = num_f; break;
      case 'W': com.params_present_axis |= PARAM_AXIS_W; com.uvw[2] = num_f; break;
      case 'T': com.params_present_misc |= PARAM_MISC_T; com.t = num_f.character; break;
      case 'P': com.params_present_misc |= PARAM_MISC_P; com.p = num_f; break;
      case 'D': com.params_present_misc |= PARAM_MISC_D; com.d = num_f.character; break;
      case 'S': com.params_present_misc |= PARAM_MISC_S; com.s = num_f; break;
      // If in 3D printer mode works same as 'H'
      // It might also serve as dwell time in seconds
      // Otherwise it should be spindle speed
      case 'H': com.params_present_misc |= PARAM_MISC_H; com.h = num_f.character; break;
      // TODO: If in 3D printer mode check if the endstop was hit
      // Otherwise it should be tool length offset index
      case 'Q': com.params_present_misc |= PARAM_MISC_Q; com.q = num_f; break;
      case '*': // Checksum calculation
        for (int ii = 0; newline[ii]!='*'; ii++)
          checksum^=newline[ii];
        if (checksum != num_f.character) {
          // TODO: Set printer to error mode
          consPrintf("Incorrect checksum. Local checksum: %d!\n", checksum);
          com.programflow = PROGRAM_FLOW_STOPPED;
          return com;
        }
        break;
      default:
        consPrintf("Unsupported word: %c, on line!\n", line[i]);
        // TODO: Set interpreter to error mode
        com.programflow = PROGRAM_FLOW_STOPPED;
        return com;
        break;
      }
      free(num);
      i = j - 1;
    }
  }
  //check for line end char (carriage return, line feed or both)
  // perform commands
  gcode_parsecomments(comments);
  // set parameters
  return com;
}


float parse_expression(char* expr)
{
  // TODO: figure out calculating expression with recursive calling of this function
}

int gcodeParseCommand(gcommand_t cmd)
{
  stpCoordF_t coord, mov;
  stpCoord_t steps;
  stpModeInc = com.distancemode; // TODO: Should be written more flexibly?
  stpSetFeedRate(com.f.character);
  // Check non modal commands
  switch (cmd.nonmodalcmd) {
  case NON_MODAL_DWELL: // G4
    if (cmd.params_present_misc & PARAM_MISC_P)
      chThdSleepMilliseconds(cmd.p.character);
    else if (cmd.params_present_misc & PARAM_MISC_S)
      chThdSleepMicroseconds(cmd.s.character);
    else{
      consPrintf("error param s or p not present"CONSOLE_NEWLINE_STR);
      return 1;
    }
    break;
  case NON_MODAL_GET_EXTR_TEMP: //M105
    consPrintf("ok T:%d B:%d"CONSOLE_NEWLINE_STR, heaterGetTemp(&Heater1), heaterGetTemp(&Heater2));
    return 0;
    break;
  case NON_MODAL_SET_EXTR_TEMP: // M104
    if(cmd.params_present_misc & PARAM_MISC_S)
      heaterSetTemp(&Heater1, cmd.s.character);
    else{
      consPrintf("error param s not present"CONSOLE_NEWLINE_STR);
      return 1;
    }
    break;
  case NON_MODAL_WAIT_BED_TARGET_TEMP: // M190
    if(cmd.params_present_misc & PARAM_MISC_S)
      heaterSetTemp(&Heater2, cmd.s.character);
    else{
      consPrintf("error param s not present"CONSOLE_NEWLINE_STR);
      return 1;
    }
    while(heaterGetTemp(&Heater2)<cmd.s.character)
      chThdSleepMilliseconds(1000);
    break;
  case NON_MODAL_SET_EXTR_TEMP_WAIT: // M109
    if(cmd.params_present_misc & PARAM_MISC_S)
      heaterSetTemp(&Heater1, cmd.s.character);
    else{
      consPrintf("error param s not present"CONSOLE_NEWLINE_STR);
      return 1;
    }
    while(heaterGetTemp(&Heater1)<cmd.s.character)
      chThdSleepMilliseconds(1000);
    break;
  case NON_MODAL_SET_BED_TEMP_FAST: // M140
    if(cmd.params_present_misc & PARAM_MISC_S)
      heaterSetTemp(&Heater2, cmd.s.character);
    else{
      consPrintf("error param s not present"CONSOLE_NEWLINE_STR);
      return 1;
    }
    break;
  case NON_MODAL_GET_CURRENT_POS: // M114
    coord = stpGetCoordF();
    consPrintf("ok C: X:");
    printFloat(coord.x);
    consPrintf(" Y:");
    printFloat(coord.y);
    consPrintf(" Z:");
    printFloat(coord.z);
    consPrintf(" E:");
    printFloat(coord.stpE);
    consPrintf(CONSOLE_NEWLINE_STR);
    return 0;
    break;
  case NON_MODAL_GOHOME: // G28
    coord = stpGetCoordF();
    if(cmd.params_present_axis == 0) // TODO: Add ABC axis support
      mov = stpCoordFZero();
    else
      mov = stpGetCoordF();
    if(cmd.params_present_axis & PARAM_AXIS_X)
      mov.x = fzero();
    if(cmd.params_present_axis & PARAM_AXIS_Y)
      mov.y = fzero();
    if(cmd.params_present_axis & PARAM_AXIS_Z)
      mov.z = fzero();
    mov = stpCoordFSub(mov, coord);
    mov.stpE=fzero();
    stpMoveLinearInit(mov);
    break;
  default:
    break;
  }
  if(cmd.nonmodalcmd !=0){
    consPrintf("ok"CONSOLE_NEWLINE_STR);
    cmd.nonmodalcmd =0;
    return 0;
  }
  // Process modal commands
  switch(cmd.movemode)
  {
  case MOVE_MODE_SET_HOME: //G92
    coord = stpGetCoordF();
    if(cmd.params_present_axis == 0 && !(cmd.params_present_misc & PARAM_MISC_E))
      coord = stpCoordFZero();
    if(cmd.params_present_axis & PARAM_AXIS_X)
      coord.x = cmd.xyz[0];
    if(cmd.params_present_axis & PARAM_AXIS_Y)
      coord.y = cmd.xyz[1];
    if(cmd.params_present_axis & PARAM_AXIS_Z)
      coord.z = cmd.xyz[2];
    if(cmd.params_present_misc & PARAM_MISC_E)
      coord.stpE = cmd.e;
    stpSetPosition(coord);
    break;
  case MOVE_MODE_FAST: //G0
  case MOVE_MODE_LINE: //G1
    // Check for any axis parameter
    if(!((cmd.params_present_axis & (PARAM_AXIS_X | PARAM_AXIS_Y | PARAM_AXIS_Z |
        PARAM_AXIS_A | PARAM_AXIS_B | PARAM_AXIS_C)) |
        (cmd.params_present_misc & PARAM_MISC_E)))
    {
      consPrintf("Error! No axis given!"CONSOLE_NEWLINE_STR);
      return 0;
    }
    mov = stpCoordFZero();
    coord = stpGetCoordF();
    mov.x = cmd.xyz[0];
    mov.y = cmd.xyz[1];
    mov.z = cmd.xyz[2];
    mov.stpE = cmd.e;
    if(cmd.distancemode==DISTANCE_MODE_ABSOLUTE){
    stpSetPosition(mov);
    mov = stpCoordFSub(mov, coord);
    }
    else if(cmd.distancemode==DISTANCE_MODE_INCREMENTAL){
      stpSetPosition(stpCoordFAdd(coord, mov));
    }
    stpMoveLinearInit(mov);
    break;
  default:
    break;
  }
  consPrintf("ok"CONSOLE_NEWLINE_STR);
  return 0;
}

