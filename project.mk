# This file can be used to set build configuration
# variables.  These variables are defined in a file called 
# "Makefile" that is located next to this one.

# For instructions on how to use this system, see
# https://analog-devices-msdk.github.io/msdk/USERGUIDE/#build-system

#MXC_OPTIMIZE_CFLAGS = -Og
# ^ For example, you can uncomment this line to 
# optimize the project for debugging

# **********************************************************

# Add your config here!

MFLOAT_ABI = hard


VPATH += A121
PROJ_LDFLAGS += -LA121/rss/lib
PROJ_LIBS += acconeer_a121
PROJ_LIBS += acc_detector_distance_a121
PROJ_LIBS += acc_detector_presence_a121
