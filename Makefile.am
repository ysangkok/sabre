## Process this file with automake to create Makefile.in

# This is part of Dan Hammer's SABRE Fighter Plane Simulator
# portions donated by Antti Barck
#
# 2000-01-22: Modified by Milan Zamazal <pdm@debian.org> to allow compilation
# for Debian.
# 2000-02-12: Milan Zamazal <pdm@debian.org> -- non-SVGAlib fix.
#
# Here are the top level make parameters
# 
# Special update distribution
AUTOMAKE_OPTIONS = foreign
#SUBDIRS = lib libzip gdev src dialogsrc doc scenarios joystick
if SVGALIB
SVGALIB_SUBDIRS = gdev
else
SVGALIB_SUBDIRS =
endif
SUBDIRS = $(SVGALIB_SUBDIRS) lib libzip src doc scenarios
EXTRA_DIST = configure TROUBLE_SHOOTING WHATSNEW VERSION REQUIREMENTS RunSabre CONTRIBUTORS fly JOYSTICK.README cflags HISTORY flySDL RunSabreSDL sdlsrc/Makefile.in sdlsrc/Makefile.am sdlsrc/*.h sdlsrc/*.C sdlsrc/SDL_PORT_README sdlsrc/README



