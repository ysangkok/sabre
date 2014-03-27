## Process this file with automake to create Makefile.in

# This is part of Dan Hammer's SABRE Fighter Plane Simulator
# portions donated by Antti Barck
#
# 2000-01-22: Modified by Milan Zamazal <pdm@debian.org> for Debian
# compilation.
# 2000-02-12: Modified by Milan Zamazal <pdm@debian.org> for non-SVGAlib
# compilation.
AUTOMAKE_OPTIONS = foreign
if SVGALIB
bin_PROGRAMS = sabre
endif
if SVGALIB
SVGALIBSRCS = main.C vga_13.C input.C kbdhit.C
else
SVGALIBSRCS =
endif
# sabre_SOURCES = main.C game.C scnedit.C vga_13.C vmath.C traveler.C pen.C \
# 		transblt.C port_3d.C clip.C cpoly.C obj_3d.C group_3d.C \
# 		copoly.C txtrmap.C bits.C convpoly.C rendpoly.C rndrpoly.C \
# 		rndzpoly.C zview.C rotate.C terrain.C earth.C colormap.C \
# 		dvector.C flight.C weapons.C pilot.C fltlite.C \
# 		moveable.C unguided.C fltzview.C viewobj.C fltmngr.C \
# 		grndunit.C font8x8.C plltt.C input.C key_map.C linux_joy.C \
# 		kbdhit.C siminput.C simfile.C hud.C cockpit.C instrmnt.C \
# 		ppm.C mytimer.C colorspc.C globals.C portkey.C vtable2.C \
# 		led2.C aaaunit.C waypoint.C fltobj.C simfilex.C simmath.C \
# 		target.C sbrkeys.C aibase.C aiflite.C aigunner.C aipilot.C \
# 		aipilot2.C aipilot3.C aipilot4.C aipilot5.C aipilot6.C sairfld.C \
# 		sarray.C sattkr.C sfltmdl.C sfrmtn.C smath.C smnvrst.C \
# 		spilcaps.C splncaps.C srunway.C sslewer.C stact.C starget.C \
#                 sweapon.C sbfltmdl.C simsnd.C dhlist.c stime.c spid.c pilobj.C
sabre_SOURCES = $(SVGALIBSRCS) game.C scnedit.C vmath.C traveler.C pen.C \
		transblt.C port_3d.C clip.C cpoly.C obj_3d.C group_3d.C \
		copoly.C txtrmap.C bits.C convpoly.C rendpoly.C rndrpoly.C \
		rndzpoly.C zview.C rotate.C terrain.C earth.C colormap.C \
		dvector.C flight.C weapons.C pilot.C fltlite.C \
		moveable.C unguided.C fltzview.C viewobj.C fltmngr.C \
		grndunit.C font8x8.C plltt.C key_map.C linux_joy.C \
		siminput.C simfile.C hud.C cockpit.C instrmnt.C \
		ppm.C mytimer.C colorspc.C globals.C portkey.C vtable2.C \
		led2.C aaaunit.C waypoint.C fltobj.C simfilex.C simmath.C \
		target.C sbrkeys.C aibase.C aiflite.C aigunner.C aipilot.C \
		aipilot2.C aipilot3.C aipilot4.C aipilot5.C aipilot6.C sairfld.C \
		sarray.C sattkr.C sfltmdl.C sfrmtn.C smath.C smnvrst.C \
		spilcaps.C splncaps.C srunway.C sslewer.C stact.C starget.C \
                sweapon.C sbfltmdl.C simsnd.C dhlist.c stime.c spid.c pilobj.C
sabre_LDADD = ../libzip/libcomp.a ../libzip/libuncl.a ../gdev/libgdev.a
EXTRA_DIST = *.h *.c
INCLUDES=-I../gdev -I../libzip
CXXFLAGS = @CXXFLAGS@ -DDEBIAN

if SVGALIB
else
all-local: $(sabre_OBJECTS)
endif
