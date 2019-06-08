# Possible Targets:	all clean Debug cleanDebug Release cleanRelease

##############################################################################################
# Settings
#

# General settings
	# See $(GFXLIB)/tools/gmake_scripts/readme.txt for the list of variables
	OPT_OS					= chibios
	OPT_THUMB				= yes
	OPT_LINK_OPTIMIZE		= no
	OPT_CPU					= stm32m4

# uGFX settings
	# See $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk for the list of variables
	GFXLIB					= ugfx_2.8
	GFXBOARD				= STM32F429i-Discovery
	#GFXDEMO					= modules/gdisp/basics
	GFXSINGLEMAKE			= no
	LIBS = -lm
	

# ChibiOS settings
ifeq ($(OPT_OS),chibios)
	# See $(GFXLIB)/tools/gmake_scripts/os_chibios_x.mk for the list of variables
	CHIBIOS					= chibios1822
	CHIBIOS_VERSION			= 18
	CHIBIOS_BOARD			= ST_STM32F429I_DISCOVERY
	CHIBIOS_CPUCLASS    	= ARMCMx
	CHIBIOS_PLATFORM		= STM32
	CHIBIOS_DEVICE_FAMILY	= STM32F4xx
	CHIBIOS_STARTUP			= startup_stm32f4xx
	CHIBIOS_PORT			= v7m
	CHIBIOS_LDSCRIPT		= STM32F429xI.ld
endif

##############################################################################################
# Set these for your project
#

ARCH     = arm-none-eabi-
SRCFLAGS = -O0 -ggdb -fomit-frame-pointer -falign-functions=16
CFLAGS   = 
CXXFLAGS = -fno-rtti
ASFLAGS  =
LDFLAGS  = -O0

SRC      = Marengo/console.c Marengo/stepper.c Marengo/gcode.c Marengo/marengo.c Marengo/heater.c Marengo/gui.c
OBJS     = 
DEFS     =
LIBS     =
INCPATH  = 
LIBPATH  =
LDSCRIPT = 

##############################################################################################
# These should be at the end
#

include $(GFXLIB)/tools/gmake_scripts/library_ugfx.mk
include $(GFXLIB)/tools/gmake_scripts/os_$(OPT_OS).mk
include $(GFXLIB)/tools/gmake_scripts/compiler_gcc.mk
# *** EOF ***
