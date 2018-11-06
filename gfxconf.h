/**
 * This file has a different license to the rest of the GFX system.
 * You can copy, modify and distribute this file as you see fit.
 * You do not need to publish your source modifications to this file.
 * The only thing you are not permitted to do is to relicense it
 * under a different license.
 */

#ifndef _GFXCONF_H
#define _GFXCONF_H

/* The operating system to use. One of these must be defined - preferably in your Makefile */
#define GFX_USE_OS_CHIBIOS              TRUE
//#define GFX_USE_OS_WIN32              FALSE
//#define GFX_USE_OS_LINUX              FALSE
//#define GFX_USE_OS_OSX                FALSE

/* GFX sub-systems to turn on */
#define GFX_USE_GDISP                   TRUE

/* Features for the GDISP sub-system. */
#define GDISP_NEED_VALIDATION           TRUE
#define GDISP_NEED_CLIP                 TRUE
#define GDISP_NEED_TEXT                 TRUE
#define GDISP_NEED_CIRCLE               TRUE
#define GDISP_NEED_ELLIPSE              TRUE
#define GDISP_NEED_ARC                  FALSE
#define GDISP_NEED_SCROLL               FALSE
#define GDISP_NEED_PIXELREAD            FALSE
#define GDISP_NEED_CONTROL              FALSE
#define GDISP_NEED_MULTITHREAD          TRUE
#define GDISP_NEED_ASYNC                FALSE
#define GDISP_NEED_MSGAPI               FALSE

/* Builtin Fonts */
#define GDISP_INCLUDE_FONT_SMALL        FALSE
#define GDISP_INCLUDE_FONT_LARGER       FALSE
#define GDISP_INCLUDE_FONT_UI1          TRUE
#define GDISP_INCLUDE_FONT_UI2          FALSE
#define GDISP_INCLUDE_FONT_LARGENUMBERS FALSE
#define GDISP_INCLUDE_FONT_FIXED_5X8    TRUE

///////////////////////////////////////////////////////////////////////////
// GEVENT                                                                //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GEVENT                               TRUE

#define GEVENT_ASSERT_NO_RESOURCE                    FALSE
#define GEVENT_MAXIMUM_SIZE                          32
#define GEVENT_MAX_SOURCE_LISTENERS                  32



/* Input */
#define GFX_USE_GINPUT                          TRUE

#define GINPUT_NEED_MOUSE                       TRUE
#define GINPUT_TOUCH_STARTRAW                   FALSE
#define GINPUT_TOUCH_NOTOUCH                    FALSE
#define GINPUT_TOUCH_NOCALIBRATE                FALSE
#define GINPUT_TOUCH_NOCALIBRATE_GUI            FALSE
#define GINPUT_MOUSE_POLL_PERIOD                25
#define GINPUT_MOUSE_CLICK_TIME                 300
#define GINPUT_TOUCH_CXTCLICK_TIME              700
#define GINPUT_TOUCH_USER_CALIBRATION_LOAD      TRUE
#define GINPUT_TOUCH_USER_CALIBRATION_SAVE      FALSE
#define GMOUSE_DRIVER_LIST                      GMOUSEVMT_Win32, GMOUSEVMT_Win32
#define GINPUT_NEED_TOGGLE                      FALSE



///////////////////////////////////////////////////////////////////////////
// GTIMER                                                                  //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GTIMER                               TRUE



///////////////////////////////////////////////////////////////////////////
// GQUEUE                                                                  //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GQUEUE                               TRUE

#define GQUEUE_NEED_ASYNC                            TRUE



///////////////////////////////////////////////////////////////////////////
// GWIN                                                                  //
///////////////////////////////////////////////////////////////////////////
#define GFX_USE_GWIN                                 TRUE

#define GWIN_NEED_WINDOWMANAGER                      TRUE
//    #define GWIN_REDRAW_IMMEDIATE                    FALSE
//    #define GWIN_REDRAW_SINGLEOP                     FALSE
//    #define GWIN_NEED_FLASHING                       FALSE
//        #define GWIN_FLASHING_PERIOD                 250

#define GWIN_NEED_CONSOLE                            TRUE
//    #define GWIN_CONSOLE_USE_HISTORY                 FALSE
//        #define GWIN_CONSOLE_HISTORY_AVERAGING       FALSE
//        #define GWIN_CONSOLE_HISTORY_ATCREATE        FALSE
//    #define GWIN_CONSOLE_ESCSEQ                      FALSE
//    #define GWIN_CONSOLE_USE_BASESTREAM              FALSE
    #define GWIN_CONSOLE_USE_FLOAT                   TRUE
//#define GWIN_NEED_GRAPH                              FALSE
//#define GWIN_NEED_GL3D                               FALSE

#define GWIN_NEED_WIDGET                             TRUE
#define GWIN_FOCUS_HIGHLIGHT_WIDTH                   1
    #define GWIN_NEED_LABEL                          TRUE
        #define GWIN_LABEL_ATTRIBUTE                 TRUE
    #define GWIN_NEED_BUTTON                         TRUE
        #define GWIN_BUTTON_LAZY_RELEASE             TRUE
//    #define GWIN_NEED_SLIDER                         FALSE
//        #define GWIN_SLIDER_NOSNAP                   FALSE
//        #define GWIN_SLIDER_DEAD_BAND                5
//        #define GWIN_SLIDER_TOGGLE_INC               20
//    #define GWIN_NEED_CHECKBOX                       FALSE
//    #define GWIN_NEED_IMAGE                          FALSE
//        #define GWIN_NEED_IMAGE_ANIMATION            FALSE
//    #define GWIN_NEED_RADIO                          FALSE
//    #define GWIN_NEED_LIST                           FALSE
//        #define GWIN_NEED_LIST_IMAGES                FALSE
//    #define GWIN_NEED_PROGRESSBAR                    FALSE
//        #define GWIN_PROGRESSBAR_AUTO                FALSE
//    #define GWIN_NEED_KEYBOARD                       FALSE
//        #define GWIN_KEYBOARD_DEFAULT_LAYOUT         VirtualKeyboard_English1
//        #define GWIN_NEED_KEYBOARD_ENGLISH1          TRUE
//    #define GWIN_NEED_TEXTEDIT                       FALSE
//    #define GWIN_FLAT_STYLING                        FALSE
//    #define GWIN_WIDGET_TAGS                         FALSE

//#define GWIN_NEED_CONTAINERS                         FALSE
//    #define GWIN_NEED_CONTAINER                      FALSE
//    #define GWIN_NEED_FRAME                          FALSE
//    #define GWIN_NEED_TABSET                         FALSE
//        #define GWIN_TABSET_TABHEIGHT                18


#endif /* _GFXCONF_H */
