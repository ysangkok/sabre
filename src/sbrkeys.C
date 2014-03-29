/*
    Sabre Fighter Plane Simulator
    Copyright (c) 1997/98 Dan Hammer

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
 * File   : sbrkeys.C                            *
 * Date   : December, 1998                       *
 * Author : Dan Hammer                           *
 * Key map                                       *
 *************************************************/
#include "sbrvkeys.h"
#include "sbrkeys.h"

/*
 * So Init() gets called, instantiate a keyboard
 */
static SbrKeyboard theKeyboard;
int SbrKeyboard::keyBuff[256];
int SbrKeyboard::keyInIndex = 0;
int SbrKeyboard::keyOutIndex = 0;

/*
 *  Set up default key map
 */
typedef struct sbrInitKey_struct
{
  int keyCode0;
  int keyCode1;
  int keyCode2;
  int sabreVirtKey;
} sbrInitKey;

static sbrInitKey sbrInitKeyTab[] =
{
  { 'Q',  'q', ESC,  FI_EXIT_FLIGHT },		
  { 'I',  'i',  0,   FI_YOKE_UP },
  { 'M',  'm',  0,   FI_YOKE_DOWN },
  { 'J',  'j',  0,   FI_YOKE_LEFT },
  { 'L',  'l',  0,   FI_YOKE_RIGHT },
  { 'K',  'k',  0,   FI_YOKE_CENTER },
  { ':',  ';',  0,   FI_YOKE_TRIM_UP },
  { '\'', '"',  0,   FI_YOKE_TRIM_DOWN },
  {   0,  0,    0,   FI_YOKE_TRIM_CENTER },
  { '*',  0,    0,   FI_THROTTLE_FULL },
  { '/',  0,    0,   FI_THROTTLE_ZERO },
  { '+',  '=',  0,   FI_THROTTLE_PLUS },
  { '-',  '_',  0,   FI_THROTTLE_MINUS },
  { '[',  0,    0,   FI_RUDDER_LEFT },
  { ']',  0,    0,   FI_RUDDER_RIGHT },
  { '{', '}',   0,   FI_RUDDER_CENTER },
  { 'F', 'f',   0,   FI_FLAPS_TOGGLE },
  { '.',  0,    0,   FI_FLAPS_PLUS },
  { ',',  0,    0,   FI_FLAPS_MINUS },
  { 'G', 'g',   0,   FI_GEAR_TOGGLE },
  { 'B', 'b',   0,   FI_SPEED_BRAKES_TOGGLE },
  { 'W', 'w',   0,   FI_WHEEL_BRAKES_TOGGLE },
  { 'A', 'a',   0,   FI_AUTOPILOT_TOGGLE },
  { 'H', 'h',   0,   FI_HUD_TOGGLE },
  { 'P', 'p',   0,   FI_PAUSE_TOGGLE },
  { 'X', 'x',   0,   FI_WEAPON_ARM_TOGGLE },
  { ' ',   0,   0,   FI_BANG_BANG },
  { 'N', 'n',  BS,   FI_NEXT_WEAPON },
  { CR,  'T',  't',  FI_NEXT_TARGET },
  { 'C', 'c',   0,   FI_COCKPIT_TOGGLE },
  { '~', HT,    0,   FI_NEXT_VIEW_NODE },
  { '(',  0,    0,   FI_PLAYER_VIEW_NODE },
  { '1',  0,    0,   FI_VIEW_FRONT },
  { '!',  0,    0,   FI_VIEW_FRONT_UP },
  {   0,  0,    0,   FI_VIEW_FRONT_DOWN },
  { '2',  0,    0,   FI_VIEW_LEFT },
  { '@',  0,    0,   FI_VIEW_LEFT_UP },
  {   0,  0,    0,   FI_VIEW_LEFT_DOWN },
  { '3',  0,    0,   FI_VIEW_RIGHT },
  { '#',  0,    0,   FI_VIEW_RIGHT_UP },
  {   0,  0,    0,   FI_VIEW_RIGHT_DOWN },
  { '4',  0,    0,   FI_VIEW_REAR },
  { '$',  0,    0,   FI_VIEW_REAR_UP },
  {   0,  0,    0,   FI_VIEW_REAR_DOWN },
  { 'z',  0,    0,   FI_VIEW_FRONT_LEFT },
  { 'Z',  0,    0,   FI_VIEW_FRONT_LEFT_UP },
  {   0,  0,    0,   FI_VIEW_FRONT_LEFT_DOWN },
  { '\\',  0,    0,   FI_VIEW_FRONT_RIGHT },
  { '|',  0,    0,   FI_VIEW_FRONT_RIGHT_UP },
  {   0,  0,    0,   FI_VIEW_FRONT_RIGHT_DOWN },
  {   0,  0,    0,   FI_VIEW_REAR_LEFT },
  {   0,  0,    0,   FI_VIEW_REAR_LEFT_UP },
  {   0,  0,    0,   FI_VIEW_REAR_LEFT_DOWN },
  {   0,  0,    0,   FI_VIEW_REAR_RIGHT },
  {   0,  0,    0,   FI_VIEW_REAR_RIGHT_UP },
  {   0,  0,    0,   FI_VIEW_REAR_RIGHT_DOWN },
  {   0,  0,    0,   FI_VIEW_UP },
  {   0,  0,    0,   FI_VIEW_DOWN },
  { '5',  0,    0,   FI_VIEW_SATELLITE },
  { '6',  0,    0,   FI_VIEW_EXTERNAL },
  { '7',  0,    0,   FI_VIEW_TRACK },
  { '8',  0,    0,   FI_VIEW_TARGET_TRACK },
  { '9',  0,    0,   FI_VIEW_FLYBY },
  {   0,  0,    0,   FI_VIEW_WEAPON_TRACK },
  { '0',  0,    0,   FI_VIEW_VIRTUAL },
  { ')',  0,    0,   FI_VIEW_VIRTUAL_SNAP_FRONT },
  { 's',  0,    0,   FI_VIEW_THETA_PLUS },
  { 'S',  0,    0,   FI_VIEW_THETA_MINUS },
  { 'd',  0,    0,   FI_VIEW_PHI_PLUS },
  { 'D',  0,    0,   FI_VIEW_PHI_MINUS },
  { '&',  0,    0,   FI_VIEW_PADLOCK },
  { 'u',  0,    0,   FI_EXTERN_VIEW_DISTANCE_PLUS },
  { 'U',  0,    0,   FI_EXTERN_VIEW_DISTANCE_MINUS },
  { '?',  0,    0,   FI_WIREFRAME_TOGGLE },
  { '>',  0,    0,   FI_FRAMERATE_TOGGLE },
  { '%',  0,    0,   FI_DISPLAY_CLOUDS_TOGGLE },
  { '^',  0,    0,   FI_DISPLAY_TEXTURED_GROUND_TOGGLE },
  { 'V', 'v',   0,   FI_FLIGHTPATH_INDICATOR_TOGGLE },
  { 'R', 'r',   0,   FI_RADAR_TOGGLE },
  { 'Y', 'y',   0,   FI_PLANEFRAME_TOGGLE },
  {   0,  0,    0,   FI_CONTROL_POSITIONS_INDICATOR_TOGGLE },
  { 'e',  0,    0,   FI_SCREEN_DUMP },
  {   0,  0,    0,   FI_NO_KEY }
};

static int keyTab[256];

void SbrKeyboard::Init(void)
{
  sbrInitKey *initKey;
  int sbrVirtKey;

  for (int i=0;i<256;i++)
    keyTab[i] = FI_NO_KEY;

  Flush();
  initKey = sbrInitKeyTab;
  sbrVirtKey = initKey->sabreVirtKey;
  while (sbrVirtKey != FI_NO_KEY)
    {
      if (initKey->keyCode0 > 0 && initKey->keyCode0 < 256)
	keyTab[initKey->keyCode0] = sbrVirtKey;
      if (initKey->keyCode1 > 0 && initKey->keyCode1 < 256)
	keyTab[initKey->keyCode1] = sbrVirtKey;
      if (initKey->keyCode2 > 0 && initKey->keyCode2 < 256)
	keyTab[initKey->keyCode2] = sbrVirtKey;
      initKey++;
      sbrVirtKey = initKey->sabreVirtKey;
    }
}

void SbrKeyboard::Flush(void)
{
  keyInIndex = keyOutIndex = 0;
}

int SbrKeyboard::GetNextSabreVKey(void)
{
  int result = FI_NO_KEY;
  if (keyInIndex != keyOutIndex)
    {
      result = keyBuff[keyOutIndex++];
      if ( keyOutIndex >= 256)
	keyOutIndex = 0;
    }
  return result;
}

void SbrKeyboard::PushSabreVKey(int sabreVKey)
{
  if (sabreVKey != FI_NO_KEY)
    {
      keyBuff[keyInIndex++] = sabreVKey;
      if (keyInIndex >= 256)
	keyInIndex = 0;
    }
}

void SbrKeyboard::PressKey(int keyCode)
{
  if (keyCode > 0 && keyCode < 256)
    PushSabreVKey(keyTab[keyCode]);
}



