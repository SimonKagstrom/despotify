# Local build settings. Do NOT edit Makefile, edit this instead.
# After a fresh checkout, you have to copy this to Makefile.local.mk.

## Enable debug output to /tmp/gui.log
#DEBUG = 1

#CC=arm-unknown-linux-uclibc-gcc
CC_BIN_PATH = $(shell which $(CC))
CC_DIR_NAME = $(shell dirname $(CC_BIN_PATH))
SYSROOT     = $(CC_DIR_NAME)/../../


## Enable additional clients.
CLIENT_DESPOTIFY = 0
CLIENT_GATEWAY   = 0
# CLIENT_MAEMIFY   = 1

## Enable Nokia Maemo4 specific code in maemify client. 
## At least LINUX_BACKEND = gstreamer seems to work with this.
# MAEMO4 = 1

## Install prefix
INSTALL_PREFIX = /home/ska/local/

## Specify ncurses include path explicitly. (should contain curses.h)
# NCURSES_INCLUDE = /usr/local/include/ncursesw

## Choose audio backend
LINUX_BACKEND = libao

## Add more CFLAGS
# CFLAGS += -DDEBUG_SNDQUEUE
# CFLAGS += -DDEBUG_PACKETS

## Adjust linking flags
# LDFLAGS += -L/usr/local/lib

## If you're on Mac OS X and have installed libvorbisfile
## via 'port install ..', try uncommenting these lines
#CFLAGS += -I$(SYSROOT)/usr/include
#LDFLAGS += -L$(SYSROOT)/usr/lib
