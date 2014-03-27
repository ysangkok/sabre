## Process this file with automake to create Makefile.in

# This is part of Dan Hammer's SABRE Fighter Plane Simulator
# portions donated by Antti Barck
#
# 2000-01-22: Modified by Milan Zamazal <pdm@debian.org> for compilation with
# newer libsdl versions.
# 2000-02-12: Modified by Milan Zamazal <pdm@debian.org> for compilation
# without SVGAlib.
AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = sabresdl
SABREOBJS = ../src/game.o ../src/scnedit.o ../src/vmath.o ../src/traveler.o ../src/pen.o \
		../src/transblt.o ../src/port_3d.o ../src/clip.o ../src/cpoly.o ../src/obj_3d.o ../src/group_3d.o \
		../src/copoly.o ../src/txtrmap.o ../src/bits.o ../src/convpoly.o ../src/rendpoly.o ../src/rndrpoly.o \
		../src/rndzpoly.o ../src/zview.o ../src/rotate.o ../src/terrain.o ../src/earth.o ../src/colormap.o \
		../src/dvector.o ../src/flight.o ../src/weapons.o ../src/pilot.o ../src/fltlite.o \
		../src/moveable.o ../src/unguided.o ../src/fltzview.o ../src/viewobj.o ../src/fltmngr.o \
		../src/grndunit.o ../src/font8x8.o ../src/plltt.o ../src/key_map.o ../src/linux_joy.o \
		../src/siminput.o ../src/simfile.o ../src/hud.o ../src/cockpit.o ../src/instrmnt.o \
		../src/ppm.o ../src/mytimer.o ../src/colorspc.o ../src/globals.o ../src/portkey.o ../src/vtable2.o \
		../src/led2.o ../src/aaaunit.o ../src/waypoint.o ../src/fltobj.o ../src/simfilex.o ../src/simmath.o \
		../src/target.o ../src/sbrkeys.o ../src/aibase.o ../src/aiflite.o ../src/aigunner.o ../src/aipilot.o \
		../src/aipilot2.o ../src/aipilot3.o ../src/aipilot4.o ../src/aipilot5.o ../src/aipilot6.o ../src/sairfld.o \
		../src/sarray.o ../src/sattkr.o ../src/sfltmdl.o ../src/sfrmtn.o ../src/smath.o ../src/smnvrst.o \
		../src/spilcaps.o ../src/splncaps.o ../src/srunway.o ../src/sslewer.o ../src/stact.o ../src/starget.o \
		../src/sweapon.o ../src/sbfltmdl.o ../src/simsnd.o ../src/dhlist.o ../src/stime.o ../src/spid.o ../src/pilobj.o
#sabresdl_SOURCES =  vga_13.C kbdhit.C input.C fontdev.C
sabresdl_SOURCES = main.C vga_13.C kbdhit.C input.C fontdev.C
sabresdl_LDADD = $(SABREOBJS) ../libzip/libcomp.a ../libzip/libuncl.a
EXTRA_DIST = *.h *.c SDL_PORT_README
INCLUDES=-I../src -I../libzip
#LIBS = -lm -ldl -lSDL -lncurses
LIBS = -lm -ldl `sdl-config --libs` -lncurses
CXXFLAGS = @CXXFLAGS@ -DDEBIAN -DDEBIAN_NO_LIBVGA
