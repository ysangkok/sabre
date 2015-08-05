/*
    Sabre Fighter Plane Simulator
    Copyright (C) 1997	Dan Hammer

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
/*************************************************
 * Sabre Fighter Plane Simulator                 *
 * File   : simsnd.cpp                           *
 * Date   : December, 1998                       *
 * Author : Dan Hammer                           *
 *************************************************/
#ifdef USES_DSOUND
#define WIN32_LEAN_AND_MEAN  
#include <windows.h> 
#include <windowsx.h> 
#include <mmsystem.h>
#include <objbase.h>
#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <dsound.h>
#include "winsndx.h"
#endif
#include <stdio.h>
#ifdef USES_SOUNDCLIENT
#include "unistd.h"
#include "SoundClient.h"
#endif
#include "sim.h"
#include "simerr.h"
#include "simfile.h"
#include "simsnd.h"

/*
 *  Map enumerated sounds to id
 */
static const char *enumSounds[4] = 
{
  "JET_ENGINE",
  "GUN",
  "CRASH",
  "CANNON"
};

static const char *soundErr2String(int err);

static R_3DPoint soundViewPoint;
static int soundLock = 0;
static int soundActive = 1;
static int soundAffiliation = 0;

int sound_avail = 0;


#ifdef USES_SOUNDCLIENT
#define MAX_SOUND_IDS 256
#define MAX_SOUND_ID_LEN 32
typedef struct tag_sound_id
{
  char id[MAX_SOUND_ID_LEN+1];
  int  channel;
  int  channelx;
  int  playing;
  int  looping;
} sound_id;
sound_id sound_ids[MAX_SOUND_IDS];
int sound_count =  0;
static sound_id *GetSoundId(const char *id)
{
  sound_id *result = NULL;
  for (int i=0;i<sound_count;i++)
    {
      if (!strcmp(sound_ids[i].id,id))
	{
	  result = &sound_ids[i];
	  break;
	}
    }
  return result;
}
SoundClient *soundClient = NULL;
#endif
/***************************************************************************
 * generic sound interface 
 ***************************************************************************/
int sound_init(__attribute__((unused)) long param)
{
  int result = SOUND_NOT_LOADED;
#ifdef USES_DSOUND
  result = sound_error_check(dsnd_init((HWND)param));
#endif
#ifdef USES_SOUNDCLIENT
  /*
   *  watch out .. this fellow forks!
   */
  soundClient = new SoundClient();
  if (soundClient)
    {
      /*
       * How do we check server is okay?
       */
      result = SOUND_OK;
      sound_count = 0;
      for (int i=0;i<MAX_SOUND_IDS;i++)
	{
	  sound_ids[i].id[0] = 0;
	  sound_ids[i].playing = 0;
	  sound_ids[i].looping = 0;
	}
    }
#endif
  if (result == SOUND_OK)
    sound_avail = 1;
  else
    sound_avail = 0;
  return result;
}

void sound_destroy(void)
{
#ifdef USES_DSOUND
  dsnd_destroy();
#endif
#ifdef USES_SOUNDCLIENT
  if (soundClient)
    {
      for (int i=0;i<sound_count;i++)
	  if (sound_ids[i].id != NULL && sound_ids[i].playing == 1)
	    soundClient->StopSample(sound_ids[i].channel);
      delete soundClient;
      soundClient = NULL;
    }
  sound_count = 0;
#endif
}

int sound_load_wav(__attribute__((unused)) const char *path, __attribute__((unused)) const char *id)
{
#ifdef USES_DSOUND
  int result = dsnd_load_wav(path,id);
  if (result == SOUND_OK)
    sim_printf("Loaded sound id \"%s\" from %s\n",
	       id,path);
  else
    sound_error_check(result);
  return (result);
#else
#ifdef USES_SOUNDCLIENT
  if (soundClient)
    {
      if (sound_count < MAX_SOUND_IDS)
	{
	  soundClient->LoadSample(path,(char *)id);
	  strncpy(sound_ids[sound_count].id,id,MAX_SOUND_ID_LEN);
	  sim_printf("Loaded sound id \"%s\" from %s\n",
		     id,path);
	  sound_count++;
	}
      return 1;
    }
  else
    return 0;
#else
  return 0;
#endif
#endif
}

int sound_on(__attribute__((unused)) const char *id, __attribute__((unused)) int mode, __attribute__((unused)) int vol)
{
  if (soundLock)
    return SOUND_OK;
  if (!soundActive)
    return SOUND_OK;
#ifdef USES_DSOUND
  return sound_error_check(dsnd_play_sound(id,mode,vol));
#else
#ifdef USES_SOUNDCLIENT
  if (soundClient)
    {
      sound_id *sid = GetSoundId(id);
      if (sid)
	{
	  if (sid->looping)
	    return SOUND_OK;
	  sid->channel = soundClient->PlaySample((char *)id,mode);
	  /*
           * This can't be right but dunno whats wrong
           */
	  sid->channelx = soundClient->PlaySample((char *)id,mode);
	  sid->playing = 1;
	  if (mode == LOOP)
	      sid->looping = 1;
	  else
	    sid->looping = 0;
	  return SOUND_OK;
	}
      else
	return SOUND_NOT_FOUND;
    }
  else
    return SOUND_NOT_LOADED;
#else
  return 0;
#endif
#endif
}

int sound_on(int idx, int mode, int vol)
{
  if (soundLock)
    return SOUND_OK;
  if (idx >= 0 && idx <= 4)
    return sound_on(enumSounds[idx],mode,vol);
  else
    return SOUND_NOT_FOUND;
}

int sound_off(__attribute__((unused)) const char *id)
{
#ifdef USES_DSOUND
  return sound_error_check(dsnd_stop_sound(id));
#else
#ifdef USES_SOUNDCLIENT
  if (soundClient)
    {
      sound_id *sid = GetSoundId(id);
      if (sid)
	{
	  soundClient->StopSample(sid->channel);
	  soundClient->StopSample(sid->channelx);
	  sid->playing = sid->looping = 0;
	  return SOUND_OK;
	}
      else
	return SOUND_NOT_FOUND;
    }
  else
    return SOUND_NOT_LOADED;
#else
  return 0;
#endif
#endif
}

int sound_off(int idx)
{
  if (idx >= 0 && idx <= 4)
    return sound_off(enumSounds[idx]);
  else
    return SOUND_NOT_FOUND;
}	

int sound_off_all(void)
{
#ifdef USES_DSOUND
  return sound_error_check(dsnd_stop_sounds());
#else
#ifdef USES_SOUNDCLIENT
  if (soundClient)
    {
      for (int i=0;i<sound_count;i++)
	{
	  if (sound_ids[i].playing)
	    {
	      soundClient->StopSample(sound_ids[i].channel);
	      sound_ids[i].playing = 0;
	    }
	}
      return SOUND_OK;
    }
  else
    return SOUND_NOT_LOADED;
#else
  return 0;
#endif
#endif
}

int sound_vol(__attribute__((unused)) const char *id, __attribute__((unused)) int vol)
{
#ifdef USES_DSOUND
  return sound_error_check(dsnd_set_vol(id,vol));
#else
#ifdef USES_SOUNDCLIENT
  if (soundClient)
    {
      sound_id *sid = GetSoundId(id);
      if (sid)
	{
	  /*  
           *  soundClient->SetSampleVolume(sid->channel,vol);
	   */
	}
      else
	return SOUND_NOT_FOUND;
    }
  else
    return SOUND_NOT_LOADED;
#else
  return 0;
#endif
#endif
}

int sound_freq(__attribute__((unused)) const char *id, __attribute__((unused)) int freq)
{
#ifdef USES_DSOUND
  return sound_error_check(dsnd_set_freq(id,freq));
#else
#ifdef USES_SOUNDCLIENT
  if (soundClient)
    {
      sound_id *sid = GetSoundId(id);
      if (sid)
	{
	  /*  
           *  soundClient->SetSampleFrequency(sid->channel,freq);
	   */
	}
      else
	return SOUND_NOT_FOUND;
    }
  else
    return SOUND_NOT_LOADED;
#else
  return 0;
#endif
#endif
}

int sound_pan(__attribute__((unused)) const char *id, __attribute__((unused)) int pan)
{
#ifdef USES_DSOUND
  return sound_error_check(dsnd_set_pan(id,pan));
#else
  return 0;
#endif
}

int sound_free(__attribute__((unused)) const char *id)
{
#ifdef USES_DSOUND
  return sound_error_check(dsnd_delete_sound(id));
#else
  return 0;
#endif
}

int sound_free_all(void)
{
#ifdef USES_DSOUND
  return sound_error_check(dsnd_delete_all_sounds());
#else
  return 0;
#endif
}

unsigned int sound_status(__attribute__((unused)) const char *id)
{
#ifdef USES_DSOUND
  unsigned int result = 0;
  dsnd_status_sound(id,&result);
  return result;
#else
#ifdef USES_SOUNDCLIENT
  if (soundClient)
    {
      sound_id sid = GetSoundId(id);
      if (sid)
	{
	  unsigned int flags = 0;
	  if (sid->playing)
	    flags |= SND_STATUS_PLAYING;
	  if (sid->looping)
	    flags |= SND_STATUS_LOOPING;
	  return flags;
	}
      else
	return SOUND_NOT_FOUND;
    }
  else
    return SOUND_NOT_LOADED;
#else
  return 0;
#endif
#endif
}

void sound_set_viewpoint(const R_3DPoint &p)
{
  soundViewPoint = p;
}

int sound_calc_distant_vol(const R_3DPoint &origin, REAL_TYPE maxDistSq)
{
  int result = 0;
  REAL_TYPE distSq;
  REAL_TYPE f;

  distSq = distance_squared(origin,soundViewPoint);
  distSq *= world_scale;
  if (distSq < maxDistSq)
    {
      f = distSq / maxDistSq;
      result = static_cast<int>(100 - (100.0 * f));
    }
  return result;
}

void sound_set_lock(int lock) // TODO bool
{
  soundLock = lock;
}

void sound_set_active(int active) // TODO bool
{
  if (!active && soundActive)
    sound_off_all();
  soundActive = active;
}

int sound_get_active(void)
{
  return (sound_avail && soundActive);
}

void sound_toggle_active(void)
{
  soundActive = !soundActive;
  if (!soundActive)
    sound_off_all();
}

void sound_set_affiliation(int affiliation)
{
  soundAffiliation = affiliation;
}

int sound_get_affiliation(void)
{
  return soundAffiliation;
}

int sound_error_check(int err)
{
  if (err != SOUND_OK)
    {
      if (err == SOUND_NOT_FOUND)
	return err; //sim_printf("\"%s\" sound id not found\n", dsnd_search_id);
      else
	sim_printf("sound error: %s\n",
		   soundErr2String(err));
    }
  return err;
}

const char *soundErr2String(int err)
{
  switch (err)
    {
    case SOUND_OK:
      return "SOUND_OK";

    case SOUND_FULL:
      return "SOUND_FULL";

    case SOUND_ERROR_INIT:
      return "SOUND_ERROR_INIT";

    case SOUND_ERROR_CREATE:
      return "SOUND_ERROR_CREATE";

    case SOUND_ERROR_LOCK:
      return "SOUND_ERROR_LOCK";

    case SOUND_ERROR_UNLOCK:
      return "SOUND_ERROR_UNLOCK";

    case SOUND_NOT_FOUND:
      return "SOUND_NOT_FOUND";

    case SOUND_NOT_LOADED:
      return "SOUND_NOT_LOADED";

    case SOUND_ERROR_SETPOS:
      return "SOUND_ERROR_SETPOS";

    case SOUND_ERROR_PLAY:
      return "SOUND_ERROR_PLAY";

    case SOUND_ERROR_VOL:
      return "SOUND_ERROR_VOL";

    case SOUND_ERROR_FREQ:
      return "SOUND_ERROR_FREQ";

    case SOUND_ERROR_PAN:
      return "SOUND_ERROR_PAN";

    case WAV_PATH_NOT_FOUND:
      return "WAV_PATH_NOT_FOUND";

    case WAV_NO_SECTION:
      return "WAV_NO_SECTION";

    case WAV_NO_FORMAT:
      return "WAV_NO_FORMAT";

    case WAV_NO_DATA:
      return "WAV_NO_DATA";

    case WAV_BAD_FORMAT:
      return "WAV_BAD_FORMAT";

    case WAV_CANT_ASCEND:
      return "WAV_CANT_ASCEND";

    case WAV_NO_DATA_CHUNK:
      return "WAV_NO_DATA_CHUNK";
    }
  return ("unknown");
}

