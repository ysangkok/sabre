## Process this file with automake to create Makefile.in

# This is part of Dan Hammer's SABRE Fighter Plane Simulator
# portions donated by Antti Barck
AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = sabrejoycal
DEFS =-DJOYCAL 
sabrejoycal_SOURCES = ../src/linux_joy.C
sabrejoycal_LDADD = 
EXTRA_DIST = 
INCLUDES=-I../src -I../libzip

linux_joy.C :
	echo

linux_joy.o : ../src/linux_joy.C
	$(CXX) $(INCLUDES) -DJOYCAL -c -o linux_joy.o ../src/linux_joy.C

 
