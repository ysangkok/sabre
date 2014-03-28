/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997 Dan Hammer
    Portions Donated By Antti Barck

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/***********************************************************
 * simsnd.h                                                *
 ***********************************************************/
#ifndef __simsnd_h
#define __simsnd_h

#include "vmath.h"

#define JET_ENGINE 0
#define GUN        1
#define CRASH      2
#define CANNON     3

#define NORM  0
#define LOOP  1
#define TIMED 2

#define SND_STATUS_PLAYING		0x01
#define SND_STATUS_LOOPING	   0x02

// define some global sounds
#define SNDGLOBAL_BADLANDING        "gbadlanding"
#define SNDGLOBAL_CRASHBURN         "gcrashburn"
#define SNDGLOBAL_EXPLOSION_LIGHT   "gexpl0"
#define SNDGLOBAL_EXPLOSION_MEDIUM  "gexpl1"
#define SNDGLOBAL_EXPLOSION_HEAVY   "gexpl2"
#define SNDGLOBAL_GEAR					"ggear"
#define SNDGLOBAL_FLAPS					"gflaps"
#define SNDGLOBAL_SPEED_BRAKES		"gspeedbrakes"
#define SNDGLOBAL_FLAK					"gflak"
#define SNDGLOBAL_HITME					"ghitme"
#define SNDGLOBAL_CLICK					"gclick"
#define SNDGLOBAL_WIND					"gwind"
#define SNDGLOBAL_BACKGROUND			"gback"
#define SNDGLOBAL_INTRO					"gintro"
#define SNDGLOBAL_SCREECH				"gscreech"

extern int sound_avail;
extern int sound_init(long);
extern int sound_on(int idx, int mode, int vol = -1);
extern int sound_on(const char *id, int mode, int vol = -1);
extern int sound_off(const char *id);
extern int sound_off(int idx);
extern int sound_off_all(void);
extern int sound_load_wav(const char *path, const char *id);
extern int sound_vol(const char *id, int vol);
extern int sound_freq(const char *id, int freq);
extern int sound_pan(const char *id, int pan);
extern unsigned int sound_status(const char *id);
extern int sound_free(const char *id);
extern int sound_free_all(void);
extern void sound_destroy(void);
extern int sound_error_check(int err);
extern void sound_set_viewpoint(const R_3DPoint &p);
extern void sound_set_affiliation(int affiliation);
extern int  sound_get_affiliation(void);
extern void sound_set_lock(int lock);
extern void sound_set_active(int active);
extern int sound_get_active(void);
extern void sound_toggle_active(void);

extern int sound_calc_distant_vol(const R_3DPoint &soundPoint, REAL_TYPE maxDistSq);
// error enums
enum {
			SOUND_OK,
			SOUND_FULL,
			SOUND_ERROR_INIT,
			SOUND_ERROR_CREATE,
			SOUND_ERROR_LOCK,
			SOUND_ERROR_UNLOCK,
			SOUND_NOT_FOUND,
			SOUND_NOT_LOADED,
			SOUND_ERROR_SETPOS,
			SOUND_ERROR_PLAY,
			SOUND_ERROR_VOL,
			SOUND_ERROR_FREQ,
			SOUND_ERROR_PAN,
			WAV_PATH_NOT_FOUND,
			WAV_NO_SECTION,
			WAV_NO_FORMAT,
			WAV_NO_DATA,
			WAV_BAD_FORMAT,
			WAV_CANT_ASCEND,
			WAV_NO_DATA_CHUNK
};

#endif
