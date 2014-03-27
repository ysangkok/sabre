## Process this file with automake to create Makefile.in

# This is part of Dan Hammer's SABRE Fighter Plane Simulator
# portions donated by Antti Barck

# Hacked by Milan Zamazal <pdm@debian.org> in December 2001 to stop automake
# complaining about subdirectories.

AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = sabrejoycal
DEFS =-DJOYCAL 
sabrejoycal_SOURCES = linux_joy.C
sabrejoycal_LDADD = 
EXTRA_DIST = 
INCLUDES=-I../src -I../libzip

linux_joy.C :
	echo

linux_joy.o : linux_joy.C
	$(CXX) $(INCLUDES) -DJOYCAL -c -o linux_joy.o linux_joy.C

 
