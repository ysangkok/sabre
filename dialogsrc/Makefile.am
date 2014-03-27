## Process this file with automake to create Makefile.in

# This is part of Dan Hammer's SABRE Fighter Plane Simulator
# portions donated by Antti Barck
AUTOMAKE_OPTIONS=foreign

noinst_PROGRAMS=dialog
dialog_SOURCES=checklist.c inputbox.c menubox.c msgbox.c \
	       textbox.c util.c yesno.c dialog.c

# disable mouse support (for now)
# Dan -- keep dialogsrc compiled with these flags
#        others seem to cause problems
CFLAGS=-g -O2
EXTRA_DIST = *.h

