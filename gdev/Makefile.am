## Process this file with automake to create Makefile.in

#
# Minefields:
#
#  This code is mine. You may use, modify or redistribute this code
#  in any APPROPRIATE manner you may choose.
#
#  If you use, modify or redistribute this code, I am NOT responsible
#  for ANY issue raised.
# 
# * If you modify or remove this note, this code is no longer mine. *
#
#  proff@iki.fi
#

# forget strictness
AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES=libgdev.a
libgdev_a_SOURCES=gdev.h gdev.C gdev-svgalib.h gdev-svgalib.C fontdev.h \
		  fontdev.C
FONT_UTILS=fontutils.h fontutils.C		  
noinst_PROGRAMS=hello fontedit
hello_SOURCES=hello.h hello.C $(FONT_UTILS)
hello_LDADD=libgdev.a
fontedit_SOURCES=fontedit.h fontedit.C $(FONT_UTILS)
fontedit_LDADD=libgdev.a
