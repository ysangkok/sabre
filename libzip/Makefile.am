## Process this file with automake to create Makefile.in

# This is part of Dan Hammer's SABRE Fighter Plane Simulator
# portions donated by Antti Barck
AUTOMAKE_OPTIONS = foreign
noinst_LIBRARIES = libcomp.a libuncl.a
libcomp_a_SOURCES = bits.c crc.c deflate.c trees.c
libuncl_a_SOURCES = inflate.c unc.c crc.c
#noinst_LIBRARIES = comp uncl
#comp_SOURCES = bits.c crc.c deflate.c trees.c
#uncl_SOURCES = inflate.c unc.c crc.c
EXTRA_DIST = *.h
CFLAGS=-O2 
